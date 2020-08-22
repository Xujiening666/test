/**
  ******************************************************************************
  * @file      startup_sl28xx.s
  * @author    MCD Application Team
  * @version   V0.1
  * @date      15-May-2017
  * @brief     SL28xx Devices vector table for Atollic TrueSTUDIO toolchain.
  *            This module performs:
  *                - Set the initial SP
  *                - Set the initial PC == Reset_Handler,
  *                - Set the vector table entries with the exceptions ISR address
  *                - Branches to main in the C library (which eventually
  *                  calls main()).
  *            After Reset the Cortex-M4 processor is in Thread mode,
  *            priority is Privileged, and the Stack is set to Main.
  ******************************************************************************
  */

  .syntax unified
  .cpu cortex-m4
  .fpu softvfp
  .thumb

.global  g_pfnVectors
.global  Default_Handler
.global  exception_entry

/* start address for the initialization values of the .data section.
defined in linker script */
.word  _sidata
/* start address for the .data section. defined in linker script */
.word  _sdata
/* end address for the .data section. defined in linker script */
.word  _edata
/* start address for the .bss section. defined in linker script */
.word  _sbss
/* end address for the .bss section. defined in linker script */
.word  _ebss
/* stack used for SystemInit_ExtMemCtl; always internal RAM used */

/**
 * @brief  This is the code that gets called when the processor first
 *          starts execution following a reset event. Only the absolutely
 *          necessary set is performed, after which the application
 *          supplied main() routine is called.
 * @param  None
 * @retval : None
*/

  .section  .text.Reset_Handler
  .weak  Reset_Handler
  .type  Reset_Handler, %function
Reset_Handler:

  movs	  r0, 	#0
  movs	  r1, 	#0
  movs	  r2, 	#0
  movs	  r3, 	#0
  movs	  r4, 	#0
  movs	  r5, 	#0
  movs	  r6, 	#0
  movs	  r7, 	#0
  movs	  r8, 	#0
  movs	  r9, 	#0
  movs	  r10, 	#0
  movs	  r11, 	#0
  movs	  r12, 	#0
  movs	  r14, 	#0

  ldr     r0, =__app_stack_end
  mov     sp, r0

/* Copy the data segment initializers from flash to SRAM */
/*  movs  r1, #0
  b  LoopCopyDataInit

CopyDataInit:
  ldr  r3, =_sidata
  ldr  r3, [r3, r1]
  str  r3, [r0, r1]
  adds  r1, r1, #4

LoopCopyDataInit:
  ldr  r0, =_sdata
  ldr  r3, =_edata
  adds  r2, r0, r1
  cmp  r2, r3
  bcc  CopyDataInit*/

  ldr  r2, =_sbss
  b  LoopFillZerobss
/* Zero fill the bss segment. */
FillZerobss:
  movs  r3, #0
  str  r3, [r2], #4
LoopFillZerobss:
  ldr  r3, = _ebss
  cmp  r2, r3
  bcc  FillZerobss

/* Call the clock system intitialization function.*/
  bl  SystemInit
/* Call static constructors */
/*    bl __libc_init_array */ /* unused function, if wanna open, refer to ldscript file */
/* Call the application's entry point.*/
  bl  startup_main
  bx  lr
.size  Reset_Handler, .-Reset_Handler

.thumb_func
.section  .text.Exception_Handler
.weak  Exception_Handler
.type  Exception_Handler, %function
Exception_Handler:
				CPSID   F
				TST     LR, #4
				ITE     EQ
				MRSEQ   R0, MSP
				MRSNE   R0, PSP
				STMDB   R0!, {R4-R11}
				MOV       R1, LR
				/* jump to exception_entry */
				BL      exception_entry
.size  Exception_Handler, .-Exception_Handler
