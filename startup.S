    .syntax unified            @ as: ARM Directives
    .cpu cortex-m7
    .thumb

    .section .isr_vector, "a", %progbits   @ as: .section; name matches KEEP in ld
    .word _estack              @ DUI0646 Vector table: word 0 = initial SP
    .word Reset_Handler        @ word 1 = reset vector (needs Thumb bit)

    .text
    .global Reset_Handler      @ visible to the linker
    .type Reset_Handler, %function   @ marks Thumb function → bit 0 set
Reset_Handler:
    b .                        @ branch to self
