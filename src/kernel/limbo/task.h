
#ifndef task_h
#define task_h

#include <types.h>

#include "atomic.h"
#include "mmu.h"

typedef enum __attribute__((__packed__)) {
	TASK_NONE,
	TASK_RUNNING,
	TASK_WAITING,
	TASK_SLEEPING
} task_state;


typedef struct {
	task_state state;

	u16 parent;
	u16 id;

	splk lock;

	u8 tired;

	u64 pad[1];

	mmu_pte* pagetable;
	u64      pc;
	u8*      program;
	u8*      stack;

	trap_frame* frame;
} task;


void tasks_init(void);
void schedule_task(void);


#endif // task_h
