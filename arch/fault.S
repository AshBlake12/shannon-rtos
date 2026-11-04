.syntax unified
.cpu cortex-m7
.thumb

.text 
.global Fault_Entry
.type Fault_Entry, %function

Fault_Entry:

/* We check the 2nd bit of LR corresponding to EXEC_Return*/

tst lr, #4
ite eq
mrseq r0, msp 
mrsne r0, psp 

mov r1,lr

b fault_report

.size Fault_Entry, . -Fault_Entry
