#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "write_irq.h"
#include "cpu.h"
#include "device.h"
#include "system.h"


extern uint32_t pui32Stack[SYS_STACK_SIZE];
uint32_t get_scb_reg(uint32_t num);


__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_PSP(void)
{
  register uint32_t result;

  __ASM volatile ("MRS %0, psp\n"  : "=r" (result) );
  return(result);
}


#if CPU_LOAD_EN
static uint64_t idle_begin, idle_end, idle_total;
uint32_t cpu0_load_per = 0;

static uint64_t cpu1_time_mark = 0, cpu1_time_total = 0;
uint32_t cpu1_load_per = 0;

AT(.sys_com_seg)
void cpu1_load_calc_start()
{
    cpu1_time_mark = get_timestamp();
}

AT(.sys_com_seg)
void cpu1_load_calc_end()
{
    cpu1_time_total += (get_timestamp() - cpu1_time_mark);
}

AT(.sys_com_seg)
uint32_t sys_get_cpu0_load_per()
{
    return cpu0_load_per;
}

AT(.sys_com_seg)
uint32_t sys_get_cpu1_load_per()
{
    return cpu1_load_per;
}

AT(.sys_com_seg)
void cpu_load_timer_cb(xTimerHandle xTimer)
{
    cpu0_load_per = 100 - (uint32_t)(idle_total/10/CPU_LOAD_PERIOD);
    cpu1_load_per = (uint32_t)(cpu1_time_total/10/CPU_LOAD_PERIOD);
#if CPU_LOAD_SHOW_STUS
    printf_debug("cpu load:%u\n", cpu0_load_per);
#endif
    idle_total = 0;
    cpu1_time_total = 0;
}
#else

AT(.sys_com_seg)
uint32_t sys_get_cpu0_load_per()
{
    return 0;
}

AT(.sys_com_seg)
uint32_t sys_get_cpu1_load_per()
{
    return 0;
}

AT(.sys_com_seg)
void cpu1_load_calc_start()
{
}

AT(.sys_com_seg)
void cpu1_load_calc_end()
{
}

#endif

uint32_t idle_task_process_count;

AT(.sys_com_seg)
uint32_t get_idle_process_count(void)
{
    return idle_task_process_count;
}

AT(.sys_com_seg)
void vTaskErrorProcess(uint8_t id, uint32_t param)
{
    switch (id) {
    case OS_USER_STACK_OVERFLOW:
        loge("task:%s stack overflow, stack start addr:0x%x, stack top addr:0x%x",
            pcTaskName(),
            (uint32_t)puTaskStackAddr(),
            (uint32_t)__get_PSP());
        break;

    case OS_USER_MALLOC_FAILED:
        loge("task: %s malloc failed", pcTaskName());
        break;

    case OS_USER_APPLICATION_IDLE:
#if CPU_LOAD_EN
        vPortSetTimeBeforeSleep(0);
        idle_begin = get_timestamp();
#endif
#if SLEEP_EN
#if MODE_BT_EN
    #if BT_BLE_EN
        sleep_btble();
    #else
        sleep_bt();
    #endif
#else
        sleep();
#endif
#endif
        idle_task_process_count++;
        break;

#if CPU_LOAD_EN
    case OS_USER_POST_SLEEP:
        idle_end = get_timestamp();
        idle_total +=  (idle_end - idle_begin);
        break;
#endif
    }
}

/*
*********************************************************************************************************
*                                               EXCEPTION ENTRY
*
* Description:  the entry of CPU exception, mainly for CPU nmi, hard fault,
* memmanger fault, bus fault, usage fault, SVCall, debugmonitor exception.
*
* register list in stack:
* ---------LOW ADDR--------
* R11              <-PSP(pstack)
* R10              <-PSP+4*1
* R9               <-PSP+4*2
* R8               <-PSP+4*3
* R7               <-PSP+4*4
* R6               <-PSP+4*5
* R5               <-PSP+4*6
* R4               <-PSP+4*7
* R0               <-PSP+4*8
* R1               <-PSP+4*9
* R2               <-PSP+4*10
* R3               <-PSP+4*11
* R12              <-PSP+4*12
* R14(LR)          <-PSP+4*13
* R15(PC)          <-PSP+4*14
* xPSR             <-PSP+4*15
* --------HIGH ADDR--------
*
* Arguments  :  pstack:the pointer of stack, PSP before the exception happen.
*
* Returns    :  OK if process CPU exception succeeded, others if failed.
*********************************************************************************************************
*/


