
#ifndef riscv_h
#define riscv_h

#include <types.h>

#define asm __asm__ volatile


#define csrr(reg) ({u64 __x; asm("csrr %0, "#reg : "=r" (__x)); __x;})
#define csrw(reg, x) asm("csrw "#reg", %0" : : "r" (x));


// Machine Status Register
#define MSTATUS_MPP_MASK (3UL << 11) // machine previous mode
#define MSTATUS_MPP_M    (3UL << 11)
#define MSTATUS_MPP_S    (1UL << 11)
#define MSTATUS_MPP_U    (0UL << 11)

#define MSTATUS_MPIE_MASK (1UL << 7)
// ----



// Physical Memory Protection Configuration
#define PMP_R (1UL << 0)
#define PMP_W (1UL << 1)
#define PMP_X (1UL << 2)

#define PMP_TOR   (1UL << 3)
#define PMP_NA4   (2UL << 3)
#define PMP_NAPOT (3UL << 3)
// ----



// Supervisor Address Translation and Protection Register
#define SATP_SV39 (8UL << 60)
#define SATP_SV48 (9UL << 60)

#define MAKE_SATP(PAGE_TABLE) ((((u64)PAGE_TABLE) >> 12) | SATP_SV39)

// flush the TLB
static inline void sfence_vma() {
	asm("sfence.vma zero, zero");
}
// ----



// Supervisor Interrupt Enable
#define SIE_SEIE (1UL << 9) // external
#define SIE_STIE (1UL << 5) // timer
#define SIE_SSIE (1UL << 1) // software
// ----



// read and write tp, the thread pointer, which holds this core's hartid
static inline u64 get_tp() {
	u64 x;
	asm("mv %0, tp" : "=r" (x));
	return x;
}

#define CPU_ID get_tp()

static inline void set_tp(u64 x) {
	asm("mv tp, %0" : : "r" (x));
}
// ----

#endif // riscv_h
