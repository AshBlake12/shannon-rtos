#include "uart.h"
#include "stm32h7xx.h"

extern uint32_t SystemCoreClock;

void uart_init(void)
{
    /* enable clocks for GPIOD (AHB4) and USART3 (APB1L) */
    RCC->AHB4ENR  |= RCC_AHB4ENR_GPIODEN;
    RCC->APB1LENR |= RCC_APB1LENR_USART3EN;
    (void)RCC->AHB4ENR;
    (void)RCC->APB1LENR;

    /* configure PD8 (TX) and PD9 (RX) to alt func mode (10b) */
    GPIOD->MODER &= ~((3UL << (8 * 2)) | (3UL << (9 * 2)));
    GPIOD->MODER |=  ((2UL << (8 * 2)) | (2UL << (9 * 2)));

   //medium speed is enuf for 115200
   GPIOD->OSPEEDR &= ~((3UL << (8 * 2)) | (3UL << (9 * 2)));
   GPIOD->OSPEEDR |=  ((1UL << (8 * 2)) | (1UL << (9 * 2)));

    /* AF7 for pins 8 and 9 in AFR[1] (AFRH) */
    GPIOD->AFR[1] &= ~((0xFUL << ((8 - 8) * 4)) | (0xFUL << ((9 - 8) * 4)));
    GPIOD->AFR[1] |=  ((7UL   << ((8 - 8) * 4)) | (7UL   << ((9 - 8) * 4)));

    /*baud 64/115200 ~ 556 */
    USART3->BRR = (SystemCoreClock + (115200UL / 2UL)) / 115200UL;

    USART3->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

//spin till tx fifo has space
void uart_putc(char c)
{
    while (!(USART3->ISR & USART_ISR_TXE_TXFNF)) {}

    USART3->TDR = (uint8_t)c;
}

char uart_getc(void)
{
    while (!(USART3->ISR & USART_ISR_RXNE_RXFNE)) {}

    return (char)(USART3->RDR & 0xFFU);
}

//\n -> \r\n so putty dosent staircase
void uart_write(const char *s)
{
    while (*s) 
     {
        if (*s == '\n') 
        {
            uart_putc('\r');
        }
        uart_putc(*s++);
    }
}
