
#define LOG_LEV 3
#include "sys_inc.h"
#include "user_config.h"
//#include "bt_host_config.h"

#include "ews_process.h"
//#include "ews_con.h"
//#include "ews_sync.h"

#include "bt_ews.h"

#if BT_EWS_EN

#define EWS_WAIT_CONNECT_TIMEOUT    0x2000
#define EWS_RECON_PEER_TIMEOUT      0x2000
#define EWS_RECON_PHONE_TIMEOUT     0x2000

TaskHandle_t bt_ews_handler;


bt_ews_state_t ews_master_state_table[] =
{
    {BT_EWS_RECONNECT_PEER,    EWS_RECON_PEER_TIMEOUT},
    {BT_EWS_WAIT_CONNECT,      EWS_WAIT_CONNECT_TIMEOUT},
    {BT_EWS_RECONNECT_PHONE,   EWS_RECON_PHONE_TIMEOUT},
    {BT_EWS_WAIT_CONNECT,      EWS_WAIT_CONNECT_TIMEOUT},
    {BT_EWS_RECONNECT_PEER,    EWS_RECON_PEER_TIMEOUT},
    {BT_EWS_WAIT_CONNECT,      EWS_WAIT_CONNECT_TIMEOUT},
    {BT_EWS_RECONNECT_PHONE,   EWS_RECON_PHONE_TIMEOUT},
    {BT_EWS_WAIT_CONNECT,      EWS_WAIT_CONNECT_TIMEOUT},
    {BT_EWS_RECONNECT_PEER,    EWS_RECON_PEER_TIMEOUT},
    {BT_EWS_WAIT_CONNECT,      EWS_WAIT_CONNECT_TIMEOUT},
    {BT_EWS_RECONNECT_PHONE,   EWS_RECON_PHONE_TIMEOUT},
    {BT_EWS_WAIT_CONNECT,      EWS_WAIT_CONNECT_TIMEOUT},

    {BT_EWS_IDLE,              0x0000},
};

bt_ews_state_t ews_slave_state_table[] =
{
    {BT_EWS_WAIT_CONNECT,      EWS_WAIT_CONNECT_TIMEOUT},
    {BT_EWS_RECONNECT_PEER,    EWS_RECON_PEER_TIMEOUT},
    {BT_EWS_WAIT_CONNECT,      EWS_WAIT_CONNECT_TIMEOUT},
    {BT_EWS_RECONNECT_PHONE,   EWS_RECON_PHONE_TIMEOUT},
    {BT_EWS_WAIT_CONNECT,      EWS_WAIT_CONNECT_TIMEOUT},
    {BT_EWS_RECONNECT_PEER,    EWS_RECON_PEER_TIMEOUT},
    {BT_EWS_WAIT_CONNECT,      EWS_WAIT_CONNECT_TIMEOUT},
    {BT_EWS_RECONNECT_PHONE,   EWS_RECON_PHONE_TIMEOUT},
    {BT_EWS_WAIT_CONNECT,      EWS_WAIT_CONNECT_TIMEOUT},
    {BT_EWS_RECONNECT_PEER,    EWS_RECON_PEER_TIMEOUT},
    {BT_EWS_WAIT_CONNECT,      EWS_WAIT_CONNECT_TIMEOUT},
    {BT_EWS_RECONNECT_PHONE,   EWS_RECON_PHONE_TIMEOUT},
    {BT_EWS_WAIT_CONNECT,      EWS_WAIT_CONNECT_TIMEOUT},

    {BT_EWS_IDLE,              0x0000},
};

//extern uint8_t ews_peer_addr[6];
extern uint8_t ews_peer_connected, ews_conn_state_change;

bt_ews_state_t *bt_ews_state_table;
uint8_t bt_ews_state_size;
uint32_t    bt_ews_state_timeout;
TimerHandle_t ews_state_timer = NULL;
SemaphoreHandle_t btews_xSemaphore;
uint32_t ews_cur_state;
uint32_t start_time;

extern uint8_t need_recon_phone, need_recon_peer;
extern void bt_reconnect_phone();
extern void hci_write_scan_enable(uint8_t scan_enable);
extern void hci_write_page_timeout(uint16_t timeout);

void ews_state_timeout_handler(xTimerHandle xTimer)
{
    logi("ews_state_timeout_handler, %u ms elapsed", timestamp_to_ms(get_timestamp()) - start_time);
    if(btews_xSemaphore != NULL){
        osSemaphoreGive(btews_xSemaphore);
    }
}

void bt_ews_init_state_table(uint8_t ews_role)
{
    if(ews_role == EWS_MASTER){
        bt_ews_state_table = ews_master_state_table;
        bt_ews_state_size = sizeof(ews_master_state_table)/sizeof(ews_master_state_table[0]);
    }else if(ews_role == EWS_SLAVE){
        bt_ews_state_table = ews_slave_state_table;
        bt_ews_state_size = sizeof(ews_slave_state_table)/sizeof(ews_slave_state_table[0]);
    }

    logi("ews_init_role: %u, bt_ews_state_size = %u", ews_role, bt_ews_state_size);
}

