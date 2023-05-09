
#include <types.h>

#include "memory.h"
#include "system.h"

sys_info* system = NULL;

void sys_init(u16 hart_count) {

	system = alloc(1);
	system->hart_count = hart_count;

	for (u16 i = 0; i < hart_count; i++) {
		system->harts[i].id = i;
		system->harts[i].task = NULL;
	}
}
