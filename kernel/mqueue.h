#ifndef KERNEL_MQUEUE_H
#define KERNEL_MQUEUE_H

#include <stdint.h>
#include <stddef.h>
#include "semaphore.h"
#include "kerr.h"

//fixed size msg ring buffer, 2 sems for full/empty (classic producer consumer)
typedef struct {
    uint8_t   *buffer;
    size_t     msg_size;
    size_t     capacity;
    size_t     head;
    size_t     tail;
    size_t     count;
    sem_t      items_sem;
    sem_t      spaces_sem;
} mqueue_t;

void   mqueue_init(mqueue_t *mq, void *storage, size_t msg_size, size_t capacity);
kerr_t mqueue_send(mqueue_t *mq, const void *msg, uint32_t timeout);
kerr_t mqueue_recv(mqueue_t *mq, void *msg, uint32_t timeout);

#endif
