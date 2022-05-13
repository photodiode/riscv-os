
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


extern volatile mmu_table kernel_pagetable;


void __attribute__((aligned(4096))) program_001() {

	char* str = (void*)0x1000;

	str[0] = 'H';
	str[1] = 'e';
	str[2] = 'l';
	str[3] = 'l';
	str[4] = 'o';
	str[5] = '!';
	str[6] = '\n';

	asm("li a0, 4");
	asm("li a1, 0x1000");
	asm("ecall");

	while (1);
}

void kernel_trap_user();


#define TASK_PAGES 1
u16   task_count      = 0;
u16   task_first_free = 0;
task* tasks           = NULL;

#define TASK_ENTRY_POINT 0x0


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
		.pagetable = alloc(1),
		.frame     = {0}
	};

	tasks[0].frame.satp = MAKE_SATP(tasks[0].pagetable);

	/*mmu_map(tasks[0].pagetable, (u64)program_001, TASK_ENTRY_POINT, 0x1000, MMU_PTE_READ_EXECUTE | MMU_PTE_USER);
	mmu_map(tasks[0].pagetable, (u64)tasks[0].stack, 0x1000, 0x2000, MMU_PTE_READ_WRITE | MMU_PTE_USER);

	csrw(satp, MAKE_SATP(tasks[0].pagetable));
	sfence_vma();*/

	mmu_map(kernel_pagetable, (u64)program_001, TASK_ENTRY_POINT, 0x1000, MMU_PTE_READ_EXECUTE | MMU_PTE_USER);
	mmu_map(kernel_pagetable, (u64)tasks[0].stack, 0x1000, 0x2000, MMU_PTE_READ_WRITE | MMU_PTE_USER);

	sfence_vma();

	u64 x  = csrr(sstatus);
	    x &= ~MSTATUS_SPP;
	    x |=  MSTATUS_SPIE;
	csrw(sstatus, x);

	csrw(sscratch, (u64)(&tasks[0].frame));

	asm("li sp, 0x3000");

	csrw(stvec, (u64)kernel_trap_user);
	csrw(sepc, TASK_ENTRY_POINT);
	asm("sret");
}


