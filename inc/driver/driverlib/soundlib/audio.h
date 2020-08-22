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
#ifndef __AUDIO_H
#define __AUDIO_H

#include "sys_types.h"

#ifdef __cplusplus
 extern "C" {
#endif

typedef enum {
    AUDIO_FORMAT_INVALID                = 0xFFFFFFFFUL,
    AUDIO_FORMAT_DEFAULT                = 0x00000000UL,

    /* PCM format */
    AUDIO_FORMAT_PCM_16_BIT             = 0x1,
    AUDIO_FORMAT_PCM_8_BIT              = 0x2,
    AUDIO_FORMAT_PCM_32_BIT             = 0x4,
    AUDIO_FORMAT_PCM_8_24_BIT           = 0x8,
    AUDIO_FORMAT_PCM_FLOAT              = 0x10,
    AUDIO_FORMAT_PCM_24_BIT_PACKED      = 0x20,
} audio_format_t;


/* channel mask */
typedef enum {
    AUDIO_CHANNEL_INVALID       = 0x0,
    AUDIO_CHANNEL_MONO          = 0x1,
    AUDIO_CHANNEL_STEREO        = 0x2,
    AUDIO_CHANNEL_2POINT1       = 0x4,
    AUDIO_CHANNEL_QUAD          = 0x8,
    AUDIO_CHANNEL_5POINT1       = 0x10,
    AUDIO_CHANNEL_7POINT1       = 0x20,
} audio_channels_mask_t;


typedef enum {
    AUDIO_8K_RATE_MASK          = (0x1 << 0),
    AUDIO_16K_RATE_MASK         = (0x1 << 1),
    AUDIO_24K_RATE_MASK         = (0x1 << 2),
    AUDIO_32K_RATE_MASK         = (0x1 << 3),
    AUDIO_48K_RATE_MASK         = (0x1 << 4),
    AUDIO_64K_RATE_MASK         = (0x1 << 5),
    AUDIO_96K_RATE_MASK         = (0x1 << 6),
    AUDIO_192K_RATE_MASK        = (0x1 << 7),
    AUDIO_8K_SERIES_RATE_MASK   =
        AUDIO_8K_RATE_MASK |
        AUDIO_16K_RATE_MASK |
        AUDIO_24K_RATE_MASK |
        AUDIO_32K_RATE_MASK |
        AUDIO_48K_RATE_MASK |
        AUDIO_64K_RATE_MASK |
        AUDIO_96K_RATE_MASK |
        AUDIO_192K_RATE_MASK,

    /* 11025 Hz */
    AUDIO_11K_RATE_MASK         = (0x10000 << 0),
    /* 22050 Hz */
    AUDIO_22K_RATE_MASK         = (0x10000 << 1),
    /* 44100 Hz */
    AUDIO_44K_RATE_MASK         = (0x10000 << 2),
    /* 88200 Hz */
    AUDIO_88K_RATE_MASK         = (0x10000 << 3),
    /* 176400 Hz */
    AUDIO_176K_RATE_MASK        = (0x10000 << 4),
    AUDIO_11K_SERIES_RATE_MASK  =
        AUDIO_11K_RATE_MASK |
        AUDIO_22K_RATE_MASK |
        AUDIO_44K_RATE_MASK |
        AUDIO_88K_RATE_MASK |
        AUDIO_176K_RATE_MASK,

    AUDIO_ALL_RATE_MASK         =
        AUDIO_8K_SERIES_RATE_MASK |
        AUDIO_11K_SERIES_RATE_MASK,

    AUDIO_UNKNOWN_RATE_MASK     = ~AUDIO_ALL_RATE_MASK,
} audio_rate_mask_t;

struct pcm_mask_info {
    uint32_t rate_mask;
    uint32_t ch_mask;
    uint32_t format_mask;
};

static inline uint32_t audio_rate_to_mask(uint32_t rate)
{
    switch (rate) {
    case 8000:
        return AUDIO_8K_RATE_MASK;

    case 16000:
        return AUDIO_16K_RATE_MASK;

    case 24000:
        return AUDIO_24K_RATE_MASK;

    case 32000:
        return AUDIO_32K_RATE_MASK;

    case 48000:
        return AUDIO_48K_RATE_MASK;

    case 64000:
        return AUDIO_64K_RATE_MASK;

    case 96000:
        return AUDIO_96K_RATE_MASK;

    case 192000:
        return AUDIO_192K_RATE_MASK;

    case 11025:
        return AUDIO_11K_RATE_MASK;

    case 22050:
        return AUDIO_22K_RATE_MASK;

    case 44100:
        return AUDIO_44K_RATE_MASK;

    case 88200:
        return AUDIO_88K_RATE_MASK;

    case 176400:
        return AUDIO_176K_RATE_MASK;

    default:
        return AUDIO_UNKNOWN_RATE_MASK;
    }

    return AUDIO_UNKNOWN_RATE_MASK;
}

static inline uint32_t audio_mask_to_rate(uint32_t rate_mask)
{
    switch (rate_mask) {
    case AUDIO_8K_RATE_MASK:
        return 8000;

    case AUDIO_16K_RATE_MASK:
        return 16000;

    case AUDIO_24K_RATE_MASK:
        return 24000;

    case AUDIO_32K_RATE_MASK:
        return 32000;

    case AUDIO_48K_RATE_MASK:
        return 48000;

    case AUDIO_64K_RATE_MASK:
        return 64000;

    case AUDIO_96K_RATE_MASK:
        return 96000;

    case AUDIO_192K_RATE_MASK:
        return 19200;

    case AUDIO_11K_RATE_MASK:
        return 11025;

    case AUDIO_22K_RATE_MASK:
        return 22050;

    case AUDIO_44K_RATE_MASK:
        return 44100;

    case AUDIO_88K_RATE_MASK:
        return 88200;

    case AUDIO_176K_RATE_MASK:
        return 176400;

    default:
        return 48000;
    }

    return 48000;
}



static inline uint32_t audio_bytes_per_sample(audio_format_t format)
{
    uint32_t  size = 0;

    switch (format) {
    case AUDIO_FORMAT_PCM_32_BIT:
    case AUDIO_FORMAT_PCM_8_24_BIT:
        size = sizeof(uint32_t);
        break;
    case AUDIO_FORMAT_PCM_24_BIT_PACKED:
        size = sizeof(uint8_t) * 3;
        break;
    case AUDIO_FORMAT_PCM_16_BIT:
        size = sizeof(uint16_t);
        break;
    case AUDIO_FORMAT_PCM_8_BIT:
        size = sizeof(uint8_t);
        break;
    case AUDIO_FORMAT_PCM_FLOAT:
        size = sizeof(float);
        break;
    default:
        break;
    }

    return size;
}

static inline uint32_t get_audio_format_mask(uint32_t sample_size)
{

    switch (sample_size) {
    case 4:
        return AUDIO_FORMAT_PCM_32_BIT;

    case 3:
        return AUDIO_FORMAT_PCM_24_BIT_PACKED;

    case 2:
        return AUDIO_FORMAT_PCM_16_BIT;

    case 1:
        return AUDIO_FORMAT_PCM_8_BIT;

    default:
        break;
    }

    return 4;
}


static inline int popcount(uint32_t x)
{
    return __builtin_popcount(x);
}

static inline int clz(uint32_t x)
{
    return __builtin_clz(x);
}

static inline uint32_t return_address(uint32_t x)
{
    return (uint32_t)__builtin_return_address(x);
}

static inline int get_audio_channels_count(audio_channels_mask_t channels_mask)
{

    switch (channels_mask) {
    case AUDIO_CHANNEL_MONO:
        return 1;

    case AUDIO_CHANNEL_STEREO:
        return 2;

    case AUDIO_CHANNEL_2POINT1:
        return 3;

    case AUDIO_CHANNEL_QUAD:
        return 4;

    case AUDIO_CHANNEL_5POINT1:
        return 6;

    case AUDIO_CHANNEL_7POINT1:
        return 8;

    default:
        return 1;
    }
}

static inline audio_channels_mask_t get_audio_channels_mask(int channels_count)
{
    switch (channels_count) {
    case 1:
        return AUDIO_CHANNEL_MONO;
    case 2:
        return AUDIO_CHANNEL_STEREO;
    case 3:
        return AUDIO_CHANNEL_2POINT1;
    case 4:
        return AUDIO_CHANNEL_QUAD;
    case 6:
        return AUDIO_CHANNEL_5POINT1;
    case 8:
        return AUDIO_CHANNEL_7POINT1;
    default:
        return AUDIO_CHANNEL_INVALID;
    }

}


static inline audio_format_t get_audio_format_by_bits(int bits_per_sample)
{
    audio_format_t format = AUDIO_FORMAT_INVALID;

    switch (bits_per_sample) {
    case 8:
        format = AUDIO_FORMAT_PCM_8_BIT;
        break;
    case 16:
        format = AUDIO_FORMAT_PCM_16_BIT;
        break;
    case 24:
        format = AUDIO_FORMAT_PCM_24_BIT_PACKED;
        break;
    case 32:
        format = AUDIO_FORMAT_PCM_32_BIT;
        break;
    default:
        break;
    }

    return format;
}


#ifdef __cplusplus
}
#endif

#endif



