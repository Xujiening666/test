/*
HT ble app
*/
#define LOG_LEV 5
#include "sys_inc.h"
#include "user_config.h"
#include "ble_profile.h"

#define HT_HEADER 0x55AA

#define BLE_INDICATE_ATTR_HANDLE 0x91


static ble_profile_t ble_app_ht;


uint8_t ht_adv_data[] =
{
    0x02, 0x01, 0x02,
    0x0e, 0x09, 'H', 'T', ' ', 'C', 'A', 'R', ' ', 'B', 'T', ' ', '4', '.', '0',
    0x03, 0x03, 0x32, 0xa0,
};

uint8_t ht_scan_rsp[] =
{
    0x03, 0x03, 0x32, 0xa0,
};

/*
att db is like this: if the attr is not empty
|Byte0~1|Byte2~3|Byte4~5|Byte6 ~ Byte7 |   Byte8~ Byte(size -1)  |
|   size   |    flg    |    handle  |  UUID          |             data              |
    or
|Byte0~1|Byte2~3|Byte4~19|Byte6~ Byte21|   Byte22 ~ Byte(size -1) |
|   size   |    flg    |    handle   |  UUID          |        data                    |
*/
uint8_t ht_att_db[] =
{
    // 0x0001 PRIMARY_SERVICE-GAP_SERVICE
    0x0a, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x28, 0x00, 0x18,
    // 0x0002 CHARACTERISTIC-GAP_DEVICE_NAME-READ
    0x0d, 0x00, 0x02, 0x00, 0x04, 0x00, 0x03, 0x28, 0x0a, 0x05, 0x00, 0x00, 0x2a,
    // 0x0003 VALUE-GAP_DEVICE_NAME-READ-
    0x15, 0x00, 0x0a, 0x00, 0x05, 0x00, 0x00, 0x2a, 'H','T',' ','C','A','R',' ','B','T',' ','4','.','0',

    // 0x0004 PRIMARY_SERVICE-GATT_SERVICE
    0x0d, 0x00, 0x02, 0x00, 0x06, 0x00, 0x03, 0x28, 0x0a, 0x07, 0x00, 0x01, 0x2a,
    // 0x0005 CHARACTERISTIC-GATT_SERVICE_CHANGED-READ
    0x08, 0x00, 0x0a, 0x00, 0x07, 0x00, 0x01, 0x2a,

    // 0x0004 PRIMARY_SERVICE-GATT_SERVICE
    0x0d, 0x00, 0x02, 0x00, 0x08, 0x00, 0x03, 0x28, 0x0a, 0x09, 0x00, 0x02, 0x2a,
    // 0x0006 VALUE-GATT_SERVICE_CHANGED-READ-''
    0x08, 0x00, 0x0a, 0x00, 0x09, 0x00, 0x02, 0x2a,

    // 0x0004 PRIMARY_SERVICE-GATT_SERVICE
    0x0d, 0x00, 0x02, 0x00, 0x0a, 0x00, 0x03, 0x28, 0x0a, 0x0b, 0x00, 0x03, 0x2a,
    // 0x0006 VALUE-GATT_SERVICE_CHANGED-READ-''
    0x08, 0x00, 0x0a, 0x00, 0x0b, 0x00, 0x03, 0x2a,


    // 0x0007 PRIMARY_SERVICE
    0x0a, 0x00, 0x02, 0x00, 0x80, 0x00, 0x00, 0x28, 0x32, 0xa0,

    // 0x0008 CHARACTERISTIC | WRITE_WITHOUT_RESPONSE
    0x0d, 0x00, 0x02, 0x00, 0x90, 0x00, 0x03, 0x28, 0x20, 0x91, 0x00, 0x42, 0xa0,
    // 0x0009 VALUE | INDICATE
    0x08, 0x00, 0x20, 0x00, 0x91, 0x00, 0x42, 0xa0,
    // 0x0009 VALUE | WRITE_WITHOUT_RESPONSE
    0x08, 0x00, 0x02, 0x00, 0x92, 0x00, 0x02, 0x29,

    // 0x000a CHARACTERISTIC | NOTIFY
    0x0d, 0x00, 0x02, 0x00, 0x02, 0x01, 0x03, 0x28, 0x08, 0x03, 0x01, 0x40, 0xa0,
    // 0x000b VALUE | WRITE
    0x08, 0x00, 0x08, 0x01, 0x03, 0x01, 0x40, 0xa0,

    // 0x000a CHARACTERISTIC | NOTIFY
    0x0d, 0x00, 0x02, 0x00, 0x04, 0x01, 0x03, 0x28, 0x02, 0x05, 0x01, 0x41, 0xa0,
    // 0x000b VALUE | NOTIFY
    0x08, 0x00, 0x02, 0x00, 0x05, 0x01, 0x41, 0xa0,

    // END
    0x00, 0x00,
};

void ble_app_ht_init_db()
{
}

int ble_app_ht_data_send(uint8_t *value, uint16_t value_len)
{
    return ble_profile_data_send_indicate(BLE_INDICATE_ATTR_HANDLE, value, value_len);
}

uint16_t ble_app_ht_data_read(uint16_t attr_handle, uint8_t *buffer, uint16_t buff_size)
{
    return 0;
}

#if 1
void ble_app_ht_dump_buffer(char *name, uint8_t *buffer, uint16_t buff_size)
{
    printf_debug("ble_app_ht %s: 0x", name);
    for(int i = 0; i < buff_size; i++)
        printf_debug("%02X ", buffer[i]);
    printf_debug("\n");
}

