#ifndef _EQ_CONFIG_H_
#define _EQ_CONFIG_H_

#include "sys_inc.h"
#include "integer.h"
#include "audio_effects.h"

#if EQ_LOAD_EN
extern TimerHandle_t EqLoadTimerHandle;
#endif

EQ_SETTING_STU on_x_eq_setting_for_pageid(PageClass page_id, EQ_SUBBAND band, uint32_t mask, eq_band_setting* setting, bool set);
EQ_SETTING_STU on_x_eq_preamp_for_pageid(PageClass page_id, float* preamp, bool set);
EQ_SETTING_STU on_x_dr_switch_for_pageid(PageClass page_id, bool* on_off, bool set);
EQ_SETTING_STU on_x_dr_setting_for_pageid(PageClass page_id, dly_setting* setting, bool set);
EQ_SETTING_STU on_x_drc_makeup_gain_for_pageid(PageClass page_id, float* makeup, bool set);
EQ_SETTING_STU on_x_drc_subfunc_for_pageid(PageClass page_id, uint32_t mask, DRC_SUBFUNC drc_sb, drc_subfunc_setting* setting, bool set);
EQ_SETTING_STU on_x_tsps_for_pageid(PageClass page_id, uint32_t mask, tsps_setting* setting, bool set);
EQ_SETTING_STU on_x_atune_for_pageid(PageClass page_id, uint32_t mask, atune_setting* setting, bool set);
EQ_SETTING_STU on_x_formant_for_pageid(PageClass page_id, uint32_t mask, fc_setting* setting, bool set);
EQ_SETTING_STU on_x_output_flt_type_setting_for_pageid(PageClass page_id, hl_type hl, hl_filter_setting* setting, bool set);
EQ_SETTING_STU on_x_echo_for_pageid(PageClass page_id, uint32_t mask, echo_setting* setting, bool set);
EQ_SETTING_STU on_x_rev_for_pageid(PageClass page_id, uint32_t mask, rev_setting* setting, bool set);
EQ_SETTING_STU on_x_vss_pro_for_pageid(PageClass page_id, vss_mode* mode, vss_pro_setting* setting, bool set);
EQ_SETTING_STU on_x_vss_switch_for_pageid(PageClass page_id, bool* on_off, bool set);

void on_set_natural_noise_gate(EFFECT_SWTICH sw, bool hold);

void on_set_rock_mic_eq_profile(EFFECT_SWTICH sw);
void on_set_sonic_boom_eq_profile(EFFECT_SWTICH sw);

void on_set_bypass_profile();

void on_set_default_reverb_profile(bool holdRevb, bool cleanOtherAfx);
void on_set_reverb_time(uint8_t time);
void on_set_reverb_gwet(uint8_t g);
void on_set_rec_reverb_ms_switch(bool stereo);

void on_set_default_echo_profile(bool holdEcho, bool cleanOtherAfx);
void on_set_echo_delay(uint8_t dly);
void on_set_echo_gwet(uint8_t g);

void on_set_radio_profile(bool cleanOtherAfx);
void on_set_treb_bass_gain(int8_t gain, uint8_t stream, eq_type_t eq_band);
void on_set_eq_focus_preamp(int8_t db);

void on_set_robot_profile(bool cleanOtherAfx);
void on_set_kid_profile(bool cleanOtherAfx);
void on_set_voc_change_profile(bool cleanOtherAfx);
void on_set_female_profile(bool cleanOtherAfx);
void on_set_male_profile(bool cleanOtherAfx);

void on_set_autotune_profile(ATUNE_CMD cmd, uint8_t val);

void on_set_tbvc_switch(bool on);

void clear_all_holder();

void on_set_default_vss_hp_profile(EFFECT_SWTICH sw);
void on_set_default_vss_spk_profile(EFFECT_SWTICH sw);

void on_set_eq_data(uint8_t *buff,uint32_t length);
void on_set_eq_data_isr(uint8_t *buff,uint32_t length);
void on_get_eq_data(uint8_t page_id,uint8_t *buff);

void on_unload_for_page_id(PageClass page_id);

uint8_t* get_eq_param_buff();
uint8_t* get_eq_param_buff_atom();

void eq_load_timer_cb(xTimerHandle xTimer);
void eq_load_internal();

