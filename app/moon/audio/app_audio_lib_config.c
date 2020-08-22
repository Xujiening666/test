#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "audio_service.h"
#include "audiopath_conf.h"
#include "sndlib.h"
#include "audio_in.h"
#include "audio_out.h"
#include "log.h"
#include "usbaudio.h"
#include "audio_internal_config.h"
#include "audio_player.h"
#include "linein_audio.h"

/*
 * CONFIG_USBA_MIX_TO_OUTPUT indicates USB Audio mixing position
 * false: USB Audio mixing at "music(audio_track) afx input" position
 * true:  USB Audio mixing at "karaoke mix out" position
 */
bool CONFIG_USBA_MIX_TO_OUTPUT = false;

/*
 * audio_player module overwrite
 *
 */
#if ((TONE_EN == 0) && (MODE_MUSIC_EN == 0))
/* unlink audio_player lib */
AT(.audioplayer_sram_seg)
int32_t player_cmd(player_cmd_t cmd, uint32_t arg){return 0;}

AT(.audioplayer_flash_seg)
int32_t player_open(source_info_t *info){return 0;}

AT(.audioplayer_flash_seg)
void player_close(uint32_t max_wait_complete_time){}

AT(.audioplayer_flash_seg)
MODULE_FUNC void* create_player(ap_init_params_t *params)
{
    return (void*)0xFFFFFFFF;
}

AT(.audioplayer_flash_seg)
void destroy_player(void *player){}
#endif /* audio_player module overwrite */

/*
 * audio_track module overwrite
 *
 */
#if (AUDIO_KARAOKE_MODE && (USB_AUDIO_ALL_MODE_SUPPORT || LINEIN_ALL_MODE_SUPPORT))
extern const void __karaoke_audio_track_lib;
void *audio_track_lib = (void*)&__karaoke_audio_track_lib;
#endif /* audio_track module overwrite */

/*
 * karaoke module overwrite
 *
 */
#if (!AUDIO_KARAOKE_MODE) && (!AUDIO_ADJUST_HPLR_LINEOUTLR)
void *hwasrc_open(void *params)
{
    return NULL;
}

void hwasrc_close(void *hwasrc_handle)
{
}

int hwasrc_blockproc(void *hwasrc_handle,
    int32_t **in, int32_t *in_samples,
    int32_t **out, int32_t *out_samples)
{
    return 0;
}

void *create_karaoke(void)
{
    return NULL;
}
void destroy_karaoke(void *handle)
{
}

int karaoke_cmd(void *handle, uint32_t cmd, uint32_t val)
{
    return -1;
}

AT(.audio_effects_seg)
int karaoke_write(void *handle, int32_t *buf,
    int32_t samples, uint8_t sample_size)
{
    return -1;
}

AT(.audio_effects_seg)
void karaoke_fs_nhs_proc(void *handle, void* src, bool run_cpu1)
{
}

aout_interface_t *create_karaoke_aout(void)
{
    return NULL;
}

#if ((0 == RECORD_LINEIN_FUNC) && ( 0 == RECORD_FM_FUNC))
ain_interface_t* create_recorder_ain(void) { return NULL;}
#endif


#endif /* karaoke module overwrite */

#if (!AUDIO_ADJUST_HPLR_LINEOUTLR)
void anc_task_init(void){}
void anc_task_start(uint32_t fs, uint8_t ch, uint8_t sample_size){}
void anc_task_stop(void){}
void anc_get_dac_data(int32_t *pL, int32_t *pR){}
uint32_t get_sndcodec_r_avail(void){return 0;}
uint8_t *get_sndcodec_rptr(void){return NULL;}
void sndcodec_update_rpos(uint32_t n){}

void auss_anc_start(struct snd_hardware *hw){}
void auss_anc_stop(struct snd_hardware *hw){}
void auss_play_anc_data_enable(bool enable){}
void auss_play_fifo_data_enable(bool enable){}
void set_auss_anc_config(uint32_t rate, uint32_t order){}
int32_t auss_anc_read_ch1(void){return 0;}
int32_t auss_anc_read_ch2(void){return 0;}
int32_t auss_anc_read_ch3(void){return 0;}
int32_t auss_anc_read_ch4(void){return 0;}
void auss_anc_write(int32_t Ldata, int32_t Rdata){}
void anc_irq_handler(void){}

int sndcodec_anc_out_params(struct snd_hardware *hw,
    enum pcm_hw_direction dir, struct pcm_hw_params *params)
{
    return 0;
}

int sndcodec_anc_out_free(struct snd_hardware *hw, enum pcm_hw_direction dir)
{
    return 0;
}

int sndcodec_anc_out_prepare(struct snd_hardware *hw, enum pcm_hw_direction dir)
{
    return 0;
}

int sndcodec_anc_out_trigger_start(struct snd_hardware *hw,
    enum pcm_hw_direction dir, enum pcm_hw_trigger_cmd cmd)
{
    return 0;
}

#endif

/*
 * linein_audio module overwrite
 *
 */
#if ((!MODE_LINEIN_EN) && (!MODE_FM_EN))
void create_linein_audio(void){}
void destroy_linein_audio(void){}
int32_t linein_audio_cmd(uint32_t cmd, uint32_t val){return 0;}
#endif

#if ((!AUDIO_KARAOKE_MODE) && (!LINEIN_ALL_MODE_SUPPORT))

