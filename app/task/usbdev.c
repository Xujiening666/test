#define LOG_LEV     5
#define SLTK_LOG_TAG "usbdev"
#include "sys_inc.h"
#include "user_config.h"
#include "task_main.h"
#include "usb.h"
#include "libc_common.h"
#include "loadcode.h"
#include "usbaudio.h"
#include "xfs.h"
#include "sdmmc_io.h"
#include "timer.h"
#include "ble_app.h"

#if (MODE_USBDEV_EN || EQ_ALL_MODE_SUPPORT || USB_AUDIO_ALL_MODE_SUPPORT)

#define MAX_LOGIG_VOLUME    0xa0f0
#define MIN_LOGIG_VOLUME    0xa000
#define BLOCK_SAMPLES       (48000 / 1000)

//#define DEBUG_USB_MIC
#define ISOBUFF_SIZE    (192*2)

#if USB_STEREO_MIC_EN
#define ISOC_PACKET_LENGTH         (96*2)
#else
#define ISOC_PACKET_LENGTH         96
#endif

#define USBMICBUF_SIZE  (ISOC_PACKET_LENGTH*2)

typedef struct {
    USBD_COMP_AUDIO_ItfTypeDef USBD_AUDIO_fops_new;
    //  do not init a struct buffer use direct action, because it may not initialize when startup
    USBD_Audio_CallbackTypedef compCallback;
    uint16_t g_logicPcmVolume;
    uint16_t g_logicMicVolume;
    uint8_t  g_isPcmMute;
    uint8_t  g_isMicMute;

    uint8_t isoBuff[ISOBUFF_SIZE];
    uint8_t usb_mic_buf[USBMICBUF_SIZE];
    uint8_t usb_mic_toggle;
} usbdev_context_t;

usbdev_context_t *usbdev_ct;

bool usbdev_init_flag;


AT(.usbd_init_seg)
void audio_init(uint32_t  AudioFreq, uint32_t Volume, uint32_t options)
{
}

AT(.usbd_init_seg)
void  DeInit(uint32_t options)
{
}

AT(.usbd_seg)
void AudioVolumeCtl(uint16_t unit, uint16_t vol)
{
    if(is_in_update_process())  //when pc send cmd update via usb, we must not play pcm
        return;

    //logi("vol ctrl %x ,%x",unit, vol);
    if(unit == 5){  //mic control
        if (vol >= MIN_LOGIG_VOLUME && vol <=MAX_LOGIG_VOLUME) {
            usbdev_ct->g_logicMicVolume = vol;
            event_put(USB_EVENT_MIC_VOLUME);
        }
    } else if(unit == 2) {  //speaker control
        if (vol >= MIN_LOGIG_VOLUME && vol <=MAX_LOGIG_VOLUME) {
            usbdev_ct->g_logicPcmVolume = vol;
            event_put(USB_EVENT_PCM_VOLUME);
        }
    }
}

AT(.usbd_seg)
void  AudioMuteCtl(uint16_t unit, uint16_t cmd)
{
    if(is_in_update_process())  //when pc send cmd update via usb, we must not play pcm
        return;

    //logi("mute ctrl %x ,%x",unit, cmd);
    if(unit == 5){  //mic control
        usbdev_ct->g_isMicMute = cmd;
    } else if(unit == 2) {  //speaker control
        usbdev_ct->g_isPcmMute = cmd;
    }
}

AT(.usbd_init_seg)
void InitAudio(uint32_t OUT *ptrPcmBuf,uint32_t IN pcmBufLength)
{
    *ptrPcmBuf = (uint32_t)usbdev_ct->isoBuff;
}

#if USB_STEREO_MIC_EN
AT(.usbd_init_seg)
void AudioSystemParamInit(uint32_t* pcmPacketSize, uint32_t* micPacketSize)
{
    *micPacketSize = 192;
}

extern USBD_AudioSystem_Typedef* g_pUsbAudioSystem;

