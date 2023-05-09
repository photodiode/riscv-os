
.global mtimer_vector

.equ MTIMECMP, 0x02004000

.balign 4
mtimer_vector:

	// make room to save registers
	addi sp, sp, -16

	// save the registers.
	sd a0, 0(sp)
	sd a1, 8(sp)

	// trigger supervisor timer interrupt
	csrr a1, sie
	ori  a1, a1, (1 << 5)
	csrw sie, a1

	csrr a1, mip
	ori  a1, a1, (1 << 5) // STIP
	csrw mip, a1
	// ----

	// set mtimecmp[hart_id] to -1
	li  a0, MTIMECMP
	li  a1, 8
	mul a1, a1, tp
	add a0, a0, a1
	li  a1, -1
	sw  a1, 0(a0)
	// ----

	// restore registers.
	ld a0, 0(sp)
	ld a1, 8(sp)

	addi sp, sp, 16

	// return to whatever we were doing
	mret
