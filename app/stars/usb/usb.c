#define LOG_LEV 1
#define SLTK_LOG_TAG "usb"
#include "sys_inc.h"
#include "user_config.h"
#include "event.h"
#include "device.h"
#include "usb.h"
#include "gpio.h"
#include "task_main.h"


TimerHandle_t UsbSofCheckTimerHandle;
TimerHandle_t UsbEdgeChecktimer;

usb_det_t usb_det;


AT(.usb_init_seg)
void usb_init(void)
{
#if USB_EN
    usb_clk_init();

#if (SYS_PLATFORM != PLATFORM_FPGA)
#if !SD_DM_SHARE_EN
    pin_config(USB_DM_PIN_CFG);
#endif
    pin_config(USB_DP_PIN_CFG);

#if USB_DETECT_EN
    usb_det.sta = USB_DETECT_INIT;
    usb_det.edge_check_en = false;
    dev.udisk_sta = DEV_OFFLINE;
    dev.pc_sta = DEV_OFFLINE;
    usb_detect_set(USB_DETECT);
    pin_irq_clk_config(USB_DP_PIN_NUM, (0x80<<8) | (0x00<<7) | (0x01<<4));
    pin_irq_clk_config(USB_DM_PIN_NUM, (0x80<<8) | (0x00<<7) | (0x01<<4));
    //[15:8]-debounce clock cycles; [7]-interrupt clock select; [6:4]-debounce clock pre-scale n;
#endif
#endif

#if ((MODE_USBDEV_EN || EQ_ALL_MODE_SUPPORT || USB_AUDIO_ALL_MODE_SUPPORT) && USB_DETECT_EN)
    if(NULL == UsbSofCheckTimerHandle) {
        UsbSofCheckTimerHandle = xTimerCreate("UsbSofDetectTimer", pdMS_TO_TICKS(100),
            pdTRUE, (void *)1, usb_device_detect_through_sof);
        if(UsbSofCheckTimerHandle != NULL) {
            xTimerStart(UsbSofCheckTimerHandle, 10);
        }
    }
    usb_det.sof_frame_num = -1;
#endif
#endif
}

AT(.usb_init_seg)
void usb_pin_init(void)
{
    pin_config(USB_DM_PIN_CFG);
    pin_config(USB_DP_PIN_CFG);
}

AT(.usb_init_seg)
void usb_clk_init(void)
{
    module_ccu_enable(CCU_USB);
    clk_enable(CLK_USB);
    clk_bypass_usb_suspend_clk(true);
}

AT(.usb_init_seg)
void usb_clk_deinit(void)
{
    clk_disable(CLK_USB);
    module_ccu_disable(CCU_USB);
}

AT(.usb_init_seg)
uint32_t usb_dm_pin_get(void)
{
    pin_set_pull(USB_DM_PIN_NUM, USB_DM_PULL_DETECT);
    pin_set_func(USB_DM_PIN_NUM, USB_DM_FUNC_DETECT);
    msleep(2);
    return pin_get_value(USB_DM_PIN_NUM);
}

AT(.usb_init_seg)
void usb_dp_pin_set(bool detecting)
{
#if (USB_EN && USB_DETECT_EN)
    if (detecting) {
        pin_set_func(USB_DP_PIN_NUM, USB_DP_FUNC_DETECT);
        pin_set_pull(USB_DP_PIN_NUM, USB_DP_PULL_DETECT);
    } else {
        pin_set_func(USB_DP_PIN_NUM, USB_DP_FUNC_WORKING);
        pin_set_pull(USB_DP_PIN_NUM, USB_DP_PULL_DISABLE);
        pin_set_driving(USB_DP_PIN_NUM, USB_DP_DRI);
    }

#endif
}

AT(.usb_init_seg)
void usb_dm_pin_set(bool detecting)
{
#if (USB_EN && USB_DETECT_EN)
#if SD_DM_SHARE_EN
    if (DEV_OFFLINE != device_get_sta(DEV_SDCARD)) {
        return;
    }
#endif

    if (detecting) {
        pin_set_func(USB_DM_PIN_NUM, USB_DM_FUNC_DETECT);
        pin_set_pull(USB_DM_PIN_NUM,USB_DM_PULL_DETECT);
    } else {
        pin_set_func(USB_DM_PIN_NUM, USB_DM_FUNC_WORKING);
        pin_set_pull(USB_DM_PIN_NUM,USB_DM_PULL_DISABLE);
        pin_set_driving(USB_DM_PIN_NUM, USB_DM_DRI);
    }
#endif
}

