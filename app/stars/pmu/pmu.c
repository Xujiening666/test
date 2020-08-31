#define LOG_LEV 5
#include "sys_inc.h"
#include "user_config.h"
#include "gpio.h"
#include "pmu.h"
#include "key.h"
#include "task_main.h"
#include "bt_host_api.h"
#include "ble_app.h"

//general variable
QueueHandle_t pmu_mutex = NULL;
TaskHandle_t pmu_isr_bh_handler;
void pmu_isr_bh(void *pvParameters);

//pmu key variable
#if KEY_PMU_EN
#if KEY_PMU_MULTFUC_EN
static uint8_t key_num = 0;
#define LONG_PRESS_SECONDS ((uint32_t)1e6)
#define TIME_OUT_TIMES 5
TimerHandle_t PmuLongKeyWarningTimerHandle;
TimerHandle_t PmuKeyDoubleTimerHandle;
TimerHandle_t PmuKeyTripleTimerHandle;
static uint64_t key_ts;
#else
TimerHandle_t PmuKeyTimerHandle;
static bool first_key_flag = false;
#endif
#endif

//bat variable
/*
 * coulomp: battery quantity
 * voltage: battery voltage
 */
#if BAT_EXIST
#if BAT_QUANTITY_STEP_BIG
AT(.pmu_rodata_seg)
const uint16_t coulomp_voltage_table[CV_TABLE_LEN] = {
    3300, //step0    0%:(0000~3300]
    3728, //step1   25%:(3300~3728]
    3809, //step2   50%:(3728~3809]
    3952, //step3   75%:(3809~3952]
    4200, //step4  100%:(3900~4200]
};
#else
AT(.pmu_rodata_seg)
const uint16_t coulomp_voltage_table[CV_TABLE_LEN] = {
    3300, //step0    0%:(0000~3300]
    3500, //step1   10%:(3300~3500]  200
    3618, //step2   20%:(3500~3618]  118
    3739, //step3   30%:(3618~3739]  121
    3770, //step4   40%:(3739~3770]  31 
    3809, //step5   50%:(3770~3809]  39
    3856, //step6   60%:(3809~3856]  47
    3916, //step7   70%:(3856~3916]  60
    3989, //step8   80%:(3916~3989]  73
    4098, //step9   90%:(3989~4098]  109
    4200, //step10 100%:(4098~4200]  102
};
#endif

struct pmu_battery_info bat_info;
struct pmu_battery_cfg bat_cfg;
uint32_t bat_quantity_average;
uint8_t bat_quantity_percent = 0;
uint8_t bat_quantity_percent_old = 0;
uint8_t bat_quantity_record_cnt = 0;
uint16_t bat_quantity_buf[BAT_RECORD_TIME];
uint16_t bat_low_warning_cnt = BAT_SW_LB_WARNING_PERIOD/BAT_RECORD_PERIOD - 1;

#if !RTC_EN
TimerHandle_t PmuTimerHandle;
#endif

#endif

#if BAT_EXIST
AT(.pmu_bat_flash_seg)
uint8_t pmu_voltage_to_percent(uint32_t volt)
{
    uint8_t i, percent;

    for (i = 0; i < CV_TABLE_LEN; i++) {
        if (volt <= coulomp_voltage_table[i])
            break;
    }

    percent = BAT_QUANTITY_STEP_PERCENT * i;
    if (percent > 100) {
        percent = 100;
    }
    logv("step:%u, per:%u", i, percent);

    return percent;
}

AT(.pmu_bat_flash_seg)
uint8_t pmu_get_bat_quantity_percent(void)
{
#if BAT_EXIST
    return bat_quantity_percent;
#else
    return 0;
#endif
}

/*
 * buf: array buffer pointer for find max and min
 * start: array buffer start index
 * end: array buffer end index
 * max: pointer of the max value
 * min: pointer of the min value
 */
AT(.pmu_bat_flash_seg)
void maxmin(uint16_t *buf, uint16_t start, uint16_t end, uint16_t *max, uint16_t *min)
{
    uint16_t m, _max1, _min1, _max2, _min2;

    if (start == end) {
       *max = buf[start];
       *min = buf[end];
    } else {
       m = (start + end) >> 1;
       maxmin(buf, start, m, &_max1, &_min1);
       maxmin(buf, m + 1, end, &_max2, &_min2);
       *max = _max1 > _max2 ? _max1 : _max2;
       *min = _min1 > _min2 ? _min2 : _min1;
    }
}

