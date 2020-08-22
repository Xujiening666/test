#ifndef _MODE_CLOCK_H_
#define _MODE_CLOCK_H_


#include "rtc.h"


typedef struct {
    uint8_t sub_status;
    uint8_t tone_status;
} clock_mode_t;
extern clock_mode_t clock_mode;

void mode_clock(void);
void mode_clock_enter(void);
void mode_clock_loop(void);
void mode_clock_exit(void);
void clock_event_process(uint32_t msg);
void clock_alarm_play_tone(void);

#endif

