#define LOG_LEV     5

#include "sys_inc.h"
#include "user_config.h"
#include "disp.h"
#include "pmu.h"
#include "bt_host_api.h"

#if LED_DIODE_EN
led_diode_t led_diode;


AT(.disp_init_seg)
void disp_led_diode_init(void)
{
    pin_config(LED_RED_PIN_CFG);
    pin_config(LED_BLUE_PIN_CFG);

    led_diode.red_cnt = 0;
    led_diode.red_sta_last = DISP_OFF;
    led_diode.blue_cnt = 0;
    led_diode.blue_sta_last = DISP_OFF;
}

static bool red_led_sta = 0;
static bool blue_led_sta = 0;
static uint8_t rec_led_flash_num = 0;
static bool rec_blu_status = 0;

AT(.disp_seg)
void disp_led_diode_timer(void)
{
    //logi("arunboy led_diode.blue_sta = %d",led_diode.blue_sta);
    if (led_diode.red_sta == DISP_FLASH) {
        led_diode.red_cnt++;
        if (led_diode.red_cnt >= DISP_FLASH_TIME) {
            led_diode.red_cnt = 0;
            if (red_led_sta) {
                pin_config(LED_RED_PIN_LOW_CFG);
                red_led_sta = 0;
            } else {
                pin_config(LED_RED_PIN_HIGH_CFG);
                red_led_sta = 1;
            }
        }
    } else if (led_diode.red_sta == DISP_FLASH_INTERVAL) {
        led_diode.red_cnt++;
        if (led_diode.red_cnt >= DISP_FLASH_TIME) {
            led_diode.red_cnt = 0;
            if (red_led_sta) {
                pin_config(LED_RED_PIN_LOW_CFG);
                red_led_sta = 0;
            } else {
                pin_config(LED_RED_PIN_HIGH_CFG);
                red_led_sta = 1;
            }
        }
    } else if (led_diode.red_sta == DISP_FLASH_FAST) {
        led_diode.red_cnt++;
        if (led_diode.red_cnt >= DISP_FLASH_FAST_TIME) {
            led_diode.red_cnt = 0;
            if (red_led_sta) {
                pin_config(LED_RED_PIN_LOW_CFG);
                red_led_sta = 0;
            } else {
                pin_config(LED_RED_PIN_HIGH_CFG);
                red_led_sta = 1;
            }
        }
    } else if (led_diode.red_sta == DISP_FLASH_SLOW) {
        led_diode.red_cnt++;
        if (led_diode.red_cnt >= DISP_FLASH_SLOW_TIME) {
            led_diode.red_cnt = 0;
            pin_config(LED_RED_PIN_LOW_CFG);
            red_led_sta = 1;
        } else {
            pin_config(LED_RED_PIN_HIGH_CFG);
            red_led_sta = 0;
        }
    } else if (led_diode.red_sta == DISP_ON_DELAY_OFF) {
        led_diode.red_cnt++;
        if (led_diode.red_cnt >= DISP_DELAY_OFF_TIME) {
            led_diode.red_cnt = 0;
            led_diode.red_sta = DISP_OFF;
            led_diode.red_sta_last = DISP_OFF;
            pin_config(LED_RED_PIN_LOW_CFG);
            red_led_sta = 0;
        }
    }

    if (led_diode.blue_sta == DISP_FLASH) {
        led_diode.blue_cnt++;
        if (led_diode.blue_cnt >= DISP_FLASH_TIME) {
            led_diode.blue_cnt = 0;
            if (blue_led_sta) {
                pin_config(LED_BLUE_PIN_LOW_CFG);
                blue_led_sta = 0;
            } else {
                pin_config(LED_BLUE_PIN_HIGH_CFG);
                blue_led_sta = 1;
            }
        }
    } else if (led_diode.blue_sta == DISP_FLASH_INTERVAL) {
        led_diode.blue_cnt++;
        if (led_diode.blue_cnt >= DISP_FLASH_TIME) {
            led_diode.blue_cnt = 0;
            if (red_led_sta) {
                pin_config(LED_BLUE_PIN_LOW_CFG);
                blue_led_sta = 0;
            } else {
                pin_config(LED_BLUE_PIN_HIGH_CFG);
                blue_led_sta = 1;
            }
        }
    } else if (led_diode.blue_sta == DISP_FLASH_FAST) {
        led_diode.blue_cnt++;
        if (led_diode.blue_cnt >= DISP_FLASH_FAST_TIME) {
            led_diode.blue_cnt = 0;
            if (blue_led_sta) {
                pin_config(LED_BLUE_PIN_LOW_CFG);
                blue_led_sta = 0;
            } else {
                pin_config(LED_BLUE_PIN_HIGH_CFG);
                blue_led_sta = 1;
            }
        }
    } else if (led_diode.blue_sta == DISP_FLASH_SLOW) {
        led_diode.blue_cnt++;
        if (led_diode.blue_cnt >= DISP_FLASH_SLOW_TIME) {
            led_diode.blue_cnt = 0;
           pin_config(LED_BLUE_PIN_LOW_CFG);
            blue_led_sta = 1;
        } else {
             pin_config(LED_BLUE_PIN_HIGH_CFG);
            blue_led_sta = 0;
        }
    } else if (led_diode.blue_sta == DISP_ON_DELAY_OFF) {
        led_diode.blue_cnt++;
        if (led_diode.blue_cnt >= DISP_DELAY_OFF_TIME) {
            led_diode.blue_cnt = 0;
            led_diode.blue_sta = DISP_OFF;
            led_diode.blue_sta_last = DISP_OFF;
            pin_config(LED_BLUE_PIN_HIGH_CFG);
            blue_led_sta = 0;
        }
    } else if (led_diode.blue_sta == DISP_REC_START) {
        led_diode.blue_cnt++;
        if (led_diode.blue_cnt >= DISP_REC_START_TIME) {
			led_diode.blue_cnt = 0;
			if (rec_blu_status) {
				if(rec_led_flash_num >= 5){
					led_diode.blue_sta = DISP_OFF;
                    led_diode.blue_sta_last = DISP_OFF;
					pin_config(LED_BLUE_PIN_HIGH_CFG);
					rec_blu_status = 0;
					rec_led_flash_num = 0;
				}
				if(blue_led_sta){
					pin_config(LED_BLUE_PIN_HIGH_CFG);
					blue_led_sta = 0;
				} else {
					pin_config(LED_BLUE_PIN_LOW_CFG);
					blue_led_sta = 1;
					rec_led_flash_num++;
				}
			}
        }
    }
}

