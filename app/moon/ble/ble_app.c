#define LOG_LEV 5
#include "sys_inc.h"
#include "user_config.h"
#include "event.h"
#include "norflash.h"
#include "ble_profile.h"
#include "ble_app.h"
#include "ble.h"
#include "key.h"
#include "pmu.h"
#include "info_mem.h"
#include "version.h"
#include "system.h"
#include "record.h"
#include "rtc.h"
#include "task_main.h"
#include "./ai/ai.h"



#define SHAREBUF_PERIOD_SZ_BlE (864)
#define SHAREBUF_SZ_BLE (SHAREBUF_PERIOD_SZ_BlE * 2)

#define MIN(a,b)    ((a) < (b) ? (a) : (b))


extern void *general_calloc(uint32_t size);
extern void general_free(void *ptr);
extern uint32_t get_current_wav_id(void);

typedef struct ble_app_s {
    volatile uint8_t status;
    TaskHandle_t task_handle;
    xQueueHandle cmd_queue;
    struct ring_buffer bufctrl;         /* buffer manager */
    SemaphoreHandle_t read_sem_sync;    /* share pcm buffer read/write synchronization semaphore */
    uint8_t *sharebuf;                  /* share pcm buffer */
    volatile uint8_t exit_flag;
} ble_app_st;

static void *g_ble_app_t;

typedef struct ble_app_msg {
    uint32_t cmd;
    uint32_t val;
} ble_app_msg_t;

#define BLE_APP_MSG_QUEUE_LENGTH       3
#define BLE_APP_TASK_NAME              "ble_app_task"

static ble_profile_t ble_app;
uint8_t buf_tx[16];         //buf，用于发送
uint32_t system_status;     //系统状态

uint8_t adv_data[31];
uint8_t scan_rsp[31];

static bool ble_con_flag;
//static uint8_t ble_send_id_flag;
static bool first_send_real_time_flag;//标识第一帧头

/* Modify by arunboy 20200529 协议头*/
uint8_t BANBEN_ID[2] = {0};                                 //版本号
uint8_t FIRST_FRAME[6] = {0x61,0x69,0x6d,0x74,0x2d,0x30};   //帧开始符
uint8_t END_FRAME[6] = {0x61,0x69,0x6d,0x74,0x2d,0x31};     //帧结束符
uint8_t error_code = ERR_CODE_SUCCESS;                      //错误代码
uint8_t SN_NUM[8] = {0};                                    //全局的SN号码，每次开机读取一次


uint8_t simple_scan_rsp[31] =
{
    0x00, AD_TYPE_COMPLETE_LOCAL_NAME,
};

/*
profile data is like this: if the attr is not empty
|Byte0~1    |Byte2~3    |Byte4~5        |Byte6 ~ Byte7  |   Byte8~ Byte(size -1)    |
|   size    |    flg    |    handle     |  UUID         |             data          |
    or
|Byte0~1    |Byte2~3    |Byte4~19       |Byte6~ Byte21  |   Byte22 ~ Byte(size -1)  |
|   size    |    flg    |    handle     |  UUID         |        data               |
*/

const uint8_t simple_att_db[] =
{
    // 0x0001 PRIMARY_SERVICE-GAP_SERVICE
    ATT_SIZE(0x000a), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0001), ATT_UUID(GATT_PRIMARY_SERVICE_UUID), ATT_UUID(GAP_SERVICE_UUID),
    // 0x0002 CHARACTERISTIC-GAP_DEVICE_NAME-READ
    ATT_SIZE(0x000d), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0002), ATT_UUID(GATT_CHARACTERISTICS_UUID), ATT_PROPERTY_READ, ATT_HANDLE(0x0003), ATT_UUID(GAP_DEVICE_NAME_UUID),
    // 0x0003 VALUE-GAP_DEVICE_NAME-READ-'SmartLink BLE'
    ATT_SIZE(0x000d), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0003), ATT_UUID(GAP_DEVICE_NAME_UUID), 'R','V','1','0','0',
    //ATT_SIZE(0x0015), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0003), ATT_UUID(GAP_DEVICE_NAME_UUID), 'S','m','a','r','t','L','i','n','k',' ','B','L','E',

    // 0x0004 PRIMARY_SERVICE-GATT_SERVICE
    ATT_SIZE(0x000a), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0004), ATT_UUID(GATT_PRIMARY_SERVICE_UUID), ATT_UUID(GATT_SERVICE_UUID),
    // 0x0005 CHARACTERISTIC-GATT_SERVICE_CHANGED-READ
    ATT_SIZE(0x000d), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0005), ATT_UUID(GATT_CHARACTERISTICS_UUID), ATT_PROPERTY_READ, ATT_HANDLE(0x0006), ATT_UUID(GAP_SERVICE_CHANGED),
    // 0x0006 VALUE-GATT_SERVICE_CHANGED-READ-''
    ATT_SIZE(0x0008), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0006), ATT_UUID(GAP_SERVICE_CHANGED),
#if 0
    // 0x0007 PRIMARY_SERVICE
    ATT_SIZE(0x000a), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0007), ATT_UUID(GATT_PRIMARY_SERVICE_UUID), ATT_UUID(UUID_SERVICE_01),
    // 0x0008 CHARACTERISTIC | WRITE_WITHOUT_RESPONSE
    ATT_SIZE(0x000d), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0008), ATT_UUID(GATT_CHARACTERISTICS_UUID), ATT_PROPERTY_WRITE_WITHOUT_RESPONSE, ATT_HANDLE(0x0009), ATT_UUID(UUID_RECV_01),
    // 0x0009 VALUE | WRITE_WITHOUT_RESPONSE
    ATT_SIZE(0x0008), ATT_FLAG(ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_DYNAMIC), ATT_HANDLE(0x0009), ATT_UUID(UUID_RECV_01),

    // 0x000a CHARACTERISTIC | NOTIFY
    ATT_SIZE(0x000d), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x000a), ATT_UUID(GATT_CHARACTERISTICS_UUID), ATT_PROPERTY_NOTIFY, ATT_HANDLE(0x000b), ATT_UUID(UUID_SEND_01),
    // 0x000b VALUE | NOTIFY
    ATT_SIZE(0x0008), ATT_FLAG(ATT_PROPERTY_NOTIFY | ATT_PROPERTY_DYNAMIC), ATT_HANDLE(HANDLE_SEND_01), ATT_UUID(UUID_SEND_01),
