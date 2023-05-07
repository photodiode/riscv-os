
.section .text.init
.global _start

_start:
	csrr tp, mhartid # keep each cores hartid in its thread pointer register

	# set up stacks per hart
	la   sp, _k_stack_start # set the initial stack pointer to the end of the stack space
	lw   a6, _k_stack_size  # stack size per hart
	mv   a7, tp             # read current hart id
	addi a7, a7, 1          # add 1 to id
	mul  a6, a6, a7         # multiply id with stack size to get offset for hart
	add  sp, sp, a6	        # move the current hart stack pointer to its place in the stack space
	mv   a6, sp
	# ----

	# a0 should be a hart id and a1 should be the device tree address
	j setup
