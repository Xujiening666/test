#define LOG_LEV 5
#define SLTK_LOG_TAG "record"
#include "sys_inc.h"
#include "user_config.h"
#include "task_main.h"
#include "record.h"
#include "key.h"
#include "task.h"
#include "xfs.h"
#include "device.h"
#include "sd.h"
#include "info_mem.h"
#include "disp.h"
#include "usb.h"
#include "diskio.h"
#include "loadcode.h"
#include "libc_common.h"
#include "device.h"
#include "audio_recorder.h"
#include "scene_manager.h"
#include "player.h"
#include "sdmmc_io.h"
#include "ble_app.h"
#include "info_mem.h"
#include "xfs.h"
#include "audio_service.h"
#include "timer.h"

extern int linein_play_vol;
extern int fm_play_vol;
extern uint8_t record_bitrate_index;

#if (RECORD_MIC_FUNC || RECORD_LINEIN_FUNC || RECORD_FM_FUNC)

record_context_t *rec_context;

extern struct rtc_t rtc;

/*
*根据文件号码获取文件大小
*/
uint32_t read_head_info(XFILE* fp,uint32_t *record_id,uint32_t *record_ms); //sz_id_flag: 1表示获取文件录音时间，0表示获取文件ID

const uint8_t NAME_BIT[LEN_NAME_BIT]         = {0x7b,0x22,0x6e,0x61,0x6d,0x65,0x22,0x3a,0x22};  //{"name":"
const uint8_t SIZE_BIT[LEN_SIZE_BIT]         = {0x22,0x2c,0x22,0x73,0x69,0x7a,0x65,0x22,0x3a};  //","size":
const uint8_t ID_BIT[LEN_ID_BIT]             = {0x2c,0x22,0x69,0x64,0x22,0x3a};                 //,"id":
const uint8_t SN_BIT[LEN_SN_BIT]             = {0x2c,0x22,0x73,0x6e,0x22,0x3a,0x22};            //,"sn":"
const uint8_t DURATION_BIT[LEN_DURATION_BIT] = {0x22,0x2c,0x22,0x64,0x75,0x72,0x61,             //","duration":
	                                            0x74,0x69,0x6f,0x6e,0x22,0x3a};
const uint8_t END_BIT[LEN_END_BIT]           = {0x7d,0x2c};                                     //},
const uint8_t END_END_BIT[LEN_END_BIT + 1]   = {0x7d,0x5d,0x7d};                                //}]}

//uint32_t NEED_SEND_FILE_ID = 0;
char NEED_SEND_DEL_FILE_NAME[15];

AT(.record_flash_seg)
uint32_t ext_getfree(uint8_t flag)//flag 1: fre_sect flag:0 tot_sect
{
//    FATFS *fs1;
//    DWORD fre_clust = 0;
//   __maybe_unused DWORD fre_sect = 0;
//   __maybe_unused DWORD tot_sect = 0;
//    uint8_t err = xgetfree("0:", &fre_clust, &fs1);
//	if(err != FR_OK){
//        return ERR_CODE_NOT_GET_STORAGE;
//	}
//    if(0 == flag){
//		tot_sect = (fs1->n_fatent-2) * fs1->csize;
//        return tot_sect>>1;
//	} else if(1 == flag) {
//		fre_sect = fre_clust*fs1->csize;
//        return fre_sect>>1;
//	} else {
//		return ERR_CODE_NOT_GET_STORAGE;
//	}
	__maybe_unused DWORD fre_sect = 0;
	__maybe_unused DWORD tot_sect = 0;

    if(MODE_USBDEV == mode_get_sta()) {//如果是在usb模式下，则从保存的数据中读取
        if(0 == flag) {
			info_get(INFO_SD_TOT_SECT, (void *)&tot_sect, 4);
			return tot_sect;
		} else if(1 == flag) {
            info_get(INFO_SD_FRE_SECT, (void *)&fre_sect, 4);
			return fre_sect;
		} else {
			return ERR_CODE_NOT_GET_STORAGE;
		}
    } else {
		FATFS *fs1;
		DWORD fre_clust = 0;
		uint8_t err = xgetfree("0:", &fre_clust, &fs1);
		if(err != FR_OK){
		    return ERR_CODE_NOT_GET_STORAGE;
		}
		if(0 == flag){
			tot_sect = (fs1->n_fatent-2) * fs1->csize;
			tot_sect >>= 1;
		    info_set(INFO_SD_TOT_SECT, (void *)&tot_sect, 4);
		    return tot_sect;
		} else if(1 == flag) {
			fre_sect = fre_clust*fs1->csize;
			fre_sect >>= 1;
			info_set(INFO_SD_FRE_SECT, (void *)&fre_sect, 4);
		    return fre_sect;
		} else {
			return ERR_CODE_NOT_GET_STORAGE;
		}
    }
	return ERR_CODE_NOT_GET_STORAGE;
}
/*
    FRESULT res;
    XFILE *file;
    uint32_t try_count = rec_context->max_play_num;

    if (rec_context->cur_play_num < 0 || rec_context->total_file == 0) {
        return;
    }

    if (REC_STATUS_RECORDING == rec_context->status ){
        logd("stop record");
        stop_record();
    } else if (REC_STATUS_PLAYING == rec_context->status) {
        logd("stop play");
        stop_play_record_file();
    } else {
        // check if the file exists.
        file = libc_calloc(sizeof(*file));
next:
        index_to_filename(rec_context->cur_play_num, rec_context->play_file_name);
        res = xfopen(file, rec_context->play_file_name, FA_READ | FA_OPEN_EXISTING);
        if (FR_NO_FILE == res || FR_NO_PATH == res) {
            xfclose(file);
            if (try_count) {
                --try_count;
                // 跳过不存在的文件 
                if (rec_context->cur_play_num < rec_context->max_play_num) {
                    rec_context->cur_play_num++;
                } else {
                    rec_context->cur_play_num = 0;
                }
                goto next;
            }
        } else {
            xfclose(file);
        }
        libc_free(file);
    }

	index_to_filename(rec_context->cur_play_num, rec_context->play_file_name);
    logd("delete record file:%s", rec_context->play_file_name);
    res = xfunlink(rec_context->play_file_name);//delete the record file
    if (FR_OK == res){
        //logd("result=%d",res);
        rec_context->total_file--;
        rec_context->cur_play_num++;
        logd("cur_play_num:%d, total_file:%d", rec_context->cur_play_num,rec_context->total_file);
    } else{
        logd("error = %d",res);
    }

*/
AT(.ble_flash_seg)
uint8_t del_rec_file_by_name(uint32_t file_index)
{
#if 1
    FRESULT res;
    XFILE *file;
	FRESULT mp3_res;
	file = libc_calloc(sizeof(*file));

    index_to_mp2_filename(file_index, rec_context->mp2_file_name);
	mp3_res = xfopen(file, rec_context->mp2_file_name, FA_READ | FA_OPEN_EXISTING);
	if(FR_NO_FILE == mp3_res && FR_NO_PATH == mp3_res) {
		xfclose(file);
	} else {
        xfclose(file);
		logd("rm mp3 file = %s",rec_context->mp2_file_name);
		mp3_res = xfunlink(rec_context->mp2_file_name);//delete the mp3 record file
		logd("mp3 res = %d",mp3_res);
		if (FR_OK == mp3_res) {
			rec_context->total_mp2_file--;
			index_to_filename(file_index, rec_context->mp2_file_name);		
			res = xfopen(file, rec_context->mp2_file_name, FA_READ | FA_OPEN_EXISTING);
            if(FR_NO_FILE == mp3_res && FR_NO_PATH == mp3_res) {
				xfclose(file);
			} else {
				xfclose(file);
				logd("rm wav file = %s",rec_context->mp2_file_name);
				res = xfunlink(rec_context->mp2_file_name);
				logd("wav res = %d",res);
				if(FR_OK == res) {
					rec_context->total_file--;
				}
			}
		}
	}
	libc_free(file);
	return mp3_res;
#elif 0
    FRESULT res;
	XFILE *file;
	FRESULT mp3_res;
    //构建文件完整路径
	strcat(file_name, RECORD_FILE_DIR);
	strcat(file_name, "/");
    strcat(file_name, NEED_SEND_DEL_FILE_NAME);
	//判断文件是否存在
	file = libc_calloc(sizeof(*file));
	mp3_res = xfopen(file, file_name, FA_READ | FA_OPEN_EXISTING);
	if(FR_NO_FILE == res && FR_NO_PATH == res) {
		xfclose(file);
	} else {
		xfclose(file);
		logd("rm mp3 file = %s",file_name);
		mp3_res = xfunlink(file_name);//delete the mp3 record file
		logd("mp3 res = %d",mp3_res);
		if (FR_OK == mp3_res) {
			rec_context->total_mp2_file--;
			logi("%s",file_name[RECORD_FILE_SUM_COUNT - 3]);
		    logi("%s",file_name[RECORD_FILE_SUM_COUNT - 2]);
			logi("%s",file_name[RECORD_FILE_SUM_COUNT - 1]);
			file_name[RECORD_FILE_SUM_COUNT - 3] = 'w';
	        file_name[RECORD_FILE_SUM_COUNT - 2] = 'a';
	        file_name[RECORD_FILE_SUM_COUNT - 1] = 'v';
			logd("rm wav file = %s",file_name);
			res = xfopen(file, file_name, FA_READ | FA_OPEN_EXISTING);
			if(FR_NO_FILE == res && FR_NO_PATH == res) {
				xfclose(file);
                //没有wav文件,直接返回mp3的执行结果
			} else {
                xfclose(file);
				res = xfunlink(file_name);//
				if(FR_OK == res) {
					rec_context->total_file--;
				}
			}
		}
	}
	return mp3_res;
#endif
}

AT(.record_flash_seg)
uint32_t mktime(void)
{
    uint8_t day, hour, min, sec;
    unsigned int mon , year;

    rtc_get_time(&rtc.t);

    logi("rtc:%04d-%02d-%02d, %02d:%02d:%02d",
       rtc.t.year+BASE_YEAR, rtc.t.month, rtc.t.day,
       rtc.t.hour, rtc.t.minute, rtc.t.second);

    year = rtc.t.year+BASE_YEAR;
    mon = rtc.t.month;
    day = rtc.t.day;
    hour = rtc.t.hour;
    min = rtc.t.minute;
    sec = rtc.t.second;

    /* 1..12 -> 11,12,1..10 */
    if (0 >= (int) (mon -= 2)) {
    mon += 12;   /* Puts Feb last since it has leap day */
    year -= 1;
    }

    return ((((unsigned long)
    (year/4 - year/100 + year/400 + 367*mon/12 + day) +
    year*365 - 719499
    )*24 + hour - 8/* now have hours */
    )*60 + min /* now have minutes */
    )*60 + sec; /* finally seconds */
}

