#define HIGH_LVL_API_EQ5_FUNC __attribute__((section(".high_lvl_api_eq5_seg")))

eq_band_setting sonic_boom_echo_eq[5] = {
    { BIQUAD_PEAKNOTCH,    20,    0.0, 0.75, true },
    { BIQUAD_HIGHPASS,     50,    0.1, 0.8, false },
    { BIQUAD_HIGHSHELVING, 2000,  3.0, 0.7, false },
    { BIQUAD_PEAKNOTCH,    3500,  4.0, 0.8, false },
    { BIQUAD_LOWPASS,      16000, 0.1, 0.8, false },
};
#define SONIC_BOOM_ECHO_EQ_PREAMP     2.0f

eq_band_setting sonic_boom_revb_eq[5] = {
    { BIQUAD_PEAKNOTCH,    20,    0.0, 0.75, true },
    { BIQUAD_HIGHPASS,     100,    0.1, 0.8, false },
    { BIQUAD_HIGHSHELVING, 2000,  1.5, 0.8, false },
    { BIQUAD_PEAKNOTCH,    3500,  4.0, 0.5, false },
    { BIQUAD_LOWPASS,      16000, 0.1, 0.8, false },
};
#define SONIC_BOOM_REVB_EQ_PREAMP    -1.0f

eq_band_setting rock_mic_echo_eq[5] = {
    { BIQUAD_PEAKNOTCH,    20,    0.0, 0.75, true },
    { BIQUAD_HIGHPASS,     150,    0.1, 1.0, false },
    { BIQUAD_HIGHSHELVING, 1500,  -6.0, 0.707, false },
    { BIQUAD_LOWPASS,      15000, 0.1, 0.707, false },
    { BIQUAD_PEAKNOTCH,    20000, 0.0, 8.0, false },
};
#define ROCK_MIC_ECHO_EQ_PREAMP     6.0f

eq_band_setting rock_mic_revb_eq[5] = {
    { BIQUAD_PEAKNOTCH,    20,    0.0, 0.75, true },
    { BIQUAD_HIGHPASS,     100,    0.1, 1.0, false },
    { BIQUAD_HIGHSHELVING, 2000,  1.5, 0.8, false },
    { BIQUAD_PEAKNOTCH,    2000,  -2.99, 0.7, false },
    { BIQUAD_LOWPASS,      16000, 0.1, 0.8, false },
};
#define ROCK_MIC_REVB_EQ_PREAMP    -1.0f


eq_band_setting* post_echo_eq5_focus         = rock_mic_echo_eq;
float            post_echo_eq5_preamp_focus  = ROCK_MIC_ECHO_EQ_PREAMP;

eq_band_setting* post_revb_eq5_focus         = rock_mic_revb_eq;
float            post_revb_eq5_preamp_focus  = ROCK_MIC_REVB_EQ_PREAMP;

HIGH_LVL_API_EQ5_FUNC static void rock_mic_eq_get_focus()
{
    post_echo_eq5_focus         = rock_mic_echo_eq;
    post_echo_eq5_preamp_focus  = ROCK_MIC_ECHO_EQ_PREAMP;

    post_revb_eq5_focus         = rock_mic_revb_eq;
    post_revb_eq5_preamp_focus  = ROCK_MIC_REVB_EQ_PREAMP;
}

HIGH_LVL_API_EQ5_FUNC static void sonic_boom_eq_get_focus()
{
    post_echo_eq5_focus         = sonic_boom_echo_eq;
    post_echo_eq5_preamp_focus  = SONIC_BOOM_ECHO_EQ_PREAMP;

    post_revb_eq5_focus         = sonic_boom_revb_eq;
    post_revb_eq5_preamp_focus  = SONIC_BOOM_REVB_EQ_PREAMP;
}

HIGH_LVL_API_EQ5_FUNC void eq5_common_setting(EFFECT_SWTICH sw)
{
    eq5_setting_list e5sl = {0};
    e5sl.ch      = LEFT_CHANNEL;
    e5sl.owner   = AFX_PROC_EQ_SEC_5 | AFX_PROC_TYPE_COMP_POST_ECHO | AFX_PROC_TYPE_PGE_EFFECT;
    e5sl.preamp  = post_echo_eq5_preamp_focus;
    e5sl.setting = post_echo_eq5_focus;
    e5sl.enable  = !!sw;

    afx_chain_cmd(audio_service_get_record_afx_chain(), AFX_CHAIN_CMD_EQ5_SETTING, (uint32_t)&e5sl);

    e5sl.owner   = AFX_PROC_EQ_SEC_5 | AFX_PROC_TYPE_COMP_POST_REVB | AFX_PROC_TYPE_PGE_EFFECT;
    e5sl.preamp  = post_revb_eq5_preamp_focus;
    e5sl.setting = post_revb_eq5_focus;
    e5sl.enable  = !!sw;

    afx_chain_cmd(audio_service_get_record_afx_chain(), AFX_CHAIN_CMD_EQ5_SETTING, (uint32_t)&e5sl);
}

HIGH_LVL_API_EQ5_FUNC void on_set_rock_mic_eq_profile(EFFECT_SWTICH sw)
{
    if(sw == ESWITCH_HARD_ON) {
        reset_all_effects();
    }
    rock_mic_eq_get_focus();
    eq5_common_setting(sw);
}

HIGH_LVL_API_EQ5_FUNC void on_set_sonic_boom_eq_profile(EFFECT_SWTICH sw)
{
    if(sw == ESWITCH_HARD_ON) {
        reset_all_effects();
    }
    sonic_boom_eq_get_focus();
    eq5_common_setting(sw);
}


