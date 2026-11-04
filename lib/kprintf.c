#include "kprintf.h"
#include "uart.h"


static void print_unsigned(uint32_t val, uint32_t base, int uppercase)
{
    char buf[32];
    int i = 0;
    const char *digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";

    if (val == 0) {
        uart_putc('0');
        return;
    }

    while (val > 0) {
        buf[i++] = digits[val % base];
        val /= base;
    }

    while (i > 0) {
        uart_putc(buf[--i]);
    }
}


static void print_signed(int32_t val)
{
    if (val < 0) {
        uart_putc('-');
        /* Handle INT32_MIN corner case */
        uint32_t uval = (uint32_t)(-(val + 1)) + 1;
        print_unsigned(uval, 10, 0);
    } else {
        print_unsigned((uint32_t)val, 10, 0);
    }
}




void kprintf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    for (const char *p = fmt; *p != '\0'; p++) {
        if (*p != '%') {
            if (*p == '\n') {
                uart_putc('\r');
            }
            uart_putc(*p);
            continue;
        }

        p++; 

        switch (*p) {
        case 'c': {
            char c = (char)va_arg(args, int);
            uart_putc(c);
            break;
        }
        case 's': {
            const char *str = va_arg(args, const char *);
            if (!str) {
                str = "(null)";
            }
            while (*str) {
                if (*str == '\n') {
                    uart_putc('\r');
                }
                uart_putc(*str++);
            }
            break;
        }
        case 'd': {
            int32_t d = va_arg(args, int32_t);
            print_signed(d);
            break;
        }
        case 'u': {
            uint32_t u = va_arg(args, uint32_t);
            print_unsigned(u, 10, 0);
            break;
        }
        case 'x': {
            uint32_t x = va_arg(args, uint32_t);
            print_unsigned(x, 16, 0);
            break;
        }
        case 'p': {
            uintptr_t ptr = (uintptr_t)va_arg(args, void *);
            uart_write("0x");
            print_unsigned((uint32_t)ptr, 16, 0);
            break;
        }
        case '%': {
            uart_putc('%');
            break;
        }
        default:
            uart_putc('%');
            uart_putc(*p);
            break;
        }
    }

    va_end(args);
}
