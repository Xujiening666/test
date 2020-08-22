#define LOG_LEV 0
#include "sys_inc.h"
#include "user_config.h"
#include "dio.h"

#if MOTOR_EN
static volatile uint8_t motor_cnt = 0;
static volatile uint8_t motor_startup = 1;
static volatile uint8_t motor_timer_en = 0;

AT(.motor_sram_seg)
void motor_onoff(bool onoff)
{
    if (onoff) {
        dio_set_value(DIO_ID1, DIO_VALUE_H);
    } else {
        dio_set_value(DIO_ID1, DIO_VALUE_L);
    }
    logd("onoff:%u", onoff);
}

AT(.motor_sram_seg)
void motor_timer_cb(void)
{
    if (motor_timer_en) {
        motor_cnt++;
        if (motor_startup == 1) {
            if (motor_cnt == 1) {
                motor_onoff(0);
            } else if (motor_cnt == 2) {
                motor_onoff(1);
            } else if (motor_cnt == 3) {
                motor_onoff(0);
                motor_timer_en = 0;
                motor_cnt = 0;
                motor_startup = 0;
            }
        } else {
            if (motor_cnt == 6) {
                logd("motor:3s");
                motor_onoff(1);
            } else if (motor_cnt == 8) {
                logd("motor:4s");
                motor_onoff(0);
                motor_cnt = 0;
            }
        }
    }
}

AT(.motor_flash_seg)
void motor_init(void)
{
    dio_set_func(DIO_ID1, DIO_FUNC_OUTPUT);
    dio_set_driving(DIO_ID1, DIO_DRI3);
    motor_onoff(1);
    motor_timer_en = 1;
}

AT(.motor_flash_seg)
void motor_callin(bool callin)
{
    if (callin) {
        if (motor_timer_en == 0) {
            motor_cnt = 6;
            motor_timer_en = 1;
            motor_onoff(1);
            logd("motor:en");
        }
    } else {
        if (motor_timer_en == 1) {
            motor_timer_en = 0;
            motor_cnt = 0;
            motor_onoff(0);
            logd("motor:dis");
        }
    }
}
#endif
