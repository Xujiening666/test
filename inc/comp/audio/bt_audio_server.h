#ifndef _BT_AUDIO_SERVER_H_
#define _BT_AUDIO_SERVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "sys_inc.h"
#include "audio_out.h"
#include "audio_in.h"
#include "event.h"

enum bt_audio_status_e {
    BT_AUDIO_STATUS_IDLE,
    BT_AUDIO_STATUS_A2DP,       // 表示当前处于蓝牙音乐模式
    BT_AUDIO_STATUS_SCO,        // 表示当前处于通话模式
    BT_AUDIO_STATUS_TRANSITION, // 表示正在处理command
};

enum bt_audio_cmd_e {
    BT_AUDIO_CMD_NONE,

    /* 开始蓝牙音乐播放
     * 命令参数表示最大等待命令处理的时间
     * Example:
     *    bt_audio_server_cmd(BT_AUDIO_CMD_A2DP_START, portMAX_DELAY);
     */
    BT_AUDIO_CMD_A2DP_START,

    /* 蓝牙音乐淡出处理命令
     * 命令参数无意义
     * Example:
     *    bt_audio_server_cmd(BT_AUDIO_CMD_A2DP_FADEOUT, 1);
     * NOTE: 这是内部command， APP不要使用它
     */
    BT_AUDIO_CMD_A2DP_FADEOUT,

    /* 停止播放蓝牙音乐
     * 命令参数表示最大等待命令处理的时间
     * Example:
     *    bt_audio_server_cmd(BT_AUDIO_CMD_A2DP_STOP, portMAX_DELAY);
     */
    BT_AUDIO_CMD_A2DP_STOP,

    /* 通话开始命令
     * 命令参数表示最大等待命令处理的时间
     * Example:
     *    bt_audio_server_cmd(BT_AUDIO_CMD_SCO_START, portMAX_DELAY);
     */
    BT_AUDIO_CMD_SCO_START,

    /* 通话停止命令
     * 命令参数表示最大等待命令处理的时间
     * Example:
     *    bt_audio_server_cmd(BT_AUDIO_CMD_SCO_STOP, portMAX_DELAY);
     */
    BT_AUDIO_CMD_SCO_STOP,

    /* 配置当前的音量（通话算法会根据音量作相应的算法处理调整)
     * 命令参数为当前音量值
     * Example:
     *    bt_audio_server_cmd(BT_AUDIO_CMD_SET_VOLUME, current_vol);
     */
    BT_AUDIO_CMD_SET_VOLUME,

    /* 跳过音乐数据不播放
     * 命令参数表示跳过多少毫秒的音乐数据不播放
     * Example:
     *    //skip 500 ms
     *    bt_audio_server_cmd(BT_AUDIO_CMD_SET_A2DP_SKIP_TIME, 500);
     */
    BT_AUDIO_CMD_SET_A2DP_SKIP_TIME,
};

/* 给bt_audio发送一个命令
 * @param cmd: 参考 @enum bt_audio_cmd_e
 * @param val: 命令参数
 */
void bt_audio_server_cmd(uint32_t cmd, uint32_t val);


/***************************************************************************
 *               bt audio API
 ***************************************************************************/
//创建一个bt_audio实例
void bt_audio_server_create(void);

//销毁一个bt_audio实例
void bt_audio_server_destroy(void);

/* 获取bt_audio当前的状态
 * @return: 返回状态，参考@enum bt_audio_status_e
 */
uint8_t bt_audio_sta_get(void);

//开始播放蓝牙音乐
#define bt_audio_a2dp_start()  \
    ({ \
        if (AUDIOFOCUS_HOLDER_AUDIOPLAYER == get_audiofocus_holder()) { \
            player_close(0); \
            player_set_next_file_func(NULL); \
        } \
        bt_audio_server_cmd(BT_AUDIO_CMD_A2DP_START, portMAX_DELAY); \
    })

//停止播放蓝牙音乐
#define bt_audio_a2dp_stop()                bt_audio_server_cmd(BT_AUDIO_CMD_A2DP_STOP, portMAX_DELAY)

//开始通话
#define bt_audio_sco_start()  \
    ({ \
        if (AUDIOFOCUS_HOLDER_AUDIOPLAYER == get_audiofocus_holder()) { \
            player_close(0); \
            player_set_next_file_func(NULL); \
        } \
        bt_audio_server_cmd(BT_AUDIO_CMD_SCO_START, portMAX_DELAY); \
    })

//停止通话
#define bt_audio_sco_stop()                 bt_audio_server_cmd(BT_AUDIO_CMD_SCO_STOP, portMAX_DELAY)

//通知bt_audio当前的音量
#define bt_audio_set_volume(vol)            bt_audio_server_cmd(BT_AUDIO_CMD_SET_VOLUME, vol);

//跳过多少毫秒的音乐数据不播放
#define bt_audio_a2dp_skip_time(skip_ms)    bt_audio_server_cmd(BT_AUDIO_CMD_SET_A2DP_SKIP_TIME, skip_ms);


#ifdef __cplusplus
}
#endif

#endif /* _BT_AUDIO_SERVER_H_ */
