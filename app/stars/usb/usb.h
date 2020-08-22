#ifndef _USB_H_
#define _USB_H_

typedef enum _E_HID_KEY
{
    HID_KEY_UP = 0,
    HID_KEY_VOL_UP = 0x01,
    HID_KEY_VOL_DOWN = 0x02,
    HID_KEY_PLAY_PAUSE = 0x08,
    HID_KEY_NEXT = 0x10,
    HID_KEY_PRE = 0x20
}E_HID_KEY;


enum usb_detect_status_e {
    USB_DETECT_INIT = 0,
    USB_DETECT,
    USB_WORKING,
    USB_DISABLE,
};

typedef struct {
    enum usb_detect_status_e sta;
    uint8_t host_out_cnt;
    uint8_t host_in_cnt;
    uint8_t pc_in_cnt;
    uint8_t pc_out_cnt;
    uint32_t sof_frame_num;
    uint8_t dm_status_check_cnt;
    uint8_t edge_cnt;
    bool sof_check_en;
    bool dm_status_check_en;
    bool edge_check_en;
} usb_det_t;
extern usb_det_t usb_det;

enum usb_detect_mode_e {
    USB_HOST = 0,
    USB_DEVICE,
};

#define USB_HOST_DET_CNT        10  //20ms * 10
#define USB_DEVICE_DET_CNT      20  //20ms * 20     /* when some udisks are inserted, the DM will be low for 160ms */


void COMP_HID_feed_back(E_HID_KEY keyType);
void USBD_COMP_Start();
void USBD_Deinitialize();
void COMP_USB_EQ_Enable();
uint8_t USBD_EnableCardReader(BOOL bEnable);
void USB_UsbAudioInquiry_Init();
void usb_suspend(void);
void usb_resume(void);
void COMP_USB_Data_Dump_Enable();
BOOL is_in_update_process();
void USBD_Comp_Stereo_Enable();
void USBD_Mic_Stereo_Enable();
void USBD_Audio_Stereo_Enable();

#define DEVICE_USER_HANDLE_CFG                      1
#define DEVICE_USER_MSP_INIT                        2
#define DEVICE_USER_MSP_DEINIT                      3
#define DEVICE_USER_COMP_SOF                        4
#define DEVICE_USER_COMP_IsoINIncomplete            5
#define DEVICE_USER_COMP_IsoOutIncomplete           6
#define DEVICE_USER_COMP_DEBUG                      7
#define DEVICE_USER_COMP_EP0TXRDY                   8
#define DEVICE_USER_COMP_AudioDataIn                9
#define DEVICE_USER_COMP_AudioEP0TxReady            10
#define DEVICE_USER_COMP_AudioSOF                   11
#define DEVICE_USER_COMP_AudioIsoINIncomplete       12
#define DEVICE_USER_COMP_AudioIsoOUTIncomplete      13
#define DEVICE_USER_COMP_AudioRecCallback           14
#define DEVICE_USER_COMP_HIDGetReportData           15
#define DEVICE_USER_COMP_AudioRecordCallback        16

#define USBD_COMP_AUDIO_FREQ                        48000

#define USB_HS_MAX_PACKET_SIZE                      512
#define USB_FS_MAX_PACKET_SIZE                      64
#define USB_MAX_EP0_SIZE                            64

#define COMP_AUDIO_OUT_PACKET                       (uint32_t)(((USBD_COMP_AUDIO_FREQ * 2 * 2) /1000))
#define COMP_AUDIO_OUT_PACKET_NUM                   8
#define COMP_AUDIO_TOTAL_BUF_SIZE                   ((uint32_t)(COMP_AUDIO_OUT_PACKET * COMP_AUDIO_OUT_PACKET_NUM))

typedef struct
{
   uint8_t cmd;
   uint8_t data[USB_MAX_EP0_SIZE];
   uint8_t len;
   uint8_t unit;
}
USBD_COMP_AUDIO_ControlTypeDef;

typedef enum
{
  COMP_AUDIO_OFFSET_NONE = 0,
  COMP_AUDIO_OFFSET_HALF,
  COMP_AUDIO_OFFSET_FULL,
  COMP_AUDIO_OFFSET_UNKNOWN,
}
COMP_AUDIO_OffsetTypeDef;

#if (SYS_CHIP_ID == CHIP_SL6800)

#define IN
#define OUT

#define COMP_AUDIO_MUTE_TYPE_VALUE                  0x100
#define COMP_AUDIO_VOLUME_TYPE_VALUE                0x200

typedef struct _USBD_AUDIO_CALLBACK_
{
    void (*InitAudio)(uint32_t OUT *ptrPcmBuf,uint32_t IN pcmBufLength);
    uint32_t (*OnPrepareMicData)(uint32_t OUT *ptrMicBuf, uint32_t* IN OUT length);
    void (*OnNewPcmReceiveBuff)(uint8_t IN *newBuf);
    void (*GetAudioCurrentVolumeState)(uint16_t IN unit,uint16_t IN volType, uint16_t* OUT curValue);
    void (*GetAudioVolumeMin)(uint16_t* OUT min);
    void (*GetAudioVolumeMax)(uint16_t* OUT max);
    void (*GetAudioVolumeStep)(uint16_t* OUT step);
}USBD_Audio_CallbackTypedef;

