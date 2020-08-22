#define LOG_LEV     3
#define SLTK_LOG_TAG "poweroff"
#include "sys_inc.h"
#include "user_config.h"
#include "task_main.h"
#include "info_mem.h"
#include "bt.h"
#include "pmu.h"
#include "record.h"

extern TimerHandle_t AutoShuntdownTimerHandle;

AT(.mode_poweroff_seg)
void mode_poweroff(void)
{
    logi("poweroff mode.");

    if (pmu_is_use_hsw()) {
        pmu_hsw_reset_enable(true);
    }
	/* Modify by arunboy 20200720 */
    if (rec_context) {
        libc_free(rec_context);
        rec_context = NULL;
    }
	if(auto_shutdown_context)
    {
        if(!auto_shutdown_context->shutdown_ing)
            xTimerStop(AutoShuntdownTimerHandle, 10);
        libc_free(auto_shutdown_context);
		auto_shutdown_context = NULL;
    }
	logi("poweroff %d", __LINE__);
#if DISP_EN
    disp_state(STATE_POWEROFF);
#endif
    logi("poweroff %d", __LINE__);

#if (TONE_EN && SUNCHIP_SHUTDOWN)
    if (scene_mode.switch_reason != SW_CHARGE
        && scene_mode.switch_reason != SW_STARTUP_FIRST) {
        play_tone(TONE_SHUTDOWN, true);
    }
#endif
    logi("arunboy %d",device_get_num());
#if AUDIO_KARAOKE_MODE
    if(DEV_NONE != device_get_num()) {
        audio_service_karaoke_exit();
    }
#endif
    logi("arunboy ^~^");
    auss_enter_sleep();

#if (MODE_MUSIC_EN && MUSIC_INFO_SAVE_MODE == 2)
    play_info_write_when_poweroff();
#endif
#if BT_BACKGROUND_EN
    bt_info_save_when_poweroff();
#endif
    /* 将info_buf内的数据统一写入flash */
    info_write_page();
#if MODE_MUSIC_EN
    /* 务必在info_write_page()后 */
    player_store_priv_data_to_flash();
#endif

    /* PC3/4在软关机（VBAT引脚保持有电）中，不会像其他GPIO那样自动处于低电平状态，
    * 需要手动配置GPIO的功能和状态，例如输出0。
    */
    pin_set_value(PIN_C3, 0);
    pin_set_value(PIN_C4, 0);
    pin_set_func(PIN_C3, PIN_FUNCOUTPUT);
    pin_set_func(PIN_C4, PIN_FUNCOUTPUT);

    /* suspend all tasks */
    vTaskSuspendAll(); //FIXME: or vTaskEndScheduler() ?
    pmu_task_deinit();
    INTC_IRQ_Suspend();

    watchdog_deinit();

    mdelay(POWER_OFF_DELAY);
#if !BAT_EXIST
    pmu_set_poweron_wakeupsrc(PMU_VBUS_WAKEUPSRC, false);
#endif
    set_rc_cal_paras(RC_CAL_PRD_BASE, RC_CAL_PRD, RC_CAL_NUM, RC_CAL_CON_MODE, RC_POWEROFF_CAL_EN);
#if RTC_EN
    rtc_clk_sel(RTC_CLK_LOSC);
#endif
    logi("poweroff mode end");
    enter_poweroff_mode();
}

