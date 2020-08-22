
#if MODE_USBDEV_EN && USB_AUDIO_EN && USB_MSC_EN

#define UL_EQ_FUNC __attribute__((section(".upload_eq_seg")))

extern void COMP_HID_Send_Data(uint8_t* buff,uint8_t length );

#define DEBUG_USING_TIME 1

#if DEBUG_USING_TIME
__maybe_unused static const char* drc_sub_name[4] = {
    "lmt",
    "cpr",
    "exp",
    "ngt"
};

__maybe_unused static const char* page_name[6] = {
    "music",
    "effect",
    "mic",
    "o1",
    "o2",
    "vss"
};

#endif

#define CHECK_AND_PREPARE_BUFFER(buf, size, exception) \
    if(size != 64) exception;\
    else libc_memset(buf, 0xff, size)

#define SEND_DATA_TO_PC(buf) \
    COMP_HID_Send_Data(buf, 64)

extern float sp_log2(float x);

#define EQ_SETTING_RANGE_FOR_PAGE_ID(page_id) (page_id != PAGE_EFFECT)
#define DRC_SETTING_RANGE_FOR_PAGE_ID(page_id) (page_id != PAGE_EFFECT)
#define TSPS_SETTING_RANGE_FOR_PAGE_ID(page_id) (page_id == PAGE_MUSIC || page_id == PAGE_MIC)
#define FORMANT_SETTING_RANGE_FOR_PAGE_ID(page_id) (page_id == PAGE_MIC)
#define DR_SETTING_RANGE_FOR_PAGE_ID(page_id) (page_id == PAGE_OUTPUT1 || page_id == PAGE_OUTPUT2)
#define ECHO_REV_SETTING_RANGE_FOR_PAGE_ID(page_id) (page_id == PAGE_EFFECT)
#define VSS_SETTING_RANGE_FOR_PAGE_ID(page_id) (page_id == PAGE_VSS)

