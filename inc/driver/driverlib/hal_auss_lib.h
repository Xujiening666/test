#ifndef _HAL_AUSS_LIB_H_
#define _HAL_AUSS_LIB_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "sys_hal_def.h"

enum auss_features_e {
    AUSS_FEATURES_DAC_SDM2          = (1 << 0),
    AUSS_FEATURES_LINEIN_OPTIMISE   = (1 << 1),
    AUSS_FEATURES_ANC_LINEOUT       = (1 << 2),
    AUSS_FEATURES_HEADSET_OPTIMISE  = (1 << 3),
    AUSS_FEATURES_MICBIAS_FOR_MUTE  = (1 << 4),
};

typedef struct {
    uint32_t features;
} auss_module_init_params_t;

typedef struct {
    uint8_t enable;
    uint8_t gain;   /* gain: 0 ~ 0xf */
    uint8_t map;    /* sidetone map: 0-ADC0, 1-ADC1, 2-ADC2 */
} AUSS_SIDETONE_Config;

typedef struct {
    uint8_t enable;
    uint8_t coef_mp;
    uint16_t coef_rshift;
} AUSS_HPF_Config;

enum audio_anc_source_e {
    AUDIO_ANC_NONE                      = 0,
    AUDIO_ANC_AMIC1_2                   = 1, /* analog mic1/2 */
    AUDIO_ANC_DMICDATA0                 = 2, /* DMICDATA0 falling/rising edge */
    AUDIO_ANC_DMICDATA1                 = 3, /* DMICDATA1 falling/rising edge */
    AUDIO_ANC_DMICDATA0_1               = 4, /* DMICDATA0/1 falling/rising edge */
    AUDIO_ANC_AMIC1_2_DMICDATA0         = 5, /* analog mic1/2, DMICDATA0 falling/rising edge */
    AUDIO_ANC_AMIC1_2_DMICDATA1         = 6, /* analog mic1/2, DMICDATA1 falling/rising edge */
};

typedef struct {
    enum audio_anc_source_e source;
    uint32_t fs;
    uint8_t order;
} AUSS_ANC_Config;

enum auss_ctl {
    AUSS_CTL_NONE,

    SIDETONE_CFG,   /* see @AUSS_SIDETONE_Config */
    MICBIAS,        /* mic bias: 0 ~ 3 */
    LPA_GAIN,       /* left pa gain: 0:0dB, 1:-3dB, ..., 7:-27dB; -3dB/step */
    RPA_GAIN,       /* right pa gain: 0:0dB, 1:-3dB, ..., 7:-27dB; -3dB/step */
    OUTPUT_POWER_BOOST,

    LINEOUT_RMIXL,  /* 0: normal; 1: mix pa R to lineout L */
    LINEOUT_LMIXR,  /* 0: normal; 1: mix pa L to lineout R */
    LINEOUTL_EN,    /* 0: disable lineout L; 1: enable lineout L */
    LINEOUTR_EN,    /* 0: disable lineout R; 1: enable lineout R */
    LINEOUTL_GAIN,  /* lineout L gain: 0 ~ 7 */
    LINEOUTR_GAIN,  /* lineout R gain: 0 ~ 7 */

    LINEOUTL_GAIN_EX,
    LINEOUTR_GAIN_EX,


    ADC0_GAIN,      /* ADC gain: 0~0x1ff */
    ADC1_GAIN,
    ADC2_GAIN,
    ADC3_GAIN,
    ADC4_GAIN,

    DMIXER_RCH,     /* DMIXER RCH select: bit0-DataR, bit1-DataL, bit2-SideTone */
    DMIXER_LCH,     /* DMIXER LCH select: bit0-DataL, bit1-DataR, bit2-SideTone */
    DMIXER_RMODE,   /* DMIXER RCH mode: 0-L, 1-R, 2-(L+R)/2, 3-(L-R)/2, 4-(R-L)/2 */
    DMIXER_LMODE,   /* DMIXER LCH mode: 0-L, 1-R, 2-(L+R)/2, 3-(L-R)/2, 4-(R-L)/2 */
    DAC_GAIN,       /* DAC gain: 0~0x1ff */

    ADC0_DITHER_EN,
    ADC1_DITHER_EN,
    ADC2_DITHER_EN,

    ADC0_HPF_EN,
    ADC1_HPF_EN,
    ADC2_HPF_EN,
    ADC3_HPF_EN,
    ADC4_HPF_EN,
    DAC_HPF_EN,

    ADC0_HPF_CFG,   /* see @AUSS_HPF_Config */
    ADC1_HPF_CFG,
    ADC2_HPF_CFG,
    ADC3_HPF_CFG,
    ADC4_HPF_CFG,
    DAC_HPF_CFG,


    DMICDATA0F_EN, /* DMICDATA0 falling edge data enable */
    DMICDATA0R_EN, /* DMICDATA0 rising edge data enable */

    AMIC0_EN,
    AMIC1_EN,
    AMIC2_EN,

    AMIC0_GAIN,
    AMIC1_GAIN,
    AMIC2_GAIN,

    AMIC0_SINGLE_END,
    AMIC1_SINGLE_END,
    AMIC2_SINGLE_END,

    LINEIN0_EN,
    LINEIN1_EN,

    LINEIN0_SINGLE_END,
    LINEIN1_SINGLE_END,

    LINEIN0_GAIN,
    LINEIN1_GAIN,

    BIAS_SHRINK_EN,

    ANC_CFG,
    ANC_EN,
};

enum asrc_outdev_e {
    ASRC_OUTDEV_SRAM,
    ASRC_OUTDEV_I2S,
    ASRC_OUTDEV_DAC,
};

typedef struct {
    uint16_t in_fs;
    uint16_t out_fs;
    uint8_t ch;
    uint8_t sample_size;
    uint8_t out_dev;
} ASRC_InitTypeDef;

void HAL_ASRC_Init(ASRC_InitTypeDef *params);
void HAL_ASRC_DeInit(void);
uint32_t HAL_ASRC_InFifo_Addr(void);
uint32_t HAL_ASRC_OutFifo_Addr(void);

void anc_irq_init(void);
void anc_irq_deinit(void);
bool anc_lineout_is_enable(void);

void auss_micbias_enable(bool enable);
void auss_enter_sleep(void);

int32_t auss_anc_read_ch1(void);
int32_t auss_anc_read_ch2(void);
int32_t auss_anc_read_ch3(void);
int32_t auss_anc_read_ch4(void);

#ifdef __cplusplus
}
#endif

#endif

