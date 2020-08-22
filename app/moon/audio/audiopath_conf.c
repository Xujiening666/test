#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include <math.h>
#include "audiopath_conf.h"
#include "sndlib.h"
#include "audio_in.h"
#include "task_main.h"
#include "system.h"
#include "audio_out.h"
#include "log.h"

#define MIN(a,b)    ((a) < (b) ? (a) : (b))

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(X) (sizeof(X) / sizeof(X[0]))
#endif

#define ADC0_NEED_EN(src) \
    (((src) == AUDIO_INPUT_AMIC0) || ((src) == AUDIO_INPUT_AMIC0_1) || \
     ((src) == AUDIO_INPUT_AMIC0_2) || ((src) == AUDIO_INPUT_LINEIN0) || \
     ((src) == AUDIO_INPUT_LINEIN0_1))

#define ADC1_NEED_EN(src) \
    (((src) == AUDIO_INPUT_AMIC1) || ((src) == AUDIO_INPUT_DMIC0F) || \
     ((src) == AUDIO_INPUT_AMIC0_1) || ((src) == AUDIO_INPUT_AMIC1_2) || \
     ((src) == AUDIO_INPUT_DMIC0F_R) || ((src) == AUDIO_INPUT_LINEIN1) || \
     ((src) == AUDIO_INPUT_LINEIN0_1))

#define ADC2_NEED_EN(src) \
    (((src) == AUDIO_INPUT_AMIC2) || ((src) == AUDIO_INPUT_DMIC0R) || \
     ((src) == AUDIO_INPUT_AMIC0_2) || ((src) == AUDIO_INPUT_AMIC1_2) || \
     ((src) == AUDIO_INPUT_DMIC0F_R))

#define ADC1_AUDIO_INPUT_IS_DMIC(src) \
    (((src) == AUDIO_INPUT_DMIC0F) || ((src) == AUDIO_INPUT_DMIC0F_R))

#define ADC2_AUDIO_INPUT_IS_DMIC(src) \
    (((src) == AUDIO_INPUT_DMIC0R) || ((src) == AUDIO_INPUT_DMIC0FR))

#define AMIC0_NEED_EN(src) \
    (((src) == AUDIO_INPUT_AMIC0) || ((src) == AUDIO_INPUT_AMIC0_1) || ((src) == AUDIO_INPUT_AMIC0_2))

#define AMIC1_NEED_EN(src) \
    (((src) == AUDIO_INPUT_AMIC1) || ((src) == AUDIO_INPUT_AMIC0_1) || ((src) == AUDIO_INPUT_AMIC1_2))

#define AMIC2_NEED_EN(src) \
    (((src) == AUDIO_INPUT_AMIC2) || ((src) == AUDIO_INPUT_AMIC0_2) || ((src) == AUDIO_INPUT_AMIC1_2))

#define LINEIN0_NEED_EN(src) \
    (((src) == AUDIO_INPUT_LINEIN0) || ((src) == AUDIO_INPUT_LINEIN0_1))

#define LINEIN1_NEED_EN(src) \
    (((src) == AUDIO_INPUT_LINEIN1) || ((src) == AUDIO_INPUT_LINEIN0_1))

#define AUDIO_INPUT_CH(src) \
    ( \
    (((src) == AUDIO_INPUT_AMIC0_1) || ((src) == AUDIO_INPUT_AMIC0_2) || \
     ((src) == AUDIO_INPUT_AMIC1_2) || ((src) == AUDIO_INPUT_DMIC0F_R) || \
     ((src) == AUDIO_INPUT_LINEIN0_1)) ? \
     2 : 1 \
    )

/* 获取ADC输入源对应的ADC channels mask
 * @param src: ADC输入源选择， 参考@enum audio_input_source_e
 * @return: 返回 channels mask; bit0-ADC0, bit1-ADC1, bit2-ADC2
 */
AT(.ain_flash_seg)
uint32_t get_adc_ch_mask(uint32_t src)
{
    uint32_t mask = 0;

    if (ADC0_NEED_EN(src)) {
        mask |= (1 << 0);
    }

    if (ADC1_NEED_EN(src)) {
        mask |= (1 << 1);
    }

    if (ADC2_NEED_EN(src)) {
        mask |= (1 << 2);
    }

    return mask;
}

