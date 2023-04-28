
#include <types.h>

#include "print.h"
#include "memory.h"

#include "mmu.h"


#define MMU_PTE_ENTRIES 512


static inline void map(mmu_pte* root, const u64 physical_address, const u64 virtual_address, const mmu_pte_flags flags, const u8 level) {

	if (!(flags.read || flags.write || flags.execute)) return;

	const u64 vpn[] = {
		(virtual_address >> 12) & 0x1ff,
		(virtual_address >> 21) & 0x1ff,
		(virtual_address >> 30) & 0x1ff
	};

	const u64 ppn[] = {
		(physical_address >> 12) & 0x1ff,
		(physical_address >> 21) & 0x1ff,
		(physical_address >> 30) & 0x3ffffff
	};

	mmu_pte* v = &root[vpn[2]];
	v->reserved = 0;

	for (int i = 1; i >= level; i--) {

		if (!v->flag.valid) {
			mmu_pte* pagetable = alloc(1);

			v->ppn = (u64)pagetable / PAGE_SIZE;
			v->flag.valid = 1;
		}

		v = (void*)((v->ppn * PAGE_SIZE) + (vpn[i] * sizeof(mmu_pte)));
		v->reserved = 0;
	}

	v->ppn = (ppn[2] << 18) | (ppn[1] << 9) | (ppn[0] << 0);

	v->flags = flags.raw | MMU_PTE_VALID | MMU_PTE_ACCESSED | MMU_PTE_DIRTY;
}


static inline void unmap(mmu_pte* root) {

	for (int i = 0; i < MMU_PTE_ENTRIES; i++) {

		mmu_pte* entry = &root[i];

		if (entry->flag.valid && !(entry->flag.read || entry->flag.write || entry->flag.execute)) {

			mmu_pte* table_level_1 = (mmu_pte*)(entry->ppn * PAGE_SIZE);

			for (int k = 0; k < MMU_PTE_ENTRIES; k++) {
				entry = &table_level_1[i];

				if (entry->flag.valid && !(entry->flag.read || entry->flag.write || entry->flag.execute)) {

					mmu_pte* table_level_0 = (mmu_pte*)(entry->ppn * PAGE_SIZE);

					free(table_level_0);
				}
			}
			free(table_level_1);
		}
	}
}


#define CEIL_DIV(A, B) (1 + ((A - 1) / B))
void mmu_map(mmu_pte* root, u64 physical_address, u64 virtual_address, const u64 size, const u8 flags) {

	// round to page boundry
	physical_address = (physical_address / PAGE_SIZE) * PAGE_SIZE;
	virtual_address  = (virtual_address  / PAGE_SIZE) * PAGE_SIZE;

	u64 page_count = 1;
	if (size > 0) page_count = CEIL_DIV(size, PAGE_SIZE);

	for (u64 i = 0; i < page_count; i++) {
		map(root, physical_address, virtual_address, (mmu_pte_flags){.raw = flags}, 0);
		physical_address += PAGE_SIZE;
		virtual_address  += PAGE_SIZE;
	}
}

void mmu_unmap(mmu_pte* root) {
	unmap(root);
}


u64 mmu_v2p(mmu_pte* root, const u64 virtual_address) {

	u64 vpn[] = {
		(virtual_address >> 12) & 0x1ff,
		(virtual_address >> 21) & 0x1ff,
		(virtual_address >> 30) & 0x1ff
	};

	mmu_pte* v = &root[vpn[2]];

	for (int i = 2; i >= 0; i--) {

		if (!v->flag.valid) {
			//printf("mmu: invalid entry (%x)\n", virtual_address);
			break;

		} else if ((v->flag.read || v->flag.write || v->flag.execute)) {
			return (v->ppn * PAGE_SIZE) | (virtual_address & 0xfff);
		}

		v = (void*)((v->ppn * PAGE_SIZE) + (vpn[i - 1] * sizeof(mmu_pte)));
	}

	return 0;
}
