
#include <types.h>
#include <riscv.h>
#include <sbi.h>
#include <bytes.h>

#include "memory.h"
#include "devicetree.h"

#include "print.h"


void _start_hart(u64 hart_id);


void __attribute__((aligned(4))) timer(void) {
	puts("timer\n");
	while (1);
}


void kernel_setup(u64 hart_id, u64 dtb_address) {

	puts("\n\33[31;1m]\33[0m RISC-V OS \33[31;1m[\33[0m\n\n");

	sbi_ret ret;
	ret = sbi_get_spec_version();
	printf("SBI: v%d.%d\n", ret.value >> 24, ret.value & 0xffffff);

	// device tree
	dt_init(dtb_address);

	//dt_node root;
	//dt_parse_node(0, &root);

	dt_prop mem_reg;
	dt_parse_prop(dt_get_prop(dt_find_node(0, "memory@", 0), "reg"), &mem_reg);

	u64 memory     = swap_u32(((u32*)mem_reg.data)[1]) | ((u64)swap_u32(((u32*)mem_reg.data)[0]) << 32);
	u64 memory_len = swap_u32(((u32*)mem_reg.data)[3]) | ((u64)swap_u32(((u32*)mem_reg.data)[2]) << 32);
	u64 memory_end = memory + memory_len;

	u32 cpus = dt_find_node(0, "cpus", 0);

	u32 hart_count = dt_count_nodes(cpus, "cpu@");
	printf("Harts: %d\n", hart_count);
	// ----

	alloc_init(4, memory_end);

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
