#define HIGH_LVL_API_VSS_FUNC __attribute__((section(".high_lvl_api_vss_seg")))
#define VSS_PARA_DECIDED_BY_DRIVER 0

vss_settings defualt_vss_common = {
    {
        { BIQUAD_PEAKNOTCH, 25,     0.0, 0.75, false },
        { BIQUAD_PEAKNOTCH, 300,    0.0, 0.75, false },
        { BIQUAD_PEAKNOTCH, 3000,   0.0, 0.75, false },
        { BIQUAD_PEAKNOTCH, 8000,   0.0, 0.75, false },
        { BIQUAD_PEAKNOTCH, 16000,  0.0, 0.75, false },
    },

    {
        -6, 32, 10, 200, true
    },

    {
        {
            3,
            { 1 },
            10,
            6,
            6,
            100
        },
        {
            3,
            { 5 },
            10,
            6,
            6,
            100,
        }
    },

    VSS_PARA_DECIDED_BY_DRIVER
};

vss_settings*        vss_common_focus = &defualt_vss_common;
int8_t               vss_mode_focus   = VSS_PARA_DECIDED_BY_DRIVER;
bool                 vss_switch_focus = VSS_PARA_DECIDED_BY_DRIVER;

static void default_vss_common_get_focus()
{
    vss_common_focus = &defualt_vss_common;
}

static void vss_common_setting()
{
    for(int b = 0; b < 5; b++)
    {
        on_x_eq_setting_for_pageid(PAGE_VSS, b, EQ_SB_ALL_PAPAS_MASK,
                                    &vss_common_focus->eq5[b], EQ_SET_DATA);
    }
    on_x_drc_subfunc_for_pageid(PAGE_VSS, DRC_SUBFUNC_ALL_MASK, DRC_LIMITER, &vss_common_focus->limiter, EQ_SET_DATA);
    on_x_vss_pro_for_pageid(PAGE_VSS, (vss_mode*)&vss_mode_focus, &vss_common_focus->hp_and_spk[vss_mode_focus], EQ_SET_DATA);
    on_x_vss_switch_for_pageid(PAGE_VSS, &vss_switch_focus, EQ_SET_DATA);
}


HIGH_LVL_API_VSS_FUNC void on_set_default_vss_hp_profile(EFFECT_SWTICH sw)
{
    if(sw == ESWITCH_HARD_ON) {
        reset_all_effects();
    }
    default_vss_common_get_focus();
    vss_mode_focus   = VSS_MODE_HEADPHONE;
    vss_switch_focus = (sw == ESWITCH_SOFT_ON || sw == ESWITCH_HARD_ON);
    vss_common_setting();
}

HIGH_LVL_API_VSS_FUNC void on_set_default_vss_spk_profile(EFFECT_SWTICH sw)
{
    if(sw == ESWITCH_HARD_ON) {
        reset_all_effects();
    }
    default_vss_common_get_focus();
    vss_mode_focus = VSS_MODE_SPEAKER;
    vss_switch_focus = (sw == ESWITCH_SOFT_ON || sw == ESWITCH_HARD_ON);
    vss_common_setting();
}



