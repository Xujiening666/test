#define BASIC_AFX_FUNC __attribute__((section(".basic_afx_api_seg")))

static void eq_setting_check(eq_band_setting* setting)
{
    setting->filter_type = setting->filter_type<BIQUAD_START?BIQUAD_START:(
                        setting->filter_type>BIQUAD_END?BIQUAD_END:setting->filter_type
    );
    setting->G = setting->G<-12.0?-12.0:(
                setting->G>12.0?12.0:setting->G
    );
    setting->Q_factor = setting->Q_factor<0.1?0.1:(
                setting->Q_factor>16.0?16.0:setting->Q_factor
    );
}

static void hlp_setting_check(hl_filter_setting* setting)
{
    setting->fc = setting->fc < 20?20:
                 (setting->fc > 20000?20000:setting->fc);
}

static void tsps_setting_check(tsps_setting* setting)
{
    //should be 2^(-1) - 2^(1)
    setting->alpha_ts = setting->alpha_ts<0.5?0.5:(setting->alpha_ts>2.0?2.0:setting->alpha_ts);
    //should be 2^(-1) - 2^(1)
    setting->alpha_ps = setting->alpha_ps<0.5?0.5:(setting->alpha_ps>2.0?2.0:setting->alpha_ps);
}

static void fc_setting_check(fc_setting* setting)
{
    setting->alpha_fc = setting->alpha_fc<0.25?0.25:(setting->alpha_fc>4.0?4.0:setting->alpha_fc);
}

static void echo_setting_check(echo_setting* setting)
{
    setting->gFbc = setting->gFbc<0.0?0.0:(setting->gFbc>0.85?0.85:setting->gFbc);
    setting->gDry = setting->gDry<0.0?0.0:(setting->gDry>1.0?1.0:setting->gDry);
    setting->gWet = setting->gWet<0.0?0.0:(setting->gWet>1.0?1.0:setting->gWet);
    setting->dly  = setting->dly>300?300:setting->dly;
}

static void dr_setting_check(dly_setting* setting)
{
    setting->rev_factor = setting->rev_factor>0?1:-1;
    setting->rtdly = setting->rtdly<0?0:(
                    setting->rtdly>200?200:setting->rtdly
    );
}

static void rerb_setting_check(rev_setting* setting)
{
    setting->revb_time = setting->revb_time > 10000?10000:(setting->revb_time < 312?312:setting->revb_time);
    setting->damp_freq_lo = setting->damp_freq_lo > 20000?20000:(setting->damp_freq_lo < 20?20:setting->damp_freq_lo);
    setting->damp_freq_hi = setting->damp_freq_hi > 20000?20000:(setting->damp_freq_hi < 20?20:setting->damp_freq_hi);
    setting->lpf_freq = setting->lpf_freq > 20000?20000:(setting->lpf_freq < 20?20:setting->lpf_freq);
    setting->hpf_freq = setting->hpf_freq > 20000?20000:(setting->hpf_freq < 20?20:setting->hpf_freq);
    setting->pre_delay = setting->pre_delay > 100?100:(setting->pre_delay < 0?0:setting->pre_delay);
    setting->diff = setting->diff > 0.99?0.99:(setting->diff < 0.0?0.0:setting->diff);
    setting->damp_ratio_lo = setting->damp_ratio_lo > 1.0?1.0:(setting->damp_ratio_lo < 0.1?0.1:setting->damp_ratio_lo);
    setting->damp_ratio_hi = setting->damp_ratio_hi > 1.0?1.0:(setting->damp_ratio_hi < 0.1?0.1:setting->damp_ratio_hi);
    setting->g_wet = setting->g_wet > 1.0?1.0:(setting->g_wet < 0.0?0.0:setting->g_wet);
    setting->g_dry = setting->g_dry > 1.0?1.0:(setting->g_dry < 0.0?0.0:setting->g_dry);
}

static void vss_setting_check(vss_pro_setting* setting)
{
}

static void drc_subfunc_setting_check(drc_subfunc_setting* setting)
{
    setting->x_thd = setting->x_thd<-80?-80:(
                    setting->x_thd>0?0:setting->x_thd
    );
    setting->x_ratio = setting->x_ratio<1?1:setting->x_ratio;
}

#define EQ_SBPARA_FLT_OFFSET            (uint32_t)(&((eq_band_setting*)0)->filter_type)
#define EQ_SBPARA_FC_OFFSET             (uint32_t)(&((eq_band_setting*)0)->fc)
#define EQ_SBPARA_G_OFFSET              (uint32_t)(&((eq_band_setting*)0)->G)
#define EQ_SBPARA_Q_OFFSET              (uint32_t)(&((eq_band_setting*)0)->Q_factor)
#define EQ_SBPARA_DISABLE_OFFSET        (uint32_t)(&((eq_band_setting*)0)->disable)

const uint8_t eq_setting_offset_map[EQ_SBPARA_NUM] = {
    EQ_SBPARA_FLT_OFFSET,
    EQ_SBPARA_FC_OFFSET,
    EQ_SBPARA_G_OFFSET,
    EQ_SBPARA_Q_OFFSET,
    EQ_SBPARA_DISABLE_OFFSET,
};

extern bool AFX_HLF_EN;

