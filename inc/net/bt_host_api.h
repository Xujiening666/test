#ifndef _BT_HOST_API_H
#define _BT_HOST_API_H


//#define BT_HSHF_EN                      1
//#define BT_A2DP_EN                      1
//#define BT_SPP_EN                       1
//#define BT_HID_EN                       0

#define BT_HSHF_3WCALL_EN               1
#define BT_HSHF_BAT_SHOW_EN             1
#define BT_VOL_CTRL_EN                  1   //use absolute volume

#define FT_HSF_BAT_SHOW                 0x01    //iphone battery level
#define FT_HSF_3WAY_CALL                0x02    //3way calling control
#define FT_HSF_IOS_RING                 0x04    //IOS incoming call ring
#define FT_HSF_VOICE_REC                0x08    //voice recognization
#define FT_A2DP_VOL_CTRL                0x01    //iphone volume control
#define FT_HSF_CODEC_NEGO               0x10    //codec negotiation

//#define BT_SUPPORT_MODE         ((0x02*BT_BLE_EN) | (0x01*BT_EDR_EN))
//#define BT_SUPPORT_PROFILE      ((0x0C*BT_HSHF_EN) | (0x03*BT_A2DP_EN) | (0x10*BT_SPP_EN) | (0x60*BT_HID_EN))
#define BT_SUPPORT_FT_HSF       ((FT_HSF_3WAY_CALL*BT_HSHF_3WCALL_EN) | (FT_HSF_BAT_SHOW*BT_HSHF_BAT_SHOW_EN))
#define BT_SUPPORT_FT_A2DP      (FT_A2DP_VOL_CTRL*BT_VOL_CTRL_EN)



//bt_get_status can get the status of BT
enum {
    BT_STATUS_OFF,                                 //0 bt mode disable
    BT_STATUS_IDLE,                                //1 not connect
    BT_STATUS_SLEEP,                               //2 low power

    BT_STATUS_DISCONNECTING,                       //3 disconnecting
    BT_STATUS_CONNECTING,                          //4 connecting

    BT_STATUS_CONNECTED,                           //5 connected
    BT_STATUS_PLAYING,                             //6 playing
    BT_STATUS_INCOMING,                            //7 incoming call ring
    BT_STATUS_OUTGOING,                            //8 outgoing calling
    BT_STATUS_CALLING,                             //9 calling

    BT_STATUS_CALL_TEST_START,                     //10 call test start
    BT_STATUS_CALL_TEST_END,                       //11 call test end
};

//BT event
#define BT_EVENT            0xD0000000
#define BLE_EVENT           0xD1000000
#define EWS_EVENT           0xD2000000

typedef enum {
    BT_EVENT_NONE           = 0,
    BT_EVENT_DISCONNECT     = BT_EVENT | 0x1,   //disconncet
    BT_EVENT_LOSTCONNECT    = BT_EVENT | 0x2,   //far distance connect lost
    BT_EVENT_CONNECTED      = BT_EVENT | 0x3,   //connect success
    BT_EVENT_PLAY           = BT_EVENT | 0x4,   //start play
    BT_EVENT_STOP           = BT_EVENT | 0x5,   //stop play
    BT_EVENT_INCOMING       = BT_EVENT | 0x6,   //incoming event
    BT_EVENT_OUTGOING       = BT_EVENT | 0x7,   //outgoing event
    BT_EVENT_CALLING        = BT_EVENT | 0x8,   //calling
    BT_EVENT_INCOMING_END   = BT_EVENT | 0x9,   //incoming end event
    BT_EVENT_CALLING_END    = BT_EVENT | 0xA,   //calling end event
    BT_EVENT_BAT_EN         = BT_EVENT | 0xB,   //enable update battery level
    BT_EVENT_BAT_UP         = BT_EVENT | 0xC,   //update battery level
    BT_EVENT_VOL_DOWN       = BT_EVENT | 0xD,   //set volume down of device from phone
    BT_EVENT_VOL_UP         = BT_EVENT | 0xE,   //set volume up of device from phone
    BT_EVENT_ANSWERING      = BT_EVENT | 0xF,
    BT_EVENT_ANSWERING_END  = BT_EVENT | 0x10,
    BT_EVENT_SCO_CON        = BT_EVENT | 0x11,  //sco connected
    BT_EVENT_SCO_DISCON     = BT_EVENT | 0x12,  //sco disconnected
    BT_EVENT_SIRI_ENABLE    = BT_EVENT | 0x13,  //siri enable
    BT_EVENT_STOP_SIRI      = BT_EVENT | 0x14,  //stop siri
    BT_EVENT_START_SIRI     = BT_EVENT | 0x15,  //start siri from iphone
    BT_EVENT_SIRI_ERROR     = BT_EVENT | 0x16,  //siri error
    BT_EVENT_INCOMING_NUM   = BT_EVENT | 0x17,
    BT_EVENT_SAVE_LINKKEY   = BT_EVENT | 0x18,   //save linkkey
    BT_EVENT_OTA_UPDATE     = BT_EVENT | 0x19,   //update
    BT_EVENT_SYNC_LINKKEY   = BT_EVENT | 0x1A,   //sync linkkey with Master/left earphone
    BT_EVENT_SML_PEER_DONE  = BT_EVENT | 0x1B,   //seamless peer done
    BT_EVENT_PAIR           = BT_EVENT | 0x1C,

    BT_EVENT_LE_CONNECTED               = BLE_EVENT | 0x1,
    BT_EVENT_LE_DISCONNECTED            = BLE_EVENT | 0x2,

	BT_EVENT_EWS_PEER_CONNECTED         = EWS_EVENT | 0x1,
    BT_EVENT_EWS_PHONE_CONNECTED        = EWS_EVENT | 0x2,
    BT_EVENT_EWS_CONNECT_PEER_TIMEOUT   = EWS_EVENT | 0x3,
    BT_EVENT_EWS_CONNECT_PHONE_TIMEOUT  = EWS_EVENT | 0x4,
    BT_EVENT_EWS_ROLE_CHANGED           = EWS_EVENT | 0x5,
    BT_EVENT_EWS_SERVICE_M_CONNECTED    = EWS_EVENT | 0x6,
    BT_EVENT_EWS_SERVICE_S_CONNECTED    = EWS_EVENT | 0x7,
    BT_EVENT_EWS_PEER_DISCONNECTED      = EWS_EVENT | 0x8,
    BT_EVENT_EWS_PHONE_DISCONNECTED     = EWS_EVENT | 0x9,
    BT_EVENT_EWS_SEAMLESS_FINISH        = EWS_EVENT | 0xa,
} bt_event_t;

