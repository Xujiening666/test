#ifndef _PWM_H_
#define _PWM_H_


#if (PWM_CH0_PIN_SEL == 0)
#define PWM_CH0_PIN_CFG     (PIN_A15 | PIN_FUNC3)
#elif (PWM_CH0_PIN_SEL == 1)
#define PWM_CH0_PIN_CFG     (PIN_B5 | PIN_FUNC3)
#elif (PWM_CH0_PIN_SEL == 2)
#define PWM_CH0_PIN_CFG     (PIN_B12 | PIN_FUNC10)
#endif

#if (PWM_CH1_PIN_SEL == 0)
#define PWM_CH1_PIN_CFG     (PIN_A16 | PIN_FUNC3)
#elif (PWM_CH1_PIN_SEL == 1)
#define PWM_CH1_PIN_CFG     (PIN_B6 | PIN_FUNC3)
#elif (PWM_CH1_PIN_SEL == 2)
#define PWM_CH1_PIN_CFG     (PIN_B13 | PIN_FUNC10)
#endif

#if (PWM_CH2_PIN_SEL == 0)
#define PWM_CH2_PIN_CFG     (PIN_A17 | PIN_FUNC3)
#elif (PWM_CH2_PIN_SEL == 1)
#define PWM_CH2_PIN_CFG     (PIN_B7 | PIN_FUNC10)
#endif

#if (PWM_CH3_PIN_SEL == 0)
#define PWM_CH3_PIN_CFG     (PIN_A18 | PIN_FUNC3)
#elif (PWM_CH3_PIN_SEL == 1)
#define PWM_CH3_PIN_CFG     (PIN_B8 | PIN_FUNC10)
#elif (PWM_CH3_PIN_SEL == 2)
#define PWM_CH3_PIN_CFG     (PIN_B10 | PIN_FUNC10)
#endif

#if (PWM_CH4_PIN_SEL == 0)
#define PWM_CH4_PIN_CFG     (PIN_A19 | PIN_FUNC3)
#elif (PWM_CH4_PIN_SEL == 1)
#define PWM_CH4_PIN_CFG     (PIN_B9 | PIN_FUNC10)
#elif (PWM_CH4_PIN_SEL == 2)
#define PWM_CH4_PIN_CFG     (PIN_B11 | PIN_FUNC10)
#elif (PWM_CH4_PIN_SEL == 3)
#define PWM_CH4_PIN_CFG     (PIN_D1 | PIN_FUNC5)
#endif

#if (PWM_CH5_PIN_SEL == 0)
#define PWM_CH5_PIN_CFG     (PIN_A20 | PIN_FUNC3)
#elif (PWM_CH5_PIN_SEL == 1)
#define PWM_CH5_PIN_CFG     (PIN_D0 | PIN_FUNC5)
#endif


enum pwm_ch_id_e {
    PWM_CH0 = 0x0,
    PWM_CH1,
    PWM_CH2,
    PWM_CH3,
    PWM_CH4,
    PWM_CH5,
};

void pwm_init(void);
void pwm_test(void);


#endif

