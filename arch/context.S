#include "port_defs.h"

.syntax unified
.cpu cortex-m7
.thumb

.global SVC_Handler  
/*Supervisor call...
Since task_a has never run before, we trigger an svc #0 to force the CPU into exception mode, 
set up PSP to point to task_a's fake frame, and execute bx lr. 
The CPU gotta do the unstacking in hardware and jumps straight to task_a*/
.global PendSV_Handler
/*Cooperative Context Switching with PendSV; PendSV is Cooperative Context Switching with PendSV*/


.global port_start_first_task
.extern g_current_tcb
.extern kernel_switch

.text 
.align 2

/*Equivalent of void port_start_first task*/

.thumb_func
port_start_first_task:
cpsie i         /*change processor state, interrupt enable; i enables IRQ*/ 
svc #0         /*call supervisor with req 0*/
nop            /*no operation*/
bx lr 


.thumb_func
SVC_Handler:
/*First we should fetch the SP of running task*/
ldr r0, =g_current_tcb
ldr r1, [r0]
ldr r0, [r1]

/*Pop R4-R11; from the above set r0 points to R4*/
ldmia r0!, {r4-r11}     /*Increment r0 by 8 words*/

/*Now load this pointer PSP*/
msr psp, r0

/*when in thread mode use psp from now on*/
/*CONTROL reg bit 1 is SPSEL [0->MSP] [1->PSP]*/
mrs r0, control
orr r0, r0, #2
msr control, r0
isb 

/*exception return xFFFFFFFD returns the threadmode in PSP; this basically resets after series of pops thru R4-R11; then repointing to task_A*/
ldr lr, =0xFFFFFFFD
bx lr 

.thumb_func
PendSV_Handler:


mov     r0, #KERNEL_PRIO_THRESHOLD
msr     basepri, r0
isb
/*cpsid i*/ /*Disable interrupts*/ 

mrs r0, psp 
stmdb r0!, {r4-r11}

ldr r1, =g_current_tcb
ldr r2, [r1]
str r0, [r2]

push {r4, lr}   /*Preserve EXEC_RETURN*/

/*stack must be 8-byte aligned before calling any C function, why even ;(  Pushing a single 32-bit register misaligns MSP by 4 bytes :/*/


bl kernel_switch

pop {r4, lr}  /*Restore EXC_RETURN*/

/*we need to restore incoming task context*/ 
ldr r1, =g_current_tcb
ldr r2, [r1]
ldr r0, [r2]


/*Pop R4-R11; from the above set r0 points to R4*/
ldmia r0!, {r4-r11}     /*Increment r0 by 8 words*/

/*Now load this pointer PSP*/
msr psp, r0

mov     r0, #0
msr     basepri, r0
isb

/* cpsie i */

bx lr 


