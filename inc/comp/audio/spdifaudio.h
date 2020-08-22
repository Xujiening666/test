#ifndef _SPDIFAUDIO_H_
#define _SPDIFAUDIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "FreeRTOS.h"
#include "task.h"
#include "audio_out.h"
#include "audio_in.h"

enum spdifaudio_status_e {
    SPDIFAUDIO_STATUS_IDLE,
    SPDIFAUDIO_STATUS_ACTIVE,
};

enum spdifaudio_cmd_e {
    SPDIFAUDIO_CMD_NONE,

    /* start play spdif audio command;
     * The command argument is ignored
     * NOTE: This command is an internal command. APP should not use it
     */
    SPDIFAUDIO_CMD_START,

    /* stop play spdif audio command;
     * The command argument is ignored
     * NOTE: This command is an internal command. APP should not use it
     */
    SPDIFAUDIO_CMD_STOP,

    /* mute/unmute spdif audio
     * 命令参数表示mute或者unmute
     * Example:
     *    // mute spidf audio
     *    spdifaudio_cmd(SPDIFAUDIO_CMD_MUTE, 1);
     *    // unmute spdif audio
     *    spdifaudio_cmd(SPDIFAUDIO_CMD_MUTE, 0);
     */
    SPDIFAUDIO_CMD_MUTE,

    /* 获取mute状态
     * 命令参数是一个uint32_t指针
     * Example:
     *    uint32_t is_mute;
     *    spdifaudio_cmd(SPDIFAUDIO_CMD_GET_MUTE, (uint32_t)&is_mute);
     */
    SPDIFAUDIO_CMD_GET_MUTE,

    /* 等待所有命令处理完成
     * 命令参数无意义
     * NOTE:这是内部命令，APP不要使用它
     * Example:
     *    spdifaudio_cmd(SPDIFAUDIO_CMD_WAIT_DONE, 1);
     */
    SPDIFAUDIO_CMD_WAIT_DONE,
};

/* 给spdifaudio发送一个命令
 * @param cmd: 参考@enum spdifaudio_cmd_e
 * @param val: 命令参数
 * @return: 成功返回0；失败返回负值错误码
 */
int32_t spdifaudio_cmd(uint32_t cmd, uint32_t val);


/***************************************************************************
 *               spdif audio API
 ***************************************************************************/
//创建一个spdifaudio实例
void create_spdifaudio(bool mute);

//销毁一个spdifaudio实例
void destroy_spdifaudio(void);

//mute/unmute spdifaudio
#define spdifaudio_mute(mute) spdifaudio_cmd(SPDIFAUDIO_CMD_MUTE, mute)

//获取spdifaudio mute状态
#define spdifaudio_is_muted() \
    ({ \
        uint32_t is_mute; \
        spdifaudio_cmd(SPDIFAUDIO_CMD_GET_MUTE, (uint32_t)&is_mute); \
        is_mute; \
    })

#ifdef __cplusplus
}
#endif

#endif /* _SPDIFAUDIO_H_ */