void bt_ews_task(void *pvParameters)
{
    uint8_t i = 0, skip;
    uint32_t ews_state_timeout;
    logi("bt_ews_task");

    while(1){
        //logi("i = %d, ews_conn_state_change = %d", i, ews_conn_state_change);
        if(i < bt_ews_state_size){
            xSemaphoreTake(btews_xSemaphore, portMAX_DELAY);

            if(start_time){
                xTimerStop(ews_state_timer, 10);
                start_time = 0;
            }
            skip = 0;
            ews_cur_state = bt_ews_state_table[i].state;
            ews_state_timeout = bt_ews_state_table[i].timeout;

            logi("ews_state[%u] = %u ---------------", i, ews_cur_state);

            switch(ews_cur_state)
            {
                case BT_EWS_RECONNECT_PEER:
                {
                    if(ews_peer_connected){
                        skip = 1;
                    }else if(ews_phone_connected() && ews_get_play_status()){
                        logi("playing---");
                        skip = 1;
                    }else{
                        uint8_t ews_peer_addr[6];
                        ews_get_peer_addr(ews_peer_addr);
                        printf_debug("connect to peer %02x", ews_peer_addr[0]);
                        for(int i = 1; i < 6; i++){
                            printf_debug(":%02x", ews_peer_addr[i]);
                        }
                        printf_debug("\n");

                        hci_write_scan_enable(0x00);
                        hci_write_page_timeout(ews_state_timeout);
                        ews_con_create_connection_to_peer(ews_state_timeout*0.625);
                    }
                    break;
                }

                case BT_EWS_RECONNECT_PHONE:
                {
                    if(ews_phone_connected() || ews_get_last_role() == EWS_SLAVE){
                        skip = 1;
                    }else{
                        hci_write_scan_enable(0x00);
                        hci_write_page_timeout(ews_state_timeout);
                        if(!ews_con_create_connection_to_phone(ews_state_timeout*0.625)){
                            skip = 1;
                        }
                    }
                    break;
                }

                case BT_EWS_WAIT_CONNECT:
                {
                    if(ews_phone_connected() && ews_peer_connected){
                        hci_write_scan_enable(0x00);
                        skip = 1;
                    }else if(ews_peer_connected && ews_get_last_role() == EWS_SLAVE){
                        hci_write_scan_enable(0x00);
                        skip = 1;
                    }else{
                        if(ews_phone_connected()){
                            hci_write_scan_enable(0x02);
                        }else{
                            hci_write_scan_enable(0x03);
                        }

                        ews_state_timeout *= 0.625;
                        logi("wait connect (0x%02x) for %u ms", ews_phone_connected() ? 0x02:0x03, ews_state_timeout);

                        xTimerStart(ews_state_timer, 10);
                        start_time =  timestamp_to_ms(get_timestamp());
                    }

                    break;
                }

                case BT_EWS_IDLE:
                    break;
                default:
                    break;
            }

            if(skip){
                osSemaphoreGive(btews_xSemaphore);
            }

            i++;
        }else if(ews_conn_state_change){
            logi("ews_phone_connected = %d, ews_peer_connected = %d", ews_phone_connected(), ews_peer_connected);
            //ews_cur_state = BT_EWS_IDLE;
            if(ews_phone_connected() && ews_peer_connected){
                hci_write_scan_enable(0x00);
            }else if(ews_phone_connected()){
                hci_write_scan_enable(0x02);
            }else if(ews_peer_connected && ews_get_last_role() == EWS_SLAVE){
                hci_write_scan_enable(0x00);
            }else{
                hci_write_scan_enable(0x03);
            }
            ews_conn_state_change = 0;
        }else{
            //ews_cur_state = BT_EWS_IDLE;
            msleep(100);
        }

        if(ews_cur_state == BT_EWS_IDLE){
            if(need_recon_phone){
                logi("reconnect phone");
                bt_reconnect_phone();
                need_recon_phone = 0;
            }
            if(need_recon_peer){
                logi("restart bt ews process");
                //ews_set_last_role(ews_get_last_role() ? EWS_SLAVE : EWS_MASTER);
                bt_ews_init_state_table(ews_get_last_role());
                i = 0;
                need_recon_peer = 0;
                osSemaphoreGive(btews_xSemaphore);
            }
        }
    }

    vSemaphoreDelete(btews_xSemaphore);
    logi("xTimerDelete");
    xTimerDelete(ews_state_timer, 10);
    ews_state_timer = NULL;
    btews_xSemaphore = NULL;
    bt_ews_handler = NULL;
    logi("bt_ews_task exit");
    vTaskDelete(bt_ews_handler);
}

void bt_ews_task_create()
{
    bt_ews_init_state_table(ews_get_last_role());

    btews_xSemaphore = xSemaphoreCreateBinary();
    if (btews_xSemaphore != NULL){
        osSemaphoreGive(btews_xSemaphore);
    }else{
        loge("btews_xSemaphore NULL");
    }
    ews_state_timer = xTimerCreate("ews state timeout", pdMS_TO_TICKS(EWS_WAIT_CONNECT_TIMEOUT*0.625), pdFALSE, (void *)1, ews_state_timeout_handler);

    xTaskCreate(bt_ews_task, "task bt ews", 128, NULL, 1, &bt_ews_handler);
}

void bt_ews_task_delete()
{
    vTaskDelete(bt_ews_handler);
    if (btews_xSemaphore) {
        vSemaphoreDelete(btews_xSemaphore);
        btews_xSemaphore = NULL;
    }
    xTimerDelete(ews_state_timer, 10);
}

uint32_t bt_ews_get_cur_state()
{
    return ews_cur_state;
}

#endif