BASIC_AFX_FUNC EQ_SETTING_STU on_x_eq_setting_for_pageid(PageClass page_id, EQ_SUBBAND band, uint32_t mask, eq_band_setting* setting, bool set)
{
    if(band > EQ_BAND_14 || band < EQ_BAND_1)
        return EQ_SET_INVALID_STU;

    if(page_id == PAGE_EFFECT)
    {
        return EQ_SET_PATCH_WELCOME;
    }

    if(page_id == PAGE_OUTPUT1 ||
        page_id == PAGE_OUTPUT2)
    {
        if(AFX_HLF_EN) {
            if(band > EQ_BAND_10 || band < EQ_BAND_1)
                return EQ_SET_INVALID_STU;
        }
        else {
            if(band > EQ_BAND_14 || band < EQ_BAND_1)
                return EQ_SET_INVALID_STU;
        }
    }

    if(set) eq_setting_check(setting);

    void *eq_left  = 0;
    void *eq_right = 0;

    if(PAGE_VSS == page_id) {
        eq_left  = getVssXAddrForPageId(PAGE_VSS, VSS_LEFT_EQ);
        eq_right = getVssXAddrForPageId(PAGE_VSS, VSS_RIGHT_EQ);
    }
    else {
        eq_left  = getLeftChannelEqAddrForPageId(page_id);
        eq_right = getRightChannelEqAddrForPageId(page_id);
    }

    if(!eq_left && !eq_right) {
        return EQ_XET_NONE_RESPONSE;
    }

    for(int m = 0; m < EQ_SBPARA_NUM; m++) {
        if(mask & COMMON_PARAS_MASK(m)) {
            char* addr = ((char*)setting) + eq_setting_offset_map[m];
            x_eq_subband_para(eq_left, band, m, (void*)addr, set);
            //In music page , do not get right channel para again, case these two is the same
            if(PAGE_MUSIC != page_id || set)
            {
                x_eq_subband_para(eq_right, band, m, (void*)addr, set);
            }
        }
    }

    if(set)
    {
        uint8_t *eq_buf = get_eq_param_buff();
        eq_data ed   = {0};
        uint8_t cnt  = 0;

        ed.page_id    = page_id;
        ed.item_index = band;
        ed.item_type  = TYPE_EQ_PRE_SETTING;
        ed.fill.clbk  = AFX_PSAL_HOOK;
        libc_memcpy(eq_buf + 8*cnt++, &ed, sizeof(ed));

        on_set_eq_data(eq_buf, cnt*8);
    }

    return EQ_SET_NO_ERR;
}