#endif
    // 0x000c PRIMARY_SERVICE
    ATT_SIZE(0x000a), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x000c), ATT_UUID(GATT_PRIMARY_SERVICE_UUID), ATT_UUID(UUID_SERVICE_OTA),
    // 0x000d CHARACTERISTIC | WRITE_WITHOUT_RESPONSE
    ATT_SIZE(0x000d), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x000d), ATT_UUID(GATT_CHARACTERISTICS_UUID), ATT_PROPERTY_WRITE_WITHOUT_RESPONSE, ATT_HANDLE(0x000e), ATT_UUID(UUID_RECV_OTA),
    // 0x000e VALUE | WRITE_WITHOUT_RESPONSE
    ATT_SIZE(0x0008), ATT_FLAG(ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_DYNAMIC), ATT_HANDLE(0x000e), ATT_UUID(UUID_RECV_OTA),

    // 0x000f CHARACTERISTIC | READ | NOTIFY
    ATT_SIZE(0x000d), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x000f), ATT_UUID(GATT_CHARACTERISTICS_UUID), ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY, ATT_HANDLE(0x0010), ATT_UUID(UUID_SEND_OTA),
    // 0x0010 VALUE | READ | NOTIFY | DYNAMIC
    ATT_SIZE(0x0008), ATT_FLAG(ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY | ATT_PROPERTY_DYNAMIC), ATT_HANDLE(HANDLE_SEND_OTA), ATT_UUID(UUID_SEND_OTA),
    // 0x0011 CLINT_CONFIGURATION READ | DYNAMIC
    ATT_SIZE(0x0008), ATT_FLAG(ATT_PROPERTY_READ | ATT_PROPERTY_WRITE | ATT_PROPERTY_DYNAMIC), ATT_HANDLE(0x0011), ATT_UUID(GATT_CLIENT_CHARACTERISTICS_CONFIGURATION),

    // Modify by arunboy 20200528
	// 0x0012 PRIMARY_SERVICE
	ATT_SIZE(0x000a), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0012), ATT_UUID(GATT_PRIMARY_SERVICE_UUID), ATT_UUID(UUID_SERVICE_AUDIO),
	// 0x0013 CHARACTERISTIC | WRITE_WITHOUT_RESPONSE
	ATT_SIZE(0x000d), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0013), ATT_UUID(GATT_CHARACTERISTICS_UUID), ATT_PROPERTY_WRITE_WITHOUT_RESPONSE, ATT_HANDLE(0x0014), ATT_UUID(UUID_RECV_AUDIO),
	// 0x0014 VALUE | WRITE_WITHOUT_RESPONSE
	ATT_SIZE(0x0008), ATT_FLAG(ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_DYNAMIC), ATT_HANDLE(HANDLE_RECV_AUDIO), ATT_UUID(UUID_RECV_AUDIO),

	// 0x0015 CHARACTERISTIC | READ | NOTIFY
	ATT_SIZE(0x000d), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0015), ATT_UUID(GATT_CHARACTERISTICS_UUID), ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY, ATT_HANDLE(0x0016), ATT_UUID(UUID_SEND_AUDIO),
	// 0x0016 VALUE | READ | NOTIFY | DYNAMIC
	ATT_SIZE(0x0008), ATT_FLAG(ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY | ATT_PROPERTY_DYNAMIC), ATT_HANDLE(HANDLE_SEND_AUDIO), ATT_UUID(UUID_SEND_AUDIO),
	// 0x0017 CLINT_CONFIGURATION READ | DYNAMIC
	ATT_SIZE(0x0008), ATT_FLAG(ATT_PROPERTY_READ | ATT_PROPERTY_WRITE | ATT_PROPERTY_DYNAMIC), ATT_HANDLE(0x0017), ATT_UUID(GATT_CLIENT_CHARACTERISTICS_CONFIGURATION),

	// 0x0018 PRIMARY_SERVICE
	ATT_SIZE(0x000a), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0018), ATT_UUID(GATT_PRIMARY_SERVICE_UUID), ATT_UUID(UUID_SERVICE_FILE),
	// 0x0019 CHARACTERISTIC | WRITE_WITHOUT_RESPONSE
	ATT_SIZE(0x000d), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0019), ATT_UUID(GATT_CHARACTERISTICS_UUID), ATT_PROPERTY_WRITE_WITHOUT_RESPONSE, ATT_HANDLE(0x001a), ATT_UUID(UUID_RECV_FILE),
	// 0x001a VALUE | WRITE_WITHOUT_RESPONSE
	ATT_SIZE(0x0008), ATT_FLAG(ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_DYNAMIC), ATT_HANDLE(HANDLE_RECV_FILE), ATT_UUID(UUID_RECV_FILE),

	// 0x001b CHARACTERISTIC | READ | NOTIFY
	ATT_SIZE(0x000d), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x001b), ATT_UUID(GATT_CHARACTERISTICS_UUID), ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY, ATT_HANDLE(0x001c), ATT_UUID(UUID_SEND_FILE),
	// 0x001c VALUE | READ | NOTIFY | DYNAMIC
	ATT_SIZE(0x0008), ATT_FLAG(ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY | ATT_PROPERTY_DYNAMIC), ATT_HANDLE(HANDLE_SEND_FILE), ATT_UUID(UUID_SEND_FILE),
	// 0x001d CLINT_CONFIGURATION READ | DYNAMIC
	ATT_SIZE(0x0008), ATT_FLAG(ATT_PROPERTY_READ | ATT_PROPERTY_WRITE | ATT_PROPERTY_DYNAMIC), ATT_HANDLE(0x001d), ATT_UUID(GATT_CLIENT_CHARACTERISTICS_CONFIGURATION),

	// 0x001e PRIMARY_SERVICE
	ATT_SIZE(0x000a), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x001e), ATT_UUID(GATT_PRIMARY_SERVICE_UUID), ATT_UUID(UUID_SERVICE_CMD),
	// 0x001f CHARACTERISTIC | WRITE_WITHOUT_RESPONSE
	ATT_SIZE(0x000d), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x001f), ATT_UUID(GATT_CHARACTERISTICS_UUID), ATT_PROPERTY_WRITE_WITHOUT_RESPONSE, ATT_HANDLE(0x0020), ATT_UUID(UUID_RECV_CMD),
	// 0x0020 VALUE | WRITE_WITHOUT_RESPONSE
	ATT_SIZE(0x0008), ATT_FLAG(ATT_PROPERTY_WRITE_WITHOUT_RESPONSE | ATT_PROPERTY_DYNAMIC), ATT_HANDLE(HANDLE_RECV_CMD), ATT_UUID(UUID_RECV_CMD),

	// 0x0021 CHARACTERISTIC | READ | NOTIFY
	ATT_SIZE(0x000d), ATT_FLAG(ATT_PROPERTY_READ), ATT_HANDLE(0x0021), ATT_UUID(GATT_CHARACTERISTICS_UUID), ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY, ATT_HANDLE(0x0022), ATT_UUID(UUID_SEND_CMD),
	// 0x0022 VALUE | READ | NOTIFY | DYNAMIC
	ATT_SIZE(0x0008), ATT_FLAG(ATT_PROPERTY_READ | ATT_PROPERTY_NOTIFY | ATT_PROPERTY_DYNAMIC), ATT_HANDLE(HANDLE_SEND_CMD), ATT_UUID(UUID_SEND_CMD),
	// 0x0023 CLINT_CONFIGURATION READ | DYNAMIC
	ATT_SIZE(0x0008), ATT_FLAG(ATT_PROPERTY_READ | ATT_PROPERTY_WRITE | ATT_PROPERTY_DYNAMIC), ATT_HANDLE(0x0023), ATT_UUID(GATT_CLIENT_CHARACTERISTICS_CONFIGURATION),
    // END

    0x00, 0x00,
};
extern uint32_t general_get_avaheapsize();

AT(.ble_app_sram_seg)
int ble_app_data_send(uint16_t attr_handle, uint8_t *value, uint16_t value_len)
{   
    return ble_profile_data_send_notify(attr_handle, value, value_len);
}

AT(.ble_app_sram_seg)
uint16_t ble_app_data_read(uint16_t attr_handle, uint8_t *buffer, uint16_t buff_size)
{
    logi("rr,handle:%x, size:%d", attr_handle,buff_size);
    return 0;
}

const uint8_t tag_tbl[4] = {0x55, 0xAA, 0xAA, 0x55};


