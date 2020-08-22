#ifndef _APP_EWS_H
#define _APP_EWS_H

//#ifdef EWS_SERVICE_SUPPORT

//#include "ews_service.h"

#define EWS_LOCAL_VERSION     (1)
#define EWS_VERSION_INVALID   (0xff)

typedef enum
{
    APP_EWS_IDLE = 0,
    APP_EWS_CONNECTTING,
    APP_EWS_CONNECTED,
    APP_TWS_DISCONNECTTING,
    APP_EWS_DISCONNECTED,
    APP_EWS_RECONNECTTING,
}  ews_state_e;

typedef enum
{
    EWS_CHANNEL_SINGLE = 0,
    EWS_CHANNEL_STEREO,
    EWS_CHANNEL_LEFT,
    EWS_CHANNEL_RIGHT,
} ews_channel_type_e;

typedef enum
{
   EWS_INIT = 0,
   EWS_MASTER,
   EWS_SLAVE,
} ews_dev_mode_e;

enum audio_cmd_e {
    EWS_AUDIO_CMD_NONE        = 0x00,
    EWS_AUDIO_RSP_NONE        = 0x01,

    EWS_AUDIO_CMD_A2DP_START  = 0x02,
    EWS_AUDIO_RSP_A2DP_START  = 0x03,

    EWS_AUDIO_CMD_A2DP_STOP   = 0x04,
    EWS_AUDIO_RSP_A2DP_STOP   = 0x05,

    EWS_AUDIO_CMD_SCO_START  = 0x06,
    EWS_AUDIO_RSP_SCO_START  = 0x07,

    EWS_AUDIO_CMD_SCO_STOP   = 0x08,
    EWS_AUDIO_RSP_SCO_STOP   = 0x09,

    EWS_AUDIO_CMD_PLAY_TONE   = 0x0A,
    EWS_AUDIO_RSP_PLAY_TONE   = 0x0B,

    EWS_AUDIO_CMD_STOP_TONE   = 0x0C,
    EWS_AUDIO_RSP_STOP_TONE   = 0x0D,

    EWS_AUDIO_CMD_PLAY_STATUS   = 0x0E,
    EWS_AUDIO_RSP_PLAY_STATUS   = 0x0F,

    EWS_AUDIO_CMD_SCO_UNMUTE  = 0x12,
    EWS_AUDIO_RSP_SCO_UNMUTE  = 0x13,

    EWS_AUDIO_CMD_SCO_SYNC  = 0x14,
    EWS_AUDIO_RSP_SCO_SYNC  = 0x15,

    EWS_AUDIO_CMD_MUSIC_SYNC  = 0x16,
    EWS_AUDIO_RSP_MUSIC_SYNC  = 0x17,

    EWS_AUDIO_CMD_CHANGE_MUSIC_VOLUME         = 0x18,
    EWS_AUDIO_RSP_CHANGE_MUSIC_VOLUME         = 0x19,

    EWS_AUDIO_CMD_NOTIFY_CHANGE_MUSIC_VOLUME = 0x1A,
    EWS_AUDIO_RSP_NOTIFY_CHANGE_MUSIC_VOLUME = 0x1B,

    EWS_AUDIO_CMD_CHANGE_CALL_VOLUME         = 0x1C,
    EWS_AUDIO_RSP_CHANGE_CALL_VOLUME         = 0x1D,

    EWS_AUDIO_CMD_NOTIFY_CHANGE_CALL_VOLUME = 0x1E,
    EWS_AUDIO_RSP_NOTIFY_CHANGE_CALL_VOLUME = 0x1F,

    EWS_AUDIO_CMD_MUSIC_PLL  = 0x20,
    EWS_AUDIO_RSP_MUSIC_PLL  = 0x21,

    EWS_AUDIO_CMD_SAVE_LINKKEY  = 0x22,
    EWS_AUDIO_RSP_SAVE_LINKKEY  = 0x23,
};

enum app_cmd_e {
    EWS_APP_CMD_NONE                        = 0x00,
    EWS_APP_RSP_NONE                        = 0x01,

    EWS_APP_CMD_G_SENSOR_DOUBLE_TAP         = 0x02,
    EWS_APP_RSP_G_SENSOR_DOUBLE_TAP         = 0x03,

    EWS_APP_CMD_P_SENSOR_LT                 = 0x04,
    EWS_APP_RSP_P_SENSOR_LT                 = 0x05,

    EWS_APP_CMD_P_SENSOR_HT                 = 0x06,
    EWS_APP_RSP_P_SENSOR_HT                 = 0x07,

    EWS_APP_CMD_P_SENSOR_PS_SYNC            = 0x08,
    EWS_APP_RSP_P_SENSOR_PS_SYNC            = 0x09,

    EWS_APP_CMD_PHONE_CONNECTED             = 0x0A,
    EWS_APP_RSP_PHONE_CONNECTED             = 0x0B,

    EWS_APP_CMD_PHONE_DISCONNECTED          = 0x0C,
    EWS_APP_RSP_PHONE_DISCONNECTED          = 0x0D,

