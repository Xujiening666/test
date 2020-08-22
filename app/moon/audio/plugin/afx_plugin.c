#define LOG_LEV 4
#include "afx_plugin.h"
#include "log.h"

#define AFX_PLG_FUNC __attribute__((section(".afx_plug_seg")))

//-------- USBEXTENDCMD回调插件
AFX_PLG_FUNC int on_get_afxmsk_clbk(uint8_t cmd, uint32_t* fill)
{
    int ret = 0;
    if(cmd == 0xCF) {
        uint32_t mask = 0;
        if(freq_shift_lib_assign()) {
            mask |= 0x01;
        }
        if(nhs_lib) {
            mask |= 0x02;
        }
        *fill = mask;
        ret = 4;
    }
    return ret;
}
//-------- 音效Reset插件回调
EQ_SETTING_STU on_x_amicx_para_for_pageid(PageClass page_id, amicx_setting* setting, bool set);

AFX_PLG_FUNC void afx_reset_plug()
{
    amicx_setting as = {0};
    as.amic0_gain = AUDIO_AMIC0_ANALOG_GAIN;
    as.amic0_en   = (AUDIO_REC_INPUT_SEL == 0);
    as.amic1_gain = AUDIO_AMIC1_ANALOG_GAIN;
    as.amic1_en   = (AUDIO_REC_INPUT_SEL == 1);
    as.amic2_gain = AUDIO_AMIC2_ANALOG_GAIN;
    as.amic2_en   = (AUDIO_REC_INPUT_SEL == 2);
    on_x_amicx_para_for_pageid(PAGE_PLG, &as, EQ_SET_DATA);

    app_fsrtprm.deltafreq = app_fs_init.deltafreq;
    audio_service_cmd(AS_CMD_NOTIFY_FS_LOAD_RTPRM, 0);

    libc_memcpy(&app_nhsrtprm, &app_nhs_init, sizeof(nhs_rtprms_t));
    audio_service_cmd(AS_CMD_NOTIFY_NHS_LOAD_RTPRM, 0);
}

