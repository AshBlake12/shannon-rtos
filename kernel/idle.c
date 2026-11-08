#include <stdint.h>
#include <stddef.h>
#include "stm32h7xx.h"
#include "idle.h"
#include "sched.h"

static uint32_t s_idle_stack[IDLE_STACK_WORDS];
static tcb_t s_idle_tcb;

static void idle_task_entry(void *arg)
{
    (void)arg;
    for (;;) {
        __WFI(); /*No exits, low power blk*/

    }
}

void idle_init(void)
{
    task_create(&s_idle_tcb,
                s_idle_stack,
                sizeof(s_idle_stack),
                idle_task_entry,
                NULL,
                "idle",
                SCHED_PRIO_IDLE);

}

tcb_t *idle_get_tcb(void)
{
    return &s_idle_tcb;

  }