AT(.disp_seg)
void disp_led_diode(enum disp_led_id_e id, enum disp_onoff_e onoff)
{
    if (id == LED_RED) {
        if (led_diode.red_sta_last != onoff) {
            led_diode.red_sta = onoff;
            led_diode.red_sta_last = onoff;
            if ((onoff == DISP_ON) || (onoff == DISP_ON_DELAY_OFF)) {
                pin_config(LED_RED_PIN_LOW_CFG);
                red_led_sta = 1;
            } else if((onoff == DISP_OFF) || (onoff == DISP_FLASH_INTERVAL)) {
                pin_config(LED_RED_PIN_HIGH_CFG);
                red_led_sta = 0;
            } else if(onoff == DISP_FLASH) {
                if (red_led_sta) {
                    pin_config(LED_RED_PIN_LOW_CFG);
                    red_led_sta = 0;
                } else {
                    pin_config(LED_RED_PIN_HIGH_CFG);
                    red_led_sta = 1;
                }
            }
        }
    } else {
        if (led_diode.blue_sta_last != onoff) {
            led_diode.blue_sta = onoff;
            led_diode.blue_sta_last = onoff;
            if ((onoff == DISP_ON) || (onoff == DISP_ON_DELAY_OFF) || (onoff == DISP_FLASH_INTERVAL)) {
                pin_config(LED_BLUE_PIN_LOW_CFG);
                blue_led_sta = 1;
            } else if(onoff == DISP_OFF) {
                pin_config(LED_BLUE_PIN_HIGH_CFG);
                blue_led_sta = 0;
            } else if(onoff == DISP_FLASH) {
                if (pin_get_value(LED_BLUE_PIN_NUM)) {
                    pin_config(LED_BLUE_PIN_LOW_CFG);
                    blue_led_sta = 0;
                } else {
                    pin_config(LED_BLUE_PIN_HIGH_CFG);
                    blue_led_sta = 1;
                }
            }
        }
    }
}

