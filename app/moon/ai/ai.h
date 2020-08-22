#ifndef _AI_
#define _AI_

//typedef struct {
//    FATFS fs;                 /* 文件系统上下文 */
//    struct rec_time rtime;    /* 录音时间 */

//    int32_t cur_num;          /* 当前录音文件编号 */
//    int32_t cur_play_num;     /* 当前播放的文件编号 */
//    int32_t max_play_num;     /* 最大可播放的文件编号 */
//	int32_t mp2_max_play_num; /* mp3最大可播放的文件编号 */
//    uint32_t total_file;      /* 总文件数量 */
//	uint32_t total_mp2_file;  /* mp3文件总文件数量 */
//    uint32_t file_fail_count; /* 连续播放失败的文件数量（当连续播放失败的文件数等于总文件数时说明此设备无可播的录音文件） */
//    uint32_t breakpoint_time; /* 断点时间，用于恢复播放时跳转到上次播放的时间点 */
//    uint32_t status;          /* 录音状态，标识正在录音还是正在播放 */

//    char file_name[50];       /* 录音文件名 */
//    char play_file_name[50];  /* 播放文件名 */
//	char mp2_file_name[50];   /* 录音文件名 */
//    uint8_t new_dev;          /* 标识最新插入的设备是SD卡还是U盘 */
//    uint8_t mount_dev;        /* 标识已经mount文件系统的设备 */
//    uint8_t streamtype;       //录音保存介质，参考@streamtype_t
//    bool play_direction;      /* 播放方向，标识是播上一首还是播下一首 */
//    bool file_scaned;         /* 标识是否文件夹已经被遍历扫描过 */
//    struct rec_bitrate_param bit_rate_parm; /*比特率对应参数 */
//} record_context_t;
//extern record_context_t *rec_context;
typedef struct send_rec_file {
	volatile uint8_t status;
    TaskHandle_t task_handle;
    xQueueHandle cmd_queue;
} send_rec_file_t;

typedef struct send_rec_file_msg {
    uint8_t cmd;
    uint8_t val;
} send_rec_file_msg_t;

enum send_file_cmd_e {
    SEND_FILE_CMD_NONE,
    SEND_FILE_CMD_START,
    SEND_FILE_CMD_ING,
    SEND_FILE_CMD_STOP,
};
	
static void *g_send_rec_file_t;
uint8_t send_file_init(void);
uint8_t send_file(bool need);
void set_stop_sending_flag(uint8_t flag);
#endif


