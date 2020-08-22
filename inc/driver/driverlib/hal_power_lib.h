
#ifndef _HAL_POWER_LIB_H_
#define _HAL_POWER_LIB_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "sys_types.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

enum pm_prio_type {
    PM_ARCH = 0,          /* arch operation */
    PM_SYSTEM,            /* system operation */
    PM_LATE_DEVICE,       /* late device operation */
    PM_EARLY_DEVICE,      /* early device operation */
};

enum pm_notify_cmd_type {
    PM_SUSPEND_NOTIFY = 0,
    PM_RESUME_NOTIFY,
    PM_SHUTDOWN_NOTIFY,
};

enum power_supply_type {
    POWER_SUPPLY_UNKNOWN = 0,   /* unknown power supply */
    POWER_SUPPLY_BATTERY,       /* battery power supply */
    POWER_SUPPLY_USB,           /* USB power supply */
    POWER_SUPPLY_WIRELESS,      /* wireless power supply */
};

enum pm_prevent_sleep
{
    PS_AUDIO_PLAY   = 1 << 0,
    PS_AUDIO_RECORD = 1 << 1,
    PS_AUDIO_ANC    = 1 << 2,
    PS_BT_INCOMING  = 1 << 3,
    PS_BTC          = 1 << 4,
    PS_APP_PROCESS  = 1 << 5,
    PS_BTH          = 1 << 6,
};

/* Battery&Charger info */
struct power_supply_info {
    uint32_t battery_level;     /* level:0 ~ 100; 0:empty; 100:full */
    uint32_t type;              /* see @power_supply_type */
    uint32_t voltage;           /* voltage(uV) of power supply */
    uint32_t current;           /* current(uA) of power supply */
};

/*
 * Twait = cal_prd_base * (2 ^ rc_cal_prd) * 64 * 20us
 * Tcal  = rc_cal_num * 4 * 20us
 */
struct rc_cal_paras {
    uint32_t cal_prd_base;
    uint32_t rc_cal_prd;
    uint32_t rc_cal_num;
    bool continous_mode;
    bool enable;
};

void get_power_supply_info(uint32_t supply_id, struct power_supply_info *info);

void *get_regulator_obj(uint32_t id);

void regulator_enable(void *obj);
void regulator_disable(void *obj);

uint32_t regulator_get_voltage(void *obj);
void regulator_set_voltage(void *obj, uint32_t new_uV);

uint32_t regulator_get_current(void *obj);
void regulator_set_current(void *obj, uint32_t new_uA);
uint32_t btble_enter_sleep_mode(void);
uint32_t enter_poweroff_mode(void);
void watchdog_init(uint8_t sec);
void watchdog_deinit(void);
void watchdog_refresh(xTimerHandle xTimer);
void restart_system(uint8_t sec);
void power_test(void);
void set_rc_cal_paras(uint32_t cal_prd_base, uint32_t rc_cal_prd, uint32_t rc_cal_num, bool continous_mode, bool enable);
void set_rc_cal_reg(void);

void sleep_btble(void);
void sleep_bt(void);
void sleep(void);

void sleep_wakeup_enable(void);
void sleep_wakeup_disable(void);

void rtc_prcm_init(void);
extern void (*bt_rtc_prcm_process)(void);
void pm_prevent_sleep_set(enum pm_prevent_sleep prv_slp_bit);
void pm_prevent_sleep_clear(enum pm_prevent_sleep prv_slp_bit);
uint32_t pm_prevent_sleep_get(void);
uint8_t pm_prevent_sleep_delay_get(void);
void pm_prevent_sleep_delay_set(uint8_t sec);
void pm_prevent_sleep_delay_inc(uint8_t sec);
void pm_prevent_sleep_delay_dec(uint8_t sec);
void pm_prevent_sleep_delay_clear(void);

//user interfaces
void sleep_delay_set(void);
uint8_t sleep_delay_get(void);
void sleep_delay_inc(uint8_t sec);
void sleep_delay_dec(uint8_t sec);
void sleep_prevent_set(enum pm_prevent_sleep prevent);
void sleep_prevent_clear(enum pm_prevent_sleep prevent);
uint32_t sleep_prevent_get(void);

#ifdef __cplusplus
}
#endif

#endif /* _HAL_POWER_LIB_H_ */
