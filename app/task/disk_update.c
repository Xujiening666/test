#define LOG_LEV 4
#define SLTK_LOG_TAG "disk_update"
#include "sys_inc.h"
#include "user_config.h"
#include "task_main.h"
#include "update.h"
#include "update_api.h"


AT(.mode_diskupdate_seg)
void mode_diskupdate(void)
{
#if MODE_DISKUPDATE_EN
    logi("mode disk update");

    sleep_prevent_set(PS_APP_PROCESS);
    update_init();

    uint32_t device_1st;
    uint32_t device_2nd;
    uint32_t switch_reason_temp;

    if (scene_mode.switch_reason == SW_SDCARD) {
        //play sdcard
        device_1st = DEV_SDCARD;
        device_2nd = DEV_NONE;
    } else if (scene_mode.switch_reason == SW_UDISK) {
        //play udisk
        device_1st = DEV_UDISK;
        device_2nd = DEV_NONE;
    } else {
        //first try play udisk,if fail,try play sdcard
        device_1st = DEV_UDISK;
        device_2nd = DEV_SDCARD;
    }

    if (device_try(device_1st) == 0) {
        if (try_update(device_1st) == UPDATE_RET_MOUNT_ERROR) {
            device_set_sta(device_1st, DEV_FS_FAIL);
        }
    } else if(device_try(device_2nd) == 0) {
        if (try_update(device_2nd) == UPDATE_RET_MOUNT_ERROR) {
            device_set_sta(device_2nd, DEV_FS_FAIL);
        }
    } else {
        logi("no device");
    }
    //once enter update process,
    scene_mode.last_sta = MODE_DISKUPDATE;
    switch_reason_temp = scene_mode.switch_reason;
    mode_switch_auto();
    scene_mode.switch_reason = switch_reason_temp;

    sleep_prevent_clear(PS_APP_PROCESS);

    logi("disk update exit");

#endif
}

