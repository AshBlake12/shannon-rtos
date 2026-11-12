#include "wait.h"
#include "sched.h"
#include "time.h"
#include "port.h"
#include <stddef.h>

kerr_t wait_block(wait_queue_t *wq, uint32_t timeout_ticks, void *sync_obj, uint32_t key)
{
    tcb_t *curr = g_current_tcb;

    // mark task blocked and remember what sync primitive put it to sleep
    curr->state = TASK_BLOCKED;
    curr->blocked_on = sync_obj;
    curr->wait_result = K_OK;

    // yoink task out of ready list and bitmap so scheduler stops picking it
    sched_unready(curr);

    // walk wait list and insert sorted by priorit
    // fifo orfer
    list_node_t *pos = wq->head.next;
    while (pos != &wq->head) 
    {
        tcb_t *t = container_of(pos, tcb_t, event_node);
        if (curr->prio < t->prio) 
        {
            break;
        }
        pos = pos->next;
    }
    list_insert_before(pos, &curr->event_node);

    // if caller wants a timeout, register into timer delay list
    // (this was pain) 2^31-1 ticks so signed wrap compare in tick handler doesnt blow (u)
    if (timeout_ticks != K_FOREVER) 
    {
        if (timeout_ticks > K_MAX_TIMEOUT) 
        {
            timeout_ticks = K_FOREVER;
        } 
        else 
        {
            time_delay_insert(curr, timeout_ticks);
        }
    }

    // pend pendsv then immediately unmask basepri so switch happens right here
    // this stops us from falling through and returning before actually blocking
    port_yield();
    exit_critical(key);

    // execution pauses here. when waker or tick handler wakes us, we land here
    return curr->wait_result;
}

tcb_t *wait_wake_one(wait_queue_t *wq, kerr_t result)
{
    // nobody waiting, nothing to wake
    if (list_is_empty(&wq->head)) 
    {
        return NULL;
    }

    // first node is always the highest prio waiter
    list_node_t *node = wq->head.next;
    tcb_t *task = container_of(node, tcb_t, event_node);

    // yoinnk out of wait queue
    list_remove(node);
    list_init(node);

    // if it was also waiting on a timeout, kill the timer entry
    if (task->in_delay_list) 
    {
        time_delay_remove(task);
    }

    task->blocked_on = NULL;
    task->wait_result = result;

    // sched_ready puts it back on ready list and flips state to TASK_READY
    sched_ready(task);

    return task;
}

void wait_wake_all(wait_queue_t *wq, kerr_t result)
{
    // wake every waiter on this object until queue is empty
    while (!list_is_empty(&wq->head)) 
    {
        wait_wake_one(wq, result);
    }
}
