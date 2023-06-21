#include <sys/spinlock.h>

static uint32_t atomic_test_and_set(spinlock_t* lock) {
    register spinlock_t value = 1;
	
	__asm__ volatile("lock xchg	%0, %1"
					  : "=q" (value), "=m" (*lock)
					  : "0" (value));

    return value;
}

void spin_lock(spinlock_t* lock) {
    while(atomic_test_and_set(lock) == 0);
}

void spin_unlock(spinlock_t* lock) {
    *lock = 0;
}
