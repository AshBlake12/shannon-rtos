#include "stm32h7a3xxq.h"
#include <stdint.h>

void tim6_init(uint16_t psc, uint16_t arr)
{
    // enable tim6 clock on apb1 bus
    RCC->APB1LENR |= RCC_APB1LENR_TIM6EN;
    __DSB();

    TIM6->CR1 = 0;
    TIM6->PSC = psc;
    TIM6->ARR = arr;
    TIM6->EGR = TIM_EGR_UG;
    TIM6->SR  = 0;

    // enable update interrupt
    TIM6->DIER |= TIM_DIER_UIE;

    // priority 6 is below KERNEL_PRIO_THRESHOLD (5), so it can call kernel apis safely
    NVIC_SetPriority(TIM6_DAC_IRQn, 6);
    NVIC_EnableIRQ(TIM6_DAC_IRQn);

    // fire up the counter
    TIM6->CR1 |= TIM_CR1_CEN;
}
