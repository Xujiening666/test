#define LOG_LEV 5
#include "sys_inc.h"
#include "user_config.h"
#include "diskio.h"
#include "sdmmc_io.h"
#include "usb.h"
#include "sd.h"
#include "device.h"
#include "queue.h"
#include "event.h"
#include "task_main.h"
#include "libc_common.h"


device_t dev;
TimerHandle_t DeviceDetectTimerHandle;


AT(.device_seg)
void device_init(void)
{
    dev.cur_dev = DEV_NONE;

    dev.sdcard_sta = DEV_OFFLINE;
#if SD_EN
#if (SD_DETECT_EN == 0)
    dev.sdcard_sta = DEV_UNINITIALIZED;
    event_put(DEVICE_EVENT_SD_IN);
#endif
    sd_init();
#endif

    dev.udisk_sta = DEV_OFFLINE;
#if USB_EN
    usb_init();
#if (SYS_PLATFORM == PLATFORM_FPGA)
    usb_host_init();
#endif
#endif

#if LINEIN_DETECT_EN
    linein_detect_init();
#endif

#if (SD_DETECT_EN || USB_DETECT_EN || LINEIN_DETECT_EN)
    if (NULL == DeviceDetectTimerHandle) {
        DeviceDetectTimerHandle = xTimerCreate("DeviceDetectTimer", pdMS_TO_TICKS(20), pdTRUE, (void *)1, device_detect);
    }
    if (DeviceDetectTimerHandle != NULL) {
        xTimerStart(DeviceDetectTimerHandle, 10);
    }
#endif
}

AT(.device_sram_seg)
void device_detect(xTimerHandle xTimer)
{
	uint8_t private_flag;
    info_get(INFO_PRIVATE_MODE_FLAG, (void *)&private_flag, 1);
#if SD_DETECT_EN && SD_EN
    sd_detect();
#endif

#if USB_DETECT_EN && USB_EN
    if(private_flag != 1) //非隐私模式
    {
		usb_detect();
	}
#endif

#if LINEIN_DETECT_EN
    linein_detect();
#endif
}

AT(.device_seg)
int device_initialize(uint32_t dev_num)
{
    logi("device_initialize: %d", dev_num);

#if SD_EN
    if (dev_num == DEV_SDCARD) {
        sd_detect_set(0);
#if SD_DM_SHARE_EN
#if USB_DISK_EN
        usb_host_deinit();
        usb_clk_deinit();
#endif
        sd_clk_init();
        sd_pin_init();
#endif
#if SD_DM_SHARE_EN && USB_EN
        if (device_is_online(DEV_UDISK)) {
            usb_detect_set(USB_DETECT);     //enable udisk out detect
        }
#endif

        for(uint8_t i=0;i<10;i++) {
            if (sd_disk_initialize() == 0) {
                dev.sdcard_sta = DEV_OK;
                logi("sd init ok.");
                sd_detect_set(1);
                return 0;
            } else {
                msleep(30);
            }
        }

        dev.sdcard_sta = DEV_FAIL;
        logi("sd init error.");
        sd_detect_set(1);
        return -1;
    }
#endif

#if USB_EN && USB_DISK_EN
    if (dev_num == DEV_UDISK) {
#if SD_DM_SHARE_EN
        sd_clk_deinit();
#endif
#if (SD_DM_SHARE_EN || TWI2_DM_SHARE_EN)
        usb_clk_init();
        usb_pin_init();
#endif
        usb_detect_set(USB_WORKING);
        usb_clk_init();
        usb_host_deinit();
        usb_host_init();
        if (udisk_initialize() == 0) {
            dev.udisk_sta = DEV_OK;
            logi("udisk init ok.");
            return 0;
        } else {
            dev.udisk_sta = DEV_FAIL;
            usb_host_deinit();
#if TWI2_DM_SHARE_EN
            usb_clk_deinit();
#endif
            usb_detect_set(USB_DETECT);
            return -1;
        }
    }
#endif  //USB_EN & ...

    return -1;
}

AT(.device_seg)
bool device_is_initialized(uint32_t dev_num)
{
    uint32_t dev_sta;
    dev_sta = device_get_sta(dev_num);
    if (dev_sta == DEV_OK || dev_sta == DEV_FS_FAIL || dev_sta == DEV_FAIL) {
        return true;
    }

    return false;
}

AT(.device_sram_seg)
bool device_is_online(uint32_t dev_num)
{
    uint32_t dev_sta;
    dev_sta = device_get_sta(dev_num);
    if (dev_sta == DEV_UNINITIALIZED || dev_sta == DEV_OK) {
        return true;
    }
    return false;
}

AT(.device_sram_seg)
uint32_t device_get_sta(uint32_t dev_num)
{
    uint32_t sta;
    sta = DEV_FAIL;

    if (dev_num == DEV_SDCARD) {
#if SD_EN
        sta = dev.sdcard_sta;
#else
        sta = DEV_OFFLINE;
#endif
    } else if (dev_num == DEV_UDISK) {
#if USB_EN
        sta = dev.udisk_sta;
#else
        sta = DEV_OFFLINE;
#endif
    } else if (dev_num == DEV_PC) {
#if USB_EN
        sta = dev.pc_sta;
#else
        sta = DEV_OFFLINE;
#endif
    }
    return sta;
}

AT(.device_seg)
void device_set_sta(uint32_t dev_num, uint32_t dev_sta)
{
    if (dev_num == DEV_SDCARD) {
        dev.sdcard_sta = dev_sta;
    } else if (dev_num == DEV_UDISK) {
        dev.udisk_sta = dev_sta;
    } else if(dev_num == DEV_PC) {
        dev.pc_sta = dev_sta;
    }
}

AT(.device_seg)
uint32_t device_get_num(void)
{
    return dev.cur_dev;
}

AT(.device_seg)
uint32_t device_get_next_num(void)
{
    return (dev.cur_dev == DEV_SDCARD) ? DEV_UDISK : DEV_SDCARD;
}

AT(.device_seg)
void device_set_num(uint32_t dev_num)
{
    dev.cur_dev = dev_num;
    dev.mounted = 0;
    logi("dev sel[%d]", dev_num);
}

AT(.device_seg)
int device_try(uint32_t dev_num)
{
//sd
#if SD_DM_SHARE_EN
    if ((DEV_SDCARD == dev_num) && (device_get_sta(DEV_SDCARD) != DEV_OFFLINE)) {
        device_set_sta(DEV_SDCARD, DEV_UNINITIALIZED);
    }
#endif
//usb
#if (SD_DM_SHARE_EN || TWI2_DM_SHARE_EN)
    if ((DEV_UDISK == dev_num) && (device_get_sta(DEV_UDISK) != DEV_OFFLINE)) {
        device_set_sta(DEV_UDISK, DEV_UNINITIALIZED);
    }
#endif

    if (device_is_online(dev_num)) {
        if (!device_is_initialized(dev_num)) {
            if (device_initialize(dev_num) < 0) {
                logd("init dev(%d) fail", dev_num);
                device_set_sta(dev_num, DEV_FAIL);
                return -1;
            }
        } else if ((DEV_UDISK == dev_num) && USB_DP_FUNC_DETECT == pin_get_func(USB_DP_PIN_NUM)) {
            logi("force dp to working");
            pin_set_func(USB_DP_PIN_NUM, USB_DM_FUNC_WORKING);
        }

        device_set_num(dev_num);
        return 0;
    } else {
        return -2;
    }
}