extern uint32_t general_get_avaheapsize();

DWORD get_fattime(void)
{
    DWORD time_temp = 0;
   
    rtc_get_time(&rtc.t);
	
    time_temp |= (DWORD)(rtc.t.year+BASE_YEAR-1980) << 25;
    time_temp |= (DWORD)(rtc.t.month) << 21;
    time_temp |= (DWORD)(rtc.t.day) << 16;
    time_temp |= (DWORD)(rtc.t.hour) << 11;
    time_temp |= (DWORD)(rtc.t.minute) << 5;
    time_temp |= (DWORD)(rtc.t.second) >> 1;
    return time_temp;
}


/* 录音事件监听器；
 * 此函数被audio库编码线程调用；
 * @param recorder: 录音器句柄；
 * @param event: 录音事件， 参考enum ar_event_e；
 */
AT(.record_flash_seg)
void recorder_event_listener(void *recorder, uint32_t event)
{
    event_force_put(event); /* 给APP线程发送事件 */
}

/* 判断文件是否是录音文件；
 * @param file_name: 文件名
 * @return: true:是录音文件； false：不是录音文件
 */
AT(.record_flash_seg)
bool is_record_file(char *file_name)
{
    char *p, *fix;

    /* check prefix */
    p = file_name;
    fix = RECORD_FILE_PREFIX;
    for (; *fix != '\0'; fix++, p++) {
        if (*p != *fix) {
            return false;
        }
    }

    /* check file number */
    for (int i = 0; i < RECORD_FILE_COUNT_BITS; i++) {
        if (*p > '9' || *p < '0') {
            return false;
        }
        ++p;
    }

    /* check suffix */
    fix = RECORD_FILE_SUFFIX;
    for (; *fix != '\0'; fix++, p++) {
        if (*p != *fix) {
            return false;
        }
    }

    if (*p != '\0') {
        return false;
    }

    return true;
}

AT(.record_flash_seg)
bool is_mp2_record_file(char *file_name)
{
    char *p, *fix;

    /* check prefix */
    p = file_name;
    fix = RECORD_FILE_PREFIX;
    for (; *fix != '\0'; fix++, p++) {
        if (*p != *fix) {
            return false;
        }
    }

    /* check file number */
    for (int i = 0; i < RECORD_FILE_COUNT_BITS; i++) {
        if (*p > '9' || *p < '0') {
            return false;
        }
        ++p;
    }

    /* check suffix */
    fix = ".mp3";
    for (; *fix != '\0'; fix++, p++) {
        if (*p != *fix) {
            return false;
        }
    }

    if (*p != '\0') {
        return false;
    }

    return true;
}

/* 文件编号转换成文件名；
 * @param index: 文件编号
 * @return name: 文件名
 */
AT(.record_flash_seg)
void index_to_mp2_filename(int32_t index, char *name)
{
    int32_t len;
    uint32_t num;
    char *p;

    strcpy(name, RECORD_FILE_DIR "/" RECORD_FILE_PREFIX);

    len = strlen(name);
    num = index;
    p = name + len + RECORD_FILE_COUNT_BITS;
    *p-- = '\0';
    for (int i = 0; i < RECORD_FILE_COUNT_BITS; i++) {
        *p-- = (num % 10) + '0';
        num = num / 10;
    }
    strcat(name, ".mp3");
}


AT(.record_flash_seg)
void index_to_filename(int32_t index, char *name)
{
    int32_t len;
    uint32_t num;
    char *p;

    strcpy(name, RECORD_FILE_DIR "/" RECORD_FILE_PREFIX);

    len = strlen(name);
    num = index;
    p = name + len + RECORD_FILE_COUNT_BITS;
    *p-- = '\0';
    for (int i = 0; i < RECORD_FILE_COUNT_BITS; i++) {
        *p-- = (num % 10) + '0';
        num = num / 10;
    }
    strcat(name, RECORD_FILE_SUFFIX);
}

/*
AT(.record_flash_seg)
int get_record_file_size_by_num(int32_t cur_send_num,uint8_t sz_id_flag)
{
    XFILE *frxin;
    FRESULT res;
    //__maybe_unused uint32_t file_ms;
    char file_name[30]; 
    
    frxin = libc_calloc(sizeof(*frxin));
    index_to_filename(cur_send_num, file_name);
    res = xfopen(frxin, file_name, FA_READ | FA_OPEN_EXISTING);
    if (FR_OK != res) {
        loge("open %s fail",file_name);
        return REC_RET_ERR;
    }
    loge("open %s",file_name);
    //file_ms = read_head_info(frxin,sz_id_flag);
    //loge("file_ms:%d",file_ms);
	FSIZE_t i = xfsize(frxin);
	logi("arunboy size = %lu",i);
    xfclose(frxin);
    libc_free(frxin);
    return 0;
}
*/

/*
 * 扫描录音文件夹，获取录音文件数量和最大文件编号；
 */
AT(.record_flash_seg)
void scan_record_file(void)
{
    FRESULT res;
    DIR dir;
    //DWORD fre_clust;
	//FATFS *pfs;
	//pfs = &rec_context->fs;
    if (!rec_context->file_scaned) {
try_opendir:
        res = xfopendir(&dir, RECORD_FILE_DIR); /* 打开录音文件夹 */
        logv("opendir res:%d", res);

        /* make dir if no path */
        if (res != FR_OK) {
            if (FR_NO_PATH == res) {
                res = xfmkdir(RECORD_FILE_DIR); /* 创建录音文件夹 */
                logv("mkdir res:%d", res);
                if (res == FR_OK) {
                    goto try_opendir;
                }
            } else {
                //TODO
            }
        }

        FILINFO *pfno = libc_calloc(sizeof(FILINFO));
        if (NULL == pfno) {
            loge("calloc pfno fail");
        }

        int32_t num = 0;
        int32_t num_max = -1;
		int32_t mp2_num = 0;
        int32_t mp2_num_max = -1;
        uint32_t offset = strlen(RECORD_FILE_PREFIX);

        rec_context->total_file = 0;
		rec_context->total_mp2_file = 0;
		
        /* 遍历文件夹，获取录音文件数量和最大文件编号 */
        while (1) {
            res = xfreaddir(&dir, pfno);
            if (res != FR_OK || pfno->fname[0] == 0) {
                break;
            }

            if (!(pfno->fattrib & AM_DIR)) {
                if (is_record_file(pfno->fname)) {
                    logv("fname:%s", pfno->fname);
                    int n = 1;
                    num = 0;
                    for (int i = 1; i <= RECORD_FILE_COUNT_BITS; i++) {
                        int index = offset + RECORD_FILE_COUNT_BITS - i;
                        num += (pfno->fname[index] - '0') * n;
                        n *= 10;
                    }

                    num_max = num_max > num ? num_max : num;

                    rec_context->total_file++;
                } else if(is_mp2_record_file(pfno->fname)) {
					logv("fname:%s", pfno->fname);
					int n = 1;
					mp2_num = 0;
                    for (int i = 1; i <= RECORD_FILE_COUNT_BITS; i++) {
                        int index = offset + RECORD_FILE_COUNT_BITS - i;
                        mp2_num += (pfno->fname[index] - '0') * n;
                        n *= 10;
                    }

                    mp2_num_max = mp2_num_max > mp2_num ? mp2_num_max : mp2_num;
					rec_context->total_mp2_file++;
				}
            }
        }
        num_max = num_max > mp2_num_max ? num_max : mp2_num_max;
        logv("num_max:%d", num_max);
		rec_context->mp2_max_play_num = mp2_num_max;
        rec_context->max_play_num = num_max;
        rec_context->cur_play_num = num_max;
        rec_context->cur_num = num_max;
        rec_context->file_fail_count = 0;
        logd("total file:%d", rec_context->total_file);
        logd("total mp2 file:%d", rec_context->total_mp2_file);
        libc_free(pfno);
        xfclosedir(&dir);
        rec_context->file_scaned = true;
		//test   need   closd
		/*for(int i=0; i<10; i++)
		{
             get_record_file_size_by_num((int32_t)i,1);
			 get_record_file_size_by_num((int32_t)i,0);      
		}*/
    }
}

/* 删除录音文件；
 * @param rec_context: 录音上下文
 */
// 
//AT(.record_flash_seg)
//void delete_record_file_by_num(void)
//{
//    FRESULT res;
//    XFILE *file;
//    uint32_t try_count = rec_context->max_play_num;

//    if (rec_context->cur_play_num < 0 || rec_context->total_file == 0) {
//        return;
//    }

//    if (REC_STATUS_RECORDING == rec_context->status ){
//        logd("stop record");
//        stop_record();
//    } else if (REC_STATUS_PLAYING == rec_context->status) {
//        logd("stop play");
//        stop_play_record_file();
//    } else {
//        /* check if the file exists.*/
//        file = libc_calloc(sizeof(*file));
//next:
//        index_to_filename(rec_context->cur_play_num, rec_context->play_file_name);
//        res = xfopen(file, rec_context->play_file_name, FA_READ | FA_OPEN_EXISTING);
//        if (FR_NO_FILE == res || FR_NO_PATH == res) {
//            xfclose(file);
//            if (try_count) {
//                --try_count;
//                /* 跳过不存在的文件 */
//                if (rec_context->cur_play_num < rec_context->max_play_num) {
//                    rec_context->cur_play_num++;
//                } else {
//                    rec_context->cur_play_num = 0;
//                }
//                goto next;
//            }
//        } else {
//            xfclose(file);
//        }
//        libc_free(file);
//    }

//	index_to_filename(rec_context->cur_play_num, rec_context->play_file_name);
//    logd("delete record file:%s", rec_context->play_file_name);
//    res = xfunlink(rec_context->play_file_name);//delete the record file
//    if (FR_OK == res){
//        //logd("result=%d",res);
//        rec_context->total_file--;
//        rec_context->cur_play_num++;
//        logd("cur_play_num:%d, total_file:%d", rec_context->cur_play_num,rec_context->total_file);
//    } else{
//        logd("error = %d",res);
//    }
//}

/* 停止录音文件的播放；
 * @param rec_context: 录音上下文
 */
