#define LOG_LEV 5
#include "sys_inc.h"
#include "user_config.h"
#include "ai.h"
#include "ble_app.h"
#include "xfs.h"

 
TaskHandle_t send_file_task_handler;

#define SEND_FILE_TASK_MSG_QUEUE_LENGTH   2
#define SEND_FILE_TASK_NAME               "send_file_task"

uint32_t read_head_info(XFILE* fp,uint32_t *record_id,uint32_t *record_ms); //sz_id_flag: 1表示获取文件录音时间，0表示获取文件ID
uint8_t stop_sending_flag = false;

uint64_t NEED_SEND_FILE_OFFSET = 0;
extern uint8_t BANBEN_ID[2];
extern uint8_t FIRST_FRAME[6];
extern uint8_t END_FRAME[6];
extern void *general_calloc(uint32_t size);
extern void general_free(void *ptr);
extern char NEED_SEND_DEL_FILE_NAME[15];

AT(.ble_app_flash_seg)
uint8_t send_file(bool need){
	send_rec_file_t *send_rec_file = (send_rec_file_t*)g_send_rec_file_t;
    send_rec_file_msg_t msg;
    if(need) {
		msg.cmd = SEND_FILE_CMD_START;
	} else {
        msg.cmd = SEND_FILE_CMD_STOP;
	}
	msg.val = need;
	if (pdPASS != osQueueSend(send_rec_file->cmd_queue, &msg, 2000)) {
        return ERR_CODE_SEND_FILE_ERR;
    }
	return ERR_CODE_SUCCESS;
}

AT(.ble_app_flash_seg)
static int process_send_file_cmd(void *handle, uint32_t wait_ticks)
{
    send_rec_file_t *send_rec_file = (send_rec_file_t*)handle;

    send_rec_file_msg_t msg;
    msg.cmd = SEND_FILE_CMD_NONE;
    msg.val = 0;
    
    if (pdPASS != osQueueReceive(send_rec_file->cmd_queue, &msg, wait_ticks)){
        return -1;
    }

    switch (msg.cmd) {
    case SEND_FILE_CMD_START:
		send_rec_file->status = SEND_FILE_CMD_START;
        break;

    case SEND_FILE_CMD_STOP:
		send_rec_file->status = SEND_FILE_CMD_STOP;
        logi("SEND_FILE_CMD_STOP");
        break;
    default:
        break;
    }
    return 0;
}

AT(.record_flash_seg)
void set_stop_sending_flag(uint8_t flag)
{
    stop_sending_flag = flag;
}


