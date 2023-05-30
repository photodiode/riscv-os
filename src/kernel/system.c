
#include <types.h>
#include <bytes.h>
#include <devicetree.h>

#include "system.h"
#include "print.h"


sys_info system = {0};


void system_init(dt_node dt_root) {

	dt_prop mem_reg = dt_get_prop(dt_get_node(dt_root, "memory@", 0), "reg");
	if (mem_reg.error) fatal("Couldn't find memory information in Device Tree\n");

	u64 memory_size = swap_u32(((u32*)mem_reg.data)[3]) | ((u64)swap_u32(((u32*)mem_reg.data)[2]) << 32);
	u64 memory      = swap_u32(((u32*)mem_reg.data)[1]) | ((u64)swap_u32(((u32*)mem_reg.data)[0]) << 32);

	system.memory_size = memory_size;
	system.memory      = (u8*)memory;


	dt_node cpus = dt_get_node(dt_root, "cpus", 0);

	system.hart_count = dt_count_nodes(cpus, "cpu@");

	dt_prop timebase = dt_get_prop(cpus, "timebase-frequency");
	system.timebase = swap_u32(*(u32*)timebase.data);


	printf("Harts:  %d\n", system.hart_count);
	printf("Timer:  %d Hz\n", system.timebase);
	printf("Memory: %d bytes (%p ... %p)\n", system.memory_size, system.memory, system.memory + system.memory_size);

	for (u32 i = 0; i < system.hart_count; i++) {
		dt_node hart = dt_get_node(cpus, "cpu@", i);

		dt_prop id  = dt_get_prop(hart, "reg");
		dt_prop isa = dt_get_prop(hart, "riscv,isa");
		dt_prop mmu = dt_get_prop(hart, "mmu-type");

		dt_prop interrupt = dt_get_prop(dt_get_node(hart, "interrupt-controller", 0), "compatible");

		system.harts[i].id = swap_u32(*(u32*)id.data);

		printf("%d: %s\n", system.harts[i].id, isa.data);
		printf("   %s\n", mmu.data);
		printf("   %s\n", interrupt.data);
	}

}
