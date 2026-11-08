#include "task.h"
#include "../arch/port.h"
#include "kprintf.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "sched.h"
#include "idle.h"
#include "sched.h"


tcb_t *g_current_tcb = NULL;

void task_exit(void)
{
    kprintf("\n[SHADY KERNEL] Task '%s' exited , halting.\n",  g_current_tcb ? g_current_tcb->name : "unknown");
    
    for (;;) 
    {
        __asm volatile ("wfi");
    }
}

void kernel_start(void)
{
    idle_init();

    g_current_tcb = sched_pick();

    if (g_current_tcb != NULL) 
    {
        g_current_tcb->state = TASK_RUNNING;

        port_start_first_task();
    }

    for (;;) {
        __asm volatile ("wfi");
    }

   /* if (g_current_tcb == NULL)
    {
        kprintf("[SHADY KERNEL] Tasks not registered \n");
        for(;;){
            __asm volatile ("wfi");
        }
    }

    port_start_first_task();*/
}

void kernel_switch(void)
{
        if (g_current_tcb != NULL)
    {
        if (g_current_tcb->state == TASK_RUNNING) 
        {
            g_current_tcb->state = TASK_READY;
        }
    }

    tcb_t *next_tcb = sched_pick();
    if (next_tcb != NULL) 
    {
        next_tcb->state = TASK_RUNNING;
        g_current_tcb = next_tcb;
    }              
    // Priority p 0 =< p =< 31; the mapping is as 31-p.. highest priority is found b 
                    // CLZ(bitmap) = prio ... at a lower abstarction (post port) ARM clz instruct makes sense 

  
}

bool task_canary_check(const tcb_t *tcb)
{
    if (!tcb || !tcb->stack_bottom) 
        return false;
    
    return (tcb->stack_bottom[0] == STACK_CANARY_VAL);
}


void task_create(tcb_t *tcb, 
                 uint32_t *stack, 
                 uint32_t stack_bytes,
                 void (*entry)(void *), 
                 void *arg, 
                 const char *name,
                 uint8_t prio)
{
    tcb->stack_bottom = stack;
    tcb->stack_bytes  = stack_bytes;
    tcb->name         = name;

    tcb->prio         = prio;
    tcb->state        = TASK_READY;
    tcb->wake_tick    = 0;

    stack[0] = STACK_CANARY_VAL;  //Canary at stack bottom 

    uint32_t *stack_top = &stack[stack_bytes / sizeof(uint32_t)];

    tcb->sp = port_stack_init(entry, arg, stack_top, task_exit);  //16 word const frame
    uint32_t status = enter_critical();
    sched_ready(tcb);
    exit_critical(status);
}

// void task_create(tcb_t *tcb,
//                  uint32_t *stack,
//                  uint32_t stack_bytes,
//                  void (*entry)(void *),
//                  void *arg,
//                  const char *name)
// {
//     uint32_t words = stack_bytes / sizeof(uint32_t);
//
//     for (uint32_t i = 0; i < words; i++) {
//         stack[i] = STACK_CANARY_VAL;
//     }
//
//     tcb->stack_bottom = stack;
//     tcb->stack_bytes  = stack_bytes;
//     tcb->name         = name;
//
//     //sp top, enforce 8 byte align on stack top
//     uint32_t *stack_top = (uint32_t*)((uint32_t) (stack + words) & ~7UL);
//
//     //16 word constant frame
//     //
//     tcb->sp = port_stack_init(entry, arg, stack_top, task_exit);
//
//     //Circular Linked list insertion
//     port_critical_enter();
//     if (g_current_tcb == NULL) {
//         g_current_tcb = tcb;
//         tcb->next = tcb;
//     } else {
//         tcb->next = g_current_tcb->next;
//         g_current_tcb->next = tcb;
//     }
//
//     port_critical_exit();
// }

void task_yield(void)
{
    port_yield();
}