AT(.pmu_bat_flash_seg)
void pmu_timer_cb(xTimerHandle xTimer)
{
    uint16_t vbat_max, vbat_min;
    uint32_t i, bat_quantity_total = 0;
#if ((PMU_EXIST == 0) || (PMU_VERSION == 0))
    bat_info.vbat = 4200;
    bat_info.exist = true;
#else
    pmu_get_bat_volt(&bat_info);
#endif
    bat_quantity_buf[bat_quantity_record_cnt] = bat_info.vbat;

    maxmin(bat_quantity_buf, 0, BAT_RECORD_TIME - 1, &vbat_max, &vbat_min);
    for (i = 0; i < BAT_RECORD_TIME; i++) {
        bat_quantity_total += bat_quantity_buf[i];
    }
    bat_quantity_total = bat_quantity_total - vbat_max - vbat_min;
    bat_quantity_average = bat_quantity_total / (BAT_RECORD_TIME - 2);
    bat_quantity_percent = pmu_voltage_to_percent(bat_quantity_average);
    bat_quantity_record_cnt++;
    if (bat_quantity_record_cnt == BAT_RECORD_TIME) {
        bat_quantity_record_cnt = 0;
    }
    logv("vbat:%u-%u-%u%%, max:%u, min:%u, low:%u, changind:%d", bat_info.vbat,
        bat_quantity_average, bat_quantity_percent, vbat_max, vbat_min, bat_info.bat_low, bat_info.charging);

    if (bat_quantity_average && (bat_quantity_average <= BAT_SW_LB_SHUT_VOLT)) {
        logi("lb sw shut");
        scene_mode.switch_reason = SW_LB;
        event_put(SYSTEM_EVENT_POWEROFF);
    } else if (bat_quantity_average && (bat_quantity_average <= BAT_SW_LB_WRN_VOLT)) {
        bat_info.bat_low = true;
        if (bat_low_warning_cnt == BAT_SW_LB_WARNING_PERIOD/BAT_RECORD_PERIOD) {
            bat_low_warning_cnt = 0;
            event_put(PMU_EVENT_BAT_LOW_WRN);
        }
        bat_low_warning_cnt++;
    } else if (bat_info.bat_low) {
        bat_info.bat_low = false;
        bat_low_warning_cnt = BAT_SW_LB_WARNING_PERIOD/BAT_RECORD_PERIOD - 1;
        logv("unlow:%u", bat_low_warning_cnt);
        event_put(PMU_EVENT_BAT_UNLOW);
    }
    disp_bat_low_digital(bat_info.bat_low);

    //update bt bat level
    if (bat_quantity_percent != bat_quantity_percent_old) {
#if MODE_BT_EN
        bt_send_message(BT_EVENT_BAT_UP);
#endif
        send_battery_message();
    }

    bat_quantity_percent_old = bat_quantity_percent;
#if AUTO_POWER_OFF
    if (poweroff_begin_time != 0xffffffff) {
        if ((rtc_get_second() - poweroff_begin_time) > AUTO_POWER_OFF_PERIOD) {
            logi("over 5min poweroff");
            if (pmu_is_use_hsw()) {
                event_put(PMU_EVENT_HSW_OFF);
            } else {
                scene_mode.switch_reason = SW_AUTO;
                event_put(SYSTEM_EVENT_POWEROFF);
            }
            poweroff_begin_time = 0xffffffff;
        }
    }
#endif
}
#endif

AT(.pmu_bat_flash_seg)
void pmu_charge_enable(bool enable)
{
#if BAT_EXIST
    pmu_set_charge(&bat_info, enable);
#endif
}

AT(.pmu_seg)
bool pmu_bat_is_exist(void)
{
#if BAT_EXIST
    return bat_info.exist;
#else
    return false;
#endif
}

//use in disp, must in sram
AT(.pmu_bat_flash_seg)
bool pmu_bat_is_low(void)
{
#if BAT_EXIST
    return bat_info.bat_low;
#else
    return false;
#endif
}

//use in disp, must in sram
AT(.pmu_bat_flash_seg)
bool pmu_bat_is_charging(void)
{
#if BAT_EXIST
    return pmu_is_charging();
#else
    return false;
#endif
}

