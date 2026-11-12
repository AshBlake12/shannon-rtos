#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

//led + systick stuff, nucleo specific
void board_init(void);
void board_led_on(void);
void board_led_off(void);
void board_led_toggle(void);
void board_delay_ms(uint32_t ms); //busy wait dont use after kernel starts
uint32_t board_get_ms(void);


#endif