AT(.ble_flash_seg)
void ble_app_process(void)
{
    uint8_t *tag_buf;

    logi("ble status:%x", system_status);
    switch (system_status) {
    case STATUS_INIT:
        break;

    case STATUS_START:
        buf_tx[OFFSET_TAG] = TAG_START;
        ble_ota_send(buf_tx, TX_LEN_START);
        break;

    case STATUS_RESET:
        buf_tx[OFFSET_TAG] = TAG_RESET;
        ble_ota_send(buf_tx, TX_LEN_RESET);
        //if (ews_peer_connected && ews_is_left_ear()) {
            /* 左耳通知右耳 */
          //  ews_start_ble_ota();
        //}
        mdelay(200);        //此处延时目的：等待ble_ota_send和ews_start_ble_ota完成
        logi("write tag to flash");
        tag_buf = libc_malloc(SIZE_TAG);
        libc_memcpy(tag_buf+OFFSET_BRANCH, tag_tbl, SIZE_BRANCH);
        libc_memcpy(tag_buf+OFFSET_BT_ADDR, bt_local_addr, SIZE_BT_ADDR);
        libc_memcpy(tag_buf+OFFSET_BLE_ADDR, ble_app.own_addr, SIZE_BLE_ADDR);
        //if (ews_is_left_ear()) {
            *(tag_buf+OFFSET_LR_FLAG) = 0xAA;
       // } else {
        //    *(tag_buf+OFFSET_LR_FLAG) = 0x00;
        //}
        printf_array(tag_buf, SIZE_TAG);
        norflash_write((uint32_t)tag_buf, 0x1000, SIZE_TAG, 1, 1);
        libc_free(tag_buf);

        logi("write ok, restart pmu.");
        restart_pmu();      //restart system
        while(1);
        break;

    default:
        break;
    }
}

AT(.ble_flash_seg)
uint8_t send_device_stats()
{ 
    uint8_t *msg;
	msg = general_calloc(BT_BUFFER_SIZE);
    __maybe_unused uint8_t tmp_8;
	__maybe_unused uint64_t tmp_64;
	uint8_t tmp_len;
	uint8_t len_sum = 0;
	uint8_t *u64_str;
	u64_str = libc_calloc(24);
    libc_memcpy(msg, BANBEN_ID, LEN_FIRST_BANBEN_ID);
    len_sum += LEN_FIRST_BANBEN_ID;
	libc_memcpy(msg + len_sum, FIRST_FRAME, LEN_FIRST_FRAME);
	len_sum += LEN_FIRST_FRAME;
	*(msg + OFFSET_CMD) = TAG_GET_DEVICE_STATS;
	*(msg + OFFSET_ERROR) = ERR_CODE_SUCCESS;
	len_sum += 2;
	
	libc_memcpy(msg + len_sum, "{\"data\":{\"battery\":", 19);
	len_sum += 19;

	tmp_64 = pmu_get_bat_quantity_percent();
	tmp_len = uint64_to_str(tmp_64, u64_str);
	libc_memcpy(msg + len_sum, u64_str, tmp_len);
	len_sum += tmp_len;

	libc_memcpy(msg + len_sum, ",\"bound\":", 9);
	len_sum += 9;

	info_get(INFO_USER_BIND_FLAG, (void *)&tmp_8, 1);
	*(msg + len_sum) = tmp_8 ? '1' : '0';
	len_sum += 1;

	libc_memcpy(msg + len_sum, ",\"charging\":", 12);
	len_sum += 12;

	tmp_8 = pmu_bat_is_charging();
	*(msg + len_sum) = tmp_8 ? '1' : '0';
	len_sum += 1;

	libc_memcpy(msg + len_sum, ",\"storage\":", 11);
	len_sum += 11;

	tmp_64 = ext_getfree(0);
	if(tmp_64 != ERR_CODE_NOT_GET_STORAGE) {
		tmp_len = uint64_to_str(tmp_64, u64_str);
		libc_memcpy(msg + len_sum, u64_str, tmp_len);
		len_sum += tmp_len;
	} else {
        *(msg + len_sum) = '0';
		len_sum += 1;
	}

	libc_memcpy(msg + len_sum, ",\"available_storage\":", 21);
	len_sum += 21;

	tmp_64 = ext_getfree(1);
	if(tmp_64 != ERR_CODE_NOT_GET_STORAGE) {
		tmp_len = uint64_to_str(tmp_64, u64_str);
		libc_memcpy(msg + len_sum, u64_str, tmp_len);
		len_sum += tmp_len;
	} else {
		*(msg + len_sum) = '0';
		tmp_64 = 0;
		len_sum += 1;
	}
	
	ble_cmd_send(msg, len_sum);//发送一包数据
	len_sum = 0;

	libc_memcpy(msg + len_sum, ",\"available_record_time\":", 25);
	len_sum += 25;

    if(tmp_64 != 0) {
		tmp_64 = tmp_64 >> 6;//右移6位==>除以64,计算剩余录音时长;
		tmp_len = uint64_to_str(tmp_64, u64_str);
		libc_memcpy(msg + len_sum, u64_str, tmp_len);
		len_sum += tmp_len;
	} else {
        *(msg + len_sum) = '0';
		len_sum += 1;
	}
	

	libc_memcpy(msg + len_sum, ",\"support_wifi_p2p\":0,", 22);
	len_sum += 22;
	
	libc_memcpy(msg + len_sum, "\"time\":", 7);
	len_sum += 7;

	tmp_64 = mktime();
	tmp_len = uint64_to_str(tmp_64, u64_str);
	libc_memcpy(msg + len_sum, u64_str, tmp_len);
	len_sum += tmp_len;

	ble_cmd_send(msg, len_sum);//发送一包数据
	len_sum = 0;

	libc_memcpy(msg + len_sum, ",\"audio_channel\":1,", 19);
	len_sum += 19;

	libc_memcpy(msg + len_sum, "\"del_after_sync\":0,", 19);
	len_sum += 19;

	libc_memcpy(msg + len_sum, "\"usb_mode\":", 11);
	len_sum += 11;

	if(MODE_USBDEV == mode_get_sta())
	    *(msg + len_sum) = '1';
    else
	    *(msg + len_sum) = '0';
    len_sum += 1;
	
	libc_memcpy(msg + len_sum, ",\"recording\":", 13);
	len_sum += 13;
	
	if(REC_STATUS_RECORDING == rec_context->status)
		*(msg + len_sum) = '1';
	else		
    	*(msg + len_sum) = '0';
	len_sum += 1;

	libc_memcpy(msg + len_sum, ",\"ble_protocol_ver\":", 20);
    len_sum += 20;

	tmp_64 = 0;
	tmp_64 += BANBEN_ID[1];tmp_64 = tmp_64 << 8;
	tmp_64 += BANBEN_ID[0];
	tmp_len = uint64_to_str(tmp_64, u64_str);
	libc_memcpy(msg + len_sum, u64_str, tmp_len);
	len_sum += tmp_len;
	ble_cmd_send(msg, len_sum);//发送一包数据
	len_sum = 0;

	libc_memcpy(msg + len_sum, ",\"sn\":\"", 7);
	len_sum += 7;

	/*SN号 Modify by arunboy 20200723 */
	libc_memcpy(msg + len_sum, SN_NUM, 8);
	len_sum += 8;
	
	libc_memcpy(msg + len_sum, "\",\"firm_ver\":\"", 14);
	len_sum += 14;

	libc_memcpy(msg + len_sum, CODE_VERSION, sizeof(CODE_VERSION) - 1);
    len_sum += sizeof(CODE_VERSION) - 1;

	libc_memcpy(msg + len_sum, "\",\"privacy\":", 12);
	len_sum += 12;
	
	info_get(INFO_PRIVATE_MODE_FLAG, (void *)&tmp_8, 1);
	if(1 == tmp_8) {
	    *(msg + len_sum) = '1';
	} else {
		*(msg + len_sum) = '0';
	}
	len_sum += 1;

	libc_memcpy(msg + len_sum, "}}", 2);
	len_sum += 2;

	libc_memcpy(msg + len_sum, END_FRAME, LEN_END_FRAME);
	len_sum += LEN_END_FRAME;

	ble_cmd_send(msg, len_sum);//发送一包数据
	
	libc_free(u64_str);
	general_free(msg);
	return ERR_CODE_SUCCESS;
}

