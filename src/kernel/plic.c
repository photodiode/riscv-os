
#include <types.h>
#include <riscv.h>

#include "plic.h"


static volatile u32* const plic = (void*)PLIC;


void plic_enable(const u32 hart_id, const u32 id) {
	plic[0x820 + hart_id*0x40] |= 1U << id;
}


void plic_disable(const u32 hart_id, const u32 id) {
	plic[0x820 + hart_id*0x40] &= ~(1U << id);
}


void plic_set_priority(const u32 id, const u8 priority) {
	plic[id] = priority & 0x7; // 0 - 7
}


void plic_set_threshold(const u32 hart_id, const u8 threshold) {
	plic[0x80400 + hart_id*0x800] = threshold & 0x7; // 0 - 7
}


u32 plic_get_claim(const u32 hart_id) {
	return plic[0x80401 + hart_id*0x800];
}


void plic_complete(const u32 hart_id, const u32 claim_id) {
	plic[0x80401 + hart_id*0x800] = claim_id;
}


void plic_init(void) {
	plic_set_priority(PLIC_UART0_ID, 1);
}


void plic_hart_init(const u32 hart_id) {
	plic_enable(hart_id, PLIC_UART0_ID);
	plic_set_threshold(hart_id, 0);
}