AT(.record_flash_seg)
void stop_play_record_file(void)
{
    if (REC_STATUS_PLAYING == rec_context->status) {
        logv("stop play.");
        player_loop_play(0);
        player_close(0); //close player immediately

        rec_context->status = REC_STATUS_IDLE;
#if DISP_EN
        //disp_menu(MENU_SCENE_MAIN, scene_mode.sta, 0);
#endif
    }
}

/*
 * 录音设备准备；
 */
AT(.record_flash_seg)
void prepare_record_device(void)
{
    int ret;
    uint32_t dev;
    uint32_t device_1st;
    uint32_t device_2nd;

#if RECORD_NEW_DEVICE_FIRST
    /* new device first */
    dev = rec_context->new_dev;
    if (DEV_UDISK == dev) {
        device_1st = DEV_UDISK;
        device_2nd = DEV_SDCARD;
    } else if (DEV_SDCARD == dev) {
        device_1st = DEV_SDCARD;
        device_2nd = DEV_UDISK;
    } else {
        device_1st = DEV_UDISK;
        device_2nd = DEV_SDCARD;
    }
#else
    /* old device first */
    dev = rec_context->new_dev;
    if (DEV_UDISK == dev) {
        if (device_is_online(DEV_SDCARD)) {
            device_1st = DEV_SDCARD;
            device_2nd = DEV_UDISK;
        } else {
            device_1st = DEV_UDISK;
            device_2nd = DEV_SDCARD;
        }
    } else if (DEV_SDCARD == dev) {
        if (device_is_online(DEV_UDISK)) {
            device_1st = DEV_UDISK;
            device_2nd = DEV_SDCARD;
        } else {
            device_1st = DEV_SDCARD;
            device_2nd = DEV_UDISK;
        }
    } else {
        device_1st = DEV_UDISK;
        device_2nd = DEV_SDCARD;
    }
#endif
    if (DEV_OK != device_get_sta(device_1st)) {
        ret = select_record_device(device_1st);
        if (ret < 0) {
            ret = select_record_device(device_2nd);
            if (ret < 0) {
                device_set_num(DEV_NONE);
            }
        }
    }
}

AT(.record_flash_seg)
uint8_t digits10(uint64_t value)
{
   uint8_t length = 0;
   while(value)
   {
	   length++;
	   value /= 10;
   }
   return length;
}

AT(.record_flash_seg)
uint8_t uint64_to_str(uint64_t value, uint8_t *string_bit)
{
	const uint8_t digits[201] =
		"0001020304050607080910111213141516171819"
		"2021222324252627282930313233343536373839"
		"4041424344454647484950515253545556575859"
		"6061626364656667686970717273747576777879"
		"8081828384858687888990919293949596979899";
	libc_memset(string_bit, 0, 24);
	uint8_t length = digits10(value);
	uint8_t next = length - 1;

	while(value >= 100) {
        const uint8_t i = (value % 100) * 2;
		value /= 100;
	    *(string_bit + next) = digits[i + 1];
		*(string_bit + next - 1) = digits[i];
		next -= 2;
	}
	
	if(value < 10) {
        *(string_bit + next) = '0' + value;
	} else {
        const int i = value * 2;
		*(string_bit + next) = digits[i + 1];
		*(string_bit + next - 1) = digits[i];
	}
	
	return length;
}

/*
*开始传输文件列表
*/
extern uint8_t BANBEN_ID[2];
extern uint8_t FIRST_FRAME[6];
extern uint8_t END_FRAME[6];
extern uint8_t SN_NUM[8];
extern void *general_calloc(uint32_t size);
extern void general_free(void *ptr);

AT(.record_flash_seg)
void start_send_dir_files_list()
{	
    uint8_t *msg;
	uint8_t *u64_str;
	XFILE *frxin;
	
	uint8_t len_sum = 0;
	uint8_t len_tmp_num = 0;
	FSIZE_t file_size = 0;
	uint32_t file_id = 0;
	uint32_t file_duration = 0;
	uint32_t send_file_list_num = 0;

	char file_name[30];
	FRESULT res;
	
	msg = general_calloc(BT_BUFFER_SIZE);
	frxin = general_calloc(sizeof(*frxin));
	u64_str = general_calloc(20);
	
	libc_memcpy(msg, BANBEN_ID, LEN_FIRST_BANBEN_ID);
	len_sum += LEN_FIRST_BANBEN_ID;
	libc_memcpy(msg + len_sum, FIRST_FRAME, LEN_FIRST_FRAME);
	len_sum += LEN_FIRST_FRAME;
	*(msg + len_sum) = TAG_GET_DIR;
	*(msg + len_sum + 1) = 0;
	len_sum += 2;
	libc_memcpy(msg + len_sum, "{\"data\":[", 9);
	len_sum += 9;
	if(rec_context->total_mp2_file == 0) {
        libc_memcpy(msg + len_sum, "]}", 2);
	    len_sum += 2;
	    libc_memcpy(msg + len_sum, END_FRAME, LEN_END_FRAME);
		len_sum += LEN_END_FRAME;
	}
	ble_cmd_send(msg, len_sum);
	len_sum = 0;

	for(int32_t i = 0; i <= rec_context->mp2_max_play_num; i++){
		
		libc_memset(msg, 0, BT_BUFFER_SIZE);
	    index_to_mp2_filename(i, file_name);
        res = xfopen(frxin, file_name, FA_READ | FA_OPEN_EXISTING);
		if (FR_OK != res) {
            loge("open %s fail,error_code = %d",file_name,res);
            continue;
        }
		
		file_size = xfsize(frxin) - RECORD_MP2_HEAD_SIZE;
		read_head_info(frxin, &file_id, &file_duration);
		xfclose(frxin);
		
		libc_memcpy(msg + len_sum, NAME_BIT, LEN_NAME_BIT);
		len_sum += LEN_NAME_BIT;
		
	    libc_memcpy(msg + len_sum, &file_name[7], LEN_NAME_DATA);
		len_sum += LEN_NAME_DATA;
		
		libc_memcpy(msg + len_sum, SIZE_BIT, LEN_SIZE_BIT);
		len_sum += LEN_SIZE_BIT;
		
        len_tmp_num = uint64_to_str(file_size, u64_str);
		libc_memcpy(msg + len_sum, u64_str, len_tmp_num);
		len_sum += len_tmp_num;
		
		libc_memcpy(msg + len_sum, ID_BIT, LEN_ID_BIT);
		len_sum += LEN_ID_BIT;
		
		len_tmp_num = uint64_to_str(file_id, u64_str);
		libc_memcpy(msg + len_sum, u64_str, len_tmp_num);
		len_sum += len_tmp_num;
		
		libc_memcpy(msg + len_sum, SN_BIT, LEN_SN_BIT);
		len_sum += LEN_SN_BIT;
		
		libc_memcpy(msg + len_sum, SN_NUM, 8);
		len_sum += 8;

        libc_memcpy(msg + len_sum, DURATION_BIT, LEN_DURATION_BIT);
		len_sum += LEN_DURATION_BIT;

		len_tmp_num = uint64_to_str(file_duration, u64_str);
		libc_memcpy(msg + len_sum, u64_str, len_tmp_num);
		len_sum += len_tmp_num;
		
		logi("arunboy send file list = %d/%d",send_file_list_num,rec_context->total_mp2_file);
        if(send_file_list_num == rec_context->total_mp2_file - 1)
        {
            send_file_list_num += 1;
			libc_memcpy(msg + len_sum, END_END_BIT, LEN_END_END_BIT);
			len_sum += LEN_END_END_BIT;
            libc_memcpy(msg + len_sum, END_FRAME, LEN_END_FRAME);
			len_sum += LEN_END_FRAME;
			ble_cmd_send(msg, len_sum);
			len_sum = 0;
			logi("arunboy list end---------------------");
		} else {
			send_file_list_num += 1;
			libc_memcpy(msg + len_sum, END_BIT, LEN_END_BIT);
			len_sum += LEN_END_BIT;
			ble_cmd_send(msg, len_sum);
			len_sum = 0;
		}
	}
	general_free(u64_str);
	general_free(frxin);
	general_free(msg);
}

/* 开始传输录音文件；
 * @param rec_context: 录音上下文
*/

AT(.record_flash_seg)
int start_send_record_file_by_num(uint8_t *bs_buf ,int32_t cur_send_num)
{
//    XFILE *frxin;
//    FRESULT res;
//    uint32_t red;
//	FSIZE_t file_size;
//    char send_file_name[30]; 
//    bool need_send_end_frame = true;
//	if(bs_buf == NULL) {
//      loge("bs_buf NULL");
//      return REC_RET_ERR;
//    }
//    frxin = libc_calloc(sizeof(*frxin));
//    index_to_mp2_filename(cur_send_num, send_file_name);
//    res = xfopen(frxin, send_file_name, FA_READ | FA_OPEN_EXISTING);
//    if (FR_OK != res) {
//        loge("open %s fail",send_file_name);
//        return REC_RET_ERR;
//    }
//	file_size = xfsize(frxin);
//    loge("send %s",send_file_name);
//	libc_memcpy(bs_buf, BANBEN_ID, LEN_FIRST_BANBEN_ID);
//	libc_memcpy(bs_buf + OFFSET_FIRST_FRAME, First_frame, LEN_FIRST_FRAME);
//	libc_memcpy(bs_buf + 8, &NEED_SEND_FILE_ID, 4);
//	libc_memcpy(bs_buf + 12, &file_size, 6);
//	ble_file_send(bs_buf, 18);
//	if(NEED_SEND_FILE_OFFSET > 0 && NEED_SEND_FILE_OFFSET < file_size){
//		res = xflseek(frxin, NEED_SEND_FILE_OFFSET);
//		if (FR_OK != res) {
//        loge("offset %d fail",NEED_SEND_FILE_OFFSET);
//        return REC_RET_ERR;
//        }
//	}
//    while (1) {
//        xfread(frxin, bs_buf, BT_BUFFER_SIZE, &red);
//        if (red > 0) {
//            ble_file_send(bs_buf,red);
//        } else {
//            break;
//        }
//        if (stop_sending_flag || get_ble_con_status() == false) {
//			libc_memcpy(bs_buf, END_FRAME, LEN_END_FRAME);
//			ble_file_send(bs_buf, LEN_END_FRAME);
//		    need_send_end_frame = false;
//				//发送接收到的stop命令，在ble_app里面会概率性出现重复的现象
//			libc_memcpy(bs_buf, BANBEN_ID, LEN_FIRST_BANBEN_ID);
//			libc_memcpy(bs_buf + OFFSET_FIRST_FRAME, First_frame, LEN_FIRST_FRAME);
//			*(bs_buf + OFFSET_CMD) = TAG_SYNC_FILE_ID_STOP;
//			*(bs_buf + OFFSET_ERROR) = ERR_CODE_SUCCESS;
//			libc_memcpy(bs_buf + OFFSET_ERROR + 1, END_FRAME, LEN_END_FRAME);
//			ble_cmd_send(bs_buf, LEN_SUM + 2);
//            break;
//        }
//    }
//	if(need_send_end_frame) {
//	    libc_memcpy(bs_buf, END_FRAME, LEN_END_FRAME);
//        ble_file_send(bs_buf, LEN_END_FRAME);
//	}

//	xfclose(frxin);
//    libc_free(frxin);
//    return REC_RET_SUCCESS;
    return 0;
}

	
AT(.record_flash_seg)
void send_rec_file_by_id(void)
{
//	XFILE *frxin;
//	uint8_t *bs_buf;
//	FRESULT res;
//	char file_name[30]; 
//	bs_buf = libc_malloc(BT_BUFFER_SIZE);
//	frxin = libc_calloc(sizeof(*frxin));
//	uint32_t get_file_id;
//	uint32_t no_use;
//	if(bs_buf == NULL) {
//	  loge("bs_buf NULL");
//	}
//	logi("need_send_id %lx",NEED_SEND_FILE_ID);
//	for(uint32_t i = 0; i < rec_context->max_play_num; i++)
//	{
//		index_to_mp2_filename(i, file_name);
//		res = xfopen(frxin, file_name, FA_READ | FA_OPEN_EXISTING);
//		if (FR_OK != res) {
//			loge("open %s fail1",file_name);
//			continue;
//		}
//		read_head_info(frxin,&get_file_id,&no_use);
//		xfclose(frxin);
//		logi("name = %s",file_name);
//		logi("get_file_id = %lx",get_file_id);
//		if(NEED_SEND_FILE_ID == get_file_id){
//			start_send_record_file_by_num(bs_buf,i);
//		    break;
//		}
//	}
//	logi("arunboy1");
//	libc_free(frxin);
//	libc_free(bs_buf);
//	//return REC_RET_ERR;
}