AT(.default_int_seg)
int32_t exception_entry(uint32_t *pstack, uint32_t fault_handler_lr)
{
	uint32_t i;
	uint32_t exceptionno = get_current_exception();
#if DEBUG_FAULT_DIAGNOSIS
    uint32_t mfsr, bfsr, ufsr;
#endif
    uint32_t on_thread_before_fault = false, stack_is_overflow = false;
    uint32_t stack_start_addr, stack_pointer;

    stack_start_addr = ((uint32_t)pui32Stack);
    stack_pointer = (uint32_t)pstack;

    on_thread_before_fault = fault_handler_lr & (1UL << 2);

    if (on_thread_before_fault) {
        stack_start_addr = (uint32_t)puTaskStackAddr();
        stack_pointer = __get_PSP();
        printf_debug("thread:%s\n", pcTaskName() != NULL ? pcTaskName() : "NO_NAME");
        //vTaskCheckStackOverflow();
    }

    //R0~R11
    stack_pointer += sizeof(size_t) * 16;

    /* check stack overflow */
    if (stack_pointer < stack_start_addr) {
        stack_is_overflow = true;
    }

    if (stack_is_overflow) {
        if (on_thread_before_fault) {
            printf_debug("Error: Thread(%s) stack(0x%08x) was overflow, start addr:0x%08x\n",
                pcTaskName() != NULL ? pcTaskName() : "NO_NAME",
                stack_pointer,
                stack_start_addr);
        } else {
            printf_debug("Error: Main stack(%08x) was overflow, start addr:0x%08x\n", stack_pointer, stack_start_addr);
        }
    }

	printf_debug("pstack:0x%x\n", (uint32_t)pstack);
	printf_debug("exception:%d happen\n", exceptionno);
	//print R0-R3
	for (i = 0; i <=3; i++)
	{
		printf_debug("R%d:0x%x:0x%x\n", i, (pstack + (8 + i)), *(pstack + (8 + i)));
	}
	//print R4-R11
	for (i = 0; i <=7; i++)
	{
		printf_debug("R%d:0x%x:0x%x\n", i + 4, (pstack + (7 - i)), *(pstack + (7 - i)));
	}
	//print R12, R14(LR), R15(PC), xPSR
	printf_debug("R12:0x%x:0x%x\n", (pstack + 12), *(pstack + 12));
	printf_debug("R14(LR):0x%x:0x%x\n", (pstack + 13), *(pstack + 13));
	printf_debug("R15(PC):0x%x:0x%x\n", (pstack + 14), *(pstack + 14));
	printf_debug("xPSR:0x%x:0x%x\n", (pstack + 15), *(pstack + 15));
    hexdump("stack",(char*)pstack,5);

	switch (exceptionno)
	{
	case 2:
	{
		printf_debug("NMI happen\n");
		break;
	}
	case 3:
	case 4:
	case 5:
	case 6:
	{
		printf_debug("hard fault happen, HFSR:0x%x\n", get_scb_reg(REG_HFSR));
		printf_debug("memm fault maybe happen, MFSR:0x%x, MMFAR:0x%x\n", readb(0xE000ED28), get_scb_reg(REG_MMFAR));
		printf_debug("bus fault maybe happen, BFSR:0x%x, BFAR:0x%x\n", readb(0xE000ED29), get_scb_reg(REG_BFAR));
		printf_debug("usage fault maybe happen, UFSR:0x%x\n", readw(0xE000ED2A));
#if DEBUG_FAULT_DIAGNOSIS
        if (get_scb_reg(REG_HFSR_VECTTBL)) {
            printf_debug("Hard fault is caused by failed vector fetch\n");
        }
        if (get_scb_reg(REG_HFSR_FORCED)) {
            mfsr = readb(0xE000ED28);
            if (mfsr & (1 << 0)) {
                printf_debug("Memory management fault is caused by instruction access violation\n");
            } else if (mfsr & (1 << 1)) {
                printf_debug("Memory management fault is caused by data access violation\n");
            } else if (mfsr & (1 << 3)) {
                printf_debug("Memory management fault is caused by unstacking error\n");
            } else if (mfsr & (1 << 4)) {
                printf_debug("Memory management fault is caused by stacking error\n");
            } else if (mfsr & (1 << 5)) {
                printf_debug("Memory management fault is caused by floating-point lazy state preservation\n");
            } else if ((mfsr & (1 << 7)) && (mfsr & 0x3)) {
                printf_debug("The memory management fault occurred address is 0x%x\n", get_scb_reg(REG_MMFAR));
            }

            bfsr = readb(0xE000ED29);
            if (bfsr & (1 << 0)) {
                printf_debug("Bus fault is caused by instruction access violation\n");
            } else if (bfsr & (1 << 1)) {
                printf_debug("Bus fault is caused by precise data access violation\n");
            } else if (bfsr & (1 << 2)) {
                printf_debug("Bus fault is caused by imprecise data access violation\n");
            } else if (bfsr & (1 << 3)) {
                printf_debug("Bus fault is caused by unstacking error\n");
            } else if (bfsr & (1 << 4)) {
                printf_debug("Bus fault is caused by stacking error\n");
            } else if (bfsr & (1 << 5)) {
                printf_debug("Bus fault is caused by floating-point lazy state preservation\n");
            }else if ((bfsr & (1 << 7)) && (bfsr & (1 << 1))) {
                printf_debug("The bus fault occurred address is 0x%x\n", get_scb_reg(REG_BFAR));
            }

            ufsr = readw(0xE000ED2A);
            if (ufsr & (1 << 0)) {
                printf_debug("Usage fault is caused by attempts to execute an undefined instruction.0x%x:0x%x\n", *(pstack + 14), readl(*(pstack + 14)));
            } else if (ufsr & (1 << 1)) {
                printf_debug("Usage fault is caused by attempts to switch to an invalid state (e.g., ARM)\n");
            } else if (ufsr & (1 << 2)) {
                printf_debug("Usage fault is caused by attempts to do an exception with a bad value in the EXC_RETURN number\n");
            } else if (ufsr & (1 << 3)) {
                printf_debug("Usage fault is caused by attempts to execute a coprocessor instruction\n");
            } else if (ufsr & (1 << 8)) {
                printf_debug("Usage fault is caused by indicates that an unaligned access fault has taken place\n");
            } else if (ufsr & (1 << 9)) {
                printf_debug("Usage fault is caused by Indicates a divide by zero has taken place (can be set only if DIV_0_TRP is set)\n");
            }

        }
#endif
		break;
	}
	case 11:
	{
		printf_debug("SVCall fault happen\n");
		break;
	}
	case 12:
	{
		printf_debug("SVCall fault happen, DFSR:0x%x\n", get_scb_reg(REG_DFSR));
		break;
	}
    case 56:
	{
		printf_debug("Write irq happen, CUR PC:0x%x, SR:0x%x, CPU0 ADDR:0x%x, PC:0x%0x, CPU1 ADDR:0x%x, PC:0x%x\n",
            *(pstack + 14),
            write_irq_get_sr(),
            write_irq_get_addr(WRIRQ_CPU0),
            write_irq_get_pc(WRIRQ_CPU0),
            write_irq_get_addr(WRIRQ_CPU1),
            write_irq_get_pc(WRIRQ_CPU1));

		break;
	}
	default:
	{
		//invalid exception nr
		printf_debug("invalid exception nr\n");
	}
	}
	// if happen fault, print important information and drop-dead halt
	while(1);

	return 0;
}