AT(.pmu_bat_flash_seg)
void pmu_bat_init(void)
{
#if BAT_EXIST
    uint32_t i;

    bat_cfg.sub_charge_volt = 3300;
    bat_cfg.sub_charge_curt = 30;
    bat_cfg.main_charge_curt = PMU_CHARGE_CURRENT;
    bat_cfg.charge_time_en = PMU_SET_EN;
    bat_cfg.vbat_adc_en = PMU_SET_EN;
    bat_cfg.charge_en = PMU_SET_KEEP;
    bat_cfg.mcharge_min_en = PMU_SET_EN;
    bat_cfg.avg_lpf_num = 16; //16/32/64/128
    bat_cfg.vilp_en = PMU_SET_EN;
    bat_cfg.r_vil = 4600;
#if BAT_VOLT_DET
    bat_cfg.iref_adj_bias = 7;
#else
    bat_cfg.iref_adj_bias = 0;
#endif

#if PMU_VERSION
    //FIXME:pmu bug, can not open charge
    pmu_set_bat_cfg(&bat_cfg);
#endif

    pmu_set_protect(PMU_PRO_BAT_SHUT_VOLT, true, BAT_HW_LB_SHUT_VOLT);
    pmu_set_protect(PMU_PRO_BAT_OK_VOLT, true, BAT_HW_OK_VOLT);
#if BAT_HW_LB_SHUT_EN
    pmu_set_protect(PMU_PRO_BAT_LB_SHUT, true, 0);
#else
    pmu_set_protect(PMU_PRO_BAT_LB_SHUT, false, 0);
#endif

    //mdelay(5000);//FIXME
    pmu_get_bat_info(&bat_info);

#if ((PMU_EXIST == 0) || (PMU_VERSION == 0))
    bat_info.vbat = 4200;
#endif
    bat_info.exist = true;

    bat_quantity_average = bat_info.vbat;
    bat_quantity_percent = pmu_voltage_to_percent(bat_info.vbat);
    for (i = 0; i < BAT_RECORD_TIME; i++) {
        bat_quantity_buf[i] = bat_info.vbat;
    }
    logd("percent:%u%%", bat_quantity_percent);
    logd("exist:%u", bat_info.exist);
    logd("charging:%u", bat_info.charging);
    logd("vbat:%u", bat_info.vbat);
    logd("chgvolt_4185mv:%u", bat_info.chgvolt_4185mv);
#endif
}

AT(.pmu_seg)
bool pmu_is_enter_charge_mode(void)
{
#if BAT_EXIST
    return pmu_is_vbus_pon();
#else
    return false;
#endif
}

#if KEY_PMU_EN
#if KEY_PMU_MULTFUC_EN
AT(.pmu_flash_seg)
void pmu_long_key_warning_timer_cb(xTimerHandle xTimer)
{
    event_put(KEY_EVENT_H | KEY_SIRI);
}

AT(.pmu_flash_seg)
void pmu_double_key_timer_cb(xTimerHandle xTimer)
{
    key_num = 0;
    event_put(KEY_EVENT_SU | KEY_PLAY);
    logd("single short p/p");

}

AT(.pmu_flash_seg)
void pmu_triple_key_timer_cb(xTimerHandle xTimer)
{
    key_num = 0;
    event_put(KEY_EVENT_L | KEY_PLAY);
    logd("double short p/p");
}

AT(.pmu_seg)
void pmu_key_process(uint8_t key_state)
{
    //logd("key:%u", key_state);
    //logd("pmu_key_process");
    if (key_state == KEY_PRESS){
        xTimerStop(PmuLongKeyWarningTimerHandle, 10);
        xTimerStart(PmuLongKeyWarningTimerHandle, 10);
        key_ts = get_timestamp();
    }else if (key_state == KEY_RELEASE) {
        xTimerStop(PmuLongKeyWarningTimerHandle, 10);
        key_ts = get_timestamp() - key_ts;
        if (key_ts >= LONG_PRESS_SECONDS) {
            key_num = 0;
            event_put(KEY_EVENT_L | KEY_PLAY);
            logd("single long p/p");
        } else {
            key_num++;
            if (key_num == 1) {
                xTimerStart(PmuKeyDoubleTimerHandle, 10);
            } else if (key_num == 2) {
                xTimerStop(PmuKeyDoubleTimerHandle, 10);
                xTimerStart(PmuKeyTripleTimerHandle, 10);
            } else if (key_num == 3) {
                key_num = 0;
                xTimerStop(PmuKeyTripleTimerHandle, 10);
                mode_update(1);
                event_put(KEY_EVENT_SU | KEY_MODE);
                key_mode = true;
                logd("triple short p/p");
            }
        }
    }
    //logd("key_num:%u", key_num);
}

