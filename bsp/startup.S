    .syntax unified            @ as: ARM Directives
    .cpu cortex-m7
    .thumb

    .section .isr_vector, "a", %progbits
    .align 2
    .global g_pfnVectors

g_pfnVectors:
    .word _estack              
    .word Reset_Handler        
    .word NMI_Handler
    // .word HardFault_Handler    
    //  .word MemManage_Handler    
    // .word BusFault_Handler      
    // .word UsageFault_Handler    
    .word Fault_Entry           /* 3. HArdFault */
    .word Fault_Entry           /*  4: MemManage */
    .word Fault_Entry           /*  5: BusFault */
    .word Fault_Entry           /*  6: UsageFault */
    .word 0                     
    .word 0                     
    .word 0                     
    .word 0                     
    .word SVC_Handler            /*yoink yoink*/ 
    .word DebugMon_Handler      
    .word 0                     
    .word PendSV_Handler        
    .word SysTick_Handler       

    .text
    .global Reset_Handler      
    .type Reset_Handler, %function   
Reset_Handler:
    
    /*Enabling FPU, CPACR IS AT 0xE000ED88 */
    /*dsb is data sync barrier and isb is instruction sync barrier. dsb ensures write is done and 
    isb flushes the pipeline, so instructions after it are fetched again and see the FPU as enabled*/

    ldr r0, =0xE000ED88
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

    .section .text.Default_Handler, "ax", %progbits
Default_Handler:
    b .
    .size Default_Handler, .- Default_Handler

    /*Weak Alias everything*/ 
    /*points to the same memory address or code as another symbol (the target), 
    marked with a weak attribute so that the linker can override it if a strong definition of the same name exists*/

    /*When the processor branches to a function, it determines state to switch to based on the LSB of the target address*/

    .weak NMI_Handler
    .thumb_set NMI_Handler, Default_Handler

  /*  .weak HardFault_Handler
    .thumb_set HardFault_Handler, Default_Handler

    .weak MemManage_Handler
    .thumb_set MemManage_Handler, Default_Handler

    .weak BusFault_Handler
    .thumb_set BusFault_Handler, Default_Handler

    .weak UsageFault_Handler
    .thumb_set UsageFault_Handler, Default_Handler */

    .weak SVC_Handler
    .thumb_set SVC_Handler, Default_Handler

    .weak DebugMon_Handler
    .thumb_set DebugMon_Handler, Default_Handler

    .weak PendSV_Handler
    .thumb_set PendSV_Handler, Default_Handler

    .weak SysTick_Handler
    .thumb_set SysTick_Handler, Default_Handler







