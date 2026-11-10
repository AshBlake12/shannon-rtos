#include "fpu.h"
#include <stdbool.h>

void fpu_init(bool enable_lazy_stacking)
{
    // Enable CP10 and CP11 FULL ACCESS in CPACR 

    volatile uint32_t *cpacr = (volatile uint32_t *)0xE000ED88UL;
    *cpacr |= ((3UL << 20) | (3UL << 22));
    __asm volatile ("dsb\n\tisb");

    uint32_t fpccr = FPCCR_REG;

    // ASPEN = 1, LSPEN = 0 
    //Then, ASPEN = 1 , LSPEN = 1 

    fpccr |= FPCCR_ASPEN;
    if (enable_lazy_stacking) 
    {
        fpccr |= FPCCR_LSPEN;
    } 
    else 
    {
        fpccr &= ~FPCCR_LSPEN;
    }

    FPCCR_REG = fpccr;

    __asm volatile ("dsb\n\tisb");
}
