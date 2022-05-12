
#ifndef riscv_h
#define riscv_h

#include <types.h>

#define asm __asm__ volatile


#define csrr(reg) ({u64 __x; asm("csrr %0, "#reg : "=r" (__x)); __x;})
#define csrw(reg, x) asm("csrw "#reg", %0" : : "r" (x));


#define MTIME   *((volatile u64*)0x0200bff8)
#define MTIMECMP ((volatile u64*)0x02004000)


// Machine Status Register
#define MSTATUS_MPP_MASK (3UL << 11) // machine previous mode
#define MSTATUS_MPP_M    (3UL << 11)
#define MSTATUS_MPP_S    (1UL << 11)
#define MSTATUS_MPP_U    (0UL << 11)

#define MSTATUS_SIE (1UL << 1)
#define MSTATUS_MIE (1UL << 3)

#define MSTATUS_SPIE (1UL << 5)
#define MSTATUS_MPIE (1UL << 7)

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


// interrupt bits mie/sie/uie, mip/sip/uip, medeleg and mideleg
#define INT_USI (1UL <<  0) // user software
#define INT_SSI (1UL <<  1) // supervisor software
#define INT_MSI (1UL <<  3) // machine software

#define INT_UTI (1UL <<  4) // user timer
#define INT_STI (1UL <<  5) // supervisor timer
#define INT_MTI (1UL <<  7) // machine timer

#define INT_UEI (1UL <<  8) // user external
#define INT_SEI (1UL <<  9) // supervisor external
#define INT_MEI (1UL << 11) // machine external

#define INT_ALL (INT_USI | INT_SSI | INT_MSI | INT_UTI | INT_STI | INT_MTI | INT_UEI | INT_SEI | INT_MEI)
// ----



// read and write tp, the thread pointer, which holds this core's hartid
static inline u64 get_tp() {
	u64 x;
	asm("mv %0, tp" : "=r" (x));
	return x;
}

#define HART_ID get_tp()

static inline void set_tp(u64 x) {
	asm("mv tp, %0" : : "r" (x));
}
// ----

#endif // riscv_h
