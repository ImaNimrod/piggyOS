#ifndef _KERNEL_SPINLOCK_H
#define _KERNEL_SPINLOCK_H

#include <stddef.h>
#include <stdint.h>
#include <sys/tasking.h>

typedef struct {
    volatile uint32_t value;
    task_t* task; 
} spinlock_t;

extern task_t* current_task;

/* function declarations */
void spinlock_init(spinlock_t* lock);
void spin_lock(spinlock_t* lock);
void spin_unlock(spinlock_t* lock);

#endif
