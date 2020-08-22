#define LOG_LEV     4
#define SLTK_LOG_TAG "sleep"
#include "sys_inc.h"
#include "user_config.h"
#include "task_main.h"
#include "hal_auss_lib.h"
#include "hal_power_lib.h"
#include "key.h"
#include "sd.h"

AT(.sleep1_sram_seg)
void sleep_wakeup_enable(void)
{
#if SLEEP_EN
#if KADC0_EN
    #if KADC0_IRQ
        kadc_wakeup_set(KADC_ID_00, true);
    #else
        pin_set_wakeup(KADC0_PIN_NUM, true);
        pin_irq_config(KADC0_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_RF_EDGE);
    #endif
#endif
#if KADC1_EN
    #if KADC1_IRQ
        kadc_wakeup_set(KADC_ID_01, true);
    #else
        pin_set_wakeup(KADC1_PIN_NUM, true);
        pin_irq_config(KADC1_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_RF_EDGE);
    #endif
#endif
#if KADC2_EN
    #if KADC2_IRQ
        kadc_wakeup_set(KADC_ID_02, true);
    #else
        pin_set_wakeup(KADC2_PIN_NUM, true);
        pin_irq_config(KADC2_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_RF_EDGE);
    #endif
#endif
#if KADC3_EN
    #if KADC3_IRQ
        kadc_wakeup_set(KADC_ID_03, true);
    #else
        pin_set_wakeup(KADC3_PIN_NUM, true);
        pin_irq_config(KADC3_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_RF_EDGE);
    #endif
#endif
#if KADC4_EN
    #if KADC4_IRQ
        kadc_wakeup_set(KADC_ID_04, true);
    #else
        pin_set_wakeup(KADC4_PIN_NUM, true);
        pin_irq_config(KADC4_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_RF_EDGE);
    #endif
#endif
#if KADC5_EN
    #if KADC5_IRQ
        kadc_wakeup_set(KADC_ID_05, true);
    #else
        pin_set_wakeup(KADC5_PIN_NUM, true);
        pin_irq_config(KADC5_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_RF_EDGE);
    #endif
#endif

#if KEY_IO_EN
#ifdef PIN_KIO0
    pin_set_wakeup(PIN_KIO0, true);
#endif
#ifdef PIN_KIO1
    pin_set_wakeup(PIN_KIO1, true);
#endif
#ifdef PIN_KIO2
    pin_set_wakeup(PIN_KIO2, true);
#endif
#ifdef PIN_KIO3
    pin_set_wakeup(PIN_KIO3, true);
#endif
#ifdef PIN_KIO4
    pin_set_wakeup(PIN_KIO4, true);
#endif
#ifdef PIN_KIO5
    pin_set_wakeup(PIN_KIO5, true);
#endif
#ifdef PIN_KIO6
    pin_set_wakeup(PIN_KIO6, true);
#endif
#ifdef PIN_KIO7
    pin_set_wakeup(PIN_KIO7, true);
#endif
#endif

#if (SYS_PLATFORM == PLATFORM_CHIP)
    pmu_set_wakeup(true);
#endif

#if RTC_EN
    rtc_enable_wakeup(0x7);
#endif

#if KEY_IR_EN
#if 0
    clk_set_src(CLK_IR, CLK_LOSC);
#else
    clk_set_src(CLK_IR, CLK_32K_HOSC);  //ROM BUG: the actual configuration is CLK_LOSC
#endif
    irrx_set_wakeup(true);
#endif

#if SD_DETECT_EN && SD_EN
    sd_set_wakeup(true);
#endif

#if USB_DETECT_EN && USB_EN
    usb_set_wakeup(true);
#endif

    auss_enter_sleep();
    audiopll_sleep();
#endif
}

