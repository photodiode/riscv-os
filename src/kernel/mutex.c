
#include <types.h>
#include <riscv.h>

#include "mutex.h"


void mtx_lock(mtx* lock) {
	while(__sync_lock_test_and_set(lock, 1) != 0);
	__sync_synchronize();
}

void mtx_unlock(mtx* lock) {
	__sync_synchronize();
	__sync_lock_release(lock);
}
