#include "timing.h"
#include "../third_party/cmsis/stm32h7a3xxq.h"

void timing_init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->LAR = 0XC5ACCE55UL; //DWT register access on cortex m7 

    DWT->CYCCNT = 0UL;  //reset cycle counter 

    DWT -> CTRL |= DWT_CTRL_CYCCNTENA_Msk;

}
void timing_dwt_init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
#if defined(DWT_LAR_KEY) || defined(CMSIS_DWT_LAR)
    DWT->LAR = 0xC5ACCE55;                                              /* unlock software lock access reg*/
#else
    *(volatile uint32_t *)0xE0001FB0 = 0xC5ACCE55;
#endif
    
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}


uint32_t timing_get_cycles(void)
{
    return DWT->CYCCNT;
}
