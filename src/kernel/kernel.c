
#include <types.h>
#include <riscv.h>

#include "uart.h"
#include "print.h"

#include "memory.h"
#include "mmu.h"

#include "system.h"

#include "pcie.h"

#include "plic.h"

#include "task.h"


void kernel_trap_user(void);


// kernel page table mapping
mmu_pte* kernel_pagetable;

static void mmu_map_kernel(void) {

	kernel_pagetable = alloc(1);

	mmu_map(kernel_pagetable, K_TEXT_START,   K_TEXT_START,   K_TEXT_END   - K_TEXT_START,   MMU_PTE_READ_EXECUTE);
	mmu_map(kernel_pagetable, K_RODATA_START, K_RODATA_START, K_RODATA_END - K_RODATA_START, MMU_PTE_READ);

	mmu_map(kernel_pagetable, K_DATA_START, K_DATA_START, MEMORY_END - K_DATA_START, MMU_PTE_READ_WRITE);

	mmu_map(kernel_pagetable, 0x100000, 0x100000, 0x1000, MMU_PTE_READ_WRITE); // QEMU VIRT TEST

	mmu_map(kernel_pagetable, UART0, UART0, 0x100,    MMU_PTE_READ_WRITE);
	mmu_map(kernel_pagetable, PLIC,  PLIC,  0x400000, MMU_PTE_READ_WRITE);

	mmu_map(kernel_pagetable, (u64)MTIMECMP, (u64)MTIMECMP, 0x8000, MMU_PTE_READ_WRITE); // timer interrupts
}
// ----


//void virtio_init();


#define FW_CFG 0x10100000UL

#define FW_CFG_SIGNATURE 0x0000
#define FW_CFG_ID        0x0100
#define FW_CFG_NOGRAPHIC 0x0400
#define FW_CFG_NB_CPUS   0x0500
#define FW_CFG_FILE_DIR  0x1900

static volatile u16* fw_cfg_selector = (void*)(FW_CFG +  8);
static volatile u64* fw_cfg_data     = (void*)(FW_CFG +  0);


void kernel(void) {

	static volatile bool started = false;

	if (HART_ID == 0) {

		uart_init();

		puts("\n\33[31;1m]\33[0m RISC-V OS \33[31;1m[\33[0m\n\n");

		// qemu fw_cfg
		*fw_cfg_selector = FW_CFG_NB_CPUS;
		u16 hart_count = (u32)*fw_cfg_data;
		// ----

		printf("CPU: %d cores\n", hart_count);

		alloc_init(hart_count);

		sys_init(hart_count);

		//pcie_init();

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

	/*rv_status status = {.raw = csrr(sstatus)};
	status.spp  = 0; // set to user mode
	status.sie  = 0; // turn off interrupts
	status.spie = 0; // turn off interrupts after trap / mode change to supervisor
	csrw(sstatus, status.raw);*/

	// enable interrupts
	csrw(sie, csrr(sie) | INT_SEI | INT_SSI);
	// ----

	schedule_task();
}
