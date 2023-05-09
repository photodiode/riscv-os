
#ifndef atomic_h
#define atomic_h


#include <types.h>


typedef i32 splk;


void splk_lock(splk*);
bool splk_trylock(splk*);
void splk_unlock(splk*);


#endif // atomic_h
