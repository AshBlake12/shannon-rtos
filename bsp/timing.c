#include "timing.h"
#include "../third_party/cmsis/stm32h7a3xxq.h"

void timing_init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->LAR = 0XC5ACCE55UL; //DWT register access on cortex m7 

    DWT->CYCCNT = 0UL;  //reset cycle counter 

    DWT -> CTRL |= DWT_CTRL_CYCCNTENA_Msk;

}


