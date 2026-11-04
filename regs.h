#include <stdint.h>
#define REG32(addr) (*(volatile uint32_t *)(addr))

/*RCC*/
#define RCC_BASE               0x58024400UL
#define RCC_AHB4ENR            REG32(RCC_BASE + 0x0E0UL)
#define RCC_AHB4ENR_GPIOBEN    (1UL << 1)

/*GPIOB*/
#define GPIOB_BASE             0x58020400UL
#define GPIOB_MODER            REG32(GPIOB_BASE + 0x00UL)
#define GPIOB_ODR              REG32(GPIOB_BASE + 0x14UL)
#define GPIOB_BSRR             REG32(GPIOB_BASE + 0x18UL)

/*Systick*/ 
#define SYSTICK_BASE           0xE000E010UL
#define SYSTICK_CTRL           REG32(SYSTICK_BASE + 0x00UL)
#define SYSTICK_LOAD           REG32(SYSTICK_BASE + 0x04UL)
#define SYSTICK_VAL            REG32(SYSTICK_BASE + 0x08UL)
#define SYSTICK_CALIB          REG32(SYSTICK_BASE + 0x0CUL)

#define SYSTICK_CTRL_ENABLE    (1UL << 0)
#define SYSTICK_CTRL_TICKINT   (1UL << 1)
#define SYSTICK_CTRL_CLKSOURCE (1UL << 2)
