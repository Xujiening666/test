#ifndef _HAL_TIMER_LIB_H_
#define _HAL_TIMER_LIB_H_

typedef enum
{
    TIMER0  = 0x00U,
    TIMER1  = 0x01U,
    TIMER2  = 0x02U,
    TIMER3  = 0x03U
} HAL_TimerId;

typedef struct {
    uint32_t int_priority;  //timer interupt priority
    uint32_t period_in_us;  //timer period in us
    bool repeat;            // 1: repeat mode, 0: single mode
    void (* callback)(void);//timer callback
} timer_param_t;

/**
  * @brief  Initializes the timer specified by timer_id
  * @param  timer_id: timer_id
                  init_para:
  * @retval pdPASS if success, pdFAIL if fail.
  */
uint8_t timer_init(HAL_TimerId timer_id, timer_param_t *init_para);

/**
  * @brief  De-initializes the timer specified by timer_id
  * @param  timer_id: timer_id
  * @retval none
  * note: before a timer can be re-initialized and reused, timer_deinit should firstly called, or timer_init will fail
  */
void timer_deinit(HAL_TimerId timer_id);

/**
  * @brief  start the timer specified by timer_id
  * @param  timer_id: timer_id
  * @retval none
  * note: before a timer can be started, timer_init should firstly called and success
  */
void timer_start(HAL_TimerId timer_id);

/**
  * @brief  stop the timer specified by timer_id
  * @param  timer_id: timer_id
  * @retval none
  */
void timer_stop(HAL_TimerId timer_id);

#endif

