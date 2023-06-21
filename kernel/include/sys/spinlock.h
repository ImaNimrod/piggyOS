#ifndef _KERNEL_SPINLOCK_H
#define _KERNEL_SPINLOCK_H

#include <stdint.h>

typedef volatile uint32_t spinlock_t;

/* function declarations */
void spin_lock(spinlock_t* lock);
void spin_unlock(spinlock_t* lock);

#endif
