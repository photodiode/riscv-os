	.text
	.attribute	4, 16
	.attribute	5, "rv64i2p0_m2p0_a2p0_c2p0"
	.file	"hello.c"
	.globl	_start
	.p2align	1
	.type	_start,@function
_start:
	addi	sp, sp, -16
	sd	ra, 8(sp)
	sd	s0, 0(sp)
	addi	s0, sp, 16
	call	main
	j	.LBB0_1
.LBB0_1:
	j	.LBB0_1
.Lfunc_end0:
	.size	_start, .Lfunc_end0-_start

	.globl	main
	.p2align	1
	.type	main,@function
main:
	addi	sp, sp, -48
	sd	ra, 40(sp)
	sd	s0, 32(sp)
	addi	s0, sp, 48
	li	a0, 0
	sw	a0, -20(s0)
	lui	a1, %hi(.L.str)
	addi	a1, a1, %lo(.L.str)
	sd	a1, -32(s0)
	sw	a0, -36(s0)
	j	.LBB1_1
.LBB1_1:
	lw	a1, -36(s0)
	li	a0, 4
	blt	a0, a1, .LBB1_4
	j	.LBB1_2
.LBB1_2:
	lb	a0, -36(s0)
	addiw	a0, a0, 65
	ld	a1, -32(s0)
	sb	a0, 6(a1)
	ld	a0, -32(s0)
	call	print
	call	wait
	j	.LBB1_3
.LBB1_3:
	lw	a0, -36(s0)
	addiw	a0, a0, 1
	sw	a0, -36(s0)
	j	.LBB1_1
.LBB1_4:
	li	a0, 0
	ld	ra, 40(sp)
	ld	s0, 32(sp)
	addi	sp, sp, 48
	ret
.Lfunc_end1:
	.size	main, .Lfunc_end1-main

	.globl	print
	.p2align	1
	.type	print,@function
print:
	addi	sp, sp, -32
	sd	ra, 24(sp)
	sd	s0, 16(sp)
	addi	s0, sp, 32
	sd	a0, -24(s0)
	ld	a0, -24(s0)
	#APP
	mv	a1, a0
	#NO_APP
	#APP
	li	a0, 4
	#NO_APP
	#APP
	ecall	
	#NO_APP
	ld	ra, 24(sp)
	ld	s0, 16(sp)
	addi	sp, sp, 32
	ret
.Lfunc_end2:
	.size	print, .Lfunc_end2-print

	.globl	wait
	.p2align	1
	.type	wait,@function
wait:
	addi	sp, sp, -32
	sd	ra, 24(sp)
	sd	s0, 16(sp)
	addi	s0, sp, 32
	li	a0, 0
	sd	a0, -24(s0)
	j	.LBB3_1
.LBB3_1:
	ld	a1, -24(s0)
	lui	a0, 16384
	addiw	a0, a0, -2
	bltu	a0, a1, .LBB3_4
	j	.LBB3_2
.LBB3_2:
	j	.LBB3_3
.LBB3_3:
	ld	a0, -24(s0)
	addi	a0, a0, 1
	sd	a0, -24(s0)
	j	.LBB3_1
.LBB3_4:
	ld	ra, 24(sp)
	ld	s0, 16(sp)
	addi	sp, sp, 32
	ret
.Lfunc_end3:
	.size	wait, .Lfunc_end3-wait

	.type	.L.str,@object
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"Hello  !\n"
	.size	.L.str, 10

	.ident	"clang version 14.0.0 (Fedora 14.0.0-1.fc36)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym main
	.addrsig_sym print
	.addrsig_sym wait