typedef struct
{
    void  (*Init)         (uint32_t  AudioFreq, uint32_t Volume, uint32_t options);
    void  (*DeInit)       (uint32_t options);
    //void  (*AudioCmd)     (uint8_t* pbuf, uint32_t size, uint8_t cmd);
    void  (*VolumeCtl)    (uint16_t unit, uint16_t vol);
    void  (*MuteCtl)      (uint16_t unit, uint16_t cmd);
    //void  (*PeriodicTC)   (uint8_t cmd);
    //void  (*GetState)     (void);
}USBD_COMP_AUDIO_ItfTypeDef;

typedef struct
{
  __IO uint32_t             alt_setting;
  USBD_COMP_AUDIO_ControlTypeDef control;
}
USBD_COMP_AUDIO_HandleTypeDef;


void USBD_Comp_Initialize(USBD_COMP_AUDIO_ItfTypeDef* pAudioParam,USBD_Audio_CallbackTypedef* pAudioCallback);
void USBD_EQ_device_Init(BOOL bDumpMode);
void UsbDetectDisable();
void USBD_Scsi_Init();
void USBD_CardReader_Init();
void USBD_SetPid(uint16_t usbPid);
void USBD_SetManufacturerString(char* string,uint8_t len /*max is 8*/);
void USBD_SetProductString(char* string,uint8_t len /*max is 16*/);
void USBD_SetVersionString(char* string,uint8_t len /*max is 4*/);
#endif

int32_t udisk_initialize(void);
void usb_detect(void);
void usb_init(void);
void usb_pin_init(void);
void usb_clk_init(void);
void usb_clk_deinit(void);
uint32_t usb_dm_pin_get(void);
void usb_dp_pin_set(bool detecting);
void usb_dm_pin_set(bool detecting);
void usb_host_init(void);
void usb_host_deinit(void);
void usb_set_wakeup(bool enable);
void usb_detect_set(enum usb_detect_status_e sta);
DRESULT usb_get_status(void);
void usb_dev_enter(void);
void usb_dev_exit(void);
void usb_nodata_loop(void);
void usb_pc_in_proc(void);
void usb_pc_out_proc(void);
void usb_pc_connect_proc(void);
void usb_edge_cb(void);
void usb_pc_connect_detect_start(void);
bool usb_pc_is_connect(void);
void usb_device_detect_through_edge(xTimerHandle xTimer);
void usb_device_detect_through_sof(xTimerHandle xTimer);
uint32_t usb_get_sof_frame(void); //use in device mode only and before shut down usb module
void USBD_Audio_Initialize(USBD_COMP_AUDIO_ItfTypeDef* pAudioParam,USBD_Audio_CallbackTypedef* pAudioCallback);
//禁止声卡音量与PC同步功能，这样设备或者PC音量改变时不会影响到另一端，默认是打开的
void USBD_Audio_PC_Volume_Synchronize_Disable();
void USBD_Comp_PC_Volume_Synchronize_Disable();

void USBD_Microphone_Initialize(USBD_COMP_AUDIO_ItfTypeDef* pAudioParam,USBD_Audio_CallbackTypedef* pAudioCallback);
void  USBD_Start();
void  USBD_Stop();

#define HOST_USER_SELECT_CONFIGURATION      1
#define HOST_USER_CLASS_ACTIVE              2
#define HOST_USER_CLASS_SELECTED            3
#define HOST_USER_CONNECTION                4
#define HOST_USER_DISCONNECTION             5
#define HOST_USER_UNRECOVERED_ERROR         6
#define HOST_USER_MSP_INIT                  7
#define HOST_USER_MSP_DEINIT                8
#define HOST_USER_HANDLE_CFG                9
#define HOST_USER_SOFPROCESS                10


#if (USB_DM_PIN_SEL == 0)
#define USB_DM_PIN_CFG          (PIN_B3 | PIN_FUNC15 | PIN_DRI1)
#define USB_DM_PIN_NUM          (PIN_B3)
#define USB_DM_FUNC_WORKING     (PIN_FUNC15)
#define USB_DM_FUNC_DETECT      (PIN_FUNCINPUT)
#define USB_DM_FUNC_WKUP        (PIN_FUNC14)
#define USB_DM_PULL_DISABLE     (PIN_PULL_DISABLE)
#define USB_DM_PULL_DETECT      (PIN_PULL_UP100K)
#define USB_DM_PULL_WKUP        (PIN_PULL_UP100K)
#define USB_DM_DRI              (PIN_DRI1)
#elif (USB_DM_PIN_SEL == 1)

#endif

#if (USB_DP_PIN_SEL == 0)
#define USB_DP_PIN_CFG          (PIN_B4 | PIN_FUNC15 | PIN_DRI1)
#define USB_DP_PIN_NUM          (PIN_B4)
#define USB_DP_FUNC_WORKING     (PIN_FUNC15)
#define USB_DP_FUNC_DETECT      (PIN_FUNCINPUT)
#define USB_DP_FUNC_WKUP        (PIN_FUNC14)
#define USB_DP_PULL_DISABLE     (PIN_PULL_DISABLE)
#define USB_DP_PULL_DETECT      (PIN_PULL_DOWN100K)
#define USB_DP_PULL_WKUP        (PIN_PULL_DOWN100K)
#define USB_DP_DRI              (PIN_DRI1)

#elif (USB_DP_PIN_SEL == 1)

#endif

#endif