AT(.mode_set_seg)
void send_rec_file_by_name(void)
{
//    uint8_t err = xTaskCreate(send_rec_file, "send_file",
//        64, NULL, MAIN_TASK_PRIO, &send_rec_file_handler);
//	logi("arunboy err = %d",err);
    //askStartScheduler();
//	char file_name[50] = RECORD_FILE_DIR;
//	XFILE *frxin;
//	FRESULT res;
//	uint32_t red;
//	FSIZE_t file_size;
//	uint32_t file_id = 0;
//	uint32_t file_duration = 0;
//	bool need_send_end_frame = true;
//	uint8_t *bs_buf;
//	bs_buf = libc_malloc(BT_BUFFER_SIZE);
//	if(bs_buf == NULL) {
//	  loge("bs_buf NULL");
//	}
//	frxin = libc_calloc(sizeof(*frxin));
//	
//	strcat(file_name,"/");
//    strcat(file_name, NEED_SEND_DEL_FILE_NAME);

//	logi("sync_file_name : %s",file_name);
//    logi("NEED_SEND_FILE_OFFSET = %d",NEED_SEND_FILE_OFFSET);
//	res = xfopen(frxin, file_name, FA_READ | FA_OPEN_EXISTING);
//    if (FR_OK != res) {
//        loge("open %s fail",file_name);
//		return;
//    }
//	read_head_info(frxin, &file_id, &file_duration);
//	file_size = xfsize(frxin);
//	libc_memcpy(bs_buf, BANBEN_ID, LEN_FIRST_BANBEN_ID);
//	libc_memcpy(bs_buf + OFFSET_FIRST_FRAME, First_frame, LEN_FIRST_FRAME);
//	libc_memcpy(bs_buf + 8, &file_id, 4);
//	libc_memcpy(bs_buf + 12, &file_size, 6);
//	ble_file_send(bs_buf, 18);
//	if(NEED_SEND_FILE_OFFSET > 0 && NEED_SEND_FILE_OFFSET < file_size){
//		res = xflseek(frxin, NEED_SEND_FILE_OFFSET);
//		if (FR_OK != res) {
//        loge("offset %d fail",NEED_SEND_FILE_OFFSET);
//        }
//	}
//    while (1) {
//        xfread(frxin, bs_buf, BT_BUFFER_SIZE, &red);
//        if (red > 0) {
//            ble_file_send(bs_buf,red);
//        } else {
//            break;
//        }
//        if (stop_sending_flag) {
//			libc_memcpy(bs_buf, END_FRAME, LEN_END_FRAME);
//			ble_file_send(bs_buf, LEN_END_FRAME);
//		    logi("zhudong send END_FRAME--------------");
//		    need_send_end_frame = false;
//			libc_memcpy(bs_buf, BANBEN_ID, LEN_FIRST_BANBEN_ID);
//	        libc_memcpy(bs_buf + OFFSET_FIRST_FRAME, First_frame, LEN_FIRST_FRAME);
//	        *(bs_buf + OFFSET_CMD) = TAG_SYNC_FILE_NAME_STOP;
//	        *(bs_buf + OFFSET_ERROR) = ERR_CODE_SUCCESS;
//	        libc_memcpy(bs_buf + OFFSET_ERROR + 1, END_FRAME, LEN_END_FRAME);
//	        ble_cmd_send(bs_buf, LEN_SUM + 2);
//            break;
//        }
//		if(get_ble_con_status() == false) {
//			need_send_end_frame = false;
//			break;
//		}
//    }
//	if(need_send_end_frame) {
//	    libc_memcpy(bs_buf, END_FRAME, LEN_END_FRAME);
//        ble_file_send(bs_buf, LEN_END_FRAME);
//	    logi("over send END_FRAME--------------");
//	}
//	//发送接收到的stop命令，在ble_app里面会概率性出现重复的现象
//	

//	xfclose(frxin);
//    libc_free(frxin);
//	libc_free(bs_buf);
}

/*
AT(.record_flash_seg)
void start_send_record(void)
{
    int32_t send_num;
    uint8_t *bs_buf;
    send_num = rec_context->max_play_num;
    bs_buf= libc_malloc(BT_BUFFER_SIZE);
    if (bs_buf == NULL) {
        logi("malloc fail");
    }
    while(1) {
       start_send_record_file_by_num(bs_buf,send_num);
       send_num--;
       if (send_num < 0 || stop_sending_flag || get_ble_con_status() == false){
            break;
       }
    }
    if (bs_buf) {
        libc_free(bs_buf);
    }
}
*/

/* 开始播放录音文件；
 * @param rec_context: 录音上下文
 */
AT(.record_flash_seg)
int start_play_record_file(void)
{
    if (SLTK_STYPE_FATFS == rec_context->streamtype) {
        if (rec_context->total_file <= 0) {
#if (TONE_EN && SUNCHIP_NO_FILES)
			play_tone(TONE_NO_FILES, true);
#endif
            return REC_RET_ERR;
        }

        if (rec_context->cur_play_num < 0) {
#if (TONE_EN && SUNCHIP_NO_FILES)
			play_tone(TONE_NO_FILES, true);
#endif
            return REC_RET_ERR;
        }

        if (rec_context->file_fail_count >= rec_context->total_file) {
#if (TONE_EN && SUNCHIP_NO_FILES)
			play_tone(TONE_NO_FILES, true);
#endif
            stop_play_record_file();
            return REC_RET_ERR;
        }

        if (SLTK_STYPE_FATFS == rec_context->streamtype) {
            prepare_record_device();
        }
    }

    np.paused = false;
    rec_context->play_direction = PLAY_NEXT;
    rec_context->status = REC_STATUS_PLAYING;
    stop_play_record_file();
    rec_context->status = REC_STATUS_PLAYING;
    index_to_filename(rec_context->cur_play_num, rec_context->play_file_name);
    event_clear(PLAYER_EVENT_ERR);

    source_info_t info;
    memset(&info, 0, sizeof(info));

    if (SLTK_STYPE_FATFS == rec_context->streamtype) {
        info.type = SLTK_STYPE_FATFS;
        info.dev_type = (DEV_UDISK == device_get_num()) ? PLAYER_DEV_UDISK : PLAYER_DEV_SDCARD;
        info.num = 0;
        strcpy(info.name, rec_context->play_file_name);
    } else {
        info.type = SLTK_STYPE_FLASH_RECORD;
        info.dev_type = 0;
        info.num = rec_context->cur_play_num;
    }
    info.seek_time = rec_context->breakpoint_time;
    rec_context->breakpoint_time = 0;
    player_loop_play(RECORD_PLAY_LOOP);
    player_open(&info);
    logd("play record file:%s", rec_context->play_file_name);

#if DISP_EN
    disp.menu = MENU_SCENE_MAIN;
    get_play_time();
    disp_menu(MENU_RECORD_PLAY, np.ptime.min, np.ptime.sec);
    disp_state(STATE_MUSIC_PLAY);
#endif
    auto_shutdown(false);
    return REC_RET_SUCCESS;
}

/* 播放下一个录音文件；
 * @param rec_context: 录音上下文
 */
AT(.record_flash_seg)
int play_next_record_file(void)
{
    FRESULT res;
    XFILE *file;
    uint32_t try_count = rec_context->max_play_num;

    if (false == device_is_online(DEV_SDCARD)
        && false == device_is_online(DEV_UDISK)) {
        loge("no rec dev");
        return REC_RET_ERR;
    }

    if (rec_context->total_file <= 0) {
        return REC_RET_ERR;
    }

    if (rec_context->file_fail_count >= rec_context->total_file) {
        stop_play_record_file();
        return REC_RET_ERR;
    }

    sleep_delay_set();
    sleep_prevent_set(PS_APP_PROCESS);

    prepare_record_device();

    if (REC_STATUS_RECORDING != rec_context->status) {
        stop_play_record_file();
        file = libc_calloc(sizeof(*file));
next:
        if (rec_context->cur_play_num < rec_context->max_play_num) {
            rec_context->cur_play_num++;
        } else {
            rec_context->cur_play_num = 0;
        }
        index_to_filename(rec_context->cur_play_num, rec_context->play_file_name);
        res = xfopen(file, rec_context->play_file_name, FA_READ | FA_OPEN_EXISTING);
        if (FR_NO_FILE == res || FR_NO_PATH == res) {
            xfclose(file);
            if (try_count) {
                --try_count;
                goto next;
            }
            libc_free(file);
        } else {
            xfclose(file);
            libc_free(file);
            if (FR_OK == res) {
                start_play_record_file();
                rec_context->play_direction = PLAY_NEXT;
            }
        }
    }

    sleep_delay_set();
    sleep_prevent_clear(PS_APP_PROCESS);

    return REC_RET_SUCCESS;
}

