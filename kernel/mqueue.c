#include "mqueue.h"
#include "port.h"
#include <string.h>

void mqueue_init(mqueue_t *mq, void *storage, size_t msg_size, size_t capacity)
{
    mq->buffer   = (uint8_t *)storage;
    mq->msg_size = msg_size;
    mq->capacity = capacity;
    mq->head     = 0;
    mq->tail     = 0;
    mq->count    = 0;

    // items starts empty, spaces starts full
    semaphore_init(&mq->items_sem, 0, (uint32_t)capacity);
    semaphore_init(&mq->spaces_sem, (uint32_t)capacity, (uint32_t)capacity);
}

kerr_t mqueue_send(mqueue_t *mq, const void *msg, uint32_t timeout)
{
    if (mq == NULL || msg == NULL) 
    {
        return K_ERR_PARAM;
    }
    // isr cant wait, only K_NO_WAIT allowed there
    if (port_in_isr() && timeout != K_NO_WAIT) 
    {
        return K_ERR_ISR;
    }

    kerr_t res = semaphore_take(&mq->spaces_sem, timeout);
    if (res != K_OK) 
    {
        return res;
    }

    uint32_t key = enter_critical();
    //copy under lock so head/tail dont get corupted
    memcpy(mq->buffer + (mq->tail * mq->msg_size), msg, mq->msg_size);
    mq->tail = (mq->tail + 1U) % mq->capacity;
    mq->count++;
    exit_critical(key);

    semaphore_give(&mq->items_sem); //wake reciever
    return K_OK;
}

kerr_t mqueue_recv(mqueue_t *mq, void *msg, uint32_t timeout)
{
    if (mq == NULL || msg == NULL) 
    {
        return K_ERR_PARAM;
    }
    if (port_in_isr() && timeout != K_NO_WAIT) 
    {
        return K_ERR_ISR;
    }

    kerr_t res = semaphore_take(&mq->items_sem, timeout);
    if (res != K_OK) 
    {
        return res;
    }

    uint32_t key = enter_critical();
    memcpy(msg, mq->buffer + (mq->head * mq->msg_size), mq->msg_size);
    mq->head = (mq->head + 1U) % mq->capacity;
    mq->count--;
    exit_critical(key);

    semaphore_give(&mq->spaces_sem);
    return K_OK;
}
