
#include <types.h>
#include <riscv.h>

#include "uart.h"
#include "print.h"

#include "memory.h"
#include "mmu.h"

#include "plic.h"
//#include "task.h"
void tasks_init(void);
void task_start(void);


void kernel_trap_user(void);


// kernel page table mapping
volatile mmu_table kernel_pagetable;

static void mmu_map_kernel(void) {

	kernel_pagetable = alloc(1);

	mmu_map(kernel_pagetable, K_TEXT_START,   K_TEXT_START,   K_TEXT_END   - K_TEXT_START,   MMU_PTE_READ_EXECUTE);
	mmu_map(kernel_pagetable, K_RODATA_START, K_RODATA_START, K_RODATA_END - K_RODATA_START, MMU_PTE_READ);

	mmu_map(kernel_pagetable, K_DATA_START, K_DATA_START, MEMORY_END - K_DATA_START, MMU_PTE_READ_WRITE);

	mmu_map(kernel_pagetable, UART0, UART0, 0x100,    MMU_PTE_READ_WRITE);
	mmu_map(kernel_pagetable, PLIC,  PLIC,  0x400000, MMU_PTE_READ_WRITE);

	mmu_map(kernel_pagetable, (u64)MTIMECMP, (u64)MTIMECMP, 0x8000, MMU_PTE_READ_WRITE); // timer interrupts
}
// ----


extern volatile u32 cpu_count;
volatile static bool started = false;


//void virtio_init();


void kernel(void) {

	while (!cpu_count); // wait for cpu count to be official

	if (HART_ID == 0) {

		uart_init();

		puts("\n\33[31;1m]\33[0m RISC-V OS \33[31;1m[\33[0m\n\n");

		printf("CPU: %d cores\n", cpu_count);

		//virtio_init();

		memory_init(cpu_count);

		mmu_map_kernel();

		plic_init();
		plic_hart_init(HART_ID);

		tasks_init();

		printf("\n\33[90;1mQuit: Ctrl + A, then X\33[0m\n");

		started = true;

	} else { // other cores
		while (!started);
	}

	__sync_synchronize(); // memory barrier

	// enable kernel pagetable
	csrw(satp, MAKE_SATP(kernel_pagetable));
	sfence_vma();
	// ----

	// enable interrupts
	csrw(sie, INT_SEI | INT_STI | INT_SSI);
	csrw(stvec, (u64)kernel_trap_user);
	// ----

	rv_status status = {.raw = csrr(sstatus)};
	status.spp  = 0; // set to user mode
	status.sie  = 0; // turn off interrupts
	status.spie = 0; // turn off interrupts after trap / mode change to supervisor
	csrw(sstatus, status.raw);

	MTIMECMP[HART_ID] = MTIME;

	task_start();
}
