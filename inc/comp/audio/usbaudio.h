/* The Alps Open Source Project
 * Copyright (c) 2016 - 2017 Smartlink Technology Co. Ltd. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _USBAUDIO_H_
#define _USBAUDIO_H_

#include "sys_types.h"

#include <FreeRTOS.h>
#include <portmacro.h>
#include <task.h>
#include <semphr.h>

#include "sltk_audio.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    USBAUDIO_CMD_NONE   = 0,

    /* usbaudio start play command;
     * The command argument is ignored
     * NOTE: This command is an internal command. APP should not use it
     */
    USBAUDIO_CMD_START_PLAYBACK     = 1,

    /* usbaudio stop play command;
     * The command argument is ignored
     * NOTE: This command is an internal command. APP should not use it
     */
    USBAUDIO_CMD_STOP_PLAYBACK      = 2,

    /* usbaudio start record command;
     * The command argument is ignored
     * NOTE: This command is an internal command. APP should not use it
     */
    USBAUDIO_CMD_START_RECORD       = 3,
    /* usbaudio stop record command;
     * The command argument is ignored
     * NOTE: This command is an internal command. APP should not use it
     */
    USBAUDIO_CMD_STOP_RECORD        = 4,

    /* wait all command done;
     * The command argument is ignored
     * Example:
     *    usbaudio_cmd(USBAUDIO_CMD_WAIT_DONE, 1);
     */
    USBAUDIO_CMD_WAIT_DONE          = 5,

    USBAUDIO_CMD_PLAYBACK_SET_MUTE  = 6,
    USBAUDIO_CMD_PLAYBACK_GET_MUTE  = 7,
    USBAUDIO_CMD_RECORD_SET_MUTE    = 8,
    USBAUDIO_CMD_RECORD_GET_MUTE    = 9,
} usbaudio_cmd_t;

typedef enum {
    USBAUDIO_STATUS_IDLE,
    USBAUDIO_STATUS_ACTIVE,
} usbaudio_status_t;

typedef struct {
    /* playback sample rate */
    uint32_t playback_fs;
    /* playback sample size; 2:16bit, 4:32bit */
    uint8_t playback_sample_size;
    /* playback channels */
    uint8_t playback_ch;
    /* Indicates how much pcm data is transferred per
     * call to function @usbaudio_playback_write
     */
    uint16_t playback_samples;

    /* record sample rate */
    uint32_t record_fs;
    /* record sample size; 2:16bit, 4:32bit */
    uint8_t record_sample_size;
    /* record channels */
    uint8_t record_ch;
    /* Indicates how much pcm data is transferred per
     * call to function @usbaudio_record_read
     */
    uint16_t record_samples;
} usbaudio_init_t;

/* send a command to the usbaudio
 * @param cmd: usbaudio command, see @usbaudio_cmd_t
 * @param val: usbaudio command argument
 * @return: return zero on success, a negative errno value on failure
 */
int32_t usbaudio_cmd(uint32_t cmd, uint32_t val);


/***************************************************************************
 *               usb audio API
 ***************************************************************************/
/* 创建一个usbaudio实例
 * @param params: usbaudio初始化参数
 */
void create_usbaudio(usbaudio_init_t *params);

//销毁一个usbaudio实例
void destroy_usbaudio(void);


//设置USB audio播放音量(范围:0~100)
#if USB_AUDIO_ALL_MODE_SUPPORT
#define usbaudio_playback_set_volume(vol) \
    usb_audio_source_ctrl_set_volume(vol)
void usb_audio_source_ctrl_set_volume(uint8_t volume);
#else
#define usbaudio_playback_set_volume(vol) \
    audio_service_set_playback_volume(vol)
#endif

//获取USB audio播放音量(范围:0~100)
#if USB_AUDIO_ALL_MODE_SUPPORT
#define usbaudio_playback_get_volume() \
    usb_audio_source_ctrl_get_volume()
uint8_t usb_audio_source_ctrl_get_volume(void);
#else
#define usbaudio_playback_get_volume() \
    audio_service_get_playback_volume()
#endif

// mute/unmute USB audio播放
#if USB_AUDIO_ALL_MODE_SUPPORT
#define usbaudio_playback_set_mute(mute) \
    usb_audio_source_ctrl_set_mute(mute)
void usb_audio_source_ctrl_set_mute(bool mute);
#else
#define usbaudio_playback_set_mute(mute) \
    usbaudio_cmd(USBAUDIO_CMD_PLAYBACK_SET_MUTE, mute)
#endif

#if USB_AUDIO_ALL_MODE_SUPPORT
#define usbaudio_playback_get_mute() \
    usb_audio_source_ctrl_get_mute()
bool usb_audio_source_ctrl_get_mute(void);
#else
#define usbaudio_playback_get_mute() \
    ({ \
        uint32_t muted; \
        usbaudio_cmd(USBAUDIO_CMD_PLAYBACK_GET_MUTE, (uint32_t)&muted); \
        muted; \
    })