BASIC_AFX_FUNC EQ_SETTING_STU on_x_output_flt_type_setting_for_pageid(PageClass page_id, hl_type hl, hl_filter_setting* setting, bool set)
{
    if(page_id != PAGE_OUTPUT1 &&
        page_id != PAGE_OUTPUT2)
        return EQ_SET_PATCH_WELCOME;

    if(set) hlp_setting_check(setting);

    void *eq_x  = 0;
    if(page_id == PAGE_OUTPUT1) {
        eq_x = getLeftChannelEqAddrForPageId(page_id);
    }
    if(page_id == PAGE_OUTPUT2) {
        eq_x = getRightChannelEqAddrForPageId(page_id);
    }
    if(!eq_x) {
        return EQ_XET_NONE_RESPONSE;
    }

    EQ_SUBBAND band = hl == LOW_PASS?EQ_BAND_11:EQ_BAND_13;

    if(!set)
    {
        int16_t filter_type;

        x_eq_subband_para(eq_x, band, EQ_FILTER_TYPE, (void*)&filter_type, set);
        x_eq_subband_para(eq_x, band, EQ_FC, (void*)&setting->fc, set);

        switch(filter_type){
            case APF2ZF(APF2ZF_DFTYPE_LPF, APF2ZF_APFTYPE_1STORDER, APF2ZF_SLOPE_6DBOCT):
                setting->sub_type    = TYPE_6DB_1ST_ORDER;
                break;
            case APF2ZF(APF2ZF_DFTYPE_LPF, APF2ZF_APFTYPE_BESSEL, APF2ZF_SLOPE_12DBOCT):
                setting->sub_type    = TYPE_12DB_BESSEL;
                break;
            case APF2ZF(APF2ZF_DFTYPE_LPF, APF2ZF_APFTYPE_BUTTER, APF2ZF_SLOPE_12DBOCT):
                setting->sub_type    = TYPE_12DB_BUTTER;
                break;
            case APF2ZF(APF2ZF_DFTYPE_LPF, APF2ZF_APFTYPE_LINK, APF2ZF_SLOPE_12DBOCT):
                setting->sub_type    = TYPE_12DB_LINK;
                break;
            case APF2ZF(APF2ZF_DFTYPE_LPF, APF2ZF_APFTYPE_BESSEL, APF2ZF_SLOPE_18DBOCT):
                setting->sub_type    = TYPE_18DB_BESSEL;
                break;
            case APF2ZF(APF2ZF_DFTYPE_LPF, APF2ZF_APFTYPE_BUTTER, APF2ZF_SLOPE_18DBOCT):
                setting->sub_type    = TYPE_18DB_BUTTER;
                break;
            case APF2ZF(APF2ZF_DFTYPE_LPF, APF2ZF_APFTYPE_BESSEL, APF2ZF_SLOPE_24DBOCT):
                setting->sub_type    = TYPE_24DB_BESSEL;
                break;
            case APF2ZF(APF2ZF_DFTYPE_LPF, APF2ZF_APFTYPE_BUTTER, APF2ZF_SLOPE_24DBOCT):
                setting->sub_type    = TYPE_24DB_BUTTER;
                break;
            case APF2ZF(APF2ZF_DFTYPE_LPF, APF2ZF_APFTYPE_LINK, APF2ZF_SLOPE_24DBOCT):
                setting->sub_type    = TYPE_24DB_LINK;
                break;

            case APF2ZF(APF2ZF_DFTYPE_HPF, APF2ZF_APFTYPE_1STORDER, APF2ZF_SLOPE_6DBOCT):
                setting->sub_type    = TYPE_6DB_1ST_ORDER;
                break;
            case APF2ZF(APF2ZF_DFTYPE_HPF, APF2ZF_APFTYPE_BESSEL, APF2ZF_SLOPE_12DBOCT):
                setting->sub_type    = TYPE_12DB_BESSEL;
                break;
            case APF2ZF(APF2ZF_DFTYPE_HPF, APF2ZF_APFTYPE_BUTTER, APF2ZF_SLOPE_12DBOCT):
                setting->sub_type    = TYPE_12DB_BUTTER;
                break;
            case APF2ZF(APF2ZF_DFTYPE_HPF, APF2ZF_APFTYPE_LINK, APF2ZF_SLOPE_12DBOCT):
                setting->sub_type    = TYPE_12DB_LINK;
                break;
            case APF2ZF(APF2ZF_DFTYPE_HPF, APF2ZF_APFTYPE_BESSEL, APF2ZF_SLOPE_18DBOCT):
                setting->sub_type    = TYPE_18DB_BESSEL;
                break;
            case APF2ZF(APF2ZF_DFTYPE_HPF, APF2ZF_APFTYPE_BUTTER, APF2ZF_SLOPE_18DBOCT):
                setting->sub_type    = TYPE_18DB_BUTTER;
                break;
            case APF2ZF(APF2ZF_DFTYPE_HPF, APF2ZF_APFTYPE_BESSEL, APF2ZF_SLOPE_24DBOCT):
                setting->sub_type    = TYPE_24DB_BESSEL;
                break;
            case APF2ZF(APF2ZF_DFTYPE_HPF, APF2ZF_APFTYPE_BUTTER, APF2ZF_SLOPE_24DBOCT):
                setting->sub_type    = TYPE_24DB_BUTTER;
                break;
            case APF2ZF(APF2ZF_DFTYPE_HPF, APF2ZF_APFTYPE_LINK, APF2ZF_SLOPE_24DBOCT):
                setting->sub_type    = TYPE_24DB_LINK;
                break;

            default:
                setting->sub_type    = TYPE_BYPASS;
                break;
        }
        int8_t disable = 0;
        x_eq_subband_para(eq_x, band, EQ_DISABLE, (void*)&disable, set);
        if(disable)
        {
            setting->sub_type    = TYPE_BYPASS;
        }
    }
    else
    {
        int8_t disable = 0;
        int16_t filter_type;
        switch(setting->sub_type){
            case TYPE_6DB_1ST_ORDER:
                filter_type = APF2ZF(hl, APF2ZF_APFTYPE_1STORDER, APF2ZF_SLOPE_6DBOCT);
                break;
            case TYPE_12DB_BESSEL:
                filter_type = APF2ZF(hl, APF2ZF_APFTYPE_BESSEL, APF2ZF_SLOPE_12DBOCT);
                break;
            case TYPE_12DB_BUTTER:
                filter_type = APF2ZF(hl, APF2ZF_APFTYPE_BUTTER, APF2ZF_SLOPE_12DBOCT);
                break;
            case TYPE_12DB_LINK:
                filter_type = APF2ZF(hl, APF2ZF_APFTYPE_LINK, APF2ZF_SLOPE_12DBOCT);
                break;
            case TYPE_18DB_BESSEL:
                filter_type = APF2ZF(hl, APF2ZF_APFTYPE_BESSEL, APF2ZF_SLOPE_18DBOCT);
                break;
            case TYPE_18DB_BUTTER:
                filter_type = APF2ZF(hl, APF2ZF_APFTYPE_BUTTER, APF2ZF_SLOPE_18DBOCT);
                break;
            case TYPE_24DB_BESSEL:
                filter_type = APF2ZF(hl, APF2ZF_APFTYPE_BESSEL, APF2ZF_SLOPE_24DBOCT);
                break;
            case TYPE_24DB_BUTTER:
                filter_type = APF2ZF(hl, APF2ZF_APFTYPE_BUTTER, APF2ZF_SLOPE_24DBOCT);
                break;
            case TYPE_24DB_LINK:
                filter_type = APF2ZF(hl, APF2ZF_APFTYPE_LINK, APF2ZF_SLOPE_24DBOCT);
                break;
            default:
                disable = 1;
                filter_type = BIQUAD_PEAKNOTCH;
                break;
        }

        x_eq_subband_para(eq_x, band, EQ_FILTER_TYPE, (void*)&filter_type, set);
        x_eq_subband_para(eq_x, band, EQ_FC, (void*)&setting->fc, set);
        x_eq_subband_para(eq_x, band, EQ_DISABLE, (void*)&disable, set);

        uint8_t *eq_buf = get_eq_param_buff();
        eq_data ed   = {0};
        uint8_t cnt  = 0;

        ed.page_id    = page_id;
        ed.item_index = band;
        ed.item_type  = TYPE_EQ_PRE_SETTING;
        ed.fill.clbk  = AFX_PSAL_HOOK;
        libc_memcpy(eq_buf + 8*cnt++, &ed, sizeof(ed));

        on_set_eq_data(eq_buf, cnt*8);
    }

    return EQ_SET_NO_ERR;
}

