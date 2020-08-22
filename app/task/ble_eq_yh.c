/*
YH ble app
*/
#define LOG_LEV 3
#include "sys_inc.h"
#include "user_config.h"
#include "ble_profile.h"
#include "eq_yh.h"

#define YH_SERVICE_UUID 0xAE00
#define RECV_ATT_UUID 0xAE01
#define SEND_ATT_UUID 0xAE02

#define SEND_ATT_HANDLE 0x0B

extern void OnDataReceive(uint8_t* buffer, uint8_t length);
extern void OnEqInit(void);
extern void OnEqExit();
static ble_profile_t ble_app_yh;
TimerHandle_t timer_sendeq;
static uint8_t eqdata[7], equpdate = 0;

/*
Reference BT Spec 4.2 Vol 3, Part C, Chapter 11
adv_data and scan_rsp format:
|  Byte0  |  Byte1~N  |  Byte(N+1)~Byte(length)  |
|  length |   AD type  |               AD data             |
*/


uint8_t yh_adv_data[31] =
{
    0x02, AD_TYPE_FLAGS, FLAG_LE_GENERAL_DISCOVER_MODE | FLAG_BR_EDR_NOT_SUPPORTED,
};

uint8_t yh_scan_rsp[31] =
{
    0x07, AD_TYPE_COMPLETE_LOCAL_NAME, 'Y','H','_','B','L','E',
};

/*
att db is like this: if the attr is not empty
|Byte0~1|Byte2~3|Byte4~5|Byte6 ~ Byte7 |   Byte8~ Byte(size -1)  |
|   size   |    flg    |    handle  |  UUID          |             data              |
    or
|Byte0~1|Byte2~3|Byte4~19|Byte6~ Byte21|   Byte22 ~ Byte(size -1) |
|   size   |    flg    |    handle   |  UUID          |        data                    |
*/

uint8_t yh_att_db[] =
{
    // 0x0001 PRIMARY_SERVICE-GAP_SERVICE
    ATT_SIZE(0x0a), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x01), ATT_UUID(GATT_PRIMARY_SERVICE_UUID), ATT_UUID(GAP_SERVICE_UUID),
    // 0x0002 CHARACTERISTIC-GAP_DEVICE_NAME-READ
    ATT_SIZE(0x0d), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x02), ATT_UUID(GATT_CHARACTERISTICS_UUID), ATT_PROPERTY_READ, ATT_HANDLE(0x0003), ATT_UUID(GAP_DEVICE_NAME_UUID),
    // 0x0003 VALUE-GAP_DEVICE_NAME-READ-'YH_BLE'
    ATT_SIZE(0x0e), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x03), ATT_UUID(GAP_DEVICE_NAME_UUID), 'Y', 'H', '_', 'B', 'L', 'E',

    // 0x0004 PRIMARY_SERVICE-GATT_SERVICE
    ATT_SIZE(0x0a), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x04), ATT_UUID(GATT_PRIMARY_SERVICE_UUID), ATT_UUID(GATT_SERVICE_UUID),
    // 0x0005 CHARACTERISTIC-GATT_SERVICE_CHANGED-READ
    ATT_SIZE(0x0d), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x05), ATT_UUID(GATT_CHARACTERISTICS_UUID), ATT_PROPERTY_READ, ATT_HANDLE(0x0006), ATT_UUID(GAP_SERVICE_CHANGED),
    // 0x0006 VALUE-GATT_SERVICE_CHANGED-READ-''
    ATT_SIZE(0x08), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x06), ATT_UUID(GAP_SERVICE_CHANGED),

    // 0x0007 PRIMARY_SERVICE
    ATT_SIZE(0x0a), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x07), ATT_UUID(GATT_PRIMARY_SERVICE_UUID), ATT_UUID(YH_SERVICE_UUID),
    // 0x0008 CHARACTERISTIC | WRITE_WITHOUT_RESPONSE
    ATT_SIZE(0x0d), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x08), ATT_UUID(GATT_CHARACTERISTICS_UUID), ATT_PROPERTY_WRITE_WITHOUT_RESPONSE, ATT_HANDLE(0x0009), ATT_UUID(RECV_ATT_UUID),
    // 0x0009 VALUE | WRITE_WITHOUT_RESPONSE
    ATT_SIZE(0x08), ATT_FLAG(ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_DYNAMIC), ATT_HANDLE(0x09), ATT_UUID(RECV_ATT_UUID),

    // 0x000a CHARACTERISTIC | NOTIFY
    ATT_SIZE(0x0d), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0a), ATT_UUID(GATT_CHARACTERISTICS_UUID), ATT_PROPERTY_NOTIFY, ATT_HANDLE(0x000b), ATT_UUID(SEND_ATT_UUID),
    // 0x000b VALUE | NOTIFY
    ATT_SIZE(0x08), ATT_FLAG(ATT_PROPERTY_NOTIFY | ATT_PROPERTY_DYNAMIC), ATT_HANDLE(0x0b), ATT_UUID(SEND_ATT_UUID),

    // 0x000c CLIENT_CHARACTERISTIC_CONFIGURATION
    ATT_SIZE(0x08), 0x0a, 0x01, ATT_HANDLE(0x0c), ATT_UUID(GATT_CLIENT_CHARACTERISTICS_CONFIGURATION),

    // END
    0x00, 0x00,
};


