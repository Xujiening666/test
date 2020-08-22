#define HIGH_LVL_API_TSPS_FUNC __attribute__((section(".high_lvl_api_tsps_fc_seg")))
#define TSPS_FC_PARA_DECIDED_BY_DRIVER 0
#define TSPS_TS_PARA_DECIDED_BT_DRIVER 1.0f
#define TSPS_PS_PARA_DECIDED_BT_DRIVER 1.0f

tsps_setting female_tsps = {
    (float)TSPS_TS_PARA_DECIDED_BT_DRIVER,
    (float)2.0,//2^(1.0)
    true
};

tsps_setting male_tsps = {
    (float)TSPS_TS_PARA_DECIDED_BT_DRIVER,
    (float)0.73713460864555060614578238386647,//2^(-0.44)
    true
};

tsps_setting robot_tsps = {
    (float)TSPS_TS_PARA_DECIDED_BT_DRIVER,
    (float)0.65975395538644712968700098561482,//2^(-0.6)
    true
};

tsps_setting kid_tsps = {
    (float)TSPS_TS_PARA_DECIDED_BT_DRIVER,
    (float)1.8403753012497501567808624764036,//2^(1.0)
    true
};

tsps_setting vc_tsps = {
    (float)TSPS_TS_PARA_DECIDED_BT_DRIVER,
    (float)1.569168195793501471449700633635,//2^(0.65)
    true
};

tsps_setting atune_tsps = {
    (float)TSPS_TS_PARA_DECIDED_BT_DRIVER,
    (float)TSPS_PS_PARA_DECIDED_BT_DRIVER,
    TSPS_FC_PARA_DECIDED_BY_DRIVER
};

fc_setting female_fc = {
    0.60,
    true
};

fc_setting none_fc = {
    1.0,
    false
};

tsps_setting* tsps_focus = &female_tsps;
fc_setting*   fc_focus   = &female_fc;
afx_listener  focus_listener = NULL;

typedef enum {
    FOCUS_CMD_NONE,
    FOCUS_CMD_COMMIT_SUICIDE,
    FOCUS_CMD_NUM
}FOCUS_LISTERN_CMD;

static void female_tsps_fc_get_focus()
{
    tsps_focus = &female_tsps;
    fc_focus   = &female_fc;
    if(focus_listener) {
        focus_listener(NULL, FOCUS_CMD_COMMIT_SUICIDE, 0);
    }
    focus_listener = NULL;
}

static void male_tsps_fc_get_focus()
{
    tsps_focus = &male_tsps;
    fc_focus   = &none_fc;
    if(focus_listener) {
        focus_listener(NULL, FOCUS_CMD_COMMIT_SUICIDE, 0);
    }
    focus_listener = NULL;
}

static void robot_tsps_fc_get_focus()
{
    tsps_focus = &robot_tsps;
    fc_focus   = &none_fc;
    if(focus_listener) {
        focus_listener(NULL, FOCUS_CMD_COMMIT_SUICIDE, 0);
    }
    focus_listener = NULL;
}

static void kid_tsps_fc_get_focus()
{
    tsps_focus = &kid_tsps;
    fc_focus   = &none_fc;
    if(focus_listener) {
        focus_listener(NULL, FOCUS_CMD_COMMIT_SUICIDE, 0);
    }
    focus_listener = NULL;
}

static void vc_tsps_fc_get_focus()
{
    tsps_focus = &vc_tsps;
    fc_focus   = &none_fc;
    if(focus_listener) {
        focus_listener(NULL, FOCUS_CMD_COMMIT_SUICIDE, 0);
    }
    focus_listener = NULL;
}

//Atune need a listern to
static void atune_tsps_fc_get_focus(afx_listener listener)
{
    tsps_focus = &atune_tsps;
    fc_focus   = &none_fc;
    if(focus_listener) {
        focus_listener(NULL, FOCUS_CMD_COMMIT_SUICIDE, 0);
    }
    focus_listener = listener;
}

static void tsps_fc_common_setting()
{
    on_x_tsps_for_pageid(PAGE_MIC, TSPS_ALL_PAPAS_MASK, tsps_focus, EQ_SET_DATA);
    on_x_formant_for_pageid(PAGE_MIC, FC_ALL_PAPAS_MASK, fc_focus, EQ_SET_DATA);
}

static void female_tsps_setting()
{
    female_tsps_fc_get_focus();
    tsps_fc_common_setting();
}

static void male_tsps_setting()
{
    male_tsps_fc_get_focus();
    tsps_fc_common_setting();
}

static void robot_tsps_setting()
{
    robot_tsps_fc_get_focus();
    tsps_fc_common_setting();
}

static void kid_tsps_setting()
{
    kid_tsps_fc_get_focus();
    tsps_fc_common_setting();
}

static void vc_tsps_setting()
{
    vc_tsps_fc_get_focus();
    tsps_fc_common_setting();
}

HIGH_LVL_API_TSPS_FUNC void on_set_female_profile(bool cleanOtherAfx)
{
    RECORD_SLEEP;

    if(cleanOtherAfx) {
        reset_all_effects();
    }

    female_tsps_setting();

    RECORD_WAKEUP_FOR_MS(200);
}

HIGH_LVL_API_TSPS_FUNC void on_set_male_profile(bool cleanOtherAfx)
{
    RECORD_SLEEP;

    if(cleanOtherAfx) {
        reset_all_effects();
    }

    male_tsps_setting();

    RECORD_WAKEUP_FOR_MS(200);
}

HIGH_LVL_API_TSPS_FUNC void on_set_robot_profile(bool cleanOtherAfx)
{
    RECORD_SLEEP;

    if(cleanOtherAfx) {
        reset_all_effects();
    }

    robot_tsps_setting();
    robot_echo_setting();

    RECORD_WAKEUP_FOR_MS(200);
}

HIGH_LVL_API_TSPS_FUNC void on_set_voc_change_profile(bool cleanOtherAfx)
{
    RECORD_SLEEP;

    if(cleanOtherAfx) {
        reset_all_effects();
    }

    vc_tsps_setting();

    RECORD_WAKEUP_FOR_MS(200);
}

HIGH_LVL_API_TSPS_FUNC void on_set_kid_profile(bool cleanOtherAfx)
{
    RECORD_SLEEP;

    if(cleanOtherAfx) {
        reset_all_effects();
    }

    kid_tsps_setting();
    female_kid_echo_setting();

    RECORD_WAKEUP_FOR_MS(200);
}

