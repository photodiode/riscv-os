
#ifndef mmu_h
#define mmu_h

#include <types.h>


// mmu page table entry flags
#define MMU_PTE_VALID (1 << 0)

#define MMU_PTE_READ               (1UL << 1)
#define MMU_PTE_READ_WRITE         (3UL << 1)
#define MMU_PTE_EXECUTE            (4UL << 1)
#define MMU_PTE_READ_EXECUTE       (5UL << 1)
#define MMU_PTE_READ_WRITE_EXECUTE (7UL << 1)

#define MMU_PTE_USER     (1UL << 4)
#define MMU_PTE_GLOBAL   (1UL << 5)
#define MMU_PTE_ACCESSED (1UL << 6)
#define MMU_PTE_DIRTY    (1UL << 7)
// ----


typedef union {
	struct {
		u8 valid    :  1;
		u8 read     :  1;
		u8 write    :  1;
		u8 execute  :  1;
		u8 user     :  1;
		u8 global   :  1;
		u8 accessed :  1;
		u8 dirty    :  1;
	};
	u8 raw;
} mmu_pte_flags;

typedef union {
	mmu_pte_flags flag;

	struct {
		u64 flags    :  8;
		u64 rsw      :  2; // ignore
		u64 ppn      : 44; // physical page number
		u64 reserved : 10; // must be set to zero
	};

	u64 raw;
} mmu_pte;


void mmu_map(mmu_pte*, u64 physical_address, u64 virtual_address, u64 size, u8 flags);
void mmu_unmap(mmu_pte*);

u64  mmu_v2p(mmu_pte*, u64 virtual_address);


#endif // mmu_h
