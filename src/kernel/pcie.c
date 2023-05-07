
#include <types.h>

#include "memory.h"
#include "mmu.h"
#include "print.h"

#include "pcie.h"


extern mmu_pte* kernel_pagetable;


typedef struct __attribute__((packed)) {
	const u16 vendor;
	const u16 device;
	      u16 command;
	const u16 status;

	const u8  rev, interface, subclass, class;
	      u8  bist;
	const u8  type;
	      u8  latency_timer;
	const u8  cache_line_size;

	u32 bar[6];

} pcie_header;


#define BAR_32(BAR, INDEX) (BAR[INDEX] & 0xFFFFFFF0U)
#define BAR_64(BAR, INDEX) (((u64)BAR[INDEX] & 0xFFFFFFF0UL) + (((u64)BAR[INDEX + 1] & 0xFFFFFFFFUL) << 32))


void pcie_init(void) {

	pcie_header* header = (void*)PCIE_BUS(0, 2, 0);

	//u8* pcie_mmio = (void*)PCIE_MMIO;

	puts("\n---- PCIE ----\n");

	printf("0x%x: 0x%x (0x%x)\n", header->vendor, header->device, header->type);
	printf("| status:  0b%b\n", header->status);
	printf("| class:   0x%x, 0x%x, 0x%x\n", header->class, header->subclass, header->interface);

	/*u8* framebuffer = alloc(1);
	//u16* pcie_mmio = alloc(1);

	mmu_map(kernel_pagetable, PCIE_ECAM, PCIE_ECAM, 0x10000000UL, MMU_PTE_READ_WRITE);
	mmu_map(kernel_pagetable, PCIE_MMIO, PCIE_MMIO, 0x40000000UL, MMU_PTE_READ_WRITE);

	mmu_map(kernel_pagetable, (u64)framebuffer, (u64)framebuffer, 0x1000, MMU_PTE_READ_WRITE);
	//mmu_map(kernel_pagetable, (u64)pcie_mmio, (u64)pcie_mmio, 0x1000, MMU_PTE_READ_WRITE);

	header->bar[2] = (u64)pcie_mmio;
	header->bar[0] = (u64)framebuffer | 0x4;

	pcie_mmio[0] = 0x4;
	pcie_mmio[1] = 0x1;

	printf("| BAR: 0x%x\n", (u64)header->bar[0]);
	printf("| BAR: 0x%x\n", (u64)header->bar[2]);

	printf("0x%x\n", pcie_mmio[0]);
	printf("0x%x\n", pcie_mmio[1]);
	printf("0x%x\n", pcie_mmio[2]);
	printf("0x%x\n", pcie_mmio[3]);*/

	puts("---- ---- ----\n\n");
}
