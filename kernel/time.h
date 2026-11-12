#ifndef KERNEL_TIME_H
#define KERNEL_TIME_H

#include <stdint.h>
#include <stdbool.h>
#include "task.h"

#define TIME_SLICE_TICKS 5 //5ms for eq priority task

extern volatile uint32_t g_tick;

void time_init(void);
void kernel_tick(void);
void task_sleep(uint32_t ticks);
void time_slice_reset(void);
void time_delay_insert(tcb_t *tcb, uint32_t ticks);
void time_delay_remove(tcb_t *tcb);


#endif
