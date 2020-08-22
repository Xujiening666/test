#ifndef _KEY_ADC_H_
#define _KEY_ADC_H_


#define KADC0_PIN_NUM   (PIN_B9)
#define KADC1_PIN_NUM   (PIN_B10)
#define KADC2_PIN_NUM   (PIN_B11)
#define KADC3_PIN_NUM   (PIN_B12)
#define KADC4_PIN_NUM   (PIN_B13)
#define KADC5_PIN_NUM   (PIN_B8)

#if !KADC0_IRQ && SLEEP_EN
#define KADC0_FUNC      PIN_FUNCEINT
#else
#define KADC0_FUNC      PIN_FUNCDISABLE
#endif

#if !KADC1_IRQ && SLEEP_EN
#define KADC1_FUNC      PIN_FUNCEINT
#else
#define KADC1_FUNC      PIN_FUNCDISABLE
#endif

#if !KADC2_IRQ && SLEEP_EN
#define KADC2_FUNC      PIN_FUNCEINT
#else
#define KADC2_FUNC      PIN_FUNCDISABLE
#endif

#if !KADC3_IRQ && SLEEP_EN
#define KADC3_FUNC      PIN_FUNCEINT
#else
#define KADC3_FUNC      PIN_FUNCDISABLE
#endif

#if !KADC4_IRQ && SLEEP_EN
#define KADC4_FUNC      PIN_FUNCEINT
#else
#define KADC4_FUNC      PIN_FUNCDISABLE
#endif

#if !KADC5_IRQ && SLEEP_EN
#define KADC5_FUNC      PIN_FUNCEINT
#else
#define KADC5_FUNC      PIN_FUNCDISABLE
#endif


#if KADC_SHARE1
#define SHARE1_SUB_PIN  PIN_A4
#endif

#define KADC_IRQ_EN     ((KADC0_EN && KADC0_IRQ) || \
     (KADC1_EN && KADC1_IRQ) || \
     (KADC2_EN && KADC2_IRQ) || \
     (KADC3_EN && KADC3_IRQ) || \
     (KADC4_EN && KADC4_IRQ) || \
     (KADC5_EN && KADC5_IRQ))

#define KADC_SCAN_EN    ((KADC0_EN && !KADC0_IRQ) || \
    (KADC1_EN && !KADC1_IRQ) || \
    (KADC2_EN && !KADC2_IRQ) || \
    (KADC3_EN && !KADC3_IRQ) || \
    (KADC4_EN && !KADC4_IRQ) || \
    (KADC5_EN && !KADC5_IRQ))

#define KADC_KNOB_EN    (KADC0_KNOB || KADC1_KNOB || KADC2_KNOB || \
     KADC3_KNOB || KADC4_KNOB || KADC5_KNOB)

enum kadc_ch_e {
    KADC_CH0 = 0,
    KADC_CH1,
    KADC_CH2,
    KADC_CH3,
    KADC_CH4,
    KADC_CH5
};

typedef struct {
    uint32_t val;
    uint32_t cnt;
    uint32_t long_sta;
} kadc_t;

typedef struct {
#if KADC0_KNOB
    uint32_t knob0_val;
    uint8_t knob0_step;
#endif
#if KADC1_KNOB
    uint32_t knob1_val;
    uint8_t knob1_step;
#endif
#if KADC2_KNOB
    uint32_t knob2_val;
    uint8_t knob2_step;
#endif
#if KADC3_KNOB
    uint32_t knob3_val;
    uint8_t knob3_step;
#endif
#if KADC4_KNOB
    uint32_t knob4_val;
    uint8_t knob4_step;
#endif
#if KADC5_KNOB
    uint32_t knob5_val;
    uint8_t knob5_step;
#endif

#if KADC_SHARE1
    uint32_t share1_val;
    uint8_t share1_step;

    uint8_t share1_num;
    uint32_t share1_vr1;
#endif
} kadc_vol_t;

#define KNOB0_VOL_MAX       12
#define KNOB1_VOL_MAX       12
#define KNOB2_VOL_MAX       12
#define KNOB3_VOL_MAX       12
#define KNOB4_VOL_MAX       5
#define KNOB5_VOL_MAX       12

#define SHARE1_VOL_MAX      5

#define KNOB0_THRESHOLD     0x700

#define KNOB0_DIFF          0x80
#define KNOB1_DIFF          0x80
#define KNOB2_DIFF          0x80
#define KNOB3_DIFF          0x80
#define KNOB4_DIFF          0x40
#define KNOB5_DIFF          0x80

#define SHARE1_DIFF         1000
#define KADC_CH_SHARE1      11

#define KNOB_INIT           0xFFFF


#define HP_CON_CONFIG()      pin_config(PIN_B12 | PIN_FUNCOUTPUT | PIN_HIGH)
#define HP_DICON_CONFIG()    pin_config(PIN_B12 | PIN_FUNCOUTPUT | PIN_LOW)

void kadc_ccu_init(void);
void knob_vol_init(void);
void kadc_gpio_init(void);
void kadc_wakeup_init(void);
void kadc_init(void);
uint32_t kadc_get_key(uint32_t data);
uint32_t kadc_scan_get_key(void);
void kadc_share1_init(void);
void kadc_share1(uint32_t data);
bool hp_get_get_detach_state(void);

#endif

