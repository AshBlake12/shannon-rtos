#include <stdint.h> 

uint32_t g_init = 0xCAFEBABE;   // goes to .data
uint32_t g_zero;                // goes to .bss
volatile float g_f = 1.5f;      // .data, and forces real FPU instructions

int main(void)
{
    volatile float x = g_f * 2.0f;
    volatile uint32_t a = g_init;
    volatile uint32_t b = g_zero;
    (void)x;
    (void)a;
    (void)b;

    for (;;) { }
}

