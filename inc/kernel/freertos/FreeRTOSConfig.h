/*
    FreeRTOS V8.0.1 - Copyright (C) 2014 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that has become a de facto standard.             *
     *                                                                       *
     *    Help yourself get started quickly and support the FreeRTOS         *
     *    project by purchasing a FreeRTOS tutorial book, reference          *
     *    manual, or both from: http://www.FreeRTOS.org/Documentation        *
     *                                                                       *
     *    Thank you!                                                         *
     *                                                                       *
    ***************************************************************************

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>!AND MODIFIED BY!<< the FreeRTOS exception.

    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available from the following
    link: http://www.freertos.org/a00114.html

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?"                                     *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org - Documentation, books, training, latest versions,
    license and Real Time Engineers Ltd. contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.OpenRTOS.com - Real Time Engineers ltd license FreeRTOS to High
    Integrity Systems to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H
#include "sys_config.h"

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

#define configUSE_PREEMPTION			1
#define configUSE_IDLE_HOOK	                1
#define configUSE_TICK_HOOK				1

#if (SYS_CHIP_ID == CHIP_SL2801)
#define configMINIMAL_STACK_SIZE		( ( unsigned short ) 256 ) //70 )
#define configCPU_CLOCK_HZ				(( unsigned long )SYS_CPU_CLOCK)
#define configTICK_RATE_HZ				( ( TickType_t ) 1000 )
#define configUSE_TRACE_FACILITY		1
#define configQUEUE_REGISTRY_SIZE		10
#define configTIMER_TASK_PRIORITY		11
#define configTIMER_QUEUE_LENGTH		16
#define configCHECK_FOR_STACK_OVERFLOW	1
#else
extern uint16_t mini_stack_size;
extern unsigned long timer_task_priority;
extern unsigned long timer_queue_length;
#define configPRE_SLEEP_PROCESSING( x ) vPreSleepHook(x)
#define configPOST_SLEEP_PROCESSING( x ) vPostSleepHook(x)
#define configUSE_TRACE_FACILITY		0
#define configQUEUE_REGISTRY_SIZE		0
#define configMINIMAL_STACK_SIZE        (mini_stack_size)
#define configTIMER_TASK_PRIORITY		(timer_task_priority)
#define configTIMER_QUEUE_LENGTH		(timer_queue_length)
#define configCHECK_FOR_STACK_OVERFLOW	2
#endif

//FIXME: configTOTAL_HEAP_SIZE = total - text - bss - data
#define configTOTAL_HEAP_SIZE			((size_t) (71 * 512))
#define configMAX_TASK_NAME_LEN			( 12 )
#define configUSE_16_BIT_TICKS			0
#define configIDLE_SHOULD_YIELD			0
#define configUSE_CO_ROUTINES 			0
#define configUSE_MUTEXES				1
#define configUSE_RECURSIVE_MUTEXES		0
#define configUSE_QUEUE_SETS			1
#define configUSE_COUNTING_SEMAPHORES	1
#define configUSE_ALTERNATIVE_API		1

#define configMAX_PRIORITIES			( 20UL )
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Timer related defines. */
#define configUSE_TIMERS				1
#define configTIMER_TASK_STACK_DEPTH	( configMINIMAL_STACK_SIZE * 2 )
#define configUSE_MALLOC_FAILED_HOOK    1
/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet				1
#define INCLUDE_uxTaskPriorityGet				1
#define INCLUDE_vTaskDelete						1
#define INCLUDE_vTaskCleanUpResources			1
#define INCLUDE_vTaskSuspend					1
#define INCLUDE_vTaskDelayUntil					1
#define INCLUDE_vTaskDelay						1
#define INCLUDE_uxTaskGetStackHighWaterMark		0
#define INCLUDE_xTaskGetSchedulerState			1
#define INCLUDE_xTimerGetTimerDaemonTaskHandle	0
#define INCLUDE_xTaskGetIdleTaskHandle			1
#define INCLUDE_pcTaskGetTaskName				1
#define INCLUDE_eTaskGetState					1
#define INCLUDE_xSemaphoreGetMutexHolder		0
#define INCLUDE_xTimerPendFunctionCall			1


/* Processor specific definitions. */
#ifdef __INTC_PRIO_BITS
  #define configPRIO_BITS           __INTC_PRIO_BITS
#else
  #define configPRIO_BITS           4        /* 15 priority levels */
#endif

/* The lowest interrupt priority that can be used in a call to a "set priority"
function. */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY      0xf

/* The highest interrupt priority that can be used by any interrupt service
routine that makes calls to interrupt safe FreeRTOS API functions.  DO NOT CALL
INTERRUPT SAFE FREERTOS API FUNCTIONS FROM ANY INTERRUPT THAT HAS A HIGHER
PRIORITY THAN THIS! (higher priorities are lower numeric values. */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY  5

/* Interrupt priorities used by the kernel port layer itself.  These are generic
to all RISC32 ports, and do not rely on any particular library functions. */
#define configKERNEL_INTERRUPT_PRIORITY     ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-RISC32.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY   ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )



/* Use the RISC32 optimised task selection rather than the generic C code
version. */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1

#define configUSE_TICKLESS_IDLE         1
#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP    3



#ifdef __ICCARM__
	void vAssertCalled( const char *pcFile, unsigned long ulLine );
	#define configASSERT( x ) if( x == 0 ) vAssertCalled( __FILE__, __LINE__ );
#endif

#endif /* FREERTOS_CONFIG_H */