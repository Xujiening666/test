#ifndef __MODE_CHARGE_H__
#define __MODE_CHARGE_H__

void restart(void);
void mode_charge(void);
void mode_charge_enter(void);
void mode_charge_loop(void);
void mode_charge_exit(void);

void charge_event_process(uint32_t msg);

#endif
