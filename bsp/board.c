#include"board.h"
#include"stm32h7xx.h"

#define LED 0

uint32_t g_init = 0xCAFEBABE;   // goes to .data
uint32_t g_zero;                // goes to .bss
volatile float g_f = 1.5f;      // .data, and forces real FPU instructions

volatile uint32_t g_ms = 0;

void SysTick_Handler(void)
{
    g_ms++;
}

uint32_t board_get_ms(void)
{
    return g_ms;
}

void board_init(void)
{
    RCC->AHB4ENR |= RCC_AHB4ENR_GPIOBEN;  //enable gpiob clk in ahb4 
    (void)RCC->AHB4ENR;

    GPIOB->MODER &= ~(0x3UL << (LED*2) );
    GPIOB->MODER |= (0x1UL << (LED*2) );

    SysTick->LOAD = 64000UL - 1UL;
    SysTick->VAL= 0UL;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;

}

void board_led_on(void)
{
    GPIOB->BSRR = (1UL << LED);
}

void board_led_off(void)
{
    GPIOB->BSRR = (1UL << (LED + 16));
}

void board_led_toggle(void)
{
    if (GPIOB->ODR & (1UL << LED)) {
        board_led_off();
    } else {
        board_led_on();
    }
}

void board_delay_ms(uint32_t ms)
{
    uint32_t start = g_ms;
    while ((g_ms - start) < ms) {
    }
}


//int main(void)
//{
  //  RCC_AHB4ENR  |= RCC_AHB4ENR_GPIOBEN;

   // volatile uint32_t dummy = RCC_AHB4ENR;
    //(void)dummy;

    //GPIOB_MODER &= ~(0x3UL << (LED*2) );
    //GPIOB_MODER |= (0x1UL << (LED*2) );

    //volatile float x = g_f * 2.0f;
    //volatile uint32_t a = g_init;
    //volatile uint32_t b = g_zero;
    //(void)x;
    //(void)a;
    //(void)b;
   
    //systick_init();


    //for (;;) { 
     //GPIOB_BSRR = (1UL << LED);
    //delay_ms(500);
       // for(volatile uint32_t i=0; i<100000; i++){ 

        //}

    //GPIOB_BSRR = (1UL << (LED + 16));
    //delay_ms(500);
          //  for(volatile uint32_t i=0; i<100000; i++){ 
        // }

    //}

    //return 0;
//}