AT(.ble_flash_seg)
void send_simple_status()
{   
    if(!ble_con_flag){
        return;
	}
    uint8_t flag;
    uint8_t *msg;
	msg =  general_calloc(21);
	libc_memcpy(msg, BANBEN_ID, LEN_FIRST_BANBEN_ID);
	libc_memcpy(msg + OFFSET_FIRST_FRAME, FIRST_FRAME, LEN_FIRST_FRAME);
	*(msg + OFFSET_CMD) = TAG_GET_STATS;
	*(msg + OFFSET_ERROR) = ERR_CODE_SUCCESS;
	//录音状态
	if(REC_STATUS_RECORDING == rec_context->status) {
        *(msg + OFFSET_ERROR + 1) = rec_context->status;
	} else {
		*(msg + OFFSET_ERROR + 1) = 0;
	} 
    //充电状态
	*(msg + OFFSET_ERROR + 2) = pmu_bat_is_charging();
	//绑定状态
    info_get(INFO_USER_BIND_FLAG, (void *)&flag, 1);
    *(msg + OFFSET_ERROR + 3) = flag;
	//隐私模式
	info_get(INFO_PRIVATE_MODE_FLAG, (void *)&flag, 1);
	*(msg + OFFSET_ERROR + 4) = flag;
	//U盘模式
	if(MODE_USBDEV == mode_get_sta())
		*(msg + OFFSET_ERROR + 5) = 1;
	else
		*(msg + OFFSET_ERROR + 5) = 0;
	libc_memcpy(msg + 15, END_FRAME, LEN_END_FRAME);
	ble_cmd_send(msg, 21);
	general_free(msg);
}

static uint8_t  ble_record_start = 0;
static bool can_send_real_record_data = false;
extern uint32_t NEED_SEND_FILE_OFFSET;
extern char NEED_SEND_DEL_FILE_NAME[15];

AT(.ble_flash_seg)
void send_record_message(uint8_t cmd)
{
	if(!ble_con_flag)
			return;

    uint8_t *msg;
	uint8_t data_len = 0;
	msg =  general_calloc(22);
    uint32_t current_wav_id = get_current_wav_id();
	libc_memcpy(msg, BANBEN_ID, LEN_FIRST_BANBEN_ID);
	data_len += LEN_FIRST_BANBEN_ID;
	libc_memcpy(msg + OFFSET_FIRST_FRAME, FIRST_FRAME, LEN_FIRST_FRAME);
	data_len += LEN_FIRST_FRAME;
	
	*(msg + OFFSET_CMD) = cmd;
	data_len += 1;
	
	if(cmd == TAG_RECORD_START || cmd == TAG_RECORD_STOP) {
		*(msg + OFFSET_ERROR) = ERR_CODE_SUCCESS;
		data_len += 1;
	}
	
	*(msg + data_len) = current_wav_id;
	*(msg + data_len + 1) = current_wav_id >> 8;
	*(msg + data_len + 2) = current_wav_id >> 16;
	*(msg + data_len + 3) = current_wav_id >> 24;
	data_len += 4;
	
	libc_memcpy(msg + data_len, END_FRAME, LEN_END_FRAME);
	data_len += LEN_END_FRAME;
	ble_cmd_send(msg, data_len);
	general_free(msg);
}

AT(.ble_flash_seg)
void send_battery_message(void)
{
	if(!ble_con_flag)
			return;
	
    uint8_t *msg;
	msg =  general_calloc(20);
	libc_memcpy(msg, BANBEN_ID, LEN_FIRST_BANBEN_ID);
	libc_memcpy(msg + OFFSET_FIRST_FRAME, FIRST_FRAME, LEN_FIRST_FRAME);
	*(msg + OFFSET_CMD) = TAG_GET_BATTERY;
	*(msg + OFFSET_ERROR) = ERR_CODE_SUCCESS;
	*(msg + OFFSET_ERROR + 1) = pmu_get_bat_quantity_percent();
	*(msg + OFFSET_ERROR + 2) = pmu_bat_is_charging();
	libc_memcpy(msg + 12, END_FRAME, LEN_END_FRAME);
	ble_cmd_send(msg, 18);
	general_free(msg);
}

AT(.ble_flash_seg)
uint8_t ble_start_record()
{
    //判断sn号是否一致
    if(!get_sd_sn_status()) {
        return ERR_CODE_NO_RECORD_MODE;
	}
    
	//判断是否处于录音模式
	if(MODE_RECORD != mode_get_sta()) {
        return ERR_CODE_NO_RECORD_MODE;
	}
	//判断是否处于设备播放模式
	if(REC_STATUS_PLAYING == rec_context->status) {
		return ERR_CODE_RECORD_PLAYING;
	}
	//判断是否处于设备自己在录音
	if(ble_record_start == 0 && REC_STATUS_RECORDING == rec_context->status) {
        send_record_message(TAG_RECORD_START);
		return ERR_CODE_SUCCESS;
	}
	//判断是否是续传模式
	if(1 == ble_record_start) {
		send_record_message(TAG_RECORD_START);
		can_send_real_record_data = true;
	    return ERR_CODE_SUCCESS;
	}
	//处于录音模式,空闲状态,非续传模式,允许app开启一个新的录音
	ble_record_start = 1;
	first_send_real_time_flag = true;
	event_put(KEY_BLE_START_RECORD);
	can_send_real_record_data = true;
	return ERR_CODE_SUCCESS;
}

AT(.ble_flash_seg)
uint8_t reset_device()
{
    uint8_t flag = 0;
    info_set(INFO_PRIVATE_MODE_FLAG, (void *)&flag, 1);
	info_set(INFO_USER_BIND_FLAG, (void *)&flag, 1);
	return ERR_CODE_SUCCESS;
}

AT(.ble_flash_seg)
void update_dav_data()
{
	adv_data[23] = 0;
	adv_data[24] = 0;
	adv_data[25] = 0;
	adv_data[26] = 0;
	adv_data[27] = 0;
	gap_advertisements_set_data(31,adv_data);
}


AT(.ble_flash_seg)
void send_unbind_user_msg()
{
    if(!ble_con_flag)
		return;
	
    uint8_t *msg;
	msg =  general_calloc(16);
	libc_memcpy(msg, BANBEN_ID, LEN_FIRST_BANBEN_ID);
	libc_memcpy(msg + OFFSET_FIRST_FRAME, FIRST_FRAME, LEN_FIRST_FRAME);
	*(msg + OFFSET_CMD) = TAG_VOLUNT_UNBIND_USER;
	*(msg + OFFSET_ERROR) = ERR_CODE_SUCCESS;
	libc_memcpy(msg + 10, END_FRAME, LEN_END_FRAME);
	ble_cmd_send(msg, 16);
	general_free(msg);
	update_dav_data();
}

