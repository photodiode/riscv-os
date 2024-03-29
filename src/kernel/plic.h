
#ifndef plic_h
#define plic_h


#include <types.h>


#define PLIC 0x0C000000UL

#define PLIC_UART0_IRQ 0x0A
#define PLIC_PCIE_IRQ  0x20


void plic_init(void);
void plic_hart_init(u32 hart_id);

u32  plic_get_claim(u32 hart_id);
void plic_complete(u32 hart_id, u32 claim_id);


#endif // plic_h
