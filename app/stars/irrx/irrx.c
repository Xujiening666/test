#include "sys_inc.h"
#include "user_config.h"
#include "irrx.h"


AT(.irrx_init_seg)
void irrx_init(void)
{
    module_ccu_enable(CCU_IR);
#if 0
    //clk_set_src(CLK_IR, CLK_32K_HOSC);
#else
    clk_set_src(CLK_IR, CLK_LOSC);      //the actual configuration is CLK_32K_HOSC
#endif
    clk_enable(CLK_IR);

    pin_config(IRRX_PIN_CFG);

    IRRX_InitTypeDef irrx_param;
    irrx_param.logic_threshold = 0x50;
    irrx_param.noise_threshold = 0x4;
    irrx_param.rppi = 1;
    irrx_param.wk_t9ms_max = 0xFF;
    irrx_param.wk_t9ms_min = 0x10;
    irrx_param.t9ms_max = 0x5A;
    irrx_param.t9ms_min = 0x3C;
    irrx_param.t2d25ms_max = 0x58;
    irrx_param.t2d25ms_min = 0x40;
    irrx_param.t4d5ms_max = 0x5A;
    irrx_param.t4d5ms_min = 0x3C;
    irrx_param.t1d69ms_max = 0x3D;
    irrx_param.t1d69ms_min = 0x28;
    irrx_param.t0d56ms_max = 0x20;
    irrx_param.t0d56ms_min = 0x08;

    irrx_module_init(&irrx_param);
}

