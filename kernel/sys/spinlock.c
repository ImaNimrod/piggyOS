#include <sys/spinlock.h>

void spinlock_init(spinlock_t* lock) {
    lock->value = 0;
    lock->task = NULL;
}

void spin_lock(spinlock_t* lock) {
    do {
        while (__sync_lock_test_and_set(&lock->value, 0x01)); 
        if(current_task) 
            lock->task = current_task;
    } while (0);
}

void spin_unlock(spinlock_t* lock) {
    do { 
        lock->task = NULL; 
        __sync_lock_release(&lock->value);
    } while (0);
}
