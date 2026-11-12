#ifndef KERNEL_MUTEX_H
#define KERNEL_MUTEX_H

#include <stdint.h>
#include <stdbool.h>
#include "task.h"
#include "wait.h"
#include "kerr.h"

typedef struct 
{
    tcb_t        *owner;
    wait_queue_t  wq;
    bool          inherit;   // runtime switch for A/B priority inversion demo
} mutex_t;

void   mutex_init(mutex_t *m, bool inherit);

kerr_t mutex_lock(mutex_t *m, uint32_t timeout);
kerr_t mutex_unlock(mutex_t *m);

#endif
