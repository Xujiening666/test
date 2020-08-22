#ifndef _INFO_MEM_H_
#define _INFO_MEM_H_

#define FILE_NUM_DEFAULT        1
#define FILE_MS_DEFAULT         0

#define FM_FREQ_DEFAULT         0
#define FM_TOTAL_DEFAULT        1
#define FM_CH_CUR_DEFAULT       1

//info mem addr define (rtc memory，地址范围：0x00 ~ 0x0F，共64 byte)
#define INFO_MUSIC_FNUM         0x00    //0-sd fnum; 1-u fnum;
#define INFO_MUSIC_PTIME        0x02    //2-sd ptime; 3-u ptime;
#define INFO_FILE_CHECKSUM      0x04    //4-sd checksum; 5-u checksum;
#define INFO_SYS_STARTUP        0x06    //6-system startup first flag

//info mem addr define (norflash，地址范围：0x00 ~ 0xFF，共256 byte)
#define INFO_SDCARD_NUM         0x00    //music sdcard num          4
#define INFO_UDISK_NUM          0x04    //music udisk num           4
#define INFO_SDCARD_CHECKSUM    0x08    //music sdcard checksum     4
#define INFO_UDISK_CHECKSUM     0x0C    //music udisk checksum      4
#define INFO_SDCARD_TIME        0x10    //music sdcard time         4
#define INFO_UDISK_TIME         0x14    //music udisk time          4
#define INFO_MUSIC_DEVICE       0x18    //music device              1
#define INFO_VOLUME             0x19    //system volume             1
#define INFO_BTCALL_VOLUME      0x1A    //bt call volume            1
#define INFO_REPEAT_SEL         0x1B    //music repeat sel          1
#define INFO_EQ_SEL             0x1C    //music eq sel              1

#define INFO_ALARM_HOUR         0x20    //alarm hour                1
#define INFO_ALARM_MIN          0x21    //alarm min                 1
#define INFO_ALARM_EN           0x22    //alarm en                  1
#define INFO_ALARM_MODE         0x23    //alarm mode                1

#define INFO_FM_FREQ            0x30    //fm freq                   1
#define INFO_FM_TOTAL           0x31    //fm ch total               1
#define INFO_FM_CH_CUR          0x32    //fm ch cur                 1
#define INFO_FM_CH              0x33    //fm ch                     1*50 (0x33~0x64)
#define INFO_RECORD_INDEX       0x70    //record index              1
/* Modify by arunboy 20200605 */
#define INFO_USER_BIND_FLAG     0x71    //user_id bind bit          1
#define INFO_USER_INFO          0x72    //user_id_info              4(0x72~0x75)
#define INFO_MTU_NUM            0x7C    //MTU data                  1
#define INFO_PRIVATE_MODE_FLAG  0x7D    //private mode flag         1
#define INFO_SD_FRE_SECT        0x80    //sd fre_sect               4
#define INFO_SD_TOT_SECT        0x84    //sd tot_sect               4


#define INFO_BT_OFFSET          0x90    //bt                        96(0x90~0xEF)
#define INFO_BLE_RAND_NUM       0xF4    //ble rand num              4
#define INFO_RAND_NUM           0xF8    //rand num                  4

#define INFO_CRC                0xFC    //crc                       2
#define INFO_SIGN               0xFE    //sign(0x53,0x4C)           2

void info_mem_init(void);
void info_set(uint32_t param, void *addr, uint32_t bytes);
void info_get(uint32_t param, void *addr, uint32_t bytes);
void info_write_page(void);
void info_setting_default(void);

void sys_setting_get(uint8_t *ibuf);
void sys_setting_default(uint8_t *ibuf);
void info_mem_test(void);
void info_write_seek_table(uint32_t table, uint32_t len,uint32_t load_flag,uint32_t load_flag_len);
void info_read_seek_table(uint32_t table, uint32_t len,uint32_t load_flag,uint32_t load_flag_len);


#endif

