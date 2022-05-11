
#include <types.h>
#include <riscv.h>

#include "uart.h"
#include "print.h"

#include "memory.h"
#include "mmu.h"

#include "plic.h"
//#include "task.h"
void tasks_init();


// kernel page table mapping
volatile mmu_table kernel_pagetable;

static void mmu_map_kernel() {

	kernel_pagetable = alloc(1);

	mmu_map(kernel_pagetable, K_TEXT_START,   K_TEXT_START,   K_TEXT_END   - K_TEXT_START,   MMU_PTE_READ_EXECUTE);
	mmu_map(kernel_pagetable, K_RODATA_START, K_RODATA_START, K_RODATA_END - K_RODATA_START, MMU_PTE_READ);

	mmu_map(kernel_pagetable, K_DATA_START, K_DATA_START, MEMORY_END - K_DATA_START, MMU_PTE_READ_WRITE);

	mmu_map(kernel_pagetable, UART0, UART0, 0x100,    MMU_PTE_READ_WRITE);
	mmu_map(kernel_pagetable, PLIC,  PLIC,  0x400000, MMU_PTE_READ_WRITE);
}


void mmu_enable() {
	csrw(satp, MAKE_SATP(kernel_pagetable));
	sfence_vma();
}
// ----


extern volatile u32 cpu_count;
volatile static bool started = false;

void kernel() {

	while (!cpu_count); // wait for cpu count to be official

	if (CPU_ID == 0) {

		uart_init();

		//puts("\n◢◤◣◤◣◢◤◢◤◣◤◢◤◣\n\33[1;31m━━━━━━━━━━━━━━\33[0m\n\n");
		puts("\n\33[31;1m]\33[0m RISC-V OS \33[31;1m[\33[0m\n\n");

		printf("CPU: %d cores\n", cpu_count);

		memory_init(cpu_count);

		mmu_map_kernel();
		mmu_enable();

		plic_init();
		plic_hart_init(CPU_ID);

		tasks_init();

		asm("ecall");

		printf("\n\33[90;1mQuit: Ctrl + A, then X\33[0m\n");

		__sync_synchronize(); // memory barrier

		started = true;

	} else { // other cores
		while(!started);

		__sync_synchronize(); // memory barrier

		mmu_enable();
		plic_hart_init(CPU_ID);

		putchar('#');
	}

	//scheduler();
	while (1);
}
