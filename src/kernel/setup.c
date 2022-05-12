
#include <types.h>
#include <riscv.h>

#include "uart.h"
#include "print.h"

#include "memory.h"


void kernel();
void kernel_trap_vector();
void mtimer_vector();


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
	csrw(mideleg, 0xffff);
	csrw(medeleg, 0xffff);
	csrw(sie, csrr(sie) | INT_SEI | INT_STI | INT_SSI);
	csrw(stvec, (u64)kernel_trap_vector); // set trap vector
	// ----

	// timer
	csrw(mie, csrr(mie) | INT_MTI);
	csrw(mtvec, (u64)mtimer_vector);
	// ----

	// set Machine Previous Privilege mode to Supervisor for mret
	u64 x  = csrr(mstatus);
	    x &= ~MSTATUS_MPP_MASK;
	    x |=  MSTATUS_MPP_S;
	    x |=  MSTATUS_SIE;
	csrw(mstatus, x);
	// ----

	// put the hart's kernel trap frame into sscratch for switching
	u64 hart_frame = K_STACK_START + ((K_STACK_SIZE + K_FRAME_SIZE) * HART_ID);
	csrw(sscratch, hart_frame);
	// ----

	// set Machine Exception Program Counter to kernel() and "return" into it
	csrw(mepc, (u64)kernel);
	asm("mret");
	// ----
}
