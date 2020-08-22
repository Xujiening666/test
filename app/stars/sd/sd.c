#define LOG_LEV 4
#define SLTK_LOG_TAG "sd"
#include "sys_inc.h"
#include "user_config.h"
#include "sd.h"
#include "gpio.h"
#include "event.h"
#include "device.h"
#include "sdmmc_io.h"


#if SD_EN

sd_det_t sd_det;


AT(.sd_init_seg)
void sd_init(void)
{
#if (!SD_DM_SHARE_EN)
    sd_clk_init();
    sd_pin_init();
#endif
    pin_config(SD_CLK_PIN_CFG);

#if SD_DETECT_EN
    sd_detect_set(1);
    sd_det.sta = 0xFF;

    pin_irq_clk_config(SD_CLK_PIN_NUM, (0xFF<<8) | (0x00<<7) | (0x01<<4));
    //[15:8]-debounce clock cycles; [7]-interrupt clock select; [6:4]-debounce clock pre-scale n;
#endif

    INTC_SetVector(IRQn_SDMMC, (uint32_t)sd_irq_process);
    INTC_SetPriority(IRQn_SDMMC, IRQn_SDMMC_PRIO);
    INTC_EnableIRQ(IRQn_SDMMC);
}

AT(.sd_init_seg)
void sd_pin_init(void)
{
    pin_config(SD_D0_PIN_CFG);
    pin_config(SD_CMD_PIN_CFG);
}

AT(.sd_init_seg)
void sd_clk_init(void)
{
#if (SYS_PLATFORM == PLATFORM_FPGA)
    /*on fpga,clock source select:
        0x0: HOSC-24MHz/64=375KHz
        0x1: CorePLL-CLK96M-24MHz/2=12MHz      */
    clk_set_src(CLK_SDMMC, CLK_96M);
#else
    clk_set_src(CLK_SDMMC, CLK_24M_HOSC);
#endif

    module_ccu_enable(CCU_SDMMC);
    clk_enable(CLK_SDMMC);
}

AT(.sd_init_seg)
void sd_clk_deinit(void)
{
    clk_disable(CLK_SDMMC);
    module_ccu_disable(CCU_SDMMC);
}


AT(.sd_sram_seg)
void sd_detect(void)
{
    if (sd_det.sta) {
        if (pin_get_value(SD_DETECT_PIN_NUM)) {
            sd_det.in_cnt = 0;
            if (sd_det.out_cnt < SD_OUT_DET_CNT) {
                sd_det.out_cnt++;
                if (sd_det.out_cnt == SD_OUT_DET_CNT) {
                    sd_det.out_cnt = SD_OUT_DET_CNT + 1;

                    if (dev.sdcard_sta != DEV_OFFLINE) {
                        dev.sdcard_sta = DEV_OFFLINE;
                        logi("sd out.");
                        if (dev.startup) {
                            event_put(DEVICE_EVENT_SD_OUT);
                        }
#if SD_DM_SHARE_EN
                        usb_dm_pin_set(1);
#endif
                    }
                }
            }
        } else {
            sd_det.out_cnt = 0;
            if (sd_det.in_cnt < SD_IN_DET_CNT) {
                sd_det.in_cnt++;
                if (sd_det.in_cnt == SD_IN_DET_CNT) {
                    sd_det.in_cnt = SD_IN_DET_CNT + 1;
                    if (dev.sdcard_sta == DEV_OFFLINE || dev.sdcard_sta == DEV_FAIL) {
                        dev.sdcard_sta = DEV_UNINITIALIZED;
                        logi("sd in.");
                        if (dev.startup) {
                            event_put(DEVICE_EVENT_SD_IN);
                        }
#if SD_DM_SHARE_EN
                        pin_irq_config(USB_DM_PIN_NUM, PIN_IRQ_DIS);
#endif
                    }
                }
            }
        }
    } else {
        sd_det.in_cnt = 0;
        sd_det.out_cnt = 0;
    }
}

AT(.sd_sram_seg)
void sd_set_wakeup(bool enable)
{
    if (!module_is_enable(CCU_SDMMC)) {
        return;
    }

    if (enable) {
        sd_detect_set(2);
        pin_irq_config(SD_DETECT_PIN_NUM, PIN_IRQ_EN | PIN_IRQ_RF_EDGE);
        pin_set_wakeup(SD_DETECT_PIN_NUM, true);
    } else {
        pin_irq_config(SD_DETECT_PIN_NUM, PIN_IRQ_DIS);
        pin_set_wakeup(SD_DETECT_PIN_NUM, false);
    }
}

AT(.sd_init_seg)
uint32_t sd_clkdiv_set(void)
{
    return SD_CLKDIV;
}

AT(.sd_sram_seg)
void sd_detect_set(uint8_t sta)
{
#if SD_DETECT_EN
    if (sd_det.sta == sta) {
        return;
    }

    if (sta == 0) {
        sd_det.sta = sta;
        pin_set_pull(SD_CLK_PIN_NUM, SD_CLK_PULL);
        pin_set_func(SD_CLK_PIN_NUM, SD_CLK_FUNC);
    } else if (sta == 1) {
        pin_set_pull(SD_CLK_PIN_NUM, SD_DETECT_PULL);
        pin_set_func(SD_CLK_PIN_NUM, SD_DETECT_FUNC);
        udelay(200);
    } else if (sta == 2) {
        pin_set_pull(SD_CLK_PIN_NUM, SD_DETECT_PULL);
        pin_set_func(SD_CLK_PIN_NUM, SD_WKUP_FUNC);
        udelay(200);
    }
    sd_det.sta = sta;
#endif
}

AT(.sd_sram_seg)
DRESULT sd_get_status(void)
{
#if SD_EN
    if (dev.sdcard_sta != DEV_OK) {
        return RES_ERROR;
    }

    return RES_OK;

#else
    return RES_OK;
#endif
}

AT(.sd_sram_seg)
DRESULT sd_failcb(void)
{
#if SD_EN
    if (dev.sdcard_sta == DEV_OK) {
        event_force_put(DEVICE_EVENT_SD_FAIL);
        dev.sdcard_sta = DEV_FAIL;
    }
#endif

    return 0;
}

#else
void sd_detect_set(uint8_t sta)
{
}

DRESULT sd_get_status(void)
{
    return RES_ERROR;
}

DRESULT sd_failcb(void)
{
    return 0;
}

DRESULT sd_disk_read_start(BYTE *buff, DWORD sector)
{
    return 0;
}

DRESULT sd_disk_read2(BYTE *buff, DWORD sector, UINT count)
{
    return 0;
}

uint8_t wait_sd_ready(uint32_t timeout)
{
    return 0;
}

DRESULT sd_disk_ioctl(BYTE cmd, void *buff)
{
    return 0;
}

DRESULT sd_disk_write(BYTE *buff, DWORD sector, UINT count)
{
    return 0;
}

#endif

