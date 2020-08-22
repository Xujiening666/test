#ifndef _HAL_KEYADC_LIB_H_
#define _HAL_KEYADC_LIB_H_

#ifdef __cplusplus
 extern "C" {
#endif

#define KADC_CH_MAX     6

enum kadc_id_e {
    KADC_ID_00 = 0,
    KADC_ID_01,
    KADC_ID_02,
    KADC_ID_03,
    KADC_ID_04,
    KADC_ID_05,
};

enum kadc_irq_e {
    KADC_IRQ_DISABLE    = (0x0),
    KADC_IRQ_DATA       = (0x1 << 0),
    KADC_IRQ_DOWN       = (0x1 << 1),
    KADC_IRQ_UP         = (0x1 << 2),
};

enum kadc_mode_e {
    KADC_MODE_NORMAL    = (0x0),
    KADC_MODE_SINGLE    = (0x1 << 0),
    KADC_MODE_CONTINUE  = (0x1 << 1),
    KADC_MODE_KNOB      = (0x1 << 2),
};

enum kadc_clksrc_e {
    KADC_CLKSRC_HOSC    = (0x0 << 31),
    KADC_CLKSRC_LOSC    = (0x1 << 31),
};

enum kadc_clkdiv_e {
    KADC_CLKDIV_8       = (0x0 << 28),
    KADC_CLKDIV_16      = (0x1 << 28),
    KADC_CLKDIV_32      = (0x2 << 28),
    KADC_CLKDIV_64      = (0x3 << 28),
    KADC_CLKDIV_128     = (0x4 << 28),
    KADC_CLKDIV_256     = (0x5 << 28),
};


typedef struct {
    uint32_t en;                //en
    uint32_t irq;               //irq sel
    uint32_t cfg;               //threshold, t_cnt, t_dbc, mode
} kadc_param_t;


#define KADC_THRESHOLD_BIT          ((uint32_t)16)
#define KADC_TCNT_BIT               ((uint32_t)8)
#define KADC_TDBC_BIT               ((uint32_t)4)

#define KADC_INTS_DATA_PENDING      ((uint32_t)0x00000001)
#define KADC_INTS_KEYDOWN_PENDING   ((uint32_t)0x00000002)
#define KADC_INTS_KEYUP_PENDING     ((uint32_t)0x00000004)

void kadc_module_init(kadc_param_t *kadc_param);
void kadc_irq_en(void);
void kadc_channel_en(uint32_t channel, bool enable);
void kadc_cal_en(void);
void kadc_wakeup_set(uint32_t channel, bool enable);
void kadc_irq_cb(uint32_t pending, uint32_t *kdata);
uint32_t kadc_data_get(uint32_t ch);


#ifdef __cplusplus
}
#endif

#endif

