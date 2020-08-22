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
#ifndef _SND_HARDWARE_H_
#define _SND_HARDWARE_H_

#include "sys_types.h"
#include "ring_buffer.h"
#include "hal_dma_lib.h"

#ifdef __cplusplus
extern "C" {
#endif

enum pcm_hw_trigger_cmd {
    PCM_HW_TRIGGER_START,
    PCM_HW_TRIGGER_RESUME,
    PCM_HW_TRIGGER_STOP,
    PCM_HW_TRIGGER_PAUSE,
};

enum pcm_hw_direction {
    PCM_HW_DIR_OUT,
    PCM_HW_DIR_IN,
};

enum pcm_hw_state {
    PCM_HW_STATE_STOP,
    PCM_HW_STATE_IDLE,
    PCM_HW_STATE_RUNNING,
    PCM_HW_STATE_SUSPENDED,
    PCM_HW_STATE_XRUN,
    PCM_HW_STATE_STOPED,
};

struct pcm_hw_params {
    uint32_t channels;
    uint32_t rate;
    uint32_t sample_size;

    uint32_t period_size;
    uint32_t period_count;

    void *sw_context;
};

typedef enum snd_hardware_ret_e {
    SND_HW_RET_NORAML = 0,
    SND_HW_INIT_FAIL = -1,
    SND_HW_RET_STREAM_END = -2,
    SND_HW_RET_XRUN = -3,
} sndhw_ret;

struct snd_hardware_info {
    uint8_t sample_size;
    uint8_t channels;
};

struct snd_hardware;

struct snd_hardware_ops {
    int (*get_info)(struct snd_hardware *hw, enum pcm_hw_direction dir, struct snd_hardware_info *info);
    int (*hw_params)(struct snd_hardware *hw,
        enum pcm_hw_direction dir, struct pcm_hw_params *params);
    int (*hw_free)(struct snd_hardware *hw, enum pcm_hw_direction dir);
    int (*prepare)(struct snd_hardware *hw, enum pcm_hw_direction dir);
    int (*trigger)(struct snd_hardware *hw,
        enum pcm_hw_direction dir, enum pcm_hw_trigger_cmd cmd);
    int (*hw_set_ctl)(struct snd_hardware *hw, uint32_t ctl, uint32_t val);
    int (*hw_get_ctl)(struct snd_hardware *hw, uint32_t ctl, uint32_t *val);
};


struct snd_hardware {
    struct snd_hardware_ops ops;

    void *tx_context;
    void *rx_context;
    uint32_t txxrun_count;
    uint32_t rxxrun_count;
    void *tx_dma;
    void *rx_dma;
    uint32_t txfifo_addr;
    uint32_t rxfifo_addr;

    volatile enum pcm_hw_state last_out_state;
    volatile enum pcm_hw_state out_state;
    volatile enum pcm_hw_state in_state;
    uint32_t out_bytes_per_second;
    uint32_t play_time;
    volatile uint32_t trigger_time;
    volatile uint64_t play_period_count;

    uint16_t tx_dma_bytes;
    uint16_t rx_dma_bytes;
    volatile bool out_wait_stop;
    volatile bool in_wait_stop;
    volatile bool wait_play;
};

int get_snd_hardware(struct snd_hardware **snd_hw, uint32_t id);

void hw_update_rptr(void *sw_context);
void *hw_get_rptr(void *sw_context);
uint32_t hw_get_r_avail(void *sw_context);
void hw_update_wptr(void *sw_context);
void *hw_get_wptr(void *sw_context);
uint32_t hw_get_w_avail(void *sw_context);

sndhw_ret snd_dma_out_callback(struct snd_hardware *hw);
sndhw_ret snd_dma_in_callback(struct snd_hardware *hw);
sndhw_ret snd_dma_params(struct snd_hardware *hw,
    enum pcm_hw_direction dir,
    struct pcm_hw_params *params,
    dma_channel_config_t *config);
sndhw_ret snd_dma_prepare(struct snd_hardware *hw, enum pcm_hw_direction dir);
sndhw_ret snd_dma_free_early(struct snd_hardware *hw,
    enum pcm_hw_direction dir);
sndhw_ret snd_dma_free(struct snd_hardware *hw, enum pcm_hw_direction dir);
sndhw_ret snd_dma_trigger(struct snd_hardware *hw,
    enum pcm_hw_direction dir, enum pcm_hw_trigger_cmd cmd);

#ifdef __cplusplus
}
#endif

#endif /* _SND_HARDWARE_H_ */
