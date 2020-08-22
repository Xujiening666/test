#define HIGH_LVL_API_EQ_FUNC __attribute__((section(".high_lvl_api_eq_seg")))
#define COMMON_EQ_BAND_START EQ_BAND_4

#define RADIO_EQ_BAND_START  COMMON_EQ_BAND_START
#define RADIO_EQ_BAND_NUM    9

float radio_eq_preamp = 0.0f;
eq_band_setting radio_eq[RADIO_EQ_BAND_NUM] = {
    { BIQUAD_PEAKNOTCH,   615,  -8.4,  8.00, false },
    { BIQUAD_LOWSHELVING, 688,  -8.57, 0.75, false  },
    { BIQUAD_PEAKNOTCH,   738,  -6.17, 8.00, false  },
    { BIQUAD_HIGHPASS,    749,   3.09, 0.75, false  },
    { BIQUAD_PEAKNOTCH,   1636,  2.4,  1.00, false  },
    { BIQUAD_LOWPASS,     4114, -1.03, 0.75, false  },
    { BIQUAD_PEAKNOTCH,   6152, -5.49, 8.00, false  },
    { BIQUAD_PEAKNOTCH,   6738, -3.77, 8.00, false  },
    { BIQUAD_PEAKNOTCH,   7215, -3.77, 8.00, false  },
};

#define TREB_AND_BASS_EQ_BAND_START EQ_BAND_1
#define TREB_AND_BASS_EQ_BAND_NUM   EQ_TYPE_NUM

float bass_and_treble_eq_preamp[2] = {0};
eq_band_setting bass_and_treble_eq[2][EQ_TYPE_NUM]={
    {//Mic
        { BIQUAD_LOWSHELVING,  300,    0.0, 0.75, false },
        { BIQUAD_PEAKNOTCH,    600,    0.0, 0.75, false },
        { BIQUAD_HIGHSHELVING, 3000,   0.0, 0.75, false },
    },
    {//Playback
        { BIQUAD_PEAKNOTCH,    500,    0.0, 1.7, false },
        { BIQUAD_PEAKNOTCH,    1500,   0.0, 1.7, false },
        { BIQUAD_PEAKNOTCH,    4000,   0.0, 1.7, false },
    }
};

eq_band_setting *eq_band_setting_focus       = (eq_band_setting*)bass_and_treble_eq[REC];
float           *eq_preamp_focus             = &bass_and_treble_eq_preamp[REC];
int8_t           eq_band_setting_start_focus = TREB_AND_BASS_EQ_BAND_START;
int8_t           eq_band_setting_num_focus   = TREB_AND_BASS_EQ_BAND_NUM;
int8_t           eq_band_setting_page_focus  = PAGE_MIC;

static void treb_and_bass_eq_get_focus(int8_t stream)
{
    eq_band_setting_focus       = (eq_band_setting*)bass_and_treble_eq[stream];
    eq_preamp_focus             = &bass_and_treble_eq_preamp[stream];
    eq_band_setting_start_focus = TREB_AND_BASS_EQ_BAND_START;
    eq_band_setting_num_focus   = TREB_AND_BASS_EQ_BAND_NUM;
    eq_band_setting_page_focus  = stream?PAGE_MUSIC:PAGE_MIC;
}


static void radio_eq_get_focus()
{
    eq_band_setting_focus       = (eq_band_setting*)radio_eq;
    eq_preamp_focus             = &radio_eq_preamp;
    eq_band_setting_start_focus = RADIO_EQ_BAND_START;
    eq_band_setting_num_focus   = RADIO_EQ_BAND_NUM;
    eq_band_setting_page_focus  = PAGE_MIC;
}

static void common_eq_setting()
{
    for(int b = eq_band_setting_start_focus; b < eq_band_setting_start_focus + eq_band_setting_num_focus; b++)
    {
        on_x_eq_setting_for_pageid(eq_band_setting_page_focus, b, EQ_SB_ALL_PAPAS_MASK,
                                    &eq_band_setting_focus[b - eq_band_setting_start_focus], EQ_SET_DATA);
    }
    on_x_eq_preamp_for_pageid(eq_band_setting_page_focus, eq_preamp_focus, EQ_SET_DATA);
}

static void radio_eq_setting()
{
    radio_eq_get_focus();
    common_eq_setting();
}

HIGH_LVL_API_EQ_FUNC void on_set_radio_profile(bool cleanOtherAfx)
{
    RECORD_SLEEP;

    if(cleanOtherAfx) {
        reset_all_effects();
    }

    radio_eq_setting();

    RECORD_WAKEUP_FOR_MS(200);
}

HIGH_LVL_API_EQ_FUNC void on_set_treb_bass_gain(int8_t gain, uint8_t stream, eq_type_t eq_band)
{
    treb_and_bass_eq_get_focus(stream);

    eq_band_setting_focus[eq_band].G = (float)gain;
    on_x_eq_setting_for_pageid(eq_band_setting_page_focus, eq_band, EQ_SB_ALL_PAPAS_MASK, &eq_band_setting_focus[eq_band], EQ_SET_DATA);
}

HIGH_LVL_API_EQ_FUNC void on_set_eq_focus_preamp(int8_t db)
{
    db = db > 12?12:
        (db < -12?-12:db);
    *eq_preamp_focus = (float)db;
    on_x_eq_preamp_for_pageid(eq_band_setting_page_focus, eq_preamp_focus, EQ_SET_DATA);
}

HIGH_LVL_API_EQ_FUNC void on_set_tbvc_switch(bool sw)
{
    void* chain = audio_service_get_playback_afx_chain();
    afx_hook_node_proc_types owner = AFX_PROC_TBVC;
    afx_hook_defines* tbvc = getAfxProcNodeForOwner(chain, owner, 0);
    if(tbvc && tbvc->SettingPara) {
        setTbvcSwitch(tbvc->SettingPara, sw);
    }
    afx_chain_cmd(chain, AFX_CHAIN_CMD_UNIVERSE_SETTING, 0);
}
