
#include <types.h>

#include "print.h"

#include "pcie.h"


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

	const u32 bar[6];

} pcie_header;


#define BAR_64(BAR) (((u64)BAR[0] & 0xFFFFFFF0UL) + (((u64)BAR[0 + 1] & 0xFFFFFFFFUL) << 32))


void pcie_init(void) {

	pcie_header* header = (void*)PCIE_BUS(0, 2, 0);

	puts("\n---- PCIE ----\n");

	printf("0x%x: 0x%x (0x%x)\n", header->vendor, header->device, header->type);
	printf("| status:  0b%b\n", header->status);
	printf("| class:   0x%x, 0x%x, 0x%x\n", header->class, header->subclass, header->interface);

	printf("| BAR: 0x%x\n", BAR_64(header->bar));

	puts("---- ---- ----\n\n");
}
