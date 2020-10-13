#ifndef _MODE_RECORD_H_
#define _MODE_RECORD_H_

#include "sys_inc.h"
#include "user_config.h"
#include "xfs.h"

#if MODE_RECORD_EN
#define RECORD_MIC_FUNC                     1
#else
#define RECORD_MIC_FUNC                     0
#endif

#define RECORD_FS                           16000    //录音采样率
#define RECORD_CHANNELS                     1        //录音声道数

#define RECORD_FS_48K                       48000
#define RECORD_SZ_16BIT                     sizeof(int16_t)
#define RECORD_SZ_32BIT                     sizeof(int32_t)

#if (0 == RECORD_FORMAT_SEL)
#define RECORD_FORMAT               AR_STREAM_FMT_ADPCM
#define RECORD_FILE_SUFFIX          ".wav"    /* adpcm文件后缀名 */
#elif (1 == RECORD_FORMAT_SEL)
#define RECORD_FORMAT               AR_STREAM_FMT_MP2
#define RECORD_FILE_SUFFIX          ".mp3"    /* mp2文件后缀名 */
#elif (2 == RECORD_FORMAT_SEL)
#define RECORD_FORMAT               AR_STREAM_FMT_SBC
#define RECORD_FILE_SUFFIX          ".sbc"    /* sbc文件后缀名 */
#else
#define RECORD_FORMAT               AR_STREAM_FMT_DUMP
#define RECORD_FILE_SUFFIX          ".pcm"    /* sbc文件后缀名 */
#endif

#define RECORD_TRUNCATE_TIME_MS     (800)     /* 录音完成后截断多少毫秒的文件尾数据（此功能可丢掉结束录音的按键音） */

enum record_status_e {
    REC_STATUS_IDLE         = 0,             /* 表示当前不在录音也不在播放 */
    REC_STATUS_RECORDING,                    /* 表示当前正在录音 */
    REC_STATUS_PLAYING,                      /* 表示当前正在播放录音文件 */
};

enum record_ret_e {
    REC_RET_SUCCESS         = 0,
    REC_RET_ERR             = -1,
};


enum record_bit_rate_e {
    REC_BIT_3072KPBS         = 0,                        /* 表示比特率3072kpbs */
    REC_BIT_1536KPBS,                        /* 表示比特率1536kpbs */
    REC_BIT_512KPBS,             /* 表示比特率512kpbs */
};

struct rec_time {
    uint32_t min;
    uint32_t sec;
    uint32_t ms;
};

struct rec_bitrate_param {
    uint8_t bitrate_index;
    uint32_t fs;
    uint8_t sz;
    uint8_t ch;
    uint8_t fmt;
};

/* 录音上下文结构体 */
typedef struct {
    FATFS fs;                 /* 文件系统上下文 */
    struct rec_time rtime;    /* 录音时间 */

    int32_t cur_num;          /* 当前录音文件编号 */
    int32_t cur_play_num;     /* 当前播放的文件编号 */
    int32_t max_play_num;     /* 最大可播放的文件编号 */
	int32_t mp2_max_play_num; /* mp3最大可播放的文件编号 */
    uint32_t total_file;      /* 总文件数量 */
	uint32_t total_mp2_file;  /* mp3文件总文件数量 */
    uint32_t file_fail_count; /* 连续播放失败的文件数量（当连续播放失败的文件数等于总文件数时说明此设备无可播的录音文件） */
    uint32_t breakpoint_time; /* 断点时间，用于恢复播放时跳转到上次播放的时间点 */
    uint32_t status;          /* 录音状态，标识正在录音还是正在播放 */

    char file_name[50];       /* 录音文件名 */
    char play_file_name[50];  /* 播放文件名 */
	char mp2_file_name[50];   /* 录音文件名 */
    uint8_t new_dev;          /* 标识最新插入的设备是SD卡还是U盘 */
    uint8_t mount_dev;        /* 标识已经mount文件系统的设备 */
    uint8_t streamtype;       //录音保存介质，参考@streamtype_t
    bool play_direction;      /* 播放方向，标识是播上一首还是播下一首 */
    bool file_scaned;         /* 标识是否文件夹已经被遍历扫描过 */
    struct rec_bitrate_param bit_rate_parm; /*比特率对应参数 */
} record_context_t;
extern record_context_t *rec_context;


/* 判断文件是否是录音文件；
 * @param file_name: 文件名
 * @return: true:是录音文件； false：不是录音文件
 */
bool is_record_file(char *file_name);

/* 开始录音 */
void start_record(void);

/* 停止录音 */
void stop_record(void);

/* 开始播放录音文件 */
int start_play_record_file(void);

/* 停止录音文件的播放 */
void stop_play_record_file(void);

void record_mode_scene_save(void);
void record_mode_scene_recovery(void);

/* 选择录音设备；
 * @param dev_num: 设备号
 */
int32_t select_record_device(uint32_t dev_num);

void recorder_event_listener(void *recorder, uint32_t event);
void switch_record_device(bool auto_play);


/*
 * 获取录音时间
 */
void get_record_time(void);

/* 删除录音文件 */
void delete_record_file_by_num(void);


//linein 录音初始化
void linein_record_init(uint32_t rec_func);
//linein 录音退出
void linein_record_deinit(void);
//更新linein录音(或者录音播放)的显示时间
void linein_record_update_disp_time(void);
//linein录音处理SD卡事件
void linein_record_proc_sd_event(uint8_t mode, uint32_t event);
//linein录音处理U盘事件
void linein_record_proc_udisk_event(uint8_t mode, uint32_t event);
//linein录音文件播放
void linein_record_play_rec_file(uint32_t mode_id);
//linein停止播放录音文件
void linein_stop_play_rec_file(uint8_t mode);

//获取sd卡sn校验
bool get_sd_sn_status(void);
void record_event_process(uint32_t msg);
void mode_record(void);
void mode_record_enter(void);
void mode_record_loop(void);
void mode_record_exit(void);
void update_record_param(uint8_t bit_rate,bool flash);
uint32_t get_record_fs_param(void);
uint8_t get_record_sz_param(void);
void index_to_filename(int32_t index, char *name);
void index_to_mp2_filename(int32_t index, char *name);
uint8_t del_rec_file_by_id(int32_t del_file_id);
uint8_t del_rec_file_by_name(uint32_t file_index);
uint32_t mktime(void);
uint32_t ext_getfree(uint8_t flag);
uint8_t uint64_to_str(uint64_t value, uint8_t *string_bit);
void start_send_dir_files_list();

#endif
