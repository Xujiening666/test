#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "audio_service.h"

#if 0
int16_t data48k_0dB_mono_16bit[48] = {
 0x0000,0x10B5,0x2120,0x30FB,
 0x3FFF,0x4DEB,0x5A82,0x658C,
 0x6ED9,0x7641,0x7BA3,0x7EE7,
 0x7FFF,0x7EE7,0x7BA3,0x7641,
 0x6ED9,0x658C,0x5A82,0x4DEB,
 0x3FFF,0x30FB,0x2120,0x10B5,
 0x0000,0xEF4A,0xDEDF,0xCF04,
 0xC000,0xB214,0xA57D,0x9A73,
 0x9126,0x89BE,0x845C,0x8118,
 0x8000,0x8118,0x845C,0x89BE,
 0x9126,0x9A73,0xA57D,0xB214,
 0xC000,0xCF04,0xDEDF,0xEF4A
 };
 #endif

#if 0
int16_t data48k_0dB_stereo_16bit[96] = {
 0x0000,0x0000,0x10B5,0x10B5,0x2120,0x2120,0x30FB,0x30FB,
 0x3FFF,0x3FFF,0x4DEB,0x4DEB,0x5A82,0x5A82,0x658C,0x658C,
 0x6ED9,0x6ED9,0x7641,0x7641,0x7BA3,0x7BA3,0x7EE7,0x7EE7,
 0x7FFF,0x7FFF,0x7EE7,0x7EE7,0x7BA3,0x7BA3,0x7641,0x7641,
 0x6ED9,0x6ED9,0x658C,0x658C,0x5A82,0x5A82,0x4DEB,0x4DEB,
 0x3FFF,0x3FFF,0x30FB,0x30FB,0x2120,0x2120,0x10B5,0x10B5,
 0x0000,0x0000,0xEF4A,0xEF4A,0xDEDF,0xDEDF,0xCF04,0xCF04,
 0xC000,0xC000,0xB214,0xB214,0xA57D,0xA57D,0x9A73,0x9A73,
 0x9126,0x9126,0x89BE,0x89BE,0x845C,0x845C,0x8118,0x8118,
 0x8000,0x8000,0x8118,0x8118,0x845C,0x845C,0x89BE,0x89BE,
 0x9126,0x9126,0x9A73,0x9A73,0xA57D,0xA57D,0xB214,0xB214,
 0xC000,0xC000,0xCF04,0xCF04,0xDEDF,0xDEDF,0xEF4A,0xEF4A
 };
#endif

#if 0
int32_t data48k_0dB_mono_32bit[48] = {
 0x00000000,0x10B5150F,0x2120FB82,0x30FBC54C,
 0x3FFFFFFE,0x4DEBE4FD,0x5A827998,0x658C9A2B,
 0x6ED9EB9F,0x7641AF3A,0x7BA3751B,0x7EE7AA49,
 0x7FFFFFFD,0x7EE7AA49,0x7BA3751B,0x7641AF3A,
 0x6ED9EB9F,0x658C9A2B,0x5A827998,0x4DEBE4FD,
 0x3FFFFFFE,0x30FBC54C,0x2120FB82,0x10B5150F,
 0x00000000,0xEF4AEAF0,0xDEDF047D,0xCF043AB3,
 0xC0000001,0xB2141B02,0xA57D8667,0x9A7365D4,
 0x91261460,0x89BE50C5,0x845C8AE4,0x811855B6,
 0x80000002,0x811855B6,0x845C8AE4,0x89BE50C5,
 0x91261460,0x9A7365D4,0xA57D8667,0xB2141B02,
 0xC0000001,0xCF043AB3,0xDEDF047D,0xEF4AEAF0
 };
 #endif

