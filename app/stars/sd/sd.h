#ifndef _SD_H_
#define _SD_H_

#include "diskio.h"


#if (SD_D0_PIN_SEL == 0)
#define SD_D0_PIN_CFG       (PIN_B0 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (SD_D0_PIN_SEL == 1)
#define SD_D0_PIN_CFG       (PIN_B3 | PIN_FUNC6 | PIN_DRI2 | PIN_PULL_UP2K)
#elif (SD_D0_PIN_SEL == 2)
#define SD_D0_PIN_CFG       (PIN_A17 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (SD_D0_PIN_SEL == 3)
#define SD_D0_PIN_CFG       (PIN_B15 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (SD_D0_PIN_SEL == 4)
#define SD_D0_PIN_CFG       (PIN_B21 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP2K)
#endif

#if (SD_CLK_PIN_SEL == 0)
#define SD_CLK_PIN_CFG      (PIN_B1 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP100K)
#define SD_CLK_PIN_NUM      (PIN_B1)
#define SD_CLK_FUNC         (PIN_FUNC2)
#elif (SD_CLK_PIN_SEL == 1)
#define SD_CLK_PIN_CFG      (PIN_B6 | PIN_FUNC4 | PIN_DRI1 | PIN_PULL_UP100K)
#define SD_CLK_PIN_NUM      (PIN_B6)
#define SD_CLK_FUNC         (PIN_FUNC4)
#elif (SD_CLK_PIN_SEL == 2)
#define SD_CLK_PIN_CFG      (PIN_A15 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP100K)
#define SD_CLK_PIN_NUM      (PIN_A15)
#define SD_CLK_FUNC         (PIN_FUNC2)
#elif (SD_CLK_PIN_SEL == 3)
#define SD_CLK_PIN_CFG      (PIN_B13 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP100K)
#define SD_CLK_PIN_NUM      (PIN_B13)
#define SD_CLK_FUNC         (PIN_FUNC2)
#elif (SD_CLK_PIN_SEL == 4)
#define SD_CLK_PIN_CFG      (PIN_B19 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP100K)
#define SD_CLK_PIN_NUM      (PIN_B19)
#define SD_CLK_FUNC         (PIN_FUNC2)
#endif

#define SD_CLK_PULL         (PIN_PULL_DISABLE)
#define SD_WKUP_FUNC        (PIN_FUNC14)
#define SD_DETECT_FUNC      (PIN_FUNCINPUT)
#define SD_DETECT_PULL      (PIN_PULL_UP100K)
#define SD_DETECT_PIN_NUM   (SD_CLK_PIN_NUM)

#if (SD_CMD_PIN_SEL == 0)
#define SD_CMD_PIN_CFG      (PIN_B2 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (SD_CMD_PIN_SEL == 1)
#define SD_CMD_PIN_CFG      (PIN_B4 | PIN_FUNC6 | PIN_DRI2 | PIN_PULL_UP2K)
#elif (SD_CMD_PIN_SEL == 2)
#define SD_CMD_PIN_CFG      (PIN_A16 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (SD_CMD_PIN_SEL == 3)
#define SD_CMD_PIN_CFG      (PIN_B14 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (SD_CMD_PIN_SEL == 4)
#define SD_CMD_PIN_CFG      (PIN_B20 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP2K)
#endif


typedef struct {
    uint8_t sta;
    uint8_t out_cnt;
    uint8_t in_cnt;
} sd_det_t;
extern sd_det_t sd_det;

#define SD_IN_DET_CNT       10  //20ms * 10 = 200ms
#define SD_OUT_DET_CNT      3   //20ms *3 = 60ms


void sd_init(void);
void sd_pin_init(void);
void sd_clk_init(void);
void sd_clk_deinit(void);
void sd_detect_set(uint8_t sta);
void sd_detect(void);
DRESULT sd_get_status(void);
DRESULT sd_failcb(void);
void sd_set_wakeup(bool enable);
void sd_irq_process(void);
uint8_t sd_check_sn(void);


#endif

