#include "semaphore.h"
#include "kerr.h"
#include "sched.h"
#include "port.h"
#include <stddef.h>

void semaphore_init(sem_t *sem, uint32_t initial_count, uint32_t max_count)
{
    sem->count = initial_count;
    sem->max_count = max_count;
    wait_queue_init(&sem->wait_queue);
}

kerr_t semaphore_take(sem_t *sem, uint32_t timeout_ticks)
{
    // blocking inside an isr bad
    if (port_in_isr() && timeout_ticks != K_NO_WAIT) 
    {
        return K_ERR_ISR;
    }

    uint32_t key = enter_critical();

    // token available,if avail, take and pass
    if (sem->count > 0) 
    {
        sem->count--;
        exit_critical(key);
        return K_OK;
    }

    // empty sem and user said dont wait
    if (timeout_ticks == K_NO_WAIT) 
    {
        exit_critical(key);
        return K_WOULD_BLOCK;
    }

    // pass key into wait_block so it can drop basepri and yield cleanly
    return wait_block(&sem->wait_queue, timeout_ticks, sem, key);
}

kerr_t semaphore_give(sem_t *sem)
{
    uint32_t key = enter_critical();

    // if someone is waiting, give token directly to them without touching count
    if (!list_is_empty(&sem->wait_queue.head)) 
    {
        tcb_t *woken = wait_wake_one(&sem->wait_queue, K_OK);

        // if the woken task has higher priority than currently running task, pend a switch
        // works identical for task context and isr context without touching inappropriately any scb registers
        if (woken != NULL && g_current_tcb != NULL && woken->prio < g_current_tcb->prio) 
        {
            port_yield();
        }

        exit_critical(key);
        return K_OK;
    }

    // none waiting, put token back into bucket if not full
    if (sem->count < sem->max_count) 
    {
        sem->count++;
        exit_critical(key);
        return K_OK;
    }

    // hit ceiling, signal was lost
    exit_critical(key);
    return K_ERR_OVERFLOW;
}
