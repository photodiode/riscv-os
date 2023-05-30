
#ifndef system_h
#define system_h

#include <types.h>
#include <devicetree.h>

//#include "task.h"


typedef struct {
	u16 id;
	u8  status;
	union {
		struct {
			u64 m:1, a:1, f:1, d:1, zicsr:1, zifencei:1; // g
			u64 q:1, c:1, b:1, v:1, zk:1, h:1, s:1;
			u64 zihintpause:1;
		} extension;
		u64 extensions[1];
	};
	//task* task;
} sys_info_hart;

typedef struct {
	u64 memory_size;
	u8* memory;
	u64 kernel_end;

	u64 timebase;

	u16           hart_count;
	sys_info_hart harts[64];
} sys_info;


extern sys_info system;


void system_init(dt_node root);


#endif // system_h
