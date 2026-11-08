#ifndef KERNEL_SCHED_H
#define KERNEL_SCHED_H

#include <stdint.h>
#include <stdbool.h>
#include "list.h"

#define SCHED_PRIO_LEVELS 32
#define SCHED_PRIO_HIGHEST 0
#define SCHED_PRIO_LOWEST  31
#define SCHED_PRIO_IDLE    31

struct tcb;

void sched_init(void);
void sched_ready(struct tcb *tcb);
void sched_unready(struct tcb *tcb);
struct tcb *sched_pick(void);

void sched_rotate(uint8_t prio);
uint32_t sched_get_ready_bitmap(void);

#endif
