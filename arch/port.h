#ifndef PORT_H 
#define PORT_H

#include <stdint.h>

//M7 has 4 priority bits (__NVIC_PRIO_BITS = 4), shifted into the top 4 bits of the 8-bit register.
#define KERNEL_PRIO_THRESHOLD  (5UL << (8 - __NVIC_PRIO_BITS))
//0-5 will preempt the kernel 
//5-15 shall be masked

uint32_t enter_critical(void);
void exit_critical(uint32_t prev_basepri);


void port_init(void);
uint32_t *port_stack_init(void (*entry)(void*), void *arg, uint32_t *stack_top, void (*exit_handler)(void));
void port_start_first_task(void);
void port_yield(void);
void port_critical_enter(void);
void port_critical_exit(void);

#endif 