/* 播放上一个录音文件；
 * @param rec_context: 录音上下文
 */
AT(.record_flash_seg)
int play_prev_record_file(void)
{
    FRESULT res;
    XFILE *file;
    uint32_t try_count = rec_context->max_play_num;

    if (false == device_is_online(DEV_SDCARD)
        && false == device_is_online(DEV_UDISK)) {
        loge("no rec dev");
        return REC_RET_ERR;
    }

    if (rec_context->total_file <= 0) {
        return REC_RET_ERR;
    }

    if (rec_context->file_fail_count >= rec_context->total_file) {
        stop_play_record_file();
        return REC_RET_ERR;
    }

    sleep_delay_set();
    sleep_prevent_set(PS_APP_PROCESS);

    prepare_record_device();

    if (REC_STATUS_RECORDING != rec_context->status) {
        stop_play_record_file();
        file = libc_calloc(sizeof(*file));
prev:
        if (rec_context->cur_play_num > 0) {
            rec_context->cur_play_num--;
        } else {
            rec_context->cur_play_num = rec_context->max_play_num;
        }
        index_to_filename(rec_context->cur_play_num, rec_context->play_file_name);
        res = xfopen(file, rec_context->play_file_name, FA_READ | FA_OPEN_EXISTING);
        logv("try_count:%d, fopen res:%d", try_count, res);
        if (FR_NO_FILE == res || FR_NO_PATH == res) {
            xfclose(file);
            if (try_count) {
                --try_count;
                goto prev;
            }
            libc_free(file);
        } else {
            xfclose(file);
            libc_free(file);
            if (FR_OK == res) {
                start_play_record_file();
                rec_context->play_direction = PLAY_PREV;
            }
        }
    }

    sleep_delay_set();
    sleep_prevent_clear(PS_APP_PROCESS);

    return REC_RET_SUCCESS;
}


/* 播放上一个录音文件；
 * @param rec_context: 根据不同比特率设置采样参数
  */
AT(.record_flash_seg)
void update_record_param(uint8_t bit_rate,bool flash)
{

   if (bit_rate == REC_BIT_512KPBS) {
      rec_context->bit_rate_parm.fs = RECORD_FS;
      rec_context->bit_rate_parm.sz = RECORD_SZ_16BIT;
      logd("set 512KPBS");
      if(flash)
        disp_led_flase_time_do(0,300,2);
   } else if (bit_rate == REC_BIT_1536KPBS) {
      rec_context->bit_rate_parm.fs = RECORD_FS_48K;
      rec_context->bit_rate_parm.sz = RECORD_SZ_16BIT;
      logd("set 1536KPBS");
      if(flash)
        disp_led_flase_time_do(0,300,3);
   } else if (bit_rate == REC_BIT_3072KPBS) {
     logd("set 3072KPBS");
      rec_context->bit_rate_parm.fs = RECORD_FS_48K;
      rec_context->bit_rate_parm.sz = RECORD_SZ_32BIT;
      if(flash)
        disp_led_flase_time_do(0,300,4);
   }
   rec_context->bit_rate_parm.ch = RECORD_CHANNELS;
   rec_context->bit_rate_parm.fmt = RECORD_FORMAT;
   info_set(INFO_RECORD_INDEX, (void *)&bit_rate, 1);
   
}

AT(.record_flash_seg)
uint32_t get_record_fs_param(void)
{
    uint32_t fs;
    if (rec_context == NULL) {
        fs = RECORD_FS;
    } else {
        fs = rec_context->bit_rate_parm.fs > RECORD_FS? rec_context->bit_rate_parm.fs:RECORD_FS;
         
    }
   return fs;
}

AT(.record_flash_seg)
uint32_t get_record_ch_param(void)
{
     uint32_t ch;
    if (rec_context == NULL) {
        ch= RECORD_CHANNELS;
    } else {
         ch  =  (rec_context->bit_rate_parm.ch != RECORD_CHANNELS)?rec_context->bit_rate_parm.ch:RECORD_CHANNELS;
    }
    return ch;
}

AT(.record_flash_seg)
uint32_t get_record_bit_rate(void)
{
   return RECORD_FILE_BIT_RATE;
}


AT(.record_flash_seg)
uint8_t get_record_sz_param(void)
{
     uint8_t sz;
     if (rec_context == NULL) {
        sz = RECORD_SZ_16BIT;
        //logi("sz=%d", sz);
     } else {
        sz = rec_context->bit_rate_parm.sz > RECORD_SZ_16BIT? rec_context->bit_rate_parm.sz:RECORD_SZ_16BIT;
     }
     return sz;
}

AT(.record_flash_seg)
uint8_t get_record_fmt_param(void)
{
    uint8_t fmt;
    if (rec_context == NULL) {
        fmt = RECORD_FORMAT;
        //logi("fs=%d", fs);
    } else {
        fmt = rec_context->bit_rate_parm.fmt > RECORD_FORMAT? rec_context->bit_rate_parm.fmt:RECORD_FORMAT;
         
    }
   return fmt;
   
}

AT(.record_flash_seg)
uint8_t get_record_eq_param(void)
{
    return RECORD_EQ_SEL;
}

AT(.record_flash_seg)
uint8_t set_record_ble_param(bool en)
{
    if (rec_context != NULL) {
        if (en) {
            rec_context->bit_rate_parm.fmt = RECORD_FORMAT;
            rec_context->bit_rate_parm.ch = 1;
            rec_context->bit_rate_parm.fs = RECORD_FS;
            rec_context->bit_rate_parm.sz = RECORD_SZ_16BIT;
       } else {
           update_record_param(record_bitrate_index,0);
       }
       return 1;
    }
   return 0;
}

static uint8_t karaok_out = 0;
AT(.mode_record_seg)
void play_karaok_voice(int onoff)
{
    logi("arunboy onoff = %d",onoff);
    if (onoff) {
        update_record_param(REC_BIT_512KPBS,0);
        karaok_out = 1;
      } else {
        update_record_param(record_bitrate_index,0);
        karaok_out = 0;
    }
	/* Modify by arunboy 20200629 */
    //audio_service_reset_karaoke();
}

//extern uint32_t get_current_wav_id(void);

/* 开始录音；
 */
AT(.record_flash_seg)
void start_record(void)
{
    logd("start_record:%d, :%d", general_get_avaheapsize(), xPortGetFreeHeapSize());
    if (SLTK_STYPE_FATFS == rec_context->streamtype) {
        if (false == device_is_online(DEV_SDCARD)
            && false == device_is_online(DEV_UDISK)) {
            loge("no rec dev");
            return;
        }
        prepare_record_device();
    }

    stop_play_record_file();
    recorder_stream_params_t params;
    params.fs = get_record_fs_param();
    params.ch = get_record_ch_param();
    params.fmt = get_record_fmt_param();
    params.truncate_time = RECORD_TRUNCATE_TIME_MS;
    recorder_set_stream_params(&params);
    rec_context->cur_num++;
    rec_context->cur_play_num = rec_context->cur_num;
    rec_context->max_play_num = rec_context->cur_num;
	rec_context->mp2_max_play_num = rec_context->cur_num;
    rec_context->total_file++;
	rec_context->total_mp2_file++;
	index_to_filename(rec_context->cur_num, rec_context->file_name);
    index_to_mp2_filename(rec_context->cur_num,rec_context->mp2_file_name);
    recorder_open(rec_context->mp2_file_name, rec_context->file_name);
	/* Modify by arunboy 20200629 */
    audio_service_start_karaoke();
    recorder_start();
    logd("start record: %s", rec_context->file_name);
    logi("total file:%d", rec_context->total_file);
    rec_context->bit_rate_parm.ch = params.ch;
    rec_context->status = REC_STATUS_RECORDING;
    np.paused = false;
    if (ble_record_start_flag()) {   //透传模式
		send_record_message(TAG_RECORD_START);
        ble_app_cmd(BLE_APP_CMD_START, 1);
    } else if(get_ble_con_status()) { //普通开始录音
		send_record_message(TAG_SEND_APP_START_RECORD);
	}
#if DISP_EN
    disp.menu = MENU_SCENE_MAIN;
    get_record_time();
    disp_menu(MENU_RECORD_ING, rec_context->rtime.min, rec_context->rtime.sec);
	disp_state(STATE_REC_START);
	//disp_led_flase_time_do(1,100,5);
#endif
    auto_shutdown(false);
}

/* 停止录音；
 * @param rec_context: 录音上下文
 */
AT(.record_flash_seg)
void stop_record(void)
{
    if(get_ble_con_status() && REC_STATUS_RECORDING == rec_context->status){
		send_record_message(TAG_SEND_APP_STOP_RECORD);
	}
    recorder_stop();
    recorder_close();
    if (AR_ERR_NO_STORE_FILE == recorder_get_error_type()) {
        rec_context->cur_num--;
        rec_context->cur_play_num = rec_context->cur_num;
        rec_context->max_play_num = rec_context->cur_num;
        rec_context->total_file--;
        recorder_clear_error_type();
    }

    if (REC_STATUS_RECORDING == rec_context->status) {
		audio_service_stop_karaoke();
        rec_context->status = REC_STATUS_IDLE;
    }

#if DISP_EN
    disp_state(STATE_REC_END);
#endif
    auto_shutdown(true);
    //用户手动断掉apk开启的录音
    if(1 == ble_record_start_flag()) {
		set_ble_record_start_flag(0);
	}

}

void record_mode_scene_save(void)
{
    if (REC_STATUS_PLAYING == rec_context->status) {
        rec_context->breakpoint_time = player_get_current_play_time();
        stop_play_record_file();
    } else if (REC_STATUS_RECORDING == rec_context->status) {
        stop_record();
    }
}

void record_mode_scene_recovery(void)
{
    start_play_record_file();
}

/* 录音文件播放/暂停；
 */
