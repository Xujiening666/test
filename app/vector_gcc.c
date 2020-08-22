#define LOG_LEV 4
#include "sys_inc.h"
#include "log.h"
#include "FreeRTOS.h"
#include "task.h"
#include "write_irq.h"

void Reset_Handler(void);
void Exception_Handler(void);
static void IntDefaultHandler(void);
void nor_irq_process(void);
void vPortSVCHandler(void);
void xPortPendSVHandler(void);
void xPortSysTickHandler(void);


uint32_t pui32Stack[SYS_STACK_SIZE]  AT(.app_stack);


AT(.isr_vector)  //intvecs
void (* const g_pfnVectors[64])(void) =
{
    (void (*)(void))((uint32_t)pui32Stack + sizeof(pui32Stack)),
                                            // 0 The initial stack pointer
    Reset_Handler,                          // 1 The reset handler
    Exception_Handler,                      // 2 The NMI handler
    Exception_Handler,                      // 3 The hard fault handler
    Exception_Handler,                      // 4 The MPU fault handler
    Exception_Handler,                      //5 The bus fault handler
    Exception_Handler,                      //6 The usage fault handler
    IntDefaultHandler,                      //7 Reserved
    IntDefaultHandler,                      //8 Reserved
    IntDefaultHandler,                      //9 Reserved
    IntDefaultHandler,                      //10 Reserved
#ifdef USE_FREERTOS
    vPortSVCHandler,                        //11 SVCall handler
#else
    IntDefaultHandler,                      //11 SVCall handler
#endif
    Exception_Handler,                      //12 Debug monitor handler
    IntDefaultHandler,                      //13 Reserved
#ifdef USE_FREERTOS
    xPortPendSVHandler,                     //14 The PendSV handler
    xPortSysTickHandler,                    //15 The SysTick handler
#else
    IntDefaultHandler,                      //14 The PendSV handler
    IntDefaultHandler,                      //15 The SysTick handler
#endif
    IntDefaultHandler,                      //16 GPIO Port A
    IntDefaultHandler,                      //17 GPIO Port B
    IntDefaultHandler,                      //18 GPIO Port C
    IntDefaultHandler,                      //19 GPIO Port D
    IntDefaultHandler,                      //20 Reserved
    IntDefaultHandler,                      //21 UART0 Rx and Tx
    IntDefaultHandler,                      //22 UART1 Rx and Tx
    IntDefaultHandler,                      //23 Reserved
    IntDefaultHandler,                      //24 I2C0 Master and Slave
    IntDefaultHandler,                      //25
    IntDefaultHandler,                      //26
    IntDefaultHandler,                      //27
    IntDefaultHandler,                      //28
    IntDefaultHandler,                      //29 Reserved
    IntDefaultHandler,                      //30 ADC Channel 0
    IntDefaultHandler,                      //31 ADC Channel 1
    IntDefaultHandler,                      //32 ADC Channel 2
    IntDefaultHandler,                      //33 ADC Channel 3
    IntDefaultHandler,                      //34 Watchdog Timer
    IntDefaultHandler,                      //35 Timer 0 subtimer A
    IntDefaultHandler,                      //36 Timer 0 subtimer B
    IntDefaultHandler,                      //37 Timer 1 subtimer A
    IntDefaultHandler,                      //38 Timer 1 subtimer B
    IntDefaultHandler,                      //39 Timer 2 subtimer A
    IntDefaultHandler,                      //40 Timer 2 subtimer B
    nor_irq_process,                        //41
    IntDefaultHandler,                      //42
    IntDefaultHandler,                      //43
    IntDefaultHandler,                      //44 Reserved
    IntDefaultHandler,                      //45
    IntDefaultHandler,                      //46
    IntDefaultHandler,                      //47
    IntDefaultHandler,                      //48
    IntDefaultHandler,                      //49
    IntDefaultHandler,                      //50 Reserved
    IntDefaultHandler,                      //51 Timer 3 subtimer A
    IntDefaultHandler,                      //52 Timer 3 subtimer B
    IntDefaultHandler,                      //53
    IntDefaultHandler,                      //54
    IntDefaultHandler,                      //55
    Exception_Handler,                      //56 Rom write fault
    IntDefaultHandler,                      //57
    IntDefaultHandler,                      //58
    IntDefaultHandler,                      //59
    IntDefaultHandler,                      //60
    IntDefaultHandler,                      //61
    IntDefaultHandler,                      //62
    IntDefaultHandler,                      //63
};

//*****************************************************************************
//
// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
AT(.default_int_seg)
static void IntDefaultHandler(void)
{
    //
    // Go into an infinite loop.
    //
    printf_debug("un reg isr:IRQn:%u, Exception:%u\n", get_current_interrupt(),  get_current_exception());
    while(1);

}