AT(.ble_flash_seg)
void process_cmd(uint8_t *buf,uint16_t buff_size)
{
	/* Modify by arunboy 20200618 */
    uint8_t *msg;
	uint8_t flag;
	uint32_t tmp = 0;
	uint8_t data_len = 0;//data_len是不含首帧和尾帧的数据长度,最少为2  cmd_code和error_code
	error_code = ERR_CODE_SUCCESS;
	msg = general_calloc(30);
	libc_memcpy(BANBEN_ID, buf, 2);
	libc_memcpy(msg + OFFSET_FIRST_BANBEN_ID, BANBEN_ID, LEN_FIRST_BANBEN_ID);
	libc_memcpy(msg + OFFSET_FIRST_FRAME, FIRST_FRAME, LEN_FIRST_FRAME);	
	*(msg + OFFSET_CMD) = buf[OFFSET_CMD];
	*(msg + OFFSET_ERROR) = error_code;
	logi("cmd = 0x%x, buff_size = %d", buf[OFFSET_CMD], buff_size);
    switch(buf[OFFSET_CMD]){
        case TAG_RECORD_START:
			error_code = ble_start_record();
	        if(error_code != ERR_CODE_SUCCESS) 
			{
                data_len = 2;
			}
			break;
		case TAG_RECORD_STOP:
			can_send_real_record_data = false;
			ble_record_start = 0;
			event_put(KEY_BLE_STOP_RECORD);
		    break;
		case TAG_GET_DIR:
			start_send_dir_files_list();
		    break;
#if 0
		case TAG_SYNC_FILE_ID:
			NEED_SEND_FILE_ID = 0;
			NEED_SEND_FILE_ID += buf[12];NEED_SEND_FILE_ID = NEED_SEND_FILE_ID << 8;
			NEED_SEND_FILE_ID += buf[11];NEED_SEND_FILE_ID = NEED_SEND_FILE_ID << 8;
		    NEED_SEND_FILE_ID += buf[10];NEED_SEND_FILE_ID = NEED_SEND_FILE_ID << 8;
			NEED_SEND_FILE_ID += buf[9];

			NEED_SEND_FILE_OFFSET = 0;
			NEED_SEND_FILE_OFFSET += buf[18];NEED_SEND_FILE_OFFSET = NEED_SEND_FILE_OFFSET << 8;
			NEED_SEND_FILE_OFFSET += buf[17];NEED_SEND_FILE_OFFSET = NEED_SEND_FILE_OFFSET << 8;
			NEED_SEND_FILE_OFFSET += buf[16];NEED_SEND_FILE_OFFSET = NEED_SEND_FILE_OFFSET << 8;
			NEED_SEND_FILE_OFFSET += buf[15];NEED_SEND_FILE_OFFSET = NEED_SEND_FILE_OFFSET << 8;
		    NEED_SEND_FILE_OFFSET += buf[14];NEED_SEND_FILE_OFFSET = NEED_SEND_FILE_OFFSET << 8;
			NEED_SEND_FILE_OFFSET += buf[13];

			event_put(KEY_BLE_START_SEND_RECORD_BY_ID);
			data_len = 2;
			break;
		case TAG_DEL_FILE_ID:
			tmp += buf[12];tmp = tmp << 8;
			tmp += buf[11];tmp = tmp << 8;
		    tmp += buf[10];tmp = tmp << 8;
			tmp += buf[9];
			error_code = del_rec_file_by_id(tmp);
			data_len = 2;
			break;
#endif
		case TAG_SYNC_FILE_NAME_STOP:
		case TAG_SYNC_FILE_ID_STOP:
			set_stop_sending_flag(true);
			break;

		case TAG_GET_STORAGE:
			tmp = ext_getfree(1);
			logi("tmp free= 0x%x",tmp);
			if(tmp != ERR_CODE_NOT_GET_STORAGE)
			{
				*(msg + OFFSET_ERROR + 1) = tmp;
			    *(msg + OFFSET_ERROR + 2) = tmp >> 8;
				*(msg + OFFSET_ERROR + 3) = tmp >> 16;
				*(msg + OFFSET_ERROR + 4) = tmp >> 24;
				
				tmp = ext_getfree(0);
				logi("tmp totl= 0x%x",tmp);
				*(msg + OFFSET_ERROR + 5) = tmp;
				*(msg + OFFSET_ERROR + 6) = tmp >> 8;
				*(msg + OFFSET_ERROR + 7) = tmp >> 16;
				*(msg + OFFSET_ERROR + 8) = tmp >> 24;

				data_len = 10;
			} else {
                error_code = ERR_CODE_NOT_GET_STORAGE;
				data_len = 2;
			}
			break;
		case TAG_GET_TIME:
			tmp = mktime();
			*(msg + OFFSET_ERROR + 1) = tmp;
		    *(msg + OFFSET_ERROR + 2) = tmp >> 8;
			*(msg + OFFSET_ERROR + 3) = tmp >> 16;
			*(msg + OFFSET_ERROR + 4) = tmp >> 24;
			data_len = 6;
			break;
		case TAG_SET_TIME:
			tmp = 0;
			tmp += buf[12];tmp = tmp << 8;
			tmp += buf[11];tmp = tmp << 8;
		    tmp += buf[10];tmp = tmp << 8;
			tmp += buf[9];
			loge("time: 0x%x",tmp);
			set_time(tmp);
			data_len = 2;
			break;
		case TAG_RESET_DEVICE:
			error_code = reset_device();
			data_len = 2;
			break;
		case TAG_GET_VER:
			libc_memcpy(msg + OFFSET_ERROR + 1, CODE_VERSION, sizeof(CODE_VERSION) - 1);
			data_len = 2 + sizeof(CODE_VERSION) - 1;
			break;
		case TAG_GET_USER:
			info_get(INFO_USER_BIND_FLAG, (void *)&flag, 1);
		    if(1 == flag)
			{
			    info_get(INFO_USER_INFO,(void *)msg + OFFSET_ERROR + 1, (uint32_t)4);
                data_len = 6;
			} else {
				error_code = ERR_CODE_UNBIND_USER;//用户未绑定
				data_len = 2;
			}
			break;
		case TAG_GET_BATTERY:
			send_battery_message();
			break;
		case TAG_SET_USER:
			flag = 1;
			info_set(INFO_USER_BIND_FLAG, (void *)&flag, 1);
			info_set(INFO_USER_INFO, (void *)buf + OFFSET_CMD + 1, 4);
            data_len = 2;
			break;
		case TAG_UNBIND_USER:
			flag = 0;
			info_set(INFO_USER_BIND_FLAG, (void *)&flag, 1);
		    update_dav_data();
		    data_len = 2;
			break;
		case TAG_GET_DEVICE_STATS:
			error_code = send_device_stats();
			break;
		case TAG_SYNC_FILE_NAME:
			set_stop_sending_flag(false);
			NEED_SEND_FILE_OFFSET = 0;
			NEED_SEND_FILE_OFFSET += buf[14];NEED_SEND_FILE_OFFSET = NEED_SEND_FILE_OFFSET << 8;
			NEED_SEND_FILE_OFFSET += buf[13];NEED_SEND_FILE_OFFSET = NEED_SEND_FILE_OFFSET << 8;
			NEED_SEND_FILE_OFFSET += buf[12];NEED_SEND_FILE_OFFSET = NEED_SEND_FILE_OFFSET << 8;
			NEED_SEND_FILE_OFFSET += buf[11];NEED_SEND_FILE_OFFSET = NEED_SEND_FILE_OFFSET << 8;
		    NEED_SEND_FILE_OFFSET += buf[10];NEED_SEND_FILE_OFFSET = NEED_SEND_FILE_OFFSET << 8;
			NEED_SEND_FILE_OFFSET += buf[9];
            libc_memset(NEED_SEND_DEL_FILE_NAME, 0 ,sizeof(NEED_SEND_DEL_FILE_NAME));
			for(flag = 15; flag < buff_size; flag++)
			{
			    NEED_SEND_DEL_FILE_NAME[flag - 15] = buf[flag];
				if(flag > 45){
                    error_code = ERR_CODE_UNKNOW_CMD;
					break;
				}
			}
			logi("sync_ble_app_file %s",NEED_SEND_DEL_FILE_NAME);
			if(MODE_USBDEV != mode_get_sta() && REC_STATUS_RECORDING != rec_context->status)
			{
			    logi("begain send file");
				error_code = send_file(true);
			} else {
                error_code = ERR_CODE_NEED_CLOSE_USB;
			}
			data_len = 2;
	 	    break;
			
		case TAG_DEL_FILE_NAME:
//		    libc_memset(NEED_SEND_DEL_FILE_NAME, 0 ,sizeof(NEED_SEND_DEL_FILE_NAME));
//			for(flag = OFFSET_CMD; flag < buff_size; flag++)
//			{
//			    NEED_SEND_DEL_FILE_NAME[flag - 9] = buf[flag];
//				if(flag > 39){
//                    error_code = ERR_CODE_UNKNOW_CMD;
//					break;
//				}
//			}
//			logi("del_file_name %s", NEED_SEND_DEL_FILE_NAME);
			tmp = 0;
            tmp = (buf[OFFSET_CMD + 4] - '0') * 10000;
            tmp = (tmp + buf[OFFSET_CMD + 5] - '0') * 1000;
		    tmp = (tmp + buf[OFFSET_CMD + 6] - '0') * 100;
			tmp = (tmp + buf[OFFSET_CMD + 7] - '0') * 10;
			tmp = tmp + buf[OFFSET_CMD + 8] - '0';
			logi("del file = %d",tmp);
			error_code = del_rec_file_by_name(tmp);
			data_len = 2;
			break;
			
		case TAG_SET_PRIVATE_MODE:
		    if(buff_size == 10) {
				flag = buf[9];
				info_set(INFO_PRIVATE_MODE_FLAG, (void *)&flag, 1);
			} else {
                error_code = ERR_CODE_UNKNOW_CMD;
			}
			data_len = 2;
			break;
        case TAG_GET_STATS:
			send_simple_status();
			break;

		default:
			logi("arunboy err_code = 0x%x",buf[OFFSET_CMD]);
			error_code = ERR_CODE_UNKNOW_CMD;
			data_len = 2;
		    break;
	}
    if(data_len > 0) {
		*(msg + OFFSET_ERROR) = error_code;
		libc_memcpy(msg + OFFSET_ERROR + data_len - 1, END_FRAME, LEN_END_FRAME);
		ble_cmd_send(msg, LEN_SUM + data_len);
	}
	general_free(msg);
	logd("down audio heap:%d, os heap:%d", general_get_avaheapsize(), xPortGetFreeHeapSize());
}

