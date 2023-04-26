.text

.global _start

_start:
	lui  a0, %hi(str)
	addi a0, a0, %lo(str)
	call print
	call wait
	j _start

print:
	mv a1, a0
	li a0, 4
	ecall
	ret

wait:
	addi	sp, sp, -32
	sd	ra, 24(sp)
	sd	s0, 16(sp)
	addi	s0, sp, 32
	li	a0, 0
	sd	a0, -24(s0)
wait_loop:
	ld	a1, -24(s0)
	lui	a0, 0x8000
	addiw	a0, a0, -1
	bltu	a0, a1, wait_break
	ld	a0, -24(s0)
	addi	a0, a0, 1
	sd	a0, -24(s0)
	j	wait_loop
wait_break:
	ld	ra, 24(sp)
	ld	s0, 16(sp)
	addi	sp, sp, 32
	ret

str:
	.asciz	"Task _ on hart _!\n"
	.size	str, 9