    EWS_APP_CMD_EQ_INIT                     = 0x0E,
    EWS_APP_RSP_EQ_INIT                     = 0x0F,

    EWS_APP_CMD_EQ_EXIT                     = 0x10,
    EWS_APP_RSP_EQ_EXIT                     = 0x11,

    EWS_APP_CMD_EQ_CHANGE                   = 0x12,
    EWS_APP_RSP_EQ_CHANGE                   = 0x13,
};

enum ews_music_sync_type_e {
    EWS_MUSIC_SYNC_TYPE_NORMAL,
    EWS_MUSIC_SYNC_TYPE_FIRST_SYNC,
};

typedef struct
{
    uint8_t ews_local_version;
    uint8_t ews_remote_version;

    int16_t peer_device_music_volume;
    int16_t local_device_music_volume;

    int16_t peer_device_call_volume;
    int16_t local_device_call_volume;

    uint8_t phone_sbc_frame_num;
    uint8_t ews_state;

    uint8_t ews_role;  // master or slave

    uint8_t is_left_ear;
    ews_dev_mode_e ews_mode;

    volatile bool  transfer_blocked;
    volatile bool  transfer_is_running;
    volatile bool  transfer_need_stop;
    volatile uint8_t remote_player_role;
    volatile uint8_t remote_player_state;

    uint8_t ews_media_head_len;
    int64_t ews_media_seq_num;


    /// Current bit offset
    uint16_t bit_off;
    uint16_t rxbit_off;

    // Role switch info
    //ews_master_info_t ews_master_info;
    uint8_t remote_phone_connected;
} ews_env_t;

typedef struct {
    int64_t play_samples;
    uint32_t clkn;
    uint32_t finecnt;
    uint32_t sys_t;
    uint32_t l2cap_seq;

    uint8_t play_status;
} ews_master_time_info_t;

typedef struct {
    int64_t play_samples;
    uint32_t clkn;
    uint32_t finecnt;
    uint32_t bofs;
    uint32_t sys_t;
} ews_slave_time_info_t;

typedef struct {
    volatile int64_t play;
    volatile uint32_t clkn;
    volatile uint32_t finecnt;
    volatile uint32_t bofs;
    volatile uint32_t sys_t;
} ews_sco_slave_sync_info_t;

typedef struct {
    uint32_t pll_freq;
    uint32_t clkn;
    uint32_t finecnt;
    uint32_t delay_us;
} ews_pll_info_t;

typedef struct {
    int64_t play_samples;
    uint32_t clkn;
    uint32_t finecnt;
    uint32_t delay_us;
    uint16_t sync_sequence;
    uint8_t play_status;
    uint8_t sync_type;
}__attribute__((packed)) ews_music_master_sync_info_t;

typedef struct {
    uint32_t clkn;
    uint32_t finecnt;
    int64_t play_samples;
    uint32_t master_pll_freq;

    uint32_t packet_clkn;
    uint8_t play_status;
}__attribute__((packed)) ews_sco_master_sync_info_t;

typedef struct {
    volatile int64_t play;
    volatile uint32_t clkn;
    volatile uint32_t finecnt;

    volatile uint32_t base_clkn;
    volatile uint32_t base_finecnt;
} ews_a2dp_master_sync_info_t;

typedef struct {
    uint32_t tone_num;
    uint32_t delay_ms;

    uint32_t event;

    uint8_t tone_chain[32];
    uint8_t chain_len;

    uint32_t clkn;
    uint32_t finecnt;
} ews_tone_master_sync_info_t;

typedef struct {
    uint8_t phone_address[6];
    uint8_t linkkey[16];
} linkkey_info_t;

#define BT_EWS_TASK_PRIO  8
#define BT_EWS_TASK_STACK_SIZE  (256)

void ews_create_connection_to_slave(void);

void ews_transmit_task(void *para);

uint32_t get_bt_clk(void);

void ews_send_audio_common_cmd(uint8_t audio_cmd, uint8_t *params,
    uint8_t params_len, uint32_t wait_timeout_ms, uint32_t try_count);

void ews_send_audio_common_rsp(uint8_t audio_rsp, uint8_t *params,
    uint8_t params_len);

void ews_send_app_common_cmd(uint8_t app_cmd, uint8_t *params, uint8_t params_len);

/* ews music functions */
void ews_music_set_codec(uint8_t codec_type);
bool ews_slave_music_need_wait_first_sync(void);
void ews_master_music_update_info(uint16_t l2cap_seq, uint8_t sync_type, uint32_t delay_us);
void ews_master_music_get_time_info(ews_master_time_info_t *info, uint8_t sync_type);
uint32_t ews_master_music_get_play_time(void);
void ews_master_music_set_audiopll_info(uint32_t freq);
void ews_master_music_audiopll_proc(void);
bool ews_master_music_get_audiopll_flag(void);
void ews_master_music_send_sync_info(uint8_t sync_type);
void ews_master_music_update_dec_info(int16_t dec_samples);
int64_t ews_master_music_get_dec_samples(void);
void ews_master_music_set_bs_samples(uint32_t samples);
uint8_t ews_master_music_get_sync_rsp_flag(void);
uint8_t ews_master_music_get_sync_rsp_sta(void);
void ews_master_music_reset(void);


