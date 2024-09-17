
#include <types.h>
#include <atomic.h>
#include <bytes.h>
#include <riscv.h>
#include <sbi.h>

#include "system.h"
#include "memory.h"
#include "mmu.h"

#include "print.h"


void _start_hart(u64 hart_id); // _start.s
void trap_vector(void);        // trap_vector.s


static mmu_pte* kernel_pagetable;


void kernel_setup(u64 hart_id, u64 dtb_address) {

	puts("\n\33[31;1m]\33[0m RISC-V OS \33[31;1m[\33[0m\n\n");

	// get SBI version
	const u64 sbi_ver     = sbi_get_spec_version();
	const u64 sbi_impl_id = sbi_get_impl_id();

	printf("SBI:    v%d.%d (%s)\n", sbi_ver >> 24, sbi_ver & 0xffffff, sbi_impl_id_strings[sbi_impl_id]);
	// ----

	system_init(dtb_address);

	alloc_init();

	// kernel page table mapping
	kernel_pagetable = alloc();

	mmu_map(kernel_pagetable, K_TEXT_START,   K_TEXT_START,   K_TEXT_END   - K_TEXT_START,   MMU_PTE_EXECUTE);
	mmu_map(kernel_pagetable, K_RODATA_START, K_RODATA_START, K_RODATA_END - K_RODATA_START, MMU_PTE_READ);

	mmu_map(kernel_pagetable, K_DATA_START, K_DATA_START, ((u64)system.memory + system.memory_size) - K_DATA_START, MMU_PTE_READ_WRITE);
	// ----

	printf("\n\33[90;1mQuit: Ctrl + A, then X\33[0m\n");

	// start up all the other harts
	for (u16 i = 0; i < system.hart_count; i++) {
		if (system.harts[i].id != hart_id) {
			sbi_hart_start(system.harts[i].id, (u64)_start_hart, 0);
		}
	}
	// ----

	_start_hart(hart_id);
}


void hart_setup(u64 hart_id) {

	regw(tp, hart_id); // save hart ID to hart's thread pointer

	// enable kernel pagetable
	csrw(satp, MAKE_SATP(kernel_pagetable));
	sfence_vma();
	// ----

	csrw(stvec, (u64)trap_vector); // set trap vector

	// turn on interrupts
	rv_status status = {.raw = csrr(sstatus)};
	status.sie = 1;
	csrw(sstatus, status.raw);
	csrw(sie, csrr(sie) | INT_SSI | INT_STI | INT_SEI);
	// ----

	//schedule_task();

	static splk lock;
	splk_lock(&lock);

	printf("Hello, I'm hart %d\n", hart_id);

	splk_unlock(&lock);

	sbi_set_timer(system.timebase * 1);

	while (1);
}
