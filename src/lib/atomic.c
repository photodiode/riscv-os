
#include <types.h>
#include <riscv.h>

#include "atomic.h"


void splk_lock(splk* lock) {

	// disable interrupts
	rv_status status = {.raw = csrr(sstatus)};
	status.sie = 0;
	csrw(sstatus, status.raw);
	// ----

	while (__sync_lock_test_and_set(lock, 1) != 0);
	__sync_synchronize();
}

bool splk_trylock(splk* lock) {

	// disable interrupts
	rv_status status = {.raw = csrr(sstatus)};
	status.sie = 0;
	csrw(sstatus, status.raw);
	// ----

	bool success = (__sync_lock_test_and_set(lock, 1) == 0);
	__sync_synchronize();

	if (!success) {
		// enable interrupts
		status.sie = 1;
		csrw(sstatus, status.raw);
		// ----
	}

	return success;
}

void splk_unlock(splk* lock) {
	__sync_synchronize();
	__sync_lock_release(lock);

	// enable interrupts
	rv_status status = {.raw = csrr(sstatus)};
	status.sie = 1;
	csrw(sstatus, status.raw);
	// ----
}
