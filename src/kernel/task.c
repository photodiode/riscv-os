
#include <types.h>
#include <riscv.h>

#include "print.h"

#include "memory.h"
#include "mmu.h"


typedef enum __attribute__((__packed__)) {
	TS_NONE,
	TS_RUNNING,
	TS_SLEEPING,
	TS_WAITING
} task_state;


typedef struct {
	task_state state;
	u16        parent;
	u16        id;

	u64       pc;
	u8*       stack;
	mmu_table pagetable;

	trap_frame frame;
} task;


void __attribute__((aligned(4096))) program_001() {
	asm("li a0, 17");
	asm("ecall");
}


#define TASK_PAGES 1
u16   task_count      = 0;
u16   task_first_free = 0;
task* tasks           = NULL;

#define TASK_ENTRY_POINT 0x1000



void tasks_init() {
	tasks = alloc(TASK_PAGES);
	task_count = (PAGE_SIZE * TASK_PAGES) / sizeof(task);

	printf("tasks: %p\n", tasks);
	printf("task count: %d\n", task_count);

	tasks[0] = (task) {
		.state  = TS_RUNNING,
		.parent = 0,
		.id     = 0,

		.pc        = TASK_ENTRY_POINT,
		.stack     = alloc(2),
		.pagetable = alloc(1)
	};

	mmu_map(tasks[0].pagetable, (u64)program_001, TASK_ENTRY_POINT, 0x1000, MMU_PTE_READ_EXECUTE);
	mmu_map(tasks[0].pagetable, (u64)tasks[0].stack, 0x2000, 0x2000, MMU_PTE_READ_WRITE);

	//printf("%x\n", mmu_v2p(tasks[0].pagetable, TASK_ENTRY_POINT));
	//printf("program_001: %x\n", (u64)program_001);
	//printf("task.stack:  %p\n", tasks[0].stack);
}