BASIC_AFX_FUNC EQ_SETTING_STU on_x_eq_preamp_for_pageid(PageClass page_id, float* preamp, bool set)
{
    if(page_id == PAGE_EFFECT)
    {
        return EQ_SET_PATCH_WELCOME;
    }

    if(set)
    {
        uint8_t *eq_buf = get_eq_param_buff();
        eq_data ed   = {0};
        uint8_t cnt  = 0;

        ed.page_id    = page_id;
        ed.item_type  = TYPE_INPUT_VOL;
        ed.fill.data  = *preamp>12.0?12.0:(*preamp<-12.0?-12.0:*preamp);
        libc_memcpy(eq_buf + 8*cnt++, &ed, sizeof(ed));

        on_set_eq_data(eq_buf, cnt*8);
    }
    else
    {
        void *left_preamp = getLeftChannelEqPreampForPageId(page_id);
        void *right_preamp = getRightChannelEqPreampForPageId(page_id);

        if(!left_preamp && !right_preamp) {
            return EQ_XET_NONE_RESPONSE;
        }

        x_eq_preamp_para(left_preamp, EQ_PREAMP_MAIN_FUNC, EQ_PREAMP_SBPARA_GAIN, preamp, set);


        if(PAGE_MUSIC != page_id || set)
        {
            x_eq_preamp_para(right_preamp, EQ_PREAMP_MAIN_FUNC, EQ_PREAMP_SBPARA_GAIN, preamp, set);
        }
    }
    return EQ_SET_NO_ERR;
}

#define ECHO_PARA_GFBC_OFFSET            (uint32_t)(&((echo_setting*)0)->gFbc)
#define ECHO_PARA_GDRY_OFFSET            (uint32_t)(&((echo_setting*)0)->gDry)
#define ECHO_PARA_GWET_OFFSET            (uint32_t)(&((echo_setting*)0)->gWet)
#define ECHO_PARA_DLY_OFFSET             (uint32_t)(&((echo_setting*)0)->dly)
#define ECHO_PARA_NEED_LP_OFFSET         (uint32_t)(&((echo_setting*)0)->need_lp)
#define ECHO_PARA_CUFFEQ_OFFSET          (uint32_t)(&((echo_setting*)0)->cutoff_freq)
#define ECHO_PARA_SW_OFFSET              (uint32_t)(&((echo_setting*)0)->enable)

const uint8_t echo_setting_offset_map[ECHO_PARA_NUM] = {
    ECHO_PARA_GFBC_OFFSET,
    ECHO_PARA_GDRY_OFFSET,
    ECHO_PARA_GWET_OFFSET,
    ECHO_PARA_DLY_OFFSET,
    ECHO_PARA_NEED_LP_OFFSET,
    ECHO_PARA_CUFFEQ_OFFSET,
    ECHO_PARA_SW_OFFSET
};

BASIC_AFX_FUNC EQ_SETTING_STU on_x_echo_for_pageid(PageClass page_id, uint32_t mask, echo_setting* setting, bool set)
{
    if(page_id != PAGE_EFFECT)
        return EQ_SET_PATCH_WELCOME;

    if(set) echo_setting_check(setting);

    void *er = getEchoAddrForPageId(page_id);
    if(!er) {
        return EQ_XET_NONE_RESPONSE;
    }

    for(int m = ECHO_PARA_START; m <= ECHO_PARA_END; m++) {
        if(mask & COMMON_PARAS_MASK(m)) {
            char* addr = ((char*)setting) + echo_setting_offset_map[m];
            x_echo_sunfunc_para(er, ECHO_MAIN_FUNC, m, (void*)addr, set);
        }
    }

    if(set)
    {
        uint8_t *eq_buf = get_eq_param_buff();
        eq_data ed   = {0};
        uint8_t cnt  = 0;

        ed.page_id    = page_id;
        ed.item_type  = TYPE_ECHO_PRE_SETTING;
        ed.fill.clbk  = AFX_PSAL_HOOK;
        libc_memcpy(eq_buf + 8*cnt++, &ed, sizeof(ed));

        on_set_eq_data(eq_buf, cnt*8);
    }
    return EQ_SET_NO_ERR;
}

#define TSPS_PARA_TS_OFFSET  (uint32_t)(&((tsps_setting*)0)->alpha_ts)
#define TSPS_PARA_PS_OFFSET  (uint32_t)(&((tsps_setting*)0)->alpha_ps)
#define TSPS_PARA_SW_OFFSET  (uint32_t)(&((tsps_setting*)0)->enable)