#endif

// 设置USB audio录音音量
#if USB_AUDIO_ALL_MODE_SUPPORT
#define usbaudio_record_set_volume(vol) \
    usb_audio_sink_ctrl_set_volume(vol)
void usb_audio_sink_ctrl_set_volume(uint8_t volume);
#else
#define usbaudio_record_set_volume(vol) \
    audio_service_set_record_volume(vol)
#endif

// 获取USB audio录音音量
#if USB_AUDIO_ALL_MODE_SUPPORT
#define usbaudio_record_get_volume() \
    usb_audio_sink_ctrl_get_volume()
uint8_t usb_audio_sink_ctrl_get_volume(void);
#else
#define usbaudio_record_get_volume() \
    audio_service_get_record_volume()
#endif

/* 配置USB audio录音的位置
 * @param capture_type: capture music data or record data;
 *                      see @enum audio_capture_type_e
 *
 * Example:
 *    //录音乐数据
 *    usbaudio_record_set_type(AUDIO_CAPTURE_TYPE_MUSIC_AFX_OUTPUT);
 *    //录MIC算法前数据
 *    usbaudio_record_set_type(AUDIO_CAPTURE_TYPE_MIC_AFX_INPUT);
 *    //录MIC算法后数据
 *    usbaudio_record_set_type(AUDIO_CAPTURE_TYPE_MIC_AFX_OUTPUT);
 *    //录MIC+Music数据（USB IN数据除外）
 *    usbaudio_record_set_type(AUDIO_CAPTURE_TYPE_MUSIC_MIC_MIX);
 *    //录所有数据（包含USB IN数据）
 *    usbaudio_record_set_type(AUDIO_CAPTURE_TYPE_ALL);
 */
#if USB_AUDIO_ALL_MODE_SUPPORT
#define usbaudio_record_set_type(capture_type) \
    usb_auido_sink_ctrl_set_capture_type(capture_type)
void usb_auido_sink_ctrl_set_capture_type(uint8_t capture_type);
#else
#define usbaudio_record_set_type(capture_type) do{} while(0)
#endif

// mute/unmute USB audio录音
#if USB_AUDIO_ALL_MODE_SUPPORT
#define usbaudio_record_set_mute(mute) \
    usb_audio_sink_ctrl_set_mute(mute)
void usb_audio_sink_ctrl_set_mute(bool mute);
#else
#define usbaudio_record_set_mute(mute) \
    usbaudio_cmd(USBAUDIO_CMD_RECORD_SET_MUTE, mute)
#endif

// 获取USB audio录音mute状态
#if USB_AUDIO_ALL_MODE_SUPPORT
#define usbaudio_record_get_mute() \
    usb_audio_sink_ctrl_get_mute()
bool usb_audio_sink_ctrl_get_mute(void);
#else
#define usbaudio_record_get_mute() \
    ({ \
        uint32_t muted; \
        usbaudio_cmd(USBAUDIO_CMD_RECORD_GET_MUTE, (uint32_t)&muted); \
        muted; \
    })
#endif

/* write pcm data to the usbaudio
 * @param buf: pcm buf
 * @param samples: 多少个采样点
 * @return: return zero on success, a negative errno value on failure
 */
#if USB_AUDIO_ALL_MODE_SUPPORT
#define usbaudio_playback_write_data(buf, samples) \
    usb_audio_source_push_data(buf, samples)
void usb_audio_source_push_data(void *buf, uint32_t samples);
#else
#define usbaudio_playback_write_data(buf, samples) \
    usbaudio_playback_write(buf, samples)
int32_t usbaudio_playback_write(void *buf, uint32_t samples);
#endif

/* read pcm data from the usbaudio
 * @param buf: pcm buf
 * @param samples: 多少个采样点
 * @return: return zero on success, a negative errno value on failure
 */
#if USB_AUDIO_ALL_MODE_SUPPORT
#define usbaudio_record_read_data(buf, samples) \
    usb_audio_sink_pull_data(buf, samples)
void usb_audio_sink_pull_data(void *buf, uint32_t samples);
#else
#define usbaudio_record_read_data(buf, samples) \
    usbaudio_record_read(buf, samples)
int32_t usbaudio_record_read(void *buf, uint32_t samples);
#endif

/*
 * USB audio播放预留处理接口，此接口会被audio库调用。如果需要，请实现此处理函数
 */
void app_usb_audio_source_process(audio_stream_info_t *info);

/*
 * USB audio录音预留处理接口，此接口会被audio库调用。如果需要，请实现此处理函数
 */
void app_usb_audio_sink_process(audio_stream_info_t *info);

#ifdef __cplusplus
}
#endif

#endif /* _USBAUDIO_H_ */
