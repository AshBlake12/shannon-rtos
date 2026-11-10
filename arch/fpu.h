#ifndef ARCH_FPU_H
#define ARCH_FPU_H

#include <stdint.h>
#include <stdbool.h>

#define FPCCR_ADDR         (0xE000EF34UL)
#define FPCCR_REG          (*(volatile uint32_t *)FPCCR_ADDR)

#define FPCCR_ASPEN        (1UL << 31)
#define FPCCR_LSPEN        (1UL << 30)

void fpu_init(bool enable_lazy_stacking);

#endif 
