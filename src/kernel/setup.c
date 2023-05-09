
#include <types.h>
#include <riscv.h>
#include <sbi.h>

#include "print.h"


void _start_hart(u64 hart_id);
void dt_parse(u64 dtb_address);


void setup(u64 hart_id, u64 dtb_address) {

	puts("\n\33[31;1m]\33[0m RISC-V OS \33[31;1m[\33[0m\n\n");

	printf("Booting on hart %d\n", hart_id);

	sbi_ret ret;
	ret = sbi_get_spec_version();
	printf("SBI: v%d.%d\n", ret.value >> 24, ret.value & 0xffffff);


	dt_parse(dtb_address);


	ret = sbi_hart_start(1, (u64)_start_hart, 42);


	printf("\n\33[90;1mQuit: Ctrl + A, then X\33[0m\n");

	_start_hart(hart_id);
}
