
.section .text.init
.global _start

_start:
	# setup stacks per hart
	la   sp, _stack_start # set the initial stack pointer to the end of the stack space
	lw   a0, _stack_size  # stack size per hart
	csrr a1, mhartid      # read current hart id
	addi a1, a1, 1        # add 1 to id
	mul  a0, a0, a1       # multiply id with stack size to get offset for hart
	add  sp, sp, a0	      # move the current hart stack pointer to its place in the stack space
	# ----

	call setup # setup.c

spin:
	wfi
	j spin
