#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "player.h"
#include "info_mem.h"
#include "trng.h"
#include "record.h"


uint32_t random_num;
uint32_t ble_random_num;
uint8_t record_bitrate_index;

AT(.info_init_seg)
void sys_setting_get(uint8_t *ibuf)
{
    np.volume = R1BYTE(&ibuf[INFO_VOLUME]);
    if (np.volume > VOLUME_MAX) {
        np.volume = VOLUME_MAX;
    }
    if (np.volume < VOLUME_MIN) {
        np.volume = VOLUME_MIN;
    }
    logi("vol = %d", np.volume);

#if BT_PHONE_EN
    np.btcall_volume = R1BYTE(&ibuf[INFO_BTCALL_VOLUME]);
    if (np.btcall_volume > VOLUME_MAX) {
        np.btcall_volume = VOLUME_MAX;
    }
    if (np.btcall_volume < VOLUME_MIN) {
        np.btcall_volume = VOLUME_MIN;
    }
#endif

#if (MODE_MUSIC_EN && MUSIC_INFO_SAVE_MODE == 2)
    np.breakpoint_rd = 1;
    np.file_num = R4BYTE(&ibuf[INFO_SDCARD_NUM]);
    np.ptime.ms = R4BYTE(&ibuf[INFO_SDCARD_TIME]);
    //logi("num = %u", np.file_num);
    //logi("ms = %u", np.ptime.ms);
#endif

#if MODE_FM_EN
    fm_mode.freq = R1BYTE(&ibuf[INFO_FM_FREQ]) + FM_FREQ_MIN;
    if ((fm_mode.freq < FM_FREQ_MIN) || (fm_mode.freq > FM_FREQ_MAX)) {
        fm_mode.freq = FM_FREQ_MIN;
    }
    fm_mode.ch_total = R1BYTE(&ibuf[INFO_FM_TOTAL]);
    if ((fm_mode.ch_total == 0) || (fm_mode.ch_total > FM_CH_MAX)) {
        fm_mode.ch_total = 1;
    }
    fm_mode.ch_cur = R1BYTE(&ibuf[INFO_FM_CH_CUR]);
    if ((fm_mode.ch_cur == 0) || (fm_mode.ch_cur > fm_mode.ch_total)) {
        fm_mode.ch_cur = 1;
    }
    for (uint8_t n=0;n<fm_mode.ch_total;n++) {
        fm_mode.ch[n] = R1BYTE(&ibuf[INFO_FM_CH+n]);
    }
    //logi("freq = %d", fm_mode.freq);
    //logi("ch_total = %d", fm_mode.ch_total);
    //logi("ch_cur = %d", fm_mode.ch_cur);
    //for (uint8_t n=0;n<fm_mode.ch_total;n++) {
    //    logi("ch[%d] = %d", n,fm_mode.ch[n]);
    //}

#endif

    random_num = R4BYTE(&ibuf[INFO_RAND_NUM]);
    ble_random_num = R4BYTE(&ibuf[INFO_BLE_RAND_NUM]);
    record_bitrate_index = R1BYTE(&ibuf[INFO_RECORD_INDEX]);
}

AT(.info_init_seg)
void sys_setting_default(uint8_t *ibuf)
{
    logi("sys setting default.");

    np.volume = VOLUME_DEFAULT;
    W1BYTE(&ibuf[INFO_VOLUME]) = VOLUME_DEFAULT;

#if BT_PHONE_EN
    np.btcall_volume = VOLUME_DEFAULT;
    W1BYTE(&ibuf[INFO_BTCALL_VOLUME]) = VOLUME_DEFAULT;
#endif

#if (MODE_MUSIC_EN && MUSIC_INFO_SAVE_MODE == 2)
    np.file_num = FILE_NUM_DEFAULT;
    np.ptime.ms = FILE_MS_DEFAULT;
    W4BYTE(&ibuf[INFO_SDCARD_NUM]) = FILE_NUM_DEFAULT;
    W4BYTE(&ibuf[INFO_SDCARD_TIME]) = FILE_MS_DEFAULT;
#endif

#if MODE_FM_EN
    fm_mode.freq = FM_FREQ_DEFAULT + FM_FREQ_MIN;
    fm_mode.ch_total = FM_TOTAL_DEFAULT;
    fm_mode.ch_cur = FM_CH_CUR_DEFAULT;

    W1BYTE(&ibuf[INFO_FM_FREQ]) = FM_FREQ_DEFAULT;
    W1BYTE(&ibuf[INFO_FM_TOTAL]) = FM_TOTAL_DEFAULT;
    W1BYTE(&ibuf[INFO_FM_TOTAL]) = FM_CH_CUR_DEFAULT;
#endif

#if BT_ADDR_USE_RANDOM
    random_num = trng_get_random_data();
    W4BYTE(&ibuf[INFO_RAND_NUM]) = random_num;
#endif

#if BLE_ADDR_USE_RANDOM
    ble_random_num = trng_get_random_data();
    W4BYTE(&ibuf[INFO_BLE_RAND_NUM]) = ble_random_num;
#endif
    W1BYTE(&ibuf[INFO_RECORD_INDEX]) = REC_BIT_512KPBS;
    record_bitrate_index = REC_BIT_512KPBS;
	//Ð´ÈëÊý¾Ý
	//user_id
}