USBD_AudioSystem_Typedef usbAudioSys = {
    AudioSystemParamInit
};
#endif

AT(.usbd_audio_seg)
void OnPcmReceive(uint8_t IN *buf)
{
#if USB_AUDIO_EN
    if(is_in_update_process()) {//when pc send cmd update via usb, we must not play pcm
        return;
    }

    if(!usbdev_ct->g_isPcmMute) {
        usbaudio_playback_write_data(buf, 48);
    }
#endif  //#if USB_AUDIO_EN
}

AT(.usbd_seg)
uint32_t OnPrepareMicData(uint32_t OUT *ptrMicBuf, uint32_t* IN OUT length)
{
#if USB_AUDIO_EN
    int16_t *p16;

    if (usbdev_ct->usb_mic_toggle == 0) {
        p16 = (int16_t*)(usbdev_ct->usb_mic_buf);
    } else {
        p16 = (int16_t*)(usbdev_ct->usb_mic_buf+96);
    }

    if(is_in_update_process())  //when pc send cmd update via usb, we must not play pcm
        return 0;

    usbdev_ct->usb_mic_toggle = usbdev_ct->usb_mic_toggle ? 0 : 1;

    *ptrMicBuf = (uint32_t)p16;
    *length = 96;

#ifdef DEBUG_USB_MIC
    static uint8_t setval;
    memset(p16, setval, 96);
    setval++;
#else
    if (!AUDIO_KARAOKE_MODE || USB_AUDIO_ALL_MODE_SUPPORT) {
        if(usbdev_ct->g_isMicMute) {
            memset(p16,0, 96);
        }
        usbaudio_record_read_data((uint8_t*)p16, 48);
    }
#endif  //DEBUG_USB_MIC

#endif  //#if USB_AUDIO_EN
    return 0;
}


AT(.usbd_init_seg)
void GetAudioCurrentVolumeState(uint16_t IN unit,uint16_t IN volType, uint16_t* OUT curValue)
{
#if USB_AUDIO_EN
    if(volType == COMP_AUDIO_VOLUME_TYPE_VALUE) {
        //logi("get cur Volume");
        *curValue = 0xa000 + np.volume * (MAX_LOGIG_VOLUME - MIN_LOGIG_VOLUME)/VOLUME_MAX ;
    } else if(volType == COMP_AUDIO_MUTE_TYPE_VALUE) {
        //logi("get cur mute");
        if((unit >> 8) == 5) {
            *curValue = usbdev_ct->g_isPcmMute;
        } else if((unit >> 8) == 2) {
            *curValue = usbdev_ct->g_isMicMute;
        }
    }
#endif
}

AT(.usbd_init_seg)
void GetAudioVolumeMin(uint16_t* OUT min)
{
    *min = MIN_LOGIG_VOLUME;
}

AT(.usbd_init_seg)
void GetAudioVolumeMax(uint16_t* OUT max)
{
    *max = MAX_LOGIG_VOLUME;
}

AT(.usbd_init_seg)
void GetAudioVolumeStep(uint16_t* OUT step)
{
    *step = VOLUME_STEP;
}

AT(.mode_usbdev_audio_seg)
void hid_feed_back(int keyType)
{
    if(dev.pc_sta == DEV_OFFLINE)
        return;
    COMP_HID_feed_back(keyType);
}

