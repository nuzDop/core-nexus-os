#ifndef SPINLOCK_H
#define SPINLOCK_H

#include <stdbool.h>

typedef volatile bool spinlock_t;

static inline void spinlock_acquire(spinlock_t* lock) {
    while (__sync_lock_test_and_set(lock, 1)) {
        while (*lock) {
            __asm__ volatile("pause");
        }
    }
}

static inline void spinlock_release(spinlock_t* lock) {
    __sync_lock_release(lock);
}

#endif
