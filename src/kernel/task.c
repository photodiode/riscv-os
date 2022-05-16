
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
	str[5] = ',';
	str[6] = ' ';
	str[7] = 65 + HART_ID;
	str[8] = '!';
	str[9] = '\n';

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


void kernel_trap_user();


void task_create() {

	u16 id = task_first_free;
	task_first_free += 1;

	tasks[id] = (task) {
		.state  = TS_RUNNING,
		.parent = 0,
		.id     = id,

		.stack     = alloc(2),
		.pagetable = alloc(1),
		.frame     = alloc(1)
	};

	tasks[id].frame->x[1] = 0x2fff; // sp
	tasks[id].frame->pc   = TASK_ENTRY_POINT;
	tasks[id].frame->satp = MAKE_SATP(tasks[id].pagetable);

	tasks[id].frame->kernel_satp = MAKE_SATP(kernel_pagetable);
	tasks[id].frame->kernel_sp = K_STACK_START + (1 * K_STACK_SIZE);

	tasks[id].frame->pagetable_address = (u64)tasks[id].pagetable;


	mmu_map(tasks[id].pagetable, (u64)program_001, TASK_ENTRY_POINT, 0x1000, MMU_PTE_READ_EXECUTE | MMU_PTE_USER);
	mmu_map(tasks[id].pagetable, (u64)tasks[id].stack, 0x1000, 0x2000, MMU_PTE_READ_WRITE | MMU_PTE_USER);

	mmu_map(tasks[id].pagetable, (u64)kernel_trap_user, (u64)kernel_trap_user, 0x1000, MMU_PTE_READ_EXECUTE);
	mmu_map(tasks[id].pagetable, (u64)tasks[id].frame, (u64)tasks[id].frame, PAGE_SIZE, MMU_PTE_READ_WRITE);

}


void tasks_init() {

	tasks = alloc(TASK_PAGES);
	task_count = (PAGE_SIZE * TASK_PAGES) / sizeof(task);

	printf("tasks: %p\n", tasks);
	printf("task count: %d\n", task_count);

	task_create();
	task_create();
	task_create();
	task_create();
}


void load_task(u64 pc);


mtx task_lock;

volatile u16 i = 0;

void task_start() {

	mtx_lock(&task_lock);

	u16 id = i;
	i = (i + 1) % task_first_free;

	mtx_unlock(&task_lock);

	rv_status status = {.raw = csrr(sstatus)};
	status.spp  = 0; // set to user mode
	status.sie  = 0; // turn off interrupts
	status.spie = 0; // turn off interrupts after trap / mode change to supervisor
	csrw(sstatus, status.raw);

	csrw(sscratch, (u64)tasks[id].frame);
	//csrw(sepc, tasks[id].frame->pc);

	// after this nothing outside the task will work
	/*csrw(satp, MAKE_SATP(tasks[id].pagetable));
	sfence_vma();

	asm("li sp, 0x2FFF");
	asm("sret");*/

	load_task(tasks[id].frame->pc);
}


