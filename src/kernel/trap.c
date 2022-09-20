
#include <types.h>
#include <riscv.h>

#include "uart.h"
#include "print.h"

#include "memory.h"
#include "mmu.h"

#include "mutex.h"

#include "plic.h"


typedef struct {
	const u64  code : 63;
	const bool interrupt : 1;
} trap_cause;


extern volatile mmu_table kernel_pagetable;


void kernel_trap_user();
void task_start();


static mtx print_lock;


#define panic(...) { mtx_lock(&print_lock); printf(__VA_ARGS__); while(1); }


void __attribute__((aligned(4))) kernel_trap(const trap_cause cause, const u64 value, trap_frame* frame) {

	(void)value;

	if (cause.interrupt) { // interrupt
		switch (cause.code) {
			/*case  0: printf("User software\n"); break;
			case  1: {
				printf("Supervisor software\n");
				csrw(sip, csrr(sip) & ~INT_SSI);
				break;
			}
			case  3: printf("Machine software\n"); break;

			case  4: printf("User timer\n"); break;*/
			case  5: {
				//printf("%d ", HART_ID);

				csrw(sie, csrr(sie) & ~INT_STI);
				//MTIMECMP[HART_ID] = MTIME + 10000000UL; // next interrupt
				MTIMECMP[HART_ID] = MTIME + 10000UL; // next interrupt

				task_start(); // we don't come back from here
				break;
			}
			/*case  7: printf("Machine timer (%d)\n", HART_ID); break;

			case  8: printf("User external\n"); break;*/
			case  9: { // Supervisor external

				const u32 claim_id = plic_get_claim(HART_ID);

				if (claim_id == PLIC_UART0_ID) {
					mtx_lock(&print_lock);
					char c = uart_read();
					switch (c) {
						case  10: putchar('\n'); break;
						case  13: putchar('\n'); break;
						case 127: putchar('\b'); putchar(' '); putchar('\b'); break;

						default: putchar(c); break;
					}
					mtx_unlock(&print_lock);
				}

				plic_complete(HART_ID, claim_id);

				break;
			}
			//case 11: printf("Machine external\n"); break;

			default: break;
		}
	} else { // instruction error
		switch (cause.code) {
			case  0: panic("Instruction address misaligned\n"); break;
			case  1: panic("Instruction access fault\n"); break;
			case  2: panic("Illegal instruction (%x)\n", value); break;

			//case  3: printf("Breakpoint\n"); break;

			case  4: panic("Load address misaligned\n"); break;
			case  5: panic("Load access fault\n"); break;

			case  6:  panic("Store address misaligned at %x by %x\n", value, frame->epc); break;
			case  7:  panic("Store access fault\n"); break;

			case  8: { // Environment call from User mode
				//printf("Environment call from User mode\n");
				switch (frame->a0) {
					case 4: {

						mmu_table pagetable = (void*)(frame->pagetable_address);
						char* str = (void*)mmu_v2p(pagetable, frame->a1);

						mtx_lock(&print_lock);
						puts(str);
						mtx_unlock(&print_lock);

						break;
					}
					default: break;
				}
				break;
			}
			/*case  9: {
				printf("Environment call from Supervisor mode: a1 = %x\n", frame->a1);
				break;
			}
			case 11: printf("Environment call from Machine mode\n"); break;*/

			case 12: panic("Instruction page fault by %x\n", value); break;
			case 13: panic("Load page fault at %x by %x\n", value, frame->epc); break;
			case 15: panic("Store page fault at %x by %x\n", value, frame->epc); break;

			default: break;
		}
		frame->epc += 4;
	}

	return;
}