AT(.mode_usbdev_seg)
void usbd_module_init(void)
{
    if (usbdev_init_flag) {
        return;
    }
    usbdev_init_flag = true;

#if USB_AUDIO_EN
    usbdev_ct->compCallback.InitAudio = InitAudio;
    usbdev_ct->compCallback.OnPrepareMicData = OnPrepareMicData;
    usbdev_ct->compCallback.OnNewPcmReceiveBuff = OnPcmReceive;
    usbdev_ct->compCallback.GetAudioCurrentVolumeState = GetAudioCurrentVolumeState;
    usbdev_ct->compCallback.GetAudioVolumeMin = GetAudioVolumeMin;
    usbdev_ct->compCallback.GetAudioVolumeMax = GetAudioVolumeMax;
    usbdev_ct->compCallback.GetAudioVolumeStep = GetAudioVolumeStep;

    usbdev_ct->USBD_AUDIO_fops_new.Init = audio_init;
    usbdev_ct->USBD_AUDIO_fops_new.DeInit = DeInit;
    usbdev_ct->USBD_AUDIO_fops_new.VolumeCtl = AudioVolumeCtl;
    usbdev_ct->USBD_AUDIO_fops_new.MuteCtl = AudioMuteCtl;

  #if USB_AUDIO_MIC_EN
    USBD_SetPid(0x1000);  //set device PID
    USBD_Microphone_Initialize(&usbdev_ct->USBD_AUDIO_fops_new , &usbdev_ct->compCallback);
    #if USB_STEREO_MIC_EN
        logi("mic stero en");
        USBD_Mic_Stereo_Enable();   //务必调用USBD_Microphone_Initialize函数再设置
        g_pUsbAudioSystem = &usbAudioSys;
        USBD_SetPid(0x5601);
    #endif
  #else //USB_AUDIO_MIC_EN
    USB_UsbAudioInquiry_Init();
  #if USB_MSC_EN
    USBD_Comp_Initialize(&usbdev_ct->USBD_AUDIO_fops_new , &usbdev_ct->compCallback);
    #if !USB_AUDIO_VOLUME_SYNCHRONIZE
        USBD_SetPid(0x6802);  //set device PID
        USBD_Comp_PC_Volume_Synchronize_Disable();
    #else
        USBD_SetPid(0x6801);  //set device PID
    #endif

    #if USB_STEREO_MIC_EN
        logi("comp stero en");
        USBD_Comp_Stereo_Enable();  //务必调用USBD_Comp_Initialize函数再设置
        g_pUsbAudioSystem = &usbAudioSys;
        USBD_SetPid(0x5602);
    #endif

  #else   //USB_MSC_EN
    USBD_Audio_Initialize(&usbdev_ct->USBD_AUDIO_fops_new , &usbdev_ct->compCallback);
    #if !USB_AUDIO_VOLUME_SYNCHRONIZE
        USBD_SetPid(0x4602);  //set device PID
        USBD_Audio_PC_Volume_Synchronize_Disable();
    #else
        USBD_SetPid(0x4601);  //set device PID
    #endif

    #if USB_STEREO_MIC_EN
        logi("audio stero en");
        USBD_Audio_Stereo_Enable(); //务必调用USBD_Audio_Initialize函数再设置
        g_pUsbAudioSystem = &usbAudioSys;
        USBD_SetPid(0x5603);
    #endif

  #endif  //USB_MSC_EN
  #endif  //USB_AUDIO_MIC_EN

#elif USB_MSC_EN    //#if USB_AUDIO_EN
    USBD_SetPid(0x0756);
#if SD_EN
    USBD_CardReader_Init();
#endif
    USBD_Scsi_Init();
#else   //#if USB_AUDIO_EN
    #pragma "macro 'USB_MSC_EN' and 'USB_AUDIO_EN' can not be disabled at once!"
#endif  //#if USB_AUDIO_EN


#if EQ_DEVICE_SEND_TO_PC && USB_MSC_EN
    COMP_USB_EQ_Enable(); //enable before initialization for EQ
/**
    this routine should be place behide initialization of usb module and descriptor
**/
  #if USB_AUDIO_EN
    COMP_USB_Data_Dump_Enable();
  #endif
#endif

    USBD_Start();
}


AT(.mode_usbdev_seg)
void usbd_module_deinit(void)
{
    if (!usbdev_init_flag) {
        return;
    }
    usbdev_init_flag = false;
    USBD_Deinitialize();
}