void ews_slave_music_reset(void);
uint32_t ews_slave_music_get_master_audiopll_info(void);
void ews_slave_music_set_master_audiopll_info(uint32_t freq);
void ews_music_set_fast_sync_samples(int32_t samples);
int64_t ews_slave_music_get_total_fast_sync_samples(void);

bool ews_slave_music_get_time_info(uint32_t *p_master_play_status,
    uint32_t *p_master_play, uint32_t *p_bt_delta_us,
    uint32_t *p_update_sys_t, int64_t *p_base_samples);
uint32_t ews_slave_music_get_play_time(void);
void ews_slave_set_sync_seq(uint16_t sync_seq);
void ews_slave_music_update_dec_samples(int32_t dec_samples);
bool ews_slave_music_get_pll_flag(void);
void ews_slave_music_clear_pll_flag(void);
uint32_t ews_slave_music_get_play_l2cap_seq(void);
void ews_slave_music_set_first_sync_flag(bool flag);
bool ews_slave_music_get_first_sync_flag(void);
bool ews_slave_music_get_first_sync_result(void);
uint8_t ews_slave_music_get_codec_type(void);
uint32_t ews_slave_music_get_master_play_status(void);

uint8_t ews_music_need_check_fadeout_seq(void);
uint16_t ews_music_get_fadeout_seq(void);

void ews_sniffer_seq_reset(void);


void ews_master_sco_update_info(uint32_t packet_clkn);
uint32_t ews_master_sco_get_audiopll_info(void);
void ews_master_sco_send_sync_info(void);
void ews_master_clear_sco_unmute_clkn_status(void);
bool ews_master_get_sco_unmute_clkn(uint32_t *pclkn);

int32_t ews_slave_sco_update_info(uint32_t timestamp_clkn);
uint32_t ews_slave_sco_get_master_audiopll_info(void);
void ews_slave_sco_set_codec_type(uint8_t type);
uint8_t ews_slave_sco_get_codec_type(void);
void ews_slave_sco_set_unmute_clkn(uint32_t clkn);
void ews_slave_clear_sco_unmute_clkn_status(void);
bool ews_slave_get_sco_unmute_clkn(uint32_t *pclkn);
bool ews_slave_sco_need_fadeout(void);
void ews_sco_set_fast_sync_samples(int32_t samples);

void ews_sco_reset(uint32_t out_fs);
void ews_sco_update_dec_samples(uint32_t samples);


void ews_start_timer(void);
void ews_trans_task_notify(void);
void ews_trans_task_wait(void);
void ews_master_audio_start(void);

ews_env_t *app_get_ews_env_ptr(void);

extern ews_env_t ews_env;


uint8_t ews_get_play_status();

uint8_t ews_get_connection_state(void);
void ews_master_sco_start(void);
void ews_master_sco_stop(void);

void ews_master_acl_start(void);
void ews_master_acl_stop(void);

//void ews_con_init(void);

void ews_con_create_connection_to_peer(uint32_t timeout);
void ews_con_slave_start();
void ews_con_reset();
uint8_t ews_con_get_role();
void ews_set_peer_addr(uint8_t *bt_addr);
void ews_get_peer_addr(uint8_t *bt_addr);
void ews_dump_peer_addr();
uint8_t ews_con_create_connection_to_phone(uint32_t timeout);

uint8_t ews_get_connection_state(void);
uint8_t ews_get_mode(void);
void ews_con_disconnect_slave(void);

void ews_set_left_ear(uint8_t left_ear);
uint8_t ews_is_left_ear(void);

void ews_set_last_role(uint8_t role);
uint8_t ews_get_last_role();
void ews_set_phone_connected(uint8_t connected);
uint8_t ews_phone_connected();
void ews_set_phone_sco_connected(uint8_t connected);
uint8_t ews_phone_sco_connected();

void ews_process_init(void);


void ews_master_send_audio_a2dp_start(uint8_t media_codec, bool wait_first_sync);
void ews_master_send_audio_a2dp_stop(void);
void ews_master_send_audio_sco_start(void);
void ews_master_send_audio_sco_stop(void);


void ews_peer_afh_set_ignore(uint8_t set_flag);
void ews_peer_afh_set_flag(uint8_t set_flag);
uint8_t ews_peer_afh_get_flag(void);
void ews_peer_afh_set_waiting(uint8_t waiting);
uint8_t ews_peer_afh_get_waiting(void);
void ews_peer_afh_send(void);
void ews_peer_afh_set_handler(uint8_t mode,uint8_t htbl,uint8_t *master_clk, uint8_t *map, uint8_t set_flg);
void ews_peer_afh_init(void);
void ews_peer_afh_reset(void);

void ews_process_clkn_handle(void);

void ews_start_enc(void);
void ews_service_cmd_start_enc_handler(void);
void ews_service_rsp_start_enc_handler(uint8_t *buf);
void ews_peer_enc_reset(void);

//#endif
#endif
