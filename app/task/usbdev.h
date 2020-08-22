#ifndef _MODE_USBDEV_H_
#define _MODE_USBDEV_H_

/*
以下是设置USB 声卡字符串的方法，操作步骤
1、定义一个字符串，USBD_PRODUCT_FS_STRING
2、实现一个函数USBD_COMP_ProductStrDescriptor
3、将字符串转化成UNICODE码
4、USBD_COMP_ProductStrDescriptor函数中返回转化后的字符串
5、修改COMP_Desc结构体的相关指针

下面是参考代码

typedef struct
{
  uint8_t  *(*GetDeviceDescriptor)(uint16_t *length);
  uint8_t  *(*GetLangIDStrDescriptor)(uint16_t *length);
  uint8_t  *(*GetManufacturerStrDescriptor)(uint16_t *length);
  uint8_t  *(*GetProductStrDescriptor)(uint16_t *length);
  uint8_t  *(*GetSerialStrDescriptor)(uint16_t *length);
  uint8_t  *(*GetConfigurationStrDescriptor)(uint16_t *length);
  uint8_t  *(*GetInterfaceStrDescriptor)(uint16_t *length);
#if (USBD_LPM_ENABLED == 1)
  uint8_t  *(*GetBOSDescriptor)(uint16_t *length);
#endif
  uint16_t (*GetUsbPID)(void);
} USBD_DescriptorsTypeDef;


uint8_t USBD_StrDesc[256];
extern USBD_DescriptorsTypeDef COMP_Desc;

#define USBD_PRODUCT_FS_STRING "USB device 2.0"
void USBD_GetString(uint8_t *desc, uint8_t *unicode, uint16_t *len);


AT(.usb_seg)
uint8_t *USBD_COMP_ProductStrDescriptor(uint16_t *length)
{
    USBD_GetString((uint8_t *)USBD_PRODUCT_FS_STRING, USBD_StrDesc, length);
    return USBD_StrDesc;
}


AT(.usbd_msc_init_seg)
void USBD_SetAudioString()
{
    COMP_Desc.GetProductStrDescriptor = USBD_COMP_ProductStrDescriptor_new;
}


*/

enum hid_type_e {
    HID_VOL_UP = 0x01,
    HID_VOL_DOWN = 0x02,
    HID_PLAY_PAUSE = 0x08,
    HID_NEXT = 0x10,
    HID_PRE = 0x20
};

typedef struct _USBD_AUDIO_PARAM_
{
    void (*AudioSystemParamInit)(uint32_t* pcmPacketSize, uint32_t* micPacketSize);
}USBD_AudioSystem_Typedef;

void mode_usbdev(void);
void mode_usbdev_enter(void);
void mode_usbdev_loop(void);
void mode_usbdev_exit(void);

void usbdev_event_process(uint32_t msg);

void usbaudio_init(void);
void usbaudio_deinit(void);

void usbd_module_init(void);
void usbd_module_deinit(void);
void usbdev_init(void);
void usbdev_deinit(void);
void usbdev_all_mode_exit(void);


#endif