AT(.usb_init_seg)
void usb_detect_set(enum usb_detect_status_e sta)
{
#if (USB_EN && USB_DETECT_EN)
#if (SYS_PLATFORM != PLATFORM_FPGA)
    if (usb_det.sta == sta) {
        return;
    }
    usb_det.sta = sta;
    usb_det.sof_check_en = false;
    if (sta == USB_DETECT) {
        usb_dp_pin_set(1);
#if (MODE_USBDEV_EN || EQ_ALL_MODE_SUPPORT || USB_AUDIO_ALL_MODE_SUPPORT)
        usb_dm_pin_set(1);
#endif
    } else {
        if (sta == USB_WORKING) {
            usb_dp_pin_set(0);
        }

#if (MODE_USBDEV_EN || EQ_ALL_MODE_SUPPORT || USB_AUDIO_ALL_MODE_SUPPORT)
        usb_dm_pin_set(0);
#endif
    }
#endif
#endif
}

AT(.usb_seg)
void usb_detect(void)
{
    /* 检测U盘 */
#if USB_DISK_EN
    if ((dev.pc_sta == DEV_OFFLINE) && (!usb_det.edge_check_en) && \
        (USB_DP_FUNC_DETECT == pin_get_func(USB_DP_PIN_NUM))) {
        if (pin_get_value(USB_DP_PIN_NUM)) {
            usb_det.pc_in_cnt = 0;
            usb_det.pc_out_cnt = 0;
            usb_det.host_out_cnt = 0;
            if (usb_det.host_in_cnt < USB_HOST_DET_CNT) {
                usb_det.host_in_cnt++;
                if (usb_det.host_in_cnt == USB_HOST_DET_CNT) {
                    usb_det.host_in_cnt = USB_HOST_DET_CNT + 1;
                    if(dev.udisk_sta == DEV_OFFLINE){
                        dev.udisk_sta = DEV_UNINITIALIZED;
                        logi("udisk in.");
                        if (dev.startup) {
                            event_put(DEVICE_EVENT_UDISK_IN);
                        }
                    }
                }
            }

        } else {
            usb_det.host_in_cnt = 0;
            if (usb_det.host_out_cnt < USB_HOST_DET_CNT) {
                usb_det.host_out_cnt++;
                if (usb_det.host_out_cnt == USB_HOST_DET_CNT) {
                    usb_det.host_out_cnt = USB_HOST_DET_CNT + 1;
                    if (dev.udisk_sta != DEV_OFFLINE) {
                        dev.udisk_sta = DEV_OFFLINE;
                        logi("udisk out(gpio).");
                        if (dev.startup) {
                            event_put(DEVICE_EVENT_UDISK_OUT);
                        }
                    }
                }
            }
        }
    } else {
        usb_det.host_in_cnt = 0;
        usb_det.host_out_cnt = 0;
    }
#endif

    /* 检测PC */
#if (MODE_USBDEV_EN || EQ_ALL_MODE_SUPPORT || USB_AUDIO_ALL_MODE_SUPPORT)
    if ((dev.udisk_sta == DEV_OFFLINE)
        && (!usb_det.sof_check_en)         /* 在成功收到sof前 */
        && (USB_DM_FUNC_DETECT == pin_get_func(USB_DM_PIN_NUM))) {
        if (pin_get_value(USB_DM_PIN_NUM)) {
            usb_det.pc_in_cnt = 0;
            if (usb_det.pc_out_cnt < USB_DEVICE_DET_CNT) {
                usb_det.pc_out_cnt++;
                if (usb_det.pc_out_cnt == USB_DEVICE_DET_CNT) {
                    usb_det.pc_out_cnt = USB_DEVICE_DET_CNT + 1;
                    if (dev.pc_sta != DEV_OFFLINE) {
                        dev.pc_sta = DEV_OFFLINE;
                        logi("pc out(gpio).");
                        if (dev.startup) {
                            usb_det.edge_check_en = false;
                            event_clear(DEVICE_EVENT_PC_OUT);
                            event_put(DEVICE_EVENT_PC_OUT);
                        }
                    }
                }
            }
        } else {
            usb_det.host_in_cnt = 0;
            usb_det.host_out_cnt = 0;
            usb_det.pc_out_cnt = 0;
            if (usb_det.pc_in_cnt < USB_DEVICE_DET_CNT) {
                usb_det.pc_in_cnt++;
                if (usb_det.pc_in_cnt == USB_DEVICE_DET_CNT) {
                    usb_det.pc_in_cnt = USB_DEVICE_DET_CNT + 1;
                    if(dev.pc_sta == DEV_OFFLINE) {
                        dev.pc_sta = DEV_UNINITIALIZED;
                        logi("pc in(gpio).");
                        if (dev.startup) {
                            usb_det.edge_check_en = true;
                            event_clear(DEVICE_EVENT_PC_IN);
                            event_put(DEVICE_EVENT_PC_IN);
                        }
                    }
                }
            }
        }
    } else {
        usb_det.pc_in_cnt = 0;
        usb_det.pc_out_cnt = 0;
    }
#endif
}

