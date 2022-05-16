
#include <types.h>
#include <riscv.h>

#include "print.h"

#include "memory.h"
#include "mmu.h"

#include "mutex.h"


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

	trap_frame* frame;
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

}


void kernel_trap_user();


void task_start(u32 hart_id) {

	tasks[hart_id] = (task) {
		.state  = TS_RUNNING,
		.parent = 0,
		.id     = hart_id,

		.pc        = TASK_ENTRY_POINT,
		.stack     = alloc(2),
		.pagetable = alloc(1),
		.frame     = alloc(1)
	};

	tasks[hart_id].frame->satp = MAKE_SATP(tasks[hart_id].pagetable);

	tasks[hart_id].frame->kernel_satp = MAKE_SATP(kernel_pagetable);
	tasks[hart_id].frame->kernel_sp = K_STACK_START + (1 * K_STACK_SIZE);

	tasks[hart_id].frame->pagetable_address = (u64)tasks[hart_id].pagetable;


	mmu_map(tasks[hart_id].pagetable, (u64)program_001, TASK_ENTRY_POINT, 0x1000, MMU_PTE_READ_EXECUTE | MMU_PTE_USER);
	mmu_map(tasks[hart_id].pagetable, (u64)tasks[hart_id].stack, 0x1000, 0x2000, MMU_PTE_READ_WRITE | MMU_PTE_USER);

	mmu_map(tasks[hart_id].pagetable, (u64)kernel_trap_user, (u64)kernel_trap_user, 0x1000, MMU_PTE_READ_EXECUTE);
	mmu_map(tasks[hart_id].pagetable, (u64)tasks[hart_id].frame, (u64)tasks[hart_id].frame, PAGE_SIZE, MMU_PTE_READ_WRITE);

	mmu_map(tasks[hart_id].pagetable, 0x10000000UL, 0x10000000UL, 0x100, MMU_PTE_READ_WRITE | MMU_PTE_USER);


	rv_status status = {.raw = csrr(sstatus)};
	status.spp  = 0; // set to user mode
	status.sie  = 0; // turn off interrupts
	status.spie = 0; // turn off interrupts after trap / mode change to supervisor
	csrw(sstatus, status.raw);

	csrw(sscratch, (u64)tasks[hart_id].frame);
	csrw(sepc, TASK_ENTRY_POINT);

	// after this nothing outside the task will work
	csrw(satp, MAKE_SATP(tasks[hart_id].pagetable));
	sfence_vma();

	asm("li sp, 0x2FFF");
	asm("sret");
}