const uint8_t tsps_setting_offset_map[TSPS_PARA_NUM] = {
    TSPS_PARA_TS_OFFSET,
    TSPS_PARA_PS_OFFSET,
    TSPS_PARA_SW_OFFSET,
};

BASIC_AFX_FUNC EQ_SETTING_STU on_x_tsps_for_pageid(PageClass page_id, uint32_t mask, tsps_setting* setting, bool set)
{
    if(page_id != PAGE_MUSIC &&
        page_id != PAGE_MIC)
        return EQ_SET_PATCH_WELCOME;

    if(set) tsps_setting_check(setting);

    void *tr = getTspsAtuneAddrForPageId(page_id);
    if(!tr) {
        return EQ_XET_NONE_RESPONSE;
    }

    for(int m = TSPS_PARA_START; m <= TSPS_PARA_END; m++) {
        if(mask & COMMON_PARAS_MASK(m)) {
            char* addr = ((char*)setting) + tsps_setting_offset_map[m];
            x_tsps_sunfunc_para(tr, TSPS_MAIN_FUNC, m, (void*)addr, set);
        }
    }

    if(set)
    {
        uint8_t *eq_buf = get_eq_param_buff();
        eq_data ed   = {0};
        uint8_t cnt  = 0;

        ed.page_id    = page_id;
        ed.item_type  = TYPE_TSPS_ATUNE_PRE_SETTING;
        ed.fill.clbk  = AFX_PSAL_HOOK;
        libc_memcpy(eq_buf + 8*cnt++, &ed, sizeof(ed));

        on_set_eq_data(eq_buf, cnt*8);
    }
    return EQ_SET_NO_ERR;
}

#define ATUNE_PARA_NOTE_OFFSET  (uint32_t)(&((atune_setting*)0)->note)
#define ATUNE_PARA_OCT_OFFSET   (uint32_t)(&((atune_setting*)0)->oct)
#define ATUNE_PARA_SCALE_OFFSET (uint32_t)(&((atune_setting*)0)->scale)
#define ATUNE_PARA_SPEED_OFFSET (uint32_t)(&((atune_setting*)0)->speed)
#define ATUNE_PARA_SW_OFFSET    (uint32_t)(&((atune_setting*)0)->enable)

const uint8_t atune_setting_offset_map[ATUNE_PARA_NUM] = {
    ATUNE_PARA_NOTE_OFFSET,
    ATUNE_PARA_OCT_OFFSET,
    ATUNE_PARA_SCALE_OFFSET,
    ATUNE_PARA_SPEED_OFFSET,
    ATUNE_PARA_SW_OFFSET
};

BASIC_AFX_FUNC EQ_SETTING_STU on_x_atune_for_pageid(PageClass page_id, uint32_t mask, atune_setting* setting, bool set)
{
    if(page_id != PAGE_MIC)
        return EQ_SET_PATCH_WELCOME;

    void *ar = getTspsAtuneAddrForPageId(page_id);
    if(!ar ) {
        return EQ_XET_NONE_RESPONSE;
    }

    for(int m = ATUNE_PARA_START; m <= ATUNE_PARA_END; m++) {
        if(mask & COMMON_PARAS_MASK(m)) {
            char* addr = ((char*)setting) + atune_setting_offset_map[m];
            x_atune_sunfunc_para(ar , ATUNE_MAIN_FUNC, m, (void*)addr, set);
        }
    }

    if(set)
    {
        uint8_t *eq_buf = get_eq_param_buff();
        eq_data ed   = {0};
        uint8_t cnt  = 0;

        ed.page_id    = page_id;
        ed.item_type  = TYPE_TSPS_ATUNE_PRE_SETTING;
        ed.fill.clbk  = AFX_PSAL_HOOK;
        libc_memcpy(eq_buf + 8*cnt++, &ed, sizeof(ed));

        on_set_eq_data(eq_buf, cnt*8);
    }
    return EQ_SET_NO_ERR;
}

#define FC_PARA_FC_OFFSET  (uint32_t)(&((fc_setting*)0)->alpha_fc)
#define FC_PARA_SW_OFFSET  (uint32_t)(&((fc_setting*)0)->enable)

const uint8_t fc_setting_offset_map[FC_PARA_NUM] = {
    FC_PARA_FC_OFFSET,
    FC_PARA_SW_OFFSET,
};

BASIC_AFX_FUNC EQ_SETTING_STU on_x_formant_for_pageid(PageClass page_id, uint32_t mask, fc_setting* setting, bool set)
{
    if(page_id != PAGE_MIC)
        return EQ_SET_PATCH_WELCOME;

    if(set) fc_setting_check(setting);

    void *fr = getFcAddrForPageId(page_id);
    if(!fr ) {
        return EQ_XET_NONE_RESPONSE;
    }

    for(int m = FC_PARA_START; m <= FC_PARA_END; m++) {
        if(mask & COMMON_PARAS_MASK(m)) {
            char* addr = ((char*)setting) + fc_setting_offset_map[m];
            x_fc_sunfunc_para(fr , FC_MAIN_FUNC, m, (void*)addr, set);
        }
    }

    if(set)
    {
        uint8_t *eq_buf = get_eq_param_buff();
        eq_data ed   = {0};
        uint8_t cnt  = 0;

        ed.page_id    = page_id;
        ed.item_type  = TYPE_FC_PRE_SETTING;
        ed.fill.clbk  = AFX_PSAL_HOOK;
        libc_memcpy(eq_buf + 8*cnt++, &ed, sizeof(ed));

        on_set_eq_data(eq_buf, cnt*8);
    }
    return EQ_SET_NO_ERR;
}

