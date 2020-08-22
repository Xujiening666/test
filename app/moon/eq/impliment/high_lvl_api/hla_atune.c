#define HIGH_LVL_API_ATUNE_FUNC __attribute__((section(".high_lvl_api_atune_seg")))
#define ATUNE_PARA_DECIDED_BY_DRIVER 0

#define DEFAULT_ATUNE_KEY           ATUNE_KEY_C
#define DEFAULT_ATUNE_SCALE         ATUNE_SCALE_MAJOR

atune_setting default_atune = {
    DEFAULT_ATUNE_KEY,
    ATUNE_OCT_4,
    DEFAULT_ATUNE_SCALE,
    10,
    ATUNE_PARA_DECIDED_BY_DRIVER
};

atune_setting* atune_focus       = &default_atune;

static void default_atune_get_focus()
{
    atune_focus       = &default_atune;
}

static void common_atune_setting()
{
    uint32_t atune_mask = ATUNE_ALL_PAPAS_MASK & COMMON_PARAS_MASK_EXCLUDE(ATUNE_ENABLE);
    on_x_atune_for_pageid(PAGE_MIC, atune_mask, atune_focus, EQ_SET_DATA);
}

HIGH_LVL_API_ATUNE_FUNC void atune_listener(void* user, uint32_t cmd, uint32_t arg)
{
    switch(cmd) {
        case FOCUS_CMD_COMMIT_SUICIDE:
        {
            atune_focus->enable = false;
            uint32_t atune_mask = COMMON_PARAS_MASK(ATUNE_ENABLE);
            on_x_atune_for_pageid(PAGE_MIC, atune_mask, atune_focus, EQ_SET_DATA);
            break;
        }
    }
}

static void atune_switch(bool on)
{
    atune_tsps_fc_get_focus(atune_listener);

    tsps_focus->enable = on;
    on_x_tsps_for_pageid(PAGE_MIC, COMMON_PARAS_MASK(TSPS_ENABLE), tsps_focus, EQ_SET_DATA);
    on_x_formant_for_pageid(PAGE_MIC, COMMON_PARAS_MASK(FC_ENABLE), fc_focus, EQ_SET_DATA);

    atune_focus->enable = on;
    uint32_t atune_mask = COMMON_PARAS_MASK(ATUNE_ENABLE);
    on_x_atune_for_pageid(PAGE_MIC, atune_mask, atune_focus, EQ_SET_DATA);
}

HIGH_LVL_API_ATUNE_FUNC void on_set_autotune_profile(ATUNE_CMD cmd, uint8_t val)
{
    RECORD_SLEEP;

    default_atune_get_focus();

    switch(cmd){
        case ATUNE_SET_SWITCH:
            if(ESWITCH_HARD_ON == val) {
                reset_all_effects();
            }
            atune_switch(val);
            break;
        case ATUNE_SET_KEY:
            atune_focus->note = val;
            break;
        case ATUNE_SET_SCALE:
            atune_focus->scale = val;
            break;
        default:
            break;
    }

    common_atune_setting();

    RECORD_WAKEUP_FOR_MS(200);
}