uint8_t ble_record_start_flag(void)
{
    return ble_record_start;
}

void set_ble_record_start_flag(uint8_t flag)
{
    ble_record_start = flag;
}

AT(.ble_app_sram_seg)
void ble_ota_receive_process(uint8_t *buf)
{
    logi("rx tag:%02x", buf[OFFSET_TAG]);
    switch(buf[OFFSET_TAG]) {
    case TAG_START:
        logi("start");
        system_status = STATUS_START;
        event_put(LE_EVENT_APP);
        break;
    case TAG_RESET:
        logi("reset");
        system_status = STATUS_RESET;
        event_put(LE_EVENT_APP);
        break;
    default:
        break;
    }
}


AT(.ble_app_sram_seg)
int ble_app_data_receive(uint16_t attr_handle, uint8_t *buffer, uint16_t buff_size)
{
    logi("r,handle:%x, size:%d", attr_handle,buff_size);
    
    if (HANDLE_RECV_OTA == attr_handle) {
		ble_ota_receive_process(buffer);
    } else if(HANDLE_RECV_CMD == attr_handle) {
	    process_cmd(buffer,buff_size);
	}
    return 0;
}

AT(.ble_app_sram_seg)
void ble_ota_send(uint8_t *buf, uint16_t size)
{
    ble_app_data_send(HANDLE_SEND_OTA, buf, size);
}

AT(.ble_app_sram_seg)
void ble_cmd_send(uint8_t *buf, uint16_t size)
{
    ble_app_data_send(HANDLE_SEND_CMD, buf, size);
}

AT(.ble_app_sram_seg)
uint8_t ble_file_send(uint8_t *buf, uint16_t size)
{
    uint8_t err;
    err = ble_app_data_send(HANDLE_SEND_FILE, buf, size);
	if(0 == err)
	    logi("err = %d",err);
    return err;
}

AT(.ble_app_sram_seg)
void update_ble_con_status(bool con)
{
    ble_con_flag = con;
	if(!ble_con_flag) {
		if(ble_record_start) {
			can_send_real_record_data = false;
		}
	}
	if(ble_con_flag) { //连接上以后20S未绑定则断开
        
	}
}

AT(.ble_app_sram_seg)
bool get_ble_con_status(void)
{
    return ble_con_flag;
}

uint8_t get_ascall(uint8_t tmp)
{
    logi("get_ascall = %x",tmp);
    if(tmp >=0 && tmp <= 9){
        return tmp + '0';
	} else if (tmp >=10 && tmp <= 15){
        return tmp - 10 + 'a';       
	}
	return 0;
}

void SN_2_ASCALL(uint32_t tmp)
{
    uint8_t tmp_8;
	
	tmp_8 = tmp;
	tmp_8 = tmp_8 & 0x0f;
	SN_NUM[7] = get_ascall(tmp_8);

	tmp_8 = tmp;
	tmp_8 = tmp_8 & 0xf0;
	SN_NUM[6] = get_ascall(tmp_8 >> 4);

	tmp_8 = tmp >> 8;
	tmp_8 = tmp_8 & 0x0f;
    SN_NUM[5] = get_ascall(tmp_8);

	tmp_8 = tmp >> 8;
	tmp_8 = tmp_8 & 0xf0;
	SN_NUM[4] = get_ascall(tmp_8 >> 4);

	tmp_8 = tmp >> 16;
	tmp_8 = tmp_8 & 0x0f;
	SN_NUM[3] = get_ascall(tmp_8);

	tmp_8 = tmp >> 16;
	tmp_8 = tmp_8 & 0xf0;
	SN_NUM[2] = get_ascall(tmp_8 >> 4);

	tmp_8 = tmp >> 24;
	tmp_8 = tmp_8 & 0x0f;
	SN_NUM[1] = get_ascall(tmp_8);

	tmp_8 = tmp >> 24;
	tmp_8 = tmp_8 & 0xf0;
	SN_NUM[0] = get_ascall(tmp_8 >> 4);
}