BASIC_AFX_FUNC EQ_SETTING_STU on_x_dr_switch_for_pageid(PageClass page_id, bool* on_off, bool set)
{
    if(page_id != PAGE_OUTPUT1 &&
        page_id != PAGE_OUTPUT2)
        return EQ_SET_PATCH_WELCOME;

    if(set)
    {
        uint8_t *eq_buf = get_eq_param_buff();
        eq_data ed   = {0};
        uint8_t cnt  = 0;

        ed.page_id    = page_id;
        ed.item_type  = TYPE_DR_SWITCH;
        ed.fill.data  = *on_off?1.0:0.0;
        libc_memcpy(eq_buf + 8*cnt++, &ed, sizeof(ed));

        on_set_eq_data(eq_buf, cnt*8);
    }
    else
    {
        void *dr_addr = getDrAddrForPageId(page_id);
        if(!dr_addr) {
            return EQ_XET_NONE_RESPONSE;
        }
        *on_off = getDrSwitchStu(dr_addr);
    }
    return EQ_SET_NO_ERR;
}

BASIC_AFX_FUNC EQ_SETTING_STU on_x_dr_setting_for_pageid(PageClass page_id, dly_setting* setting, bool set)
{
    if(page_id != PAGE_OUTPUT1 &&
        page_id != PAGE_OUTPUT2)
        return EQ_SET_PATCH_WELCOME;

    if(set) dr_setting_check(setting);

    void *dr = getDrAddrForPageId(page_id);
    if(!dr) {
        return EQ_XET_NONE_RESPONSE;
    }
    x_dr_sunfunc_para(dr, DR_MAIN_FUNC, DR_REV_FACTOR, (void*)&setting->rev_factor, set);
    x_dr_sunfunc_para(dr, DR_MAIN_FUNC, DR_RTDLY, (void*)&setting->rtdly, set);

    if(set)
    {
        uint8_t *eq_buf = get_eq_param_buff();
        eq_data ed   = {0};
        uint8_t cnt  = 0;

        ed.page_id    = page_id;
        ed.item_type  = TYPE_DR_PRE_SETTING;
        ed.fill.clbk  = AFX_PSAL_HOOK;
        libc_memcpy(eq_buf + 8*cnt++, &ed, sizeof(ed));

        on_set_eq_data(eq_buf, cnt*8);
    }
    return EQ_SET_NO_ERR;
}

BASIC_AFX_FUNC EQ_SETTING_STU on_x_drc_makeup_gain_for_pageid(PageClass page_id, float* makeup, bool set)
{
    if(set)
    {
        if(page_id != PAGE_OUTPUT1 && page_id != PAGE_OUTPUT2)
        {
            return EQ_SET_PATCH_WELCOME;
        }

        uint8_t *eq_buf = get_eq_param_buff();
        eq_data ed   = {0};
        uint8_t cnt  = 0;

        ed.page_id    = page_id;
        ed.item_type  = TYPE_DRC_MAKEUP_GAIN;
        ed.fill.data  = *makeup>30.0?30.0:(*makeup<-10.0?-10.0:*makeup);
        libc_memcpy(eq_buf + 8*cnt++, &ed, sizeof(ed));

        on_set_eq_data(eq_buf, cnt*8);
    }
    else
    {
        void* drc_addr = getDrcAddrForPageId(page_id);
        if(!drc_addr) {
            return EQ_XET_NONE_RESPONSE;
        }
        *makeup = getDrcMakeupGain(drc_addr);
    }

    return EQ_SET_NO_ERR;
}

#define DRC_SUBFUNC_X_THD_OFFSET  (uint32_t)(&((drc_subfunc_setting*)0)->x_thd)
#define DRC_SUBFUNC_X_RADIO_OFFSET  (uint32_t)(&((drc_subfunc_setting*)0)->x_ratio)
#define DRC_SUBFUNC_X_ATT_OFFSET  (uint32_t)(&((drc_subfunc_setting*)0)->x_att)
#define DRC_SUBFUNC_X_REL_OFFSET  (uint32_t)(&((drc_subfunc_setting*)0)->x_rel)
#define DRC_SUBFUNC_X_SW_OFFSET  (uint32_t)(&((drc_subfunc_setting*)0)->enable)

const uint8_t drc_setting_offset_map[DRC_SFPARA_NUM] = {
    DRC_SUBFUNC_X_THD_OFFSET,
    DRC_SUBFUNC_X_ATT_OFFSET,
    DRC_SUBFUNC_X_RADIO_OFFSET,
    DRC_SUBFUNC_X_REL_OFFSET,
    DRC_SUBFUNC_X_SW_OFFSET,
};

