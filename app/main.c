#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "FreeRTOS.h"
#include "task_main.h"
#include "device.h"
#include "disp.h"
#include "serial_debug_io.h"
#include "xfs.h"
#include "dual_common.h"
#include "loadcode.h"
#include "system.h"
#include "codec.h"
#include "twi.h"
#include "spi.h"
#include "i2s.h"
#include "pmu.h"
#include "pwm.h"
#include "uart.h"
#include "bt_api.h"
#include "version.h"
#include "update_api.h"
#include "fcc.h"
#include "spdif.h"
#include "trng.h"
#include "lcmconfig.h"


uint8_t PucHeap[ configTOTAL_HEAP_SIZE ]  AT(.os_heap);
TaskHandle_t main_task_handler;

int main(void);

AT(.startup_seg)
void startup_main(void)
{
    sys_init(SYS_CHIP_ID, BANK_OFFSET);
    main();
}

int main(void)
{
    vPortSetTimeBeforeSleep(TIME_BEFORE_WFI);
    exception_init();
    uart_init();
    enable_irq();
    logi("\r\n---------------hello app, code version:%s---------------", CODE_VERSION);
    prvHeapConfig((uint32_t)PucHeap,configTOTAL_HEAP_SIZE);
    pmu_init();
    ccu_reinit(1120, 192000000, 1, 1, 6, 6);
    spi_init();
    fatfs_init();
    codec_init();
    i2s_init();
    pwm_init();
    rtc_init();
    trng_init();
    load_code_init();
    bt_fcc_test();
    startup_cpu1();
#if 0
	st7735_led_init();
#endif
    xTaskCreate(task_main, "task main",
        MAIN_TASK_STACK_SIZE, NULL, MAIN_TASK_PRIO, &main_task_handler);
    vTaskStartScheduler();

    while (1) {
    }
}