//-------- 音效设置插件回调
AFX_PLG_FUNC bool afx_plugin_area_setting(uint32_t page_id, uint32_t cmd_type, uint32_t item_index,
                        eq_data_type value)
{
    bool ret = false;
    if(PLG_SETTING_RANGE_FOR_PAGE_ID(page_id)) {
        if(cmd_type >= TYPE_PLG_AMICX_SETTING_START &&
          cmd_type <= TYPE_PLG_AMICX_SETTING_END) {
            logd("hw setting(%d, %d, %d)", cmd_type, TYPE_PLG_AMICX_SETTING_START, TYPE_PLG_AMICX_SETTING_END);
            uint8_t gain = GET_U8_FOR_F32(value.data);
            logd("gain:%d", gain);
            if(cmd_type == TYPE_PLG_AMIC0_GAIN) {
                mixer_set_ctl(SND_CODEC, AMIC0_GAIN, gain);
            }
            else if(cmd_type == TYPE_PLG_AMIC1_GAIN) {
                mixer_set_ctl(SND_CODEC, AMIC1_GAIN, gain);
            }
            else if(cmd_type == TYPE_PLG_AMIC2_GAIN) {
                mixer_set_ctl(SND_CODEC, AMIC2_GAIN, gain);
            }
            else if(cmd_type == TYPE_PLG_AMIC0_ENABLE) {
                mixer_set_ctl(SND_CODEC, AMIC0_EN, gain);
            }
            else if(cmd_type == TYPE_PLG_AMIC1_ENABLE) {
                mixer_set_ctl(SND_CODEC, AMIC1_EN, gain);
            }
            else if(cmd_type == TYPE_PLG_AMIC2_ENABLE) {
                mixer_set_ctl(SND_CODEC, AMIC2_EN, gain);
            }
        }
        else if(cmd_type >= TYPE_PLG_MIC_NHS_SETTING_START &&
          cmd_type <= TYPE_PLG_MIC_NHS_SETTING_END) {
            logd("nhs setting");
            float val = value.data;
            logd("value : %d", (int)(100*val));
            bool set = false;
            switch(cmd_type) {
                case TYPE_PLG_MIC_NHS_NP:
                {
                    app_nhsrtprm.Np = GET_S8_FOR_F32(val);
                    set = true;
                    break;
                }
                case TYPE_PLG_MIC_NHS_NF:
                {
                    app_nhsrtprm.Nf = GET_S8_FOR_F32(val);
                    set = true;
                    break;
                }
                case TYPE_PLG_MIC_NHS_THD_PTPR:
                {
                    app_nhsrtprm.Tptpr = GET_S8_FOR_F32(val);
                    set = true;
                    break;
                }
                case TYPE_PLG_MIC_NHS_THD_PAPR:
                {
                    app_nhsrtprm.Tpapr = GET_S8_FOR_F32(val);
                    set = true;
                    break;
                }
                case TYPE_PLG_MIC_NHS_THD_PHPR:
                {
                    app_nhsrtprm.Tphpr = GET_S8_FOR_F32(val);
                    set = true;
                    break;
                }
                case TYPE_PLG_MIC_NHS_THD_PNPR:
                {
                    app_nhsrtprm.Tpnpr = GET_S8_FOR_F32(val);
                    set = true;
                    break;
                }
                case TYPE_PLG_MIC_NHS_THD_DROP:
                {
                    app_nhsrtprm.Tdrop = GET_S8_FOR_F32(val);
                    set = true;
                    break;
                }
                case TYPE_PLG_MIC_NHS_ATT:
                {
                    app_nhsrtprm.Att = GET_S8_FOR_F32(val);
                    set = true;
                    break;
                }
                case TYPE_PLG_MIC_NHS_TIME_DROP:
                {
                    app_nhsrtprm.tdrop = GET_U16_FOR_F32(val);
                    set = true;
                    break;
                }
                case TYPE_PLG_MIC_NHS_TIME_HISTORY:
                {
                    app_nhsrtprm.thist = GET_U16_FOR_F32(val);
                    set = true;
                    break;
                }
                case TYPE_PLG_MIC_NHS_TIME_HOLD:
                {
                    app_nhsrtprm.thold = GET_U16_FOR_F32(val);
                    set = true;
                    break;
                }
                case TYPE_PLG_MIC_NHS_THD_REL:
                {
                    app_nhsrtprm.Trel = GET_S8_FOR_F32(val);
                    set = true;
                    break;
                }
                case TYPE_PLG_MIC_NHS_THD_SINGLE_TONE:
                {
                    app_nhsrtprm.Ttone = val;
                    set = true;
                    break;
                }
                case TYPE_PLG_MIC_NHS_GAIN_DSTEP:
                {
                    app_nhsrtprm.Gdstep = GET_S8_FOR_F32(val);
                    set = true;
                    break;
                }
                case TYPE_PLG_MIC_NHS_GAIN_FLOOR_DECREASE:
                {
                    app_nhsrtprm.Gfloor = GET_S8_FOR_F32(val);
                    set = true;
                    break;
                }
                case TYPE_PLG_MIC_NHS_TIME_AVG_CONST:
                {
                    app_nhsrtprm.tav = GET_U16_FOR_F32(val);
                    set = true;
                    break;
                }
                case TYPE_PLG_MIC_NHS_TIME_DESCEND:
                {
                    app_nhsrtprm.tdes = GET_U16_FOR_F32(val);
                    set = true;
                    break;
                }
                case TYPE_PLG_MIC_NHS_TIME_AESEND:
                {
                    app_nhsrtprm.taes = GET_U16_FOR_F32(val);
                    set = true;
                    break;
                }
                case TYPE_PLG_MIC_NHS_TIME_ENTER:
                {
                    app_nhsrtprm.tent = GET_U16_FOR_F32(val);
                    set = true;
                    break;
                }
                case TYPE_PLG_MIC_NHS_TIME_COFF_SMOOTH:
                {
                    app_nhsrtprm.tsmooth = GET_U16_FOR_F32(val);
                    set = true;
                    break;
                }
            }
            if(set) {
                audio_service_cmd(AS_CMD_NOTIFY_NHS_LOAD_RTPRM, 0);
            }
        }
        else if(cmd_type >= TYPE_PLG_FS_SETTING_START &&
          cmd_type <= TYPE_PLG_FS_SETTING_END) {
            logd("fs setting");
            int32_t val = GET_S32_FOR_F32(value.data);
            logd("value : %d", val);
            bool set = false;
            if(cmd_type == TYPE_PLG_FS_SETTING_DELTAFREQ) {
                 app_fsrtprm.deltafreq = val;
                 set = true;
            }
            if(set) {
                audio_service_cmd(AS_CMD_NOTIFY_FS_LOAD_RTPRM, 0);
            }
        }
        ret = true;
    }
    return ret;
}

static void amicx_setting_check(amicx_setting* setting)
{
    setting->amic0_gain = setting->amic0_gain>14?14:
                        setting->amic0_gain<0?0:setting->amic0_gain;
    setting->amic1_gain = setting->amic1_gain>14?14:
                        setting->amic1_gain<0?0:setting->amic1_gain;
    setting->amic2_gain = setting->amic2_gain>14?14:
                        setting->amic2_gain<0?0:setting->amic2_gain;
    setting->amic0_en = !!setting->amic0_en;
    setting->amic1_en = !!setting->amic1_en;
    setting->amic2_en = !!setting->amic2_en;
}