AT(.record_flash_seg)
int play_or_pause_record(void)
{
    if (COMMON_TONE_STATUS_PLAYING == np.common_tone_status) {
        return PLAYER_RET_SUCCESS;
    }

    if (!np.paused) {
        if (AP_STATUS_PLAYING == player_get_status()) {
            logd("play -> pause");
            player_pause(0);
            np.paused = true;
#if DISP_EN
            disp_state(STATE_MUSIC_STOP);
#endif
			auto_shutdown(true);
        }
    } else {
        logd("pause -> play");
        disp_state(STATE_MUSIC_PLAY);
        player_resume(0);
        np.paused = false;
	    auto_shutdown(false);
    }

    return PLAYER_RET_SUCCESS;
}

/*
 * 获取录音时间
 */
AT(.record_flash_seg)
void get_record_time(void)
{
    uint32_t record_time;

    record_time = recorder_get_time();   //ms
    rec_context->rtime.ms = record_time;
    record_time /= 1000;
    rec_context->rtime.min = record_time / 60;
    rec_context->rtime.sec = record_time % 60;

    //logi("[%d:%d]", rec_context->rtime.min,rec_context->rtime.sec);
}

/* 选择录音设备；
 * @param dev_num: 设备号
 */
AT(.record_flash_seg)
int32_t select_record_device(uint32_t dev_num)
{
    int32_t ret;

    ret = device_try(dev_num);
    if (ret < 0) {
        if (DEV_SDCARD == dev_num) {
#if SD_EN
            sd_detect_set(1);
#endif
        } else if (DEV_UDISK == dev_num) {
#if USB_DISK_EN
            usb_host_deinit();
            usb_detect_set(USB_DETECT);
#endif
        }
        return -1;
    }

    if (dev.mounted == 0) {
        xfunmount("0:");
        if (DEV_SDCARD == dev_num) {
#if SD_EN
            xfs_fscache_sd_init();
            xfs_cmd(XFS_CMD_FSCACHE_ENABLE, 0);
            xfs_cmd(XFS_CMD_TIMEOUT_SET, 5000);     //uint: ms
#endif
        } else if (DEV_UDISK == dev_num) {
#if USB_DISK_EN
            xfs_fscache_usb_init();
            xfs_cmd(XFS_CMD_FSCACHE_ENABLE, 0);
            xfs_cmd(XFS_CMD_TIMEOUT_SET, 10000);
#endif
        }

        memset(&rec_context->fs, 0, sizeof(rec_context->fs));
        ret = xfmount(&rec_context->fs, "0:", 1);
        if (FR_OK != ret) {
            loge("xfmount err(%d)", ret);
#if SD_EN
            if (DEV_SDCARD == dev_num) {
                sd_detect_set(1);
            }
#endif
            return -1;
        }
        dev.mounted = 1;

        rec_context->mount_dev = dev_num;
        rec_context->cur_num = 0;
        rec_context->status = REC_STATUS_IDLE;
        rec_context->file_scaned = false;

        sleep_delay_set();
        sleep_prevent_set(PS_APP_PROCESS);

        scan_record_file();

        if (rec_context->total_file == 0) {
#if SD_EN
            if (dev_num == DEV_SDCARD) {
                sd_detect_set(1);
            }
#endif
        }

        event_clear(PLAYER_EVENT_ERR);

        sleep_delay_set();
        sleep_prevent_clear(PS_APP_PROCESS);
    }

    return 0;
}

/* 切换录音设备；
 * @param auto_play: true:自动播放录音文件； flase:不自动播放录音文件
 */
void switch_record_device(bool auto_play)
{
    int ret;
    uint32_t dev;
    uint32_t device_1st;
    uint32_t device_2nd;

#if RECORD_NEW_DEVICE_FIRST
    /* new device first */
    dev = rec_context->new_dev;
    if (DEV_UDISK == dev) {
        device_1st = DEV_UDISK;
        device_2nd = DEV_SDCARD;
    } else if (DEV_SDCARD == dev) {
        device_1st = DEV_SDCARD;
        device_2nd = DEV_UDISK;
    } else {
        device_1st = DEV_UDISK;
        device_2nd = DEV_SDCARD;
    }
#else
    /* old device first */
    dev = rec_context->new_dev;
    if (DEV_UDISK == dev) {
        if (device_is_online(DEV_SDCARD)) {
            device_1st = DEV_SDCARD;
            device_2nd = DEV_UDISK;
        } else {
            device_1st = DEV_UDISK;
            device_2nd = DEV_SDCARD;
        }
    } else if (DEV_SDCARD == dev) {
        if (device_is_online(DEV_UDISK)) {
            device_1st = DEV_UDISK;
            device_2nd = DEV_SDCARD;
        } else {
            device_1st = DEV_SDCARD;
            device_2nd = DEV_UDISK;
        }
    } else {
        device_1st = DEV_UDISK;
        device_2nd = DEV_SDCARD;
    }
#endif

    if (DEV_OK != device_get_sta(device_1st) || rec_context->mount_dev != device_1st) {
        stop_record();
        stop_play_record_file();
        ret = select_record_device(device_1st);
        if (ret < 0) {
            ret = select_record_device(device_2nd);
            if (ret < 0) {
                device_set_num(DEV_NONE);
            } else {
                if (auto_play) {
                    start_play_record_file();
                }
            }
        } else {
            if (auto_play) {
                start_play_record_file();
            }
        }
    }
}
#endif

#if (RECORD_LINEIN_FUNC || RECORD_FM_FUNC)

//linein 录音初始化
void linein_record_init(uint32_t rec_func)
{
    int32_t ret;
    if (NULL == rec_context) {
        rec_context = libc_calloc(sizeof(*rec_context));
        if (NULL == rec_context) {
            loge("calloc rec_context fail");
            return;
        }
    }

    recorder_init_params_t params;
#if (0 == RECORD_STREAM_TYPE)
    rec_context->streamtype = SLTK_STYPE_FATFS;
#elif (1 == RECORD_STREAM_TYPE)
    rec_context->streamtype = SLTK_STYPE_FLASH_RECORD;
#endif

    params.streamtype = rec_context->streamtype;
    switch (rec_func) {
    case 1: //录linein
        params.mode_num = 1;
        break;

    case 2: //录mic
        params.mode_num = 0;
        break;

    case 3: //录mic+linein
    default:
        params.mode_num = 2;
        break;
    }
    create_recorder(&params);
    recorder_set_event_listener(recorder_event_listener);

    if (SLTK_STYPE_FATFS == rec_context->streamtype) {
        //录到文件系统
        ret = select_record_device(DEV_UDISK);
        if (ret < 0) {
        #if SD_EN
            ret = select_record_device(DEV_SDCARD);
            if (ret < 0) {
                device_set_num(DEV_NONE);
                logi("have no dev");
            } else {
                rec_context->new_dev = DEV_SDCARD;
                if (device_get_num() == DEV_SDCARD) {
                    sd_detect_set(1);
                }
            }
        #endif
        } else {
        #if SD_EN
            if (device_get_num() == DEV_SDCARD) {
                sd_detect_set(1);
            }
        #endif
        }
    } else if (SLTK_STYPE_FLASH_RECORD == rec_context->streamtype) {
        //录到flash
    }
}

//linein 录音退出
void linein_record_deinit(void)
{
    if (rec_context) {
        if (REC_STATUS_RECORDING == rec_context->status ||
            REC_STATUS_IDLE == rec_context->status) {
            stop_record();
        } else if (REC_STATUS_PLAYING == rec_context->status) {
            stop_play_record_file();
            audio_service_set_playback_volume(np.volume);
        }
    }

    destroy_recorder();

    if (SLTK_STYPE_FATFS == rec_context->streamtype) {
        f_unmount("0:");
        xfs_cmd(XFS_CMD_FSCACHE_DEINIT, 1);
    }

    if (rec_context) {
        libc_free(rec_context);
        rec_context = NULL;
    }
}

//更新linein录音(或者录音播放)的显示时间
void linein_record_update_disp_time(void)
{
    if (REC_STATUS_RECORDING == rec_context->status) {
        get_record_time();
        //disp_menu(MENU_RECORD_ING, rec_context->rtime.min, rec_context->rtime.sec);
    } else if (REC_STATUS_PLAYING == rec_context->status) {
        get_play_time();
        //disp_menu(MENU_RECORD_PLAY, np.ptime.min, np.ptime.sec);
    }
}

//linein录音处理SD卡事件
void linein_record_proc_sd_event(uint8_t mode, uint32_t event)
{
#if 0
#if SD_EN
    if (SLTK_STYPE_FATFS != rec_context->streamtype) {
        return;
    }

    switch (event) {
    case DEVICE_EVENT_SD_IN:
        logi("sd in");
        if (REC_STATUS_PLAYING == rec_context->status) {
            stop_play_record_file();     //stop play record file
            audio_service_set_playback_volume(np.volume);
            adda_init(mode);
        } else if (REC_STATUS_RECORDING == rec_context->status) {
            logd("stop rec");
            stop_record();
        }
        rec_context->new_dev = DEV_SDCARD;
        if (device_get_num() != DEV_SDCARD) {
            if (RECORD_NEW_DEVICE_FIRST) {
                switch_record_device(0);
                sd_detect_set(1);
            }
        }
        break;

    case DEVICE_EVENT_SD_OUT:
        if (REC_STATUS_PLAYING == rec_context->status && device_get_num() == DEV_SDCARD) {
            stop_play_record_file();     //stop play record file
            audio_service_set_playback_volume(np.volume);
            adda_init(mode);
        } else if (REC_STATUS_RECORDING == rec_context->status && device_get_num() == DEV_SDCARD) {
            logd("stop rec");
            stop_record();
        }

        rec_context->new_dev = DEV_UDISK;
        sd_detect_set(1);
        if (device_get_num() == DEV_SDCARD) {
            switch_record_device(0);
        }
        break;

    default:
        break;
    }
#endif //SD_EN
#endif
}

