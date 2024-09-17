
#include <types.h>
#include <atomic.h>
#include <bytes.h>
#include <mem.h>

#include "system.h"
#include "memory.h"

//#include "print.h"


#define PAGE_MAP_CHUNK_WIDTH U64_BITS


static struct {

	u8  level_count;
	u32 size;

	struct {
		u8   overflow;
		u32  chunk_count;
		u64* chunks;
	} levels[];

}* map = NULL;


static inline u64 ceil_div(u64 a, u64 b) {
	return ((a > 0) ? (1 + ((a - 1) / b)) : 0);
}


void* alloc(void) {

	// search for first free page by going down the map levels
	u64 offset = 0;

	u64 chunk;
	u8  chunk_overflow;

	u8  level = map->level_count;
	while (level > 0) {
		level--;

		chunk_overflow = trailing_zeroes_u64(~map->levels[level].chunks[offset]);

		// if we're in the overflow chunk check that we're also within it's range
		if (map->levels[level].chunk_count == offset &&
		    map->levels[level].overflow &&
		    map->levels[level].overflow < chunk_overflow) {
			return NULL;
		}

		chunk = offset;
		offset = (offset * PAGE_MAP_CHUNK_WIDTH) + chunk_overflow;
	}
	// ----

	// set page bit in all levels upward
	for (u8 i = 0; i < map->level_count; i++) {

		map->levels[i].chunks[chunk] |= 1UL << chunk_overflow;

		if (map->levels[i].chunks[chunk] != U64_MAX) {
			break;
		}

		chunk_overflow = chunk % PAGE_MAP_CHUNK_WIDTH;
		chunk /= PAGE_MAP_CHUNK_WIDTH;
	}
	// ----

	return system.memory + (offset * PAGE_SIZE);
}


void _free(void** ptr) {

	if (*ptr == NULL) return;

	const u64 offset = ((u64)*ptr - (u64)system.memory) / PAGE_SIZE;

	u64 chunk          = offset / PAGE_MAP_CHUNK_WIDTH;
	u8  chunk_overflow = offset % PAGE_MAP_CHUNK_WIDTH;

	// clear page bit in all levels upward
	for (u8 i = 0; i < map->level_count; i++) {

		const u64 tmp = map->levels[i].chunks[chunk];

		map->levels[i].chunks[chunk] &= ~(1UL << chunk_overflow);

		if (tmp == U64_MAX) {
			break;
		}

		chunk_overflow = chunk % PAGE_MAP_CHUNK_WIDTH;
		chunk /= PAGE_MAP_CHUNK_WIDTH;
	}
	// ----

	*ptr = NULL;
}


void alloc_init(void) {

	// set up page map levels
	map = (void*)system.kernel_end;
	map->level_count = 0;

	const u64 page_count  = system.memory_size / PAGE_SIZE;
	      u64 chunk_count = page_count;

	do {
		map->levels[map->level_count].overflow = chunk_count % PAGE_MAP_CHUNK_WIDTH;

		chunk_count = chunk_count / PAGE_MAP_CHUNK_WIDTH;

		map->levels[map->level_count].chunk_count = chunk_count;

		if (map->levels[map->level_count].overflow) {
			chunk_count += 1;
		}

		map->level_count += 1;

	} while (chunk_count > 1);
	// ----

	map->size = sizeof(*map) + (sizeof(*map->levels) * map->level_count);

	// set pointer for each level's chunk array
	for (u8 i = 0; i < map->level_count; i++) {

		map->levels[i].chunks = (void*)((u8*)map + map->size);

		const u64 size = (map->levels[i].chunk_count * sizeof(u64)) + ((map->levels[i].overflow > 0) ? sizeof(u64) : 0);
		map->size += size;

		memset(map->levels[i].chunks, 0, size);

		//printf("level %d: [%d + %d] %p\n", i, map->levels[i].chunk_count, map->levels[i].overflow, map->levels[i].chunks);
	}
	// ----

	// clean the page map and "allocate" pages for the kernel and page map
	const u64 pages = ceil_div((system.kernel_end + map->size) - (u64)system.memory, PAGE_SIZE);

	for (u64 i = 0; i < pages; i++) {
		alloc();
	}
	// ----
}