#if USB_AUDIO_EN
AT(.mode_usbdev_seg)
void usbaudio_init(void)
{
    usbaudio_init_t params;
    params.playback_fs = 48000;
    params.playback_sample_size = sizeof(int16_t);
    params.playback_ch = 2;
    params.playback_samples = BLOCK_SAMPLES;
    params.record_fs = 48000;
    params.record_sample_size = sizeof(int16_t);
    params.record_ch = 1;
    params.record_samples = 48000 / 1000;
    create_usbaudio(&params);
}

AT(.mode_usbdev_seg)
void usbaudio_deinit(void)
{
    destroy_usbaudio();
}
#endif

AT(.mode_usbdev_seg)
void usbdev_event_process(uint32_t event)
{
    //logi("event = 0x%x",event);
    switch(event) {
    case PLAYER_EVENT_TONE_END_OF_FILE:
        if (common_tone_is_playing()) {
            uint32_t trigger_event = get_common_tone_trigger_event();
            common_tone_status_clear();
            switch (trigger_event) {
            default:
                audio_scene_recovery(); //恢复之前的播放
                break;
            }
        }
        break;

#if USB_AUDIO_EN
    case USB_EVENT_PCM_VOLUME:
        volume_set((usbdev_ct->g_logicPcmVolume - MIN_LOGIG_VOLUME)* 100/ (MAX_LOGIG_VOLUME - MIN_LOGIG_VOLUME));
        break;

    case USB_EVENT_MIC_VOLUME:           //set mic volume here
        break;
#endif

    case EQ_EVENT_SET_MODE_EQ:
        scene_mode.sta = MODE_MUSIC;
        break;
#if !EQ_DEVICE_SEND_TO_PC && USB_AUDIO_EN //not support in eq data dump mode
    case KEY_EVENT_SU | KEY_VOL_DOWN:   //volume down
    case KEY_EVENT_H | KEY_VOL_DOWN:
        logi("dev vol down");
        hid_feed_back(HID_VOL_DOWN);
        break;

    case KEY_EVENT_SU | KEY_VOL_UP:     //volume up
    case KEY_EVENT_H | KEY_VOL_UP:
        logi("dev vol up");
        hid_feed_back(HID_VOL_UP);
        break;

    case KEY_EVENT_SU | KEY_PLAY:       //play,pause
    case KEY_EVENT_SU | KEY_PLAY_MODE:
        logi("dev P/P");
        hid_feed_back(HID_PLAY_PAUSE);
        break;

    case KEY_EVENT_SU | KEY_PREV:       //previous file
        logi("dev vol pre");
        hid_feed_back(HID_PRE);
        break;

    case KEY_EVENT_SU | KEY_NEXT:       //next file
        logi("dev vol next");
        hid_feed_back(HID_NEXT);
        break;
#endif

#if USB_MSC_EN && SD_EN
    case DEVICE_EVENT_SD_IN:
        logi("audio sd in");
        if(device_initialize(DEV_SDCARD) == 0) {
            sd_detect_set(0);
            device_set_num(DEV_SDCARD);
            xfs_fscache_sd_init();
            xfs_cmd(XFS_CMD_FSCACHE_ENABLE,0);
            if(USBD_EnableCardReader(TRUE))
                logi("sd detect out");
        }
        break;

    case DEVICE_EVENT_SD_OUT:
        logi("audio sd out");
        xfs_cmd(XFS_CMD_FSCACHE_DEINIT,XFS_DISK_SDMMC);
        USBD_EnableCardReader(FALSE);
        break;
#endif

    default:
        common_event_process(event);
        break;
    }
}

AT(.mode_usbdev_seg)
void mode_usbdev(void)
{
    logi("usbdev mode.");
    mode_usbdev_enter();
    mode_usbdev_loop();
    mode_usbdev_exit();
}

