
#include <types.h>
#include <riscv.h>

#include "plic.h"


static volatile u32* const plic = (void*)PLIC;


/*
#define PLIC_PRIORITY (PLIC + 0x0)
#define PLIC_PENDING (PLIC + 0x1000)
#define PLIC_MENABLE(hart) (PLIC + 0x2000 + (hart)*0x100)
#define PLIC_SENABLE(hart) (PLIC + 0x2080 + (hart)*0x100)
#define PLIC_MPRIORITY(hart) (PLIC + 0x200000 + (hart)*0x2000)
#define PLIC_SPRIORITY(hart) (PLIC + 0x201000 + (hart)*0x2000)
#define PLIC_MCLAIM(hart) (PLIC + 0x200004 + (hart)*0x2000)
#define PLIC_SCLAIM(hart) (PLIC + 0x201004 + (hart)*0x2000)
*/

void plic_enable(const u32 hart_id, const u32 id) {
	plic[2080 + hart_id*64] |= 1UL << id;
}


void plic_disable(const u32 hart_id, const u32 id) {
	plic[2080 + hart_id*64] &= ~(1UL << id);
}


void plic_set_priority(const u32 id, const u8 priority) {
	plic[id] = priority & 0x7; // 0 - 7
}


void plic_set_threshold(const u32 hart_id, const u8 threshold) {
	plic[0x80400 + hart_id*2048] = threshold & 0x7; // 0 - 7
}


u32 plic_get_claim(const u32 hart_id) {
	return plic[0x80401 + hart_id*2048];
}


void plic_complete(const u32 hart_id, const u32 claim_id) {
	plic[0x80401 + hart_id*2048] = claim_id;
}


void plic_init() {
	plic_set_priority(PLIC_UART0_ID, 1);
}


void plic_hart_init(const u32 hart_id) {
	plic_set_threshold(hart_id, 0);
	plic_enable(hart_id, PLIC_UART0_ID);
}
