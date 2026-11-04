#include "fault.h"
#include "../lib/kprintf.h"
#include "../bsp/board.h"
#include "../third_party/cmsis/stm32h7a3xxq.h"
#include <stdint.h>

void fault_init(void)
{
    //mem manage, usage fault and busfaulrt 
    SCB -> SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_USGFAULTENA_Msk;                     //System Handler Control and State Register
    
    SCB->CCR |= SCB_CCR_DIV_0_TRP_Msk;
}

static void fault_busy_delay(volatile uint32_t count)
{
    while(count--)
    {
        __asm volatile("");
    }
}

void fault_report(uint32_t *frame, uint32_t exec_return)
{
    uint32_t r0   = frame[0];
    uint32_t r1   = frame[1];
    uint32_t r2   = frame[2];
    uint32_t r3   = frame[3];
    uint32_t r12  = frame[4];
    uint32_t lr   = frame[5];
    uint32_t pc   = frame[6];
    uint32_t xpsr = frame[7];

    uint32_t cfsr  = SCB->CFSR;
    uint32_t hfsr  = SCB->HFSR;         //the 4 fault exceptions we are trying 
    uint32_t mmfar = SCB->MMFAR;
    uint32_t bfar  = SCB->BFAR;

    uint16_t ufsr  = (cfsr >> 16) & 0xFFFF;
    uint8_t  bfsr  = (cfsr >> 8)  & 0xFF;
    uint8_t  mmfsr = cfsr & 0xFF;


kprintf("Fault occured bbg");
kprintf("Stack: %s  EXC_RETURN: 0x%x\n", (exec_return & (1 << 2)) ? "PSP" : "MSP", exec_return);
    kprintf("Stacked Frame: 0x%p\n", frame);
    kprintf("  R0   = 0x%x\n", r0);
    kprintf("  R1   = 0x%x\n", r1);
    kprintf("  R2   = 0x%x\n", r2);
    kprintf("  R3   = 0x%x\n", r3);
    kprintf("  R12  = 0x%x\n", r12);
    kprintf("  LR   = 0x%x\n", lr);
    kprintf("  PC   = 0x%x\n", pc);
    kprintf("  xPSR = 0x%x\n", xpsr);

    kprintf("Regs:\n");
    kprintf("  CFSR = 0x%x (UFSR: 0x%x, BFSR: 0x%x, MMFSR: 0x%x)\n", cfsr, ufsr, bfsr, mmfsr);
    kprintf("  HFSR = 0x%x\n", hfsr);

//decode hfsr
if (hfsr & SCB_HFSR_FORCED_Msk)   kprintf("  [HFSR] FORCED (Escalated Fault)\n");
if (hfsr & SCB_HFSR_VECTTBL_Msk)  kprintf("  [HFSR] VECTTBL\n");

//decode ufsr
if (ufsr & (1 << 0))  kprintf("  [UFSR] undefined instruction\n");
    if (ufsr & (1 << 1))  kprintf("  [UFSR] invalid thubstate\n");
    if (ufsr & (1 << 2))  kprintf("  [UFSR] invalid exec_return\n");
    if (ufsr & (1 << 3))  kprintf("  [UFSR] FPU skissue (attempt to use coprocessor)\n");
    if (ufsr & (1 << 8))  kprintf("  [UFSR] unaligned mem access\n");
    if (ufsr & (1 << 9))  kprintf("  [UFSR] div by 0 \n");

//decode bfsr 
    if (bfsr & (1 << 0))  kprintf("  [BFSR] instruction bus error\n");
    if (bfsr & (1 << 1))  kprintf("  [BFSR] exact data bus error\n");
    if (bfsr & (1 << 2))  kprintf("  [BFSR] inexact/buffered data bus error)\n");
    if (bfsr & (1 << 3))  kprintf("  [BFSR]  unstacking error\n");
    if (bfsr & (1 << 4))  kprintf("  [BFSR] stcking error on entry\n");
    if (bfsr & (1 << 7)) 
    {   
        kprintf("  [BFSR] BFARVALID (Fault Address = 0x%x)\n", bfar);
    }

//decode mmfsr 
if (mmfsr & (1 << 0)) kprintf("  [MMFSR] Instruction access violation\n");
    if (mmfsr & (1 << 1)) kprintf("  [MMFSR] data access no no)\n");
    if (mmfsr & (1 << 3)) kprintf("  [MMFSR] blame the MPU on destack \n");
    if (mmfsr & (1 << 4)) kprintf("  [MMFSR] blame the MPU on stack)\n");
    if (mmfsr & (1 << 7)) {
        kprintf("  [MMFSR] MMARVALID (Fault Address = 0x%x)\n", mmfar);
    }

for(;;){
    board_led_toggle();
    fault_busy_delay(15000000UL);
}

}
