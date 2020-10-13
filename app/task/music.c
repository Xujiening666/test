#define LOG_LEV 4
#define SLTK_LOG_TAG "music"
#include "sys_inc.h"
#include "user_config.h"
#include "task_main.h"
#include "music.h"
#include "key.h"
#include "task.h"
#include "xfs.h"
#include "player.h"
#include "device.h"
#include "sd.h"
#include "sdmmc_io.h"
#include "info_mem.h"
#include "disp.h"
#include "usb.h"
#include "diskio.h"
#include "loadcode.h"
#include "libc_common.h"
#include "device.h"
#include "bt_host_api.h"
#include "timer.h"


#if MODE_MUSIC_EN && !USB_DISK_EN && !SD_EN
    #pragma "Music mode must support at least one storage media, use 'USB_DISK_EN' and 'SD_EN' to enable it "
#endif

FATFS *pfs;

bool HP_OUT_BUT_NOT_START_RECORD = false;

AT(.mode_music_seg)
int file_scan(bool scan)
{
    FRESULT ret;
    uint8_t cur_dev;
    uint32_t checksum;

    cur_dev = device_get_num();

    if (scan) {
        if (dev.mounted == 0) {
            /* Unmount must be called to release memory before the next mount */
            xfunmount("0:");
            if (DEV_SDCARD == cur_dev) {
#if SD_EN
                xfs_fscache_sd_init();
                xfs_cmd(XFS_CMD_FSCACHE_ENABLE, 1);
                xfs_cmd(XFS_CMD_TIMEOUT_SET, 500);     //uint: ms
#endif
            } else if (DEV_UDISK == cur_dev) {
#if USB_DISK_EN
                xfs_fscache_usb_init();
                xfs_cmd(XFS_CMD_FSCACHE_ENABLE, 1);
                xfs_cmd(XFS_CMD_TIMEOUT_SET, 10000);
#endif
            }

            if (NULL == pfs) {
                pfs = (FATFS*)libc_malloc(sizeof(FATFS));
                if (NULL == pfs) {
                    loge("malloc fs fail");
                    return PLAYER_RET_DEV_NOFILE;
                }
            }
            libc_memset(pfs, 0, sizeof(*pfs));
            ret = xfmount(pfs, "0:", 1);

            if (FR_OK != ret) {
                loge("xfmount err(%d)", ret);
                return PLAYER_RET_DEV_NOFILE;
            }

            dev.mounted = 1;
        }

        if (file_manager_init(&np.manager, "") != FMG_OK) {
            loge("opendir err");
            return PLAYER_RET_DEV_NOFILE;
        }

    } else {
        dev.mounted = 1;
        xfs_cmd(XFS_CMD_FSCACHE_ENABLE, 1);
    }

    xfs_cmd(XFS_CMD_GET_FILES_CHECKSUM, (uint32_t)&checksum);
    if (device_get_num() == DEV_SDCARD) {
#if SD_EN
        if (checksum != np.manager.sdcard_file_checksum) {
            np.manager.sdcard_file_checksum = checksum;
            np.manager.sdcard_file_invalid = false;
            np.manager.sdcard_file_fail_count = 0;
        } else {
            if (np.manager.sdcard_file_fail_count ==
                np.manager.sdcard_total_file) {
                np.manager.sdcard_file_invalid = true;
            } else {
                np.manager.sdcard_file_invalid = false;
            }
            if (np.manager.sdcard_file_invalid) {
                np.manager.sdcard_total_file = 0;
                logw("sdcard file invalid, set total file 0");
            }
        }
        np.manager.total_file = np.manager.sdcard_total_file;
#endif
    } else if (device_get_num() == DEV_UDISK) {
#if USB_DISK_EN
        if (checksum != np.manager.udisk_file_checksum) {
            np.manager.udisk_file_checksum = checksum;
            np.manager.udisk_file_invalid = false;
            np.manager.udisk_file_fail_count = 0;
        } else {
            if (np.manager.udisk_file_fail_count ==
                np.manager.udisk_total_file) {
                np.manager.udisk_file_invalid = true;
            } else {
                np.manager.udisk_file_invalid = false;
            }
            if (np.manager.udisk_file_invalid) {
                np.manager.udisk_total_file = 0;
                logw("udisk file invalid, set total file 0");
            }
        }
        np.manager.total_file = np.manager.udisk_total_file;
#endif
    }

    return PLAYER_RET_SUCCESS;
}

