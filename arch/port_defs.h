#ifndef ARCH_PORT_DEFS_H
#define ARCH_PORT_DEFS_H

/* M7 implements 4 priority bits [7:4] in NVIC / BASEPRI.
 Setting BASEPRI = 0x50 masks all interrupt priorities >= 5 (i.e., 5 to 15).
 Hard real-time ISRs with priorities 0 to 4 can still preempt the kernel.
 Any ISR configured with priority 0-4 MUST NEVER call any Shannon-RTOS API!
 */

#define KERNEL_PRIO_THRESHOLD       0x50

#endif 