/*
 * audiocodec用户配置
 */
void codec_user_config(void)
{
    //mixer_set_ctl(SND_CODEC, ADC0_HPF_EN, 1);         /* 使能ADC0高通滤波器 */
    //mixer_set_ctl(SND_CODEC, ADC1_HPF_EN, 1);         /* 使能ADC1高通滤波器 */
    //mixer_set_ctl(SND_CODEC, ADC2_HPF_EN, 1);         /* 使能ADC2高通滤波器 */
    //mixer_set_ctl(SND_CODEC, DAC_HPF_EN, 1);          /* 使能DAC高通滤波器 */
    //mixer_set_ctl(SND_CODEC, ADC0_DITHER_EN, 1);      /* 使能ADC0 dither */
    //mixer_set_ctl(SND_CODEC, ADC1_DITHER_EN, 1);      /* 使能ADC1 dither */
    //mixer_set_ctl(SND_CODEC, ADC2_DITHER_EN, 1);      /* 使能ADC2 dither */
    //mixer_set_ctl(SND_CODEC, OUTPUT_POWER_BOOST, 1);  /* 使能输出功率增强 */
    //mixer_set_ctl(SND_CODEC, LPA_GAIN, 0); /* left pa gain: 0:0dB, 1:-3dB, ..., 7:-27dB; -3dB/step */
    //mixer_set_ctl(SND_CODEC, RPA_GAIN, 0); /* right pa gain: 0:0dB, 1:-3dB, ..., 7:-27dB; -3dB/step */
    //mixer_set_ctl(SND_CODEC, MICBIAS, 0);  /* mic bias: 0 ~ 3,   0-2.0v; 1-2.34v; 2-2.67v; 3-2.9v */

#if AUDIO_ADJUST_HPLR_LINEOUTLR
    lineout_hp_gain_init();
#endif

#if 0
    AUSS_ANC_Config anc_cfg;
    //anc_cfg.source = AUDIO_ANC_AMIC1_2;           /* 使用模拟MIC1，MIC2作为ANC输入 */
    //anc_cfg.source = AUDIO_ANC_DMICDATA0;         /* 使用数字MIC0作为ANC输入 */
    //anc_cfg.source = AUDIO_ANC_DMICDATA1;         /* 使用数字MIC1作为ANC输入 */
    //anc_cfg.source = AUDIO_ANC_AMIC1_2_DMICDATA0; /* 使用模拟MIC1，MIC2 以及数字MIC0作为ANC输入 */
    //anc_cfg.source = AUDIO_ANC_AMIC1_2_DMICDATA1; /* 使用模拟MIC1，MIC2 以及数字MIC1作为ANC输入 */
    anc_cfg.source = AUDIO_ANC_DMICDATA0_1;         /* 使用数字MIC0和数字MIC1作为ANC输入 */
    anc_cfg.fs = 192000;                            /* anc 采样率 */
    anc_cfg.order = 4;
    mixer_set_ctl(SND_CODEC, ANC_CFG, (uint32_t)&anc_cfg);
#endif

    /* 加载user_config.h中的配置 */
    mixer_set_ctl(SND_CODEC, AMIC0_SINGLE_END, AUDIO_AMIC0_SINGLE_END);     /* 模拟MIC0单端模式： 0-差分模式， 1-单端模式 */
    mixer_set_ctl(SND_CODEC, AMIC1_SINGLE_END, AUDIO_AMIC1_SINGLE_END);     /* 模拟MIC1单端模式： 0-差分模式， 1-单端模式 */
    mixer_set_ctl(SND_CODEC, AMIC2_SINGLE_END, AUDIO_AMIC2_SINGLE_END);     /* 模拟MIC2单端模式： 0-差分模式， 1-单端模式 */
    mixer_set_ctl(SND_CODEC, LINEIN0_SINGLE_END, AUDIO_LINEIN0_SINGLE_END); /* LINEIN0单端模式： 0-差分模式， 1-单端模式 */
    mixer_set_ctl(SND_CODEC, LINEIN1_SINGLE_END, AUDIO_LINEIN1_SINGLE_END); /* LINEIN1单端模式： 0-差分模式， 1-单端模式 */
    mixer_set_ctl(SND_CODEC, AMIC0_GAIN, AUDIO_AMIC0_ANALOG_GAIN);          /* 模拟MIC0模拟音量 */
    mixer_set_ctl(SND_CODEC, AMIC1_GAIN, AUDIO_AMIC1_ANALOG_GAIN);          /* 模拟MIC1模拟音量 */
    mixer_set_ctl(SND_CODEC, AMIC2_GAIN, AUDIO_AMIC2_ANALOG_GAIN);          /* 模拟MIC2模拟音量 */
    mixer_set_ctl(SND_CODEC, LINEIN0_GAIN, AUDIO_LINEIN0_ANALOG_GAIN);      /* linein0模拟音量 */
    mixer_set_ctl(SND_CODEC, LINEIN1_GAIN, AUDIO_LINEIN1_ANALOG_GAIN);      /* linein1模拟音量 */
}

