
#ifndef mutex_h
#define mutex_h


#include <types.h>


typedef volatile i32 mtx;


void mtx_lock(mtx* lock);
void mtx_unlock(mtx* lock);


#endif // mutex_h
