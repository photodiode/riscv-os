
.global mtimer_vector

.equ MTIMECMP, 0x02004000

.align 4
mtimer_vector:

	// make room to save registers
	addi sp, sp, -16

	// save the registers.
	sd a0, 72(sp)
	sd a1, 80(sp)

	// trigger supervisor timer interrupt
	li a1, (1 << 5)
        csrw sie, a1
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
	ld a0, 72(sp)
	ld a1, 80(sp)

	addi sp, sp, 16

	// return to whatever we were doing in the kernel
	mret
