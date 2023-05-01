
#include <types.h>
#include <riscv.h>


void kernel(void);
void mtimer_vector(void);
void kernel_trap_vector(void);


void setup(void) {

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
	csrw(stvec, (u64)kernel_trap_vector);
	// ----

	// timer
	MTIMECMP[HART_ID] = -1;
	csrw(mie, csrr(mie) | INT_MTI);
	csrw(mtvec, (u64)mtimer_vector);
	// ----

	rv_status status = {.raw = csrr(mstatus)};
	status.mpp = 1; // set MPP (machine previous privilege) mode to supervisor for mret
	status.mie = 1; // enable machine-mode interrupts
	csrw(mstatus, status.raw);
	// ----

	// set MEPC (machine exception program counter) to kernel() and "return" into it
	csrw(mepc, (u64)kernel);
	asm("mret");
	// ----
}
