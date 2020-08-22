#ifndef _MODE_MUSIC_H_
#define _MODE_MUSIC_H_

extern TaskHandle_t track_handler;
extern TaskHandle_t karaoke_handler;
extern TaskHandle_t dec_handler;
extern TaskHandle_t bt_isr_bh_handler;
extern TaskHandle_t bta_handler;
extern TaskHandle_t btc_handler;
extern TaskHandle_t bth_handler;
extern TaskHandle_t main_task_handler;
extern TaskHandle_t linein_handler;

int file_scan(bool scan);
void music_event_process(uint32_t msg);
void mode_music(void);
void mode_music_enter(void);
void mode_music_loop(void);
void mode_music_exit(void);


#endif