AT(.usbd_seg)
void usb_device_detect_through_sof(xTimerHandle xTimer)
{
#if (MODE_USBDEV_EN || EQ_ALL_MODE_SUPPORT || USB_AUDIO_ALL_MODE_SUPPORT)
    if (usb_det.sof_check_en) {
        static uint32_t sof_frame_num_last;
        static uint32_t error_count = 0;
        if (usb_det.sof_frame_num == sof_frame_num_last) {
            if(error_count > 3) {
                error_count = 0;
                usb_det.sof_check_en = false;
                if (dev.pc_sta != DEV_OFFLINE) {
                    logi("pc out(sof)");
                    dev.pc_sta = DEV_OFFLINE;
                    event_put(DEVICE_EVENT_PC_OUT);
                }
            } else {
                error_count++;
            }
        } else {
            error_count = 0;
        }

        sof_frame_num_last = usb_det.sof_frame_num;
    }
#endif
}

AT(.usbd_seg)
void usb_device_sof_callback(void)
{
#if (SYS_PLATFORM != PLATFORM_FPGA)
#if (MODE_USBDEV_EN || EQ_ALL_MODE_SUPPORT || USB_AUDIO_ALL_MODE_SUPPORT)
    if (!usb_det.sof_check_en) {
        usb_det.sof_check_en = true;
        usb_detect_set(USB_WORKING);
    }
    usb_det.sof_frame_num = usb_get_sof_frame();
#endif
#endif
}


AT(.usbh_seg)
void usb_host_user_process(void *p, uint8_t id)
{
    switch(id) {
    case HOST_USER_CONNECTION:
#if (SYS_PLATFORM == PLATFORM_FPGA)
        dev.udisk_sta = DEV_UNINITIALIZED;
        if (dev.startup) {
            event_put(DEVICE_EVENT_UDISK_IN);
        }
        logi("udisk in");
#endif
        break;

    case HOST_USER_DISCONNECTION:
        dev.udisk_sta = DEV_OFFLINE;
        event_put(DEVICE_EVENT_UDISK_OUT);
        logi("udisk out");
        break;

    case HOST_USER_CLASS_ACTIVE:
        break;

    case HOST_USER_SELECT_CONFIGURATION:
        break;

    default:
        break;
    }
}

AT(.usbh_seg)
void usb_set_wakeup(bool enable)
{
    if (!module_is_enable(CCU_USB)) {
        return;
    }

#if (SYS_PLATFORM != PLATFORM_FPGA)
    if (enable) {
#if USB_DISK_EN
        usb_suspend();
#endif
        pin_set_func(USB_DP_PIN_NUM, USB_DP_FUNC_WKUP);
        pin_set_pull(USB_DP_PIN_NUM, USB_DP_PULL_WKUP);
#if SD_DM_SHARE_EN
        if (false == device_is_online(DEV_SDCARD))
#endif
        {
            pin_set_func(USB_DM_PIN_NUM, USB_DM_FUNC_WKUP);
            pin_set_pull(USB_DM_PIN_NUM, USB_DM_PULL_WKUP);
        }
        if (device_is_online(DEV_UDISK)) {
            pin_irq_config(USB_DP_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_F_EDGE);
        } else {
            pin_irq_config(USB_DP_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_R_EDGE);
#if (MODE_USBDEV_EN || EQ_ALL_MODE_SUPPORT || USB_AUDIO_ALL_MODE_SUPPORT)
#if SD_DM_SHARE_EN
            if (false == device_is_online(DEV_SDCARD))
#endif
            {
                pin_irq_config(USB_DM_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_F_EDGE);
            }
#endif
        }
        pin_set_wakeup(USB_DP_PIN_NUM, true);
#if (MODE_USBDEV_EN || EQ_ALL_MODE_SUPPORT || USB_AUDIO_ALL_MODE_SUPPORT)
#if SD_DM_SHARE_EN
        if (false == device_is_online(DEV_SDCARD))
#endif
        {
            pin_set_wakeup(USB_DM_PIN_NUM, true);
        }
#endif
    }

    else {
#if SD_DM_SHARE_EN
        if (false == device_is_online(DEV_SDCARD))
#endif
        {
            pin_set_func(USB_DM_PIN_NUM, USB_DM_FUNC_DETECT);
            pin_set_pull(USB_DM_PIN_NUM, USB_DM_PULL_DETECT);
        }

        pin_set_wakeup(USB_DP_PIN_NUM, false);
#if (MODE_USBDEV_EN || EQ_ALL_MODE_SUPPORT || USB_AUDIO_ALL_MODE_SUPPORT)
#if SD_DM_SHARE_EN
        if (false == device_is_online(DEV_SDCARD))
#endif
        {
            pin_set_wakeup(USB_DM_PIN_NUM, false);
        }
#endif
        pin_irq_config(USB_DP_PIN_NUM, PIN_IRQ_DIS);
        pin_irq_config(USB_DM_PIN_NUM, PIN_IRQ_DIS);
#if USB_DISK_EN
        usb_resume();
#endif
        pin_set_func(USB_DP_PIN_NUM, USB_DP_FUNC_DETECT);
    }
#endif
}

