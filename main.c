#include <stdint.h> 
#include "board.h"
#include "uart.h"
#include "kprintf.h"
#include "timing.h"
#include "fault.h"
#include "stm32h7xx.h"

#define FAULT_TEST 6


static void run_fault_test(int test)
{
    kprintf("fault test %d \n", test);

    switch(test)
        {
        case 1: 
            { //div by 0 test 
                volatile int z =0;
                volatile int x = 1/z;
                (void)x;
            break;
            }   

        case 2:
            { //undefined instruction 
                __asm volatile ("udf #0");
            break;
            }

        case 3:
            { //bad thumb state 
                void (*bad_func)(void) = (void (*)(void))((uintptr_t)board_led_on & ~1UL);
                bad_func();
            break;
            }

        case 4: 
            {// Unmapped FMC space; precise bus error
                volatile uint32_t *unmapped = (volatile uint32_t *)0x60000000UL;
                volatile uint32_t val = *unmapped;
                (void)val;
            break;
            }

        case 5: 
            {// Imprecise bus error, buffered write to unmapped FMC space 
                volatile uint32_t *unmapped = (volatile uint32_t *)0x60000000UL;
                *unmapped = 0xDEADBEEFUL;
            break;
            }
            
        case 6: 
            { // Escalation-> clear usage fault enable and trigger div by 0
                SCB->SHCSR &= ~SCB_SHCSR_USGFAULTENA_Msk;
                volatile int z = 0;
                volatile int x = 1 / z;
                (void)x;
            break;
            }

            default:
            kprintf("select test bbg\n");
            break;
        }
}

int main(void)
{
    board_init();
    uart_init();
    timing_init();
    fault_init();

    kprintf("\nNow that i have this cute print, ill print anything; this shows sys boot :D\n");

    //  overhead measure 
    uint32_t t0 = cycles_now();
    uint32_t t1 = cycles_now();
    uint32_t overhead = t1 - t0;
    kprintf("overhead: %u cycles\n", overhead);

    //measure 10000ms delay
    uint32_t start = cycles_now();
    board_delay_ms(1000);
    uint32_t elapsed = (cycles_now() - start) - overhead;
    kprintf("1000 ms delay = %u cycles (Expected ~64,000,000)\n", elapsed);

#if FAULT_TEST != 0
    run_fault_test(FAULT_TEST);
#endif

    for (;;) {
        board_led_on();
        board_delay_ms(500);
        board_led_off();
        board_delay_ms(500);
    }

    return 0;
}


//#include "regs.h"
//
// #define LED 0
//
// uint32_t g_init = 0xCAFEBABE;   // goes to .data
// uint32_t g_zero;                // goes to .bss
// volatile float g_f = 1.5f;      // .data, and forces real FPU instructions
//
// volatile uint32_t g_ms = 0;
//
// void SysTick_Handler(void)
// {
//     g_ms++;
// }
//
// static void systick_init(void)
// {
//     SYSTICK_LOAD = 64000UL - 1UL;
//     SYSTICK_VAL= 0UL;
//     SYSTICK_CTRL = SYSTICK_CTRL_CLKSOURCE | SYSTICK_CTRL_TICKINT | SYSTICK_CTRL_ENABLE;
// }
//
// static void delay_ms(uint32_t ms)
// {
//     uint32_t start = g_ms;
//     while ((g_ms - start) < ms){
//
//     }
// }

//int main(void)
//{
  //  RCC_AHB4ENR  |= RCC_AHB4ENR_GPIOBEN;

    //volatile uint32_t dummy = RCC_AHB4ENR;
    //(void)dummy;

    //GPIOB_MODER &= ~(0x3UL << (LED*2) );
    //GPIOB_MODER |= (0x1UL << (LED*2) );

       
    //systick_init();


    //for (;;) { 
     //GPIOB_BSRR = (1UL << LED);
    //delay_ms(500);
       // for(volatile uint32_t i=0; i<100000; i++){ 

        //}

   // GPIOB_BSRR = (1UL << (LED + 16));
    //delay_ms(500);
          //  for(volatile uint32_t i=0; i<100000; i++){ 
        // }

    //}

    //return 0;
//}

/*int main(void)
{
    board_init();
    uart_init();

    kprintf("hello world from shannon :p\n");
    kprintf("%x %d %u %s\n", 0xDEADBEEF, -42, 4000000000u, "ok");

    for (;;) {
        board_led_on();
        board_delay_ms(500);

        board_led_off();
        board_delay_ms(500);

        kprintf("current tick is %u ms\n", board_get_ms());
    }

    return 0;
}
*/