/* 打开录音通路；
 * 此函数由audio库调用，打开MIC，linein等ADC输入通路
 * @param mode: 表示当前属于什么模式。参考 @audio_in_mode_t
 */
AT(.ain_flash_seg)
void record_path_on(uint8_t mode)
{
    if (AUDIO_KARAOKE_MODE && mode != AUDIO_IN_MODE_SCO) {
        audio_service_set_mic_input_source(AUDIO_REC_INPUT_SEL);
#if LINEIN_ALL_MODE_SUPPORT
        audio_service_set_linein_input_source(AUDIO_LINEIN_MODE_INPUT_SEL);
#endif
    } else {
        switch (mode) {
        case AUDIO_IN_MODE_NATIVE_REC: /* 录音模式 */
            if (0 == AUDIO_KARAOKE_MODE) {
                /* 打开MIC通路 */
                audio_service_set_mic_input_source(AUDIO_REC_INPUT_SEL);
            }
            break;

        case AUDIO_IN_MODE_SCO: /* 通话模式 */
            mixer_set_ctl(SND_CODEC, BIAS_SHRINK_EN, 1);
            /* 打开MIC通路 */
            audio_service_set_mic_input_source(AUDIO_REC_INPUT_SEL);
            break;
        }
    }
}

/* 关闭录音通路；
 * 此函数由audio库调用，关闭ADC输入通路
 * @param mode: 表示当前属于什么模式。参考 @audio_in_mode_t
 */
AT(.ain_flash_seg)
void record_path_off(uint8_t mode)
{
    mixer_set_ctl(SND_CODEC, BIAS_SHRINK_EN, 0);

    /* 关闭所有输入源 */
    audio_service_disable_input_source(AUDIO_INPUT_DMIC0F_R);
    audio_service_disable_input_source(AUDIO_INPUT_AMIC0);
    audio_service_disable_input_source(AUDIO_INPUT_AMIC1_2);
    audio_service_disable_input_source(AUDIO_INPUT_LINEIN0_1);
}

/*
 * 配置播放通路；此函数由audio库调用
 */
AT(.aout_flash_seg)
void playback_path_on(void)
{
    /* DMIXER RCH select: bit0-DataR, bit1-DataL, bit2-SideTone */
    mixer_set_ctl(SND_CODEC, DMIXER_RCH, 1<<0);
    /* DMIXER LCH select: bit0-DataL, bit1-DataR, bit2-SideTone */
    mixer_set_ctl(SND_CODEC, DMIXER_LCH, 1<<0);
#if AUDIO_EWS_MODE
    /* DMIXER RCH mode: 0-L, 1-R, 2-(L+R)/2, 3-(L-R)/2, 4-(R-L)/2 */
    mixer_set_ctl(SND_CODEC, DMIXER_RMODE, 0);
#else
    /* DMIXER RCH mode: 0-L, 1-R, 2-(L+R)/2, 3-(L-R)/2, 4-(R-L)/2 */
    mixer_set_ctl(SND_CODEC, DMIXER_RMODE, 1);
#endif
    /* DMIXER LCH mode: 0-L, 1-R, 2-(L+R)/2, 3-(L-R)/2, 4-(R-L)/2 */
    mixer_set_ctl(SND_CODEC, DMIXER_LMODE, 0);
}


