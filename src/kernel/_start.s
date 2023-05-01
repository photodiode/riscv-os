
.section .text.init
.global _start

_start:
	# setup stacks per hart
	la   sp, _k_stack_start # set the initial stack pointer to the end of the stack space
	lw   a0, _k_stack_size  # stack size per hart
	csrr a1, mhartid        # read current hart id
	mv   tp, a1             # keep each cores hartid in its thread pointer register
	addi a1, a1, 1          # add 1 to id
	mul  a0, a0, a1         # multiply id with stack size to get offset for hart
	add  sp, sp, a0	        # move the current hart stack pointer to its place in the stack space
	mv   a0, sp
	# ----

	call setup # setup.c