#else
AT(.pmu_seg)
void pmu_key_timer_cb(xTimerHandle xTimer)
{
    event_put(KEY_EVENT_SU | KEY_PLAY);
    first_key_flag = false;
    logd("singal p/p");
}

AT(.pmu_seg)
void pmu_key_process(void)
{
    if (first_key_flag == false) {
        first_key_flag = true;
        xTimerStart(PmuKeyTimerHandle, 10);
    } else {
        xTimerStop(PmuKeyTimerHandle, 10);
        event_put(KEY_EVENT_L | KEY_PLAY);
        first_key_flag = false;
        logd("double p/p");
    }
}
#endif
#endif

AT(.pmu_init_seg)
void pmu_task_init(void)
{
    if (pmu_mutex == NULL) {
        pmu_mutex = xSemaphoreCreateMutex();
        if (pmu_mutex == NULL) {
            loge("creat pmu_mutex fail");
        }
    }

#if BAT_EXIST
    pmu_get_bat_info(&bat_info);
    bat_quantity_average = bat_info.vbat;
    bat_quantity_percent = pmu_voltage_to_percent(bat_info.vbat);
#if !RTC_EN
    //creat timer for battery management
    if (PmuTimerHandle == 0) {
        PmuTimerHandle = xTimerCreate("PmuTimer", pdMS_TO_TICKS(BAT_RECORD_PERIOD), pdTRUE, (void *)1, pmu_timer_cb);
    }
    if (PmuTimerHandle != NULL) {
        //FIXME:pmu bug, can not use charge
        xTimerStart(PmuTimerHandle, 10);
    }
#endif
#endif

#if KEY_PMU_EN
#if KEY_PMU_MULTFUC_EN
    if (PmuLongKeyWarningTimerHandle == NULL){
        PmuLongKeyWarningTimerHandle = xTimerCreate("PmuLongKeyWarningTimer", pdMS_TO_TICKS(2000), pdFALSE, (void *)1, pmu_long_key_warning_timer_cb);
    }

    if (PmuKeyDoubleTimerHandle == NULL){
        PmuKeyDoubleTimerHandle = xTimerCreate("PmuDoubleKeyTimer", pdMS_TO_TICKS(500), pdFALSE, (void *)1, pmu_double_key_timer_cb);
    }

    if (PmuKeyTripleTimerHandle == NULL){
        PmuKeyTripleTimerHandle = xTimerCreate("PmuTripleKeyTimer", pdMS_TO_TICKS(500), pdFALSE, (void *)1, pmu_triple_key_timer_cb);
    }
#else
    if (PmuKeyTimerHandle == NULL) {
        PmuKeyTimerHandle = xTimerCreate("PmuKeyTimer", pdMS_TO_TICKS(500), pdFALSE, (void *)1, pmu_key_timer_cb);
    }
#endif
#endif
}

//it use in update code,so it must be put in com_sys section
AT(.pmu_seg)
void pmu_task_deinit(void)
{
#if BAT_EXIST
#if !RTC_EN
    if (PmuTimerHandle != NULL) {
        xTimerStop(PmuTimerHandle, 10);
    }
#endif
#endif

#if KEY_PMU_EN
#if !KEY_PMU_MULTFUC_EN
    if (PmuKeyTimerHandle != NULL) {
        xTimerStop(PmuKeyTimerHandle, 10);
    }
#endif
#endif

    pmu_mutex = NULL;
}

