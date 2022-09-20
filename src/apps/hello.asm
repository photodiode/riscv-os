.text

_start:
	lui  a0, %hi(str)
	addi a0, a0, %lo(str)

	mv a1, a0
	li a0, 4

	ecall
loop:
	j loop

str:
	.asciz	"Hello!\n"
	.size	str, 8
