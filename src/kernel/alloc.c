
#include <types.h>

#include "atomic.h"

#include "memory.h"
#include "system.h"

#include "print.h"


static u64* pmap[3]       = {NULL};
static u64  pmap_len[3]   = {0}; // number of elements in pmap
static u64  pmap_pages[3] = {0}; // number of pages, or bits in pmap


static inline u64 ceil_div(u64 a, u64 b) {
	return ((a > 0) ? (1 + ((a - 1) / b)) : 0);
}


void alloc_init(const u64 hart_count, const u64 memory_end) {

	// set up heap
	const u64 k_stack_end = K_STACK_START + (K_STACK_SIZE * hart_count);

	system.heap      = (void*)k_stack_end;
	system.heap_size = memory_end - k_stack_end;
	// ----

	// set up allocation page map
	pmap_pages[0] = system.heap_size / PAGE_SIZE_0;
	pmap_pages[1] = system.heap_size / PAGE_SIZE_1;
	pmap_pages[2] = system.heap_size / PAGE_SIZE_2;

	pmap_len[0] = ceil_div(pmap_pages[0], (sizeof **pmap * 8));
	pmap_len[1] = ceil_div(pmap_pages[1], (sizeof **pmap * 8));
	pmap_len[2] = ceil_div(pmap_pages[2], (sizeof **pmap * 8));

	pmap[0] = system.heap;
	pmap[1] = &pmap[0][pmap_len[0]];
	pmap[2] = &pmap[1][pmap_len[1]];
	// ----

	// manually clear and "allocate" pages for the page map itself
	const u64 pages = ceil_div((pmap_len[0] + pmap_len[1] + pmap_len[2]) * (sizeof **pmap), PAGE_SIZE_0);

	//clear_pages(&page_map[0], pages);
	//map_range(0, page_map_pages);
	// ----

	printf("stack: 0x%x (%d bytes)\n", K_STACK_START, k_stack_end - K_STACK_START);
	printf("heap:  %p (%d bytes)\n", system.heap, system.heap_size);

	printf("pmap 0: u64 x %d (%d)\n", pmap_len[0], pmap_pages[0]);
	printf("pmap 1: u64 x %d (%d)\n", pmap_len[1], pmap_pages[1]);
	printf("pmap 2: u64 x %d (%d)\n", pmap_len[2], pmap_pages[2]);
	printf("pmap pages needed: %d\n", pages);

	//printf("pmap:  %d page%c\n", page_map_pages, (page_map_pages != 1) ? 's' : '\0');
}
