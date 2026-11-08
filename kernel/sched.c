#include "sched.h"
#include "task.h"
#include "time.h"

static list_node_t ready_lists[SCHED_PRIO_LEVELS];

static uint32_t ready_bitmap = 0;

void sched_init(void)
{
    ready_bitmap = 0;

    for (int i = 0; i < SCHED_PRIO_LEVELS; i++) 
    {
        list_init(&ready_lists[i]);
    }
}


void kernel_switch(void)
{
    tcb_t *prev = g_current_tcb;
    tcb_t *next = sched_pick();

    if (prev->state == TASK_RUNNING) 
    {
        prev->state = TASK_READY;
    }

    next->state = TASK_RUNNING;
    g_current_tcb = next;

    if (prev != next) 
    {
        time_slice_reset();
    }
}


uint32_t sched_get_ready_bitmap(void)
{
    return ready_bitmap;
}

void sched_ready(tcb_t *tcb)
{
    uint8_t prio = tcb->prio;
    if (prio >= SCHED_PRIO_LEVELS) 
    {
        return;
    }

    list_insert_tail(&ready_lists[prio], &tcb->node);
    
    ready_bitmap |= (1UL << (31 - prio));
    tcb->state = TASK_READY;
}

void sched_unready(tcb_t *tcb)
{
    uint8_t prio = tcb->prio;
    if (prio >= SCHED_PRIO_LEVELS) 
    {
        return;
    }

    list_remove(&tcb->node);
    if (list_is_empty(&ready_lists[prio])) 
    {
        ready_bitmap &= ~(1UL << (31 - prio));
    }
}

tcb_t *sched_pick(void)
{
    if (ready_bitmap == 0) 
    {
        return NULL;
    }

    
    uint32_t top_prio = (uint32_t)__builtin_clz(ready_bitmap);

    list_node_t *node = list_first(&ready_lists[top_prio]);
    if (!node) 
     {
        return NULL;
    }

    return container_of(node, tcb_t, node);
}

void sched_rotate(uint8_t prio)
{
    if (prio < SCHED_PRIO_LEVELS) 
    {
        list_rotate(&ready_lists[prio]);
    }
}
