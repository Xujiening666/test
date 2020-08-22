#ifndef _MODE_SLEEP_H_
#define _MODE_SLEEP_H_


void sleep_wakeup_enable(void);
void sleep_wakeup_disable(void);
void sleep_timer_cb(xTimerHandle xTimer);
#if AUTO_SHUNTDOWN
void auto_shutdown_cb(xTimerHandle xTimer);
void auto_shutdown(bool need);
#endif
#endif