#if (0 == DEBUG_EXCEPTION_INIT)
void exception_init(void)
{
}
#endif

#define EXCEPTION_TEST 0
#define EXCEPTION_WRITE_ROM_TEST 0
#define EXCEPTION_STACK_OVERFLOW_TEST 0
#define EXCEPTION_WRITE_CODE_TEST 0
#define EXCEPTION_DIV_ZERO_TEST 0
#if EXCEPTION_TEST
AT(.default_int_seg)
void exception_test(void)
{
#if EXCEPTION_WRITE_ROM_TEST
    logi("test rom write fault begin\n");
    writel(0xef, 0x4);
    logi("test rom write fault end\n");
#endif

#if EXCEPTION_STACK_OVERFLOW_TEST
    uint32_t i;
    volatile uint8_t buf[1720];

    logi("test stack overflow begin\n");
    pui32Stack[0] = 0xef;
    logi("test stack overflow end\n");

    //triger the stack overflow exception
    for (i = 0; i < 1720; i++) {
        buf[i] = 0xef;
        logi("buf[%u]:%x", i, buf[i]);
    }
#endif

#if EXCEPTION_WRITE_CODE_TEST
    logi("test write code begin\n");
    write_irq_set_region_addr(WRIRQ_RG2, 0x00807400, 0x00807400 + 0x5000);
    write_irq_enable_region_irq(WRIRQ_RG2, WRIRQ_CPU0, true);
    write_irq_enable_region_irq(WRIRQ_RG2, WRIRQ_CPU1, true);
    logi("test write code end\n");
#endif
    //write_irq_set_region_addr(WRIRQ_RG3, text_start_addr, text_end_addr);
    //write_irq_enable_region_irq(WRIRQ_RG3, WRIRQ_CPU0, true);
    //write_irq_enable_region_irq(WRIRQ_RG3, WRIRQ_CPU1, true);

#if EXCEPTION_DIV_ZERO_TEST
    //triger the div by 0 fault
    logi("zero:%u", 1/readl(0x40085180));
#endif
}
#endif