void usbdev_all_mode_enter(void)
{
    bool usb_is_insert = true;
#if USB_AUDIO_EN
    if (NULL == usbdev_ct) {
        usbdev_ct = libc_calloc(sizeof(*usbdev_ct));
        if (NULL == usbdev_ct) {
            loge("calloc usbdev context fail");
        }
    }
#endif
    if(dev.udisk_sta != DEV_OFFLINE) {
        usb_is_insert = false;
    } else if (dev.pc_sta == DEV_OFFLINE) {
        usb_is_insert = false;
    }

    scene_mode.exit_flag = false;

    if (usb_is_insert) {
#if USB_EN
        usb_detect_set(USB_WORKING);
#endif
#if TWI2_DM_SHARE_EN
        usb_clk_init();
#endif
        usbd_module_init();
    }
    xfs_cmd(XFS_CMD_FSCACHE_ENABLE, 0);
    event_clear(DEVICE_EVENT_PC_OUT);

}

void usbdev_all_mode_exit(void)
{
    if (scene_mode.exit_flag) {
        return;
    }
    logi("usbdev exit 1");

    usb_det.sof_check_en = false;
    usbd_module_deinit();
#if TWI2_DM_SHARE_EN
    usb_host_deinit();
    usb_clk_deinit();
#endif
#if USB_AUDIO_EN
    if (usbdev_ct) {
        libc_free(usbdev_ct);
        usbdev_ct = NULL;
    }
#endif

    usb_detect_set(USB_DETECT);
#if ((EQ_ALL_MODE_SUPPORT == 0) && (USB_AUDIO_ALL_MODE_SUPPORT == 0) && USB_MSC_EN)
#if SD_EN
    USBD_EnableCardReader(FALSE);
#endif
    sd_detect_set(1);
    device_set_num(DEV_NONE);

#endif
    logi("usbdev exit 2");

    if(dev.sdcard_sta == DEV_OK){
        dev.sdcard_sta = DEV_UNINITIALIZED;
    }
}

void usb_dev_enter(void)
{
#if (MODE_USBDEV_EN || EQ_ALL_MODE_SUPPORT || USB_AUDIO_ALL_MODE_SUPPORT)
  #if USB_AUDIO_ALL_MODE_SUPPORT
    usbdev_all_mode_enter();
  #else
    if (device_get_sta(DEV_PC) != DEV_OFFLINE) {
        usb_det.sof_check_en = true;
        pin_set_pull(USB_DM_PIN_NUM, USB_DM_PULL_DISABLE);
        pin_set_func(USB_DM_PIN_NUM, USB_DM_FUNC_WORKING);
        usb_detect_set(USB_WORKING);
    #if USB_MSC_EN && USB_AUDIO_EN
        usbdev_init_flag = true;
      #if EQ_DEVICE_SEND_TO_PC
        USBD_EQ_device_Init(TRUE);
      #else
        USBD_EQ_device_Init(FALSE);
      #endif
    #endif
    } else {
        if(device_get_sta(DEV_UDISK) == DEV_OFFLINE) {
            usb_detect_set(USB_DETECT);
        }
    }
  #endif
#endif
}

void usb_dev_exit(void)
{
#if (MODE_USBDEV_EN || EQ_ALL_MODE_SUPPORT || USB_AUDIO_ALL_MODE_SUPPORT)
  #if USB_AUDIO_ALL_MODE_SUPPORT
    usbdev_all_mode_exit();
  #else
    if(device_get_sta(DEV_PC) != DEV_OFFLINE) {
        usbd_module_deinit();
        usb_detect_set(USB_DETECT);
    }
  #endif
#endif
}

void usbdev_init(void)
{
    module_ccu_disable(CCU_USB);
    mdelay(5);
    module_ccu_enable(CCU_USB);

#if USB_AUDIO_EN
    if (NULL == usbdev_ct) {
        usbdev_ct = libc_calloc(sizeof(*usbdev_ct));
    }
    if (NULL == usbdev_ct) {
        loge("calloc usbdev context fail");
    }
#endif

#if TWI2_DM_SHARE_EN
    usb_clk_init();
#endif

    if (EQ_ALL_MODE_SUPPORT && !USB_AUDIO_ALL_MODE_SUPPORT) {
#if USB_MSC_EN && USB_AUDIO_EN
        usbdev_init_flag = true;
#if EQ_DEVICE_SEND_TO_PC
        USBD_EQ_device_Init(TRUE);
#else
        USBD_EQ_device_Init(FALSE);
#endif
#endif
    } else {
        usbd_module_init();
    }
}