int ble_app_yh_data_send(uint8_t *value, uint16_t value_len)
{
    return ble_profile_data_send_notify(SEND_ATT_HANDLE, value, value_len);
}

uint16_t ble_app_yh_data_read(uint16_t attr_handle, uint8_t *buffer, uint16_t buff_size)
{
    return 0;
}

void ble_app_yh_dump_buffer(char *name, uint8_t *buffer, uint16_t buff_size)
{
    printf_debug("ble_app_yh %s: ", name);
    for(int i = 0; i < buff_size; i++)
        printf_debug("%#X ", buffer[i]);
    printf_debug("\n");
}

int ble_app_yh_construct_response(uint8_t *rsp_pkt, uint8_t rsp_code, uint8_t *rsp_data, uint8_t data_len)
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

int ble_app_yh_data_receive_process(uint8_t *buffer, uint16_t buff_size)
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
                rsp_pkt_len = ble_app_yh_construct_response(rsp_pkt, rsp_code, rsp_data, sizeof(rsp_data));
            }else if(cmd_data[0] == 0xB2){
                rsp_code = 0x0C;
                uint8_t rsp_data[] = {0x03, 0x00, 0x01, 0x00};
                rsp_pkt_len = ble_app_yh_construct_response(rsp_pkt, rsp_code, rsp_data, sizeof(rsp_data));
            }
            break;
        }
        case APP2DEV_CMD_C0:
        {
            rsp_code = 0x0D;
            uint8_t rsp_data[] = {0x02, 0x01, 0x03};
            rsp_pkt_len = ble_app_yh_construct_response(rsp_pkt, rsp_code, rsp_data, sizeof(rsp_data));
            break;
        }
        case APP2DEV_CMD_F1:
        {
            rsp_code = 0x1D;
            uint8_t rsp_data[] = {0x00, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03};
            rsp_pkt_len = ble_app_yh_construct_response(rsp_pkt, rsp_code, rsp_data, sizeof(rsp_data));
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
                rsp_pkt_len = ble_app_yh_construct_response(rsp_pkt, rsp_code, rsp_data, sizeof(rsp_data));
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
        ble_app_yh_dump_buffer("rcv", buffer, buff_size);
        ble_app_yh_dump_buffer("send", rsp_pkt, rsp_pkt_len);
        ble_app_yh_data_send(rsp_pkt, rsp_pkt_len);
    }
    return 0;
}

int ble_app_yh_data_receive(uint16_t attr_handle, uint8_t *buffer, uint16_t buff_size)
{

    logd("(%s, %d) : attr_handle = %d, buff_size = %d", __FUNCTION__, __LINE__, attr_handle, buff_size);

    ble_app_yh_data_receive_process(buffer, buff_size);

    return 0;
}
void sendeq(xTimerHandle xTimer)
{
    if(equpdate){
        for(int i = 0; i < 7; i++)
            printf_debug("%02X ", eqdata[i]);
        printf_debug("\n");
    }
    equpdate = 0;
}


void ble_app_yh_init()
{
    logi("(%s, %d)", __FUNCTION__, __LINE__);

    ble_app_yh.own_addr_type = PUBLIC_ADDRESS;
    ble_app_yh.adv_data = yh_adv_data;
    ble_app_yh.adv_data_len = sizeof(yh_adv_data);
    ble_app_yh.scan_rsp = yh_scan_rsp;
    ble_app_yh.scan_rsp_len= sizeof(yh_scan_rsp);
    ble_app_yh.att_db = yh_att_db;
    ble_app_yh.start_handle = 0x0000;
    ble_app_yh.end_handle = 0xFFFF;
    ble_app_yh.read_callback = ble_app_yh_data_read;
    ble_app_yh.write_callback = ble_app_yh_data_receive;

    ble_profile_init(&ble_app_yh);
    //if (timer_sendeq == 0) {
        //timer_sendeq = xTimerCreate("BtBatTimer", pdMS_TO_TICKS(200), pdTRUE, (void *)1, sendeq);
    //}
    //xTimerStart(timer_sendeq, 10);
    OnEqInit();
}

void ble_app_yh_exit()
{
    OnEqExit();
}


