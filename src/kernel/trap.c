
#include <stdarg.h>

#include <types.h>
#include <riscv.h>

#include "uart.h"
#include "print.h"

#include "memory.h"
#include "mmu.h"

#include "atomic.h"

#include "system.h"
#include "task.h"
#include "plic.h"


typedef struct {
	const u64  code : 63;
	const bool interrupt : 1;
} trap_cause;


//extern volatile mmu_pte* kernel_pagetable;


static splk print_lock;


#define panic(...) { splk_lock(&print_lock); printf("\33[31;1mFatal:\33[0m "); printf(__VA_ARGS__); asm("wfi"); }


void __attribute__((aligned(4))) kernel_trap(const trap_cause cause, const u64 value, trap_frame* frame) {

	if (cause.interrupt) { // interrupt
		switch (cause.code) {
			//case  0: printf("User software\n"); break;
			//case  1:  printf("Supervisor software\n"); break;
			//case  3: printf("Machine software\n"); break;

			//case  4: printf("User timer\n"); break;*/
			case  5: { // supervisor timer interrupt
				csrw(sie, csrr(sie) & ~INT_STI); // reset timer interrupts

				schedule_task();

				break;
			}
			/*case  7: printf("Machine timer (%d)\n", HART_ID); break;

			case  8: printf("User external\n"); break;*/
			case  9: { // Supervisor external

				const u32 claim_id = plic_get_claim(HART_ID);

				if (claim_id == PLIC_PCIE_IRQ) {
					putchar('!');
				}

				if (claim_id == PLIC_UART0_IRQ) {
					char c = uart_read();
					switch (c) {
						case  10: putchar('\n'); break;
						case  13: putchar('\n'); break;
						case 127: putchar('\b'); putchar(' '); putchar('\b'); break;

						case 'q': *(u32*)0x100000UL = 0x5555; break; // shutdown

						default: putchar(c); break;
					}
				}

				plic_complete(HART_ID, claim_id);

				break;
			}
			//case 11: printf("Machine external\n"); break;

			default: break;
		}
	} else { // instruction error
		switch (cause.code) {
			case  0: panic("CPU %d: Instruction address misaligned (0x%x) 0x%x\n", HART_ID, value, frame->epc); break;
			case  1: panic("CPU %d: Instruction access fault\n", HART_ID); break;
			case  2: panic("CPU %d: Illegal instruction 0x%x at 0x%x\n", HART_ID, value, frame->epc); break;

			//case  3: printf("Breakpoint\n"); break;

			case  4: panic("CPU %d: Load address misaligned\n", HART_ID); break;
			case  5: panic("CPU %d: Load access fault\n", HART_ID); break;

			case  6: panic("CPU %d: Store address misaligned at 0x%x by 0x%x\n", HART_ID, value, frame->epc); break;
			case  7: panic("CPU %d: Store access fault\n", HART_ID); break;

			case  8: { // Environment call from User mode
				switch (frame->a0) {
					case 4: {

						mmu_pte* pagetable = (void*)(frame->pagetable_address);
						char* str = (void*)mmu_v2p(pagetable, frame->a1);

						splk_lock(&print_lock);
						puts(str);
						splk_unlock(&print_lock);

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

			case 12: panic("CPU %d: Instruction page fault at 0x%x\n", HART_ID, value); break;
			case 13: panic("CPU %d: Load page fault at 0x%x by 0x%x\n", HART_ID, value, frame->epc); break;
			case 15: panic("CPU %d: Store page fault at 0x%x by 0x%x\n", HART_ID, value, frame->epc); break;

			default: break;
		}
		frame->epc += 4;
	}

	return;
}
