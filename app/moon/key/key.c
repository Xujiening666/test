#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "key.h"
#include "task_main.h"
#include "dio.h"

key_scan_t key;

#if KEY_SWITCH_EN
#define pin_switch           DIO_ID0

uint8_t key_swith_stat;

enum {
    KEY_SWITCH_SAVE =0,
    KEY_SWITCH_RECORD,
    KEY_SWITCH_NONE,
};
#endif


AT(.key_init_seg)
void key_init(void)
{
#if KEY_IR_EN
    kir_init();
#endif

#if KEY_AD_EN
    kadc_init();
#endif

#if KEY_IO_EN
    kio_init();
#endif

#if KEY_CODING_EN
    kcd_init();
#endif
#if KEY_SWITCH_EN
	dio_set_func(pin_switch, DIO_FUNC_INPUT);    
   // dio_set_pull(pin_switch, DIO_PULL_UP_100K);
	key_swith_stat = KEY_SWITCH_NONE;
#endif

}

AT(.key_seg)
uint32_t key_get_value(void)
{
    uint32_t kret;
    kret = KEY_EVENT_NONE;

    if((key.cur_val == KEY_NONE) || (key.cur_val != key.last_val)) {
        if(key.up_cnt < KEY_CNT_U) {
            key.up_cnt++;
        } else {
            if(key.cur_cnt >= KEY_CNT_L) {
                kret = key.last_val | KEY_EVENT_LU;
            } else if(key.cur_cnt >= KEY_CNT_S) {
                kret = key.last_val | KEY_EVENT_SU;
            }
            key.last_val = key.cur_val;
            key.cur_cnt = 0;
        }
    } else {
        key.cur_cnt++;
        if(key.cur_cnt == KEY_CNT_S) {
            key.up_cnt = 0;
            kret = key.last_val | KEY_EVENT_S;
        } else if(key.cur_cnt == KEY_CNT_L) {
            kret = key.last_val | KEY_EVENT_L;
        } else if(key.cur_cnt == KEY_CNT_LH) {
            kret = key.last_val | KEY_EVENT_H;
            key.cur_cnt = KEY_CNT_L;
        }
    }
//	if(kret != 0){
//		logd("kret = 0x%x",kret);
//	}
    return kret;

}

AT(.key_seg)
void key_scan(void)
{
#if (KEY_IO_EN || KEY_IR_EN || (KEY_AD_EN&&KADC_SCAN_EN) || KEY_CODING_EN)
    uint32_t kret;
    kret = KEY_NONE;
    key.cur_val = KEY_NONE;

#if KEY_IO_EN
    key.cur_val = kio_get_key();
#endif

#if KEY_IR_EN
    if (KEY_NONE == key.cur_val) {
        key.cur_val = kir_get_key();
    }
#endif

#if KEY_AD_EN
    if (KEY_NONE == key.cur_val) {
        key.cur_val = kadc_scan_get_key();
    }
#endif

#if KEY_CODING_EN
    if (KEY_NONE == key.cur_val) {
        key.cur_val = kcd_get_key();
    }
#endif

    if (KEY_NONE != key.cur_val) {
        sleep_delay_set();
    }

    kret = key_get_value();
    if (kret != KEY_EVENT_NONE) {
        if ((KEY_EVENT_SU | KEY_MODE) == kret) {
            mode_update(1);
            key_mode = true;
        }
        event_put(kret);
        //logi("k:%x", kret);
    }
#endif

#if KEY_SWITCH_EN	
		
	if (KEY_SWITCH_NONE == key_swith_stat)
	{
		key_swith_stat = dio_get_value(pin_switch);
			
		logi("%s-%u key_swith_stat=%d", __func__, __LINE__,key_swith_stat);
		if (key_swith_stat == KEY_SWITCH_SAVE) {
			event_put(KEY_EVENT_SU | KEY_PLAY_SAVE_REC);
		} else if (key_swith_stat == KEY_SWITCH_RECORD){
			event_put(KEY_EVENT_SU | KEY_PLAY_REC);
		}
		

	}else if (key_swith_stat != dio_get_value(pin_switch)){
		key_swith_stat = dio_get_value(pin_switch);		
		 logi("%s-%u key_swith_stat=%d", __func__, __LINE__,key_swith_stat);
		 if (key_swith_stat == KEY_SWITCH_SAVE) {
			event_put(KEY_EVENT_SU | KEY_PLAY_SAVE_REC);
		 } else if (key_swith_stat == KEY_SWITCH_RECORD){
		 	event_put(KEY_EVENT_SU | KEY_PLAY_REC);			
		 }
		
	}
		
#endif

}