int ble_app_ht_prepare_response(uint8_t *rsp_pkt, uint16_t rsp_cmd, uint8_t *rsp_data, uint8_t rsp_len, int8_t check_sum)
{
    uint8_t rsp_pkt_len, i = 0;

    rsp_pkt[0] = HT_HEADER >> 8;
    rsp_pkt[1] = HT_HEADER & 0xFF;
    rsp_pkt[2] = rsp_len;
    rsp_pkt[3] = rsp_cmd >> 8;
    rsp_pkt[4] = rsp_cmd & 0xFF;

    if(rsp_len > 2){
        for(i = 0; i < rsp_len - 2; i++){
            rsp_pkt[5 + i] = rsp_data[i];
        }
    }

    rsp_pkt[5 + i] = check_sum;
    rsp_pkt_len = rsp_len + 3 + 1;

    return rsp_pkt_len;
}

int ble_app_ht_data_receive_process(uint8_t *buffer)
{
    uint16_t header, cmd, rsp_cmd;
    uint8_t len;
    uint8_t rsp_pkt_len = 0, rsp_pkt[45] = {0};
    header = (buffer[0] << 8 ) | buffer[1];

    if(header != HT_HEADER){
        loge("%s: header check fail, header = %u", __func__, header);
        return 1;
    }

    len = buffer[2];
    cmd = (buffer[3] << 8 ) | buffer[4];
    ble_app_ht_dump_buffer("rcv", buffer, len + 4);

    switch(cmd)
    {
        case 0x0000:
        {
            rsp_cmd = 0x8000;
            uint8_t rsp_len = 2;
            rsp_pkt_len = ble_app_ht_prepare_response(rsp_pkt, rsp_cmd, 0, rsp_len, 0x80);
            ble_app_ht_dump_buffer("send", rsp_pkt, rsp_pkt_len);
            ble_app_ht_data_send(rsp_pkt, rsp_pkt_len);

            break;
        }
        case 0x0001:
        {
            rsp_cmd = 0x8002;
            uint8_t rsp_data[] = {0x24, 0xfd, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x2a, 0x30, 0x22, 0x2e, 0x0a, 0x06,//};
                    0xae, 0x02, 0x0a, 0x09, 0x00, 0x04};
            uint8_t rsp_len = sizeof(rsp_data) + 2;
            rsp_pkt_len = ble_app_ht_prepare_response(rsp_pkt, rsp_cmd, rsp_data, rsp_len, 0xb9);
            ble_app_ht_dump_buffer("send", rsp_pkt, rsp_pkt_len);
            ble_app_ht_data_send(rsp_pkt, rsp_pkt_len);
            break;
        }

        default:
            break;
    }

    if(0){
        ble_app_ht_dump_buffer("rcv", buffer, len + 4);
        ble_app_ht_dump_buffer("send", rsp_pkt, rsp_pkt_len);
        ble_app_ht_data_send(rsp_pkt, rsp_pkt_len);
    }
    return 0;
}
#endif

uint8_t ble_ht_rcv_buf[64];
SemaphoreHandle_t ble_ht_sem = NULL;
TaskHandle_t ble_ht_task_handler;

int ble_app_ht_data_receive(uint16_t attr_handle, uint8_t *buffer, uint16_t buff_size)
{

    logd("(%s, %d) : attr_handle = %d, buff_size = %d", __FUNCTION__, __LINE__, attr_handle, buff_size);
    if(attr_handle && buff_size){
        memcpy(ble_ht_rcv_buf, buffer, buff_size);
        if (xSemaphoreGive(ble_ht_sem) == pdTRUE) {
            logi("%s: xSemaphoreGive", __func__);
        }else{
            loge("%s: xSemaphoreGive fail", __func__);
        }
    }
    return 0;
}

void ble_ht_task(void *pvParameters)
{
    while(1)
    {
        logi("%s: wait Semaphore", __func__);
        xSemaphoreTake(ble_ht_sem, portMAX_DELAY);
        logi("%s: take Semaphore", __func__);
        ble_app_ht_data_receive_process(ble_ht_rcv_buf);
    }
}

void ble_app_ht_init()
{
    uint8_t rand_addr[] = {0x12, 0x34, 0x56, 0x78, 0x9a, 0xc0};
    memcpy(ble_app_ht.own_addr, rand_addr, 6);
    ble_app_ht.own_addr_type = RANDOM_ADDRESS;
    ble_app_ht.adv_data= ht_adv_data;
    ble_app_ht.adv_data_len = sizeof(ht_adv_data);
    ble_app_ht.scan_rsp= ht_scan_rsp;
    ble_app_ht.scan_rsp_len= sizeof(ht_scan_rsp);
    ble_app_ht.att_db = ht_att_db;
    ble_app_ht.start_handle = 0x0000;
    ble_app_ht.end_handle = 0xFFFF;
    ble_app_ht.read_callback = ble_app_ht_data_read;
    ble_app_ht.write_callback = ble_app_ht_data_receive;

    ble_profile_init(&ble_app_ht);

    ble_ht_sem = xSemaphoreCreateCounting(10, 0);
    xTaskCreate(ble_ht_task, "ble_ht_task", 256, NULL, 6, &ble_ht_task_handler);
}

void ble_app_ht_exit()
{
    vTaskDelete(ble_ht_task_handler);
    if (ble_ht_sem) {
        vSemaphoreDelete(ble_ht_sem);
        ble_ht_sem = NULL;
    }
}


