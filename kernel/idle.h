#ifndef KERNEL_IDLE_H
#define KERNEL_IDLE_H

#include "task.h"

#define IDLE_STACK_WORDS 128

void idle_init(void);
tcb_t *idle_get_tcb(void);

#endif