UL_EQ_FUNC void on_unload_for_page_id(PageClass page_id)
{
    eq_data ed[8] = {0};
    float preamp = 0.0;
    int ret = EQ_SET_NO_ERR;

#if DEBUG_USING_TIME
    __maybe_unused uint32_t t1 = timestamp_to_ms(get_timestamp());
    loge("page : %s", page_name[page_id]);
#endif
    //EQ Domain
    if(EQ_SETTING_RANGE_FOR_PAGE_ID(page_id))
    {
        int get_bands = EQ_SUBBAND_NUM;
        if(PAGE_VSS == page_id) {
            get_bands = MAX_EQ5_BANDS;
        }
        do {
            ret = on_x_eq_preamp_for_pageid(page_id, &preamp, EQ_GET_DATA);
            if(PAGE_VSS != page_id && EQ_XET_NONE_RESPONSE == ret) {
                break;
            }
#if DEBUG_USING_TIME
            loge("preamp:%d", (int)(100*preamp));
#endif
            for(int b = EQ_BAND_1; b < get_bands; b++)
            {
                CHECK_AND_PREPARE_BUFFER((uint8_t*)ed, sizeof(ed), return);

                eq_band_setting ebs = {0};
                ret = on_x_eq_setting_for_pageid(page_id, b, EQ_SB_ALL_PAPAS_MASK, &ebs, EQ_GET_DATA);
                if(EQ_XET_NONE_RESPONSE == ret) {
                    break;
                }
#if DEBUG_USING_TIME
                loge("band%d:%s, fc(%d), flt(%d), G(%d), Q(%d)", b, ebs.disable?"dis":"on",
                    ebs.fc, ebs.filter_type, (int)(100*ebs.G), (int)(100*ebs.Q_factor));
#endif
                ed[0].page_id    = page_id;
                ed[0].item_type  = TYPE_FREQ;
                ed[0].item_index = b;
                ed[0].fill.data  = (float)ebs.fc;

                ed[1].page_id    = page_id;
                ed[1].item_type  = TYPE_Q;
                ed[1].item_index = b;
                ed[1].fill.data  = (float)ebs.Q_factor;

                ed[2].page_id    = page_id;
                ed[2].item_type  = TYPE_GAIN;
                ed[2].item_index = b;
                ed[2].fill.data  = (float)ebs.G;

                ed[3].page_id    = page_id;
                ed[3].item_type  = TYPE_FILTER;
                ed[3].item_index = b;
                ed[3].fill.data  = (float)ebs.filter_type;

                ed[4].page_id    = page_id;
                ed[4].item_type  = TYPE_INPUT_VOL;
                ed[4].fill.data  = (float)preamp;

                ed[5].page_id    = page_id;
                ed[5].item_type  = TYEE_BAND_STATE_SWITCH;
                ed[5].item_index = b;
                ed[5].fill.data  = ebs.disable?0.0f:1.0f;

                SEND_DATA_TO_PC((uint8_t *)ed);
            }
        }while(0);
    }
    if(AFX_HLF_EN && (page_id == PAGE_OUTPUT1 || page_id == PAGE_OUTPUT2))
    {
        do {
            CHECK_AND_PREPARE_BUFFER((uint8_t*)ed, sizeof(ed), return);

            hl_filter_setting h = {0};
            hl_filter_setting l = {0};
            ret = on_x_output_flt_type_setting_for_pageid(page_id, HIGH_PASS, &h, EQ_GET_DATA);
            if(EQ_XET_NONE_RESPONSE == ret) {
                break;
            }
            ret = on_x_output_flt_type_setting_for_pageid(page_id, LOW_PASS, &l, EQ_GET_DATA);
            if(EQ_XET_NONE_RESPONSE == ret) {
                break;
            }
#if DEBUG_USING_TIME
            loge("hp freq:%d", h.fc);
            loge("hp sub_type:%d", h.sub_type);
            loge("lp freq:%d", l.fc);
            loge("lp sub_type:%d", l.sub_type);
#endif
            ed[0].page_id    = page_id;
            ed[0].item_type  = TYPE_DIVIDER_HP_FRE;
            ed[0].fill.data  = (float)h.fc;

            ed[1].page_id    = page_id;
            ed[1].item_type  = TYPE_DIVEDER_HP_TYPE;
            ed[1].fill.data  = (float)h.sub_type;

            ed[2].page_id    = page_id;
            ed[2].item_type  = TYPE_DIVIDER_LP_FRE;
            ed[2].fill.data  = (float)l.fc;

            ed[3].page_id    = page_id;
            ed[3].item_type  = TYPE_DIVEDER_LP_TYPE;
            ed[3].fill.data  = (float)l.sub_type;

            SEND_DATA_TO_PC((uint8_t *)ed);
        }while(0);
    }
    //Drc domain
    if(DRC_SETTING_RANGE_FOR_PAGE_ID(page_id))
    {
        do {
            float makeup = 0.0;
            ret = on_x_drc_makeup_gain_for_pageid(page_id, &makeup, EQ_GET_DATA);
            if(PAGE_VSS != page_id && EQ_XET_NONE_RESPONSE == ret) {
                break;
            }
#if DEBUG_USING_TIME
            loge("makeup:%d", (int)(100*makeup));
#endif
            for(int sdrc = DRC_LIMITER; sdrc < DRC_SUBFUNC_NUM; sdrc++)
            {
                if(page_id == PAGE_MUSIC) {
                    if(sdrc != DRC_NOSIEGATE) {
                        continue;
                    }
                }
                else if(page_id == PAGE_OUTPUT1 || page_id == PAGE_OUTPUT2)
                {
                    if(sdrc == DRC_NOSIEGATE) {
                        continue;
                    }
                }
                else if(page_id == PAGE_VSS) {
                    if(sdrc != DRC_LIMITER) {
                        continue;
                    }
                }
                else
                {
                    if(page_id != PAGE_MIC)
                    {
                        break;
                    }
                }

                CHECK_AND_PREPARE_BUFFER((uint8_t*)ed, sizeof(ed), return);

                drc_subfunc_setting dss = {0};

                ret = on_x_drc_subfunc_for_pageid(page_id, DRC_SUBFUNC_ALL_MASK, sdrc, &dss, EQ_GET_DATA);
                if(EQ_XET_NONE_RESPONSE == ret) {
                    break;
                }
#if DEBUG_USING_TIME
                loge("%s:%s", drc_sub_name[sdrc], dss.enable?"on":"off");
                loge("thd : %d", dss.x_thd);
                loge("rdio : %d", dss.x_ratio);
                loge("att : %d", dss.x_att);
                loge("rel : %d", dss.x_rel);
#endif

                ed[0].page_id    = page_id;
                ed[0].item_type  = TYPE_DRC_SUB_PROCESSOR_THRESHOLD;
                ed[0].item_index = sdrc;
                ed[0].fill.data  = (float)dss.x_thd;

                ed[1].page_id    = page_id;
                ed[1].item_type  = TYPE_DRC_SUB_PROCESSOR_RATIO;
                ed[1].item_index = sdrc;
                ed[1].fill.data  = (float)dss.x_ratio;

                ed[2].page_id    = page_id;
                ed[2].item_type  = TYPE_DRC_SUB_PROCESSOR_ATTACK_TIME;
                ed[2].item_index = sdrc;
                ed[2].fill.data  = (float)dss.x_att;

                ed[3].page_id    = page_id;
                ed[3].item_type  = TYPE_DRC_SUB_PROCESSOR_RELEASE_TIME;
                ed[3].item_index = sdrc;
                ed[3].fill.data  = (float)dss.x_rel;

                if(page_id != PAGE_VSS) {
                    ed[4].page_id    = page_id;
                    ed[4].item_type  = TYPE_DRC_MAKEUP_GAIN;
                    ed[4].fill.data  = makeup;

                    ed[5].page_id    = page_id;
                    ed[5].item_type  = TYPE_DRC_SUB_PROCESSOR_SWITCH;
                    ed[5].item_index = sdrc;
                    ed[5].fill.data  = dss.enable?1.0f:0.0f;
                }
                SEND_DATA_TO_PC((uint8_t *)ed);
            }
        }while(0);
    }
    //Tsps domain
    if(TSPS_SETTING_RANGE_FOR_PAGE_ID(page_id))
    {
        do {
            CHECK_AND_PREPARE_BUFFER((uint8_t*)ed, sizeof(ed), return);

            tsps_setting tsst = {0};
            ret = on_x_tsps_for_pageid(page_id, TSPS_ALL_PAPAS_MASK, &tsst, EQ_GET_DATA);
            if(EQ_XET_NONE_RESPONSE == ret) {
                break;
            }
            tsst.alpha_ps = sp_log2(tsst.alpha_ps);
            tsst.alpha_ts = sp_log2(tsst.alpha_ts);
#if DEBUG_USING_TIME
            loge("tsps %s", tsst.enable?"on":"off");
            loge("tsps ps : %d", (int)(100*tsst.alpha_ps));
            loge("tsps_ts : %d", (int)(100*tsst.alpha_ts));
#endif
            ed[0].page_id    = page_id;
            ed[0].item_type  = TYPE_TSPS_FC_SWITCH;
            ed[0].fill.data  = tsst.enable?1.0f:0.0f;

            ed[1].page_id    = page_id;
            ed[1].item_type  = TYPE_TSPS_PS;
            ed[1].fill.data  = tsst.alpha_ps;

            if(page_id == PAGE_MIC)
            {
                ed[2].page_id    = page_id;
                ed[2].item_type  = TYPE_TSPS_TS;
                ed[2].fill.data  = tsst.alpha_ts;
            }

            SEND_DATA_TO_PC((uint8_t *)ed);
        }while(0);
    }
    //Fc domain
    if(FORMANT_SETTING_RANGE_FOR_PAGE_ID(page_id))
    {
        do {
            CHECK_AND_PREPARE_BUFFER((uint8_t*)ed, sizeof(ed), return);

            fc_setting fcst = {0};
            ret = on_x_formant_for_pageid(page_id, FC_ALL_PAPAS_MASK, &fcst, EQ_GET_DATA);
            if(EQ_XET_NONE_RESPONSE == ret) {
                break;
            }
#if DEBUG_USING_TIME
            loge("fc %s", fcst.enable?"on":"off");
            loge("fc alpha : %d", (int)(100*fcst.alpha_fc));
#endif
            ed[0].page_id    = page_id;
            ed[0].item_type  = TYPE_TSPS_FC;
            ed[0].fill.data  = fcst.alpha_fc;

            SEND_DATA_TO_PC((uint8_t *)ed);
        }while(0);
    }
    //Dr domain
    if(DR_SETTING_RANGE_FOR_PAGE_ID(page_id))
    {
        do {
            CHECK_AND_PREPARE_BUFFER((uint8_t*)ed, sizeof(ed), return);

            bool dr_on_off = false;
            dly_setting ds = {0};
            ret = on_x_dr_switch_for_pageid(page_id, &dr_on_off, EQ_GET_DATA);
            if(EQ_XET_NONE_RESPONSE == ret) {
                break;
            }
            ret = on_x_dr_setting_for_pageid(page_id, &ds, EQ_GET_DATA);
            if(EQ_XET_NONE_RESPONSE == ret) {
                break;
            }
#if DEBUG_USING_TIME
            loge("dr %s", dr_on_off?"on":"off");
            loge("%s", (ds.rev_factor==-1)?"rev":"nor");
            loge("dly:%d", ds.rtdly);
#endif
            ed[0].page_id    = page_id;
            ed[0].item_type  = TYPE_DR_SWITCH;
            ed[0].fill.data  = dr_on_off?1.0f:0.0f;

            ed[1].page_id    = page_id;
            ed[1].item_type  = TYPE_DR_REVERSE;
            ed[1].fill.data  = ds.rev_factor == -1?1.0:0.0;

            ed[2].page_id    = page_id;
            ed[2].item_type  = TYPE_DR_DLY;
            ed[2].fill.data  = (float)ds.rtdly;

            SEND_DATA_TO_PC((uint8_t *)ed);
        }while(0);
    }
    //Echo and reverb domain
    if(ECHO_REV_SETTING_RANGE_FOR_PAGE_ID(page_id))
    {
        do {
            CHECK_AND_PREPARE_BUFFER((uint8_t*)ed, sizeof(ed), return);
            echo_setting es = {0};
            ret = on_x_echo_for_pageid(page_id, ECHO_ALL_PAPAS_MASK, &es, EQ_GET_DATA);
            if(EQ_XET_NONE_RESPONSE == ret) {
                break;
            }
#if DEBUG_USING_TIME
            loge("ec %s", es.enable?"on":"off");
            loge("lp freq : %d", es.cutoff_freq);
            loge("%s", es.need_lp?"en-lp":"dis-lp");
            loge("fb-coff:%d", (int)(100*es.gFbc));
            loge("dry:%d", (int)(100*es.gDry));
            loge("wet:%d", (int)(100*es.gWet));
            loge("dly:%d", es.dly);
#endif
            ed[0].page_id    = page_id;
            ed[0].item_type  = TYPE_ECHO_SWITCH;
            ed[0].fill.data  = es.enable?1.0f:0.0f;

            ed[1].page_id    = page_id;
            ed[1].item_type  = TYPE_ECHO_LP;
            ed[1].fill.data  = es.need_lp?1.0:0.0;

            ed[2].page_id    = page_id;
            ed[2].item_type  = TYPE_ECHO_LP_FREQ;
            ed[2].fill.data  = (float)es.cutoff_freq;

            ed[3].page_id    = page_id;
            ed[3].item_type  = TYPE_ECHO_G_DRY;
            ed[3].fill.data  = (float)(es.gDry*100);

            ed[4].page_id    = page_id;
            ed[4].item_type  = TYPE_ECHO_G_WET;
            ed[4].fill.data  = (float)(es.gWet*100);

            ed[5].page_id    = page_id;
            ed[5].item_type  = TYPE_ECHO_FEEDBACK_COFF;
            ed[5].fill.data  = (float)(es.gFbc*100);

            ed[6].page_id    = page_id;
            ed[6].item_type  = TYPE_ECHO_DELAY;
            ed[6].fill.data  = (float)es.dly;

            SEND_DATA_TO_PC((uint8_t *)ed);
        }while(0);

        do {
            rev_setting rs = {0};
            ret = on_x_rev_for_pageid(page_id, REVERB_ALL_PAPAS_MASK, &rs, EQ_GET_DATA);
            if(EQ_XET_NONE_RESPONSE == ret) {
                break;
            }
#if DEBUG_USING_TIME

            loge("rev %s", rs.enable?"on":"off");
            loge("rt : %d", rs.revb_time);
            loge("df-lo : %d", rs.damp_freq_lo);
            loge("df-hi : %d", rs.damp_freq_hi);
            loge("lpf : %d", rs.lpf_freq);
            loge("hpf : %d", rs.hpf_freq);
            loge("pre-dly : %d", rs.pre_delay);
            loge("diff:%d", (int)(100*rs.diff));
            loge("dry:%d", (int)(100*rs.g_dry));
            loge("wet:%d", (int)(100*rs.g_wet));
            loge("drh:%d", (int)(100*rs.damp_ratio_hi));
            loge("drl:%d", (int)(100*rs.damp_ratio_lo));
#endif
            CHECK_AND_PREPARE_BUFFER((uint8_t*)ed, sizeof(ed), return);

            ed[0].page_id    = page_id;
            ed[0].item_type  = TYPE_REVERB_PREDLY;
            ed[0].fill.data  = (float)rs.pre_delay;

            ed[1].page_id    = page_id;
            ed[1].item_type  = TYPE_REVERB_TIME;
            ed[1].fill.data  = (float)rs.revb_time;

            ed[2].page_id    = page_id;
            ed[2].item_type  = TYPE_REVERB_DIFFUSION;
            ed[2].fill.data  = (float)rs.diff;

            ed[3].page_id    = page_id;
            ed[3].item_type  = TYPE_REVERB_G_WET;
            ed[3].fill.data  = (float)(rs.g_wet*100);

            ed[4].page_id    = page_id;
            ed[4].item_type  = TYPE_REVERB_G_DRY;
            ed[4].fill.data  = (float)(rs.g_dry*100);

            ed[5].page_id    = page_id;
            ed[5].item_type  = TYPE_REVERB_DAMPING_FREQ_LO;
            ed[5].fill.data  = (float)rs.damp_freq_lo;

            ed[6].page_id    = page_id;
            ed[6].item_type  = TYPE_REVERB_DAMPING_FREQ_HI;
            ed[6].fill.data  = (float)rs.damp_freq_hi;

            ed[7].page_id    = page_id;
            ed[7].item_type  = TYPE_REVERB_DAMPING_RATIO_LO;
            ed[7].fill.data  = rs.damp_ratio_lo;

            SEND_DATA_TO_PC((uint8_t *)ed);

            CHECK_AND_PREPARE_BUFFER((uint8_t*)ed, sizeof(ed), return);

            ed[0].page_id    = page_id;
            ed[0].item_type  = TYPE_REVERB_DAMPING_RATIO_HI;
            ed[0].fill.data  = rs.damp_ratio_hi;

            ed[1].page_id    = page_id;
            ed[1].item_type  = TYPE_REVERB_LP_FREQ;
            ed[1].fill.data  = (float)rs.lpf_freq;

            ed[2].page_id    = page_id;
            ed[2].item_type  = TYPE_REVERB_HP_FREQ;
            ed[2].fill.data  = (float)rs.hpf_freq;

            ed[3].page_id    = page_id;
            ed[3].item_type  = TYPE_REVERB_REC_MS_SWITCH;
            ed[3].fill.data  = (float)rs.m2s?1.0f:0.0f;

            ed[4].page_id    = page_id;
            ed[4].item_type  = TYPE_REVERB_SWITCH;
            ed[4].fill.data  = rs.enable?1.0f:0.0f;

            SEND_DATA_TO_PC((uint8_t *)ed);
        }while(0);
    }
    if(VSS_SETTING_RANGE_FOR_PAGE_ID(page_id))
    {
        do {
            CHECK_AND_PREPARE_BUFFER((uint8_t*)ed, sizeof(ed), return);
            vss_pro_setting vs_hp  = {0};
            vss_pro_setting vs_spk = {0};

            uint8_t mode = VSS_MODE_HEADPHONE;
            bool    enable = false;

            ret = on_x_vss_pro_for_pageid(page_id, &mode, &vs_hp, EQ_GET_DATA);
            if(EQ_XET_NONE_RESPONSE == ret) {
                break;
            }

#if DEBUG_USING_TIME
            loge("hp - synth : %d", vs_hp.synth);
            loge("hp - depth : %d", vs_hp.sound_field_width.hp_depth);
            loge("hp - sp inte : %d", vs_hp.sp_intensity);
            loge("hp - bass inte : %d", vs_hp.bass_intensity);
            loge("hp - def : %d", vs_hp.definition);
            loge("hp - bassfreq : %d", vs_hp.bass_cofreq);
#endif
            mode = VSS_MODE_SPEAKER;

            on_x_vss_pro_for_pageid(page_id, &mode, &vs_spk, EQ_GET_DATA);
            if(EQ_XET_NONE_RESPONSE == ret) {
                break;
            }
#if DEBUG_USING_TIME
            loge("spk - synth : %d", vs_spk.synth);
            loge("spk - span : %d", vs_spk.sound_field_width.spk_span);
            loge("spk - sp inte : %d", vs_spk.sp_intensity);
            loge("spk - bass inte : %d", vs_spk.bass_intensity);
            loge("spk - def : %d", vs_spk.definition);
            loge("spk - bassfreq : %d", vs_spk.bass_cofreq);

            loge("%s", mode == 1?"hp":"spk");
#endif

            ret = on_x_vss_switch_for_pageid(page_id, &enable, EQ_GET_DATA);
            if(EQ_XET_NONE_RESPONSE == ret) {
                break;
            }
#if DEBUG_USING_TIME
            loge("%s", enable?"on":"dis");
#endif

            ed[0].page_id    = page_id;
            ed[0].item_type  = TYPE_VSS_SYNTHESIS_WIDTH;
            ed[0].fill.data  = (float)vs_spk.synth;
            ed[0].reserve    = VSS_MODE_TRANSLATE_BETWEEN_PC_AND_DRIVER(VSS_MODE_SPEAKER);

            ed[1].page_id    = page_id;
            ed[1].item_type  = TYPE_VSS_SPAN_WIDTH;
            ed[1].fill.data  = (float)vs_spk.sound_field_width.spk_span;
            ed[1].reserve    = VSS_MODE_TRANSLATE_BETWEEN_PC_AND_DRIVER(VSS_MODE_SPEAKER);

            ed[2].page_id    = page_id;
            ed[2].item_type  = TYPE_VSS_SPATIAL_INTENSITY;
            ed[2].fill.data  = (float)vs_spk.sp_intensity;
            ed[2].reserve    = VSS_MODE_TRANSLATE_BETWEEN_PC_AND_DRIVER(VSS_MODE_SPEAKER);

            ed[3].page_id    = page_id;
            ed[3].item_type  = TYPE_VSS_BASS_INTENSITY;
            ed[3].fill.data  = (float)vs_spk.bass_intensity;
            ed[3].reserve    = VSS_MODE_TRANSLATE_BETWEEN_PC_AND_DRIVER(VSS_MODE_SPEAKER);

            ed[4].page_id    = page_id;
            ed[4].item_type  = TYPE_VSS_DEFINITION;
            ed[4].fill.data  = (float)vs_spk.definition;
            ed[4].reserve    = VSS_MODE_TRANSLATE_BETWEEN_PC_AND_DRIVER(VSS_MODE_SPEAKER);

            ed[5].page_id    = page_id;
            ed[5].item_type  = TYPE_VSS_BASS_COFREQ;
            ed[5].fill.data  = (float)vs_spk.bass_cofreq;
            ed[5].reserve    = VSS_MODE_TRANSLATE_BETWEEN_PC_AND_DRIVER(VSS_MODE_SPEAKER);

            SEND_DATA_TO_PC((uint8_t *)ed);
            CHECK_AND_PREPARE_BUFFER((uint8_t*)ed, sizeof(ed), return);

            ed[0].page_id    = page_id;
            ed[0].item_type  = TYPE_VSS_SYNTHESIS_WIDTH;
            ed[0].fill.data  = (float)vs_hp.synth;
            ed[0].reserve    = VSS_MODE_TRANSLATE_BETWEEN_PC_AND_DRIVER(VSS_MODE_HEADPHONE);

            ed[1].page_id    = page_id;
            ed[1].item_type  = TYPE_VSS_DEPTH_WIDTH;
            ed[1].fill.data  = (float)vs_hp.sound_field_width.hp_depth;
            ed[1].reserve    = VSS_MODE_TRANSLATE_BETWEEN_PC_AND_DRIVER(VSS_MODE_HEADPHONE);

            ed[2].page_id    = page_id;
            ed[2].item_type  = TYPE_VSS_SPATIAL_INTENSITY;
            ed[2].fill.data  = (float)vs_hp.sp_intensity;
            ed[2].reserve    = VSS_MODE_TRANSLATE_BETWEEN_PC_AND_DRIVER(VSS_MODE_HEADPHONE);

            ed[3].page_id    = page_id;
            ed[3].item_type  = TYPE_VSS_BASS_INTENSITY;
            ed[3].fill.data  = (float)vs_hp.bass_intensity;
            ed[3].reserve    = VSS_MODE_TRANSLATE_BETWEEN_PC_AND_DRIVER(VSS_MODE_HEADPHONE);

            ed[4].page_id    = page_id;
            ed[4].item_type  = TYPE_VSS_DEFINITION;
            ed[4].fill.data  = (float)vs_hp.definition;
            ed[4].reserve    = VSS_MODE_TRANSLATE_BETWEEN_PC_AND_DRIVER(VSS_MODE_HEADPHONE);

            ed[5].page_id    = page_id;
            ed[5].item_type  = TYPE_VSS_BASS_COFREQ;
            ed[5].fill.data  = (float)vs_hp.bass_cofreq;
            ed[5].reserve    = VSS_MODE_TRANSLATE_BETWEEN_PC_AND_DRIVER(VSS_MODE_HEADPHONE);

            ed[6].page_id    = page_id;
            ed[6].item_type  = TYPE_VSS_HP_SPK_SWITCH;
            ed[6].fill.data  = mode?0.0f:1.0f; // hp or spk

            ed[7].page_id    = page_id;
            ed[7].item_type  = TYPE_VSS_SWITCH;
            ed[7].fill.data  = enable?1.0f:0.0f;

            SEND_DATA_TO_PC((uint8_t *)ed);
        }while(0);
    }

#if DEBUG_USING_TIME
    printf_debug("Using %d ms\n", timestamp_to_ms(get_timestamp()) - t1);
#endif
}

#endif  //MODE_USBDEV_EN && USB_AUDIO_EN && USB_MSC_EN