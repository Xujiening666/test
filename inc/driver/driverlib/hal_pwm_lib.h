#ifndef _HAL_PWM_LIB_H_
#define _HAL_PWM_LIB_H_

#define PWM_CH_NUM  6

enum pwm_mode_e {
    PWM_MODE_CYCLE  = (0x0<<7),
    PWM_MODE_PULSE  = (0x1<<7),
};

enum pwm_active_state_e {
    PWM_ACTIVE_LOW  = (0x0<<5),
    PWM_ACTIVE_HIGH = (0x1<<5),
};

enum pwm_ch_en_e {
    PWM_CH_DIS      = (0x0<<4),
    PWM_CH_EN       = (0x1<<4),
};

/*
 * 一级分频系数:
 * 24MHz时钟源经分频后得到一个更低时钟源，周期为t。
 */
enum pwm_prescal_e {
    PWM_PRESCAL_120 = (0x0<<0), //24MHz/120 = 200KHz (t=5us)
    PWM_PRESCAL_180 = (0x1<<0), //24MHz/180 = 133KHz (t=7.5us)
    PWM_PRESCAL_240 = (0x2<<0), //24MHz/240 = 100KHz (t=10us)
    PWM_PRESCAL_360 = (0x3<<0), //24MHz/360 = 66KHz (t=15us)
    PWM_PRESCAL_480 = (0x4<<0), //24MHz/480 = 50KHz (t=20us)
    PWM_PRESCAL_600 = (0x5<<0), //24MHz/600 = 40KHz (t=25us)
    PWM_PRESCAL_720 = (0x6<<0), //24MHz/720 = 33KHz (t=30us)
    PWM_PRESCAL_840 = (0x7<<0), //24MHz/840 = 28KHz (t=35us)
    PWM_PRESCAL_12K = (0x8<<0), //24MHz/12K = 2KHz (t=0.5ms)
    PWM_PRESCAL_24K = (0x9<<0), //24MHz/24K = 1KHz (t=1ms)
    PWM_PRESCAL_36K = (0xA<<0), //24MHz/36K = 666Hz (t=1.5ms)
    PWM_PRESCAL_48K = (0xB<<0), //24MHz/48K = 500Hz (t=2ms)
    PWM_PRESCAL_72K = (0xC<<0), //24MHz/72K = 333Hz (t=3ms)

    PWM_PRESCAL_1   = (0xF<<0), //24MHz/1 = 24MHzHz (t=0.04us)
    /* on fpga,the prescal is /1 */
};


typedef struct {
    uint32_t en;
    uint32_t mode;
    uint32_t active_state;
    uint32_t prescal;
} pwm_param_t;


void pwm_module_init(void);

/*
 * 周期：(entire_cycle + 1) * t
 * active state时间：(active_cycle + 1) * t
 * entire_cycle必须比active_cycle大
 */
void pwm_config(uint32_t ch_num, uint32_t entire_cycle, uint32_t active_cycle);
void pwm_pulse_start(uint32_t ch_num);

#endif
