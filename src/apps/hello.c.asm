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
	addi	sp, sp, -32
	sd	ra, 24(sp)
	sd	s0, 16(sp)
	addi	s0, sp, 32
	li	a0, 0
	sw	a0, -20(s0)
	sw	a0, -24(s0)
	j	.LBB1_1
.LBB1_1:
	lw	a1, -24(s0)
	li	a0, 4
	blt	a0, a1, .LBB1_4
	j	.LBB1_2
.LBB1_2:
	lw	a0, -24(s0)
	addiw	a0, a0, 65
	andi	a0, a0, 255
	call	putchar
	call	wait
	j	.LBB1_3
.LBB1_3:
	lw	a0, -24(s0)
	addiw	a0, a0, 1
	sw	a0, -24(s0)
	j	.LBB1_1
.LBB1_4:
	li	a0, 0
	ld	ra, 24(sp)
	ld	s0, 16(sp)
	addi	sp, sp, 32
	ret
.Lfunc_end1:
	.size	main, .Lfunc_end1-main

	.globl	putchar
	.p2align	1
	.type	putchar,@function
putchar:
	addi	sp, sp, -32
	sd	ra, 24(sp)
	sd	s0, 16(sp)
	addi	s0, sp, 32
	sb	a0, -17(s0)
	lbu	a0, -17(s0)
	#APP
	mv	a1, a0
	#NO_APP
	#APP
	li	a0, 3
	#NO_APP
	#APP
	ecall	
	#NO_APP
	ld	ra, 24(sp)
	ld	s0, 16(sp)
	addi	sp, sp, 32
	ret
.Lfunc_end2:
	.size	putchar, .Lfunc_end2-putchar

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
	lui	a0, 8192
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

	.ident	"clang version 16.0.1 (Fedora 16.0.1-1.fc38)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.addrsig_sym main
	.addrsig_sym putchar
	.addrsig_sym wait
