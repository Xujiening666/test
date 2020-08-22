#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "task_main.h"
#include "disp.h"
#include "bt_host_api.h"
#include "bt.h"
#include "pmu.h"
#include "eq_process.h"
#include "app_afx_config.h"
#include "ble_app.h"

#if EQ_AFX_API_TEST
extern void parse_cmd_cache(void);
#endif
extern uint8_t* get_eq_param_buff_atom();

#if BAT_EXIST
uint32_t poweroff_begin_time = 0xffffffff;
#endif

bool bt_bg_calling = false;
bool music_mode_calling = false;
bool bt_bat_update_enable = false;
uint8_t bat_quantity;
bool bt_con_flag = false;
bool powerkey_us_flag = false;
bool need_play_dis_tone = false;

extern bool poweroff_prepare;
extern bool HP_OUT_BUT_NOT_START_RECORD;

AT(.mode_com_seg)
MODULE_FUNC void on_unload_ex()
{

}

AT(.mode_com_seg)
void common_event_process(uint32_t event)
{
    __maybe_unused int ret;
    __maybe_unused bool fs_scan;
    __maybe_unused bool bt_scan;
    __maybe_unused bool bt_link;

    switch(event) {
    case PLAYER_EVENT_TONE_END_OF_FILE:
        if (np.common_tone_status == COMMON_TONE_STATUS_PLAYING) {
            np.common_tone_status = COMMON_TONE_STATUS_IDLE;
        }
        break;

    case SYSTEM_EVENT_MODE_CHANGE:
        break;

    case SYSTEM_EVENT_SECOND:
#if BAT_EXIST
#if RTC_EN
        pmu_timer_cb(NULL);
#endif
#endif
        break;

#if MODE_BT_EN

#if BT_BACKGROUND_EN
    case BT_EVENT_DISCONNECT:
        if(!bt_con_flag)
            break;

        logi("bt dis");
        music_mode_calling = false;
        bt_bat_update_enable = false;
#if BT_PHONE_EN
        bt_incoming_exit(false);
#endif
        disp_state(STATE_BT_DIS);
        bt_con_flag = false;
        bt_audio_server_stop();
        break;

    case BT_EVENT_LOSTCONNECT:
        break;

    case BT_EVENT_CONNECTED:
        logi("bt con");
#if (TONE_EN && TONE_DISCONNECTED_EN && BT_BACKGROUND_EN)
        audio_scene_save();
        play_tone(TONE_CONNECTED, true);
        audio_scene_recovery(); //恢复之前的播放
#endif
        if (MODE_BT == scene_mode.sta) {
            bt_audio_server_stop();
            bt_audio_server_start();
        }
        disp_state(STATE_BT_CON);
        bt_info_update();
        bt_con_flag = true;
        need_play_dis_tone = true;

        break;

    case BT_EVENT_INCOMING:
    case BT_EVENT_OUTGOING:
        if (bt_bg_calling == false) {
            if ((bt_mode.status == BT_STATUS_INCOMING) ||
                (bt_mode.status == BT_STATUS_OUTGOING)) {
                if (scene_mode.sta != MODE_BT) {
                    mode_set_force(MODE_BT);
                    scene_mode.switch_reason = SW_BTBG;
                    event_put(SYSTEM_EVENT_MODE_CHANGE);
                    event_put(event);
                    bt_bg_calling = true;
                }
            }
        }
        break;

    case BT_EVENT_CALLING:
        music_mode_calling = true;
        break;

    case BT_EVENT_CALLING_END:
        music_mode_calling = false;
        break;
#endif

    case BT_EVENT_SAVE_LINKKEY:
#if (TONE_EN && TONE_DISCONNECTED_EN && BT_BACKGROUND_EN)
        audio_scene_save();
        if(need_play_dis_tone){
            play_tone(TONE_DISCONNECTED, true);
            need_play_dis_tone = false;
        }
#endif
        if (bt_info_need_save()) {
            audio_serivce_fadeout_dac_aout(100);
            bt_info_save();    //save linkkey when disconnect
            audio_serivce_fadein_dac_aout(100);
        }
#if !BT_EWS_EN
        if (bt_init_complete) {
            bt_get_scan_type(&bt_scan, &bt_link);
            bt_set_scan(bt_scan, bt_link);
        }
#endif
#if (TONE_EN && TONE_DISCONNECTED_EN && BT_BACKGROUND_EN)
        audio_scene_recovery(); //恢复之前的播放
#endif
        break;

#endif

#if BAT_EXIST
    case BT_EVENT_BAT_EN:
        bat_quantity = pmu_get_bat_quantity_percent() / 10;
        //some phone can't support the 10, only support 0~9
        if (bat_quantity == 10) {
            bat_quantity--;
        }
#if MODE_BT_EN
        //send 2 times, 1 time vivo X20 can't receive successful
        hsp_hfp_sent_battery_to_phone(bat_quantity);
        hsp_hfp_sent_battery_to_phone(bat_quantity);
#endif
        bt_bat_update_enable = true;
        break;

    case BT_EVENT_BAT_UP:
        if (bt_bat_update_enable == true) {
            bat_quantity = pmu_get_bat_quantity_percent() / 10;
            //some phone can't support the 10, only support 0~9
            if (bat_quantity == 10)
                bat_quantity--;
#if MODE_BT_EN
            hsp_hfp_sent_battery_to_phone(bat_quantity);
#endif
        }
        break;
#endif

    case KEY_EVENT_SU | KEY_MODE:
        printf_debug("%s,%d\n",__func__,__LINE__);
        mode_switch();
        scene_mode.switch_reason = SW_KEY;
        break;

#if BT_BLE_EN
    case BT_EVENT_LE_CONNECTED:
        logi("ble con");
        update_ble_con_status(true);
        break;

    case BT_EVENT_LE_DISCONNECTED:
        logi("ble dis");
        update_ble_con_status(false);
        break;
#endif
    /* Modify by arunboy 20200611 */
#if 0
#if SD_EN
    case DEVICE_EVENT_SD_IN:
        if (scene_mode.sta == MODE_MUSIC) {
#if MODE_MUSIC_EN
            ret = play_new_device(DEV_SDCARD, PLAY_SELECT, 0, true);
            if (ret < 0) {
                if (ret == PLAYER_RET_DEV_OFFLINE) {
                    fs_scan = false;
                } else {
                    fs_scan = true;
                }
                play_new_device(DEV_UDISK, PLAY_SELECT, 0, fs_scan);
            }
#endif
        } else if (scene_mode.sta != MODE_USBDEV) {
#if MODE_DISKUPDATE_EN
            mode_set(MODE_DISKUPDATE);
#else
            mode_set(MODE_MUSIC);
#endif
            scene_mode.switch_reason = SW_SDCARD;
        }
        break;

    case DEVICE_EVENT_SD_OUT:
#if MODE_MUSIC_EN
        if ((MODE_MUSIC== scene_mode.sta)
            && (AP_STATUS_PLAYING != player_get_status())
            && (DEV_SDCARD == device_get_num())) {
            if (play_new_device(DEV_UDISK, PLAY_SELECT, 0, true) < 0) {
                device_set_num(DEV_NONE);
                mode_switch_auto();
            }
        }
#endif
        break;

    case DEVICE_EVENT_SD_FAIL:
        logi("common,sd fail");
        sd_detect_set(1);
        break;
#endif
#endif
#if 0
#if USB_DISK_EN
    case DEVICE_EVENT_UDISK_IN:
        if (scene_mode.sta == MODE_MUSIC) {
#if MODE_MUSIC_EN
            bool sd_sta = (DEV_SDCARD == device_get_num()) ? true : false;
            ret = play_new_device(DEV_UDISK, PLAY_SELECT, sd_sta, true);
            if (ret < 0) {
                if (ret == PLAYER_RET_DEV_OFFLINE) {
                    fs_scan = false;
                } else {
                    fs_scan = true;
                }
                play_new_device(DEV_SDCARD, PLAY_SELECT, 0, fs_scan);
            }
#endif
        } else {
#if MODE_DISKUPDATE_EN
            mode_set(MODE_DISKUPDATE);
#else
            mode_set(MODE_MUSIC);
#endif
            scene_mode.switch_reason = SW_UDISK;
        }
        break;

    case DEVICE_EVENT_UDISK_OUT:
        if (DEV_OFFLINE == device_get_sta(DEV_UDISK) ||
            DEV_FAIL == device_get_sta(DEV_UDISK)) {
            usb_host_deinit();
            usb_detect_set(USB_DETECT);
#if (SYS_PLATFORM == PLATFORM_FPGA)
            usb_host_init();
#endif
        }
#if MODE_MUSIC_EN
        if ((MODE_MUSIC== scene_mode.sta)
            && (AP_STATUS_PLAYING != player_get_status())
            && (DEV_UDISK == device_get_num())) {
            if (play_new_device(DEV_SDCARD, PLAY_SELECT, 0, true) < 0) {
                device_set_num(DEV_NONE);
                mode_switch_auto();
            }
        }
#endif
        break;
#endif
#endif
#if LINEIN_DETECT_EN
    case DEVICE_EVENT_LINE_IN:
        mode_set(MODE_LINEIN);
        break;
#endif

#if (MODE_USBDEV_EN || EQ_ALL_MODE_SUPPORT || USB_AUDIO_ALL_MODE_SUPPORT)
    case DEVICE_EVENT_PC_IN:
        //logi("event pc in");
        usb_pc_in_proc();
        break;

    case DEVICE_EVENT_PC_OUT:
        //logi("event pc out");
        if (MODE_USBDEV == scene_mode.sta) {
			HP_OUT_BUT_NOT_START_RECORD = true;
            mode_switch_auto();
        } else {
            usb_pc_out_proc();
        }
        break;

    case USB_EVENT_PC_CONNECT:
        logi("event pc connect");
        usb_pc_connect_proc();
        break;

    case USB_EVENT_PC_CONNECT_TIMEOUT:
        logi("event pc connect timeout");
        usb_pc_out_proc();
        break;
#endif

    case PMU_EVENT_INT:
		logi("PMU_EVENT_INT");
        pmu_handler();
        break;

#if BAT_EXIST
    case PMU_EVENT_CHARGE_IN:
		logi("PMU_EVENT_CHARGE_IN");
#if PMU_VBUSIN_RESTART
        scene_mode.switch_reason = SW_VBUSIN;
        mode_set_force(MODE_POWEROFF);
#else
        pmu_charge_enable(true);
        disp_state(STATE_CHARGEING);
#endif
        break;

    case PMU_EVENT_CHARGE_OUT:
		logi("PMU_EVENT_CHARGE_OUT");
        pmu_charge_enable(false);
        disp_state(STATE_UNCHARGEING);
        break;

    case PMU_EVENT_BAT_LOW_WRN:
        logi("lb wrn");
        disp_state(STATE_BAT_LOW);
        disp_bat_low_digital(true);
#if TONE_LOW_POWER_EN
        audio_scene_save();
        play_tone(TONE_LOW_POWER, true);
        audio_scene_recovery(); //恢复之前的播放
#else
        //audio_service_play_sin_tone(SIN_TONE_BAT_LOW);
        play_tone(TONE_LOW_POWER, true);
#endif
        break;

    case PMU_EVENT_BAT_UNLOW:
		logi("PMU_EVENT_BAT_UNLOW");
        disp_state(STATE_BAT_UNLOW);
        break;

    case PMU_EVENT_BAT_FULL:
		logi("PMU_EVENT_BAT_FULL");
        disp_state(STATE_BAT_FULL);
        break;
#endif

    case PMU_EVENT_POWERKEY_US:
        powerkey_us_flag = true;
#if KEY_PMU_MULTFUC_EN
        pmu_key_process(KEY_PRESS);
#endif
        break;

    case PMU_EVENT_POWERKEY_S:
#if KEY_PMU_MULTFUC_EN
        pmu_key_process(KEY_RELEASE);
#else
        pmu_key_process();
#endif
        break;

    case KEY_EVENT_L | KEY_POWER:
    case PMU_EVENT_POWERKEY_L:
        if (powerkey_us_flag) {
            /*for avoid enter to poweroff immediately after poweron when long press powerkey.*/
            powerkey_us_flag = false;
            scene_mode.switch_reason = SW_KEY;
            mode_set_force(MODE_POWEROFF);
        }
        break;

    case PMU_EVENT_HSW_OFF:
        scene_mode.switch_reason = SW_HSW;
        mode_set_force(MODE_POWEROFF);
        break;

    case SYSTEM_EVENT_POWEROFF:
        mode_set_force(MODE_POWEROFF);
        break;

    case PMU_EVENT_HSW_ON:
        break;

#if (RTC_ALARM0_EN || RTC_ALARM1_EN)
    case RTC_EVENT_ALM0:
    case RTC_EVENT_ALM1:
        logi("alarm.");
        clock_mode.sub_status = CLOCK_ALARM_PENDING_MODE;
        if (scene_mode.sta == MODE_CLOCK) {
            clock_alarm_play_tone();
        } else {
            mode_set(MODE_CLOCK);
        }
        break;
#endif

#if USB_EN && USB_MSC_EN
     case SYSTEM_EVENT_UPDATE_STANBY:
        usb_nodata_loop();
        break;
#endif

    case EQ_EVENT_SET_EQ_DATA_ISR:
    {
        uint8_t *eq_buf = get_eq_param_buff_atom();
        uint32_t length = *((uint32_t*)(&eq_buf[0]));
        on_set_eq_data(&eq_buf[4], length);
        break;
    }

    case EQ_EVENT_UPLOAD_RAM_EQ:
    {
#if MODE_USBDEV_EN && USB_AUDIO_EN && USB_MSC_EN
        do
        {
            on_unload_for_page_id(PAGE_MUSIC);
            on_unload_for_page_id(PAGE_VSS);
            on_unload_for_page_id(PAGE_OUTPUT1);
            on_unload_for_page_id(PAGE_OUTPUT2);
            on_unload_for_page_id(PAGE_MIC);
            on_unload_for_page_id(PAGE_EFFECT);
            on_unload_ex();
        }while(0);
        break;
#endif
    }
#if EQ_AFX_API_TEST
    case EQ_EVENT_AFX_TEST:
    {
        parse_cmd_cache();
        break;
    }
#endif
    case KEY_EVENT_L | KEY_UNBIND_USER:
		info_get(INFO_PRIVATE_MODE_FLAG, (void *)&fs_scan, 1);
		if(!fs_scan) {//非隐私模式
			info_get(INFO_USER_BIND_FLAG, (void *)&fs_scan, 1);
			if(1 == fs_scan) {
				fs_scan = 0;
				info_set(INFO_USER_BIND_FLAG, (void *)&fs_scan, 1);
			    info_set(INFO_USER_INFO, (void *)&fs_scan, 1);
				info_set(INFO_USER_INFO + 1, (void *)&fs_scan, 1);
				info_set(INFO_USER_INFO + 2, (void *)&fs_scan, 1);
				info_set(INFO_USER_INFO + 3, (void *)&fs_scan, 1);
			    send_unbind_user_msg();
			    play_tone(TONE_DEVICE_UNBINDER, false);
			} else {
                //play_tone();//没有绑定
			}
		}
		break;
//	case RTC_EVENT_SECOND:
//		  rtc_get_time(&rtc.t);
//		  if (0 == rtc.t.second) {
//			  logi("bbbrtc:%04d-%02d-%02d, %02d:%02d:%02d",
//				  rtc.t.year+BASE_YEAR, rtc.t.month, rtc.t.day,
//				  rtc.t.hour, rtc.t.minute, rtc.t.second);
//			  disp_menu(MENU_CLOCK_TIME,rtc.t.hour, rtc.t.minute);
//		  }
//		  /* Modify by arunboy 20200629 */
//		  /*get_record_time();
//		  logi("rec_time : %d", rec_context->rtime.min);
//		  if(rec_context->rtime.min > 120){
//              stop_record();
//			  start_record();
//		  }*/
//		  break;
    default:
        break;
    }
}

#if BT_BACKGROUND_EN
AT(.mode_com_seg)
void bt_bg_preprocess(void)
{
    if (music_mode_calling == true) {
        event_put(BT_EVENT_CALLING);
    }
}
#endif
