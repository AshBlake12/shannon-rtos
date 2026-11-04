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