/* 音量转换成分贝（dB)值函数
 * @param vol: 音量值（范围：0~100）
 * @return: dB值
 */
AT(.aout_flash_seg)
float vol2decibels(int32_t vol)
{
    float decibels;

    if (vol <= 0) {
        decibels = -100000.f;
    } else {
        decibels = 0.5f * (vol - 100);
    }

    return decibels;
}

/* 音量转换成codec寄存器值函数
 * @param volume: 音量值（范围：0~100）
 * @param val_0db: 0dB对应的寄存器值
 * @param val_max: 100音量对应的寄存器值
 * @return: 返回音量对应寄存器值
 */
AT(.aout_flash_seg)
static uint32_t volume2val(uint8_t volume, int32_t val_0db, int32_t val_max)
{
    int32_t val;

    if (0 == volume) {
        val = 0x100;
    } else if (100 == volume) {
        val = (0x200 + val_max) & 0x1ff;
    } else {
        float decibels = vol2decibels(volume);
        /* dac gain 0x1ff is 0dB */
        decibels += (val_max - val_0db) * 0.75f;
        val = (decibels + val_0db*0.75f) / 0.75f;
        val = (0x200 + val) & 0x1ff;
    }

    return (uint32_t)val;
}

/* 音量转换成增益值
 * @param vol: 音量值（范围：0~100）
 * @param max_decibels: 100音量对应多少dB
 * @return: 返回增益值
 */
AT(.aout_flash_seg)
float volume_to_amp(int32_t vol, float max_decibels)
{
    float decibels;

    if (vol <= 0) {
        return 0;
    } else {
        decibels = 0.5f * (vol - 100) + max_decibels;
        return exp(decibels * 0.115129f);
    }
}

/* 音量转换成增益值
 * @param vol: 音量值（范围：0~100）
 * @return: 返回增益值(范围0.0 ~ 1.0)
 */
AT(.aout_flash_seg)
float get_amp(int32_t vol)
{
    float decibels = vol2decibels(vol);

    if(vol <= 0) {
        return 0.0f;
    } else if (vol >= 100) {
        return 1.0f;
    } else {
        return exp(decibels * 0.115129f);
    }
}

/* 根据user_config.h配置ADC数字音量
 * @param input: 输入源选择
 */
