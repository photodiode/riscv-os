
#include <types.h>
#include <riscv.h>

#include "uart.h"
#include "print.h"
#include "plic.h"


typedef struct {
	const u64  code : 63;
	const bool interrupt : 1;
} trap_cause;


typedef struct {
	u64 x[32]; // general purpose registers
	//u64 f[32]; // floating point registers
} trap_frame;


u64 __attribute__((aligned(4))) kernel_trap(const trap_cause cause, const u64 value, u64 epc, const u64 hart_id, trap_frame* frame) {

	(void)frame;

	if (cause.interrupt) { // interrupt
		switch (cause.code) {
			case  0: printf("User software\n"); break;
			case  1: printf("Supervisor software\n"); break;
			case  3: printf("Machine software interrupt\n"); break;

			case  4: printf("User timer\n"); break;
			case  5: printf("Supervisor timer\n"); break;
			case  7: printf("Machine timer\n"); break;

			case  8: printf("User external\n"); break;
			case  9: { // Supervisor external

				const u32 claim_id = plic_get_claim(hart_id);

				if (claim_id == PLIC_UART0_ID) {
					char c = uart_read();
					switch (c) {
						case  10: putchar('\n'); break;
						case  13: putchar('\n'); break;
						case 127: putchar('\b'); putchar(' '); putchar('\b'); break;

						default: putchar(c); break;
					}
				}

				plic_complete(hart_id, claim_id);

				break;
			}
			case 11: printf("Machine external interrupt\n"); break;

			default: break;
		}
	} else { // instruction error
		printf("Error on CPU %d at address %x: ", hart_id, epc);
		switch (cause.code) {
			case  0: printf("Instruction address misaligned"); break;
			case  1: printf("Instruction access fault"); break;
			case  2: printf("Illegal instruction"); break;

			case  3: printf("Breakpoint"); break;

			case  4: printf("Load address misaligned"); break;
			case  5: printf("Load access fault"); break;

			case  6:  printf("Store address misaligned"); break;
			case  7:  printf("Store access fault"); break;

			case  8: printf("Environment call from User mode"); break;
			case  9: printf("Environment call from Supervisor mode"); break;
			case 11: printf("Environment call from Machine mode"); break;

			case 12: printf("Instruction page fault "); break;
			case 13: printf("Load page fault"); break;
			case 15: printf("Store page fault"); break;

			default: break;
		}
		printf(" (%x)\n", value); 
		epc += 4;
	}

	return epc;
}
