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


    .word Default_Handler       /* IRQ 0: WWDG */
    .word Default_Handler       /* IRQ 1: PVD_PVM */
    .word Default_Handler       /* IRQ 2: RTC */
    .word Default_Handler       /* IRQ 3: RTC_S */
    .word Default_Handler       /* IRQ 4: TAMP_STAMP */
    .word Default_Handler       /* IRQ 5: RAMCFG */
    .word Default_Handler       /* IRQ 6: FLASH */
    .word Default_Handler       /* IRQ 7: RCC */
    .word Default_Handler       /* IRQ 8: EXTI0 */
    .word Default_Handler       /* IRQ 9: EXTI1 */
    .word Default_Handler       /* IRQ 10: EXTI2 */
    .word Default_Handler       /* IRQ 11: EXTI3 */
    .word Default_Handler       /* IRQ 12: EXTI4 */
    .word Default_Handler       /* IRQ 13: DMA1_Stream0 */
    .word Default_Handler       /* IRQ 14: DMA1_Stream1 */
    .word Default_Handler       /* IRQ 15: DMA1_Stream2 */
    .word Default_Handler       /* IRQ 16: DMA1_Stream3 */
    .word Default_Handler       /* IRQ 17: DMA1_Stream4 */
    .word Default_Handler       /* IRQ 18: DMA1_Stream5 */
    .word Default_Handler       /* IRQ 19: DMA1_Stream6 */
    .word Default_Handler       /* IRQ 20: ADC1_2 */
    .word Default_Handler       /* IRQ 21: FDCAN1_IT0 */
    .word Default_Handler       /* IRQ 22: FDCAN2_IT0 */
    .word Default_Handler       /* IRQ 23: FDCAN1_IT1 */
    .word Default_Handler       /* IRQ 24: FDCAN2_IT1 */
    .word Default_Handler       /* IRQ 25: EXTI9_5 */
    .word Default_Handler       /* IRQ 26: TIM1_BRK */
    .word Default_Handler       /* IRQ 27: TIM1_UP */
    .word Default_Handler       /* IRQ 28: TIM1_TRG_COM */
    .word Default_Handler       /* IRQ 29: TIM1_CC */
    .word Default_Handler       /* IRQ 30: TIM2 */
    .word Default_Handler       /* IRQ 31: TIM3 */
    .word Default_Handler       /* IRQ 32: TIM4 */
    .word Default_Handler       /* IRQ 33: I2C1_EV */
    .word Default_Handler       /* IRQ 34: I2C1_ER */
    .word Default_Handler       /* IRQ 35: I2C2_EV */
    .word Default_Handler       /* IRQ 36: I2C2_ER */
    .word Default_Handler       /* IRQ 37: SPI1 */
    .word Default_Handler       /* IRQ 38: SPI2 */
    .word Default_Handler       /* IRQ 39: USART1 */
    .word Default_Handler       /* IRQ 40: USART2 */
    .word Default_Handler       /* IRQ 41: USART3 */
    .word Default_Handler       /* IRQ 42: EXTI15_10 */
    .word Default_Handler       /* IRQ 43: RTC_Alarm */
    .word Default_Handler       /* IRQ 44: Reserved */
    .word Default_Handler       /* IRQ 45: TIM8_BRK_TIM12 */
    .word Default_Handler       /* IRQ 46: TIM8_UP_TIM13 */
    .word Default_Handler       /* IRQ 47: TIM8_TRG_COM_TIM14 */
    .word Default_Handler       /* IRQ 48: TIM8_CC */
    .word Default_Handler       /* IRQ 49: DMA1_Stream7 */
    .word Default_Handler       /* IRQ 50: FMC */
    .word Default_Handler       /* IRQ 51: SDMMC1 */
    .word Default_Handler       /* IRQ 52: TIM5 */
    .word Default_Handler       /* IRQ 53: SPI3 */
    .word TIM6_DAC_IRQHandler   /* IRQ 54: TIM6 and DAC1&2 */

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

    .weak TIM6_DAC_IRQHandler
    .thumb_set TIM6_DAC_IRQHandler, Default_Handler