AT(.mode_music_seg)
void music_event_process(uint32_t event)
{
    switch(event) {
#if SD_EN
    case DEVICE_EVENT_SD_FAIL:
        logi("music,sd fail");
        sd_detect_set(1);
        break;
#endif

    case PLAYER_EVENT_TONE_END_OF_FILE:
        if (common_tone_is_playing()) {
            uint32_t trigger_event = get_common_tone_trigger_event();
            common_tone_status_clear();

            switch (trigger_event) {
            /* 播放完提示音后播上一曲 */
            case KEY_EVENT_SU | KEY_PREV:       //previous file
            case KEY_EVENT_SU | KEY_PREV_VOLDOWN:
                np.breakpoint_rd = 0; //取消断点播放
                play_switch_file(false);
                break;

            /* 播放完提示音后播下一曲 */
            case KEY_EVENT_SU | KEY_NEXT:       //next file
            case KEY_EVENT_SU | KEY_NEXT_VOLUP:
                np.breakpoint_rd = 0; //取消断点播放
                play_switch_file(true);
                break;

            default:
                audio_scene_recovery(); //恢复之前的播放
                break;
            }
            disp_menu(MENU_NUM, np.file_num, 0);
        }
        break;

    case PLAYER_EVENT_END_OF_FILE:        //play completed
        logi("play completed");
        set_play_direction(PLAY_NEXT);
        auto_play_file(0);
        disp_menu(MENU_NUM, np.file_num, 0);
        break;

    case PLAYER_EVENT_ERR:              //play err
        player_proc_event(PLAYER_EVENT_ERR);
        break;

    case PLAYER_EVENT_PAUSE_COMPLETED: //pause
        if (!np.paused || np.fast) {
            break;
        }
        get_play_time();
        play_time_write();
        disp_menu(MENU_MUSIC_STOP, np.ptime.min, np.ptime.sec);
        break;

    case KEY_EVENT_SU | KEY_MODE:       //mode
        if(mode_get_sta() == MODE_MUSIC) {
            logi("ignore mode key");
            break;
        }
        if (device_get_num() == DEV_UDISK) {
            if (play_new_device(DEV_SDCARD, PLAY_SELECT, 0, true) < 0) {
                if (key_mode_exit) {
                    key_mode_exit = false;
                    mode_update(-1);
                }
                common_event_process(event);
            } else {
                mode_update(-1);
            }
        } else {
            common_event_process(event);
        }
        break;

#if MUSIC_NUM_STEP10
    case KEY_EVENT_SU | KEY_N10_MINUS:  // 10-
        logi("10-");
        if (np.manager.total_file <= 10) {
            break;
        }
        np.n10 = true;
#endif

    case KEY_EVENT_SU | KEY_PREV:       //previous file
    case KEY_EVENT_SU | KEY_PREV_VOLDOWN:
        logi("pre");
#if (TONE_EN && TONE_MUSIC_PREV_EN)
        play_common_tone(TONE_BT_PREV, event);
#else

#if MUSIC_FOLDER_SELECT_EN
        play_switch_folder(false);
#else
        play_switch_file(false);
#endif

#endif
        break;

#if MUSIC_NUM_STEP10
    case KEY_EVENT_SU | KEY_N10_PLUS:   // 10+
        logi("10+");
        if (np.manager.total_file <= 10) {
            break;
        }
        np.n10 = true;
#endif
    case KEY_EVENT_SU | KEY_NEXT:       //next file
    case KEY_EVENT_SU | KEY_NEXT_VOLUP:
        logi("next");
#if (TONE_EN && TONE_MUSIC_NEXT_EN)
        play_common_tone(TONE_BT_NEXT, event);
#else

#if MUSIC_FOLDER_SELECT_EN
        play_switch_folder(true);
#else
        play_switch_file(true);
#endif
#endif
        break;

    case KEY_EVENT_SU | KEY_PLAY:       //play,pause
    case KEY_EVENT_SU | KEY_PLAY_MODE:
        logi("p/p");
#if 0
        np.paused = !np.paused;
        if (np.paused) {
            play_common_tone(TONE_PAUSE, event); //暂停提示音
        } else {
            play_common_tone(TONE_PLAY, event); //继续播放提示音
        }
#else
        play_or_pause();
#endif
        break;

    case KEY_EVENT_H | KEY_PREV:        //fast backward
#if MUSIC_FAST_SKIP_EN
        fast_skip(0);
#endif
        break;

    case KEY_EVENT_H | KEY_NEXT:        //fast forward
#if MUSIC_FAST_SKIP_EN
        fast_skip(1);
#endif
        break;

    case KEY_EVENT_LU | KEY_PREV:       //fast backward end
    case KEY_EVENT_LU | KEY_NEXT:       //fast forward end
        logi("arunboy changan xiayiju");
        fast_end();
        break;

    case KEY_EVENT_LU | KEY_VOL_DOWN:   //volume down
    case KEY_EVENT_H | KEY_VOL_DOWN:
    case KEY_EVENT_H | KEY_PREV_VOLDOWN:
        logi("vol-");
        volume_down();
        disp_menu(MENU_VOLUME, np.volume, 0);
        break;

    case KEY_EVENT_LU | KEY_VOL_UP:     //volume up
    case KEY_EVENT_H | KEY_VOL_UP:
    case KEY_EVENT_H | KEY_NEXT_VOLUP:
        logi("vol+");
        volume_up();
        disp_menu(MENU_VOLUME, np.volume, 0);
        break;

    case KEY_EVENT_SU | KEY_REPEAT:     //repeat switch
    case KEY_EVENT_L | KEY_PLAY:
//        set_repeat();
//        logi("repeat:%x", np.repeat_mode);
         mode_switch_auto();
        break;
/* Modify by arunboy 20200507 */
	case KEY_EVENT_SU | KEY_VOL_UP:
		logi("shagnyiju +");
		play_switch_file(false);
		break;
	case KEY_EVENT_SU | KEY_VOL_DOWN:
		logi("xiayishou -");
		play_switch_file(true);
		break;
		

    case KEY_EVENT_SU | KEY_EQ:         //eq switch
        logi("eq");
        set_eq();
        break;

    case KEY_EVENT_S | KEY_NUM0:        //select file
    case KEY_EVENT_S | KEY_NUM1:
    case KEY_EVENT_S | KEY_NUM2:
    case KEY_EVENT_S | KEY_NUM3:
    case KEY_EVENT_S | KEY_NUM4:
    case KEY_EVENT_S | KEY_NUM5:
    case KEY_EVENT_S | KEY_NUM6:
    case KEY_EVENT_S | KEY_NUM7:
    case KEY_EVENT_S | KEY_NUM8:
    case KEY_EVENT_S | KEY_NUM9:
        knum.val = knum.val * 10 + (event - (KEY_EVENT_S|KEY_NUM0));
        knum.bcnt++;
        if ((knum.val > 9999) || (knum.bcnt > 4)) {
            knum.val = event - (KEY_EVENT_S|KEY_NUM0);
            knum.bcnt = 1;
        }
#if DISP_EN
        disp_menu(MENU_INPUT_NUM, knum.val, 0);
#endif
        break;

#if DISP_EN
    case DISP_EVENT_SWITCH:
        if (disp.menu == MENU_INPUT_NUM) {
            logw("system event :%d", knum.val);
            play_file(knum.val, true);
            disp_menu(MENU_NUM, np.file_num, 0);

            knum.val = 0;
            knum.bcnt = 0;
        } else {
            disp.menu = MENU_SCENE_MAIN;
            get_play_time();
            disp_menu(MENU_MUSIC_PLAY, np.ptime.min, np.ptime.sec);
        }
        break;

    case DISP_EVENT_UPDATE:
        if (!np.fast) {
            get_play_time();
            disp_menu(MENU_MUSIC_PLAY, np.ptime.min, np.ptime.sec);
        }
        break;
#endif

#if USB_DISK_EN
     case USB_EVENT_TEST_UNIT:
        if (DEV_UDISK == device_get_num()) {
            event_clear(USB_EVENT_TEST_UNIT);
            uint8_t status = xfs_is_usb_unit_ready();
            if (status == 0) {
                event_put(DEVICE_EVENT_UDISK_IN);
            } else if(status == 2) {
                event_put(USB_EVENT_TEST_UNIT);
            }
        }
        break;
#endif
/* Modify by arunboy 20200619 */
    case KEY_EVENT_SU | KEY_HP_OUT:   //拔出耳机
        HP_OUT_BUT_NOT_START_RECORD = true;
        player_mode_switch_proc(MODE_MUSIC, false);
		mode_switch_auto();
	    break;

    default:
        common_event_process(event);
        break;
    }
}