typedef enum {
    API_NONE = 0,
    /*
        函数 ：void on_set_bypass_profile();
        描述 ：直通模式设置
    */
    API_CLEAR_HOLDERS, // 1
    /*
        函数 ：void clear_all_holder();
        描述 ：取消所有保持节点
    */
    API_REVB_ON_SET_DEFAULT_REVB,// 2
    /*
        函数       ：void on_set_default_reverb_profile(bool holdRevb, bool onlyRevb);
        描述       ：默认混响效果设置
        编译要求 ：AUDIO_MIC_REVERB(1)
    */
    API_REVB_ON_SWITCH_MS,// 3
    /*
        函数 ：void on_set_rec_reverb_ms_switch(bool stereo);
        描述 ：设置目前混响效果的单双通道模式，true双，false单
    */
    API_REVB_ON_SET_GWET,// 4
    /*
        函数 ：void on_set_reverb_gwet(uint8_t g); 0 <= g <= 100, means 0% -> 100%
        描述 ：设置当前混响效果的湿声大小，0% - 100%
    */
    API_REVE_ON_SET_RT60,// 5
    /*
        函数 ：void on_set_reverb_time(uint8_t time); 0 <= time <= 100, means 0% -> 100%
        描述 ：设置当前混响效果的长度大小，0% - 100%
    */
    API_ECHO_ON_SET_DEFAULT_ECHO, //6
    /*
        函数       ：void on_set_default_echo_profile(bool holdEcho, bool cleanOtherAfx);
        描述       ：设置默认的回声模式
                   holdEcho      -> 是否保持回声
                   cleanOtherAfx -> 是否清除其他效果
        编译要求 ：AUDIO_MIC_ECHO(1)
    */
    API_ECHO_ON_SET_DELAY, // 7
    /*
        函数 ：void on_set_echo_delay(uint8_t dly); 0 <= dly <= 100, means 0% -> 100%
        描述 ：设置目前回声效果的延时大小，0% - 100%
    */
    API_ECHO_ON_SET_GWET, // 8
    /*
        函数 ：void on_set_echo_gwet(uint8_t g); 0 <= g <= 100, means 0% -> 100%
        描述 ：设置目前回声效果的湿声大小，0% - 100%
    */
    API_TSPS_ON_SET_FEMALE,// 9
    /*
        函数       ：void on_set_female_profile(bool cleanOtherAfx);
        描述       ：设置女声模式
                   cleanOtherAfx -> 是否清除其他效果
        编译要求 ：AUDIO_MIC_TSPS(1)
                   AUDIO_AFX_REC_FORMANT_EN(1)
    */
    API_TSPS_ON_SET_MALE,// 10
    /*
        函数       ：void on_set_male_profile(bool cleanOtherAfx);
        描述       ：设置男声模式
                   cleanOtherAfx -> 是否清除其他效果
        编译要求 ：AUDIO_MIC_TSPS(1)
    */
    API_TSPS_ON_SET_ROBOT, // 11
    /*
        函数       ：void on_set_robot_profile(bool cleanOtherAfx);
        描述       ：设置机器人模式
                   cleanOtherAfx -> 是否清除其他效果
        编译要求 ：AUDIO_MIC_TSPS(1)
                   AUDIO_MIC_ECHO(1)
    */
    API_TSPS_ON_SET_KID, // 12
    /*
        函数 ：void on_set_kid_profile(bool cleanOtherAfx);
        描述 ：设置童声模式
               cleanOtherAfx -> 是否清除其他效果
        编译要求 ：AUDIO_MIC_TSPS(1)
                   AUDIO_MIC_ECHO(1)
    */
    API_TSPS_ON_SET_VC_CHANGE, // 13
    /*
        函数 ：void on_set_voc_change_profile(bool cleanOtherAfx);
        描述 ：设置变声模式
               cleanOtherAfx -> 是否清除其他效果
        编译要求 ：AUDIO_MIC_TSPS(1)
    */
    API_EQ_ON_SET_RADIO_PROFILE, // 14
    /*
        函数 ：void on_set_radio_profile(bool cleanOtherAfx);
        描述 ：设置喇叭音模式
               cleanOtherAfx -> 是否清除其他效果
        编译要求 ：AUDIO_MIC_EQ(1)
    */
    API_EQ_ON_SET_TREB_BASS_GAIN, // 15
    /*
        函数 ：void on_set_treb_bass_gain(int8_t gain, uint8_t stream, eq_type_t eq_band);
        描述 ：设置MIC/播放流的高低音
               gain    -> 增益， (-12,12）db
               stream  -> 音源， 0(MIC), 1(播放)
               eq_band -> 频带， 0(低频), 1(中频), 2(高频)
        编译要求 ：AUDIO_MIC_EQ(1)
                   AUDIO_PLAYBACK_EQ(1)
    */
    API_EQ_ON_SET_PREAMP, // 16
    /*
        函数 ：void on_set_eq_focus_preamp(int8_t db);
        描述 ：设置当前EQ配置的预增益
               db : -12 - 12
    */
    API_EQ_ON_SET_ATUNE, // 17
    /*
        函数 ：void on_set_autotune_profile(ATUNE_CMD cmd, uint8_t val);
        描述 ：设置电音模式
        cmd    ：     ATUNE_SET_SWITCH
            val  :    0(off), 1(on, donot clear other effects), 2(on, and clear other effects)

        cmd    :     ATUNE_SET_KEY
            val  :    0(KEY_C), 1(KEY_Db), 2(KEY_D), 3(KEY_Eb), 4(KEY_E), 5(KEY_F)
                       6(KEY_Gb), 7(KEY_G), 8(KEY_Ab), 9(KEY_A), 10(KEY_Bb), 11(KEY_B)

        cmd    :     ATUNE_SET_SCALE
            val :     0(CHROMATIC), 1(MAJOR), 2(MINOR)
        编译要求 ：AUDIO_MIC_TSPS(1)
                   AUDIO_AFX_REC_AUTOTUNE_EN(1)
    */
    API_DRC_ON_SET_NGATE, // 18
    /*
        函数 ：void on_set_natural_noise_gate(EFFECT_SWTICH sw, bool hold);
        描述 ：设置噪声门
        sw   : 0(off), 1(on, donot clear other effects), 2(on, and clear other effects)
        hold : 0(do not hold), 1(hold)
        编译要求 ：AUDIO_MIC_DRC(1)
    */
    API_VSS_ON_SET_SPK_PROFILE, // 19
    /*
        函数 ：void on_set_default_vss_spk_profile(EFFECT_SWTICH sw)；
        描述 ：设置3D效果扬声器模式
        sw   : 0(off), 1(on, donot clear other effects), 2(on, and clear other effects)
        编译要求 ：AUDIO_PLAYBACK_VSS(1)
                   AUDIO_AFX_VSS_EQ_ENABLE(1)
    */
    API_VSS_ON_SET_HP_PROFILE,  // 20
    /*
        函数 ：void on_set_default_vss_hp_profile(EFFECT_SWTICH sw)；
        描述 ：设置3D效果耳机模式
        sw   : 0(off), 1(on, donot clear other effects), 2(on, and clear other effects)
        编译要求 ：同上
    */
    API_EQ5_ROCK_MIC,  //21
    /*
        函数 ：void on_set_rock_mic_eq_profile(EFFECT_SWTICH sw)
        描述 ：设置MIC的喊麦模式
        sw   : 0(off), 1(on, donot clear other effects), 2(on, and clear other effects)
        编译要求 : AUDIO_MIC_ECHO(1)
                   AUDIO_AFX_POST_ECHO_EQ_EN(1)
                   AUDIO_MIC_REVERB(1)
                   AUDIO_AFX_POST_REVB_EQ_EN(1)
    */
    API_EQ5_SONIC_BOOM, // 22
    /*
        函数 ：void on_set_sonic_boom_eq_profile(EFFECT_SWTICH sw)
        描述 ：设置MIC的爆音模式
        sw   : 0(off), 1(on, donot clear other effects), 2(on, and clear other effects)
      */
    API_AS_TEST, //23
    /*
        函数 ：int audio_service_cmd(uint32_t cmd, uint32_t val)
        描述 ：Audio service指令集测试，参见audio_service_cmd_e
               消原音是1
      */
    API_TEST_NUM
}ALL_APIS;

#endif