AFX_PLG_FUNC EQ_SETTING_STU on_x_amicx_para_for_pageid(PageClass page_id, amicx_setting* setting, bool set)
{
    if(page_id != PAGE_PLG)
        return EQ_XET_NONE_RESPONSE;

    if(set) amicx_setting_check(setting);
    if(set) {
        mixer_set_ctl(SND_CODEC, AMIC0_GAIN, (uint32_t)setting->amic0_gain);
        mixer_set_ctl(SND_CODEC, AMIC0_EN,   (uint32_t)setting->amic0_en);
        mixer_set_ctl(SND_CODEC, AMIC1_GAIN, (uint32_t)setting->amic1_gain);
        mixer_set_ctl(SND_CODEC, AMIC1_EN,   (uint32_t)setting->amic1_en);
        mixer_set_ctl(SND_CODEC, AMIC2_GAIN, (uint32_t)setting->amic2_gain);
        mixer_set_ctl(SND_CODEC, AMIC2_EN,   (uint32_t)setting->amic2_en);
    }
    else {
        mixer_get_ctl(SND_CODEC, AMIC0_GAIN, (uint32_t*)&setting->amic0_gain);
        mixer_get_ctl(SND_CODEC, AMIC0_EN,   (uint32_t*)&setting->amic0_en);
        mixer_get_ctl(SND_CODEC, AMIC1_GAIN, (uint32_t*)&setting->amic1_gain);
        mixer_get_ctl(SND_CODEC, AMIC1_EN,   (uint32_t*)&setting->amic1_en);
        mixer_get_ctl(SND_CODEC, AMIC2_GAIN, (uint32_t*)&setting->amic2_gain);
        mixer_get_ctl(SND_CODEC, AMIC2_EN,   (uint32_t*)&setting->amic2_en);
    }
    return EQ_SET_NO_ERR;
}