AT(.ble_app_flash_seg)
void ble_adv_init(void)
{
#if 0
    uint8_t name_len;
    uint8_t adv_offset;
	uint8_t adv_offset_bat;
	uint16_t sys_version;

    //01-AD_TYPE_FLAGS
    adv_data[0] = 0x02;
    adv_data[1] = AD_TYPE_FLAGS;
    adv_data[2] = FLAG_LE_GENERAL_DISCOVER_MODE | FLAG_BR_EDR_NOT_SUPPORTED;
    adv_offset = 3;

    //09-AD_TYPE_COMPLETE_LOCAL_NAME
    name_len = ble_name_len_get();
    adv_data[adv_offset] = name_len + 1;
    adv_data[adv_offset+1] = AD_TYPE_COMPLETE_LOCAL_NAME;
    ble_name_set(&adv_data[adv_offset+2]);
    adv_offset += name_len + 2;

    //02-AD_TYPE_INCOMPLETE_UUID_16
    adv_data[adv_offset] = 3;
    adv_data[adv_offset+1] = AD_TYPE_INCOMPLETE_UUID_16;
    adv_data[adv_offset+2] = (uint8_t)UUID_SERVICE_OTA;
    adv_data[adv_offset+3] = (uint8_t)(UUID_SERVICE_OTA>>8);
    adv_offset += 4;

    //FF-AD_TYPE_MANUFACTURER_SPECIFIC_DATA
    adv_data[adv_offset] = ADV_FF_LEN + 1;                                     //length
    adv_data[adv_offset+1] = AD_TYPE_MANUFACTURER_SPECIFIC_DATA;               //ad type

    //经典蓝牙地址
    //logx("bt addr:");
     //printf_array(bt_local_addr, 6);
    
    memcpy(adv_data+adv_offset+2+ADV_FF_OFFSET_ADDR, bt_local_addr, 6);

    //设备电量
    adv_offset_bat = adv_offset+2+ADV_FF_OFFSET_BAT;
    adv_data[adv_offset_bat] = 100;
    //logi("bat offset: %d", adv_offset_bat);

    //固件版本
    sys_version = 0x9527;
    adv_data[adv_offset+2+ADV_FF_OFFSET_VER] = (uint8_t)sys_version;
    adv_data[adv_offset+2+ADV_FF_OFFSET_VER+1] = (uint8_t)(sys_version>>8);

    logx("adv:");
    printf_array(adv_data, 31);


    //SCAN RSP
    scan_rsp[0] = 1 + ble_name_len_get();
    scan_rsp[1] = AD_TYPE_COMPLETE_LOCAL_NAME;
    ble_name_set(&scan_rsp[2]);
#else
    uint8_t tmp_8;
    uint8_t adv_offset;
    uint32_t tmp;
    //01-AD_TYPE_FLAGS
    adv_data[0] = 0x02;
    adv_data[1] = AD_TYPE_FLAGS;
    adv_data[2] = FLAG_LE_GENERAL_DISCOVER_MODE | FLAG_BR_EDR_NOT_SUPPORTED;
    adv_offset = 3;

    //09-AD_TYPE_COMPLETE_LOCAL_NAME
    tmp_8 = ble_name_len_get();
    adv_data[adv_offset] = tmp_8 + 1;
    adv_data[adv_offset+1] = AD_TYPE_COMPLETE_LOCAL_NAME;
    ble_name_set(&adv_data[adv_offset+2]);
    adv_offset += tmp_8 + 2;

      //02-AD_TYPE_INCOMPLETE_UUID_16
//    adv_data[adv_offset] = 3;
//    adv_data[adv_offset+1] = AD_TYPE_INCOMPLETE_UUID_16;
//    adv_data[adv_offset+2] = (uint8_t)UUID_SERVICE_OTA;
//    adv_data[adv_offset+3] = (uint8_t)(UUID_SERVICE_OTA>>8);
//    adv_offset += 4;

    //FF-AD_TYPE_MANUFACTURER_SPECIFIC_DATA
    adv_data[adv_offset] = 17;                                     //length
    adv_data[adv_offset+1] = AD_TYPE_MANUFACTURER_SPECIFIC_DATA;               //ad type
    adv_offset += 2;
    //经典蓝牙地址
    //logx("bt addr:");
     //printf_array(bt_local_addr, 6);
    
    memcpy(adv_data+adv_offset+ADV_FF_OFFSET_ADDR, bt_local_addr, 6);
	adv_offset += 6;

    //设备电量
    //adv_offset_bat = adv_offset+2+ADV_FF_OFFSET_BAT;
    adv_data[adv_offset] = pmu_get_bat_quantity_percent();
	adv_offset += 1;
    //logi("bat offset: %d", adv_offset_bat);
    
    //固件版本
//    sys_version = 0x9527;
//    adv_data[adv_offset+2+ADV_FF_OFFSET_VER] = (uint8_t)sys_version;
//    adv_data[adv_offset+2+ADV_FF_OFFSET_VER+1] = (uint8_t)(sys_version>>8); 

	//SN号
	tmp = sys_get_sn();
	logi("arunboy SN_NUM : 0x%x",tmp);
	SN_2_ASCALL(tmp);
    //info_get(INFO_SET_SN, (void *)&tmp, 4);
	adv_data[adv_offset + 3] = tmp;
	adv_data[adv_offset + 2] = tmp >> 8;
	adv_data[adv_offset + 1] = tmp >> 16;
	adv_data[adv_offset + 0] = tmp >> 24;
	adv_offset += 4;

	info_get(INFO_USER_BIND_FLAG, (void *)&tmp_8, 1);
	adv_data[adv_offset] = tmp_8;
	adv_offset += 1;

	info_get(INFO_USER_INFO, (void *)&tmp, 4);
	adv_data[adv_offset + 3] = tmp;
	adv_data[adv_offset + 2] = tmp >> 8;
	adv_data[adv_offset + 1] = tmp >> 16;
	adv_data[adv_offset + 0] = tmp >> 24;
	
    logx("adv:");
    printf_array(adv_data, 31);

    //SCAN RSP
    scan_rsp[0] = 1 + ble_name_len_get();
    scan_rsp[1] = AD_TYPE_COMPLETE_LOCAL_NAME;
    ble_name_set(&scan_rsp[2]);
#endif
}


AT(.ble_app_sram_seg)
static int32_t ble_app_record_data_read(ble_app_st *self,
    void *buf, int32_t bytes)
{
    ble_app_st *ble_app_t = (ble_app_st*)self;;
    uint32_t avail;
    uint32_t r_bytes;
     uint8_t *p;

    while (bytes) {
        ring_buffer_get_r_block(&ble_app_t->bufctrl, &avail, &p);
           if (avail > 0) {
            r_bytes = MIN(avail, SHAREBUF_PERIOD_SZ_BlE); /* period limit */
            r_bytes = MIN(r_bytes, bytes);
            memcpy(buf, p, r_bytes);
			
            buf = (uint8_t*)buf + r_bytes;
            bytes -= r_bytes;
            ring_buffer_update_rpos(&ble_app_t->bufctrl, r_bytes);
        } else {
            if (0 == uxQueueMessagesWaiting(ble_app_t->cmd_queue)) {
                osSemaphoreTake(ble_app_t->read_sem_sync, portMAX_DELAY);
                if(ble_app_t->exit_flag) {
                    loge("%s, %d\n", __func__, __LINE__);
                    break;
               }
            }
        }
    }
    return 0;
}

AT(.ble_app_sram_seg)
int32_t ble_app_record_data_push(  uint8_t *buf, int32_t bytes)
{
    ble_app_st *ble_app_t = (ble_app_st*)g_ble_app_t;
    uint32_t avail;
    uint32_t w_bytes;
    uint8_t *p;

    if (ble_app_t == NULL || ble_app_t->status != BLE_APP_STATUS_RECORDING /*|| !ble_con_flag*/) {
        logi("ble_app_t null");
        return -1;
    }
//    loge("%s, %d", __func__, __LINE__);
    while (bytes) {
        ring_buffer_get_w_block(&ble_app_t->bufctrl, &avail, &p);
         if (avail > 0) {
                w_bytes = MIN(avail, SHAREBUF_PERIOD_SZ_BlE); /* period limit */
                w_bytes = MIN(w_bytes, bytes);
                memcpy(p, buf, w_bytes);
                buf = (uint8_t*)buf + w_bytes;
                bytes -= w_bytes;
                ring_buffer_update_wpos(&ble_app_t->bufctrl, w_bytes);
                osSemaphoreGive(ble_app_t->read_sem_sync);
        } else {
            logw("ble app ain push overrun");
            break;
        }
    }

    return 0;
}


AT(.ble_app_flash_seg)
static int ble_app_start(void *handle)
{
    ble_app_st *ble_app_t = (ble_app_st*)handle;
    ble_app_t->sharebuf = general_calloc(SHAREBUF_SZ_BLE);//general是申请的audio_heap里面的，libc_mem是申请os_heap里面的  都是在BSS段
    if (NULL == ble_app_t->sharebuf) {
        loge("malloc share buf fail");
        return -1;
    }
    ring_buffer_init(&ble_app_t->bufctrl, ble_app_t->sharebuf, SHAREBUF_SZ_BLE);
    if(NULL == ble_app_t->read_sem_sync)
        ble_app_t->read_sem_sync = xSemaphoreCreateBinary();

    if (NULL == ble_app_t->read_sem_sync) {
        loge("create read_sem_sync fail");
        libc_free(ble_app_t);
        return 0;
    }
//    ble_send_id_flag = false;
    ble_app_t->status = BLE_APP_STATUS_RECORDING;
    loge("%s, %d", __func__, __LINE__);
    return 0;
}