AT(.sleep1_sram_seg)
void sleep_wakeup_disable(void)
{
#if SLEEP_EN
#if KADC0_EN
    #if KADC0_IRQ
        kadc_wakeup_set(KADC_ID_00, false);
    #else
        pin_irq_config(KADC0_PIN_NUM, PIN_IRQ_DIS);
        pin_set_wakeup(KADC0_PIN_NUM, false);
    #endif
#endif
#if KADC1_EN
    #if KADC1_IRQ
        kadc_wakeup_set(KADC_ID_01, false);
    #else
        pin_irq_config(KADC1_PIN_NUM, PIN_IRQ_DIS);
        pin_set_wakeup(KADC1_PIN_NUM, false);
    #endif
#endif
#if KADC2_EN
    #if KADC2_IRQ
        kadc_wakeup_set(KADC_ID_02, false);
    #else
        pin_irq_config(KADC2_PIN_NUM, PIN_IRQ_DIS);
        pin_set_wakeup(KADC2_PIN_NUM, false);
    #endif
#endif
#if KADC3_EN
    #if KADC3_IRQ
        kadc_wakeup_set(KADC_ID_03, false);
    #else
        pin_irq_config(KADC3_PIN_NUM, PIN_IRQ_DIS);
        pin_set_wakeup(KADC3_PIN_NUM, false);
    #endif
#endif
#if KADC4_EN
    #if KADC4_IRQ
        kadc_wakeup_set(KADC_ID_04, false);
    #else
        pin_irq_config(KADC4_PIN_NUM, PIN_IRQ_DIS);
        pin_set_wakeup(KADC4_PIN_NUM, false);
    #endif
#endif
#if KADC5_EN
    #if KADC5_IRQ
        kadc_wakeup_set(KADC_ID_05, false);
    #else
        pin_irq_config(KADC5_PIN_NUM, PIN_IRQ_DIS);
        pin_set_wakeup(KADC5_PIN_NUM, false);
    #endif
#endif

#if KEY_IO_EN
#ifdef PIN_KIO0
    pin_set_wakeup(PIN_KIO0, false);
#endif
#ifdef PIN_KIO1
    pin_set_wakeup(PIN_KIO1, false);
#endif
#ifdef PIN_KIO2
    pin_set_wakeup(PIN_KIO2, false);
#endif
#ifdef PIN_KIO3
    pin_set_wakeup(PIN_KIO3, false);
#endif
#ifdef PIN_KIO4
    pin_set_wakeup(PIN_KIO4, false);
#endif
#ifdef PIN_KIO5
    pin_set_wakeup(PIN_KIO5, false);
#endif
#ifdef PIN_KIO6
    pin_set_wakeup(PIN_KIO6, false);
#endif
#ifdef PIN_KIO7
    pin_set_wakeup(PIN_KIO7, false);
#endif
#endif

#if (SYS_PLATFORM == PLATFORM_CHIP)
    pmu_set_wakeup(false);
#endif

#if RTC_EN
    rtc_disable_wakeup(0x7);
#endif

#if KEY_IR_EN
#if 0
    clk_set_src(CLK_IR, CLK_32K_HOSC);
#else
    clk_set_src(CLK_IR, CLK_LOSC);  //ROM BUG: the actual configuration is CLK_32K_HOSC
#endif
    irrx_set_wakeup(false);
#endif

#if SD_DETECT_EN && SD_EN
    sd_set_wakeup(false);
#endif

#if USB_DETECT_EN && USB_EN
    usb_set_wakeup(false);
#endif
#endif
}

AT(.sleep_com_sram_seg)
void sleep_delay_set(void)
{
#if SLEEP_EN
    pm_prevent_sleep_delay_set(SLEEP_DELAY);
#endif
}

AT(.sleep_com_sram_seg)
uint8_t sleep_delay_get(void)
{
#if SLEEP_EN
    return pm_prevent_sleep_delay_get();
#else
    return 0xff;
#endif
}

AT(.sleep_com_sram_seg)
void sleep_delay_inc(uint8_t sec)
{
#if SLEEP_EN
    pm_prevent_sleep_delay_inc(sec);
#endif
}

AT(.sleep_com_sram_seg)
void sleep_delay_dec(uint8_t sec)
{
#if SLEEP_EN
    pm_prevent_sleep_delay_dec(sec);
#endif
}

AT(.sleep_com_sram_seg)
void sleep_prevent_set(enum pm_prevent_sleep prevent)
{
#if SLEEP_EN
    pm_prevent_sleep_set(prevent);
#endif
}

AT(.sleep_com_sram_seg)
void sleep_prevent_clear(enum pm_prevent_sleep prevent)
{
#if SLEEP_EN
    pm_prevent_sleep_clear(prevent);
#endif
}

AT(.sleep_com_sram_seg)
uint32_t sleep_prevent_get(void)
{
#if SLEEP_EN
    return pm_prevent_sleep_get();
#else
    return 0xffffffff;
#endif
}

#if SLEEP_EN
AT(.sleep1_sram_seg)
void sleep_timer_cb(xTimerHandle xTimer)
{
    pm_prevent_sleep_delay_clear();
}
#endif

#if AUTO_SHUNTDOWN
extern TimerHandle_t AutoShuntdownTimerHandle;

AT(.sleep1_sram_seg)
void auto_shutdown_cb(xTimerHandle xTimer)
{
	auto_shutdown_context->shutdown_ing = true;
    event_put(SYSTEM_EVENT_POWEROFF);
}

AT(.sleep1_sram_seg)
void auto_shutdown(bool need)
{
    if(!auto_shutdown_context->shutdown_ing) {
		logd("arunboy status = %d, set status = %d",auto_shutdown_context->auto_shutdown_status,need);
		if(auto_shutdown_context->auto_shutdown_status != need){
		    if(need){
				xTimerReset(AutoShuntdownTimerHandle, 10);
			} else {
				xTimerStop(AutoShuntdownTimerHandle, 10);
			}
			auto_shutdown_context->auto_shutdown_status = need;
		}
    }
}

#endif