//---------------- 音效上传插件回调
AFX_PLG_FUNC void on_unload_for_page_id_ex(uint8_t page_id)
{
    eq_data ed[8] = {0};
    int ret = EQ_SET_NO_ERR;

    if(PLG_SETTING_RANGE_FOR_PAGE_ID(page_id))
    {
        do {
            CHECK_AND_PREPARE_BUFFER((uint8_t*)ed, sizeof(ed), return);
            amicx_setting hs = {0};
            ret = on_x_amicx_para_for_pageid(PAGE_PLG, &hs, EQ_GET_DATA);
            if(EQ_XET_NONE_RESPONSE == ret) {
                break;
            }
            loge("amic0 g:%d", hs.amic0_gain);
            loge("amic0 %s", hs.amic0_en?"on":"off");
            loge("amic1 g:%d", hs.amic1_gain);
            loge("amic1 %s", hs.amic1_en?"on":"off");
            loge("amic2 g:%d", hs.amic2_gain);
            loge("amic2 %s", hs.amic2_en?"on":"off");

            ed[0].page_id    = page_id;
            ed[0].item_type  = TYPE_PLG_AMIC0_GAIN;
            ed[0].fill.data  = (float)hs.amic0_gain;

            ed[1].page_id    = page_id;
            ed[1].item_type  = TYPE_PLG_AMIC1_GAIN;
            ed[1].fill.data  = (float)hs.amic1_gain;

            ed[2].page_id    = page_id;
            ed[2].item_type  = TYPE_PLG_AMIC2_GAIN;
            ed[2].fill.data  = (float)hs.amic2_gain;

            ed[3].page_id    = page_id;
            ed[3].item_type  = TYPE_PLG_AMIC0_ENABLE;
            ed[3].fill.data  = (float)hs.amic0_en?1.0f:0.0f;

            ed[4].page_id    = page_id;
            ed[4].item_type  = TYPE_PLG_AMIC1_ENABLE;
            ed[4].fill.data  = (float)hs.amic1_en?1.0f:0.0f;

            ed[5].page_id    = page_id;
            ed[5].item_type  = TYPE_PLG_AMIC2_ENABLE;
            ed[5].fill.data  = (float)hs.amic2_en?1.0f:0.0f;

            loge("Fs deltafreq : %d", app_fsrtprm.deltafreq);

            ed[6].page_id    = page_id;
            ed[6].item_type  = TYPE_PLG_FS_SETTING_DELTAFREQ;
            ed[6].fill.data  = (float)app_fsrtprm.deltafreq;

            SEND_DATA_TO_PC((uint8_t *)ed);

            CHECK_AND_PREPARE_BUFFER((uint8_t*)ed, sizeof(ed), return);

            ed[0].page_id    = page_id;
            ed[0].item_type  = TYPE_PLG_MIC_NHS_NP;
            ed[0].fill.data  = (float)app_nhsrtprm.Np;

            ed[1].page_id    = page_id;
            ed[1].item_type  = TYPE_PLG_MIC_NHS_NF;
            ed[1].fill.data  = (float)app_nhsrtprm.Nf;

            ed[2].page_id    = page_id;
            ed[2].item_type  = TYPE_PLG_MIC_NHS_THD_PTPR;
            ed[2].fill.data  = (float)app_nhsrtprm.Tptpr;

            ed[3].page_id    = page_id;
            ed[3].item_type  = TYPE_PLG_MIC_NHS_THD_PAPR;
            ed[3].fill.data  = (float)app_nhsrtprm.Tpapr;

            ed[4].page_id    = page_id;
            ed[4].item_type  = TYPE_PLG_MIC_NHS_THD_PHPR;
            ed[4].fill.data  = (float)app_nhsrtprm.Tphpr;

            ed[5].page_id    = page_id;
            ed[5].item_type  = TYPE_PLG_MIC_NHS_THD_PNPR;
            ed[5].fill.data  = (float)app_nhsrtprm.Tpnpr;

            ed[6].page_id    = page_id;
            ed[6].item_type  = TYPE_PLG_MIC_NHS_THD_DROP;
            ed[6].fill.data  = (float)app_nhsrtprm.Tdrop;

            ed[7].page_id    = page_id;
            ed[7].item_type  = TYPE_PLG_MIC_NHS_ATT;
            ed[7].fill.data  = (float)app_nhsrtprm.Att;

            SEND_DATA_TO_PC((uint8_t *)ed);
            CHECK_AND_PREPARE_BUFFER((uint8_t*)ed, sizeof(ed), return);

            ed[0].page_id    = page_id;
            ed[0].item_type  = TYPE_PLG_MIC_NHS_TIME_DROP;
            ed[0].fill.data  = (float)app_nhsrtprm.tdrop;

            ed[1].page_id    = page_id;
            ed[1].item_type  = TYPE_PLG_MIC_NHS_TIME_HISTORY;
            ed[1].fill.data  = (float)app_nhsrtprm.thist;

            ed[2].page_id    = page_id;
            ed[2].item_type  = TYPE_PLG_MIC_NHS_TIME_HOLD;
            ed[2].fill.data  = (float)app_nhsrtprm.thold;

            ed[3].page_id    = page_id;
            ed[3].item_type  = TYPE_PLG_MIC_NHS_THD_REL;
            ed[3].fill.data  = (float)app_nhsrtprm.Trel;

            ed[4].page_id    = page_id;
            ed[4].item_type  = TYPE_PLG_MIC_NHS_THD_SINGLE_TONE;
            ed[4].fill.data  = (float)app_nhsrtprm.Ttone;

            ed[5].page_id    = page_id;
            ed[5].item_type  = TYPE_PLG_MIC_NHS_GAIN_DSTEP;
            ed[5].fill.data  = (float)app_nhsrtprm.Gdstep;

            ed[6].page_id    = page_id;
            ed[6].item_type  = TYPE_PLG_MIC_NHS_GAIN_FLOOR_DECREASE;
            ed[6].fill.data  = (float)app_nhsrtprm.Gfloor;

            ed[7].page_id    = page_id;
            ed[7].item_type  = TYPE_PLG_MIC_NHS_TIME_AVG_CONST;
            ed[7].fill.data  = (float)app_nhsrtprm.tav;

            SEND_DATA_TO_PC((uint8_t *)ed);
            CHECK_AND_PREPARE_BUFFER((uint8_t*)ed, sizeof(ed), return);

            ed[0].page_id    = page_id;
            ed[0].item_type  = TYPE_PLG_MIC_NHS_TIME_DESCEND;
            ed[0].fill.data  = (float)app_nhsrtprm.tdes;

            ed[1].page_id    = page_id;
            ed[1].item_type  = TYPE_PLG_MIC_NHS_TIME_AESEND;
            ed[1].fill.data  = (float)app_nhsrtprm.taes;

            ed[2].page_id    = page_id;
            ed[2].item_type  = TYPE_PLG_MIC_NHS_TIME_ENTER;
            ed[2].fill.data  = (float)app_nhsrtprm.tent;

            ed[3].page_id    = page_id;
            ed[3].item_type  = TYPE_PLG_MIC_NHS_TIME_COFF_SMOOTH;
            ed[3].fill.data  = (float)app_nhsrtprm.tsmooth;

            SEND_DATA_TO_PC((uint8_t *)ed);
        }while(0);
    }
}

AFX_PLG_FUNC void on_unload_ex()
{
    on_unload_for_page_id_ex(PAGE_PLG);
}