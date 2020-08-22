#define HIGH_LVL_API_DRC_FUNC __attribute__((section(".high_lvl_drc_seg")))
#define DRC_PARA_DECIEDED_BY_DRIVER 0

drc_subfunc_setting natual_noise_gate = {
    -40,
      3,
      3,
     50,
     DRC_PARA_DECIEDED_BY_DRIVER
};

drc_subfunc_setting* drc_subfunc_focus      = &natual_noise_gate;
int8_t               drc_subfunc_name_focus = DRC_NOSIEGATE;
int8_t               drc_subfunc_page_focus = PAGE_MIC;

static void natural_noise_gate_get_focus()
{
    drc_subfunc_focus      = &natual_noise_gate;
    drc_subfunc_name_focus = DRC_NOSIEGATE;
    drc_subfunc_page_focus = PAGE_MIC;
}

static void common_drc_subfunc_setting()
{
    on_x_drc_subfunc_for_pageid(drc_subfunc_page_focus, DRC_SUBFUNC_ALL_MASK, drc_subfunc_name_focus, drc_subfunc_focus, EQ_SET_DATA);
}

HIGH_LVL_API_DRC_FUNC void on_set_natural_noise_gate(EFFECT_SWTICH sw, bool hold)
{
    bool on = (sw == ESWITCH_SOFT_ON || sw == ESWITCH_HARD_ON);
    if(sw == ESWITCH_HARD_ON) {
        reset_all_effects();
    }

    setDrcHolderForPageId(hold, drc_subfunc_page_focus);

    natural_noise_gate_get_focus();

    drc_subfunc_focus->enable = on;

    common_drc_subfunc_setting();
}

