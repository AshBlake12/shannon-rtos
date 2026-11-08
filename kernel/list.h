#ifndef KERNEL_LIST_H
#define KERNEL_LIST_H

#include<stdint.h>
#include<stdbool.h>
#include <stddef.h>

/*A doubly linked list basically helps any kernel object that needs queuing*/

typedef struct list_node
{
    struct list_node *prev;
    struct list_node *next;
}list_node_t;

/*Also calculate the parent struct of a member pointer*/

#ifndef container_of
#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))
#endif

static inline void list_init(list_node_t *head)
{
    head->next = head;
    head->prev = head;
}

static inline bool list_is_empty(const list_node_t *head)
{
    return (head->next == head);
}


static inline void list_insert_tail(list_node_t *head, list_node_t *node)
{
    node->next = head;
    node->prev = head->prev;
    head->prev->next = node;
    head->prev = node;
}


static inline void list_insert_head(list_node_t *head, list_node_t *node)
{
    node->next =head->next;
    node->prev = head;
    head->next->prev = node;
    head->next = node;
}

/*smhow we need to clear the pointers of prev node to detect double removes? also relink the neighbours*/ 

static inline void list_remove(list_node_t *node)
{
    node->prev->next = node->next;
    node->next->prev = node->prev;
    node->next = NULL;
    node->prev = NULL;
}

static inline void list_insert_before(list_node_t *pos, list_node_t *node)
{
    node->next = pos;
    node->prev = pos->prev;
    pos->prev->next = node;
    pos->prev = node;
}


static inline list_node_t *list_first(const list_node_t *head)
{
    return list_is_empty(head) ? NULL : head->next;
}

static inline void list_rotate(list_node_t *head)
{
    if (!list_is_empty(head) && (head->next != head->prev)) {
        list_node_t *first = head->next;
        head->next = first->next;
        first->next->prev = head;

        first->next = head;
        first->prev = head->prev;
        head->prev->next = first;
        head->prev = first;
    }
}


#endif