#if 0 /* 正弦波调试示例代码 */
static int32_t data48k_0dB_stereo_32bit[96]={
 0x00000000,0x00000000,0x10B5150F,0x10B5150F,0x2120FB82,0x2120FB82,0x30FBC54C,0x30FBC54C,
 0x3FFFFFFE,0x3FFFFFFE,0x4DEBE4FD,0x4DEBE4FD,0x5A827998,0x5A827998,0x658C9A2B,0x658C9A2B,
 0x6ED9EB9F,0x6ED9EB9F,0x7641AF3A,0x7641AF3A,0x7BA3751B,0x7BA3751B,0x7EE7AA49,0x7EE7AA49,
 0x7FFFFFFD,0x7FFFFFFD,0x7EE7AA49,0x7EE7AA49,0x7BA3751B,0x7BA3751B,0x7641AF3A,0x7641AF3A,
 0x6ED9EB9F,0x6ED9EB9F,0x658C9A2B,0x658C9A2B,0x5A827998,0x5A827998,0x4DEBE4FD,0x4DEBE4FD,
 0x3FFFFFFE,0x3FFFFFFE,0x30FBC54C,0x30FBC54C,0x2120FB82,0x2120FB82,0x10B5150F,0x10B5150F,
 0x00000000,0x00000000,0xEF4AEAF0,0xEF4AEAF0,0xDEDF047D,0xDEDF047D,0xCF043AB3,0xCF043AB3,
 0xC0000001,0xC0000001,0xB2141B02,0xB2141B02,0xA57D8667,0xA57D8667,0x9A7365D4,0x9A7365D4,
 0x91261460,0x91261460,0x89BE50C5,0x89BE50C5,0x845C8AE4,0x845C8AE4,0x811855B6,0x811855B6,
 0x80000002,0x80000002,0x811855B6,0x811855B6,0x845C8AE4,0x845C8AE4,0x89BE50C5,0x89BE50C5,
 0x91261460,0x91261460,0x9A7365D4,0x9A7365D4,0xA57D8667,0xA57D8667,0xB2141B02,0xB2141B02,
 0xC0000001,0xC0000001,0xCF043AB3,0xCF043AB3,0xDEDF047D,0xDEDF047D,0xEF4AEAF0,0xEF4AEAF0
 };
static uint32_t data_index;

/* 使用正弦波数据代替原音频数据 */
AT(.com_seg)
void sin_debug(audio_stream_info_t *info)
{
    if (info->sample_size == 2) {
        int16_t *p16 = (int16_t*)info->buf;
        if (2 == info->channels) {
            for (int i = 0; i < info->samples; i++) {
            p16[2*i+1] = p16[2*i] = data48k_0dB_stereo_32bit[data_index] >> 16;
            data_index = (data_index + 2) % 96;
            }
        } else {
            for (int i = 0; i < info->samples; i++) {
                p16[i] = data48k_0dB_stereo_32bit[data_index] >> 16;
                data_index = (data_index + 2) % 96;
            }
        }
    } else {
        int32_t *p32 = (int32_t*)info->buf;
        if (2 == info->channels) {
            for (int i = 0; i < info->samples; i++) {
                p32[2*i+1] = p32[2*i] = data48k_0dB_stereo_32bit[data_index];
                data_index = (data_index + 2) % 96;
            }
        } else {
            for (int i = 0; i < info->samples; i++) {
                p32[i]= data48k_0dB_stereo_32bit[data_index];
                data_index = (data_index + 2) % 96;
            }
        }
    }
}
#endif

/*
 * 本地/蓝牙音乐预留处理接口，此接口会被audio库调用。如果需要，请实现此处理函数
 */
AT(.audiotrack_sram_seg)
void app_music_process(audio_stream_info_t *info)
{
}

/*
 * USB audio播放预留处理接口，此接口会被audio库调用。如果需要，请实现此处理函数
 */
AT(.usb_audio_src_sram_seg)
void app_usb_audio_source_process(audio_stream_info_t *info)
{
}

/*
 * USB audio录音预留处理接口，此接口会被audio库调用。如果需要，请实现此处理函数
 */
AT(.usb_audio_sink_sram_seg)
void app_usb_audio_sink_process(audio_stream_info_t *info)
{
}

/*
 * linein播放预留处理接口，此接口会被audio库调用。如果需要，请实现此处理函数
 */
AT(.linein_audio_src_sram_seg)
void app_linein_audio_source_process(audio_stream_info_t *info)
{
    //logd("samples:%d,%d,%d,%d", info->samples, info->fs, info->channels, info->sample_size);

#if 0 /* 使用右声道数据覆盖左声道数据示例代码 */
    //copy data of linein R to linein L:
    if (info->channels == 2) {
        if (info->sample_size == sizeof(int16_t)) {
            int16_t *p16 = (int16_t*)info->buf;
            for (int i = 0; i < info->samples; i++) {
                p16[2*i] = p16[2*i+1];
            }
        } else {
            int32_t *p32 = (int32_t*)info->buf;
            for (int i = 0; i < info->samples; i++) {
                p32[2*i] = p32[2*i+1];
            }
        }
    }
#endif
}

/*
 * DAC PCM数据预留处理接口，此接口会被audio库调用。如果需要，请实现此处理函数
 */
