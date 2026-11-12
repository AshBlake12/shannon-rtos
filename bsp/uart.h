#ifndef UART_H
#define UART_H

//usart3 on PD8/PD9 , goes to stlink vcp
void uart_init(void);
void uart_putc(char c);
void uart_write(const char *s);
char uart_getc(void); //blocking, polls rxne

#endif 