AT(.ble_app_flash_seg)
void start_send_file()
{
//	uint32_t send_size = 0;
	send_rec_file_t *send_rec_file = (send_rec_file_t*)g_send_rec_file_t;
	send_rec_file->status = SEND_FILE_CMD_ING;
	char file_name[50] = RECORD_FILE_DIR;
	XFILE *frxin;
	FRESULT res;
	uint32_t read_size;
	FSIZE_t file_size;
	uint32_t file_id = 0;
	uint32_t file_duration = 0;
	bool need_send_end_frame = true;
    bool ble_send_state = 0;
	uint8_t *bs_buf;
	bs_buf = general_calloc(BT_BUFFER_SIZE);
	if(bs_buf == NULL) {
	  loge("bs_buf NULL");
	}
	frxin = general_calloc(sizeof(*frxin));
	strcat(file_name,"/");
    strcat(file_name, NEED_SEND_DEL_FILE_NAME);

	logi("sync_file_name : %s",file_name);
    logi("NEED_SEND_FILE_OFFSET = %d",NEED_SEND_FILE_OFFSET);
	res = xfopen(frxin, file_name, FA_READ | FA_OPEN_EXISTING);
    if (FR_OK != res) {
        loge("open %s fail",file_name);
		xfclose(frxin);
        general_free(frxin);
	    general_free(bs_buf);
		return;
    }
	read_head_info(frxin, &file_id, &file_duration);
	file_size = xfsize(frxin) - RECORD_MP2_HEAD_SIZE;
	libc_memcpy(bs_buf, BANBEN_ID, LEN_FIRST_BANBEN_ID);
	libc_memcpy(bs_buf + OFFSET_FIRST_FRAME, FIRST_FRAME, LEN_FIRST_FRAME);
	libc_memcpy(bs_buf + 8, &file_id, 4);
	libc_memcpy(bs_buf + 12, &file_size, 6);
	ble_send_state = ble_file_send(bs_buf, 18);
	logi("file_size = %d,file_id = %d",file_size,file_id);
    while (ble_send_state == 0){
        msleep(5);
        ble_send_state = ble_file_send(bs_buf, 18);
    }
	if(NEED_SEND_FILE_OFFSET > 0 && NEED_SEND_FILE_OFFSET < file_size){
		res = xflseek(frxin, NEED_SEND_FILE_OFFSET + RECORD_MP2_HEAD_SIZE);
		if (FR_OK != res) {
        loge("offset %d fail",NEED_SEND_FILE_OFFSET);
        }
	}
	logi("file_size = %d,NEED_SEND_FILE_OFFSET = %d",file_size,NEED_SEND_FILE_OFFSET);
//	send_size += NEED_SEND_FILE_OFFSET;
	while (1) {
        
        xfread(frxin, bs_buf, BT_BUFFER_SIZE, &read_size);
        if (read_size > 0) {
            ble_send_state = ble_file_send(bs_buf,read_size);
            while (ble_send_state == 0){
                msleep(5);
                ble_send_state = ble_file_send(bs_buf, read_size);
            }
//			send_size += read_size;
//			logi("send size = %d/%d",send_size,file_size);
        } else {
            break;
        }
        if (stop_sending_flag) {
			libc_memcpy(bs_buf, END_FRAME, LEN_END_FRAME);
			ble_send_state = ble_file_send(bs_buf, LEN_END_FRAME);
             while (ble_send_state == 0) {
                msleep(5);
                ble_send_state = ble_file_send(bs_buf, LEN_END_FRAME);
            }
		    logi("zhudong send END_FRAME--------------");
		    need_send_end_frame = false;
			libc_memcpy(bs_buf, BANBEN_ID, LEN_FIRST_BANBEN_ID);
	        libc_memcpy(bs_buf + OFFSET_FIRST_FRAME, FIRST_FRAME, LEN_FIRST_FRAME);
	        *(bs_buf + OFFSET_CMD) = TAG_SYNC_FILE_NAME_STOP;
	        *(bs_buf + OFFSET_ERROR) = ERR_CODE_SUCCESS;
	        libc_memcpy(bs_buf + OFFSET_ERROR + 1, END_FRAME, LEN_END_FRAME);
	        ble_cmd_send(bs_buf, LEN_SUM + 2);
            break;
        }
		if(get_ble_con_status() == false) {
			need_send_end_frame = false;
			break;
		}
    }
	if(need_send_end_frame) {
	    libc_memcpy(bs_buf, END_FRAME, LEN_END_FRAME);
        ble_send_state = ble_file_send(bs_buf, LEN_END_FRAME);
        while (ble_send_state == 0) {
            msleep(5);
            ble_send_state = ble_file_send(bs_buf, LEN_END_FRAME);
        }
	    logi("over send END_FRAME--------------");
	}
	//发送接收到的stop命令，在ble_app里面会概率性出现重复的现象
	xfclose(frxin);
    general_free(frxin);
	general_free(bs_buf);
	send_rec_file->status = SEND_FILE_CMD_STOP;
}

AT(.ble_app_flash_seg)
static void send_file_loop(void *handle)
{
    send_rec_file_t *send_rec_file = (send_rec_file_t*)handle;
     
    while (1) {
		if (SEND_FILE_CMD_START == send_rec_file->status){
            start_send_file();
		} else {
            process_send_file_cmd(send_rec_file, portMAX_DELAY);
		}
    }
}

AT(.ble_app_flash_seg)
uint8_t send_file_init(void) {
	send_rec_file_t *send_rec_file;
	send_rec_file = general_calloc(sizeof(send_rec_file_t));
	if (NULL == send_rec_file) {
		loge("malloc send_rec_file fail");
		return 0;
	}

	send_rec_file->cmd_queue = xQueueCreate(SEND_FILE_TASK_MSG_QUEUE_LENGTH,
		    sizeof(send_rec_file_msg_t));
	if (NULL == send_rec_file->cmd_queue) {
        loge("creat cmd_queue fail");
	}

	/* create ble app task */
	if (pdFAIL == xTaskCreate(send_file_loop, SEND_FILE_TASK_NAME,
		BLE_SEND_RECORD_FILE, send_rec_file, BLE_APP_TASK_PRIO, &send_rec_file->task_handle)) {
		  loge("xTaskCreate_fail");
		goto create_task_fail;
	}
		
	send_rec_file->status = SEND_FILE_CMD_STOP;
	g_send_rec_file_t = send_rec_file;

	return 0;
	create_task_fail:
	general_free(send_rec_file);
	return 0;
}


