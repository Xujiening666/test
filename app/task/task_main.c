#define LOG_LEV 5
#include "sys_inc.h"
#include "user_config.h"
#include "device.h"
#include "task_main.h"
#include "dual_mailbox.h"
#include "info_mem.h"
#include "audio_effects.h"
#include "linein_audio.h"
#include "usbaudio.h"
#include "pmu.h"
#include "bt_host_api.h"
#include "bt_api.h"
#include "timer.h"
#include "motor.h"
#include "spdifaudio.h"
#include "system.h"
#include "eq_process.h"
#include "uart.h"
#include "trng.h"


scene_mode_t scene_mode;
volatile mode_hook_t current_mode_hook;
volatile uint32_t mode_count;
volatile bool key_mode;
bool key_mode_exit;
knum_t knum;


#if SLEEP_EN
TimerHandle_t SleepTimerHandle;
#endif

#if CPU_LOAD_EN
TimerHandle_t CpuLoadTimerHandle;
#endif

#if AUTO_SHUNTDOWN
TimerHandle_t AutoShuntdownTimerHandle;
auto_shutdown_t *auto_shutdown_context;
#endif


AT(.mode_set_seg)
void var_init(void)
{
    knum.val = 0;
    knum.bcnt = 0;
    knum.tcnt = 0xFF;

#if (MODE_BT_EN || MODE_BTHID_EN)
    bt_profile_select = ((0x8000*BT_BLE_EN) | (0x60*BT_HID_EN)| (0x08*BT_PHONE_EN) | (0x20*BT_SPP_EN) | 0x07);
    rf_para_dft();
    bt_ll_init();
#endif
}

AT(.mode_set_seg)
void timer_create(void)
{
#if AUTO_SHUNTDOWN
	if (NULL == auto_shutdown_context) {
		auto_shutdown_context = libc_calloc(sizeof(*auto_shutdown_context));
		if (NULL == auto_shutdown_context) {
			loge("calloc auto_shutdown fail");
		}
	}

    if (AutoShuntdownTimerHandle == 0) {
		AutoShuntdownTimerHandle = xTimerCreate("AutoShuntdownTimerHandle", pdMS_TO_TICKS(AUTO_SHUNTDOWN_TIME), pdFALSE, (void *)1, auto_shutdown_cb);
	}
	if(pdPASS == xTimerStart(AutoShuntdownTimerHandle, 10)) {
		auto_shutdown_context->auto_shutdown_init = true;
		auto_shutdown_context->auto_shutdown_status = true;
	    auto_shutdown_context->shutdown_ing = false;
    } else {
        auto_shutdown_context->auto_shutdown_init = false;
		auto_shutdown_context->auto_shutdown_status = false;
		auto_shutdown_context->shutdown_ing = false;
        logw("AutoShuntdownTimer init fail");
    }
#endif

#if SLEEP_EN
    sleep_delay_set();
    if (SleepTimerHandle == 0) {
        SleepTimerHandle = xTimerCreate("SleepTimer", pdMS_TO_TICKS(1000), pdTRUE, (void *)1, sleep_timer_cb);
    }
    xTimerStart(SleepTimerHandle, 10);
#endif

#if CPU_LOAD_EN
    if (CpuLoadTimerHandle == 0) {
        CpuLoadTimerHandle = xTimerCreate("CpuLoadTimer", pdMS_TO_TICKS(CPU_LOAD_PERIOD), pdTRUE, (void *)1, cpu_load_timer_cb);
    }
    xTimerStart(CpuLoadTimerHandle, 10);
#endif
}

#if 1
AT(.mode_set_seg)
bool startup_is_first(void)
{
    bool ret;
    logi("arunboy startup_is_first");
    if (0x5A == rtc_memory_read(INFO_SYS_STARTUP)) {
        ret = false;
    } else {
        rtc_memory_write(INFO_SYS_STARTUP, 0x5A);
        ret = true;
    }

    return ret;
}
#endif


#define W4BYTE(p)       (*(uint32_t *)(p))