AT(.ble_app_flash_seg)
static void ble_app_stop(void *handle)
{
    ble_app_st *ble_app_t = (ble_app_st*)handle;
    if (ble_app_t->sharebuf) {
        general_free(ble_app_t->sharebuf);
        ble_app_t->sharebuf = NULL;
    }
    if (ble_app_t->read_sem_sync) {
        vSemaphoreDelete(ble_app_t->read_sem_sync);
        ble_app_t->read_sem_sync = NULL;
    }
    ble_app_t->status = BLE_APP_STATUS_STOPPED;
}

AT(.ble_app_sram_seg)
static int ble_app_process_cmd(void *handle, uint32_t wait_ticks)
{
    ble_app_st *ble_app_t = (ble_app_st*)handle;

    ble_app_msg_t msg;
    msg.cmd = BLE_APP_CMD_NONE;
    msg.val = 0;
    
    if (pdPASS !=osQueueReceive(ble_app_t->cmd_queue, &msg, wait_ticks)){
        return -1;
    }

    switch (msg.cmd) {
    case BLE_APP_CMD_START:
        ble_app_start(ble_app_t);
        break;

    case BLE_APP_CMD_STOP:
        ble_app_stop(ble_app_t);
        break;
    default:
        break;
    }
    return 0;
}

AT(.ble_app_sram_seg)
int32_t process_app_recording(ble_app_st *ble_app_t)
{
	uint8_t send_record_data[BT_BUFFER_SIZE];
    if (NULL == ble_app_t) {
        return -1;
    }
	if(first_send_real_time_flag) {
		uint32_t current_wav_id = get_current_wav_id();
		libc_memcpy(send_record_data, BANBEN_ID, LEN_FIRST_BANBEN_ID);
		libc_memcpy(send_record_data + 2, FIRST_FRAME, 6);
		*(send_record_data + 8) = current_wav_id;
		*(send_record_data + 9) = current_wav_id >> 8;
		*(send_record_data + 10) = current_wav_id >> 16;
		*(send_record_data + 11) = current_wav_id >> 24;
        first_send_real_time_flag = false;
		ble_app_record_data_read(ble_app_t,send_record_data + 12,BT_BUFFER_SIZE - 12);
		if (can_send_real_record_data) {
			ble_app_data_send(HANDLE_SEND_AUDIO,send_record_data,BT_BUFFER_SIZE);
		}
	} else {
        ble_app_record_data_read(ble_app_t,send_record_data,BT_BUFFER_SIZE);
		if (can_send_real_record_data) {
			ble_app_data_send(HANDLE_SEND_AUDIO,send_record_data,BT_BUFFER_SIZE);
		}
	}
	
    return 0;
}


AT(.ble_app_sram_seg)
static void ble_app_loop(void *handle)
{
    ble_app_st *ble_app_t = (ble_app_st*)handle;
     
    while (1) {
        if (BLE_APP_STATUS_RECORDING == ble_app_t->status) {
            ble_app_process_cmd(ble_app_t, 0);
        } else {
            ble_app_process_cmd(ble_app_t, portMAX_DELAY);
        }
        if (BLE_APP_STATUS_RECORDING == ble_app_t->status) {
            process_app_recording(ble_app_t);
        }
    }
}


AT(.ble_app_sram_seg)
int ble_app_cmd_send(ble_app_st *ble_app_t, ble_app_msg_t *msg,bool start_flag)
{
    if (pdPASS != osQueueSend(ble_app_t->cmd_queue, msg, 2000)) {
          return -1;
      }
      /* wait start complete */
      while (1) {
           if(start_flag) {
               if (BLE_APP_STATUS_STOPPED == ble_app_t->status) {
                  vTaskDelay(2);
              } else {
                  break;
              }
           } else {
                 if (BLE_APP_STATUS_STOPPED != ble_app_t->status) {
                    vTaskDelay(2);
                } else {
                    break;
                }
           }
      }
      return 0;
}

AT(.ble_app_sram_seg)
int ble_app_cmd(uint32_t cmd, uint32_t val)
{
    ble_app_st *ble_app_t = (ble_app_st*)g_ble_app_t;
    ble_app_msg_t msg;

    if (NULL == ble_app_t) {
        return -1;
    }

    switch (cmd) {
    case BLE_APP_CMD_START:
        msg.cmd = BLE_APP_CMD_START;
        msg.val = val;
        ble_app_t->exit_flag = false;
        ble_app_cmd_send(ble_app_t,&msg,true);
        break;

    case BLE_APP_CMD_STOP:
        msg.cmd = BLE_APP_CMD_STOP;
        msg.val = val;
        ble_app_t->exit_flag= true;
        osSemaphoreGive(ble_app_t->read_sem_sync);
        ble_app_cmd_send(ble_app_t,&msg,false);
        break;
    }
    return 0;
}

AT(.ble_app_flash_seg)
uint8_t ble_tast_init(void)
{
    ble_app_st *ble_app_t;
    ble_app_t = libc_calloc(sizeof(ble_app_st));
    if (NULL == ble_app_t) {
        loge("malloc ble_app_t fail");
        return 0;
    }

    ble_app_t->cmd_queue = xQueueCreate(BLE_APP_MSG_QUEUE_LENGTH,
        sizeof(ble_app_msg_t));
    if (NULL == ble_app_t->cmd_queue) {
          loge("creat cmd_queue fail");
       
    }
    
    /* create ble app task */
    if (pdFAIL == xTaskCreate(ble_app_loop, BLE_APP_TASK_NAME,
        BLE_APP_TASK_STACK_SIZE, ble_app_t, BLE_APP_TASK_PRIO, &ble_app_t->task_handle)) {
          loge("xTaskCreate fail");
        goto create_task_fail;
    }

    ble_app_t->status = BLE_APP_STATUS_STOPPED;
    g_ble_app_t = ble_app_t;
 
    return 0;
    create_task_fail:
    return 0;
}

AT(.ble_app_flash_seg)
void ble_app_init()
{
#if 1
    ble_adv_init();

   // ble_name_set(&simple_scan_rsp[2]);
   // simple_scan_rsp[0] = 1 + ble_name_len_get();
    ble_addr_set(ble_app.own_addr);
#if BLE_ADDR_USE_RANDOM
    ble_app.own_addr_type = RANDOM_ADDRESS;
#else
    ble_app.own_addr_type = PUBLIC_ADDRESS;
#endif
    memcpy(ble_app.own_addr, ble_local_addr, 6);

    ble_app.adv_data = adv_data;
    ble_app.adv_data_len = sizeof(adv_data);
    ble_app.scan_rsp = simple_scan_rsp;
    ble_app.scan_rsp_len = sizeof(simple_scan_rsp);
    ble_app.att_db = (uint8_t *)simple_att_db;
    ble_app.start_handle = 0x0000;
    ble_app.end_handle = 0xFFFF;
    ble_app.read_callback = ble_app_data_read;
    ble_app.write_callback = ble_app_data_receive;
    ble_profile_init(&ble_app);
 #endif
#if 1
    ble_tast_init();
	send_file_init();
    ble_con_flag = false;
#endif
}

AT(.ble_app_flash_seg)
void ble_app_exit()
{
    if (g_ble_app_t)
    libc_free(g_ble_app_t);
	if (g_send_rec_file_t)
	libc_free(g_send_rec_file_t);
}