AT(.usbh_seg)
DRESULT usb_get_status(void)
{
     if ((dev.udisk_sta == DEV_OFFLINE) || (dev.udisk_sta == DEV_FAIL)) {
        return RES_ERROR;
    }
    return RES_OK;
}

AT(.usbd_msc_seg)
void usb_nodata_loop(void)
{
    logi("dead loop");
    vTaskSuspendAll();
#if WATCHDOG_EN
    watchdog_deinit()
#endif
    while(1);
}

AT(.usbd_init_seg)
void usb_pc_in_proc(void)
{
    usb_pc_connect_detect_start();
    usbdev_init();

    if (NULL == UsbEdgeChecktimer) {
        /* 检查在设定时间内是否有信号翻转 */
        UsbEdgeChecktimer = xTimerCreate("usbedgedet", pdMS_TO_TICKS(10000),
            pdTRUE, (void *)1, usb_device_detect_through_edge);
    }
    if (NULL != UsbEdgeChecktimer) {
        xTimerStart(UsbEdgeChecktimer, 10);
    }
}

AT(.usbd_init_seg)
void usb_pc_out_proc(void)
{
    usbdev_deinit();

    pin_irq_config(USB_DP_PIN_NUM, PIN_IRQ_DIS);
    pin_set_pull(USB_DP_PIN_NUM, USB_DP_PULL_DETECT);
    pin_set_func(USB_DP_PIN_NUM, USB_DP_FUNC_DETECT);
    usb_detect_set(USB_DETECT);

    usb_det.sof_check_en = false;
    usb_det.edge_cnt = 0;

    if (UsbEdgeChecktimer != NULL) {
        xTimerStop(UsbEdgeChecktimer,10);
    }
}

AT(.usbd_init_seg)
void usb_pc_connect_proc(void)
{
#if (!EQ_ALL_MODE_SUPPORT && !USB_AUDIO_ALL_MODE_SUPPORT)
    mode_set(MODE_USBDEV);
#endif
}

AT(.usbd_seg)
void usb_edge_cb(void)
{
    usb_det.edge_cnt++;
	/* Modify by arunboy 20200617 待测试*/
    if (usb_det.edge_check_en && usb_det.edge_cnt >= 2) {   //has usb signal
        pin_irq_config(USB_DP_PIN_NUM, PIN_IRQ_DIS);
        if (UsbEdgeChecktimer != NULL) {
            xTimerStop(UsbEdgeChecktimer,10);
        }

        dev.pc_sta = DEV_OK;
        usb_det.edge_check_en = false;
        event_put(USB_EVENT_PC_CONNECT);
    }
}

AT(.usbd_init_seg)
void usb_device_detect_through_edge(xTimerHandle xTimer)
{
    pin_irq_config(USB_DP_PIN_NUM, PIN_IRQ_DIS);
    if (UsbEdgeChecktimer != NULL) {
        xTimerStop(UsbEdgeChecktimer,10);
    }

    if (usb_det.edge_cnt < 5) {
        event_put(USB_EVENT_PC_CONNECT_TIMEOUT);
    }

    usb_det.edge_cnt = 0;
    usb_det.edge_check_en = false;
}

AT(.usbd_init_seg)
void usb_pc_connect_detect_start(void)
{
    usb_det.edge_cnt = 0;
    gpiob_usb_cb = usb_edge_cb;
    pin_irq_config(USB_DP_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_RF_EDGE);
}

AT(.usbd_init_seg)
bool usb_pc_is_connect(void)
{
    uint8_t count;

    if (device_is_online(DEV_PC)) {
        count = 200;//count = 100;  //100*20ms  = 2s
        usbdev_init();
        usb_det.edge_check_en = true;
        usb_pc_connect_detect_start();
        while(count--) {
            if(device_get_sta(DEV_PC) == DEV_OK) {
                return true;
            }
            mdelay(20);
        }
        
        usbdev_deinit();
    }

    return false;
}

