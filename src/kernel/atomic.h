
#ifndef atomic_h
#define atomic_h


#include <types.h>


typedef i32 splk;


void splk_lock(splk* lock);
void splk_unlock(splk* lock);


#endif // atomic_h
