#ifndef KPRINTF_H
#define KPRINTF_H

#include <stdint.h>
#include <stdarg.h>

void kprintf(const char *fmt, ...);

void kprintf(const char *fmt, ...);

//call after sched_init, before tasks start printing
void kprintf_init_lock(void);

#endif