AT(.linein_audio_src_flash_seg)
void linein_audio_source_init(void){}
AT(.linein_audio_src_flash_seg)
void linein_audio_source_deinit(void){}
AT(.linein_audio_src_sram_seg)
void linein_audio_source_push_data(void *buf, uint32_t samples){}
AT(.linein_audio_src_sram_seg)
void linein_audio_source_pull_data(void *buf, uint32_t samples)
{
    memset(buf,0, samples * 2 * sizeof(int16_t));
}

AT(.linein_audio_src_flash_seg)
void linein_audio_source_ctrl_set_volume(uint8_t volume)
{
    audio_service_set_playback_volume(volume);
}

AT(.linein_audio_src_flash_seg)
uint8_t linein_audio_source_ctrl_get_volume(void)
{
    return audio_service_get_playback_volume();
}

AT(.linein_audio_src_flash_seg)
void linein_audio_source_ctrl_set_mute(bool mute)
{
    linein_audio_mute(mute);
}

AT(.linein_audio_src_flash_seg)
bool linein_audio_source_ctrl_get_mute(void)
{
    return linein_audio_is_muted();
}

#endif

/*
 * audio_source/audio_sink module overwrite
 *
 */
#if (!USB_AUDIO_ALL_MODE_SUPPORT)
AT(.usb_audio_src_flash_seg)
void usb_audio_source_init(void){}
AT(.usb_audio_src_flash_seg)
void usb_audio_source_deinit(void){}
AT(.usb_audio_src_sram_seg)
void usb_audio_source_push_data(void *buf, uint32_t samples){}
AT(.usb_audio_src_sram_seg)
void usb_audio_source_pull_data(void *buf, uint32_t samples)
{
    memset(buf, 0, samples * 2 * sizeof(int16_t));
}
AT(.usb_audio_src_flash_seg)
void usb_audio_source_ctrl_set_volume(uint8_t volume)
{
    audio_service_set_playback_volume(volume);
}
AT(.usb_audio_src_flash_seg)
uint8_t usb_audio_source_ctrl_get_volume(void)
{
    return audio_service_get_playback_volume();
}
AT(.usb_audio_src_flash_seg)
void usb_audio_source_ctrl_set_mute(bool mute){}
AT(.usb_audio_src_flash_seg)
bool usb_audio_source_ctrl_get_mute(void){return false;}

AT(.usb_audio_sink_flash_seg)
void usb_audio_sink_init(void){}
AT(.usb_audio_sink_flash_seg)
void usb_audio_sink_deinit(void){}
AT(.usb_audio_sink_sram_seg)
void usb_audio_sink_push_data(void *interpbuf, void *buf,
    uint32_t samples, uint8_t capture_type){}
AT(.usb_audio_sink_sram_seg)
void usb_audio_sink_pull_data(void *buf, uint32_t samples)
{
    memset(buf, 0, samples * 2 * sizeof(int16_t));
}
AT(.usb_audio_sink_sram_seg)
void usb_audio_sink_update_capture_type(void){}
AT(.usb_audio_sink_flash_seg)
void usb_audio_sink_ctrl_set_volume(uint8_t volume)
{
    audio_service_set_record_volume(volume);
}
AT(.usb_audio_sink_flash_seg)
uint8_t usb_audio_sink_ctrl_get_volume(void)
{
    return audio_service_get_record_volume();
}
AT(.usb_audio_sink_flash_seg)
void usb_audio_sink_ctrl_set_capture_type(uint8_t capture_type){}
AT(.usb_audio_sink_flash_seg)
void usb_audio_sink_ctrl_set_mute(bool mute){}
AT(.usb_audio_sink_flash_seg)
bool usb_audio_sink_ctrl_get_mute(void){return false;}

#endif /* audio_source/audio_sink module overwrite */

#if (!AUDIO_KARAOKE_MODE  || ((!USB_AUDIO_ALL_MODE_SUPPORT) && (!LINEIN_ALL_MODE_SUPPORT)))
AT(.karaoke_flash_seg)
void karaoke_init_component(void *ok_handle){
    CONFIG_USBA_MIX_TO_OUTPUT = false;
}
AT(.karaoke_flash_seg)
void karaoke_deinit_component(void *ok_handle){}
#endif

/*
 * usbaudio module overwrite
 *
 */
#if (!MODE_USBDEV_EN)
void create_usbaudio(usbaudio_init_t *params){}
void destroy_usbaudio(void){}
int32_t usbaudio_cmd(uint32_t cmd, uint32_t val){return -1;}
int32_t usbaudio_playback_write(void *buf, uint32_t samples)
{
    return -1;
}
int32_t usbaudio_record_read(void *buf, uint32_t samples)
{
    return -1;
}
#endif /* usbaudio module overwrite */


#if (0 == BT_PHONE_EN)
void bt_audio_adapter_sco_init(void){}
void bt_audio_adapter_sco_deinit(void){}
void bt_phone_uplink_block_read(uint8_t *buf){}
void bt_phone_downlink_block_write(uint8_t *buf){}
int sco_audio_start(void *handle){return 0;}
int sco_audio_stop(void *handle){return 0;}
int sco_audio_process(void *handle){return 0;}
int sco_audio_cmd(void *handle, uint32_t cmd, uint32_t val){return 0;}
#endif

#if (0 == TONE_EN)
void* create_tone_stream(void)
{
    return NULL;
}
#endif

#if (0 == AUDIO_KARAOKE_AUXTRACK_EN)
void* create_auxtrack_stream(void)
{
    return NULL;
}
#endif

#if (0 == RECORD_STREAM_TYPE)
void* create_flash_rec_stream(void)
{
    return NULL;
}
#endif