void load_digital_gain_config(enum audio_input_source_e input)
{
    switch (input) {
    case AUDIO_INPUT_LINEIN0: /* 设置 LINEIN0数字音量 */
        mixer_set_ctl(SND_CODEC, ADC0_GAIN, AUDIO_LINEIN0_DIGITAL_GAIN);
        break;
    case AUDIO_INPUT_LINEIN1: /* 设置 LINEIN1数字音量 */
        mixer_set_ctl(SND_CODEC, ADC1_GAIN, AUDIO_LINEIN1_DIGITAL_GAIN);
        break;

    case AUDIO_INPUT_LINEIN0_1: /* 设置 LINEIN0和LINEIN1数字音量 */
        mixer_set_ctl(SND_CODEC, ADC0_GAIN, AUDIO_LINEIN0_DIGITAL_GAIN);
        mixer_set_ctl(SND_CODEC, ADC1_GAIN, AUDIO_LINEIN1_DIGITAL_GAIN);
        break;

    case AUDIO_INPUT_AMIC0: /* 设置 AMIC0数字音量 */
        mixer_set_ctl(SND_CODEC, ADC0_GAIN, AUDIO_AMIC0_DIGITAL_GAIN);
        break;


    case AUDIO_INPUT_AMIC1: /* 设置 AMIC1数字音量 */
        mixer_set_ctl(SND_CODEC, ADC1_GAIN, AUDIO_AMIC1_DIGITAL_GAIN);
        break;

    case AUDIO_INPUT_AMIC2: /* 设置 AMIC2数字音量 */
        mixer_set_ctl(SND_CODEC, ADC2_GAIN, AUDIO_AMIC2_DIGITAL_GAIN);
        break;

    case AUDIO_INPUT_AMIC0_1: /* 设置 AMIC0 AMIC1数字音量 */
        mixer_set_ctl(SND_CODEC, ADC0_GAIN, AUDIO_AMIC0_DIGITAL_GAIN);
        mixer_set_ctl(SND_CODEC, ADC1_GAIN, AUDIO_AMIC1_DIGITAL_GAIN);
        break;

    case AUDIO_INPUT_AMIC0_2: /* 设置 AMIC0 AMIC2数字音量 */
        mixer_set_ctl(SND_CODEC, ADC0_GAIN, AUDIO_AMIC0_DIGITAL_GAIN);
        mixer_set_ctl(SND_CODEC, ADC2_GAIN, AUDIO_AMIC2_DIGITAL_GAIN);
        break;

    case AUDIO_INPUT_AMIC1_2: /* 设置 AMIC1 AMIC2数字音量 */
        mixer_set_ctl(SND_CODEC, ADC1_GAIN, AUDIO_AMIC1_DIGITAL_GAIN);
        mixer_set_ctl(SND_CODEC, ADC2_GAIN, AUDIO_AMIC2_DIGITAL_GAIN);
        break;

    case AUDIO_INPUT_DMIC0F: /* 设置 数字MIC0下降沿 数字音量 */
        mixer_set_ctl(SND_CODEC, ADC1_GAIN, AUDIO_DMICDATA0F_DIGITAL_GAIN);
        break;

    case AUDIO_INPUT_DMIC0R: /* 设置 数字MIC0上升沿 数字音量 */
        mixer_set_ctl(SND_CODEC, ADC2_GAIN, AUDIO_DMICDATA0R_DIGITAL_GAIN);
        break;


    case AUDIO_INPUT_DMIC0F_R: /* 设置 数字MIC0上升和下降沿 数字音量 */
        mixer_set_ctl(SND_CODEC, ADC1_GAIN, AUDIO_DMICDATA0F_DIGITAL_GAIN);
        mixer_set_ctl(SND_CODEC, ADC2_GAIN, AUDIO_DMICDATA0R_DIGITAL_GAIN);
        break;

    default:
        break;
    }
}

/* 获取最大的数字音量值（由user_config.h给出）
 * @param input: 输入源选择
 * @return: 最大数字音量值
 */
int32_t get_config_max_digital_gain(enum audio_input_source_e input)
{
    switch (input) {
    case AUDIO_INPUT_LINEIN0:   return AUDIO_LINEIN0_DIGITAL_GAIN;
    case AUDIO_INPUT_LINEIN1:   return AUDIO_LINEIN1_DIGITAL_GAIN;
    case AUDIO_INPUT_LINEIN0_1: return MIN(AUDIO_LINEIN0_DIGITAL_GAIN, AUDIO_LINEIN1_DIGITAL_GAIN);
    case AUDIO_INPUT_AMIC0:     return AUDIO_AMIC0_DIGITAL_GAIN;
    case AUDIO_INPUT_AMIC1:     return AUDIO_AMIC1_DIGITAL_GAIN;
    case AUDIO_INPUT_AMIC2:     return AUDIO_AMIC2_DIGITAL_GAIN;
    case AUDIO_INPUT_AMIC0_1:   return MIN(AUDIO_AMIC0_DIGITAL_GAIN, AUDIO_AMIC1_DIGITAL_GAIN);
    case AUDIO_INPUT_AMIC0_2:   return MIN(AUDIO_AMIC0_DIGITAL_GAIN, AUDIO_AMIC2_DIGITAL_GAIN);
    case AUDIO_INPUT_AMIC1_2:   return MIN(AUDIO_AMIC1_DIGITAL_GAIN, AUDIO_AMIC2_DIGITAL_GAIN);
    case AUDIO_INPUT_DMIC0F:     return AUDIO_DMICDATA0F_DIGITAL_GAIN;
    case AUDIO_INPUT_DMIC0R:     return AUDIO_DMICDATA0R_DIGITAL_GAIN;
    case AUDIO_INPUT_DMIC0F_R:   return MIN(AUDIO_DMICDATA0F_DIGITAL_GAIN, AUDIO_DMICDATA0R_DIGITAL_GAIN);

    default:
        break;
    }

    return 0;
}