//linein录音处理U盘事件
void linein_record_proc_udisk_event(uint8_t mode, uint32_t event)
{
#if 0
#if USB_DISK_EN
    if (SLTK_STYPE_FATFS != rec_context->streamtype) {
        return;
    }

    switch (event) {
    case DEVICE_EVENT_UDISK_IN:
        logi("udisk in");
        if (REC_STATUS_PLAYING == rec_context->status) {
            stop_play_record_file();     //stop play record file
            audio_service_set_playback_volume(np.volume);
            adda_init(mode);
        } else if (REC_STATUS_RECORDING == rec_context->status) {
            logd("stop rec");
            stop_record();
        }

        rec_context->new_dev = DEV_UDISK;
        if (device_get_num() != DEV_UDISK) {
            if (RECORD_NEW_DEVICE_FIRST) {
                switch_record_device(0);
            }
            sd_detect_set(1);
        }
        break;

    case DEVICE_EVENT_UDISK_OUT:
        logi("udisk out");
        if (REC_STATUS_PLAYING == rec_context->status  && device_get_num() == DEV_UDISK) {
            stop_play_record_file();     //stop play record file
            audio_service_set_playback_volume(np.volume);
            adda_init(mode);
        } else if (REC_STATUS_RECORDING == rec_context->status && device_get_num() == DEV_UDISK) {
            logd("stop rec");
            stop_record();
        }

        rec_context->new_dev = DEV_SDCARD;
        usb_host_deinit();
        usb_detect_set(USB_DETECT);
        if (device_get_num() == DEV_UDISK) {
            switch_record_device(0);
            sd_detect_set(1);
        }
        break;

    default:
        break;
    }
#endif //USB_DISK_EN
#endif
}

void linein_record_play_rec_file(uint32_t mode_id)
{
    stop_record();               //stop record
    adda_deinit(mode_id);
    /* 如果录linein,由于录的是音量处理前的数据，因此播放时保持原音量可听到相同音量大小的声音
     * 如果录的是linein+mic，由于录的是音量处理后的数据，因此播放需要设到100音量，否则声音大小会有衰减
     */
    if (MODE_LINEIN == mode_id) {
        if (RECORD_LINEIN_FUNC == 3) {
            linein_play_vol = 100;
            audio_service_set_playback_volume(linein_play_vol);
        }
    } else if (MODE_FM == mode_id) {
        if (RECORD_FM_FUNC == 3) {
            fm_play_vol = 100;
            audio_service_set_playback_volume(fm_play_vol);
        }
    }

    start_play_record_file();    //start play record file
}

void linein_stop_play_rec_file(uint8_t mode)
{
    stop_play_record_file();     //stop play record file
    audio_service_set_playback_volume(np.volume);
    adda_init(mode);
#if SD_EN
    if (DEV_SDCARD == device_get_num()) {
        sd_detect_set(1);
    }
#endif
}

#endif

AT(.karaoke_sram_seg)
uint8_t get_karaok_aout_enable(void)
{
    return karaok_out;
}

//static uint8_t karaok_enable = 0;


#if MODE_RECORD_EN

AT(.mode_record_seg)
void record_event_process(uint32_t event)
{
    uint32_t type;

    switch(event) {
#if 0
#if SD_EN
    case DEVICE_EVENT_SD_IN:
        logi("sd in");
        rec_context->new_dev = DEV_SDCARD;
        if (device_get_num() != DEV_SDCARD) {
            if (RECORD_NEW_DEVICE_FIRST || RECORD_AUTO_PLAY) {
                switch_record_device(RECORD_AUTO_PLAY);
            }
        }
        if (RECORD_AUTO_PLAY == 0) {
            sd_detect_set(1);
        }
        break;

    case DEVICE_EVENT_SD_OUT:
        logi("sd out");
        rec_context->new_dev = DEV_UDISK;
        sd_detect_set(1);
        if (device_get_num() == DEV_SDCARD) {
            switch_record_device(RECORD_AUTO_PLAY);
        }
        break;

    case DEVICE_EVENT_SD_FAIL:
        logi("record,sd fail.");
        rec_context->new_dev = DEV_UDISK;
        sd_detect_set(1);

        if (device_get_num() == DEV_SDCARD) {
            switch_record_device(RECORD_AUTO_PLAY);
        }
        break;
#endif

#if USB_DISK_EN
    case DEVICE_EVENT_UDISK_IN:
        logi("udisk in");
        rec_context->new_dev = DEV_UDISK;
        if (device_get_num() != DEV_UDISK) {
            if (RECORD_NEW_DEVICE_FIRST || RECORD_AUTO_PLAY) {
                switch_record_device(RECORD_AUTO_PLAY);
            }
            sd_detect_set(1);
        }
        break;

    case DEVICE_EVENT_UDISK_OUT:
        logi("udisk out");
        rec_context->new_dev = DEV_SDCARD;
        usb_host_deinit();
        usb_detect_set(USB_DETECT);
        if (device_get_num() == DEV_UDISK) {
            switch_record_device(RECORD_AUTO_PLAY);
        }
        break;
#endif
#endif
    case AR_EVENT_CLOSE:
        logd("close file");
        break;

    case AR_EVENT_ERROR:
        logd("record error event");
        stop_record();
#if SD_EN
        if (device_get_num() == DEV_SDCARD) {
            sd_detect_set(1);
        }
#endif
        break;

#if (0 == RECORD_PLAY_LOOP)
    case PLAYER_EVENT_END_OF_FILE:
        if (REC_RET_SUCCESS == play_next_record_file()) {
            //disp_menu(MENU_NUM, rec_context->cur_play_num, 0);
        } else {
#if SD_EN
            if (device_get_num() == DEV_SDCARD) {
                sd_detect_set(1);
            }
#endif
        }
        break;
#endif

    case PLAYER_EVENT_ERR:              //play err
        type = player_get_error_type();
        logw("play error (type=%d)", type);
        if (APERR_FORMAT_PROBE == type ||
            APERR_CREATE_PARSER == type ||
            APERR_OPEN_INVALID_PARAMS == type ||
            APERR_CREATE_DEC == type ||
            APERR_OPEN_DEC == type ||
            APERR_OPEN_TRACK == type) {
            if (device_get_num() == DEV_SDCARD) {
#if SD_EN
                if (DEV_OK == device_get_sta(DEV_SDCARD)) {
                    ++(rec_context->file_fail_count);
                } else {
                    rec_context->file_fail_count = 0;
                }
#endif
            } else if (device_get_num() == DEV_UDISK) {
#if USB_DISK_EN
                if (DEV_OK == device_get_sta(DEV_UDISK)) {
                    ++(rec_context->file_fail_count);
                } else {
                    rec_context->file_fail_count = 0;
                }
#endif
            }
        }

        if (PLAY_NEXT == rec_context->play_direction) {
            if (REC_RET_SUCCESS == play_next_record_file()) {
                //disp_menu(MENU_NUM, rec_context->cur_play_num, 0);
            } else {
#if SD_EN
                if (device_get_num() == DEV_SDCARD) {
                    sd_detect_set(1);
                }
#endif
            }
        } else {
            if (REC_RET_SUCCESS == play_prev_record_file()) {
                //disp_menu(MENU_NUM, rec_context->cur_play_num, 0);
            } else {
#if SD_EN
                if (device_get_num() == DEV_SDCARD) {
                    sd_detect_set(1);
                }
#endif
            }
        }
        break;

    case PLAYER_EVENT_PAUSE_COMPLETED:            //pause
        logd("pause");
        if (!np.paused) {
            break;
        }
        get_play_time();
        //disp_menu(MENU_RECORD_PLAY, np.ptime.min, np.ptime.sec);
        break;

    case KEY_EVENT_SU | KEY_VOL_UP:
#if maikefeng
        logi("maikefengvol+");
        volume_up();
		break;
#endif
    case KEY_EVENT_SU | KEY_PREV:       //previous file
        logd("prev");
        if (REC_STATUS_PLAYING == rec_context->status) {
            rec_context->file_fail_count = 0;
            if (REC_RET_SUCCESS == play_prev_record_file()) {
                //disp_menu(MENU_NUM, rec_context->cur_play_num, 0);
            } else {
#if SD_EN
                if (device_get_num() == DEV_SDCARD) {
                    sd_detect_set(1);
                }
#endif
            }
        }
        break;

    case KEY_EVENT_SU | KEY_VOL_DOWN:
#if maikefeng
		logi("maikefengvol-");
		volume_down();
		break;
#endif
    case KEY_EVENT_SU | KEY_NEXT:       //next file
        logd("next");
        if (REC_STATUS_PLAYING == rec_context->status) {
            rec_context->file_fail_count = 0;
            if (REC_RET_SUCCESS == play_next_record_file()) {
                //disp_menu(MENU_NUM, rec_context->cur_play_num, 0);
            } else {
#if SD_EN
                if (device_get_num() == DEV_SDCARD) {
                    sd_detect_set(1);
                }
#endif
            }
        }
        break;
	case KEY_EVENT_SU | KEY_PLAY_REC:
    case KEY_EVENT_L | KEY_PLAY:        //start rec
    case KEY_EVENT_L | KEY_PLAY_MODE:
		logi("event 0x%x",event);
        mode_switch_auto();
        break;
#if 0
	case KEY_EVENT_SU | KEY_PLAY_SAVE_REC:		
		 if (REC_STATUS_RECORDING == rec_context->status) {
            logd("save");
            stop_record();
#if DISP_EN
        disp_state(STATE_REC_END);
#endif
           // start_play_record_file();
        } 
		break;
#endif
    case KEY_EVENT_SU | KEY_PLAY:       //play rec
        if(!hp_get_get_detach_state()) { //不插耳机的时候
            if(REC_STATUS_RECORDING == rec_context->status) {//如果正在录音
               stop_record();
			   #if DISP_EN
                   disp_state(STATE_REC_END);//蓝灯亮
               #endif
			} else {
               start_record();
			}
			break;
		}
    case KEY_EVENT_SU | KEY_PLAY_MODE:
    case KEY_EVENT_SU | KEY_HP_IN:   //插入耳机
#if RECORD_MIC_FUNC
        if (REC_STATUS_IDLE == rec_context->status ||
            REC_STATUS_RECORDING == rec_context->status) {
            logd("play");
            stop_record();
#if DISP_EN
            disp_state(STATE_REC_END);
#endif
            start_play_record_file();
        } else if (REC_STATUS_PLAYING == rec_context->status) {
            play_or_pause_record();
        }
#endif
        break;
	case KEY_EVENT_SU | KEY_HP_OUT:  //拔出耳机
	    stop_play_record_file();
		disp_state(STATE_REC_END);
		auto_shutdown(true);
		break;
    case KEY_EVENT_L | KEY_PREV://delete rec
        logd("del rec");
        //delete_record_file_by_num();
#if SD_EN
        if (device_get_num() == DEV_SDCARD) {
            sd_detect_set(1);
        }
#endif
        break;

    //case KEY_EVENT_SU | KEY_VOL_DOWN:   //volume down
    case KEY_EVENT_H | KEY_VOL_DOWN:
#if maikefeng
        break;
#endif
        if (REC_STATUS_PLAYING == rec_context->status) {
            logi("vol-");
            volume_down();
            //disp_menu(MENU_VOLUME, np.volume, 0);
        }
        break;

    //case KEY_EVENT_SU | KEY_VOL_UP:     //volume up
    case KEY_EVENT_LU | KEY_VOL_UP:     //volume up
    case KEY_EVENT_H | KEY_VOL_UP:
#if maikefeng
        break;
#endif
        if (REC_STATUS_PLAYING == rec_context->status) {
            logi("vol+");
            volume_up();
            //disp_menu(MENU_VOLUME, np.volume, 0);
        }
        break;

#if DISP_EN
    case DISP_EVENT_UPDATE:
        if(disp.menu == MENU_SCENE_MAIN) {
            if (REC_STATUS_RECORDING == rec_context->status) {
                get_record_time();
                //disp_menu(MENU_RECORD_ING, rec_context->rtime.min, rec_context->rtime.sec);
            } else if (REC_STATUS_PLAYING == rec_context->status){
                get_play_time();
                //disp_menu(MENU_RECORD_PLAY, np.ptime.min, np.ptime.sec);
            }
        }
        break;
#endif
    case KEY_BLE_START_RECORD:
        //stop_record();
        if (set_record_ble_param(true)){
		    /* Modify by arunboy 20200629 */
            //audio_service_reset_karaoke();
            logd("start rec");
            start_record();
        }
        break;
    case KEY_BLE_STOP_RECORD:
		send_record_message(TAG_RECORD_STOP);
        logd("stop rec");
        stop_record();
	    ble_app_cmd(BLE_APP_CMD_STOP, 1);
        set_record_ble_param(false);
        break;
	/* Modify by arunboy 20200601 */
	case KEY_BLE_SEND_DIR_FILES_LIST:
//		if (REC_STATUS_RECORDING == rec_context->status) {
//            stop_record();
//        } else if (REC_STATUS_PLAYING == rec_context->status) {
//            stop_play_record_file();
//        }
        start_send_dir_files_list();
        break;
    case KEY_BLE_START_SEND_RECORD_BY_ID:
        if (REC_STATUS_RECORDING == rec_context->status) {
            stop_record();
        } else if (REC_STATUS_PLAYING == rec_context->status) {
            stop_play_record_file();
        }
//       stop_sending_flag = false;
		send_rec_file_by_id();
        break;
	case KEY_BLE_START_SEND_RECORD_BY_NAME:
		if (REC_STATUS_RECORDING == rec_context->status) {
            stop_record();
        } else if (REC_STATUS_PLAYING == rec_context->status) {
            stop_play_record_file();
        }
//        stop_sending_flag = false;
		send_rec_file_by_name();
        break;
    case LE_EVENT_APP:
        ble_app_process();
        break;
	
	case RTC_EVENT_SECOND:
		  rtc_get_time(&rtc.t);
		  if (0 == rtc.t.second) {
			  logi("bbbrtc:%04d-%02d-%02d, %02d:%02d:%02d",
				  rtc.t.year + BASE_YEAR, rtc.t.month, rtc.t.day,
				  rtc.t.hour, rtc.t.minute, rtc.t.second);
//			  disp_menu(MENU_CLOCK_TIME,rtc.t.hour, rtc.t.minute);
			  get_record_time();
			  logi("rec_time : %d", rec_context->rtime.min);
			  if(rec_context->rtime.min >= 120){
				  stop_record();
				  start_record();
			  }
		  }
		  break;
		  
    default:
        common_event_process(event);
        break;
    }
}
extern uint8_t ble_tast_init(void);

