//#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "scene_manager.h"
#include "audiopath_conf.h"



#if (RECORD_LINEIN_FUNC || RECORD_FM_FUNC || LINEIN_ALL_MODE_SUPPORT || USB_AUDIO_ALL_MODE_SUPPORT)
#define KARAOKE_BUF_SZ      (24*1024) //linein录音调整以加大码流buffer到16k
#else
#define KARAOKE_BUF_SZ      (38*1024)
#endif

#define SND_OUT_MAX_SAMPLES     (256*2)

#define SND_IN_MAX_SAMPLES      (256*2)

#define EWS_SND_OUT_MAX_SAMPLES (256*50)

#if BT_EWS_EN
#define SND_OUT_BUF_SIZE(ch, sample_size) \
    (EWS_SND_OUT_MAX_SAMPLES * (ch) * (sample_size))
#else
#define SND_OUT_BUF_SIZE(ch, sample_size) \
    (SND_OUT_MAX_SAMPLES * (ch) * (sample_size))
#endif

#define SND_IN_BUF_SIZE(ch, sample_size) \
    (SND_IN_MAX_SAMPLES * (ch) * (sample_size))

const uint32_t KARAOKE_AFX_BUF_SZ = KARAOKE_BUF_SZ;


#define AUDIO_HEAP_SIZE   (100*1024)
#define SND_OUT_SIZE      SND_OUT_BUF_SIZE(AUDIO_PLAYBACK_DEVICE_CH, AUDIO_PLAYBACK_DEVICE_SAMPLE_SIZE)
#define SND_IN_SIZE       SND_IN_BUF_SIZE(AUDIO_REC_DEVICE_MAX_CH, AUDIO_REC_DEVICE_SAMPLE_SIZE)
#define DEC_BUF_SIZE      (AUDIO_HEAP_SIZE - SND_OUT_SIZE - SND_IN_SIZE)

typedef struct audio_heap_buffer_s {
    uint8_t dec_buffer[DEC_BUF_SIZE];
    uint8_t snd_out_buffer[SND_OUT_SIZE];
    uint8_t snd_in_buffer[SND_IN_SIZE];
} audio_heap_buffer_t;

AT(.audio_heap)
audio_heap_buffer_t audio_heap_buffer;

void get_decbuf(void **pbuf, uint32_t *pbytes)
{
    *pbuf = &(audio_heap_buffer.dec_buffer);
    *pbytes = DEC_BUF_SIZE;
}

void get_sndoutbuf(void **pbuf, uint32_t *pbytes)
{
    *pbuf = &(audio_heap_buffer.snd_out_buffer);
    *pbytes = SND_OUT_SIZE;
}

void get_sndinbuf(void **pbuf, uint32_t *pbytes)
{
    *pbuf = &(audio_heap_buffer.snd_in_buffer);
    *pbytes = SND_IN_SIZE;
}
