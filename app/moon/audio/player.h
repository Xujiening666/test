#ifndef _PLAYER_H_
#define _PLAYER_H_


#ifdef __cplusplus
extern "C" {
#endif


#include "FreeRTOS.h"
#include "task.h"
#include "audio_service.h"
#include "audio_player.h"
#include "event.h"
#include "disp.h"
#include "xfs.h"
#include "task_main.h"

/* FAST_STEP_MS use for fast backward and fast forward */
#define FAST_STEP_MS 5000

#define FAST_EVENT_PERIOD 500


enum play_direction_e {
    PLAY_NEXT,
    PLAY_PREV,
    PLAY_SELECT,
};

enum play_repeat_e {
    REPEAT_ALL,
    REPEAT_SINGLE,
    REPEAT_RANDOM,
    REPEAT_FOLDER,
    REPEAT_MAX = REPEAT_FOLDER,
};

enum common_tone_status_e {
    COMMON_TONE_STATUS_IDLE,
    COMMON_TONE_STATUS_PLAYING,
};

enum special_tone_status_e {
    TONE_STATUS_IDLE    = 0,
    TONE_STATUS_PLAYING,        //playing tone
    TONE_STATUS_ANSWERING,
    TONE_STATUS_INCOMING_EXIT,  //incoming exit
    TONE_STATUS_CALLING_ENTER,  //calling enter

    TONE_STATUS_FORWARD,
    TONE_STATUS_BACKWARD,
    TONE_STATUS_ALARM,          //clock alarm

    TONE_STATUS_COMMON,
} ;

struct play_time {
    uint32_t min;
    uint32_t sec;
    uint32_t ms;
};

struct file_manager {
    int32_t total_file;               /* 当前设备音频文件数量 */
    int32_t limit_min_num;            /* 限制播放的文件最小编号（文件夹循环播放使用到此变量) */
    int32_t limit_max_num;            /* 限制播放的文件最大编号（文件夹循环播放使用到此变量) */
    uint32_t sdcard_total_file;       /* SD卡音频文件数量 */
    uint32_t udisk_total_file;        /* U盘音频文件数量 */
    uint32_t sdcard_file_checksum;    /* SD卡文件检索的校验和（此参数用于标识是否是同一个SD卡） */
    uint32_t udisk_file_checksum;     /* U盘文件检索的校验和（此参数用于标识是否是同一个U盘） */
    uint32_t sdcard_file_fail_count;  /* 表示SD卡连续出现无法播放的文件个数 */
    uint32_t udisk_file_fail_count;   /* 表示U盘连续出现无法播放的文件个数 */
    bool sdcard_file_invalid : 1;     /* 当SD卡中所有文件都不可播放时，此变量被置为1 */
    bool udisk_file_invalid : 1;      /* 当U盘中所有文件都不可播放时，此变量被置为1 */
};

struct native_player {
    struct file_manager manager;            /* 文件管理 */
    struct play_time ptime;                 /* 播放时间 */

    uint32_t file_num;                      /* 当前的文件编号 */
    uint32_t total_dir_num;                 /* 总文件夹个数 */
    uint32_t dir_num;                       /* 当前文件夹编号 */

    /* fast_ms use for fast backward and fast forward */
    int32_t last_time;                     /* 快进快退前的播放时间点 */
    int32_t fast_ms;                       /* 总共快进快退的时间 */
    uint32_t fast_event_timestamp;         /* 快进快退事件时间戳 */

    uint8_t volume;                        /* 音乐音量（范围：0~100） */
    uint8_t btcall_volume;                 /* 通话音量（范围：0~100） */

    uint8_t repeat_mode;                   /* 重复播放模式 */

    volatile uint8_t common_tone_status;   /* 通用提示音状态（标识通用提示音是否在播放） */
    uint32_t common_tone_trigger_event;    /* 触发播放提示音的事件，播放完成后可根据此变量做不同的处理 */

    uint8_t breakpoint_wr;                 /* 0:null; 1: save break info; 2: clear; */
    uint8_t breakpoint_rd;                 /* 0:null; 1: read break info; */
    uint8_t play_direction;                /* 播放方向，决定播上一首还是播下一首或者选择指定文件号播放 */

    bool mute_for_fast : 1;                /* 快进快退时是否mute */
    bool resume_for_fast : 1;              /* 此变量标识快进快退完成后是否需要调用resume cmd */
    bool file_num_dirty : 1;               /* 标识文件编号是否已经过时（文件夹切换时用到此变量） */
    bool paused : 1;                       /* 标识是否是暂停状态 */
    bool fast : 1;                         /* 标识是否是快进快退状态 */
    bool wait_get_folder_range : 1;        /* 标识是否需要获取文件夹文件编号范围 */
    bool is_music_mode : 1;                /* 标识是否是music mode */
#if MUSIC_NUM_STEP10
    bool n10 : 1;                          /* 标识上下首时文件号是加减10还是加减1 */
#endif
};
extern struct native_player np;

enum file_manager_ret {
    FMG_OK              = 0,
    FMG_ERR             = -1,
    FMG_OPENDIR_ERR     = -2,
    FMG_READDIR_ERR     = -3,
    FMG_END_OF_DIR      = -4,
    FMG_NO_DIR          = -5,
    FMG_NO_FILE         = -6,
    FMG_NO_NEXT_FILE    = -7,
    FMG_NO_LAST_FILE    = -8,
};

enum player_ret {
    PLAYER_RET_SUCCESS      = 0,
    PLAYER_RET_DEV_OFFLINE  = -1,
    PLAYER_RET_DEV_NOFILE   = -2,
};

int file_manager_init(struct file_manager *m, char *root);
int file_manager_deinit(struct file_manager *m);

void startup_audio_service(void);
int player_init(void);

/* 切换模式时，播放器处理函数；
 * 此函数完成切换模式时播放器的清除或者初始化工作；
 * @param mode: 当前模式；
 * @param enter: true:表示进入该模式； false:表示退出该模式；
 */
void player_mode_switch_proc(uint8_t mode, bool enter);

/*
 * 等待通用提示音播放完成
 */
void player_wait_common_tone_done(void);

void player_exit_music_mode(void);

void player_deinit(void);

/* 播放提示音
 * @param tone_num: 提示音编号， 参考“tone.h"提示音编号定义；
 * @param block: true：阻塞直到按键切模式或者播放完成才退出此函数；
 *               false: 不等播放完成就退出此函数；
 * @return: 成功返回0； 失败返回负值错误码
 */
int play_tone(uint8_t tone_num, bool block);

/* 播放提示音链
 * 来电报号使用此函数实现
 * @param set_next_tone_name: 设置下一个提示音文件的函数；set_next_tone_name会
 * 由audio lib回调去自动获取下一个提示音进行播放
 * @return: 成功返回0； 失败返回负值错误码
 */
int play_tone_chain(int (*set_next_tone_name)(source_info_t *));


int play_file(int index, bool reset_pause_sta);

void player_proc_event(uint32_t event);

void set_play_direction(enum play_direction_e play_direction);
int auto_play_file(int32_t dir_inc);

int play_new_device(uint32_t dev_num, uint32_t direction, bool sd, bool scan);
int play_switch_device(uint32_t direction, bool back);

/* 通用提示音播放；
 * 此函数可在各个模式下接受某一个消息时播放提示音； 提示音播放完成后自动恢复先前的播放；
 * NOTE: mode提示音等标准SDK提示音使用的是play_tone, 不需要使用此接口;
 * @param tone_num: 提示音编号， 参考“tone.h"提示音编号定义；
 * @param trigger_event: 触发此提示音播放的事件（播放完成后可通过
 *                       get_common_tone_trigger_event()获取触发提示音是什么事件
 *                       根据不同事件做不同的事情
 * @return: 成功返回0； 失败返回负值错误码;
 */
int play_common_tone(uint8_t tone_num, uint32_t trigger_event);

//切换文件播放（next_file: false-上一首； true-下一首）
void play_switch_file(bool next_file);
//切换文件夹播放（next_folder: false-上一文件夹； true-下一文件夹）
void play_switch_folder(bool next_folder);

int play_or_pause(void);
//快进快退(forward: 0-快退；1-快进)
void fast_skip(bool forward);

int fast_end(void);
int volume_down(void);
int volume_up(void);
int volume_set(uint8_t vol);
int bt_call_volume_down(void);
int bt_call_volume_up(void);
int set_repeat(void);
int set_eq(void);
int get_play_time(void);
uint32_t get_breakpoint_time(void);
void play_info_init(void);
void play_num_read(void);
void play_num_write(void);
void play_time_read(void);
void play_time_write(void);
void play_info_write_when_poweroff(void);

void audio_scene_save(void);
void audio_scene_recovery(void);

uint32_t get_common_tone_trigger_event(void);
bool common_tone_is_playing(void);
void common_tone_status_clear(void);


#ifdef __cplusplus
}
#endif

#endif
