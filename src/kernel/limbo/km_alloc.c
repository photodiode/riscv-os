
#include <types.h>

#include "print.h"

#include "k_memory.h"


// memory constants
extern const u64 _stack_size;
extern const u64 _stack_start;
#define k_stack_size  (const u64)(&_stack_size)
#define k_stack_start (const u64)(&_stack_start)

extern const u64 _memory_end;
#define memory_end (const u64)(&_memory_end)
// ----

extern volatile u32 cpu_count;


typedef struct {
	bool used   : 1;
	bool first  : 1;
	bool last   : 1;
	bool unused : 1;
} pm_entry;


static void* heap      = NULL;
static u64   heap_size = 0;

static pm_entry* page_map     = NULL;
static u64       page_map_len = 0;


static void pm_set_pages(const u64 start, const u64 count) {

	page_map[start            ].first = true;
	page_map[start + count - 1].last  = true;

	for (u64 i = start; i < (start + count); i++) {
		page_map[i].used = true;
	}
}


static bool pm_clear(const u64 start) {

	if (!page_map[start].first) return false;

	for (u64 i = start; i < page_map_len; i++) {
		page_map[i].used = false;
		if (page_map[i].last) break;
	}

	return true;
}


static u64 pm_find_free(const u64 page_count) {

	bool counting       = false;
	u64  start          = 0;
	u64  tmp_page_count = 0;

	for(u64 i = 0; i < page_map_len; i++) {

		if (!page_map[i].used) {

			if (!counting) {
				counting       = true;
				start          = i;
				tmp_page_count = 1;
			} else {
				tmp_page_count++;
			}
		} else {
			counting = false;
		}

		if (tmp_page_count == page_count) {
			return start;
		}
	}

	return 0;
}


static void km_clear_pages(void* const ptr, const u64 page_count) {

	u64 size = (PAGE_SIZE * page_count) / sizeof(u64);

	for (u64 i = 0; i < size; i++) {
		((u64*)ptr)[i] = 0;
	}
}


void* km_alloc(const u64 page_count) {

	if (!page_count) return NULL;

	u64 start = pm_find_free(page_count);
	if (!start) return NULL;

	void* ptr = (void*)((u64)heap + (start * PAGE_SIZE));

	pm_set_pages(start, page_count);
	km_clear_pages(ptr, page_count);

	return ptr;
}


void _km_free(void** ptr) {

	if (*ptr == NULL) return; 

	u64 start = ((u64)*ptr - (u64)heap) / PAGE_SIZE;

	if (pm_clear(start)) {
		*ptr = NULL;
	}
}


#define CEIL_DIV(A, B) (1 + ((A - 1) / B))
void km_init() {

	u64 k_stack_end = k_stack_start + (k_stack_size * cpu_count);

	heap      = (void*)k_stack_end;
	heap_size = memory_end - k_stack_end;

	page_map     = (pm_entry*)heap;
	page_map_len = heap_size / PAGE_SIZE;

	u64 page_map_pages = CEIL_DIV(page_map_len, PAGE_SIZE);

	km_clear_pages(page_map, page_map_pages);
	pm_set_pages(0, page_map_pages);

	printf("stack: %x\n", k_stack_start);
	printf("heap:  %p (%x)\n", heap, heap_size);
	printf("pmap:  %d 4KiB pages\n", page_map_pages);

	/*printf("\n");
	for (u64 i = 0; i < 16*8; i++) {
		printf(" %x ", page_map[i]);
		if (i % 16 == 15) printf("\n");
	}
	printf("\n\n");*/

}

