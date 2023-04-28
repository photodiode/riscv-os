
#include <types.h>
#include <riscv.h>

#include "print.h"

#include "memory.h"
#include "mmu.h"

#include "atomic.h"


void kernel_trap_user(void);


typedef enum __attribute__((__packed__)) {
	TASK_NONE,
	TASK_RUNNING,
	TASK_WAITING,
	TASK_SLEEPING
} task_state;


typedef struct {
	task_state state;
	u16        parent;
	u16        id;

	u64        pad[2];

	u64      pc;
	u8*      program;
	u8*      stack;
	mmu_pte* pagetable;

	trap_frame* frame;
} task;


extern mmu_pte* kernel_pagetable;

#define hello_app_size 97
const u8 hello_app[hello_app_size] = {
	0x37, 0x05, 0x00, 0x00, 0x13, 0x05, 0xe5, 0x04,
	0xef, 0x00, 0xc0, 0x00, 0xef, 0x00, 0x20, 0x01,
	0x6f, 0xf0, 0x1f, 0xff, 0xaa, 0x85, 0x11, 0x45,
	0x73, 0x00, 0x00, 0x00, 0x82, 0x80, 0x01, 0x11,
	0x06, 0xec, 0x22, 0xe8, 0x00, 0x10, 0x01, 0x45,
	0x23, 0x34, 0xa4, 0xfe, 0x83, 0x35, 0x84, 0xfe,
	0x37, 0x05, 0x00, 0x08, 0x7d, 0x35, 0x63, 0x68,
	0xb5, 0x00, 0x03, 0x35, 0x84, 0xfe, 0x05, 0x05,
	0x23, 0x34, 0xa4, 0xfe, 0xe5, 0xb7, 0xe2, 0x60,
	0x42, 0x64, 0x05, 0x61, 0x82, 0x80, 0x54, 0x61,
	0x73, 0x6b, 0x20, 0x5f, 0x20, 0x6f, 0x6e, 0x20,
	0x68, 0x61, 0x72, 0x74, 0x20, 0x5f, 0x21, 0x0a,
	0x00
};


#define TASK_PAGES 1
u16   task_count      = 0;
u16   task_first_free = 0;
task* tasks           = NULL;

#define TASK_ENTRY_POINT 0x0


void* memcpy(void* dest, const void* src, const u64 size);


u16 task_create(void) {

	u16 id = task_first_free;
	task_first_free += 1;

	#define CEIL_DIV(A, B) (1 + ((A - 1) / B))
	u64 program_pages = CEIL_DIV((u64)hello_app_size, PAGE_SIZE);

	tasks[id] = (task) {
		.state  = TASK_WAITING,
		.parent = 0,
		.id     = id,

		.program   = alloc(program_pages),
		.stack     = alloc(2),
		.pagetable = alloc(1),
		.frame     = alloc(1)
	};


	memcpy(tasks[id].program, hello_app, hello_app_size);

	tasks[id].program[hello_app_size-14] = 65 + id; // change underscore for id

	tasks[id].frame->sp   = 0x4000 + 0x2000; // sp
	tasks[id].frame->epc  = TASK_ENTRY_POINT;
	tasks[id].frame->satp = MAKE_SATP(tasks[id].pagetable);

	tasks[id].frame->kernel_satp = MAKE_SATP(kernel_pagetable);
	tasks[id].frame->kernel_sp   = K_STACK_START + (1 * K_STACK_SIZE);

	tasks[id].frame->pagetable_address = (u64)tasks[id].pagetable;


	mmu_map(tasks[id].pagetable, (u64)tasks[id].program, TASK_ENTRY_POINT, program_pages*PAGE_SIZE, MMU_PTE_READ_WRITE_EXECUTE | MMU_PTE_USER);
	mmu_map(tasks[id].pagetable, (u64)tasks[id].stack, 0x4000, 0x2000, MMU_PTE_READ_WRITE | MMU_PTE_USER);

	mmu_map(tasks[id].pagetable, (u64)kernel_trap_user, (u64)kernel_trap_user, 0x2000, MMU_PTE_READ_EXECUTE);
	mmu_map(tasks[id].pagetable, (u64)tasks[id].frame, (u64)tasks[id].frame, PAGE_SIZE, MMU_PTE_READ_WRITE);

	//printf("%x = %x\n", mmu_v2p(tasks[id].pagetable, 0x1030), (u64)tasks[id].program);

	return id;
}


void tasks_init(void) {

	tasks = alloc(TASK_PAGES);
	task_count = (PAGE_SIZE * TASK_PAGES) / sizeof(task);

	printf("task size: %d\n", sizeof(task));
	printf("task max count: %d\n", task_count);

	task_create();
	task_create();
	task_create();
}


void load_task(void); // from kernel_trap_user.s


mtx task_lock;

void task_start(void) {

	u16 id = 0;

	mtx_lock(&task_lock);

	while (tasks[id].state != TASK_WAITING) {
		id = (id + 1) % task_first_free;
	}

	tasks[id].state = TASK_RUNNING;
	tasks[id].program[hello_app_size-4] = 0x30 + HART_ID; // change underscore for hart id

	mtx_unlock(&task_lock);

	csrw(sscratch, (u64)tasks[id].frame);

	load_task();
}
