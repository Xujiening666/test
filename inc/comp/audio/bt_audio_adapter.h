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

#ifndef _BT_AUDIO_ADAPTER_H_
#define _BT_AUDIO_ADAPTER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#define SCO_EWS_EN              0
#define A2DP_EWS_EN             0

extern volatile uint8_t btmusic_inited;
extern volatile uint8_t btphone_inited;

extern SemaphoreHandle_t sbc_xSemaphore;

#define HCI_SCO_LEN         60
#define HCI_SCO_NUM         32
#define HCI_SCO_INDEX_MASK  (HCI_SCO_NUM - 1)

#define PHONE_BLOCK_MAX_LEN         60
#define PHONE_BLOCK_NUM             16      //must be 2^n
#define PHONE_BLOCK_INDEX_MASK      (PHONE_BLOCK_NUM - 1)

#define MAX_SBC_FRAME_LEN           (119)

struct hci_control {
    volatile uint8_t event_rcv_num;
    volatile uint8_t event_proc_num;

    volatile uint8_t acl_rcv_num;
    volatile uint8_t last_acl_rcv_num;
    volatile uint8_t acl_proc_num;
    volatile uint8_t last_acl_proc_num;

    volatile uint32_t sco_rcv_num;
    volatile uint32_t sco_proc_num;
};

struct hci_sco_buf_block {
    char buf[HCI_SCO_LEN];
};

extern volatile int32_t phone_rx_loss_count;

extern volatile uint32_t phone_block_len;
extern char *phone_mic_buf;
extern volatile uint32_t phone_rcv_num;
extern volatile uint32_t phone_proc_num;
extern struct hci_control hci_control;
extern struct hci_sco_buf_block* hci_sco_buf;

void bt_audio_adapter_sco_init(void);
void bt_audio_adapter_sco_deinit(void);
void bt_audio_adapter_a2dp_init(void);
void bt_audio_adapter_a2dp_deinit(void);


void bt_phone_downlink_block_write(uint8_t *buf);
void bt_phone_uplink_block_read(uint8_t *buf);
int32_t bt_downlink_read(void *bt_handle, uint8_t *buf, uint32_t bytes);
int32_t bt_uplink_write(void *bt_handle, uint8_t *buf, uint32_t bytes);

#define A2DP_BS_PER_FRAME_MAX 7
#define SBC_INSERT_THD 100
struct a2dp_bs_seq_record_s {
    uint16_t seq_num;
    uint16_t cid;
    uint8_t per_frame_max;
    uint8_t per_frame_avg;
    uint16_t l2cap_seq_pre;
    uint16_t skip_count;
    uint8_t  insert_flg;
};
extern struct a2dp_bs_seq_record_s a2dp_bs_seq_record; // only one link record now

typedef struct {
    uint32_t sync_time;
    uint32_t loss_count;
    uint16_t frame_len;
} a2dp_bs_info_t;

typedef struct a2dp_bs_ctrl_s {
    struct ring_buffer bufctrl;

    volatile uint32_t r_count;
    volatile uint32_t w_count;

    volatile uint8_t inited;
    volatile uint8_t pre_init;
    volatile uint8_t r_refer;
    volatile uint8_t w_refer;
} a2dp_bs_ctrl_t;
extern a2dp_bs_ctrl_t a2dp_bs_ctrl;

void a2dp_bs_list_init(void);
void a2dp_bs_list_deinit(void);
uint8_t a2dp_bs_list_inited(void);

int __a2dp_bs_write(void *buf, a2dp_bs_info_t *info);
int __a2dp_bs_read(void *buf, a2dp_bs_info_t *info);

uint32_t a2dp_bs_w_avail(void);
uint32_t a2dp_bs_r_avail(void);
uint32_t a2dp_bs_get_total_num(void); //total bytes
uint32_t a2dp_bs_frame_num(void); //frame num
uint32_t a2dp_bs_get_start_th(void);
uint32_t a2dp_bs_get_sync_th(void);

void a2dp_clksync_init(uint32_t rate, uint32_t samples);
void a2dp_clksync_proc(void);


#ifdef __cplusplus
}
#endif

#endif /* _BT_AUDIO_ADAPTER_H_ */
