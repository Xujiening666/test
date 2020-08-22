#ifndef _BT_API_H_
#define _BT_API_H_


typedef struct {
    uint8_t   test_control;	 // 0: stop; 1: start
    uint8_t   freq_type;     // 0: hop;  1: hop
    uint8_t   channel_idx;   // 0~79
    uint8_t   tx_power;      // 0~32
    uint8_t   rx_gain;       // 0~32
    uint8_t   rx_gain_auto;
    uint8_t   role_idx;      // 0:slave 1:master -> node_idx
    uint8_t   tx_type;       // packe type
    uint16_t  tx_pkt_len;    // 0~1024
    uint8_t   rx_type;       // packe type
    uint16_t  rx_pkt_len;    // 0~1024
    uint8_t   payload_type;  // 0: 0x0;  1:0xff  2:0x55  3:0xf0:  4:prbs9
    uint8_t   tx_edr;
    uint8_t   rx_edr;
} fcc_control_t;
extern fcc_control_t fcc_control;


void bt_status_update(void);
void bt_task_create(void);
void bt_task_delete(void);
void ble_init(void);
void ble_deinit(void);
void bt_frontend_init();
void bt_frontend_init_incharge();
void rf_para_dft();
void btdm_config(void);
void rf_config(uint8_t cal_enable);
uint8_t bt_get_hf_codec_type(void);
void get_efuse_rfpara(uint32_t *efuse_para,uint8_t len);
void bt_initial_exchange_memory(void);
void bt_ll_init();

//enable 1 to force power
// power value:   0~18
// power in dbm: (-34+2*power) dbm
void rf_power_force(uint8_t enable, uint8_t power);
void bt_rf_power_max_set(uint8_t rf_power_max_val);

void bt_testmode_set(uint8_t enable);
uint8_t bt_testmode_get(void);


#endif

