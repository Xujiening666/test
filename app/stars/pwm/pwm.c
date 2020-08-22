#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "pwm.h"


#if PWM_EN

extern pwm_param_t pwm_param[PWM_CH_NUM];

AT(.pwm_init_seg)
void pwm_init(void)
{
#if PWM_EN
    module_ccu_enable(CCU_PWM);

#if PWM_CH0_EN
    pin_config(PWM_CH0_PIN_CFG);
    pwm_param[PWM_CH0].en = PWM_CH_EN;
    pwm_param[PWM_CH0].mode = PWM_MODE_CYCLE;
    pwm_param[PWM_CH0].active_state = PWM_ACTIVE_HIGH;
    pwm_param[PWM_CH0].prescal= PWM_PRESCAL_24K;
#else
    pwm_param[PWM_CH0].en = PWM_CH_DIS;
#endif

#if PWM_CH1_EN
    pin_config(PWM_CH1_PIN_CFG);
    pwm_param[PWM_CH1].en = PWM_CH_EN;
    pwm_param[PWM_CH1].mode = PWM_MODE_CYCLE;
    pwm_param[PWM_CH1].active_state = PWM_ACTIVE_HIGH;
    pwm_param[PWM_CH1].prescal= PWM_PRESCAL_24K;
#else
    pwm_param[PWM_CH1].en = PWM_CH_DIS;
#endif

#if PWM_CH2_EN
    pin_config(PWM_CH2_PIN_CFG);
    pwm_param[PWM_CH2].en = PWM_CH_EN;
    pwm_param[PWM_CH2].mode = PWM_MODE_CYCLE;
    pwm_param[PWM_CH2].active_state = PWM_ACTIVE_HIGH;
    pwm_param[PWM_CH2].prescal= PWM_PRESCAL_24K;
#else
    pwm_param[PWM_CH2].en = PWM_CH_DIS;
#endif

#if PWM_CH3_EN
    pin_config(PWM_CH3_PIN_CFG);
    pwm_param[PWM_CH3].en = PWM_CH_EN;
    pwm_param[PWM_CH3].mode = PWM_MODE_CYCLE;
    pwm_param[PWM_CH3].active_state = PWM_ACTIVE_HIGH;
    pwm_param[PWM_CH3].prescal= PWM_PRESCAL_24K;
#else
    pwm_param[PWM_CH3].en = PWM_CH_DIS;
#endif

#if PWM_CH4_EN
    pin_config(PWM_CH4_PIN_CFG);
    pwm_param[PWM_CH4].en = PWM_CH_EN;
    pwm_param[PWM_CH4].mode = PWM_MODE_CYCLE;
    pwm_param[PWM_CH4].active_state = PWM_ACTIVE_HIGH;
    pwm_param[PWM_CH4].prescal= PWM_PRESCAL_24K;
#else
    pwm_param[PWM_CH4].en = PWM_CH_DIS;
#endif

#if PWM_CH5_EN
    pin_config(PWM_CH5_PIN_CFG);
    pwm_param[PWM_CH5].en = PWM_CH_EN;
    pwm_param[PWM_CH5].mode = PWM_MODE_CYCLE;
    pwm_param[PWM_CH5].active_state = PWM_ACTIVE_HIGH;
    pwm_param[PWM_CH5].prescal= PWM_PRESCAL_24K;
#else
    pwm_param[PWM_CH5].en = PWM_CH_DIS;
#endif

    pwm_module_init();
#endif
}


#define PWM_TEST_EN 0

#if PWM_TEST_EN
AT(.pwm_init_seg)
void pwm_test(void)
{
    pwm_config(PWM_CH0, 48000, 24000);

    logi("pwm test.");
    while(1);
}
#endif

#else
void pwm_init(void)
{
}

#endif

