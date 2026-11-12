#ifndef KERNEL_WAIT_H
#define KERNEL_WAIT_H

#include <stdint.h>
#include "list.h"
#include "task.h"
#include "kerr.h"

typedef struct {
    list_node_t head;
} wait_queue_t;

static inline void wait_queue_init(wait_queue_t *wq)
{
    list_init(&wq->head);
}

// blocks calling task on wait queue
// MUST be called with critical section active (pass key)
// returns with critical section r already dropped
kerr_t wait_block(wait_queue_t *wq, uint32_t timeout_ticks, void *sync_obj, uint32_t key);

// wakes the highest priority waiter and returns its tcb so caller can check preemption
// returns NULL if nobody was waiting
tcb_t *wait_wake_one(wait_queue_t *wq, kerr_t result);

// wakes everyone sleeping on this queue
void wait_wake_all(wait_queue_t *wq, kerr_t result);

#endif // KERNEL_WAIT_H
