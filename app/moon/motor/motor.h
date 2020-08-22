#ifndef _MOTOR_H_
#define _MOTOR_H_

#if MOTOR_EN
void motor_init(void);
void motor_callin(bool callin);
void motor_timer_cb(void);
#else
#define motor_init()
#define motor_callin(callin)
#define motor_timer_cb()
#endif

#endif

