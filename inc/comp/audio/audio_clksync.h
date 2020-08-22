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

#ifndef _AUDIO_CLKSYNC_H_
#define _AUDIO_CLKSYNC_H_

#include "sys_inc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct audiosync_ctrl_params_s {
    uint32_t fs;

    float adjust1_error;
    float adjust2_error;

    uint16_t max_accumulate_count;

    uint16_t normal_th;
    uint16_t adjust1_th;
    uint16_t adjust2_th;
} audiosync_ctrl_params_t;

void audiosync_ctrl_init(audiosync_ctrl_params_t *params);
void audiosync_ctrl_reset(void);
void audiosync_ctrl_proc(uint32_t delta_ref_samples);

#ifdef __cplusplus
}
#endif

#endif /* _AUDIO_CLKSYNC_H_ */
