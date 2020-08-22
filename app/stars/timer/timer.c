#define LOG_LEV 5
#include "sys_inc.h"
#include "user_config.h"
#include "hal_power_lib.h"
#include "key.h"
#include "motor.h"
#include "timer.h"

volatile uint32_t cnt_t0;

//auto_shutdown_t *auto_shutdown_context;
//bool shutdown_ing = false;

AT(.timer_seg)
void timer0_cb(void)
{
    key_scan();

    cnt_t0++;
    if (cnt_t0 % 200 == 0) {
#if WATCHDOG_EN
        watchdog_refresh(NULL);
#endif
    }
    if (cnt_t0 % 100 == 0) {
#if MOTOR_EN
        motor_timer_cb();
#endif
    }
}

AT(.timer_seg)
void timer1_cb(void)
{
	logi("%s: %u.", __func__, timestamp_to_ms(get_timestamp()));
}

AT(.timer_seg)
void timer2_cb(void)
{
    logi("%s: %u.", __func__, timestamp_to_ms(get_timestamp()));
}

AT(.timer_seg)
void timer3_cb(void)
{
    logi("%s: %u.", __func__, timestamp_to_ms(get_timestamp()));
}

AT(.timer_init_seg)
void timer0_init(void)
{
#if (TIMER0_EN || MOTOR_EN || WATCHDOG_EN || KEY_IO_EN || KEY_IR_EN || \
    (KADC0_EN && !KADC0_IRQ) || (KADC1_EN && !KADC1_IRQ) || (KADC2_EN && !KADC2_IRQ) || \
    (KADC3_EN && !KADC3_IRQ) || (KADC4_EN && !KADC4_IRQ) || (KADC5_EN && !KADC5_IRQ))
    timer_param_t init_para;

    init_para.int_priority  = IRQn_TIMER0_PRIO;
    init_para.period_in_us  = TIMER0_PERIOD;
    init_para.repeat        = 1;
    init_para.callback      = timer0_cb;
    if (pdPASS == timer_init(TIMER0, &init_para)) {
        timer_start(TIMER0);
    } else {
        logw("timer0 init fail");
    }

#if WATCHDOG_EN
    watchdog_init(WATCHDOG_TIMEOUT);
#endif

#endif
}

AT(.timer_init_seg)
void timer1_init(void)
{
#if TIMER1_EN
    timer_param_t init_para;

    init_para.int_priority  = IRQn_TIMER1_PRIO;
    init_para.period_in_us  = TIMER1_PERIOD;
    init_para.repeat        = 1; //单次模式
    init_para.callback      = timer1_cb;
	if (pdPASS == timer_init(TIMER1, &init_para)) {
        timer_start(TIMER1);
    } else {
        logw("TIMER1 init fail");
    }
#endif
}

//AT(.timer_seg)
//void auto_shutdown(bool need)
//{
//#if TIMER1_EN
//    if(!shutdown_ing) {
//		logd("arunboy status = %d, set status = %d",auto_shutdown_context->auto_shutdown_status,need);
//		if(auto_shutdown_context->auto_shutdown_status != need){
//		    if(need){
//				timer_deinit(TIMER1);
//				timer1_init();
//				timer_start(TIMER1);
//			} else {
//				timer_stop(TIMER1);
//			}
//			auto_shutdown_context->auto_shutdown_status = need;
//		}
//    }
//#endif
//}


AT(.timer_init_seg)
void timer2_init(void)
{
#if TIMER2_EN
    logi("timer2 init.");
    timer_param_t init_para;

    init_para.int_priority  = IRQn_TIMER2_PRIO;
    init_para.period_in_us  = TIMER2_PERIOD;
    init_para.repeat        = 1;
    init_para.callback      = timer2_cb;
    if (pdPASS == timer_init(TIMER2, &init_para)){
        timer_start(TIMER2);
    } else {
        logw("timer2 init fail");
    }
#endif
}

AT(.timer_init_seg)
void timer3_init(void)
{
#if TIMER1_EN
    logi("timer3 init.");
    timer_param_t init_para;

    init_para.int_priority  = IRQn_TIMER3_PRIO;
    init_para.period_in_us  = TIMER3_PERIOD;
    init_para.repeat        = 1;
    init_para.callback      = timer3_cb;
    if (pdPASS == timer_init(TIMER3, &init_para)){
        timer_start(TIMER3);
    } else {
        logw("timer3 init fail");
    }
#endif
}


#define TIMER_TEST_EN 0

#if TIMER_TEST_EN
AT(.timer_init_seg)
void timer_test(void)
{
    logi("timer_test.");
    timer_param_t init_para;

#if 1
    //timer0 test
    init_para.int_priority  = IRQn_TIMER0_PRIO;
    init_para.period_in_us  = 10000; // 10ms
    init_para.repeat        = 1;
    init_para.callback      = timer0_cb;
    if(pdPASS == timer_init(TIMER0, &init_para)){
        timer_start(TIMER0);
    }else{
        logw("timer0 init fail");
    }
#endif
    //timer1 test
    init_para.int_priority  = IRQn_TIMER1_PRIO;
    init_para.period_in_us  = 20000;// 20ms
    init_para.repeat        = 1;
    init_para.callback      = timer1_cb;
    if(pdPASS == timer_init(TIMER1, &init_para)){
        timer_start(TIMER1);
    }else{
        logw("timer1 init fail");
    }
#if 1
    //timer2 test
    init_para.int_priority  = IRQn_TIMER2_PRIO;
    init_para.period_in_us  = 3000000;// 3s
    init_para.repeat        = 0;
    init_para.callback      = timer2_cb;
    if(pdPASS == timer_init(TIMER2, &init_para)){
        timer_start(TIMER2);
    }else{
        logw("timer2 init fail");
    }

    //timer3 test
    init_para.int_priority  = IRQn_TIMER3_PRIO;
    init_para.period_in_us  = 4000000;// 4s
    init_para.repeat        = 1;
    init_para.callback      = timer3_cb;
    if(pdPASS == timer_init(TIMER3, &init_para)){
        timer_start(TIMER3);
    }else{
        logw("timer3 init fail");
    }
#endif
}
#endif

