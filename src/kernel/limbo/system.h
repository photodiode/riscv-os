
#ifndef system_h
#define system_h

#include <types.h>
#include "task.h"


typedef struct {
	u16   id;
	task* task;
} sys_info_hart;

typedef struct {
	u16           hart_count;
	sys_info_hart harts[];
} sys_info;


extern sys_info* system;


void sys_init(u16 hart_count);


#endif // system_h
