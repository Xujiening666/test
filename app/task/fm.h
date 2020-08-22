#ifndef _FM_H_
#define _FM_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "fm_api.h"


typedef struct {
    uint8_t id;
    uint8_t ch_cur;
    uint8_t ch_total;
    uint8_t ch[FM_CH_MAX];
    uint16_t freq;
} fm_mode_t;
extern fm_mode_t fm_mode;


void mode_fm(void);
void mode_fm_enter(void);
void mode_fm_loop(void);
void mode_fm_exit(void);

void fm_event_process(uint32_t msg);

void fm_audio_init(void);
void fm_audio_deinit(void);
void fm_mode_scene_save(void);
void fm_mode_scene_recovery(void);

#ifdef __cplusplus
}
#endif

#endif

