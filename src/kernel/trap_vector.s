.global trap_vector
//.global load_task

.balign 4
trap_vector:

	csrr a0, scause
	csrr a1, stval
	//csrr a2, sscratch // trap frame
	csrr a2, sepc

	call trap // trap.c

	csrw sepc, a0

	sret