AT(.pmu_init_seg)
void pmu_init(void)
{
    uint32_t int_bits;
    struct pmu_user_config config;

    config.core_use_dcdc = PMU_CORE_USE_DCDC;
    config.vcc_rtc_alwayson = PMU_VCC_RTC_ALWAYS_ON;
    config.vccio_volt = PMU_VCC_IO_VOLT;
    config.avcc_volt = PMU_AVCC_VOLT;
#if BAT_EXIST
    config.poweroff_current = 0;
#else
    config.poweroff_current = 2;
#endif
    pmu_module_init(config);

    if (pmu_is_use_hsw()) {
        pmu_hsw_reset_enable(true);
    }
    pmu_set_int(0xffff, false); //disable all firstly

#if !PMU_VBUS_POWERON_EN
    pmu_set_poweron_wakeupsrc(PMU_VBUS_WAKEUPSRC, false);
#endif

#if PMU_SHORT_PRESS_POWERON_EN
    pmu_set_poweron_wakeupsrc(PMU_ONOFFS_WAKEUPSRC, true);
#endif
#if PMU_ULTRA_SHORT_PRESS_POWERON_EN
    pmu_set_poweron_wakeupsrc(PMU_ONOFFUS_WAKEUPSRC, true);
#endif

    int_bits = PMU_VBUSIN_INTSRC | PMU_VBUSOUT_INTSRC;
#if BAT_EXIST
        int_bits |= PMU_BATFULL_INTSRC | PMU_BATTO_INTSRC;
#endif

    if (pmu_is_use_hsw()) {
        int_bits |= PMU_HSWOFF_INTSRC | PMU_HSWON_INTSRC;
    } else {
        int_bits |= PMU_ONOFFUS_INTSRC | PMU_ONOFFS_INTSRC | PMU_ONOFFL_INTSRC;
        pmu_set_time(PMU_ONOFFL_TIME, PMU_LONG_PRESS_TIME);
        pmu_set_time(PMU_ONOFFUL_TIME, PMU_ULTRA_LONG_PRESS_TIME);
    }
    pmu_set_int(int_bits, true);

#if PMU_POWEROFF_WHEN_BAT_FIRST_IN
    if (pmu_is_vbat_pon()) {
        logd("poweroff after vbat in poweron");
        pmu_set_poweron_wakeupsrc(PMU_VBUS_WAKEUPSRC, false);
        enter_poweroff_mode();
    }
#endif
    pmu_set_protect(PMU_PRO_BAT_LB_VOLT, false, PMU_NOR_SEC_ERASE_PRO_VOLT);
    pmu_bat_init();
#if SLEEP_EN
    rtc_prcm_init();
#endif
    pmu_lowpower_set(true);
    pmu_write(0x13, 0x38);
    pmu_write(0x15, 0x23);

}

//use in dvfs
AT(.pmu_seg)
void pmu_set_core_voltage(uint32_t val)
{
    if (val < 1100) {
        clk_set_sram_speed(SRAM_NORMAL);
    }

#if PMU_CORE_USE_DCDC
    pmu_set_voltage(PMU_VCC_CORE_DCDC, val);
#else
    /* beacuse the ldo4 step is 100mV, the dcdc1 step is 20mV,
     * upround the voltage.
     */
    uint32_t temp = ((val + 100 - 1) / 100) * 100;
    pmu_set_voltage(PMU_VCC_CORE_LDO, temp);
#endif

    if (val >= 1100) {
        clk_set_sram_speed(SRAM_HIGH);
    }
}

AT(.pmu_sram_seg)
void pmu_lock(void)
{
    if (pmu_mutex == NULL) {
        return;
    }

    if (inHandlerMode()) {
        loge("rtwi op in isr");
        return;
    }

    xSemaphoreTake(pmu_mutex, portMAX_DELAY);
}

AT(.pmu_sram_seg)
void pmu_unlock(void)
{
    if (pmu_mutex == NULL) {
        return;
    }

    if (inHandlerMode()) {
        loge("rtwi op in isr");
        return;
    }

    xSemaphoreGive(pmu_mutex);
}

AT(.pmu_seg)
void pmu_isr_bh(void *pvParameters)
{
    while (1) {
        xTaskNotifyWait(0x0, 0xffffffff, NULL, portMAX_DELAY);
        pmu_handler();
    }
}

AT(.pmu_init_seg)
void pmu_isr_bh_init(void)
{
    xTaskCreate(pmu_isr_bh, "pmu_isr_bh",
        PMU_ISR_BH_TASK_STACK_SIZE, NULL, PMU_ISR_BH_TASK_PRIO, &pmu_isr_bh_handler);

    pmu_send_startup_int_event(PMU_INTS_VBUSIN|PMU_INTS_VBUSOUT);
}

AT(.pmu_seg)
bool pmu_poweroff_is_hswoff(void)
{
    if (pmu_is_use_hsw() && scene_mode.switch_reason == SW_HSW) {
        return true;
    } else {
        return false;
    }
}

AT(.pmu_sram_seg)
bool pmu_nor_sec_can_erase(void)
{
#if PMU_NOR_SEC_ERASE_PRO_EN
    if (pmu_is_low_bat_direct()) {
        return false;
    } else {
        return true;
    }
#else
    return true;
#endif
}
