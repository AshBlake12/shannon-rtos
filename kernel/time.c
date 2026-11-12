#include "time.h"
#include "task.h"
#include "sched.h"
#include "port.h"
#include "list.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

volatile uint32_t g_tick = 0;
static uint32_t s_ticks_in_slice = 0;

static list_node_t s_delay_list; //wake tick ascending

extern volatile uint32_t g_systick_entry_cycle;
extern volatile uint32_t g_task_woken_cycle;

void time_delay_insert(tcb_t *tcb, uint32_t ticks)
{
    tcb->wake_tick = g_tick + ticks;
    tcb->in_delay_list = true;

    // sorted insert into delay list by wake_tick
    // using signed diff so tick count wrap-around behaves nicely (this was pain)
    list_node_t *pos = s_delay_list.next;
    while (pos != &s_delay_list) 
    {
        tcb_t *curr = container_of(pos, tcb_t, node);
        if ((int32_t)(tcb->wake_tick - curr->wake_tick) < 0) 
        {
            break;
        }
        pos = pos->next;
    }
    list_insert_before(pos, &tcb->node);
}

void time_delay_remove(tcb_t *tcb)
{
    // explicit bool check, never guess list presence from wake_tick
    if (tcb->in_delay_list) 
    {
        list_remove(&tcb->node);
        list_init(&tcb->node);
        tcb->in_delay_list = false;
        tcb->wake_tick = 0;
    }
}

void time_init(void)
{
    g_tick = 0;
    s_ticks_in_slice = 0;
    list_init(&s_delay_list);
}

void time_slice_reset(void)
{
    s_ticks_in_slice = 0;
}

void delay_list_insert_sorted(tcb_t *tcb)
{
    list_node_t *curr = s_delay_list.next;

    while (curr != &s_delay_list) {
        tcb_t *entry = container_of(curr, tcb_t, node);
        //tick swap.. please this was a pain in arse 
        if ((int32_t)(tcb->wake_tick - entry->wake_tick) < 0) 
        {
            break;
        }
        curr = curr->next;
    }
    list_insert_before(curr, &tcb->node);
}


void task_sleep(uint32_t ticks)
{
    if (ticks == 0 || g_current_tcb == NULL) 
    {
        return;
    }

    uint32_t key = enter_critical();

    g_current_tcb->state = TASK_BLOCKED;
    sched_unready(g_current_tcb);
    time_delay_insert(g_current_tcb, ticks);

    // pend pendsv and drop lock so context switch kicks in
    port_yield();
    exit_critical(key);
}

void kernel_tick(void)
{
    if (!g_kernel_running) 
    {
        return;
    }

    bool need_resched = false;
    g_tick++;
    s_ticks_in_slice++;

    // check head of delay list for any sleeping or timed-out tasks
    while (!list_is_empty(&s_delay_list)) 
    {
        list_node_t *head_node = list_first(&s_delay_list);
        tcb_t *sleeper = container_of(head_node, tcb_t, node);

        // signed compare: handles 32-bit tick wrap up to 2^31 ticks
        if ((int32_t)(g_tick - sleeper->wake_tick) >= 0) 
        {
            list_remove(&sleeper->node);
            list_init(&sleeper->node);
            sleeper->in_delay_list = false;
            sleeper->wake_tick = 0;

            // if task was blocked on a wait queue, timeout hit before someone gave it
            if (sleeper->state == TASK_BLOCKED) 
            {
                list_remove(&sleeper->event_node);
                list_init(&sleeper->event_node);
                sleeper->blocked_on = NULL;
                sleeper->wait_result = K_TIMEOUT;
            }

            sched_ready(sleeper);

            // higher prio task woke up, need preemption
            if (g_current_tcb == NULL || sleeper->prio < g_current_tcb->prio) 
            {
                need_resched = true;
            }
        } 
        else 
        {
            // list is sorted, so if head isnt ready nothing behind it is either
            break;
        }
    }

    // round robin time slice for equal priority tasks
    if (s_ticks_in_slice >= TIME_SLICE_TICKS) 
    {
        s_ticks_in_slice = 0;
        if (g_current_tcb != NULL && g_current_tcb->prio < SCHED_PRIO_IDLE) 
        {
            sched_rotate(g_current_tcb->prio);
            tcb_t *next = sched_pick();
            if (next != g_current_tcb) 
            {
                need_resched = true;
            }
        }
    }

    if (need_resched) 
    {
        port_yield();
    }
}
