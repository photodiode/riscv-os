
#include <types.h>
#include <riscv.h>

#include "uart.h"
#include "print.h"


void kernel();
void kernel_trap_vector();


volatile u32 cpu_count = 0;
static volatile u32 tmp_cpu_count = 0;


void setup() {

	// keep each cores hartid in its thread pointer register
	const u64 id = csrr(mhartid);
	set_tp(id);

	// wait for other cores to count their id
	while (tmp_cpu_count < id);
	tmp_cpu_count += 1;

	for (volatile u64 i = 0; i < 0xfffff; i++); // small wait to see if any other cores exist
	if (id == tmp_cpu_count-1) cpu_count = tmp_cpu_count; // if not assume we're last
	// ----

	// set basic PMP (physical memory protection)
	csrw(pmpcfg0,  PMP_R | PMP_W | PMP_X | PMP_NAPOT);
	csrw(pmpaddr0, U64_MAX >> 10); // entire 56 bit range
	// ----

	// turn off page based virtual memory system
	csrw(satp, 0); // supervisor address translation & protection
	sfence_vma();
	// ----

	// send all interrupts and exceptions to supervisor mode
	//csrw(mie, 0xaaa);
	csrw(medeleg, 0xffff);
	csrw(mideleg, 0xffff);
	csrw(sie, csrr(sie) | SIE_SEIE | SIE_STIE | SIE_SSIE);
	// ----

	// set Machine Previous Privilege mode to Supervisor for mret
	u64 x  = csrr(mstatus);
	    x &= ~MSTATUS_MPP_MASK;
	    x |=  MSTATUS_MPP_S;
	    x |=  1UL << 1; // SIE to 1
	//    x |=  1UL << 7; // MPIE to 1
	csrw(mstatus, x);
	// ----

	csrw(stvec, (u64)kernel_trap_vector); // set trap vector

	// set Machine Exception Program Counter to kernel() and "return" into it
	csrw(mepc, (u64)kernel);
	asm("mret");
	// ----
}
