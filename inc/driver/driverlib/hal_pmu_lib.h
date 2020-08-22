#ifndef _HAL_PMU_LIB_H_
#define _HAL_PMU_LIB_H_

#ifdef __cplusplus
 extern "C" {
#endif


enum pmu_set {
    PMU_SET_DIS,
    PMU_SET_EN,
    PMU_SET_KEEP,
};

enum pmu_int_triger_mode {
    PMU_INT_FALLING_EDGE,
    PMU_INT_RISING_EDGE,
};

enum pmu_onoff_hsw_e {
    PMU_ONOFF,
    PMU_HSW,
};

enum pmu_onoff_state {
    PMU_ONOFF_DW,
    PMU_ONOFF_UP,
};

enum pmu_hsw_state {
    PMU_HSW_STATE_OFF,
    PMU_HSW_STATE_ON,
};

enum pmu_pwron_reason {
    PMU_VBUS_PON,
    PMU_ONOFFL_PON,
    PMU_ONOFFS_PON,
    PMU_ONOFFUS_PON,
    PMU_HSW_PON,
    PMU_ONOFFLRST_PON,
    PMU_RESTART_PON,
    PMU_SYSIN_PON,
};

enum pmu_pwroff_reason {
    PMU_SW_EN_POFF,
    PMU_ONOFFRST_POFF,
    PMU_RESTART_POFF,
    PMU_LB_PON,
    PMU_DCUV_PON,
    PMU_PWROK_PON,
    PMU_LDOUV_PON,
    PMU_VBUSOV_PON,
};

enum pmu_battery_charging_state {
    PMU_BAT_NO_CHARGE,
    PMU_BAT_CHARGING,
};

enum pmu_battery_exist_state {
    PMU_BAT_NON_EXIST,
    PMU_BAT_EXIST,
};

enum pmu_regu_state {
    PMU_REGU_GOOD,
    PMU_REGU_NOGOOD,
};

enum pmu_regu_mode {
    PMU_REGU_NORMAL,
    PMU_REGU_ALWAYSON,
};

enum pmu_regu_e {
    PMU_DCDC1,                // 0 VCC_CORE
    PMU_DCDC_MAX = PMU_DCDC1, // 0
    PMU_LDO1,                 // 1 VCC_RTC
    PMU_LDO2,                 // 2 VCC_IO
    PMU_LDO3,                 // 3 AVCC
    PMU_LDO4,                 // 4 1.2V reserved
    PMU_LDO_MAX = PMU_LDO4,   //4
    PMU_VCC_CORE_DCDC = PMU_DCDC1,
    PMU_VCC_RTC = PMU_LDO1,
    PMU_VCC_IO = PMU_LDO2,
    PMU_AVCC = PMU_LDO3,
    PMU_VCC_CORE_LDO = PMU_LDO4,
};

enum pmu_regu_cmd_e {
    PMU_REGU_ENABLE,
    PMU_REGU_DISABLE,
    PMU_REGU_SET_VOLTAGE,
    PMU_REGU_GET_VOLTAGE,
};

enum pmu_state_cmd_e {
    PMU_ENTER_POFF_MODE,
    PMU_ENTER_LP_MODE,
    PMU_ENTER_CAL_MODE,
    PMU_RESTART,
    PMU_RESTART_NO_WD,
};

enum pmu_time_set_e {
    PMU_POFF_KEEP_TIME,
    PMU_ONOFFUL_TIME,
    PMU_ONOFFL_TIME,
};

enum pmu_protect_cmd_e {
    PMU_PRO_PWROK,
    PMU_PRO_VBUS_OV_SHUT,
    PMU_PRO_VBUS_OV_DET,
    PMU_PRO_VBUS_UV_VOLT,
    PMU_PRO_VBUSID_WK_VOLT,
    PMU_PRO_VBUS_5KPD,
    PMU_PRO_BAT_LB_SHUT,
    PMU_PRO_BAT_SHUT_VOLT,
    PMU_PRO_BAT_OK_VOLT,
    PMU_PRO_BAT_LB_VOLT,
};

enum pmu_wakeupsrc_e {
    PMU_HSW_WAKEUPSRC = 1 << 0,
    PMU_ONOFFL_WAKEUPSRC = 1 << 1,
    PMU_SYSIN_WAKEUPSRC = 1 << 2,
    PMU_ONOFFUL_RESET_EN = 1 << 3,
    PMU_ONOFFUS_WAKEUPSRC = 1 << 4,
    PMU_VBUS_WAKEUPSRC = 1 << 5,
    PMU_ONOFFS_WAKEUPSRC = 1 << 6,
    PMU_RESTART_EN = 1 << 7,
};

enum pmu_intsrc_e {
    PMU_ONOFFS_INTSRC = 1 << 0,
    PMU_ONOFFL_INTSRC = 1 << 1,
    PMU_ONOFFUS_INTSRC = 1 << 2,
    PMU_VBUSIN_INTSRC = 1 << 3,
    PMU_VBUSOUT_INTSRC = 1 << 4,
    PMU_LB_INTSRC = 1 << 5,
    PMU_HSWOFF_INTSRC = 1 << 6,
    PMU_HSWON_INTSRC = 1 << 7,
    PMU_VBUSUV_INTSRC = 1 << 8,
    PMU_BATFULL_INTSRC = 1 << 9,
    PMU_BATTO_INTSRC = 1 << 10,
};

enum inpmu_wakeupintsrc_e {
    INPMU_BT_WAKEUPSRC = 1 << 8,
    INPMU_ADCKEY_WAKEUPSRC = 1 << 9,
    INPMU_SEC_WAKEUPSRC = 1 << 10,
    INPMU_TKC_WAKEUPSRC = 1 << 11,
    INPMU_ALARM_WAKEUPSRC = 1 << 12,
    INPMU_GPIOB_WAKEUPSRC = 1 << 13,
    INPMU_GPIOC_WAKEUPSRC = 1 << 14,
    INPMU_IRRX_WAKEUPSRC = 1 << 15,
    INPMU_BT_INTSRC = 1 << 16,
    INPMU_ADCKEY_INTSRC = 1 << 17,
    INPMU_SEC_INTSRC = 1 << 18,
    INPMU_TKC_INTSRC = 1 << 19,
    INPMU_ALARM_INTSRC = 1 << 20,
    INPMU_GPIOB_INTSRC = 1 << 21,
    INPMU_GPIOC_INTSRC = 1 << 22,
    INPMU_IRRX_INTSRC = 1 << 23,
};

typedef struct {
    bool low_power_enable;
    bool dcdc_dec_volt_enable;
    uint16_t slave_addr;
    uint16_t dcdc_dec_volt;
    uint8_t irqno;
    enum pmu_regu_e rc_cal_regu;
} PMU_InitTypeDef;

struct pmu_user_config {
    bool core_use_dcdc;
    bool poweroff_current;
    bool vcc_rtc_alwayson;
    uint16_t vccio_volt;
    uint16_t avcc_volt;
};

struct pmu_battery_info {
    bool exist;             /* see @pmu_battery_exist_state */
    bool charging;          /* see @pmu_battery_charging_state */
    bool chgvolt_4185mv;    /* is charge voltage arrive 4185mV */
    uint16_t vbat;          /* bat voltage */
    bool bat_low;           /* bat low power */
};

struct pmu_battery_cfg {
    bool charge_en;             /* enable bat charge  */
    bool vbat_adc_en;           /* enable vbat adc  */
    bool charge_time_en;        /* enable bat charge full count time */
    bool mcharge_min_en;        /* enable min current charging after main charge full */
    bool vilp_en;               /* input under voltage cycle enable */
    uint32_t sub_charge_volt;   /* sub charge constant voltage setting(mV) */
    uint32_t sub_charge_curt;   /* sub charge constant current setting(mA)  */
    uint32_t main_charge_curt;  /* main charge constant current setting(mA) */
    uint32_t avg_lpf_num;       /* average filter num */
    uint32_t r_vil;             /* vout under voltage cycle setting(mV) */
    uint32_t iref_adj_bias;     /* IREF current output bias */
};

uint8_t pmu_read(uint8_t addr);
void pmu_write(uint8_t addr, uint8_t data);
void pmu_set_poweroff_cur(uint8_t poweroff_current);
void pmu_set_regu_tlen(enum pmu_regu_e regu, bool enable);
void pmu_set_ldo1_mode(enum pmu_regu_mode mode);
void pmu_set_voltage(enum pmu_regu_e regu, uint32_t val);
void pmu_regu_enable(enum pmu_regu_e regu, bool enable);
void pmu_set_core_voltage(uint32_t val);
void pmu_set_bat_cfg(struct pmu_battery_cfg *cfg);
void pmu_set_time(enum pmu_time_set_e time_type, uint32_t ms);
void pmu_set_protect(enum pmu_protect_cmd_e cmd, bool enable, uint32_t data);
void pmu_get_bat_info(struct pmu_battery_info *info);
void pmu_get_bat_volt(struct pmu_battery_info *info);
void pmu_set_charge(struct pmu_battery_info *info, bool enable);
void pmu_set_poweron_wakeupsrc(enum pmu_wakeupsrc_e wakeup_src, bool enable);
void pmu_set_wakeup(bool enable);
void pmu_set_int(enum pmu_intsrc_e int_src, bool enable);
bool pmu_get_int(enum pmu_intsrc_e int_src);
bool pmu_is_low_bat_direct(void);
bool pmu_is_use_hsw(void);
void pmu_sel_onoff_hsw(enum pmu_onoff_hsw_e sel);
void pmu_hsw_reset_enable(bool enable);
bool pmu_is_charging(void);
void pmu_lowpower_set(bool enable);
void pmu_clear_all_pending(void);
void pmu_send_startup_int_event(uint32_t ints);
void restart_pmu(void);

enum pmu_hsw_state pmu_get_hsw_state(void);
enum pmu_onoff_state pmu_get_onoff_state(void);

#ifdef __cplusplus
}
#endif

#endif