/* 设置DAC音量
 * @param volume:音量值（0~100）
 */
AT(.aout_flash_seg)
void set_dac_volume(uint8_t volume)
{
    int32_t val;

    val = volume2val(volume, -1, AUDIO_DAC_GAIN - 1);
    //logd("volume:%d, val:%#x", volume, val);
    mixer_set_ctl(SND_CODEC, DAC_GAIN, val);
}

/* 设置MIC数字音量
 * @param volume:音量值（0~100）
 */
void set_mic_adc_volume(uint8_t volume)
{
    enum audio_input_source_e input;
    int32_t val;

    input = audio_service_get_mic_input_source();
    val = volume2val(volume, 0, get_config_max_digital_gain(input));
    audio_service_set_input_digital_gain(input, val);
}


/* 获取输出声卡ID
 * @return: 0：I2S输出； 1：codec DAC输出
 */
AT(.aout_sram_seg)
uint8_t get_out_sndcard(void)
{
    return AUDIO_OUT_DEVICE_SEL;
}

/* 获取输入声卡ID
 * @return: 0：I2S输入； 1：codec ADC输入
 */
AT(.ain_sram_seg)
uint8_t get_in_sndcard(void)
{
    return AUDIO_IN_DEVICE_SEL;
}

#if AUDIO_ADJUST_HPLR_LINEOUTLR /* HP和LINEOUT左右声道独立调节功能 */

#define AUDIO_INDEPENDENT_ADJUST_BASE_DB    (0)//(-6)

uint8_t current_channels;
uint8_t current_sample_size;

float paL_amp;
float paR_amp;
float paL_amp_state = 1.0;
float paR_amp_state = 1.0;
float paL_amp_last = 1.0;
float paR_amp_last = 1.0;

uint8_t mainL_gain = 100;
uint8_t mainR_gain = 100;
float mainL_amp = 1.0;
float mainR_amp = 1.0;
int16_t  func_hpl_gain;
int16_t  func_hpr_gain;

/* SCO_AUDIO_OUT_CH overwrite */
bool SCO_AUDIO_OUT_CH = 2;

float get_pa_amp(uint32_t level)
{
    float decibels;

    decibels = (7 - level) * -1.5f;
    decibels += AUDIO_INDEPENDENT_ADJUST_BASE_DB;
    return exp(decibels * 0.115129f);
}

uint32_t regR;
uint32_t regL;
uint32_t regR_state;
uint32_t regL_state;
uint32_t regR_last;
uint32_t regL_last;


AT(.auss_anc_sram_seg)
void app_set_lreg(uint32_t val)
{
    uint32_t reg_val;

    reg_val = readl(0x4000901c);
    reg_val &= ~(0x7 << 1);
    reg_val |= (val & 0x7) << 1;
    if (val & 0xFFFF0000) {
        reg_val |= (1 << 8);
    } else {
        reg_val &= ~(1 << 8);
    }
    writel(reg_val, 0x4000901c);
}

AT(.auss_anc_sram_seg)
void app_set_rreg(uint32_t val)
{
    uint32_t reg_val;

    reg_val = readl(0x4000901c);
    reg_val &= ~(0x7 << 5);
    reg_val |= (val & 0x7) << 5;
    if (val & 0xFFFF0000) {
        reg_val |= (1 << 9);
    } else {
        reg_val &= ~(1 << 9);
    }
    writel(reg_val, 0x4000901c);
}

int32_t last_L = 0, last_R = 0;

AT(.auss_anc_sram_seg)
void func_clbk(int32_t *pL, int32_t *pR)
{
    int32_t this_L = *pL, this_R = *pR;

    if ((this_R ^ last_R) & 0x80000000) {
        if (paR_amp_last != paR_amp) {
            paR_amp_last = paR_amp;
        }
        if (regR_last != regR) {
            regR_last = regR;
            app_set_rreg(regR);
        }
    }
    if ((this_L ^ last_L) & 0x80000000) {
        if (paL_amp_last != paL_amp) {
            paL_amp_last = paL_amp;
        }
        if (regL_last != regL) {
            regL_last = regL;
            app_set_lreg(regL);
        }
    }

    *pL = (int32_t)((float)(*pL) * paL_amp_last);
    *pR = (int32_t)((float)(*pR) * paR_amp_last);

    last_L = this_L;
    last_R = this_R;
}

