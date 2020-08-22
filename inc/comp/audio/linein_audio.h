#ifndef _LINEIN_AUDIO_H_
#define _LINEIN_AUDIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS.h"
#include "task.h"
#include "audio_out.h"
#include "audio_in.h"
#include "audio_service.h"

//linein audio状态定义
enum linein_audio_status_e {
    LINEIN_AUDIO_STATUS_IDLE,
    LINEIN_AUDIO_STATUS_ACTIVE,
};

//linein audio命令定义
enum linein_audio_cmd_e {
    LINEIN_AUDIO_CMD_NONE,

    /* 开始播放linein
     * 命令参数无意义
     * NOTE:这是内部命令，APP不要使用它
     */
    LINEIN_AUDIO_CMD_START,

    /* 停止播放linein
     * 命令参数无意义
     * NOTE:这是内部命令，APP不要使用它
     */
    LINEIN_AUDIO_CMD_STOP,

    /* mute/unmute linein audio command;
     * 命令参数表示mute或者unmute
     * Example:
     *    // mute linein audio
     *    linein_audio_cmd(LINEIN_AUDIO_CMD_MUTE, 1);
     *    // unmute linein audio
     *    linein_audio_cmd(LINEIN_AUDIO_CMD_MUTE, 0);
     */
    LINEIN_AUDIO_CMD_MUTE,

    /* 获取mute状态
     * 命令参数是一个uint32_t指针
     * Example:
     *    uint32_t is_mute;
     *    linein_audio_cmd(LINEIN_AUDIO_CMD_GET_MUTE, (uint32_t)&is_mute);
     */
    LINEIN_AUDIO_CMD_GET_MUTE,

    /* 等待所有命令处理完成
     * 命令参数无意义
     * Example:
     *    linein_audio_cmd(LINEIN_AUDIO_CMD_WAIT_DONE, 1);
     */
    LINEIN_AUDIO_CMD_WAIT_DONE,

    /* Register a recording callback function to linein_audio;
     * The command argument is a @karaoke_record_callback_context_t pointer
     * NOTE: 这是内部命令，APP不要使用它
     * Example:
     *  karaoke_record_callback_context_t cb_context;
     *  cb_context.func = recorder_ain_push_data;
     *  cb_context.handle = ain;
     *  linein_audio_cmd(LINEIN_AUDIO_CMD_SET_RECORD_CALLBACK, (uint32_t)&cb_context);
     */
    LINEIN_AUDIO_CMD_SET_RECORD_CALLBACK,
};

/* 给linein audio发送一个命令
 * @param cmd: 参考@enum linein_audio_cmd_e
 * @param val: 命令参数
 * @return: 成功返回0，失败返回负值错误码
 */
int32_t linein_audio_cmd(uint32_t cmd, uint32_t val);



/***************************************************************************
 *               linein audio API
 ***************************************************************************/
//创建一个linein audio实例
void create_linein_audio(void);

//销毁一个linein audio实例
void destroy_linein_audio(void);

//mute/unmute linein播放
#if LINEIN_ALL_MODE_SUPPORT
void linein_audio_source_ctrl_set_mute(bool mute);
#define linein_audio_mute(mute) linein_audio_source_ctrl_set_mute(mute)
#else
#define linein_audio_mute(mute) linein_audio_cmd(LINEIN_AUDIO_CMD_MUTE, mute)
#endif

//获取linein audio muted状态
#if LINEIN_ALL_MODE_SUPPORT
bool linein_audio_source_ctrl_get_mute(void);
#define linein_audio_is_muted() linein_audio_source_ctrl_get_mute()
#else
#define linein_audio_is_muted() \
    ({ \
        uint32_t is_mute = 0; \
        linein_audio_cmd(LINEIN_AUDIO_CMD_GET_MUTE, (uint32_t)&is_mute); \
        is_mute; \
    })
#endif

//设置linein audio音量(音量范围：0 ~ 100)
#if LINEIN_ALL_MODE_SUPPORT
void linein_audio_source_ctrl_set_volume(uint8_t volume);
#define linein_audio_set_volume(vol)   linein_audio_source_ctrl_set_volume(vol)
#else
#define linein_audio_set_volume(vol)   audio_service_set_playback_volume(vol)
#endif

//获取linein audio音量(音量范围：0 ~ 100)
#if LINEIN_ALL_MODE_SUPPORT
uint8_t linein_audio_source_ctrl_get_volume(void);
#define linein_audio_get_volume() linein_audio_source_ctrl_get_volume()
#else
#define linein_audio_get_volume() audio_service_get_playback_volume()
#endif

/*
 * linein播放预留处理接口，此接口会被audio库调用。如果需要，请实现此处理函数
 */
void app_linein_audio_source_process(audio_stream_info_t *info);


#ifdef __cplusplus
}
#endif

#endif /* _LINEIN_AUDIO_H_ */
