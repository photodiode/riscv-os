
#include <atomic.h>

#include "print.h"


typedef struct {
	const u64  code : 63;
	const bool interrupt : 1;
} trap_cause;


u64 trap(const trap_cause cause, const u64 value, const u64 epc) {

	if (cause.interrupt && cause.code == 5) csrw(sie, csrr(sie) & ~INT_STI); // reset timer interrupts

	static splk lock;
	splk_lock(&lock);

	printf("interrupt %d (%d) on Hart %d\n", cause.code, value, regr(tp));

	splk_unlock(&lock);

	return epc;
}
