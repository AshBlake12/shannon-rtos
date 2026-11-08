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

void time_init(void)
{
    g_tick = 0;
    s_ticks_in_slice = 0;
    list_init(&s_delay_list);
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

    /* Insert before curr */
    tcb->node.next = curr;
    tcb->node.prev = curr->prev;
    curr->prev->next = &tcb->node;
    curr->prev = &tcb->node;
}

void task_sleep(uint32_t ticks)
{
    if (ticks == 0 || g_current_tcb == NULL) 
    {
        return;
    }

    uint32_t status = enter_critical();

    
    g_current_tcb->wake_tick = g_tick + ticks;
    g_current_tcb->state = TASK_BLOCKED;

    sched_unready(g_current_tcb);
    delay_list_insert_sorted(g_current_tcb);

    exit_critical(status);
    port_yield();

}



void kernel_tick(void)
{
    bool need_resched = false;
    g_tick++;
    s_ticks_in_slice++;

    while (!list_is_empty(&s_delay_list))  //delay list chceck for sleepers 
    {
        list_node_t *head_node = list_first(&s_delay_list);
        tcb_t *sleeper = container_of(head_node, tcb_t, node);

        if ((int32_t)(g_tick - sleeper->wake_tick) >= 0) 
        {
            list_remove(&sleeper->node);
            sleeper->state = TASK_READY;
            sched_ready(sleeper);

            if (g_current_tcb == NULL || sleeper->prio < g_current_tcb->prio) 
            {
                need_resched = true;
            }
        } 
        else 
        {
        break;
        }
    }


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

    //if (g_current_tcb == NULL) 
    //{
      //  return;
    //}
    //Implement the Round Robin Time slicing for eq pr, ready tasks

    /*if (s_ticks_in_slice >= TIME_SLICE_TICKS) 
    {
        s_ticks_in_slice = 0;


        uint8_t prio = g_current_tcb->prio;
        //if same pr has multiple ready* tasks
        if (prio < SCHED_PRIO_IDLE) 
        {
            sched_rotate(prio);

            tcb_t *next = sched_pick();
            if (next != g_current_tcb) 
             {
                port_yield(); //pendSV reset 
             }
        }
    }*/
}
