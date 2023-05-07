
#ifndef pcie_h
#define pcie_h

#include <types.h>

#define PCIE_PIO   0x3000000UL
#define PCIE_ECAM 0x30000000UL
#define PCIE_MMIO 0x40000000UL

#define PCIE_BUS(bus, slot, func) (PCIE_ECAM + (((bus * 256) + (slot * 8) + func) * 4096))

#define PCIE_DEVICE_MMIO(BUS, DEVICE, FUNCTION) (PCIE_MMIO + ((BUS - 0) << 20 | DEVICE << 15 | FUNCTION << 12))


void pcie_init(void);

#endif // pcie_h
