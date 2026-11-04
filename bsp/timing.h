#ifndef TIMING_H 
#define TIMING_H 

#include <stdint.h>
#include "stm32h7xx.h"

void timing_init(void);
static inline uint32_t cycles_now(void)
{
    return DWT->CYCCNT;
}

#endif //TIMING_H 

