#include <sbi.h>
#include <atomic.h>

#include "print.h"
#include "system.h"


typedef struct {
	const u64  code : 63;
	const bool interrupt : 1;
} trap_cause;


u64 trap(const trap_cause cause, const u64 value, const u64 epc) {

	if (cause.interrupt) { // interrupt
		switch (cause.code) {
			case  5: { // supervisor timer interrupt
				u64 time = csrr(time);
				sbi_set_timer(U64_MAX); // turn off timer

				static splk lock;
				splk_lock(&lock);

				printf("interrupt %d (%d) on Hart %d\n", cause.code, value, regr(tp));

				splk_unlock(&lock);

				sbi_set_timer(time + (system.timebase * 1)); // set new timer

				break;
			}

			default: break;
		}
	}

	return epc;
}
