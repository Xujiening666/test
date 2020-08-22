#define HIGH_LVL_API_ECHO_FUNC __attribute__((section(".high_lvl_api_echo_seg")))
#define ECHO_PARA_DECIDED_BY_DRIVER 0

#define DEFAULT_ECHO_MAX_GWET   1.0f
#define DEFAULT_ECHO_MAX_DLY    200

echo_setting default_echo = {
    0.85,//g;
    0.54,//gDry;
    DEFAULT_ECHO_MAX_GWET,//gWet;
    DEFAULT_ECHO_MAX_DLY,//dly;
    1,//need_flt;
    2890,//cutoff_freq;
    ECHO_PARA_DECIDED_BY_DRIVER
};

#define ROBOT_ECHO_MAX_GWET     1.0f
#define ROBOT_ECHO_MAX_DLY      24

echo_setting robot_echo = {
    0.85,//g;
    0.54,//gDry;
    ROBOT_ECHO_MAX_GWET,//gWet;
    ROBOT_ECHO_MAX_DLY,//dly;
    1,//need_flt;
    2890,//cutoff_freq
    ECHO_PARA_DECIDED_BY_DRIVER
};

#define FEMALE_KID_ECHO_MAX_GWET  0.6f
#define FEMALE_KID_ECHO_MAX_DLY   92

echo_setting female_kid_echo = {
    0.3,//g;
    0.5,//gDry;
    FEMALE_KID_ECHO_MAX_GWET,//gWet;
    FEMALE_KID_ECHO_MAX_DLY,//dly;
    1,//need_flt;
    3500,//cutoff_freq;
    ECHO_PARA_DECIDED_BY_DRIVER
};

echo_setting* echo_focus          = &default_echo;
float         echo_max_gwet_focus = DEFAULT_ECHO_MAX_GWET;
uint16_t      echo_max_dly_focus  = DEFAULT_ECHO_MAX_DLY;

static void default_echo_get_focus()
{
    echo_focus          = &default_echo;
    echo_max_gwet_focus = DEFAULT_ECHO_MAX_GWET;
    echo_max_dly_focus  = DEFAULT_ECHO_MAX_DLY;
}

static void robot_echo_get_focus()
{
    echo_focus          = &robot_echo;
    echo_max_gwet_focus = ROBOT_ECHO_MAX_GWET;
    echo_max_dly_focus  = ROBOT_ECHO_MAX_DLY;
}

static void female_kid_echo_get_focus()
{
    echo_focus          = &female_kid_echo;
    echo_max_gwet_focus = FEMALE_KID_ECHO_MAX_GWET;
    echo_max_dly_focus  = FEMALE_KID_ECHO_MAX_DLY;
}

static void echo_common_setting()
{
    uint32_t echo_mask = ECHO_ALL_PAPAS_MASK & COMMON_PARAS_MASK_EXCLUDE(ECHO_ENABLE);
    on_x_echo_for_pageid(PAGE_EFFECT, echo_mask, echo_focus, EQ_SET_DATA);
}

static void echo_enable()
{
    uint32_t echo_mask = COMMON_PARAS_MASK(ECHO_ENABLE);
    on_x_echo_for_pageid(PAGE_EFFECT, echo_mask, echo_focus, EQ_GET_DATA);
    if(!echo_focus->enable) {
        echo_focus->enable = true;
        on_x_echo_for_pageid(PAGE_EFFECT, echo_mask, echo_focus, EQ_SET_DATA);
    }
}

static void default_echo_setting()
{
    default_echo_get_focus();
    echo_common_setting();
    echo_enable();
}

static void robot_echo_setting()
{
    robot_echo_get_focus();
    echo_common_setting();
    echo_enable();
}

static void female_kid_echo_setting()
{
    female_kid_echo_get_focus();
    echo_common_setting();
    echo_enable();
}

HIGH_LVL_API_ECHO_FUNC void on_set_default_echo_profile(bool holdEcho, bool cleanOtherAfx)
{
    if(cleanOtherAfx) {
        reset_all_effects();
    }

    setEchoHolderForPageId(holdEcho, PAGE_EFFECT);

    default_echo_setting();
}

HIGH_LVL_API_ECHO_FUNC void on_set_echo_delay(uint8_t dly)
{
    int low = 0, high = echo_max_dly_focus;

    dly = dly > 100?100:(dly < 0?0:dly);
    echo_focus->dly = low + (int)((float)(high - low)*(float)dly/100.0f);

    uint32_t echo_mask = COMMON_PARAS_MASK(ECHO_DLY);
    on_x_echo_for_pageid(PAGE_EFFECT, echo_mask, echo_focus, EQ_SET_DATA);
}

HIGH_LVL_API_ECHO_FUNC void on_set_echo_gwet(uint8_t g)
{
    g = g > 100?100:(g < 0?0:g);
    echo_focus->gWet = (float)echo_max_gwet_focus*(float)g/100.0f;

    uint32_t echo_mask = COMMON_PARAS_MASK(ECHO_GAIN_WET);
    on_x_echo_for_pageid(PAGE_EFFECT, echo_mask, echo_focus, EQ_SET_DATA);
}


