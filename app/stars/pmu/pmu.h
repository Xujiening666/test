#ifndef _PMU_H_
#define _PMU_H_

/*
 * read 1 time per 400ms, total read 10 times, so update bat quantity percent 1 time per 4s
 * set period to 400ms can avoid vbat change suddenly when some load change, for example sdcard in/out
 *
 * when set BAT_QUANTITY_STEP_BIG to 1, will set the step to 25%, this case is for bluetooth Speaker, can save code space.
 * when set BAT_QUANTITY_STEP_BIG to 0, will set the step to 5%, this case is for bluetooth Headset, will use more code space.
 */
#define BAT_RECORD_TIME           10
#define BAT_RECORD_PERIOD         1000

#if BAT_QUANTITY_STEP_BIG
#define BAT_QUANTITY_STEP_PERCENT 25
#else
#define BAT_QUANTITY_STEP_PERCENT 10
#endif
#define CV_TABLE_LEN              ((100 / BAT_QUANTITY_STEP_PERCENT) + 1)

#if KEY_PMU_MULTFUC_EN
enum pmu_key_state
{
    KEY_NONE_STATE,
    KEY_PRESS,
    KEY_RELEASE,
 };
#endif

/* INTS */
#define PMU_INTS_SHORT_PRESS              (0x1 << 0)  //short press
#define PMU_INTS_LONE_PRESS               (0x1 << 1)  //long press
#define PMU_INTS_ULTRA_SHORT_PRESS        (0x1 << 2)  //ultra short press
#define PMU_INTS_VBUSIN                   (0x1 << 3)  //vbus in
#define PMU_INTS_VBUSOUT                  (0x1 << 4)  //vbus out
#define PMU_INTS_BATLOW                   (0x1 << 5)  //bat low
#define PMU_INTS_HSWOFF                   (0x1 << 6)  //hsw off
#define PMU_INTS_HSWON                    (0x1 << 7)  //hsw on
#define PMU_INTS_VBUSUV                   (0x1 << 8)  //vbus under voltage
#define PMU_INTS_BATFULL                  (0x1 << 9)  //bat full
#define PMU_INTS_BATTO                    (0x1 << 10) //bat charge timeout

void pmu_init(void);
void pmu_module_init(struct pmu_user_config config);
void pmu_handler(void);
void pmu_irq_proc(void);
bool pmu_is_vbus_pon(void);
bool pmu_is_vbat_pon(void);
bool pmu_is_use_hsw(void);
void pmu_sel_onoff_hsw(enum pmu_onoff_hsw_e sel);
bool pmu_is_enter_charge_mode(void);
void pmu_set_core_voltage(uint32_t val);
void pmu_task_init(void);
void pmu_task_deinit(void);
void pmu_bat_init(void);
uint8_t pmu_voltage_to_percent(uint32_t volt);
uint8_t pmu_get_bat_quantity_percent(void);
void pmu_charge_enable(bool enable);
bool pmu_bat_is_exist(void);
bool pmu_bat_is_low(void);
bool pmu_bat_is_charging(void);
void pmu_isr_bh_init(void);
void pmu_prohibit(bool prohibit);
void pmu_timer_cb(xTimerHandle xTimer);
bool pmu_nor_sec_can_erase(void);

#if KEY_PMU_EN
#if KEY_PMU_MULTFUC_EN
void pmu_key_process(uint8_t key_state);
#else
void pmu_key_process(void);
#endif
#else
#define pmu_key_process()
#endif
#endif

