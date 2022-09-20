
#include <types.h>
#include <riscv.h>

#include "print.h"

#include "memory.h"
#include "mmu.h"

#include "mutex.h"


void kernel_trap_user();


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
	u8*       program;
	u8*       stack;
	mmu_table pagetable;

	trap_frame* frame;
} task;


extern volatile mmu_table kernel_pagetable;


extern u8  _binary_hello_bin_start[];
extern u64 _binary_hello_bin_size;


#define TASK_PAGES 1
u16   task_count      = 0;
u16   task_first_free = 0;
task* tasks           = NULL;

#define TASK_ENTRY_POINT 0x0


void* memcpy(void* dest, const void* src, const u64 size);


void task_create() {

	u16 id = task_first_free;
	task_first_free += 1;

	#define CEIL_DIV(A, B) (1 + ((A - 1) / B))
	u64 program_pages = CEIL_DIV((u64)&_binary_hello_bin_size, PAGE_SIZE);

	tasks[id] = (task) {
		.state  = TS_RUNNING,
		.parent = 0,
		.id     = id,

		.program   = alloc(program_pages),
		.stack     = alloc(2),
		.pagetable = alloc(1),
		.frame     = alloc(1)
	};


	memcpy(tasks[id].program, &_binary_hello_bin_start, (u64)&_binary_hello_bin_size);


	tasks[id].frame->sp   = 0x4000; // sp
	tasks[id].frame->epc  = TASK_ENTRY_POINT;
	tasks[id].frame->satp = MAKE_SATP(tasks[id].pagetable);

	tasks[id].frame->kernel_satp = MAKE_SATP(kernel_pagetable);
	tasks[id].frame->kernel_sp   = K_STACK_START + (1 * K_STACK_SIZE);

	tasks[id].frame->pagetable_address = (u64)tasks[id].pagetable;


	mmu_map(tasks[id].pagetable, (u64)tasks[id].program, TASK_ENTRY_POINT, program_pages*PAGE_SIZE, MMU_PTE_READ_WRITE_EXECUTE | MMU_PTE_USER);
	mmu_map(tasks[id].pagetable, (u64)tasks[id].stack, 0x2000, 0x2000, MMU_PTE_READ_WRITE | MMU_PTE_USER);

	mmu_map(tasks[id].pagetable, (u64)kernel_trap_user, (u64)kernel_trap_user, 0x2000, MMU_PTE_READ_EXECUTE);
	mmu_map(tasks[id].pagetable, (u64)tasks[id].frame, (u64)tasks[id].frame, PAGE_SIZE, MMU_PTE_READ_WRITE);

	//printf("%x = %x\n", mmu_v2p(tasks[id].pagetable, 0x1030), (u64)tasks[id].program);
}


void tasks_init() {

	tasks = alloc(TASK_PAGES);
	task_count = (PAGE_SIZE * TASK_PAGES) / sizeof(task);

	task_create();
	task_create();
	task_create();
}


void load_task();


mtx task_lock;

volatile u16 i = 0;

void task_start() {

	mtx_lock(&task_lock);

	u16 id = i;
	i = (i + 1) % task_first_free;

	mtx_unlock(&task_lock);

	csrw(sscratch, (u64)tasks[id].frame);

	load_task();
}


