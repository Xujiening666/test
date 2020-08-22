#ifndef _MODE_LINEIN_H_
#define _MODE_LINEIN_H_


#define LINE_OUT_DET_CNT        20
#define LINE_IN_DET_CNT         20

extern bool linein_muted;

void mode_linein(void);
void mode_linein_enter(void);
void mode_linein_loop(void);
void mode_linein_exit(void);
void linein_event_process(uint32_t msg);

void adda_init(uint8_t mode);
void adda_pp(uint8_t mode);
void adda_deinit(uint32_t mode);

void linein_mode_scene_save(void);
void linein_mode_scene_recovery(void);

void linein_detect_init(void);
void linein_detect(void);
bool linein_detect_state(void);

#endif

