#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "dio.h"

AT(.dio_flash_seg)
void dio_set_func(uint32_t id, uint32_t func)
{
    uint32_t reg_val;
    reg_val = readl(DIO_CTRL_DAT_REG);
    reg_val &= ~(DIO_FUNC_MASK << id);
    reg_val |= (func << id);
    writel(reg_val, DIO_CTRL_DAT_REG);
}

AT(.dio_flash_seg)
void dio_set_driving(uint32_t id, uint32_t driving)
{
    uint32_t reg_val;
    reg_val = readl(DIO_CTRL_DAT_REG);
    reg_val &= ~(DIO_DRI_MASK<<id);
    reg_val |= (driving << id);
    writel(reg_val, DIO_CTRL_DAT_REG);
}

AT(.dio_flash_seg)
void dio_set_pull(uint32_t id, uint32_t pull)
{
    uint32_t reg_val;
    reg_val = readl(DIO_CTRL_DAT_REG);
    reg_val &= ~(DIO_PULL_MASK<<id);
    reg_val |= (pull<<id);
    writel(reg_val, DIO_CTRL_DAT_REG);
}

AT(.dio_flash_seg)
void dio_set_value(uint32_t id, uint32_t value)
{
    uint32_t reg_val;
    reg_val = readl(DIO_CTRL_DAT_REG);
    reg_val &= ~(DIO_VALUE_OUTPUT_MASK<<id);
    reg_val |= (value << id);
    writel(reg_val, DIO_CTRL_DAT_REG);
}

AT(.dio_flash_seg)
uint32_t dio_get_value(uint32_t id)
{
    uint32_t reg_val;
    reg_val = readl(DIO_CTRL_DAT_REG);
    reg_val &= (DIO_VALUE_INPUT_MASK<<id);
    return (reg_val ? 1 : 0);
}

void dio_test(void)
{
    logi("dio_test");
    dio_set_func(DIO_ID0, DIO_FUNC_INPUT);
    dio_set_func(DIO_ID1, DIO_FUNC_INPUT);
    dio_set_pull(DIO_ID0, DIO_PULL_UP_100K);
    dio_set_pull(DIO_ID1, DIO_PULL_UP_100K);
    mdelay(10);
    logi("reg: %08x",*(uint32_t *)0x40000108);
    logi("dio0:%x",dio_get_value(DIO_ID0));
    logi("dio1:%x",dio_get_value(DIO_ID1));
    while(1);
}

