#include <stdint.h> 
#include "regs.h"

#define LED 0

uint32_t g_init = 0xCAFEBABE;   // goes to .data
uint32_t g_zero;                // goes to .bss
volatile float g_f = 1.5f;      // .data, and forces real FPU instructions

int main(void)
{
    RCC_AHB4ENR  |= RCC_AHB4ENR_GPIOBEN;

    volatile uint32_t dummy = RCC_AHB4ENR;
    (void)dummy;

    GPIOB_MODER &= ~(0x3UL << (LED*2) );
    GPIOB_MODER |= (0x1UL << (LED*2) );

    //volatile float x = g_f * 2.0f;
    //volatile uint32_t a = g_init;
    //volatile uint32_t b = g_zero;
    //(void)x;
    //(void)a;
    //(void)b;

    for (;;) { 
     GPIOB_BSRR = (1UL << LED);

        for(volatile uint32_t i=0; i<100000; i++){ 

        }

    GPIOB_BSRR = (1UL << (LED + 16));

            for(volatile uint32_t i=0; i<100000; i++){ 
        }

    }

    return 0;
}