volatile bool bat_full_when_charging = false;
AT(.disp_seg)
void disp_state_led_diode_common(enum disp_state_e state)
{
    logi("arunboy disp_state %d",pmu_bat_is_charging());
    //disp priority:bat low > charging > connect/disconnect
    /*if (pmu_bat_is_low()) {
        disp_led_diode(LED_RED, DISP_FLASH_FAST);
    } else if (pmu_bat_is_charging()) {
        if ((state == STATE_BAT_FULL) || (bat_full_when_charging == true)) {
            disp_led_diode(LED_RED, DISP_ON);
            //disp_led_diode(LED_BLUE, DISP_ON);
            bat_full_when_charging = true;
        } else {
            disp_led_diode(LED_RED, DISP_FLASH_SLOW);
        }
    }*/
    if (pmu_bat_is_charging()) {
        if ((state == STATE_BAT_FULL) || (bat_full_when_charging == true)) {
            disp_led_diode(LED_RED, DISP_ON);
            //disp_led_diode(LED_BLUE, DISP_ON);
            bat_full_when_charging = true;
        } else {
            disp_led_diode(LED_RED, DISP_FLASH_SLOW);
        }
    } else if (pmu_bat_is_low()) {
	    disp_led_diode(LED_RED, DISP_FLASH_FAST);
	}
//#if MODE_BT_EN
//	else if (bt_is_connect()) {
//        disp_led_diode(LED_BLUE, DISP_FLASH_SLOW);
//        disp_led_diode(LED_RED, DISP_OFF);
//    } else if (!bt_is_connect()) {
//        disp_led_diode(LED_RED, DISP_OFF);
//        disp_led_diode(LED_BLUE, DISP_OFF);
//        disp_led_diode(LED_RED, DISP_FLASH_INTERVAL);
//        disp_led_diode(LED_BLUE, DISP_FLASH_INTERVAL);
//    } 
//#endif
	else {
        disp_led_diode(LED_RED, DISP_OFF);
    }

    if (state == STATE_UNCHARGEING) {
        bat_full_when_charging = false;
    }
}

AT(.disp_seg)
void disp_led_flase_time_do(enum disp_led_id_e id, uint16_t interval,uint8_t timers)
{
    for(int8_t i=0; i < timers;i++) {
        disp_led_diode(id, DISP_ON);
        msleep(interval);
        disp_led_diode(id, DISP_OFF);
        msleep(interval);
    }
}


AT(.disp_seg)
void disp_state_led_diode(enum disp_state_e state)
{
    logi("disp_state = %d", state);
    switch(state) {

    //system
    case STATE_POWERON:
    case STATE_RESET:
#if DEBUG_DEEP_SLEEP_EN
        disp_led_diode(LED_RED, DISP_ON);
#else
        disp_led_diode(LED_RED, DISP_OFF);
#endif
		//printf_debug("%s,%d 0x40081000=0x%x\r\n",__func__,__LINE__,readl(0x40081000));
        disp_led_diode(LED_BLUE, DISP_ON_DELAY_OFF);
        break;

    case STATE_POWEROFF:
        disp_led_diode(LED_RED, DISP_ON_DELAY_OFF);
        disp_led_diode(LED_BLUE, DISP_OFF);
        break;

    //charge
    case STATE_UNCHARGEING:
        disp_state_led_diode_common(state);
        break;

    case STATE_BAT_LOW:
        disp_state_led_diode_common(state);
        break;

    case STATE_BAT_UNLOW:
        disp_state_led_diode_common(state);
        break;
    //充电的时候要闪
    case STATE_CHARGEING:
		//disp_led_diode(LED_RED, DISP_FLASH_SLOW);
        disp_state_led_diode_common(state);
        break;
    //充满电
    case STATE_BAT_FULL:
		//disp_led_diode(LED_RED, DISP_ON);
        disp_state_led_diode_common(state);
        break;

    //bt
    case STATE_BT_PAIR:
    case STATE_BT_DIS:
        disp_state_led_diode_common(state);
        break;

    case STATE_BT_CON:
        disp_state_led_diode_common(state);
        break;

    //music
    case STATE_MUSIC_NODEV:
        disp_led_diode(LED_BLUE, DISP_FLASH_FAST);
        break;

    case STATE_MUSIC_PLAY:
        disp_led_diode(LED_BLUE, DISP_FLASH_SLOW);
        break;

    case STATE_MUSIC_STOP:
        disp_led_diode(LED_BLUE, DISP_ON);
        break;

    //fm
    case STATE_FM_SEEK:
        disp_led_diode(LED_BLUE, DISP_FLASH_FAST);
        break;

    case STATE_FM_PLAY:
        disp_led_diode(LED_BLUE, DISP_ON);
        break;

	//record
	case STATE_REC_START:
		rec_blu_status = 1;
		//disp_led_flase_time_do(LED_BLUE,3000,5);
		disp_led_diode(LED_BLUE, DISP_REC_START);
		break;
	case STATE_REC_END:
		rec_blu_status = 0;
	    rec_led_flash_num = 0;
		disp_led_diode(LED_BLUE, DISP_ON);
        break;
	case STATE_REC_ERROR:
		disp_led_diode(LED_BLUE, DISP_OFF);
		disp_led_diode(LED_RED, DISP_OFF);
		disp_led_diode(LED_BLUE, DISP_FLASH_FAST);
	    disp_led_diode(LED_RED, DISP_FLASH_FAST);
	    break;
//	case STATE_CLOSE_RED:
//		disp_led_diode(LED_RED, DISP_OFF);
//		break;
//	case STATE_CLOSE_BLUE:		
//		disp_led_diode(LED_BLUE, DISP_OFF);
//		break;
    }
}

#endif

