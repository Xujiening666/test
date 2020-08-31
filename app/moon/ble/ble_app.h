#ifndef _BLE_APP_
#define _BLE_APP_

enum att_uuid_e {
    UUID_SERVICE_01 = 0xAA00,
    UUID_RECV_01    = 0xAA01,
    UUID_SEND_01    = 0xAA02,

    UUID_SERVICE_OTA = 0xFFF0,
    UUID_RECV_OTA    = 0xFFF1,
    UUID_SEND_OTA    = 0xFFF2,

	UUID_SERVICE_AUDIO = 0xFFF3,
    UUID_RECV_AUDIO    = 0xFFF4,
    UUID_SEND_AUDIO    = 0xFFF5,

	UUID_SERVICE_FILE = 0xFFF6,
    UUID_RECV_FILE    = 0xFFF7,
    UUID_SEND_FILE    = 0xFFF8,

	UUID_SERVICE_CMD = 0xFFF9,
    UUID_RECV_CMD    = 0xFFFA,
    UUID_SEND_CMD    = 0xFFFB,
};

enum att_handle_e {
    HANDLE_RECV_01  = 0x09,
    HANDLE_SEND_01  = 0x0B,

    HANDLE_RECV_OTA  = 0x0E, //原来的,不动,支持OTA.
    HANDLE_SEND_OTA  = 0x10,

	HANDLE_RECV_AUDIO  = 0x14, //AudioStream
	HANDLE_SEND_AUDIO  = 0x16,

	HANDLE_RECV_FILE  = 0x1a, //FileStream
	HANDLE_SEND_FILE  = 0x1c,

	HANDLE_RECV_CMD  = 0x20, //CmdStream
	HANDLE_SEND_CMD  = 0x22,
};

enum adv_manufacturer_specific_data_e {
    ADV_FF_OFFSET_ADDR      = 0,    //0~5:经典蓝牙地址
    ADV_FF_OFFSET_BAT       = 6,    //6:设备电量
    ADV_FF_OFFSET_VER       = 7,    //7~8:固件版本

    ADV_FF_LEN_ADDR         = 6,
    ADV_FF_LEN_BAT          = 1,
    ADV_FF_LEN_VER          = 2,
    ADV_FF_LEN              = ADV_FF_LEN_ADDR + ADV_FF_LEN_BAT + ADV_FF_LEN_VER,
};

enum  cmd_Protocol_data{
    OFFSET_FIRST_BANBEN_ID     = 0,    //0~1:版本协议
	OFFSET_FIRST_FRAME		   = 2,    //2~7:首帧标识
    OFFSET_CMD				   = 8,    //8:命令,回复也是这个
	OFFSET_ERROR               = 9,    //9:错误代码

	LEN_FIRST_BANBEN_ID		   = 2,
	LEN_FIRST_FRAME 		   = 6,
	LEN_END_BANBEN_ID		   = 2,
    LEN_END_FRAME              = 6,
	LEN_DATA_LENGTH            = 4,
	
    LEN_FIRST                  = 8,
    LEN_END                    = 6,
    LEN_SUM                    = LEN_FIRST + LEN_END,
};
	
enum cmd_file_list_cmd{
	LEN_NAME_BIT               = 9,   //{"name":"
	LEN_NAME_DATA              = 12,
	
	LEN_SIZE_BIT               = 9,	  //","size":

	LEN_ID_BIT                 = 6,   //,"id":

	LEN_SN_BIT                 = 7,   //,"sn":"
	LEN_SN_DATA                = 4,

	LEN_DURATION_BIT           = 13,  //","duration":

	LEN_END_BIT                = 2,   //},	
	LEN_SUM_LIST		       = LEN_NAME_BIT + LEN_NAME_DATA + LEN_SIZE_BIT
	                             + LEN_ID_BIT +  LEN_SN_BIT + LEN_SN_DATA
	                             + LEN_DURATION_BIT + LEN_END_BIT,

