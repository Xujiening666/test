#ifndef _LEDC_H_
#define _LEDC_H_

#ifdef __cplusplus
 extern "C" {
#endif


#if (LED0_PIN_SEL == 0)
#define LEDC_D0_PIN_CFG     (PIN_B5 | PIN_DRI1 | PIN_FUNC11)
#elif (LED0_PIN_SEL == 3)
#define LEDC_D0_PIN_CFG     (PIN_A15 | PIN_DRI1 | PIN_FUNC2)
#endif

#if (LED1_PIN_SEL == 0)
#define LEDC_D1_PIN_CFG     (PIN_B6 | PIN_DRI1 | PIN_FUNC11)
#elif (LED1_PIN_SEL == 3)
#define LEDC_D1_PIN_CFG     (PIN_A16 | PIN_DRI1 | PIN_FUNC2)
#endif

#if (LED2_PIN_SEL == 0)
#define LEDC_D2_PIN_CFG     (PIN_B7 | PIN_DRI1 | PIN_FUNC11)
#elif (LED2_PIN_SEL == 3)
#define LEDC_D2_PIN_CFG     (PIN_A17 | PIN_DRI1 | PIN_FUNC2)
#endif

#if (LED3_PIN_SEL == 0)
#define LEDC_D3_PIN_CFG     (PIN_B8 | PIN_DRI1 | PIN_FUNC11)
#elif (LED3_PIN_SEL == 3)
#define LEDC_D3_PIN_CFG     (PIN_A18 | PIN_DRI1 | PIN_FUNC2)
#endif

#if (LED4_PIN_SEL == 0)
#define LEDC_D4_PIN_CFG     (PIN_B10 | PIN_DRI1 | PIN_FUNC11)
#elif (LED4_PIN_SEL == 3)
#define LEDC_D4_PIN_CFG     (PIN_A19 | PIN_DRI1 | PIN_FUNC2)
#endif

#if (LED5_PIN_SEL == 0)
#define LEDC_D5_PIN_CFG     (PIN_B0 | PIN_DRI1 | PIN_FUNC11)
#elif (LED5_PIN_SEL == 1)
#define LEDC_D5_PIN_CFG     (PIN_B11 | PIN_DRI1 | PIN_FUNC11)
#elif (LED5_PIN_SEL == 3)
#define LEDC_D5_PIN_CFG     (PIN_A20 | PIN_DRI1 | PIN_FUNC2)
#endif

#if (LED6_PIN_SEL == 0)
#define LEDC_D6_PIN_CFG     (PIN_B2 | PIN_DRI1 | PIN_FUNC11)
#elif (LED6_PIN_SEL == 1)
#define LEDC_D6_PIN_CFG     (PIN_B12 | PIN_DRI1 | PIN_FUNC11)
#elif (LED6_PIN_SEL == 3)
#define LEDC_D6_PIN_CFG     (PIN_A21 | PIN_DRI1 | PIN_FUNC2)
#endif

#if (LED7_PIN_SEL == 0)
#define LEDC_D7_PIN_CFG     (PIN_B13 | PIN_DRI1 | PIN_FUNC11)
#elif (LED7_PIN_SEL == 3)
#endif

#if (LED8_PIN_SEL == 0)
#define LEDC_D8_PIN_CFG     (PIN_D0 | PIN_DRI1 | PIN_FUNC13)
#elif (LED8_PIN_SEL == 3)
#endif



#define LED_COM_NUM     5
#define LED_SEG_NUM     7


#define LED_NULL        0x00

#define LED_COL         0x01
#define LED_TF          0x02
#define LED_USB         0x03
#define LED_BT          0x04
#define LED_FM          0x04
#define LED_AUX         0x05
#define LED_BAT0        0x06
#define LED_BAT1        0x07

#define LED_1A          0x11
#define LED_1B          0x12
#define LED_1C          0x13
#define LED_1D          0x14
#define LED_1E          0x15
#define LED_1F          0x16
#define LED_1G          0x17

#define LED_2A          0x21
#define LED_2B          0x22
#define LED_2C          0x23
#define LED_2D          0x24
#define LED_2E          0x25
#define LED_2F          0x26
#define LED_2G          0x27

#define LED_3A          0x31
#define LED_3B          0x32
#define LED_3C          0x33
#define LED_3D          0x34
#define LED_3E          0x35
#define LED_3F          0x36
#define LED_3G          0x37

#define LED_4A          0x41
#define LED_4B          0x42
#define LED_4C          0x43
#define LED_4D          0x44
#define LED_4E          0x45
#define LED_4F          0x46
#define LED_4G          0x47


//define of every segment
#define LED_SEG_A       (1<<0)
#define LED_SEG_B       (1<<1)
#define LED_SEG_C       (1<<2)
#define LED_SEG_D       (1<<3)
#define LED_SEG_E       (1<<4)
#define LED_SEG_F       (1<<5)
#define LED_SEG_G       (1<<6)

#define LED_SEG_COL     (1<<0)
#define LED_SEG_TF      (1<<1)
#define LED_SEG_USB     (1<<2)
#define LED_SEG_BT      (1<<3)
#define LED_SEG_FM      (1<<3)
#define LED_SEG_AUX     (1<<4)
#define LED_SEG_BAT0    (1<<5)
#define LED_SEG_BAT1    (1<<6)


//the segment of every number and letter:
#define LED_0       (LED_SEG_A | LED_SEG_B | LED_SEG_C | LED_SEG_D | LED_SEG_E | LED_SEG_F)
#define LED_1       (LED_SEG_B | LED_SEG_C)
#define LED_2       (LED_SEG_A | LED_SEG_B | LED_SEG_G | LED_SEG_E | LED_SEG_D)
#define LED_3       (LED_SEG_A | LED_SEG_B | LED_SEG_G | LED_SEG_C | LED_SEG_D)
#define LED_4       (LED_SEG_B | LED_SEG_C | LED_SEG_F | LED_SEG_G)
#define LED_5       (LED_SEG_A | LED_SEG_C | LED_SEG_D | LED_SEG_F | LED_SEG_G)
#define LED_6       (LED_SEG_A | LED_SEG_C | LED_SEG_D | LED_SEG_E | LED_SEG_F | LED_SEG_G)
#define LED_7       (LED_SEG_A | LED_SEG_B | LED_SEG_C)
#define LED_8       (LED_SEG_A | LED_SEG_B | LED_SEG_C | LED_SEG_D | LED_SEG_E | LED_SEG_F | LED_SEG_G)
#define LED_9       (LED_SEG_A | LED_SEG_B | LED_SEG_C | LED_SEG_D | LED_SEG_F | LED_SEG_G)
#define LED_A       (LED_SEG_A | LED_SEG_B | LED_SEG_C | LED_SEG_E | LED_SEG_F | LED_SEG_G)
#define LED_B       (LED_SEG_C | LED_SEG_D | LED_SEG_E | LED_SEG_F | LED_SEG_G)
#define LED_C       (LED_SEG_A | LED_SEG_D | LED_SEG_E | LED_SEG_F)
#define LED_D       (LED_SEG_B | LED_SEG_C | LED_SEG_D | LED_SEG_E | LED_SEG_G)
#define LED_E       (LED_SEG_A | LED_SEG_D | LED_SEG_E | LED_SEG_F | LED_SEG_G)
#define LED_F       (LED_SEG_A | LED_SEG_E | LED_SEG_F | LED_SEG_G)
#define LED_G       (LED_SEG_A | LED_SEG_B | LED_SEG_C | LED_SEG_D | LED_SEG_F | LED_SEG_G)
#define LED_H       (LED_SEG_B | LED_SEG_C | LED_SEG_E | LED_SEG_F | LED_SEG_G)
#define LED_I       (LED_SEG_B | LED_SEG_C)
#define LED_J       (LED_SEG_B | LED_SEG_C | LED_SEG_D | LED_SEG_E)
#define LED_K       (LED_SEG_E | LED_SEG_F | LED_SEG_G)
#define LED_L       (LED_SEG_D | LED_SEG_E | LED_SEG_F)
#define LED_M       (LED_SEG_C | LED_SEG_E | LED_SEG_G)
#define LED_N       (LED_SEG_A | LED_SEG_B | LED_SEG_C | LED_SEG_E | LED_SEG_F)
#define LED_O       (LED_SEG_A | LED_SEG_B | LED_SEG_C | LED_SEG_D | LED_SEG_E | LED_SEG_F)
#define LED_P       (LED_SEG_A | LED_SEG_B | LED_SEG_E | LED_SEG_F | LED_SEG_G)
#define LED_Q       (LED_SEG_A | LED_SEG_B | LED_SEG_C | LED_SEG_F | LED_SEG_G)
#define LED_R       (LED_SEG_E | LED_SEG_G)
#define LED_S       (LED_SEG_A | LED_SEG_C | LED_SEG_D | LED_SEG_F | LED_SEG_G)
#define LED_T       (LED_SEG_D | LED_SEG_E | LED_SEG_F | LED_SEG_G)
#define LED_U       (LED_SEG_B | LED_SEG_C | LED_SEG_D | LED_SEG_E | LED_SEG_F)
#define LED_V       (LED_SEG_B | LED_SEG_C | LED_SEG_D | LED_SEG_F | LED_SEG_E)
#define LED_W       (LED_SEG_C | LED_SEG_D | LED_SEG_E)
#define LED_X       (LED_SEG_B | LED_SEG_C | LED_SEG_E | LED_SEG_F)
#define LED_Y       (LED_SEG_B | LED_SEG_C | LED_SEG_D | LED_SEG_F | LED_SEG_G)
#define LED_Z       (LED_SEG_A | LED_SEG_B | LED_SEG_D | LED_SEG_E | LED_SEG_G)

#define LED__       (LED_SEG_G)
#define LED_ON      (LED_8)
#define LED_OFF     (LED_NULL)


void ledc_init(void);
void ledc_deinit(void);
void ledc_null_get(uint32_t *led_null);
void ledc_lt_get(uint32_t led_num);
void ledc_update(void);


#ifdef __cplusplus
}
#endif

#endif