AT(.dac_aout_sram_seg)
void app_dac_out_audio_process(audio_stream_info_t *info)
{
    //logd("samples:%d,%d,%d,%d", info->samples, info->fs, info->channels, info->sample_size);
}

/*
 * I2S out PCM数据预留处理接口，此接口会被audio库调用。如果需要，请实现此处理函数
 */
AT(.dac_aout_sram_seg)
void app_i2s_out_audio_process(audio_stream_info_t *info)
{
}

/*
 * ADC PCM数据预留处理接口，此接口会被audio库调用。如果需要，请实现此处理函数
 */
AT(.adc_ain_sram_seg)
void app_adc_in_audio_process(audio_stream_info_t *info)
{
}

/*
 * I2S in PCM数据预留处理接口，此接口会被audio库调用。如果需要，请实现此处理函数
 */
AT(.adc_ain_sram_seg)
void app_i2s_in_audio_process(audio_stream_info_t *info)
{
}

/*
 * SPDIF PCM数据预留处理接口，此接口会被audio库调用。如果需要，请实现此处理函数
 */
void app_spdif_in_audio_process(audio_stream_info_t *info)
{
}

/*
 * sco 正常pcm包处理预留接口，此接口会被audio库调用。如果需要，请实现此处理函数
 */
AT(.sco_audio_sram_seg)
void app_sco_normal_packet_pcm_process(audio_stream_info_t *info)
{
}

/*
 * sco 异常pcm包处理预留接口，此接口会被audio库调用。如果需要，请实现此处理函数
 */
AT(.sco_audio_sram_seg)
void app_sco_bad_packet_pcm_process(audio_stream_info_t *info)
{
}

/*
 * sco通话算法输入pcm数据预留接口，此接口会被audio库调用。如果需要，请实现此处理函数
 * @param dlbuf: 下行pcm数据
 * @param ulbuf: 上行pcm数据
 * @param samples: 采样点数
 */
AT(.sco_audio_sram_seg)
void app_sco_aenc_input_pcm_process(int16_t *dlbuf,
    int16_t *ulbuf, uint32_t samples)
{
}

/*
 * sco通话算法输出pcm数据预留接口，此接口会被audio库调用。如果需要，请实现此处理函数
 * @param dlbuf: 下行pcm数据
 * @param ulbuf: 上行pcm数据
 * @param samples: 采样点数
 */
AT(.sco_audio_sram_seg)
void app_sco_aenc_output_pcm_process(int16_t *dlbuf,
    int16_t *ulbuf, uint32_t samples)
{
}

/* The sco uplink bitstream process function, will be called by
 * the audio library. If necessary, please implement this function
 */

/*
 * sco通话上行比特流（cvsd或msbc数据流）预留处理函数，此接口会被audio库调用。如果需要，请实现此处理函数
 * @param buf: 比特流数据buffer
 * @param bytes: 数据长度
 * @param msbc: true:msbc数据；false:cvsd数据
 */
AT(.sco_audio_sram_seg)
void app_sco_uplink_bitstream_process(uint8_t *buf,
    uint32_t bytes, bool msbc)
{
}

/*
 * sco通话下行比特流（cvsd或msbc数据流）预留处理函数，此接口会被audio库调用。如果需要，请实现此处理函数
 * @param buf: 比特流数据buffer
 * @param bytes: 数据长度
 * @param msbc: true:msbc数据；false:cvsd数据
 */
AT(.sco_audio_sram_seg)
void app_sco_downlink_bitstream_process(uint8_t *buf,
    uint32_t bytes, bool msbc)
{
}

/* ANC处理回调函数， 此函数会被ANC中断调用
 * @param pL: 存放左声道数据
 * @param pL: 存放右声道数据
 */
AT(.auss_anc_sram_seg)
void app_anc_data_process(int32_t *pL, int32_t *pR)
{
    //*pL = *pR = auss_anc_read_ch2();
    //*pL = auss_anc_read_ch3() << 8; *pR = auss_anc_read_ch4() << 8;
    //*pL = auss_anc_read_ch1() << 8; *pR = auss_anc_read_ch2() << 8;

#if AUDIO_ADJUST_HPLR_LINEOUTLR
    /* HP和LINEOUT独立调节音量功能在ANC中断读取PCM数据，做音量运算后写入ANC数据寄存器进行播放 */
    extern void anc_get_dac_data(int32_t *pL, int32_t *pR);
    anc_get_dac_data(pL, pR);
    extern void func_clbk(int32_t *pL, int32_t *pR);
    func_clbk(pL, pR);
#endif
}