AT(.dac_aout_sram_seg)
int32_t dac_aout_write_callback(aout_interface_t *self,
    void *buf, int32_t samples)
{
    int i;

    if (1 == current_channels) {
        return 0;
    }

    if (2 == current_sample_size) {
        int16_t *p = (int16_t*)buf;
        for (i = 0; i < samples; i++) {
            p[i*2] = p[i*2] * mainL_amp/* * paL_amp*/;
            p[i*2+1] = p[i*2+1] * mainR_amp/* * paR_amp*/;
        }
    } else {
        int32_t *p = (int32_t*)buf;
        for (i = 0; i < samples; i++) {
            p[i*2] = p[i*2] * mainL_amp /** paL_amp*/;
            p[i*2+1] = p[i*2+1] * mainR_amp /** paR_amp*/;
        }
    }
    return 0;
}

AT(.aout_flash_seg)
int32_t dac_aout_init_callback(aout_interface_t *self, int32_t sampling_rate,
    int32_t channels, int32_t bytesPerSample)
{
    current_channels = channels;
    current_sample_size = bytesPerSample;

    return 0;
}

/* L channel gain: 0~100 */
void set_mainL_gain(uint32_t gain)
{
    mainL_gain = gain;
    mainL_amp = get_amp(mainL_gain);
}

/* R channel gain: 0~100 */
void set_mainR_gain(uint32_t gain)
{
    mainR_gain = gain;
    mainR_amp = get_amp(mainR_gain);
}

const int16_t SL6800B_LINEOUT_LEVEL_CFG[15][3] = {
    {0, 0, -27}, //0
    {0, 1, -22}, //1
    {0, 2, -13}, //2
    {0, 3,  -8}, //3
    {0, 5,  -5}, //4
    {0, 6,  -1}, //5
    {1, 1,   0}, //5
    {0, 4,   2}, //7---
    {0, 7,   6}, //8
    {1, 2,   8}, //9
    {1, 3,  13}, //10
    {1, 5,  17}, //11
    {1, 6,  20}, //12
    {1, 4,  23}, //13
    {1, 7,  26}, //14
};

const int16_t SL6800B_HP_LINTOUT_DB[7] = {
     0,
    -2,
    -7,
    -14,
    -19,
    -26,
    -27
};

float get_SL6800B_pa_amp(uint32_t index)
{
    float decibels;

    decibels = - (SL6800B_LINEOUT_LEVEL_CFG[index][2]);
    return exp(decibels * 0.115129f);
}

#define FUNC_ABS(a) ((a)<0?-(a):(a))

AT(.dac_aout_sram_seg)
void setRegLPaL(uint32_t reg, float paL)
{
    regL_state = reg;
    paL_amp_state = paL;
}

AT(.dac_aout_sram_seg)
void setRegRPaR(uint32_t reg, float paL)
{
    regR_state = reg;
    paR_amp_state = paL;
}

AT(.dac_aout_sram_seg)
void applyRegLPaL()
{
    regL = regL_state;
    paL_amp = paL_amp_state;
}

AT(.dac_aout_sram_seg)
void applyRegRPaR()
{
    regR = regR_state;
    paR_amp = paR_amp_state;
}

/* hp L channel gain: 0 ~ 6 */
AT(.com_seg)
void set_hpL_gain(uint32_t user_gain, int8_t apply)
{
    uint8_t  index = 0;
    int32_t gain = 6-user_gain;

    gain %= 7;
    float decibels;
    func_hpl_gain = SL6800B_HP_LINTOUT_DB[gain];
    decibels = (float)func_hpl_gain;
    //loge("hpl set to : %d", user_hpl_gain);

    for (int i = 0; i < 15;i++) {
        if (FUNC_ABS(SL6800B_LINEOUT_LEVEL_CFG[i][2] + func_hpl_gain) <= 1) {
            index = i;
            loge("set hpL get:%d db, func_hpl_gain : %d",
                SL6800B_LINEOUT_LEVEL_CFG[index][2], func_hpl_gain);
            break;
        }
    }
    setRegLPaL((SL6800B_LINEOUT_LEVEL_CFG[index][0] << 16) |
        (SL6800B_LINEOUT_LEVEL_CFG[index][1]), exp(decibels * 0.115129f));
    if (apply) {
        applyRegLPaL();
    }
}

