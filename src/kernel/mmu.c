
#include <types.h>

#include "print.h"
#include "memory.h"

#define mmu_internal
#include "mmu.h"


typedef union {
	struct {
		u64 valid    :  1;
		u64 read     :  1;
		u64 write    :  1;
		u64 execute  :  1;
		u64 user     :  1;
		u64 global   :  1;
		u64 accessed :  1;
		u64 dirty    :  1;
	};

	struct {
		u64 flags    :  8;
		u64 rsw      :  2; // ignore
		u64 ppn      : 44; // physical page number
		u64 reserved : 10; // must be set to zero
	};

	u64 raw;
} mmu_pte;


typedef mmu_pte* mmu_table;
#define MMU_PTE_ENTRIES 512


static inline void map(mmu_table root, const u64 physical_address, const u64 virtual_address, const u8 flags, const u8 level) {

	mmu_pte arg_flags = {.flags = flags};

	if (!(arg_flags.read || arg_flags.write || arg_flags.execute)) return;

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

		if (!v->valid) {
			mmu_table pagetable = alloc(1);

			v->ppn = (u64)pagetable / PAGE_SIZE;
			v->valid = 1;
		}

		v = (void*)((v->ppn * PAGE_SIZE) + (vpn[i] * sizeof(mmu_pte)));
		v->reserved = 0;
	}

	v->ppn = (ppn[2] << 18) | (ppn[1] << 9) | (ppn[0] << 0);

	v->flags = arg_flags.raw | MMU_PTE_VALID | MMU_PTE_ACCESSED | MMU_PTE_DIRTY;
}


static inline void unmap(mmu_table root) {

	for (int i = 0; i < MMU_PTE_ENTRIES; i++) {

		mmu_pte* entry = &root[i];

		if (entry->valid && !(entry->read || entry->write || entry->execute)) {

			mmu_table table_level_1 = (mmu_table)(entry->ppn * PAGE_SIZE);

			for (int k = 0; k < MMU_PTE_ENTRIES; k++) {
				entry = &table_level_1[i];

				if (entry->valid && !(entry->read || entry->write || entry->execute)) {

					mmu_table table_level_0 = (mmu_table)(entry->ppn * PAGE_SIZE);

					free(table_level_0);
					printf("free level 0\n");
				}
			}
			free(table_level_1);
			printf("free level 1\n");
		}
	}
}


#define CEIL_DIV(A, B) (1 + ((A - 1) / B))
void mmu_map(const mmu_table root, u64 physical_address, u64 virtual_address, const u64 size, const u8 flags) {

	// round to page boundry
	physical_address = (physical_address / PAGE_SIZE) * PAGE_SIZE;
	virtual_address  = (virtual_address  / PAGE_SIZE) * PAGE_SIZE;

	u64 page_count = 1;
	if (size > 0) page_count = CEIL_DIV(size, PAGE_SIZE);

	for (u64 i = 0; i < page_count; i++) {
		map(root, physical_address, virtual_address, flags, 0);
		physical_address += PAGE_SIZE;
		virtual_address  += PAGE_SIZE;
	}
}

void mmu_unmap(const mmu_table root) {
	unmap(root);
}


u64 mmu_v2p(const mmu_table root, const u64 virtual_address) {

	u64 vpn[] = {
		(virtual_address >> 12) & 0x1ff,
		(virtual_address >> 21) & 0x1ff,
		(virtual_address >> 30) & 0x1ff
	};

	mmu_pte* v = &root[vpn[2]];

	for (int i = 2; i >= 0; i--) {

		if (!v->valid) {
			printf("mmu: invalid entry (%x)\n", virtual_address);
			break;

		} else if ((v->read || v->write || v->execute)) {
			return (v->ppn * PAGE_SIZE) | (virtual_address & 0xfff);
		}

		v = (void*)((v->ppn * PAGE_SIZE) + (vpn[i - 1] * sizeof(mmu_pte)));
	}

	return 0;
}
