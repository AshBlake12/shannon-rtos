#ifndef KERNEL_SEM_H
#define KERNEL_SEM_H

#include <stdint.h>
#include "wait.h"
#include "kerr.h"

//counting sem, max_count=1 makes it binary
typedef struct {
    uint32_t     count;
    uint32_t     max_count;
    wait_queue_t wait_queue;
} sem_t;

void   semaphore_init(sem_t *sem, uint32_t initial_count, uint32_t max_count);
kerr_t semaphore_take(sem_t *sem, uint32_t timeout_ticks);
kerr_t semaphore_give(sem_t *sem);

#endif
