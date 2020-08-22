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

#ifndef _PCM_H_
#define _PCM_H_

#include "sys_types.h"
#include "sys_inc.h"

#include "ring_buffer.h"
#include "snd_hardware.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <FreeRTOS.h>
#include <portmacro.h>
#include <task.h>
#include <semphr.h>

struct pcm;

enum pcm_flag {
    PCM_OUT     = 0x1,
    PCM_IN      = 0x2,
    PCM_NO_OS   = 0x4,
};

enum pcm_format {
    /* Signed, 8-bit */
    PCM_FORMAT_S8 = sizeof(uint8_t),
    /* Signed, 16-bit, little endian */
    PCM_FORMAT_S16_LE = sizeof(int16_t),
    /* Signed, 24-bit, little endian */
    PCM_FORMAT_S24_LE = sizeof(int8_t) * 3,
    /* Signed, 32-bit, little endian */
    PCM_FORMAT_S32_LE = sizeof(int32_t),
};

enum pcm_state {
    PCM_STATE_NOT_READY,
    PCM_STATE_READY,
    PCM_STATE_SUSPENDED,
    PCM_STATE_RUNNING,
    PCM_STATE_XRUN,
};

#define GET_REFORMAT_TYPE(src_ch, src_sample_sz, dst_ch, dst_sample_sz) \
    ((((src_ch) <= 2) && ((dst_ch) <= 2)) ? (((src_ch)<<0) | ((src_sample_sz)<<8) | ((dst_ch)<<16) | ((dst_sample_sz)<<24)) : REFMT_NONE)

#define GET_SRC_CH(type)            (((type) >>  0) & 0xFF)
#define GET_SRC_SAMPLES_SIZE(type)  (((type) >>  8) & 0xFF)
#define GET_DST_CH(type)            (((type) >> 16) & 0xFF)
#define GET_DST_SAMPLES_SIZE(type)  (((type) >> 24) & 0xFF)

enum pcm_reformat_e {
    REFMT_NONE,

    REFMT_MONO16_TO_MONO16      = GET_REFORMAT_TYPE(1, 2, 1, 2),
    REFMT_MONO16_TO_MONO32      = GET_REFORMAT_TYPE(1, 2, 1, 4),
    REFMT_MONO16_TO_STEREO16    = GET_REFORMAT_TYPE(1, 2, 2, 2),
    REFMT_MONO16_TO_STEREO32    = GET_REFORMAT_TYPE(1, 2, 2, 4),

    REFMT_MONO32_TO_MONO16      = GET_REFORMAT_TYPE(1, 4, 1, 2),
    REFMT_MONO32_TO_STEREO16    = GET_REFORMAT_TYPE(1, 4, 2, 2),
    REFMT_MONO32_TO_MONO32      = GET_REFORMAT_TYPE(1, 4, 1, 4),
    REFMT_MONO32_TO_STEREO32    = GET_REFORMAT_TYPE(1, 4, 2, 4),

    REFMT_STEREO16_TO_MONO16    = GET_REFORMAT_TYPE(2, 2, 1, 2),
    REFMT_STEREO16_TO_STEREO16  = GET_REFORMAT_TYPE(2, 2, 2, 2),
    REFMT_STEREO16_TO_MONO32    = GET_REFORMAT_TYPE(2, 2, 1, 4),
    REFMT_STEREO16_TO_STEREO32  = GET_REFORMAT_TYPE(2, 2, 2, 4),

    REFMT_STEREO32_TO_MONO16    = GET_REFORMAT_TYPE(2, 4, 1, 2),
    REFMT_STEREO32_TO_MONO32    = GET_REFORMAT_TYPE(2, 4, 1, 4),
    REFMT_STEREO32_TO_STEREO16  = GET_REFORMAT_TYPE(2, 4, 2, 2),
    REFMT_STEREO32_TO_STEREO32  = GET_REFORMAT_TYPE(2, 4, 2, 4),
};

typedef void (*pcm_reformat_hook_t)(void *dst, const void *src, uint32_t samples);

struct pcm_config {
    /* The number of channels in a frame */
    uint32_t ch;
    uint32_t sample_size;
    /* The number of frames per second */
    uint32_t rate;
    /* The number of frames in a period */
    uint32_t period_size;
    /* The number of periods in a PCM */
    uint32_t period_count;

    uint32_t start_threshold;
    uint32_t stop_threshold;

    uint8_t *buf;

    uint32_t play_time;
    bool set_play_time;
};

struct pcm {
    uint32_t id;
    enum pcm_flag pcm_flag;
    struct pcm_config config;

    volatile enum pcm_state state;

    struct audio_share_buffer sharebuffer;

    uint32_t hw_period_bytes;

    struct snd_hardware *hw;
};

struct pcm *pcm_open(uint32_t id,
    enum pcm_flag flag, const struct pcm_config *config);
int pcm_close(struct pcm *pcm);

int pcm_start(struct pcm *pcm);
int pcm_stop(struct pcm *pcm);

bool pcm_is_ready(const struct pcm *pcm);

int pcm_write(struct pcm *pcm, const void *data, uint32_t samples);
int pcm_read(struct pcm *pcm, void *data, uint32_t samples);

uint8_t pcm_get_hardware_status(struct pcm *pcm);
uint32_t pcm_get_remain(struct pcm *pcm);
int64_t pcm_get_play_timpstamp(struct pcm *pcm);

bool sndcard_online(uint32_t id);

#ifdef __cplusplus
}
#endif

#endif /* _PCM_H_ */
