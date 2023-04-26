
#ifndef atomic_h
#define atomic_h


#include <types.h>


typedef volatile i32 mtx;


void mtx_lock(mtx* lock);
void mtx_unlock(mtx* lock);


#endif // atomic_h
