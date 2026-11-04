//#include <stdint.h> 
#include "board.h"
#include "uart.h"
#include "kprintf.h"

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

int main(void)
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