AT(.mode_set_seg)
void taskmain_init(void)
{
    event_init();
    var_init();
    info_mem_init();

    disp_init();
    key_init();
    timer0_init();
	//timer1_init();  //auto_shutdown_init
    timer_create();
    pmu_isr_bh_init();
    pmu_task_init();
    gpio_irq_en();

#if POWER_OFF_FIRST_EN
    bool startup_flag;
    startup_flag = startup_is_first();
#endif
    scene_mode.switch_reason = SW_INIT;
    scene_mode.last_sta = MODE_NULL;
    
#if MODE_USBDEV_EN
        if (pmu_is_enter_charge_mode() && (usb_dm_pin_get() == 1)) {
#else
        if (pmu_is_enter_charge_mode()) {
#endif

        mode_set_force(MODE_CHARGE);
        return;
    } 
        
#if POWER_OFF_FIRST_EN
    else if (startup_flag) {
        scene_mode.switch_reason = SW_STARTUP_FIRST;
        mode_set_force(MODE_POWEROFF);
    }
#endif
    else {
#if DISP_EN
        disp_menu(MENU_WELCOME, 0, 0);
        disp_state(STATE_POWERON);
#endif
        device_init();
        startup_audio_service();
        player_init();
#if AUDIO_KARAOKE_MODE
#if AUDIO_KARAOKE_START_WITH_REVERB
        on_set_default_reverb_profile(false, false);
#endif

#if AUDIO_KARAOKE_START_WITH_ECHO
        on_set_default_echo_profile(false, false);
#endif
#endif

#if EQ_LOAD_EN
        if (EqLoadTimerHandle == NULL) {
            EqLoadTimerHandle = xTimerCreate("EqLoadTimer", pdMS_TO_TICKS(EQ_LOAD_PERIOD), pdTRUE, (void *)1, eq_load_timer_cb);
        }
        xTimerStart(EqLoadTimerHandle, 10);
#endif
        //check_sn_id();
#if AUDIO_KARAOKE_MODE
        audio_service_set_karaoke_mixout_volume(100);
#if LINEIN_ALL_MODE_SUPPORT
        linein_audio_set_volume(100);
#endif
#if USB_AUDIO_ALL_MODE_SUPPORT
        usbaudio_playback_set_volume(100);
#endif
#endif
        audio_service_set_playback_volume(np.volume);
        audio_service_set_tone_volume(TONE_VOLUME);
        motor_init();


#if (TONE_EN && SUNCHIP_POWER_ON)
        play_tone(TONE_POWER_ON, true);
#else
        msleep(50);
#endif

#if MODE_BT_EN
#if AUDIO_KARAOKE_MODE
        /* Set mute before
         * saving the bt information(write flash MUST ENTER CRITICAL)
         */
        audio_serivce_fadeout_dac_aout(100);
        bt_frontend_init();
        bt_rf_power_force(BT_RFPOWER_FORCE_EN, BT_RFPOWER_FORCE_VAL);
        bt_rf_maxpower_set(BT_MAXPOWER_SET_EN, BT_MAXPOWER_SET_VAL);
        audio_serivce_fadein_dac_aout(100);
#else
        /*on A version, this function should be used before audio init */
        if (CHIP_VER_A == sys_get_ver()) {
            bt_frontend_init();
            bt_rf_power_force(BT_RFPOWER_FORCE_EN, BT_RFPOWER_FORCE_VAL);
            bt_rf_maxpower_set(BT_MAXPOWER_SET_EN, BT_MAXPOWER_SET_VAL);
        }
#endif
#endif
#if maikefeng
		pin_config(LED_MAIKEFENG | PIN_FUNCOUTPUT | PIN_LOW | PIN_DRI0);
#endif
		/*pin_config(PIN_B11|PIN_FUNCOUTPUT|PIN_LOW);
		msleep(10);
		pin_config(PIN_B11|PIN_FUNCOUTPUT|PIN_HIGH);*/
#if MODE_USBDEV_EN
        if (usb_pc_is_connect() && !EQ_ALL_MODE_SUPPORT && !USB_AUDIO_ALL_MODE_SUPPORT) {
            mode_set_force(MODE_USBDEV);
        } else

#endif
        {
            if (device_is_online(DEV_SDCARD) || device_is_online(DEV_UDISK)) {
#if MODE_DISKUPDATE_EN
                mode_set_force(MODE_DISKUPDATE);
#else
                mode_set_force(MODE_RECORD);
#endif
            } else if (pmu_is_use_hsw() && PMU_HSW_STATE_OFF == pmu_get_hsw_state()) {
                scene_mode.last_sta = MODE_FM;
                scene_mode.switch_reason = SW_HSW;
                mode_set_force(MODE_POWEROFF);
            } else {
                mode_set_force(MODE_BT);
            }
        }
    }

    dev.startup = 1;

    uart_dma_mode_init();
	
#if BT_BLE_EN	
	task_main_ble();
#endif

#if BT_BACKGROUND_EN
#if !AUDIO_KARAOKE_MODE
    if (CHIP_VER_A != sys_get_ver()) {
        bt_frontend_init();
        bt_rf_power_force(BT_RFPOWER_FORCE_EN, BT_RFPOWER_FORCE_VAL);
        bt_rf_maxpower_set(BT_MAXPOWER_SET_EN, BT_MAXPOWER_SET_VAL);
    }
#endif
    bt_task_create();
#if BT_BLE_EN
    if(bt_get_profile()&0x8000){
        ble_init();
        ble_app_init();
    }
#endif
#if BT_SPP_EN
    if(bt_get_profile()&0x10){
        spp_app_init();
    }
#endif
#endif

#if DEBUG_DUMP_TASK_EN
    task_dump_init();
#endif
}		

AT(.mode_set_seg)
void task_main(void *pvParameters)
{
    logi("enter task main.");
    taskmain_init();	

    while(1) {
        if (current_mode_hook) {
            current_mode_hook();
        }
    }
}

AT(.mode_set_const_seg)
mode_info_t mode_list[] = {
#if MODE_RECORD_EN
    {MODE_RECORD, mode_record},
#endif

#if MODE_MUSIC_EN
    {MODE_MUSIC, mode_music},
#endif

#if MODE_BT_EN
    {MODE_BT, mode_bt},
#endif

#if MODE_FM_EN
    {MODE_FM, mode_fm},
#endif

#if MODE_LINEIN_EN
    {MODE_LINEIN, mode_linein},
#endif

#if MODE_CLOCK_EN
    {MODE_CLOCK, mode_clock},
#endif

#if MODE_USBDEV_EN
    {MODE_USBDEV, mode_usbdev},
#endif

#if MODE_SPDIF_EN
    {MODE_SPDIF, mode_spdif},
#endif

    //NOTE:normal scene mode will insert in before the MODE_SCENE_MAX
    {MODE_SCENE_MAX, NULL},

#if MODE_CHARGE_EN
    {MODE_CHARGE, mode_charge},
#endif

#if MODE_POWEROFF_EN
    {MODE_POWEROFF, mode_poweroff},
#endif

#if MODE_DISKUPDATE_EN
    {MODE_DISKUPDATE, mode_diskupdate},
#endif

    //NOTE:system scene mode will insert in bettwin the MODE_SCENE_MAX and the MODE_SYSTEM_MAX
    {MODE_SYSTEM_MAX, NULL},
};

#define MODE_TOTAL_NUM  (sizeof(mode_list)/sizeof(mode_list[0]))

AT(.event_seg)
void mode_update(int value)
{
    uint32_t index;

#if MODE_BT_EN
    if ((scene_mode.sta == MODE_BT) && ((bt_mode.status == BT_STATUS_INCOMING) ||
        (bt_mode.status == BT_STATUS_OUTGOING) ||
        (bt_mode.status == BT_STATUS_CALLING))) {
        return;
    }
#endif

    mode_count += value;
    index = mode_count % MODE_TOTAL_NUM;
    if (mode_list[index].mode >= MODE_SCENE_MAX) {
        mode_count = 0;
    }

    logv("update mode:%d", mode_count);
}

/*
 * if the current scene is bt calling, don't mode switch.
 */
AT(.mode_set_seg)
void mode_set(uint32_t mode)
{
    uint8_t index;

#if MODE_BT_EN
    if ((scene_mode.sta == MODE_BT) && ((bt_mode.status == BT_STATUS_INCOMING) ||
        (bt_mode.status == BT_STATUS_OUTGOING) ||
        (bt_mode.status == BT_STATUS_CALLING))) {
        return;
    }
#endif

    for (index = 0; index < MODE_TOTAL_NUM; index++) {
        if (mode_list[index].mode == mode)
            break;
    }
    if (index != MODE_TOTAL_NUM) {
        mode_count = index;
        logd("set mode:%d", mode_count);
    }

    mode_switch();
}

/*
 * don't care the current scene, switch to the target mode.
 */
AT(.mode_set_seg)
void mode_set_force(uint32_t mode)
{
    uint8_t index;

    taskENTER_CRITICAL();
    for (index = 0; index < MODE_TOTAL_NUM; index++) {
        if (mode_list[index].mode == mode)
            break;
    }
    if (index != MODE_TOTAL_NUM) {
        mode_count = index;
        logd("set mode:%d", mode_count);
    }

    mode_switch();
    taskEXIT_CRITICAL();
}

/*
 * switch to the target mode.
 */
AT(.mode_set_seg)
void mode_switch(void)
{
    uint32_t index;

    index = mode_count % MODE_TOTAL_NUM;
    logv("mode index:%d", index);

    scene_mode.sta = mode_list[index].mode;
    current_mode_hook = mode_list[index].hook;

#if BT_BACKGROUND_EN
    bt_bg_preprocess();
#endif
}

/*
 * switch to the next mode.
 */
AT(.mode_set_seg)
void mode_switch_auto(void)
{
    uint32_t index;
    index = mode_count % MODE_TOTAL_NUM;
    if ((scene_mode.sta < MODE_SCENE_MAX)
        && (scene_mode.sta != mode_list[index].mode)) {
        return;
    }

    mode_update(1);
    mode_switch();
    scene_mode.switch_reason = SW_AUTO;
}

/*
 * get the current scene mode, through 'mode_count'.
 */
AT(.mode_set_seg)
enum mode_id_e mode_get_sta(void)
{
    return mode_list[mode_count % MODE_TOTAL_NUM].mode;
}