BASIC_AFX_FUNC EQ_SETTING_STU on_x_drc_subfunc_for_pageid(PageClass page_id, uint32_t mask, DRC_SUBFUNC drc_sb, drc_subfunc_setting* setting, bool set)
{
    if(page_id == PAGE_MUSIC && drc_sb != DRC_NOSIEGATE)
        return EQ_SET_PATCH_WELCOME;

    if(set) drc_subfunc_setting_check(setting);

    void* drc_addr = 0;
    if(PAGE_VSS == page_id) {
        drc_addr = getVssXAddrForPageId(PAGE_VSS, VSS_DRC);
    }
    else {
        drc_addr = getDrcAddrForPageId(page_id);
    }

    if(!drc_addr)
    {
        return EQ_XET_NONE_RESPONSE;
    }

    for(int m = DRC_SFPARA_START; m <= DRC_SFPARA_END; m++) {
        if(mask & COMMON_PARAS_MASK(m)) {
            char* addr = ((char*)setting) + drc_setting_offset_map[m];
            x_drc_sunfunc_para(drc_addr , drc_sb, m, (void*)addr, set);
        }
    }

    if(set)
    {
        uint8_t *eq_buf = get_eq_param_buff();
        eq_data ed   = {0};
        uint8_t cnt  = 0;

        ed.page_id    = page_id;
        ed.item_type  = TYPE_DRC_PRE_SETTING;
        ed.fill.clbk  = AFX_PSAL_HOOK;
        libc_memcpy(eq_buf + 8*cnt++, &ed, sizeof(ed));

        on_set_eq_data(eq_buf, cnt*8);
    }
    return EQ_SET_NO_ERR;
}

#define REVB_PARA_TIME_OFFSET            (uint32_t)(&((rev_setting*)0)->revb_time)
#define REVB_PARA_DAMP_FREQ_LO_OFFSET    (uint32_t)(&((rev_setting*)0)->damp_freq_lo)
#define REVB_PARA_DAMP_FREQ_HI_OFFSET    (uint32_t)(&((rev_setting*)0)->damp_freq_hi)
#define REVB_PARA_LPF_FREQ_OFFSET        (uint32_t)(&((rev_setting*)0)->lpf_freq)
#define REVB_PARA_HPF_FREQ_OFFSET        (uint32_t)(&((rev_setting*)0)->hpf_freq)
#define REVB_PARA_PRE_DLY_OFFSET         (uint32_t)(&((rev_setting*)0)->pre_delay)
#define REVB_PARA_DIFF_RATIO_OFFSET      (uint32_t)(&((rev_setting*)0)->diff)
#define REVB_PARA_DAMP_LO_RATIO_OFFSET   (uint32_t)(&((rev_setting*)0)->damp_ratio_lo)
#define REVB_PARA_DAMP_HI_RATIO_OFFSET   (uint32_t)(&((rev_setting*)0)->damp_ratio_hi)
#define REVB_PARA_GAIN_WET_OFFSET        (uint32_t)(&((rev_setting*)0)->g_wet)
#define REVB_PARA_GAIN_DRY_OFFSET        (uint32_t)(&((rev_setting*)0)->g_dry)
#define REVB_PARA_ENABLE_OFFSET          (uint32_t)(&((rev_setting*)0)->enable)
#define REVB_PARA_M2S_OFFSET             (uint32_t)(&((rev_setting*)0)->m2s)

const uint8_t rev_setting_offset_map[REVB_PARA_NUM] = {
    REVB_PARA_TIME_OFFSET,
    REVB_PARA_DAMP_FREQ_LO_OFFSET,
    REVB_PARA_DAMP_FREQ_HI_OFFSET,
    REVB_PARA_LPF_FREQ_OFFSET,
    REVB_PARA_HPF_FREQ_OFFSET,
    REVB_PARA_PRE_DLY_OFFSET,
    REVB_PARA_DIFF_RATIO_OFFSET,
    REVB_PARA_DAMP_LO_RATIO_OFFSET,
    REVB_PARA_DAMP_HI_RATIO_OFFSET,
    REVB_PARA_GAIN_WET_OFFSET,
    REVB_PARA_GAIN_DRY_OFFSET,
    REVB_PARA_ENABLE_OFFSET,
    REVB_PARA_M2S_OFFSET
};

BASIC_AFX_FUNC EQ_SETTING_STU on_x_rev_for_pageid(PageClass page_id, uint32_t mask, rev_setting* setting, bool set)
{
    if(page_id != PAGE_EFFECT)
    {
        return EQ_SET_PATCH_WELCOME;
    }

    if(!setting)
    {
        return EQ_SET_INVALID_STU;
    }

    if(set)
    {
        rerb_setting_check(setting);
    }

    void *rr = getRevbAddrForPageId(page_id);
    if(!rr)
    {
        return EQ_XET_NONE_RESPONSE;
    }

    for(int m = REVB_PARA_START; m <= REVB_PARA_END; m++) {
        if(mask & COMMON_PARAS_MASK(m)) {
            char* addr = ((char*)setting) + rev_setting_offset_map[m];
            x_revb_sunfunc_para(rr, REVB_MAIN_FUNC, m, (void*)addr, set);
        }
    }

    if(set)
    {
        uint8_t *eq_buf = get_eq_param_buff();
        eq_data ed   = {0};
        uint8_t cnt  = 0;

        ed.page_id    = page_id;
        ed.item_type  = TYPE_REVERB_PRE_SETTING;
        ed.fill.clbk  = AFX_PSAL_HOOK;
        libc_memcpy(eq_buf + 8*cnt++, &ed, sizeof(ed));

        on_set_eq_data(eq_buf, cnt*8);
    }
    return EQ_SET_NO_ERR;

}

