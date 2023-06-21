#ifndef _KERNEL_SPINLOCK_H
#define _KERNEL_SPINLOCK_H

#include <stdint.h>

/* function declarations */
void spinlock_lock(uint32_t* lock);
void spinlock_unlock(uint32_t* lock);

#endif 
