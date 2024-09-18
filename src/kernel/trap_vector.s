
.global trap_vector
//.global load_task

.balign 0x1000
trap_vector:

	mv   t0, a0
	mv   t1, a1
	mv   t2, a2

	csrr a0, scause
	csrr a1, stval
	//csrr a2, sscratch // trap frame
	csrr a2, sepc

	call trap // trap.c

	csrw sepc, a0

	mv   a0, t0
	mv   a1, t1
	mv   a2, t2

	sret
