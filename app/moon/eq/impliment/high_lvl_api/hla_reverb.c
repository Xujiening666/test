#define HIGH_LVL_API_REVB_FUNC __attribute__((section(".high_lvl_api_revb_seg")))
#define REVERB_PARA_DECIDED_BY_DRIVER 0

//---------  Default reverb setting area start -----
#define DEFAULT_REVB_MAX_GWET   0.5f
#define DEFAULT_REVB_MAX_RT60   2750
rev_setting default_reverb = {
    DEFAULT_REVB_MAX_RT60,//revb_time;
    20,//damp_freq_lo;
    5500,//damp_freq_hi;
    10000,//lpf_freq;
    20,//hpf_freq;
    100,//predelay in ms
    0.6,//diff;
    0.52,//damp_ratio_lo;
    0.3,//damp_ratio_hi;
    DEFAULT_REVB_MAX_GWET,//g_wet;
    1.0,//0.80,//g_dry;
    REVERB_PARA_DECIDED_BY_DRIVER,
    REVERB_PARA_DECIDED_BY_DRIVER
};
//---------  Default reverb setting area end    -----

rev_setting* revb_focus          = &default_reverb;
float        revb_max_gwet_focus = DEFAULT_REVB_MAX_GWET;
int16_t      revb_max_rt60_focus = DEFAULT_REVB_MAX_RT60;

static void default_reverb_get_focus()
{
    revb_focus          = &default_reverb;
    revb_max_gwet_focus = DEFAULT_REVB_MAX_GWET;
    revb_max_rt60_focus = DEFAULT_REVB_MAX_RT60;
}

static void reverb_common_setting()
{
    uint32_t revb_mask = REVERB_ALL_PAPAS_MASK & COMMON_PARAS_MASK_EXCLUDE(REVB_ENABLE) & COMMON_PARAS_MASK_EXCLUDE(REVB_MS_STU);
    on_x_rev_for_pageid(PAGE_EFFECT, revb_mask, revb_focus, EQ_SET_DATA);
}

static void reverb_ms_switch(bool stereo)
{
    uint32_t revb_mask = COMMON_PARAS_MASK(REVB_MS_STU);
    revb_focus->m2s = stereo;
    on_x_rev_for_pageid(PAGE_EFFECT, revb_mask, revb_focus, EQ_SET_DATA);
}

static void reverb_enable()
{
    uint32_t revb_mask = COMMON_PARAS_MASK(REVB_ENABLE);
    on_x_rev_for_pageid(PAGE_EFFECT, revb_mask, revb_focus, EQ_GET_DATA);
    if(!revb_focus->enable) {
        revb_focus->enable = true;
        on_x_rev_for_pageid(PAGE_EFFECT, revb_mask, revb_focus, EQ_SET_DATA);
    }
}

HIGH_LVL_API_REVB_FUNC void on_set_default_reverb_profile(bool holdRevb, bool onlyRevb)
{
    RECORD_SLEEP;

    default_reverb_get_focus();

    if(onlyRevb) {
        reset_all_effects();
    }

    setRevbHolderForPageId(holdRevb, PAGE_EFFECT);

    reverb_common_setting();
    reverb_enable();

    RECORD_WAKEUP_FOR_MS(200);
}

HIGH_LVL_API_REVB_FUNC void on_set_rec_reverb_ms_switch(bool stereo)
{
    reverb_ms_switch(stereo);
}

HIGH_LVL_API_REVB_FUNC void on_set_reverb_time(uint8_t time)
{
    int low = 300, high = revb_max_rt60_focus;

    time = time > 100?100:(time < 0?0:time);
    revb_focus->revb_time = low + (int)((float)(high - low)*(float)time/100.0f);

    uint32_t revb_mask = COMMON_PARAS_MASK(REVB_PARA_TIME);
    on_x_rev_for_pageid(PAGE_EFFECT, revb_mask, revb_focus, EQ_SET_DATA);
}

HIGH_LVL_API_REVB_FUNC void on_set_reverb_gwet(uint8_t g)
{
    g = g > 100?100:(g < 0?0:g);
    revb_focus->g_wet = (float)revb_max_gwet_focus*(float)g/100.0f;

    uint32_t revb_mask = COMMON_PARAS_MASK(REVB_PARA_GAIN_WET);
    on_x_rev_for_pageid(PAGE_EFFECT, revb_mask, revb_focus, EQ_SET_DATA);
}

