#define LOG_LEV 4
#include "sys_inc.h"
#include "sltk_audio.h"
#include "audio_service.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(X) (sizeof(X) / sizeof(X[0]))
#endif


#if 0
/*
 * 重写最大最小音量提示音正弦波数据
 */
AT(.tone_mixer_rodata_sram_seg)
const int32_t volume_sine_tone_data[16] = {
    0x0,
    0xCF043AB3,
    0xA57D8667,
    0X89BE50C4,
    0X80000000,
    0X89BE50C4,
    0XA57D8667,
    0XCF043AB3,
    0X0,
    0X30FBC54D,
    0X5A827999,
    0X7641AF3C,
    0X7FFFFFFF,
    0X7641AF3C,
    0X5A827999,
    0X30FBC54D,
};

/*
 * 重写最大最小音量提示音信息
 */
AT(.tone_mixer_rodata_sram_seg)
const sine_tone_info_t volume_sine_tone_info = {
    .tone_gain = (float)0.1,                        /* 提示音增益 */
    .ducking_gain = (float)0.03,                    /* 闪避增益（播放提示音时，音乐被压制的增益值） */
    .sin_samples = 48000 * 50 / 1000,               /* 正弦波播放多长时间（单位:采样点） */
    .mute_samples = 48000 * 200 / 1000,             /* 静音播放多长时间（单位:采样点） */
    .period_count = 2,                              /* 播放多少个”正弦波-静音“周期 */
    .psinbuf = volume_sine_tone_data,               /* 指定正弦波数据 */
    .sinbuf_len = ARRAY_SIZE(volume_sine_tone_data) /* 正弦波数据长度 */
};

/*
 * 重写低电量提示音信息
 */
AT(.tone_mixer_rodata_sram_seg)
const sine_tone_info_t low_power_sine_tone_info = {
    .tone_gain = (float)0.1,                            /* 提示音增益 */
    .ducking_gain = (float)0.03,                        /* 闪避增益（播放提示音时，音乐被压制的增益值） */
    .sin_samples = 48000 * 50 / 1000,                   /* 正弦波播放多长时间（单位:采样点） */
    .mute_samples = 48000 * 200 / 1000,                 /* 静音播放多长时间（单位:采样点） */
    .period_count = 6,                                  /* 播放多少个”正弦波-静音“周期 */
    .psinbuf = volume_sine_tone_data,                   /* 指定正弦波数据 */
    .sinbuf_len = ARRAY_SIZE(volume_sine_tone_data)     /* 正弦波数据长度 */
};
#endif

#if 0
/*
 * 重写app自定义正弦波提示音
 */
AT(.tone_mixer_rodata_sram_seg)
const int32_t app_sine_tone_data[16] = {
    0x0,
    0xCF043AB3,
    0xA57D8667,
    0X89BE50C4,
    0X80000000,
    0X89BE50C4,
    0XA57D8667,
    0XCF043AB3,
    0X0,
    0X30FBC54D,
    0X5A827999,
    0X7641AF3C,
    0X7FFFFFFF,
    0X7641AF3C,
    0X5A827999,
    0X30FBC54D,
};

/*
 * 重写app自定义正弦波提示音信息
 */
sine_tone_info_t app_sine_tone_info = {
    .tone_gain = (float)0.1,                        /* 提示音增益 */
    .ducking_gain = (float)0.03,                    /* 闪避增益（播放提示音时，音乐被压制的增益值） */
    .sin_samples = 48000 * 50 / 1000,               /* 正弦波播放多长时间（单位:采样点） */
    .mute_samples = 48000 * 200 / 1000,             /* 静音播放多长时间（单位:采样点） */
    .period_count = 1,                              /* 播放多少个”正弦波-静音“周期 */
    .psinbuf = app_sine_tone_data,                  /* 指定正弦波数据 */
    .sinbuf_len = ARRAY_SIZE(app_sine_tone_data)    /* 正弦波数据长度 */
};

/* 播放app自定义的正弦波提示音；
 * @param count: 播放多少个“正弦波-静音”周期
 * @return: 无
 */
void play_app_sin_tone(uint32_t count)
{
    app_sine_tone_info.period_count = count;
    audio_service_play_sin_tone(SIN_TONE_APP);
}
#endif
