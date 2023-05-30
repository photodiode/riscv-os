
#include <types.h>
#include <atomic.h>
#include <bytes.h>
#include <riscv.h>
#include <sbi.h>

#include "system.h"
#include "memory.h"

#include "print.h"


void _start_hart(u64 hart_id);


void __attribute__((aligned(4))) timer(void) {
	puts("timer\n");
	while (1);
}


void kernel_setup(u64 hart_id, u64 dtb_address) {

	puts("\n\33[31;1m]\33[0m RISC-V OS \33[31;1m[\33[0m\n\n");

	u64 sbi_ver     = sbi_get_spec_version();
	u64 sbi_impl_id = sbi_get_impl_id();

	printf("SBI:    v%d.%d (%s)\n", sbi_ver >> 24, sbi_ver & 0xffffff, sbi_impl_id_strings[sbi_impl_id]);

	// initialize device tree parser
	u64 dt_size;
	dt_node dt_root = dt_init(dtb_address, &dt_size);
	if (dt_root.error || !dt_size) fatal("Device Tree is corrupted, missing or an unsupported version\n");
	// ----

	system_init(dt_root);

	alloc_init();


	printf("\n\33[90;1mQuit: Ctrl + A, then X\33[0m\n");

	for (u16 i = 0; i < system.hart_count; i++) {
		if (system.harts[i].id != hart_id) {
			sbi_hart_start(system.harts[i].id, (u64)_start_hart, 0);
		}
	}

	rv_status status = {.raw = csrr(sstatus)};
	status.sie = 1;
	csrw(sstatus, status.raw);
	csrw(sie, csrr(sie) | INT_SSI | INT_STI | INT_SEI);

	csrw(stvec, (u64)timer);

	sbi_set_timer(system.timebase * 1);

	_start_hart(hart_id);
}


void hart_setup(u64 hart_id) {

	regw(tp, hart_id);

	static splk lock;
	splk_lock(&lock);

	printf("Hello, I'm hart %d\n", hart_id);

	splk_unlock(&lock);

	while (1);
}