/* hp R channel gain: 0 ~ 6 */
AT(.com_seg)
void set_hpR_gain(uint32_t user_gain, int8_t apply)
{
    int32_t index = 0;
    int32_t gain = 6-user_gain;
    float decibels;

    func_hpr_gain = SL6800B_HP_LINTOUT_DB[gain];
    decibels = (float)func_hpr_gain;
    //loge("hpr set to : %d", user_hpr_gain);
    for (int i = 0; i < 15;i++) {
        if (FUNC_ABS(SL6800B_LINEOUT_LEVEL_CFG[i][2] + func_hpr_gain) <= 1) {
            index = i;
            loge("set hpR get:%d db, func_hpr_gain : %d",
                SL6800B_LINEOUT_LEVEL_CFG[index][2], func_hpr_gain);
            break;
        }
    }
    setRegRPaR((SL6800B_LINEOUT_LEVEL_CFG[index][0] << 16) |
        (SL6800B_LINEOUT_LEVEL_CFG[index][1]), exp(decibels * 0.115129f));
    if (apply) {
        applyRegRPaR();
    }
}

/* lineout L channel gain: 0 ~ 6 */
AT(.com_seg)
void set_lineoutL_gain(uint32_t user_gain, int8_t apply)
{
    int8_t index = 0;
    int32_t gain = 6-user_gain;
    gain %= 7;

    int8_t target_gain = SL6800B_HP_LINTOUT_DB[gain];
    //loge("lol set to : %d, hpl is : %d", target_gain, user_hpl_gain);
    for (int i = 0; i < 15;i++) {
        if (FUNC_ABS(target_gain -
            (func_hpl_gain + SL6800B_LINEOUT_LEVEL_CFG[i][2])) <= 1) {
            loge("lol get:%d db", SL6800B_LINEOUT_LEVEL_CFG[i][2]);
            index = i;
            break;
        }
    }
    setRegLPaL((SL6800B_LINEOUT_LEVEL_CFG[index][0] << 16) |
        (SL6800B_LINEOUT_LEVEL_CFG[index][1]), paL_amp_state);
    if (apply) {
        applyRegLPaL();
    }
}

/* lineoiut R channel gain: 0 ~ 6 */
AT(.com_seg)
void set_lineoutR_gain(uint32_t user_gain, int8_t apply)
{
    int index = 0;
    int32_t gain = 6-user_gain;
    gain %= 7;

    int8_t target_gain = SL6800B_HP_LINTOUT_DB[gain];
    loge("lor set to : %d, hpr is : %d", target_gain, func_hpr_gain);
    for (int i = 0; i < 15;i++) {
        if (FUNC_ABS(target_gain -
            (func_hpr_gain + SL6800B_LINEOUT_LEVEL_CFG[i][2])) <= 1) {
            loge("lor get:%d db", SL6800B_LINEOUT_LEVEL_CFG[i][2]);
            index = i;
            break;
        }
    }
    setRegRPaR((SL6800B_LINEOUT_LEVEL_CFG[index][0] << 16) |
        (SL6800B_LINEOUT_LEVEL_CFG[index][1]), paR_amp_state);
    if (apply) {
        applyRegRPaR();
    }
}

AT(.aout_flash_seg)
void lineout_enable(bool enable)
{
    mixer_set_ctl(SND_CODEC, LINEOUTL_EN, enable);
    mixer_set_ctl(SND_CODEC, LINEOUTR_EN, enable);
}

AT(.aout_flash_seg)
void lineout_hp_gain_init(void)
{
    mainL_gain = 100;
    mainR_gain = 100;
    mainL_amp = get_amp(mainL_gain);
    mainR_amp = get_amp(mainR_gain);

    func_hpl_gain = 0;
    func_hpr_gain = 0;

    set_hpL_gain(6, true);
    set_hpR_gain(6, true);

    lineout_enable(true);
}
#endif

