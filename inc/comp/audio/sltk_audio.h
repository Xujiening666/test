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

#ifndef _SLTK_AUDIO_H_
#define _SLTK_AUDIO_H_

#include "sys_types.h"
#include "audio_internal_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SLTK_STYPE_FATFS = 0x0, /* The audio data comes from fatfs */
    SLTK_STYPE_FLASH_TONE,      /* The tone data comes from tonefs(flash) */
    SLTK_STYPE_FLASH_AUXTRACK,
    SLTK_STYPE_FLASH_RECORD,
} streamtype_t;

typedef enum {
    SLTK_TTYPE_FILE,
    SLTK_TTYPE_DAC,
} terminal_t;

/* container type, for loading parser lib */
typedef enum {
    SLTK_CTYPE_UNSUPPORTED = -1,
    SLTK_CTYPE_MP3 = 0x0,
    SLTK_CTYPE_WMA,
    SLTK_CTYPE_AAC,
    SLTK_CTYPE_M4A,
    SLTK_CTYPE_APE,
    SLTK_CTYPE_FLAC,
    SLTK_CTYPE_OGG,
    SLTK_CTYPE_WAV,
    SLTK_CTYPE_SBC,
    SLTK_CTYPE_NUM
} ctype_t;
typedef ctype_t sltk_ctype_t;

/* codec type, for loading decoder lib */
typedef enum {
    SLTK_ATYPE_UNSUPPORTED = -1,
    SLTK_ATYPE_MP3 = 0x0,
    SLTK_ATYPE_WMA,
    SLTK_ATYPE_AAC,
    SLTK_ATYPE_APE,
    SLTK_ATYPE_FLAC,
    SLTK_ATYPE_VORBIS,
    SLTK_ATYPE_PCM,
    SLTK_ATYPE_SBC,
} atype_t;
typedef atype_t sltk_atype_t;

typedef struct {
    /* codec type */
    atype_t codec_type;
    /* total time in ms */
    int32_t total_time;
    /* average bit rate in kbps */
    int32_t avg_br;
    /* sampling rate */
    int32_t fs;
    /* channels number */
    int32_t channels;
    /* frame size */
    int32_t chunk_size;
    /* bs buffer */
    uint8_t *bs_buf;
    /* init buffer accordance with the codec type */
    void *init_data;
} stream_info_t;
typedef stream_info_t sltk_stream_info_t;



typedef struct {
    uint8_t fmt_ext[8];
    uint8_t codec_ext[8];
    /* total time in ms */
    int32_t total_time;
    /* average bit rate in kbps */
    int32_t avg_br;
    /* sampling rate */
    int32_t fs;
    /* channels number */
    int32_t channels;
    /* media or muxer type */
    ctype_t media_type;
} media_info_t;

typedef struct {
    float tone_gain;            /* 提示音增益（范围:0.0~1.0） */
    float ducking_gain;         /* 闪避增益（播放提示音时，音乐被压制的增益值, 范围:0.0~1.0） */
    int32_t sin_samples;        /* 正弦波播放多长时间（单位:采样点） */
    int32_t mute_samples;       /* 静音播放多长时间（单位:采样点） */
    int32_t period_count;       /* 播放多少个”正弦波-静音“周期 */
    const int32_t *psinbuf;     /* 指定正弦波数据 */
    int32_t sinbuf_len;         /* 正弦波数据长度 */
} sine_tone_info_t;

void stereo32_to_mono32(int32_t *pout, const int32_t *pin, int32_t samples);
void stereo32_to_mono16(int16_t *pout, const int32_t *pin, int32_t samples);
void stereo32_to_stereo16(int16_t *pout, const int32_t *pin, int32_t samples);
void stereo16_to_mono32(int32_t *pout, const int16_t *pin, int32_t samples);
void stereo16_to_mono16(int16_t *pout, const int16_t *pin, int32_t samples);
void stereo16_to_stereo32(int32_t *pout, const int16_t *pin, int32_t samples);
void mono32_to_mono16(int16_t *pout, const int32_t *pin, int32_t samples);
void mono32_to_stereo16(int16_t *pout, const int32_t *pin, int32_t samples);
void mono32_to_stereo32(int32_t *pout, const int32_t *pin, int32_t samples);
void mono16_to_mono32(int32_t *pout, const int16_t *pin, int32_t samples);
void mono16_to_stereo16(int16_t *pout, const int16_t *pin, int32_t samples);
void mono16_to_stereo32(int32_t *pout, const int16_t *pin, int32_t samples);
void stereo32_interleave_notinterleave_cnv(int32_t *io, int32_t samples, int8_t dir);

typedef void (*reformat_func_t)(void *pout, const void *pin, int32_t samples);

#define NHS_INT32_SAT(x,Q)    ((x)> ((int32_t)((uint32_t)1<<(Q))-1) ? \
    ((int32_t)((uint32_t)1<<(Q))-1): \
    ((x) < (-(int32_t)((uint32_t)1<<(Q))) ? \
    (-(int32_t)((uint32_t)1<<(Q))) : (x)))

static __attribute__((always_inline)) inline
int16_t audio_mix16(int16_t a, int16_t b)
{
    int32_t c;

    a >>= 1;
    b >>= 1;
    c = (a + b) << 16;

    return (int16_t)(NHS_INT32_SAT(c, 30) >> 15);
}

static __attribute__((always_inline)) inline
int32_t audio_mix32(int32_t a, int32_t b)
{
    int32_t c;

    a >>= 1;
    b >>= 1;
    c = (a + b);

    return NHS_INT32_SAT(c, 30) << 1;
}

float volume_to_amp(int32_t vol, float max_decibels);

#ifdef __cplusplus
}
#endif

#endif /* _SLTK_AUDIO_H_ */
