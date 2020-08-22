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

#ifndef _AUDIO_IN_H_
#define _AUDIO_IN_H_

#include "sys_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @brief: audio_in_mode indicates what scenarios pcm data is used for
 */
typedef enum audio_in_mode_e {
    AUDIO_IN_MODE_NATIVE_REC = 0,
    AUDIO_IN_MODE_LINEIN     = 1,
    AUDIO_IN_MODE_SCO        = 2,
    AUDIO_IN_MODE_MIXER      = 3,
} audio_in_mode_t;

/*
 * @brief: audio in command
 */
typedef enum ain_cmd_e {
    /* set ain volume(range: 0 ~ 100)
     * Example:
     *      //set volume 50
     *      ain->cmd(ain, SLTK_CMD_AI_SET_VOLUME, 50);
     */
    SLTK_CMD_AI_SET_VOLUME    = 0,

    /* start fade-in or fade-out
     * Example:
     *      //start fade-in
     *      ain->cmd(ain, SLTK_CMD_AI_START_FADE, FADE_IN);
     *      //start fade-out
     *      ain->cmd(ain, SLTK_CMD_AI_START_FADE, FADE_OUT);
     */
    SLTK_CMD_AI_START_FADE    = 1,
} ain_cmd_t;

typedef ain_cmd_t sltk_ain_cmd_t;
typedef struct ain_interface_s ain_interface_t;

typedef struct {
    uint32_t fs;            /* sampling rate(e.g. 44100) */
    uint16_t fadein_ms;     /* Fade-in time (in milliseconds) */
    uint16_t fadeout_ms;    /* Fade-out time (in milliseconds) */
    uint16_t headmute_ms;   /* The number of sample that needs to be muted before normal recording */
    uint16_t period_size;   /* The number of sample processed at one time. (NOTE: If config 0, will use default) */
    uint16_t period_count;  /* The number of period. (NOTE: If config 0, will use default) */
    uint8_t ch;             /* channels */
    uint8_t sample_size;    /* Data size of a sample. (e.g. 2-16bit, 4-32bit) */
    uint8_t mode;           /* ain mode (e.g. AUDIO_IN_MODE_NATIVE_REC) */
} ain_init_t;

struct ain_interface_s {
    int32_t (*init)(ain_interface_t *self, ain_init_t *init);
    int32_t (*read)(ain_interface_t *self, void *buf, int32_t samples);
    int32_t (*deinit)(ain_interface_t *self);
    int32_t (*cmd)(ain_interface_t *self,
        int32_t cmd, int32_t val);
};

typedef enum ain_type_e {
    AIN_ADC         = 0,        /* for read hardware(e.g. ADC, I2S) pcm data */
    AIN_RECORD      = 1,        /* for karaoke mic record, linein/fm record */
} ain_type_t;

typedef ain_interface_t* (*create_ain_t)(uint32_t type);
typedef void (*destroy_ain_t)(ain_interface_t *ain);

typedef int32_t (*record_callback_func_t)(void *callback_handle, int32_t *buf, int32_t samples);

typedef struct {
    record_callback_func_t func;
    void *handle;
} record_callback_context_t;

ain_interface_t* create_recorder_ain(void);


#ifdef __cplusplus
}
#endif

#endif /* _AUDIO_IN_H_ */
