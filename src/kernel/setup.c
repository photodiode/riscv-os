
#include <types.h>
#include <riscv.h>


void kernel(void);
void mtimer_vector(void);


void setup(void) {

	// keep each cores hartid in its thread pointer register
	const u64 id = csrr(mhartid);
	set_tp(id);

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
	csrw(sie, csrr(sie) | INT_SEI | INT_SSI);
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
