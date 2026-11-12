#ifndef KERNEL_KERR_H
#define KERNEL_KERR_H

#include <stdint.h>

typedef int32_t kerr_t;

// kernel error and status codes
#define K_OK              0
#define K_TIMEOUT        -1
#define K_WOULD_BLOCK    -2
#define K_ERR_ISR        -3
#define K_ERR_NOT_OWNER  -4
#define K_ERR_PARAM      -5
#define K_ERR_OVERFLOW   -6
#define K_ERR_STATE      -7

// wait timeout constants
#define K_NO_WAIT         0UL
#define K_FOREVER         0xFFFFFFFFUL

// 2^31 - 1 limit so signed delta math in tick comparisons doesnt wrap and lie
#define K_MAX_TIMEOUT     0x7FFFFFFFUL

#endif 
