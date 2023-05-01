
#ifndef task_h
#define task_h

#include <types.h>

typedef enum __attribute__((__packed__)) {
	TASK_NONE,
	TASK_RUNNING,
	TASK_WAITING,
	TASK_SLEEPING
} task_state;


typedef struct {
	task_state state;
	u16        parent;
	u16        id;

	u64        pad[2];

	u64      pc;
	u8*      program;
	u8*      stack;
	mmu_pte* pagetable;

	trap_frame* frame;
} task;


void tasks_init(void);
void task_start(void);


#endif // task_h