	LEN_END_END_BIT            = 3,   //}]}
	LEN_END_SUM_LIST           = LEN_NAME_BIT + LEN_NAME_DATA + LEN_SIZE_BIT
	                             + LEN_ID_BIT +  LEN_SN_BIT + LEN_SN_DATA
	                             + LEN_DURATION_BIT + LEN_END_END_BIT,

	OFFSET_NAME_BIT 		   = 0,   //0~8 {"name":"
	OFFSET_NAME_DATA		   = OFFSET_NAME_BIT + LEN_NAME_BIT,   //9~20 rec00001.wav

	OFFSET_SIZE_BIT 		   = OFFSET_NAME_DATA + LEN_NAME_DATA,
	OFFSET_SIZE_DATA		   = OFFSET_SIZE_BIT + LEN_SIZE_BIT,

	OFFSET_ID_BIT              = OFFSET_SIZE_DATA,
	OFFSET_ID_DATA             = OFFSET_ID_BIT + LEN_ID_BIT,

	OFFSET_SN_BIT              = OFFSET_ID_DATA,
	OFFSET_SN_DATA             = OFFSET_SN_BIT + LEN_SN_BIT,

	OFFSET_DURATION_BIT        = OFFSET_SN_DATA + LEN_SN_DATA,
	OFFSET_DURATION_DATA       = OFFSET_DURATION_BIT + LEN_DURATION_BIT,

	OFFSET_END_BIT             = OFFSET_DURATION_DATA,
};

enum packet_tx_len_e {
    TX_LEN_START            = 1,
    TX_LEN_RESET            = 1,
};

enum system_status_e {
    STATUS_INIT             = 0,
    STATUS_START,
    STATUS_RESET,
};

/*
    0~3:重启后跳转标志
    4~9:蓝牙地址
    10~15:ble地址
    16:左/右耳标志
*/
enum tag_e {
    OFFSET_BRANCH   = 0,
    OFFSET_BT_ADDR  = 4,
    OFFSET_BLE_ADDR = 10,
    OFFSET_LR_FLAG  = 16,

    SIZE_BRANCH     = 4,
    SIZE_BT_ADDR    = 6,
    SIZE_BLE_ADDR   = 6,
    SIZE_LR_FLAG    = 1,
    SIZE_TAG = SIZE_BRANCH + SIZE_BT_ADDR + SIZE_BLE_ADDR + SIZE_LR_FLAG,
};

enum ble_app_status_e {
    BLE_APP_STATUS_STOPPED,
    BLE_APP_STATUS_RECORDING,
    BLE_APP_STATUS_ERROR,
};

enum BLE_APP_cmd_e {
    BLE_APP_CMD_NONE,
    BLE_APP_CMD_START,
    BLE_APP_CMD_STOP,
};

enum packet_offset_e {
    OFFSET_TAG              = 0,    //1B
    OFFSET_START_VERSION    = 1,    //2B
    OFFSET_START_BAT        = 3,    //1B

    OFFSET_SIZE_LEN         = 1,    //4B

    OFFSET_DATA_PACKET_NUM  = 1,    //2B
    OFFSET_DATA_PACKET_LEN  = 3,    //2B
    OFFSET_DATA             = 5,    //
};

enum err_code_num {
    ERR_CODE_SUCCESS         = 0x00,     //成功
    ERR_CODE_UNKNOW_CMD      = 0x11,     //命令接收出错
    ERR_CODE_UNBIND_USER     = 0x19,     //设备未绑定
    ERR_CODE_NEED_CLOSE_USB  = 0x15,     //不合法的操作，插上usb不能操作文件
    ERR_CODE_NOT_GET_STORAGE = 0x20,     //获取容量失败
    ERR_CODE_NO_RECORD_MODE  = 0x21,     //无法开启录音，需切换至录音模式
    ERR_CODE_NO_SECTION      = 0x22,     //开启录音空间不足
    ERR_CODE_NO_START_REC    = 0x23,     //开启录音设备异常
    ERR_CODE_RECORD_PLAYING  = 0x24,     //无法开启录音, 设备正在播放
	ERR_CODE_SEND_FILE_ERR   = 0x33,     //发送文件错误
	ERR_CODE_DEL_DILE_ERR    = 0x34,     //删除文件失败
};

