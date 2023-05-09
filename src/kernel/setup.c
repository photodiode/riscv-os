
#include <types.h>
#include <riscv.h>
#include <sbi.h>

#include "print.h"


void _start_hart(u64 hart_id);

void dt_parse(u64 dtb_address);


void __attribute__((aligned(4))) timer(void) {
	puts("timer\n");
	while (1);
}


void kernel_setup(u64 hart_id, u64 dtb_address) {

	(void)dtb_address;

	puts("\n\33[31;1m]\33[0m RISC-V OS \33[31;1m[\33[0m\n\n");

	sbi_ret ret;
	ret = sbi_get_spec_version();
	printf("SBI: v%d.%d\n", ret.value >> 24, ret.value & 0xffffff);


	//dt_parse(dtb_address);


	ret = sbi_hart_start(1, (u64)_start_hart, 0);


	printf("\n\33[90;1mQuit: Ctrl + A, then X\33[0m\n");

	rv_status status = {.raw = csrr(sstatus)};
	status.sie = 1;
	csrw(sstatus, status.raw);
	csrw(sie, csrr(sie) | INT_SSI | INT_STI | INT_SEI);

	csrw(stvec, (u64)timer);

	sbi_set_timer(0x1000000);

	_start_hart(hart_id);
}


void hart_setup(u64 hart_id) {

	regw(tp, hart_id);

	printf("Hello, I'm hart %d\n", hart_id);

	while (1);
}
