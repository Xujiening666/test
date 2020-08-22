/*
*****************************************************************************
**

**  File        : LinkerScript.ld
**
**  Abstract    : Linker script for SL28xx Device with
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : Smartlink Technology SL28
**
**
**  Distribution: The file is distributed as is, without any warranty
**                of any kind.
**
**  (c)Copyright mName.
**  You may use this file as-is or modify it according to the needs of your
**  project. Distribution of this file (unmodified or modified) is not
**  permitted.
**
*****************************************************************************
*/
/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 *   _estack
 */
/* Entry Point */
ENTRY(Reset_Handler)

/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x0000;      /* required amount of heap  */
_Min_Stack_Size = 0x0000; /* required amount of stack */

_data_mappping_rom_ = 0x0004A800;


/* Specify the memory areas */
MEMORY
{
    PMU_STARTUP_CONFIG(xr):      ORIGIN = 0x00000000, LENGTH = 1K
}

/* Define output sections */
SECTIONS
{
    .PMU_STARTUP_CONFIG_sec :
    {
        . = ALIGN(4);
		
		*(.chip_programmer_data)
		*(.programmer_data)        
    } >PMU_STARTUP_CONFIG

    .ARM.extab  :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } >PMU_STARTUP_CONFIG

    .ARM :
    {
        __exidx_start = .;
        *(.ARM.exidx*)
        __exidx_end = .;
    } >PMU_STARTUP_CONFIG

    .rom_data_sec :
    {
        . = ALIGN(4);
        _rom_sdata = .;        /* create a global symbol at data start */

        . = ALIGN(4);
        _rom_edata = .;        /* define a global symbol at data end */

        /* used by the startup to initialize data */
        _rom_sidata = _rom_sdata;
    } >PMU_STARTUP_CONFIG

    .stack_sec :
    {
        *(.sys_stack)
    } >PMU_STARTUP_CONFIG

    /* Initialized data sections goes into RAM, load LMA copy after code */
    .app_data_sec :
    {
        . = ALIGN(4);
        _sdata = .;        /* create a global symbol at data start */
        *(.isr_vector)
        *(.data)           /* .data sections */
        *(.data*)          /* .data* sections */
        *(.bt_appdata_seg*)

        . = ALIGN(4);
        _edata = .;        /* define a global symbol at data end */

        /* used by the startup to initialize data */
        _sidata = _sdata;
    } >PMU_STARTUP_CONFIG

    /* The program code and other data goes into FLASH */
    .app_code_sec :
    {
        . = ALIGN(4);
        *(.text)           /* .text sections (code) */
        *(.text*)          /* .text* sections (code) */

        . = ALIGN(4);
        _etext = .;        /* define a global symbols at end of code */
    } >PMU_STARTUP_CONFIG

    /* Constant data goes into FLASH */
    .app_rodata_sec :
    {
        . = ALIGN(4);
        *(.rodata)         /* .rodata sections (constants, strings, etc.) */
        *(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
        . = ALIGN(4);
    } >PMU_STARTUP_CONFIG

    .app_bss_start :
    {
        . = ALIGN(4);
        _bss = .;
        _sbss = .;         /* define a global symbol at bss start */
        __bss_start__ = _sbss;
    } >PMU_STARTUP_CONFIG

    /* User_heap_stack section, used to check that there is enough RAM left */
    ._user_heap_stack :
    {
        . = ALIGN(4);
        _heap = .;
        *(.os_heap*)
        PROVIDE ( end = . );
        PROVIDE ( _end = . );
        . = . + _Min_Heap_Size;
        . = . + _Min_Stack_Size;
        _eheap = .;
        . = ALIGN(4);
    } >PMU_STARTUP_CONFIG

    /* Uninitialized data section */
    . = ALIGN(4);
    .app_bss_sec :
    {
        /* This is used by the startup in order to initialize the .bss secion */
        *(.bss)
        *(.bss*)
    } >PMU_STARTUP_CONFIG

    .app_bss_end :
    {
        . = ALIGN(4);
        _ebss = .;         /* define a global symbol at bss end */
        __bss_end__ = _ebss;
    } >PMU_STARTUP_CONFIG

    /* Remove information from the standard libraries */
    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
    }

    __etext = _etext;
    __init_data = _sidata;
    __data_start__ = _sdata;
    _data = _sdata;
    __data_end__ = _edata;

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
