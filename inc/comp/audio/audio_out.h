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

#ifndef _AUDIO_OUT_H_
#define _AUDIO_OUT_H_

#include "sys_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AOUT_USE_STATIC_MEM


typedef enum audio_mode_e {
    AUDIO_MODE_NATIVE_MUSIC         = 0,
    AUDIO_MODE_TONE                 = 1,
    AUDIO_MODE_A2DP_MUSIC           = 2,
    AUDIO_MODE_SCO                  = 3,
} audio_mode_t;

typedef enum audio_ao_status_e {
    AUDIO_AO_STATUS_IDLE            = 0,
    AUDIO_AO_STATUS_PLAYING         = 1,
    AUDIO_AO_STATUS_XRUN            = 2,
} audio_ao_status_t;

typedef enum aout_cmd_e {
    SLTK_CMD_AO_SET_VOLUME                      = 0,
    SLTK_CMD_AO_SET_TONE_VOLUME                 = 1,
    SLTK_CMD_AO_SET_CALL_VOLUME                 = 2,
    SLTK_CMD_AO_GET_FADEIN_TIME                 = 3,
    SLTK_CMD_AO_SET_FADEIN_TIME                 = 4,
    SLTK_CMD_AO_GET_FADEOUT_TIME                = 5,
    SLTK_CMD_AO_SET_FADEOUT_TIME                = 6,
    SLTK_CMD_AO_START_FADE                      = 7,
    SLTK_CMD_AO_GET_FADEOUT_END_STATUS          = 8,
    SLTK_CMD_AO_GET_STATUS                      = 9,
    SLTK_CMD_AO_SET_TRIGGER_PLAY_TIME           = 10,
    SLTK_CMD_AO_GET_VOL_CTL_CALLBACK            = 11,
} aout_cmd_t;
typedef aout_cmd_t sltk_aout_cmd_t;

typedef struct aout_interface_s aout_interface_t;
typedef void (*volume_control_func)(aout_interface_t * aout, void *buf, int32_t samples);

typedef struct {
    volume_control_func frontend_vol_ctl_func;
    uint32_t fs;                    /* sampling rate(e.g. 44100) */
    uint16_t period_size;           /* The number of sample processed at one time. (NOTE: If config 0, will use default) */
    uint16_t period_count;          /* The number of period. (NOTE: If config 0, will use default) */
    uint16_t fadein_ms;             /* Fade-in time (in milliseconds) */
    uint16_t fadeout_ms;            /* Fade-out time (in milliseconds) */
    uint16_t headmute_ms;           /* The number of sample that needs to be muted before normal playing */
    uint8_t ch;                     /* channels */
    uint8_t sample_size;            /* Data size of a sample. (e.g. 2-16bit, 4-32bit) */
    uint8_t mode;                   /* aout mode (e.g. AUDIO_MODE_NATIVE_MUSIC) */
} aout_init_t;

struct aout_interface_s {
    int32_t (*init)(aout_interface_t *self, aout_init_t *init);

    int32_t (*write)(aout_interface_t *self, void *buf, int32_t samples);

    int32_t (*deinit)(aout_interface_t *self);

    int32_t (*cmd)(aout_interface_t *self,
        int32_t cmd, int32_t val);

    uint8_t (*get_status)(aout_interface_t *self);
    uint32_t (*get_remain)(aout_interface_t *self);
    int64_t (*get_play_timestamp)(aout_interface_t *self);
};

typedef enum aout_type_e {
    AOUT_FATFS,
    AOUT_DAC,
    AOUT_I2S,
    AOUT_KARAOKE,
} aout_type_t;

#ifdef __cplusplus
}
#endif

#endif /* _AUDIO_OUT_H_ */
