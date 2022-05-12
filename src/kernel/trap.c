
#include <types.h>
#include <riscv.h>

#include "uart.h"
#include "print.h"

#include "memory.h"

#include "plic.h"


typedef struct {
	const u64  code : 63;
	const bool interrupt : 1;
} trap_cause;


u64 __attribute__((aligned(4))) kernel_trap(const trap_cause cause, const u64 value, u64 epc, trap_frame* frame) {

	(void)frame;
	(void)value;

	if (cause.interrupt) { // interrupt
		switch (cause.code) {
			case  0: printf("User software\n"); break;
			case  1: {
				printf("Supervisor software\n");
				csrw(sip, csrr(sip) & ~INT_SSI);
				break;
			}
			case  3: printf("Machine software\n"); break;

			case  4: printf("User timer\n"); break;
			case  5: {
				printf("Supervisor timer (%d)\n", HART_ID);
				csrw(sie, csrr(sie) & ~INT_STI);
				MTIMECMP[HART_ID] = MTIME + 10000000UL; // next interrupt
				break;
			}
			case  7: printf("Machine timer (%d)\n", HART_ID); break;

			case  8: printf("User external\n"); break;
			case  9: { // Supervisor external

				const u32 claim_id = plic_get_claim(HART_ID);

				if (claim_id == PLIC_UART0_ID) {
					char c = uart_read();
					switch (c) {
						case  10: putchar('\n'); break;
						case  13: putchar('\n'); break;
						case 127: putchar('\b'); putchar(' '); putchar('\b'); break;

						default: putchar(c); break;
					}
				}

				plic_complete(HART_ID, claim_id);

				break;
			}
			case 11: printf("Machine external\n"); break;

			default: break;
		}
	} else { // instruction error
		switch (cause.code) {
			case  0: printf("Instruction address misaligned\n"); break;
			case  1: printf("Instruction access fault\n"); break;
			case  2: printf("Illegal instruction\n"); break;

			case  3: printf("Breakpoint\n"); break;

			case  4: printf("Load address misaligned\n"); break;
			case  5: printf("Load access fault\n"); break;

			case  6:  printf("Store address misaligned\n"); break;
			case  7:  printf("Store access fault\n"); break;

			case  8: printf("Environment call from User mode\n"); break;
			case  9: {
				printf("Environment call from Supervisor mode: a0 = %d\n", frame->x[9]);
				break;
			}
			case 11: printf("Environment call from Machine mode\n"); break;

			case 12: printf("Instruction page fault\n"); break;
			case 13: printf("Load page fault\n"); break;
			case 15: printf("Store page fault\n"); break;

			default: break;
		}
		epc += 4;
	}

	return epc;
}