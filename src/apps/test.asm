	.text
	.attribute	4, 16
	.attribute	5, "rv64i2p0_m2p0_a2p0_f2p0_d2p0_c2p0"
	.file	"test.c"
	.section	.text._start,"ax",@progbits
	.globl	_start
	.p2align	1
	.type	_start,@function
_start:
	#APP
	mv	a0, tp
	#NO_APP
	addiw	a0, a0, 65
.LBB0_2:
	auipc	a1, %pcrel_hi(.L.str)
	addi	a1, a1, %pcrel_lo(.LBB0_2)
	sb	a0, 7(a1)
	#APP
	mv	a1, a1
	#NO_APP
	#APP
	li	a0, 4
	#NO_APP
	#APP
	ecall	
	#NO_APP
.LBB0_1:
	j	.LBB0_1
.Lfunc_end0:
	.size	_start, .Lfunc_end0-_start

	.type	.L.str,@object
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	"Hello, -!\n"
	.size	.L.str, 11

	.ident	"clang version 14.0.0 (Fedora 14.0.0-1.fc36)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
