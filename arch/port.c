#include "port.h"
#include "stm32h7xx.h"
#include "time.h"


#define SHPR3_REG           (*(volatile uint32_t *)0xE000ED20)
#define PENDSV_PRIO_LOWEST  (0xFFUL << 16)
#define SYSTICK_PRIO_LOWEST (0xFFUL << 24)

void SysTick_Handler(void)
{
    kernel_tick();
}

void port_init(void)
{
    //Set SysTick and PendSV to the lowest interrupt priority (15 on Cortex-M7 with 4 priority bits)
    NVIC_SetPriority(SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
    NVIC_SetPriority(PendSV_IRQn,  (1UL << __NVIC_PRIO_BITS) - 1UL);
    //PendSV and SysTick run ar lowest priority, given here its 0xFF
    SHPR3_REG |= PENDSV_PRIO_LOWEST | SYSTICK_PRIO_LOWEST;
}

uint32_t *port_stack_init(void (*entry)(void *), void *arg, uint32_t *stack_top, void (*exit_handler)(void))
{
    //8 byte stack seg for AAPCS
    uint32_t *sp = (uint32_t *)((uintptr_t)stack_top & ~0x7UL);

    //16 word frame 
    sp -= 16;

    // Core pops this while unstacking 
    // xPSR, PC, LR, R12, R3. R2, R1. R0 
    sp[15] = 0x01000000UL;                    
    sp[14] = (uint32_t)entry & ~1UL;            
    sp[13] = (uint32_t)exit_handler;           
    sp[12] = 0x00000000UL;                      
    sp[11] = 0x00000000UL;                      
    sp[10] = 0x00000000UL;                     
    sp[9]  = 0x00000000UL;                   
    sp[8]  = (uint32_t)arg;                     

    //this pop happens manually using LDMIA
    // R11 - R4
    sp[7]  = 0x00000000UL;                          
    sp[6]  = 0x00000000UL;                       
    sp[5]  = 0x00000000UL;                     
    sp[4]  = 0x00000000UL;                     
    sp[3]  = 0x00000000UL;                       
    sp[2]  = 0x00000000UL;                      
    sp[1]  = 0x00000000UL;                      
    sp[0]  = 0x00000000UL;                      

    return sp;
}

void port_yield(void)
{
    //SCB-> ICSR is a PenSV exception (bit 28)
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
    __asm volatile ("dsb" ::: "memory");
    __asm volatile ("isb" ::: "memory");
}

void port_critical_enter(void)
{
    __asm volatile ("cpsid i" ::: "memory");
}

void port_critical_exit(void)
{
    __asm volatile ("cpsie i" ::: "memory");
}


//BASERPI masks only interrupts at or below a configurable kernel threshold, leaving hard real-time interrupts untouched.
uint32_t enter_critical(void)
{
    uint32_t prev_basepri = __get_BASEPRI();
    __set_BASEPRI(KERNEL_PRIO_THRESHOLD);
    __DSB();
    __ISB();
    return prev_basepri;
}

void exit_critical(uint32_t prev_basepri)
{
    __set_BASEPRI(prev_basepri);
    __DSB();
    __ISB();
}
