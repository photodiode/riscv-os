
#ifndef mmu_h
#define mmu_h

#include <types.h>


// mmu page table entry flags
#define MMU_PTE_VALID (1 << 0)

#define MMU_PTE_READ               (1 << 1)
#define MMU_PTE_READ_WRITE         (3 << 1)
#define MMU_PTE_EXECUTE            (4 << 1)
#define MMU_PTE_READ_EXECUTE       (5 << 1)
#define MMU_PTE_READ_WRITE_EXECUTE (7 << 1)

#define MMU_PTE_USER     (1 << 4)
#define MMU_PTE_GLOBAL   (1 << 5)
#define MMU_PTE_ACCESSED (1 << 6)
#define MMU_PTE_DIRTY    (1 << 7)
// ----


#ifndef mmu_internal
typedef void* mmu_table;

void mmu_map(mmu_table, u64 physical_address, u64 virtual_address, u64 size, u8 flags);
void mmu_unmap(mmu_table);

u64  mmu_v2p(mmu_table, u64 virtual_address);
#endif


#endif // mmu_h
