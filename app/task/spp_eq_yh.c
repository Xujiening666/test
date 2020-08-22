/*
YH spp app
*/
#define LOG_LEV 3
#include "sys_inc.h"
#include "user_config.h"
#include "eq_yh.h"


extern void OnDataReceive(uint8_t* buffer, uint8_t length);
extern void OnEqInit(void);
extern void OnEqExit();

//TimerHandle_t timer_sendeq;
static uint8_t eqdata[7], equpdate = 0;

extern bool spp_send_data_do(uint8_t idx, const char * cmd, uint8_t len);
int spp_app_yh_data_send(uint8_t *value, uint8_t value_len)
{
    spp_send_data_do(0, (const char *)value, value_len);
    return 0;
}


void spp_app_yh_dump_buffer(char *name, uint8_t *buffer, uint16_t buff_size)
{
    printf_debug("spp_app_yh %s: ", name);
    for(int i = 0; i < buff_size; i++)
        printf_debug("%02X ", buffer[i]);
    printf_debug("\n");
}

int spp_app_yh_prepare_response(uint8_t *rsp_pkt, uint8_t rsp_code, uint8_t *rsp_data, uint8_t data_len)
{
    uint8_t rsp_pkt_len, i, j;
    rsp_pkt[0] = DEV2APP_HEADER & 0xFF;
    rsp_pkt[1] = DEV2APP_HEADER >> 8;
    rsp_pkt[2] = rsp_code;

    if(data_len > 40){
        loge("%s: data_len error, data_len = %u", __func__, data_len);
    }

    for(i = 0; i < data_len; i++){
        rsp_pkt[3 + i] = rsp_data[i];
    }

    if(data_len < 5){
        for(j = 0; j < 5 - data_len; j++){
            rsp_pkt[3 + data_len + j] = 0x00;
        }
        rsp_pkt[8] = DEV2APP_TAILER & 0xFF;
        rsp_pkt[9] = DEV2APP_TAILER >> 8;
        rsp_pkt_len = 10;
    }else{
        rsp_pkt[3 + i] = DEV2APP_TAILER & 0xFF;
        rsp_pkt[3 + i + 1] = DEV2APP_TAILER >> 8;
        rsp_pkt_len = data_len + 5;
    }

    return rsp_pkt_len;
}

int spp_app_yh_data_receive_process(uint8_t *buffer, uint8_t buff_size)
{
    uint16_t header, tailer;
    uint8_t cmd_code, *cmd_data;
    uint8_t rsp_code, rsp_pkt_len = 0, rsp_pkt[45] = {0};
    header = buffer[0] | (buffer[1] << 8);
    tailer = buffer[buff_size - 2] | (buffer[buff_size - 1] << 8);

    if(header != APP2DEV_HEADER){
        loge("%s: header check fail, header = %u", __func__, header);
        return 1;
    }
    if(tailer != APP2DEV_TAILER){
        loge("%s: tailer check fail, tailer = %u", __func__, tailer);
        return 1;
    }

    cmd_code = buffer[2];
    cmd_data = &buffer[3];

    switch(cmd_code)
    {
        case APP2DEV_CMD_A0:
        {
            if(cmd_data[0] == 0xB0){
                rsp_code = 0x0D;
                uint8_t rsp_data[] = {0x01, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00};
                rsp_pkt_len = spp_app_yh_prepare_response(rsp_pkt, rsp_code, rsp_data, sizeof(rsp_data));
            }else if(cmd_data[0] == 0xB2){
                rsp_code = 0x0C;
                uint8_t rsp_data[] = {0x08, 0x00, 0x01, 0x00};
                rsp_pkt_len = spp_app_yh_prepare_response(rsp_pkt, rsp_code, rsp_data, sizeof(rsp_data));
            }
            break;
        }
        case APP2DEV_CMD_C0:
        {
            rsp_code = 0x0D;
            uint8_t rsp_data[] = {0x02, 0x01, 0x03};
            rsp_pkt_len = spp_app_yh_prepare_response(rsp_pkt, rsp_code, rsp_data, sizeof(rsp_data));
            break;
        }
        case APP2DEV_CMD_F1:
        {
            rsp_code = 0x1D;
            uint8_t rsp_data[] = {0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03};
            rsp_pkt_len = spp_app_yh_prepare_response(rsp_pkt, rsp_code, rsp_data, sizeof(rsp_data));
            break;
        }
        case APP2DEV_CMD_F3:
        {
            if(cmd_data[0] == 0x01){
                OnDataReceive(buffer, buff_size);
                equpdate = 1;
                for(int i = 0; i < 7; i++){
                    eqdata[i] = cmd_data[i+1];
                }
            }else if(cmd_data[0] == 0x02){
                rsp_code = 0x1E;
                uint8_t rsp_data[] = {0x02, 0xfd, 0x01, 0x01, 0x00, 0xfa, 0xfc, 0x0c};
                rsp_pkt_len = spp_app_yh_prepare_response(rsp_pkt, rsp_code, rsp_data, sizeof(rsp_data));
            }
            break;
        }
        case APP2DEV_CMD_F4:
        {
            OnDataReceive(buffer, buff_size);
            break;
        }
        default:
            break;
    }

    if(rsp_pkt_len){
        spp_app_yh_dump_buffer("rcv", buffer, buff_size);
        spp_app_yh_dump_buffer("send", rsp_pkt, rsp_pkt_len);
        spp_app_yh_data_send(rsp_pkt, rsp_pkt_len);
    }
    return 0;
}

uint8_t spp_app_yh_data_receive(uint8_t *buffer, uint8_t buff_size)
{

    logd("(%s, %d) : attr_handle = %d, buff_size = %d", __FUNCTION__, __LINE__, attr_handle, buff_size);

    spp_app_yh_data_receive_process(buffer, buff_size);

    return 0;
}

/*void sendeq(xTimerHandle xTimer)
{
    if(equpdate){
        for(int i = 0; i < 7; i++)
            printf_debug("%02X ", eqdata[i]);
        printf_debug("\n");
    }
    equpdate = 0;
}
*/
extern void spp_register_receive_data_handler(uint8_t (*func_handler)(uint8_t *ptr, uint8_t len));

void spp_app_yh_init()
{

    //if (timer_sendeq == 0) {
        //timer_sendeq = xTimerCreate("BtBatTimer", pdMS_TO_TICKS(200), pdTRUE, (void *)1, sendeq);
    //}
    //xTimerStart(timer_sendeq, 10);
    spp_register_receive_data_handler(spp_app_yh_data_receive);
    OnEqInit();
}

void spp_app_yh_exit()
{
    OnEqExit();
}