void usbdev_deinit(void)
{
    usbd_module_deinit();

#if TWI2_DM_SHARE_EN
    usb_host_deinit();
    usb_clk_deinit();
#endif

#if USB_AUDIO_EN
    if (usbdev_ct != NULL) {
        libc_free(usbdev_ct);
        usbdev_ct = NULL;
    }
#endif
}

AT(.mode_usbdev_seg)
void mode_usbdev_enter(void)
{
    if(dev.udisk_sta != DEV_OFFLINE) {
        scene_mode.exit_flag = true;
        mode_switch_auto();
        return;
    } else if (dev.pc_sta == DEV_OFFLINE) {
        scene_mode.exit_flag = true;
        mode_switch_auto();
        logi("pc not online");
        return ;
    }

    sleep_prevent_set(PS_APP_PROCESS);

    scene_mode.exit_flag = false;
    usb_detect_set(USB_WORKING);

    disp_menu(MENU_SCENE_MAIN, scene_mode.sta, 0);
#if (TONE_EN && TONE_PC_MODE_EN)
    play_tone(TONE_PC_MODE, true);
#endif

#if USB_AUDIO_EN
    if (NULL == usbdev_ct) {
        usbdev_ct = libc_calloc(sizeof(*usbdev_ct));
    }
    if (NULL == usbdev_ct) {
        loge("calloc usbdev context fail");
    }
#endif

#if TWI2_DM_SHARE_EN
    usb_clk_init();
#endif

    usbd_module_init();
    player_mode_switch_proc(MODE_USBDEV, true); /* 初始化USB audio播放/录音功能 */
#if ((EQ_ALL_MODE_SUPPORT == 0) && (USB_AUDIO_ALL_MODE_SUPPORT == 0))
    if(dev.sdcard_sta != DEV_OFFLINE) {
        event_put(DEVICE_EVENT_SD_IN);
    }
#endif
    xfs_cmd(XFS_CMD_FSCACHE_ENABLE, 0);

    auto_shutdown(false);
	send_simple_status();
}

AT(.mode_usbdev_seg)
void mode_usbdev_loop(void)
{
    uint32_t event;
    while(scene_mode.sta == MODE_USBDEV) {
        event = event_get(true);
        usbdev_event_process(event);
    }
}

AT(.mode_usbdev_seg)
void mode_usbdev_exit(void)
{
    if (scene_mode.exit_flag) {
        return;
    }

    scene_mode.last_sta = MODE_USBDEV;

#if SD_EN
    if (device_is_online(DEV_SDCARD)) {
        if(device_is_initialized(DEV_SDCARD)){
            logi("usb sd stop");
            sd_stop();
        }
    }
#endif

    player_mode_switch_proc(MODE_USBDEV, false); /* 退出USB audio播放/录音功能 */
    usb_det.sof_check_en = false;
    usbd_module_deinit();
#if TWI2_DM_SHARE_EN
    usb_host_deinit();
    usb_clk_deinit();
#endif

#if USB_AUDIO_EN
    libc_free(usbdev_ct);
    usbdev_ct = NULL;
#endif

    usb_detect_set(USB_DETECT);
#if ((EQ_ALL_MODE_SUPPORT == 0) && (USB_AUDIO_ALL_MODE_SUPPORT == 0) && USB_MSC_EN)
#if SD_EN
    USBD_EnableCardReader(FALSE);
#endif
    sd_detect_set(1);
    device_set_num(DEV_NONE);
#endif
    if(dev.sdcard_sta == DEV_OK){
        dev.sdcard_sta = DEV_UNINITIALIZED;
    }

    sleep_prevent_clear(PS_APP_PROCESS);

	auto_shutdown(true);
	send_simple_status();
}

#endif

