#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "update.h"
#include "codec.h"
#include "queue.h"
#include "event.h"
#include "device.h"
#include "Xfs.h"

extern TimerHandle_t DeviceDetectTimerHandle;
extern void restart(void);
extern void update_setvolume(uint32_t volume);

AT(.update_init)
bool update_start(uint32_t cur_dev)
{
    //todo : add you code here ,


    //if you want to return 0, do not call the code below
    xfunmount("0:");
    xfs_cmd(XFS_CMD_FSCACHE_DEINIT, 1);

    if(cur_dev == DEV_SDCARD){
#if SD_EN
        xfs_fscache_sd_init();
#endif
    }else if(cur_dev == DEV_USB){
#if USB_DISK_EN
        xfs_fscache_usb_init();
#endif
    }
    return 1;       //return 0, update process will be terminated
}

AT(.update_nor)
void update_end(void)   //this routine must put into com_sys_sec
{
    //the code and rodata below should put into com_sys_sec
    logi("update_end");
    restart_pmu();//restart system
}

AT(.update_nor)
bool begin_update_flash(void)   //this routine must put into com_sys_sec
{
    logi("begin update flash");

#if (MODE_BT_EN && BT_BACKGROUND_EN)
    INTC_DisableIRQ(IRQn_BT);
#endif

    vTaskSuspendAll();

    //we must stop sd detect, otherwise we should move the routine"sd_detect" to COM_SYS section
    xTimerStop(DeviceDetectTimerHandle,0);

    snd_spk_unmute();
    update_setvolume(-30);  //range is from -80 to  30

    return 1;   //return 0, update process will be terminated
}

UpdateCallback g_UpdateCallback = {
    update_start,   //call begin update process
    update_end,     //call when update complete
    begin_update_flash  //call when begin to operate flash
};

AT(.update_init)
void update_init(void)
{
    register_updatecallback(&g_UpdateCallback);
}