AT(.mode_music_seg)
void mode_music(void)
{
    logi("music mode.");

    mode_music_enter();
    mode_music_loop();
    mode_music_exit();
}

AT(.mode_music_seg)
void mode_music_enter(void)
{
    uint32_t device_1st;
    uint32_t device_2nd;

	HP_OUT_BUT_NOT_START_RECORD = false;

    if (!device_is_online(DEV_SDCARD) && !device_is_online(DEV_UDISK)) {
        mode_switch_auto();
        scene_mode.exit_flag = true;
        return;
    }
    scene_mode.exit_flag = false;

#if (TONE_EN && SUNCHIP_MUISC_MODE)
    play_tone(TONE_MUISC_MODE, true);
#endif
    //disp_state(STATE_MUSIC_PLAY);

#if BT_BACKGROUND_EN

#if AUDIO_KARAOKE_MODE
    vTaskPrioritySet(bt_isr_bh_handler, 20);
    vTaskPrioritySet(track_handler, 10);
    vTaskPrioritySet(karaoke_handler, 9);
    vTaskPrioritySet(dec_handler, 8);
    vTaskPrioritySet(btc_handler, 7);
    vTaskPrioritySet(bth_handler, 7);
    vTaskPrioritySet(main_task_handler, 7);
#else
    vTaskPrioritySet(bt_isr_bh_handler, 20);
    vTaskPrioritySet(track_handler, 10);
    vTaskPrioritySet(dec_handler, 8);
    vTaskPrioritySet(btc_handler, 7);
    //vTaskPrioritySet(bta_handler, 7); //bta not exist now
    vTaskPrioritySet(bth_handler, 7);
    vTaskPrioritySet(main_task_handler, 7);
#endif
#endif

    player_mode_switch_proc(MODE_MUSIC, true);

    if (scene_mode.switch_reason == SW_SDCARD) {
        //play sdcard
        device_1st = DEV_SDCARD;
        device_2nd = DEV_UDISK;
    } else if (scene_mode.switch_reason == SW_UDISK) {
        //play udisk
        device_1st = DEV_UDISK;
        device_2nd = DEV_SDCARD;
    } else if (scene_mode.switch_reason == SW_BTBG) {
        //play last device
        device_1st = device_get_num();
        device_2nd = device_get_next_num();
    } else {
        //first try play udisk,if fail,try play sdcard
        device_1st = DEV_UDISK;
        device_2nd = DEV_SDCARD;
    }

    if (play_new_device(device_1st, PLAY_SELECT, 0, true) < 0) {
        if (play_new_device(device_2nd, PLAY_SELECT, 0, true) < 0) {
            device_set_num(DEV_NONE);
            logi("no dev");
            mode_switch_auto();
        }
    }

	if (!sd_check_sn()) {
		device_set_num(DEV_NONE);
		logi("music SN check err");
		mode_switch_auto();
	}
	
    /* Modify by arunboy 20200518 */
	//clk_set_cpux_freq(DVFS_LOCAL_ENCODE_MP3_FREQ,1);
#if (EQ_ALL_MODE_SUPPORT || USB_AUDIO_ALL_MODE_SUPPORT)
    usb_dev_enter();
#endif
#if BAT_EXIST
    poweroff_begin_time = 0xffffffff;
#endif
    auto_shutdown(false);    
}

