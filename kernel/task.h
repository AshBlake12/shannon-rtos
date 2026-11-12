#ifndef TASK_H
#define TASK_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "list.h"
#include "kerr.h"

#define STACK_CANARY_VAL 0XDEADBEEFUL

//typedef struct tcb//sp. circ linked list ptr, base address of stack buffer, size, debug name 
//{
//    uint32_t *sp;
//    struct tcb *next;
//    uint32_t *stack_bottom;
//    uint32_t stack_bytes;
//    const char *name;
//} tcb_t;

typedef enum {
    TASK_RUNNING,
    TASK_READY,
    TASK_BLOCKED,
    TASK_SUSPENDED
} task_state_t;

typedef struct tcb {
    uint32_t       *sp;
    uint32_t       *stack_bottom;
    uint32_t        stack_bytes;
    const char     *name;
    uint8_t         prio;
    uint8_t         base_prio;
    task_state_t    state;
    list_node_t     node;
    list_node_t     event_node;    // SynC wait queue node 
    kerr_t          wait_result;   // Result of the wait: K_OK, K_TIMEOUT, ETC ETC 
    void           *blocked_on;    // Pointer to object waiting on (for priority inheritance
    uint32_t        wake_tick;
    bool            in_delay_list;
} tcb_t;

extern tcb_t *g_current_tcb;

//bool task_canary_check(const tcb_t *tcb);

//void task_create(
//    tcb_t *tcb,
//    uint32_t *stack,
//    uint32_t stack_bytes,
//    void (*entry)(void*),
//    void *arg,
//    const char *name
//);

typedef void (*task_entry_t)(void *arg);

void task_create(           //new def
    tcb_t *tcb, 
    uint32_t *stack, 
    uint32_t stack_bytes,
    task_entry_t entry, 
    void *arg, 
    const char *name,
    uint8_t prio
);

void task_yield(void);
void task_exit(void);
//void kernel_switch(void);
void kernel_start(void);
bool task_canary_check(const tcb_t *tcb);
size_t task_stack_watermark(const tcb_t *tcb);
void task_suspend(tcb_t *tcb);

#endif 




