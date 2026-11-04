ENTRY(Reset_Handler)

MEMORY
{
  FLASH   (rx)  : ORIGIN = 0x08000000, LENGTH = 2048K
  ITCM    (rwx) : ORIGIN = 0x00000000, LENGTH = 64K
  DTCM    (rw)  : ORIGIN = 0x20000000, LENGTH = 128K
  AXISRAM (rw)  : ORIGIN = 0x24000000, LENGTH = 1024K
}

/*The processor uses a full descending stack. This means that the stack pointer holds the address of the last stacked item, then further decrements (?) the value address*/

_estack = ORIGIN(DTCM) + LENGTH(DTCM);

SECTIONS
{
  .isr_vector : {KEEP(*(.isr_vector))} > FLASH
  .text : 
  {
    *(.text*) 
    *(.rodata*)
    . = ALIGN(4);
  } > FLASH 
  .data :
  {
    . = ALIGN(4);
    _sdata = .;          /* run address (VMA) start */
    *(.data*)
    . = ALIGN(4);
    _edata = .;          /* run address end */ 
  } > DTCM AT> FLASH

  _sidata = LOADADDR(.data);

  .bss :
  {
   . = ALIGN(4);
    _sbss = .;
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
  } > DTCM

}
