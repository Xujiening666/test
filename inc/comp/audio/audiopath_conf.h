/* The Alps Open Source Project
 * Copyright (c) 2016 - 2017 Smartlink Technology Co. Ltd. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _AUDIOPATH_CONF_H_
#define _AUDIOPATH_CONF_H_

#include "slsoundlib.h"
#include "audio_service.h"

#ifdef __cplusplus
extern "C" {
#endif

//硬件播放声道数
#if BT_EWS_EN
#define AUDIO_PLAYBACK_DEVICE_CH                1
#else
#define AUDIO_PLAYBACK_DEVICE_CH                2
#endif
//硬件播放采样精度
#define AUDIO_PLAYBACK_DEVICE_SAMPLE_SIZE       4


//硬件录音最大声道数
#if (AUDIO_IN_DEVICE_SEL == 0) /* i2s */
/* i2s mic params */
#define AUDIO_REC_DEVICE_MAX_CH        2
#else                          /* audiocodec */
#if AUDIO_KARAOKE_MODE
#define AUDIO_REC_DEVICE_MAX_CH        2
#else
#define AUDIO_REC_DEVICE_MAX_CH        2
#endif
#endif
//硬件录音采样精度
#define AUDIO_REC_DEVICE_SAMPLE_SIZE   4


//audiocodec用户配置
void codec_user_config(void);


//配置播放通路；此函数由audio库调用
void playback_path_on(void);

/* 打开录音通路；
 * 此函数由audio库调用，打开MIC，linein等ADC输入通路
 * @param mode: 表示当前属于什么模式。参考 @audio_in_mode_t
 */
void record_path_on(uint8_t mode);

/* 关闭录音通路；
 * 此函数由audio库调用，关闭ADC输入通路
 * @param mode: 表示当前属于什么模式。参考 @audio_in_mode_t
 */
void record_path_off(uint8_t mode);

/* 音量转换成增益值
 * @param vol: 音量值（范围：0~100）
 * @return: 返回增益值(范围0.0 ~ 1.0)
 */
float get_amp(int32_t vol);

/* 音量转换成分贝（dB)值函数
 * @param vol: 音量值（范围：0~100）
 * @return: dB值
 */
float vol2decibels(int32_t vol);

/* 设置DAC音量
 * @param volume:音量值（0~100）
 */
void set_dac_volume(uint8_t volume);

/* 设置MIC数字音量
 * @param volume:音量值（0~100）
 */
void set_mic_adc_volume(uint8_t volume);

/* 根据user_config.h配置ADC数字音量
 * @param input: 输入源选择
 */
void load_digital_gain_config(enum audio_input_source_e input);

/* 获取输出声卡ID
 * @return: 0：I2S输出； 1：codec DAC输出
 */
uint8_t get_out_sndcard(void);

/* 获取输入声卡ID
 * @return: 0：I2S输入； 1：codec ADC输入
 */
uint8_t get_in_sndcard(void);

#if AUDIO_ADJUST_HPLR_LINEOUTLR

/* HP lineout左右声道独立调节相关接口 */
void lineout_hp_gain_init(void); /* 初始化左右声道独立调节音量 */
void set_mainL_gain(uint32_t gain); /* 设置左声道音量: 0~100 */
void set_mainR_gain(uint32_t gain); /* 设置右声道音量: 0~100 */
void set_hpL_gain(uint32_t user_gain, int8_t apply); /* 设置HPL音量: 0 ~ 6 */
void set_hpR_gain(uint32_t user_gain, int8_t apply); /* 设置HPR音量: 0 ~ 6 */
void set_lineoutL_gain(uint32_t user_gain, int8_t apply); /* 设置lineoutL音量: 0 ~ 6 */
void set_lineoutR_gain(uint32_t user_gain, int8_t apply); /* 设置lineoutR音量: 0 ~ 6 */
#endif

#ifdef __cplusplus
}
#endif

#endif
