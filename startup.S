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
    
    /*Enabling FPU, CPACR IS AT 0xE000ED88 */
    /*dsb is data sync barrier and isb is instruction sync barrier. dsb ensures write is done and 
    isb flushes the pipeline, so instructions after it are fetched again and see the FPU as enabled*/

    ldr r0, = 0xE000ED88
    ldr r1, [r0]
    orr r1, r1, #(0xF << 20)
    str r1, [r0]
    dsb                              
    isb 

    /*now i have to copy .data from FLASH to RAM*/
    ldr r0, = _sidata
    ldr r1, = _sdata
    ldr r2, = _edata 

 .L_copy_data:
    
        cmp r1,r2
        bhs .L_copy_data_done
        ldr r3 , [r0], #4 
        str r3, [r1], #4 
        b .L_copy_data 

 .L_copy_data_done: 

    ldr r1, = _sbss
    ldr r2, = _ebss 
    movs r3, #0 

.L_zero_bss:
    cmp r1,r2
    bhs .L_zero_bss_done     /*Zero'ing .bss*/
    str r3, [r1], #4 
    b .L_zero_bss 
.L_zero_bss_done:
    
    bl main   /*Call C runtime entry point*/

.L_exit_loop:

    b .L_exit_loop

    .size Reset_Handler, .- Reset_Handler 







