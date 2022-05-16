
#include <types.h>

#include "print.h"

#include "memory.h"

#include "mutex.h"


static void* heap      = NULL;
static u64   heap_size = 0;

static u64* page_map[2]  = { NULL };
static u64  page_map_len = 0;
#define PAGE_MAP_CHUNK_WIDTH 64 // size of page_map type in bits


static u64 u64_trailing(const u64 i) {
	static const u8 tab64[64] = {
		 0,  1,  2, 53,  3,  7, 54, 27,  4, 38, 41,  8, 34, 55, 48, 28,
		62,  5, 39, 46, 44, 42, 22,  9, 24, 35, 59, 56, 49, 18, 29, 11,
		63, 52,  6, 26, 37, 40, 33, 47, 61, 45, 43, 21, 23, 58, 17, 10,
		51, 25, 36, 32, 60, 20, 57, 16, 50, 31, 19, 15, 30, 14, 13, 12
	};
	return tab64[((u64)((i & -i) * 0x022fdd63cc95386dUL)) >> 58];
}


static u64 find_range(const u64 page_count) {

	bool measuring = false;
	u64  start     = 0;
	u64  i         = 0;

	while (i < page_map_len * PAGE_MAP_CHUNK_WIDTH) {

		const u64 offset = i % PAGE_MAP_CHUNK_WIDTH;
		const u64 ci     = i / PAGE_MAP_CHUNK_WIDTH;
		if (ci+1 >= page_map_len) break;

		if (!measuring) {
			const u64 a = page_map[1][ci]   >> offset;
			const u64 b = (!offset) ? 0 : page_map[1][ci+1] << (PAGE_MAP_CHUNK_WIDTH - offset);
			const u64 chunk = a | b;

			if (chunk) {
				i += u64_trailing(chunk) + 1;
				start = i;
				measuring = true;
			} else {
				i += PAGE_MAP_CHUNK_WIDTH;
			}
		} else {
			const u64 a = page_map[0][ci]   >> offset;
			const u64 b = (!offset) ? 0 : page_map[0][ci+1] << (PAGE_MAP_CHUNK_WIDTH - offset);
			const u64 chunk = a | b;

			if (chunk) {
				i += u64_trailing(chunk);
				measuring = false;
			} else {
				i += PAGE_MAP_CHUNK_WIDTH;
			}

			if ((i - start) >= page_count) {
				return start;
			}
		}
	}

	return 0;
}


static void map_range(const u64 start, const u64 page_count) {

	if (!page_count) return;

	const u64 stop = start + page_count - 1;

	if (stop > page_map_len * PAGE_MAP_CHUNK_WIDTH) return;

	const u64 chunk      = start / PAGE_MAP_CHUNK_WIDTH;
	const u64 chunk_last = stop  / PAGE_MAP_CHUNK_WIDTH;

	page_map[0][chunk]      |= 1UL << (start % PAGE_MAP_CHUNK_WIDTH);
	page_map[1][chunk_last] |= 1UL << (stop  % PAGE_MAP_CHUNK_WIDTH);
}


static u64 unmap_range(const u64 start) {

	if (start >= page_map_len * PAGE_MAP_CHUNK_WIDTH) return 0;

	const u64 start_a = start / PAGE_MAP_CHUNK_WIDTH;
	const u64 start_b = start % PAGE_MAP_CHUNK_WIDTH;

	const u64 first = (page_map[0][start_a] >> start_b) & 1UL;
	if (!first) return 0;

	page_map[0][start_a] &= ~(1UL << start_b); // clear start

	for (u64 i = start; i < page_map_len * PAGE_MAP_CHUNK_WIDTH; i++) {
		u64 offset = i % PAGE_MAP_CHUNK_WIDTH;
		u64 ci     = i / PAGE_MAP_CHUNK_WIDTH;
		if (ci+1 >= page_map_len) return 0;

		const u64 a = page_map[1][ci] >> offset;
		const u64 b = (!offset) ? 0 : page_map[1][ci+1] << (PAGE_MAP_CHUNK_WIDTH - offset);
		const u64 chunk = a | b;

		if (chunk) {
			i += u64_trailing(chunk);

			offset = i % PAGE_MAP_CHUNK_WIDTH;
			ci     = i / PAGE_MAP_CHUNK_WIDTH;

			page_map[1][ci] &= ~(1UL << offset); // clear start

			return (i - start) + 1;
		} else {
			i += PAGE_MAP_CHUNK_WIDTH;
		}
	}
	return 0;
}


static void clear_pages(const void* const ptr, const u64 page_count) {
	const u64 size = (PAGE_SIZE * page_count) / sizeof(u64);
	for (u64 i = 0; i < size; i++) {
		((u64*)ptr)[i] = 0;
	}
}


// ---- public functions ----


mtx memory_lock;


void* alloc(const u64 page_count) {

	mtx_lock(&memory_lock);

	const u64 page = find_range(page_count);
	if (!page) return NULL;

	u64* ptr = (u64*)((u64)heap + (page * PAGE_SIZE)); // page number to address

	map_range(page, page_count);
	//clear_pages(ptr, page_count);

	mtx_unlock(&memory_lock);

	return ptr;
}


void _free(void** ptr) {

	if (*ptr == NULL) return;

	const u64 page = ((u64)*ptr - (u64)heap) / PAGE_SIZE; // address to page number

	mtx_lock(&memory_lock);

	const u64 page_count = unmap_range(page);
	(void)page_count;
	//clear_pages(*ptr, page_count);

	mtx_unlock(&memory_lock);

	*ptr = NULL;
}


#define CEIL_DIV(A, B) (1 + ((A - 1) / B))
void memory_init(const u64 cpu_count) {

	// set up heap
	const u64 k_stack_end = K_STACK_START + (K_STACK_SIZE * cpu_count);

	heap      = (void*)k_stack_end;
	heap_size = MEMORY_END - k_stack_end;
	// ----

	// set up allocation page map
	page_map_len = (heap_size / PAGE_SIZE) / PAGE_MAP_CHUNK_WIDTH;

	page_map[0] = heap;
	page_map[1] = &page_map[0][page_map_len];
	// ----

	// manually clear and "allocate" pages for the page map itself
	const u64 page_map_pages = CEIL_DIV(page_map_len * sizeof(u64) * 2, PAGE_SIZE);

	clear_pages(&page_map[0][0], page_map_pages);
	map_range(0, page_map_pages);
	// ----

	printf("RAM: %d bytes\n", heap_size);

	//printf("stack: %x (%d bytes)\n", K_STACK_START, k_stack_end - K_STACK_START);
	//printf("heap:  %p (%d bytes)\n", heap, heap_size);
	//printf("pmap:  %d 4KiB page%c\n", page_map_pages, (page_map_pages != 1) ? 's' : '\0');
}