BASIC_AFX_FUNC EQ_SETTING_STU on_x_vss_switch_for_pageid(PageClass page_id, bool* on_off, bool set)
{
    if(page_id != PAGE_VSS)
    {
        return EQ_SET_PATCH_WELCOME;
    }

    void *vss = getVssXAddrForPageId(page_id, VSS_MAIN);
    if(!vss)
    {
        return EQ_XET_NONE_RESPONSE;
    }

    if(set)
    {
        uint8_t *eq_buf = get_eq_param_buff();
        eq_data ed   = {0};
        uint8_t cnt  = 0;

        ed.page_id    = page_id;
        ed.item_type  = TYPE_VSS_SWITCH;
        ed.fill.data  = *on_off?1.0:0.0;
        libc_memcpy(eq_buf + 8*cnt++, &ed, sizeof(ed));

        on_set_eq_data(eq_buf, cnt*8);
    }
    else
    {
        x_vss_sunfunc_para(vss, VSS_MAIN_FUNC, VSS_PARA_SWITCH, (void*)on_off, EQ_GET_DATA);
    }
    return EQ_SET_NO_ERR;
}

BASIC_AFX_FUNC EQ_SETTING_STU on_x_vss_pro_for_pageid(PageClass page_id, vss_mode* mode, vss_pro_setting* setting, bool set)
{
    if(page_id != PAGE_VSS)
    {
        return EQ_SET_PATCH_WELCOME;
    }

    if(!setting)
    {
        return EQ_SET_INVALID_STU;
    }

    if(set)
    {
        vss_setting_check(setting);
    }

    void *vss = getVssXAddrForPageId(page_id, VSS_MAIN);
    if(!vss)
    {
        return EQ_XET_NONE_RESPONSE;
    }

    if(VSS_MODE_HEADPHONE == *mode) {
        x_vss_sunfunc_para(vss, VSS_MAIN_FUNC, VSS_PARA_HP_SYNTHESIS_WIDTH, (void*)&setting->synth, set);
        x_vss_sunfunc_para(vss, VSS_MAIN_FUNC, VSS_PARA_HP_DEPTH_WIDTH, (void*)&setting->sound_field_width.hp_depth, set);
        x_vss_sunfunc_para(vss, VSS_MAIN_FUNC, VSS_PARA_HP_SPATIAL_INTENSITY, (void*)&setting->sp_intensity, set);
        x_vss_sunfunc_para(vss, VSS_MAIN_FUNC, VSS_PARA_HP_BASS_INTENSITY, (void*)&setting->bass_intensity, set);
        x_vss_sunfunc_para(vss, VSS_MAIN_FUNC, VSS_PARA_HP_DEFINITION, (void*)&setting->definition, set);
        x_vss_sunfunc_para(vss, VSS_MAIN_FUNC, VSS_PARA_HP_BASS_COFREQ, (void*)&setting->bass_cofreq, set);
    }
    else {
        x_vss_sunfunc_para(vss, VSS_MAIN_FUNC, VSS_PARA_SPK_SYNTHESIS_WIDTH, (void*)&setting->synth, set);
        x_vss_sunfunc_para(vss, VSS_MAIN_FUNC, VSS_PARA_SPK_SPAN_WIDTH, (void*)&setting->sound_field_width.spk_span, set);
        x_vss_sunfunc_para(vss, VSS_MAIN_FUNC, VSS_PARA_SPK_SPATIAL_INTENSITY, (void*)&setting->sp_intensity, set);
        x_vss_sunfunc_para(vss, VSS_MAIN_FUNC, VSS_PARA_SPK_BASS_INTENSITY, (void*)&setting->bass_intensity, set);
        x_vss_sunfunc_para(vss, VSS_MAIN_FUNC, VSS_PARA_SPK_DEFINITION, (void*)&setting->definition, set);
        x_vss_sunfunc_para(vss, VSS_MAIN_FUNC, VSS_PARA_SPK_BASS_COFREQ, (void*)&setting->bass_cofreq, set);
    }

    x_vss_sunfunc_para(vss, VSS_MAIN_FUNC, VSS_PARA_MODE_SWITCH, (void*)mode, set);

    if(set)
    {
        uint8_t *eq_buf = get_eq_param_buff();
        eq_data ed   = {0};
        uint8_t cnt  = 0;

        ed.page_id    = page_id;
        ed.item_type  = TYPE_VSS_PRE_SETTING;
        ed.fill.clbk  = AFX_PSAL_HOOK;
        libc_memcpy(eq_buf + 8*cnt++, &ed, sizeof(ed));

        on_set_eq_data(eq_buf, cnt*8);
    }
    return EQ_SET_NO_ERR;

}


