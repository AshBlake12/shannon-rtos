#include "mutex.h"
#include "sched.h"
#include "port.h"
#include <stddef.h>

void mutex_init(mutex_t *m, bool inherit)
{
    m->owner = NULL;
    m->inherit = inherit;
    wait_queue_init(&m->wq);
}

kerr_t mutex_lock(mutex_t *m, uint32_t timeout)
{
    // locking from an isr is illegal
    if (port_in_isr()) 
    {
        return K_ERR_ISR;
    }

    uint32_t key = enter_critical();

    // if free: claim ownership right away
    if (m->owner == NULL) 
    {
        m->owner = g_current_tcb;
        g_current_tcb->held_mutexes++;
        exit_critical(key);
        return K_OK; 
    }

    // recursive locking bad
    if (m->owner == g_current_tcb) 
    {
        exit_critical(key);
        return K_ERR_PARAM;
    }

    // on held mutex
    if (timeout == K_NO_WAIT) 
    {
        exit_critical(key);
        return K_WOULD_BLOCK;
    }

    // some kinf of priority inheritence if we outrank the owner, boost owner to our prio
    if (m->inherit && g_current_tcb->prio < m->owner->prio) 
    {
        sched_set_prio(m->owner, g_current_tcb->prio);
    }

    // block on wait queue (releases critical section inside wait_block)
    kerr_t res = wait_block(&m->wq, timeout, m, key);

    // if we timed out while owner was boosted to our prio, drop owner back down
    if (res == K_TIMEOUT) 
    {
      key = enter_critical();
        if (m->inherit && m->owner != NULL && m->owner->held_mutexes == 1) 
        {
            uint8_t target_prio = m->owner->base_prio;
            list_node_t *pos = m->wq.head.next;
            while (pos != &m->wq.head) 
            {
                tcb_t *waiter = container_of(pos, tcb_t, event_node);
                if (waiter->prio < target_prio) 
                {
                    target_prio = waiter->prio;
                }
                pos = pos->next;
            }
            sched_set_prio(m->owner, target_prio);
        }
        exit_critical(key);
    }
    return res;
}

kerr_t mutex_unlock(mutex_t *m)
{    
    if (port_in_isr()) 
    {
        return K_ERR_ISR;
    }

    uint32_t key = enter_critical();

    // only the task that locked it can unlock it
    if (m->owner != g_current_tcb) 
    {
        exit_critical(key);
        return K_ERR_NOT_OWNER;
    }

    g_current_tcb->held_mutexes--;

    // drop inherited priority once all held mutexes are released
    

    if (g_current_tcb->held_mutexes == 0 && g_current_tcb->prio != g_current_tcb->base_prio) 
    {
        sched_set_prio(g_current_tcb, g_current_tcb->base_prio);

    }

    // 4. direct hand-off to highest priority waiter (prevents barging/starvation)
    if (!list_is_empty(&m->wq.head)) 
    {
        tcb_t *woken = wait_wake_one(&m->wq, K_OK);
        m->owner = woken;
        woken->held_mutexes++;

        if (woken->prio < g_current_tcb->prio) 
        {
            port_yield();
        }

        exit_critical(key);
        return K_OK;
    }

    // nobody waiting, mark free
    m->owner = NULL;
    exit_critical(key);
    return K_OK;
}
