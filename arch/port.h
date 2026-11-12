#ifndef PORT_H 
#define PORT_H

#include <stdint.h>
#include "port_defs.h"
#include <stdbool.h>

#include "cmsis_gcc.h"


//set once first task launches, kprintf checks this b4 taking mutex
extern volatile bool g_kernel_running;
uint32_t port_cycle_count(void);

void port_init(void);
uint32_t *port_stack_init(void (*entry)(void*), void *arg, uint32_t *stack_top, void (*exit_handler)(void));
void port_start_tick(void);
void port_start_first_task(void);
void port_yield(void);
void port_critical_enter(void);
void port_critical_exit(void);

//ipsr != 0 means were in handler mode
static inline bool port_in_isr(void)
{
    return (__get_IPSR() != 0);
}

//basepri based, returns old basepri so it nests properly
uint32_t enter_critical(void);
void exit_critical(uint32_t prev_basepri);



#endif 

