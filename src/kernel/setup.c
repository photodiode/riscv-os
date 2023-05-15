
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
	u64 sbi_ver = ret.value;

	ret = sbi_get_impl_id();
	u64 sbi_impl_id = ret.value;

	printf("SBI:   v%d.%d (%s)\n", sbi_ver >> 24, sbi_ver & 0xffffff, sbi_impl_id_strings[sbi_impl_id]);

	// device tree
	dt_node root = dt_init(dtb_address);

	if (root.error) fatal("Device Tree is corrupted, missing or an unsupported version\n");


	dt_prop mem_reg = dt_get_prop(dt_get_node(root, "memory@", 0), "reg");

	u64 memory     = swap_u32(((u32*)mem_reg.data)[1]) | ((u64)swap_u32(((u32*)mem_reg.data)[0]) << 32);
	u64 memory_len = swap_u32(((u32*)mem_reg.data)[3]) | ((u64)swap_u32(((u32*)mem_reg.data)[2]) << 32);
	u64 memory_end = memory + memory_len;


	dt_node cpus = dt_get_node(root, "cpus", 0);

	u32 hart_count = dt_count_nodes(cpus, "cpu@");
	printf("Harts: %d\n", hart_count);

	dt_prop timebase = dt_get_prop(cpus, "timebase-frequency");
	printf("-  %d Hz\n", swap_u32(*(u32*)timebase.data));

	for (u32 i = 0; i < hart_count; i++) {
		dt_node hart = dt_get_node(cpus, "cpu@", i);

		dt_prop isa = dt_get_prop(hart, "riscv,isa");
		printf("%d: %s\n", i, isa.data);
		dt_prop mmu = dt_get_prop(hart, "mmu-type");
		printf("   %s\n", mmu.data);

		dt_prop interrupt = dt_get_prop(dt_get_node(hart, "interrupt-controller", 0), "compatible");
		printf("   %s\n", interrupt.data);
	}
	// ----

	alloc_init(hart_count, memory_end);

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
