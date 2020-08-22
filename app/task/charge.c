#define LOG_LEV     5
#define SLTK_LOG_TAG "charge"
#include "sys_inc.h"
#include "user_config.h"
#include "task_main.h"
#include "info_mem.h"
#include "disp.h"
#include "pmu.h"

AT(.charge_sram_seg)
void restart(void)
{
#if WATCHDOG_EN
    timer_stop(TIMER0);
#endif
    vTaskSuspendAll();
    pmu_task_deinit();
    disable_irq();
    logi("restart");
    if (pmu_is_use_hsw()) {
        pmu_hsw_reset_enable(true);
    }
    watchdog_deinit();
    watchdog_init(0);
    while(1);
}

AT(.mode_charge_seg)
void charge_event_process(uint32_t event)
{
    switch(event) {
        case KEY_EVENT_L | KEY_POWER:
        case PMU_EVENT_POWERKEY_L:
        case PMU_EVENT_HSW_ON:
#if !PMU_VBUSIN_RESTART
            //disp_state(STATE_RESET);
            restart();
#endif
            break;

        case PMU_EVENT_INT:
            logd("int");
            pmu_handler();
            break;

        case PMU_EVENT_CHARGE_IN:
            logd("i");
            pmu_charge_enable(true);
            disp_state(STATE_CHARGEING);
            break;

        case PMU_EVENT_CHARGE_OUT:
            logd("o");
            pmu_charge_enable(false);
            scene_mode.switch_reason = SW_CHARGE;
            mode_set_force(MODE_POWEROFF);
            disp_state(STATE_UNCHARGEING);
            break;

        case PMU_EVENT_BAT_LOW_WRN:
            logd("l");
            disp_state(STATE_BAT_LOW);
            //disp_bat_low_digital(true);
            break;

        case PMU_EVENT_BAT_FULL:
            logd("f");
            //pmu_set_poweron_wakeupsrc(PMU_VBUS_WAKEUPSRC, false);
            //scene_mode.switch_reason = SW_CHARGE;
            //mode_set_force(MODE_POWEROFF);
            disp_state(STATE_BAT_FULL);
            break;

#if RTC_EN
        case SYSTEM_EVENT_SECOND:
#if BAT_EXIST
            pmu_timer_cb(NULL);
#endif
        break;
#endif

        default:
            break;
    }
}

bool rf_cal_en_flg=1;
extern uint8_t bat_quantity_percent;

AT(.mode_charge_seg)
void charge_rfcal(void)
{
#if BAT_EXIST
    // rf calibration and store paras into flash when charge
    if(scene_mode.sta == MODE_CHARGE){
        if(bat_quantity_percent>50&&(rf_cal_en_flg)){
            rf_cal_en_flg=0;
#if MODE_BT_EN
            extern void bt_frontend_init_incharge();
            bt_frontend_init_incharge();
#endif
        }
    }
#endif
}

AT(.mode_charge_seg)
void mode_charge(void)
{
    logi("charge mode.");
    mode_charge_enter();
    mode_charge_loop();
    mode_charge_exit();
}

AT(.mode_charge_seg)
void mode_charge_enter(void)
{
    //sleep_prevent_set(PS_APP_PROCESS);
    disp_state(STATE_CHARGEING);
    if (pmu_is_use_hsw()) {
        pmu_hsw_reset_enable(false);
    }
}

AT(.mode_charge_seg)
void mode_charge_loop(void)
{
    uint32_t event;
    while(scene_mode.sta == MODE_CHARGE) {
        charge_rfcal();
        event = event_get(true);
        charge_event_process(event);
    }
}

AT(.mode_charge_seg)
void mode_charge_exit(void)
{
    if (pmu_is_use_hsw()) {
        pmu_hsw_reset_enable(true);
    }
    scene_mode.last_sta = MODE_CHARGE;
}
