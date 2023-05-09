
.section .rodata
_k_stack_size: .8byte 0x2000

.macro STACK_SETUP
	la   sp, _k_stack_start # set the initial stack pointer to the end of the stack space
	lw   t0, _k_stack_size  # stack size per hart
	mv   t1, a0             # read current hart id
	addi t1, t1, 1          # add 1 to id
	mul  t0, t0, t1         # multiply id with stack size to get offset for hart
	add  sp, sp, t0	        # move the current hart stack pointer to its place in the stack space
	mv   t0, sp
.endm

.section .text
.global _start
.global _start_hart

# start here on only one hart
# a0 should be a hart id and a1 should be the device tree address
_start:
	STACK_SETUP
	j kernel_setup
# ----

# each hart comes here
# a0 should be a hart id and a1 should be ???
_start_hart:
	STACK_SETUP
	j hart_setup
# ----