bool bt_sco_connect_state(void);
bool bt_is_connect(void);

void hcc_vendor_settime(uint8_t *buf);
bool bt_test_load_code(uint8_t mode);
void btstack_profile_set(uint16_t profile);


//oatmeal uint8_t hshfvol_get_hook(void);
#define hshfvol_get_hook(x) bt_call_vol_get(x) //range in 0~15 by wxp

void bt_add_sco(void);
void bt_dis_sco(void);
 bool bt_chk_rcv_sco(uint8_t samples);
 bool bt_rcv_sco(uint8_t *ptr, uint8_t len, bool lost);
void btstack_enter_hsp_hfp_do(uint16_t handle);
void btstack_exit_hsp_hfp_do(void);
void spp_receive_data_handle(uint8_t idx, uint8_t *ptr, uint8_t len);
void bt_send_message(uint32_t event);

void bt_sco_switch(bool onoff);
bool bt_enter_sniff(void);
bool bt_exit_sniff(void);
bool bt_enter_sniff_req(void);
bool bt_exit_sniff_req(void);
bool bt_is_enter_sniff(void);
void bt_deep_sleep_enable(bool enable);
void bt_reset(void);
bool bt_is_in_sleep(void);
void bt_set_scan(bool scan, bool link);
uint8_t bt_disconnect_direct(void);
uint8_t bt_connect_link_n(void);
void bt_get_scan_type(bool *scan, bool *link);


/********************************************************
 * A2DP
********************************************************/
void sbc_init(void);
void bt_rx_id3_hook(uint8_t *ptr);
void audio_play_time_hook(uint8_t *ptr);

bool is_a2dp_rap_enable(void);

uint16_t bt_get_profile(void);
bool bt_get_linkaddr(uint8_t *addr);
bool bt_get_link_key(uint8_t *addr, uint8_t *link_key);
void bt_save_link_key(uint8_t *addr, uint8_t *link_key, uint8_t order);

void btinfo_get_pincode(uint8_t *ptr);
void btinfo_get_name(uint8_t *ptr);
void btinfo_get_addr(uint8_t *ptr);
void btinfo_get_rfparam(uint8_t *ptr);
void btinfo_get_rfparam_2(uint8_t *ptr);
void rfparam_fix(uint8_t *buf);              //BUF SIZE >= 64 + 8 = 72byte

void bt_status_update(void);
bool bt_sco_disc_clear(void);

void bt_audio_forward(void);
void bt_audio_backward(void);
void audio_play(void);
void audio_pause(void);
void audio_play_pause(void);
void audio_rewind(void);
void audio_fast_forward(void);
void audio_rewind_end(void);
void audio_fast_forward_end(void);
void bt_audio_volume_change(void);

void bt_volume_info_set(uint8_t *addr, uint8_t volume, bool sel_call);
int bt_volume_info_get(uint8_t *addr, bool sel_call);
void bt_volume_info_save(void);

#define WORK_EDR        BIT(0)
#define WORK_BLE        BIT(1)

typedef struct {
    uint16_t profile;                         //Profile control?
    uint8_t scan_enable;
    uint8_t simple_pair_enable;
    uint8_t acl_num;

    uint8_t connect_times;                   //times of reconnect, after power on
    uint8_t timeout_connect_times;           //times of reconnet for remote dropping
    uint8_t a2dp_feature;
    uint8_t hshf_feature;
    uint8_t play_state;  //for reconnect play state
    uint8_t bdaddr[6];   //for reconnect play state
} bt_config_t;

typedef struct
{
    uint8_t cur_sta;
    uint8_t update_sta;
    uint8_t scan_mode;
    uint8_t con_num;         //number of the phone  connected
    uint8_t recon_sta;
    uint8_t reserve;
} bt_state_t;


extern uint8_t bt_hshf_index;
extern uint8_t bt_a2dp_index;
extern bt_state_t bt_states;
extern bt_config_t bt_config;
extern uint8_t bt_connect(uint8_t times, uint8_t scan_en);


#endif