enum packet_tag_e {
    TAG_START                = 0xA0,             //(START)
    TAG_RESET                = 0xA1,             //设备复位(RESET)
    TAG_RECORD_START         = 0x61,             //透传开始
    TAG_RECORD_STOP          = 0x62,             //透传结束
    TAG_GET_DIR              = 0x63,             //获取文件列表
    TAG_SYNC_FILE_ID         = 0x64,             //同步文件(ID)
    TAG_SYNC_FILE_ID_STOP    = 0x65,             //停止同步文件(ID)
    TAG_DEL_FILE_ID          = 0x66,             //删除文件(ID)
    TAG_GET_STORAGE          = 0x67,             //获取容量
    TAG_GET_TIME             = 0x68,             //获取时间
    TAG_SET_TIME             = 0x69,             //设置时间
    TAG_RESET_DEVICE         = 0x6A,             //恢复出厂设置
    TAG_GET_VER              = 0x6B,             //获取固件版本
    TAG_SET_USB              = 0x6C,             //设置usb模式
    TAG_GET_USER             = 0x6D,             //获取用户信息
    TAG_GET_BATTERY          = 0x6E,             //获取电池电量
    TAG_SET_USER             = 0x6F,             //设置用户信息
    TAG_UNBIND_USER          = 0x70,             //解除绑定
    TAG_GET_DEVICE_STATS     = 0x71,             //获取设备状态
	TAG_SYNC_FILE_NAME	     = 0x72,			 //同步文件(NAME)
	TAG_SYNC_FILE_NAME_STOP  = 0x73,			 //停止同步文件(NAME)
	TAG_DEL_FILE_NAME 	     = 0x74,			 //删除文件(NAME)
    TAG_SET_PRIVATE_MODE	 = 0x75,			 //设置隐式模式
    TAG_GET_STATS            = 0x76,             //获取简单的设别状态
    TAG_SEND_APP_STOP_RECORD = 0x77,             //设备主动通知结束录音
    TAG_SEND_APP_START_RECORD= 0x78,             //设备主动开启录音通知
    TAG_VOLUNT_UNBIND_USER   = 0x79,             //设备主动解除绑定
    TAG_ARUNBOY1             = 0x01,             //测试
    TAG_ARUNBOY2             = 0x02,             //测试
    TAG_ARUNBOY3             = 0x03,             //测试
 };



void ble_app_init(void);
void ble_app_receive_process(uint8_t *buf);
void ble_addr_set(uint8_t *ble_addr);
void ble_name_set(uint8_t *ble_name);
void ble_adv_update(void);


void ble_ota_send(uint8_t *buf, uint16_t size);
void ble_cmd_send(uint8_t *buf, uint16_t size);
uint8_t ble_file_send(uint8_t *buf, uint16_t size);
void ble_ota_erase_flash(void);
void ble_tx_timer_create(void);
void ble_tx_timer_start(void);
void ble_tx_timer_stop(void);
void ble_tx_timer(xTimerHandle xTimer);

uint8_t bat_quantity_get(void);
void gap_advertisements_enable(int enabled);
void gap_advertisements_set_data(uint8_t advertising_data_length, uint8_t * advertising_data);
int ble_app_cmd(uint32_t cmd, uint32_t val);
int32_t ble_send_cmd(uint8_t msg);
void update_ble_con_status(bool con);
void ble_app_process(void);
bool get_ble_con_status(void);
void ble_ota_send(uint8_t *buf, uint16_t size);
int ble_app_data_send(uint16_t attr_handle, uint8_t *value, uint16_t value_len);
uint8_t ble_record_start_flag(void);
void send_record_message(uint8_t cmd);
void set_ble_record_start_flag(uint8_t flag);
void SN_2_ASCALL(uint32_t tmp);
void send_unbind_user_msg(void);
void send_simple_status(void);
void send_battery_message(void);


#endif