AT(.mode_record_seg)
void mode_record(void)
{
    logi("record mode.");

    mode_record_enter();
    mode_record_loop();
	logd("arunboy back mode_record_loop");
    mode_record_exit();
	logd("arunboy back mode_record_exit");
}
extern bool HP_OUT_BUT_NOT_START_RECORD;
AT(.mode_record_seg)
void mode_record_enter(void)
{
    uint32_t device_1st;
    uint32_t device_2nd;
    logi("arunboy now is mode_record_enter!");
    //disp_menu(MENU_SCENE_MAIN, scene_mode.sta, 0);
#if (TONE_EN && SUNCHIP_RECORD_MODE)
    play_tone(TONE_RECORD_MODE, true);
#endif

#if (EQ_ALL_MODE_SUPPORT || USB_AUDIO_ALL_MODE_SUPPORT)
    usb_dev_enter();
#endif

#if RECORD_MIC_FUNC
    int32_t ret;

    if (NULL == rec_context) {
        rec_context = libc_calloc(sizeof(*rec_context));
        if (NULL == rec_context) {
            loge("calloc rec_context fail");
        }
    }
    logd("111audio heap:%d, os heap:%d", general_get_avaheapsize(), xPortGetFreeHeapSize());
    update_record_param(record_bitrate_index, 0);
	/* Modify by arunboy 20200629 */
    //audio_service_reset_karaoke();
    recorder_init_params_t params;
#if (0 == RECORD_STREAM_TYPE)
    rec_context->streamtype = SLTK_STYPE_FATFS;
#elif (1 == RECORD_STREAM_TYPE)
    rec_context->streamtype = SLTK_STYPE_FLASH_RECORD;
#endif
    params.streamtype = rec_context->streamtype;
    params.mode_num = 0;
    create_recorder(&params);
    recorder_set_event_listener(recorder_event_listener);
    if (SLTK_STYPE_FATFS == rec_context->streamtype) {
        if (scene_mode.switch_reason == SW_SDCARD) {
            //play sdcard
            device_1st = DEV_SDCARD;
            device_2nd = DEV_UDISK;
        } else if (scene_mode.switch_reason == SW_UDISK) {
            //play udisk
            device_1st = DEV_UDISK;
            device_2nd = DEV_SDCARD;
        } else if (scene_mode.switch_reason == SW_BTBG) {
            //play last device
            device_1st = device_get_num();
            device_2nd = device_get_next_num();
        } else {
            //first try play udisk,if fail,try play sdcard
            device_1st = DEV_UDISK;
            device_2nd = DEV_SDCARD;
        }

        rec_context->mount_dev = DEV_NONE;
        rec_context->new_dev = device_1st;
        ret = select_record_device(device_1st);
		logi("arunboy ret = %d,device_1st = %d,device_2nd = %d",ret,device_1st,device_2nd);
        if (ret < 0) {
            ret = select_record_device(device_2nd);
            if (ret < 0) {
                device_set_num(DEV_NONE);
#if maikefeng
                play_karaok_voice(1);
#endif
				logd("arunboy no dev");
		        disp_state(STATE_REC_ERROR);
               //mode_switch_auto();
            } else {
                rec_context->new_dev = device_2nd;
                
                #if RECORD_AUTO_PLAY				
				logd("arunboy arunboy hp_get_get_detach_state() = %d", hp_get_get_detach_state());
                if (hp_get_get_detach_state()) {
                     if (rec_context->total_file == 0) {
//                     #if DISP_EN
//                         disp_led_flase_time_do(1,200,5);
//                     #endif
                         //mode_switch_auto();
                     }
                    if (REC_RET_ERR == start_play_record_file()) {
                        #if SD_EN
                        if (device_get_num() == DEV_SDCARD) {
                            sd_detect_set(1);
                        }
                        #endif
                    }
                } else {
                    #if RECORD_MIC_FUNC
                        logd("start rec");
                        player_loop_play(0);
						if(HP_OUT_BUT_NOT_START_RECORD){
							disp_state(STATE_REC_END);
						} else {
						#if TURN_ON_AUTO_REC
                            start_record();
						#endif
						}
						//#if DISP_EN
                        //    disp_state(STATE_REC_START);
                        //#endif
                    #endif
                }
                #else
                #if SD_EN
                if (device_get_num() == DEV_SDCARD) {
                    sd_detect_set(1);
                }
                #endif
                #endif
            }
        } else {
            #if RECORD_AUTO_PLAY
			logd("arunboy hp_get_get_detach_state() = %d", hp_get_get_detach_state());
            if (hp_get_get_detach_state()) {
                if (REC_RET_ERR == start_play_record_file()) {
                    #if SD_EN
                    if (device_get_num() == DEV_SDCARD) {
                        sd_detect_set(1);
                    }
                    #endif
                }
            } else {
                 #if RECORD_MIC_FUNC
                    logd("start rec");
                    player_loop_play(0);
					logi("%d start_record",__LINE__);
				 #if TURN_ON_AUTO_REC
                    start_record();
				 #endif
                    //#if DISP_EN
                    //    disp_state(STATE_REC_START);
                    //#endif
                 #endif
            }
            #else
            #if SD_EN
            if (device_get_num() == DEV_SDCARD) {
                sd_detect_set(1);
            }
            #endif
            #endif
        }
    } else {
        #if RECORD_AUTO_PLAY
        start_play_record_file();
        #endif
    }
	ext_getfree(0);
	ext_getfree(1);
#if !TURN_ON_AUTO_REC
    disp_state(STATE_REC_END);    
#endif
	logi("arunboy now is go back mode_record_enter!");
#endif
}

AT(.mode_record_seg)
void mode_record_loop(void)
{
    uint32_t event;
    while(scene_mode.sta == MODE_RECORD) {
        event = event_get(true);
        record_event_process(event);
    }
}

AT(.mode_record_seg)
void mode_record_exit(void)
{
    scene_mode.last_sta = MODE_RECORD;

#if (EQ_ALL_MODE_SUPPORT || USB_AUDIO_ALL_MODE_SUPPORT)
    usb_dev_exit();
#endif

    player_loop_play(0);

#if RECORD_MIC_FUNC
    if (rec_context) {
        if (REC_STATUS_RECORDING == rec_context->status ||
            REC_STATUS_IDLE == rec_context->status) {
			if(DEV_NONE != device_get_num()) {
				stop_record();
				//audio_service_stop_admomini();
			}
        } else if (REC_STATUS_PLAYING == rec_context->status) {
            stop_play_record_file();
        }
    }

#if SD_EN
    if (DEV_SDCARD == device_get_num()) {
        sd_stop();
    }
#endif
    audio_service_stop_admomini();
    destroy_recorder();
    xfunmount("0:");
    xfs_cmd(XFS_CMD_FSCACHE_DEINIT, 1);

//    if (rec_context) {
//        libc_free(rec_context);
//        rec_context = NULL;
//    }
#endif

    logi("record exit.");
}

#endif