AT(.mode_music_seg)
void mode_music_loop(void)
{
    uint32_t event;
    while(scene_mode.sta == MODE_MUSIC) {
        event = event_get(true);
        music_event_process(event);
    }
}

AT(.mode_music_seg)
void mode_music_exit(void)
{
    if (scene_mode.exit_flag) {
        return;
    }

    scene_mode.last_sta = MODE_MUSIC;
    file_manager_deinit(&np.manager);

    player_mode_switch_proc(MODE_MUSIC, false);

#if SD_EN
    if (DEV_SDCARD == device_get_num()) {
        uint32_t sdcard_sta = device_get_sta(DEV_SDCARD);
        if (DEV_FS_FAIL == sdcard_sta || DEV_OK == sdcard_sta) {
             sd_stop();
         }
    }
#endif

#if TWI2_DM_SHARE_EN
    uint32_t udisk_sta = device_get_sta(DEV_UDISK);
    if (DEV_FS_FAIL == udisk_sta || DEV_OK == udisk_sta) {
        usb_host_deinit();
        usb_clk_deinit();
    }
#endif

#if (EQ_ALL_MODE_SUPPORT || USB_AUDIO_ALL_MODE_SUPPORT)
    usb_dev_exit();
#endif

    /* Unmount must be called to release memory */
    xfunmount("0:");
    if (pfs) {
        libc_free(pfs);
        pfs = NULL;
    }

    xfs_cmd(XFS_CMD_FSCACHE_DEINIT, 1);
    //clk_set_cpux_freq(DVFS_KARAOKE_FREQ, 1);
    logi("music exit.");

    auto_shutdown(true);
}

