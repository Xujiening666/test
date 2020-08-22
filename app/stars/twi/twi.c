#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "twi.h"
#include "device.h"


AT(.twi_init_seg)
void twi_init(void)
{
#if TWI1_EN
    module_ccu_enable(CCU_TWI1);
    clk_enable(CLK_TWI1);

    pin_config(TWI1_SCL_PIN_CFG);
    pin_config(TWI1_SDA_PIN_CFG);

    twi_module_init(TWI_ID_01, TWI1_CLK_FREQ);
#endif

#if TWI2_EN
    module_ccu_enable(CCU_TWI2);
    clk_enable(CLK_TWI2);

    pin_config(TWI2_SCL_PIN_CFG);
    pin_config(TWI2_SDA_PIN_CFG);

    twi_module_init(TWI_ID_02, TWI2_CLK_FREQ);
#endif
}

AT(.twi_init_seg)
void twi_deinit(void)
{
#if TWI1_EN
    twi_module_deinit(TWI_ID_01);
    clk_disable(CLK_TWI1);
    module_ccu_disable(CCU_TWI1);
#endif

#if TWI2_EN
    twi_module_deinit(TWI_ID_02);
    clk_disable(CLK_TWI2);
    module_ccu_disable(CCU_TWI2);
#endif

#if (TWI1_SD_SHARE_EN || TWI2_SD_SHARE_EN)
    pin_config(SD_D0_PIN_CFG);
    pin_config(SD_CMD_PIN_CFG);
#endif
}

AT(.twi_init_seg)
int twi_write(uint32_t id, uint32_t addr, const uint8_t *data, uint32_t bytes)
{
    int ret;

#if TWI2_DM_SHARE_EN
    usb_detect_set(USB_DISABLE);
    pin_set_func(TWI2_SCL_PIN_NUM, TWI2_SCL_PIN_FUNC);
    pin_set_pull(TWI2_SCL_PIN_NUM, TWI2_SCL_PIN_PULL);
#endif

    ret = twi_module_write(id, addr, data, bytes);

#if TWI2_DM_SHARE_EN
    usb_detect_set(USB_DETECT);
#endif

    return ret;
}

AT(.twi_init_seg)
int twi_read(uint32_t id, uint32_t addr, uint8_t *data, uint32_t bytes)
{
    int ret;

#if TWI2_DM_SHARE_EN
    usb_detect_set(USB_DISABLE);
    pin_set_func(TWI2_SCL_PIN_NUM, TWI2_SCL_PIN_FUNC);
    pin_set_pull(TWI2_SCL_PIN_NUM, TWI2_SCL_PIN_PULL);
#endif

    ret = twi_module_read(id, addr, data, bytes);

#if TWI2_DM_SHARE_EN
    usb_detect_set(USB_DETECT);
#endif

    return ret;
}


/***************************************************************************
                 use gpio software mode
***************************************************************************/
AT(.twi_init_seg)
void twi_delay(void)
{
    udelay(500);
}

AT(.twi_init_seg)
void twi_io_init(void)
{
    TWI_SCL_INIT();
    TWI_SDA_INIT();

    TWI_SCL_VAL_HIGH();
    TWI_SDA_VAL_HIGH();
    TWI_SCL_FUNC_OUT();
    TWI_SDA_FUNC_OUT();
}

AT(.twi_init_seg)
void twi_io_deinit(void)
{

}

AT(.twi_init_seg)
void twi_start(void)
{
    twi_io_init();
    twi_delay();

    TWI_SDA_VAL_LOW();
    twi_delay();

    TWI_SCL_VAL_LOW();
    twi_delay();
}

AT(.twi_init_seg)
void twi_stop(void)
{
    TWI_SDA_VAL_LOW();
    TWI_SDA_FUNC_OUT();
    twi_delay();

    TWI_SCL_VAL_HIGH();
    twi_delay();

    TWI_SDA_VAL_HIGH();
    twi_delay();
}

AT(.twi_init_seg)
void twi_write_byte(uint8_t data)
{
    TWI_SDA_FUNC_OUT();
    twi_delay();
    for(uint32_t i=0;i<8;i++) {
        if (data & 0x80) {
            TWI_SDA_VAL_HIGH();
        } else {
            TWI_SDA_VAL_LOW();
        }
        twi_delay();
        TWI_SCL_VAL_HIGH();
        twi_delay();
        data <<= 1;
        TWI_SCL_VAL_LOW();
    }
}

AT(.twi_init_seg)
uint8_t twi_read_byte(void)
{
    uint8_t val = 0;

    TWI_SDA_FUNC_IN();
    twi_delay();
    for(uint32_t i=0;i<8;i++) {
        TWI_SCL_VAL_HIGH();
        twi_delay();
        val <<= 1;
        if (TWI_SDA_VAL_GET()) {
            val |= 0x01;
        }
        TWI_SCL_VAL_LOW();
        twi_delay();
    }
    return val;
}

AT(.twi_init_seg)
void twi_send_ack(void)
{
    TWI_SDA_VAL_LOW();
    TWI_SDA_FUNC_OUT();
    TWI_SCL_VAL_LOW();
    twi_delay();

    TWI_SCL_VAL_HIGH();
    twi_delay();

    TWI_SCL_VAL_LOW();
}

AT(.twi_init_seg)
void twi_send_nack(void)
{
    TWI_SDA_VAL_HIGH();
    TWI_SDA_FUNC_OUT();
    TWI_SCL_VAL_LOW();
    twi_delay();

    TWI_SCL_VAL_HIGH();
    twi_delay();

    TWI_SCL_VAL_LOW();
}

AT(.twi_init_seg)
uint8_t twi_get_ack(void)
{
    uint8_t val;

    TWI_SDA_FUNC_IN();
    twi_delay();

    TWI_SCL_VAL_HIGH();
    twi_delay();

    val = TWI_SDA_VAL_GET();

    TWI_SCL_VAL_LOW();
    twi_delay();

    return val;
}

#define EEPROM_TWI_ID           TWI_ID_02
#define EEPROM_TWI_ADDR         (0xA0>>1)

void eeprom_write(uint8_t adr, uint8_t *pdata, uint8_t bytes)
{
#if 1
    //硬件模块方式
    uint8_t data[bytes+1];  //bytes <= 8

    data[0] = adr;
    for (uint8_t i=0;i<bytes;i++) {
        data[1+i] = *(pdata+i);
    }

    twi_write(EEPROM_TWI_ID, EEPROM_TWI_ADDR, data, bytes+1);
#else
    //gpio软件模拟方式
    twi_start();

    twi_write_byte(0xa0);
    twi_get_ack();

    twi_write_byte(adr);
    twi_get_ack();

    for (uint32_t i=0;i<bytes;i++) {
        twi_write_byte(*pdata++);
        twi_get_ack();
    }

    twi_stop();
#endif
}

uint8_t eeprom_read(uint8_t adr)
{
#if 1
    //硬件模块方式
    uint8_t value;

    twi_write(EEPROM_TWI_ID, EEPROM_TWI_ADDR, &adr, 1);
    twi_read(EEPROM_TWI_ID, EEPROM_TWI_ADDR, &value, 1);

    return value;
#else
    //gpio软件模拟方式
    uint8_t value;

    twi_start();

    twi_write_byte(0xa0);
    twi_get_ack();

    twi_write_byte(adr);
    twi_get_ack();

    twi_start();
    twi_write_byte(0xa1);
    twi_get_ack();

    value = twi_read_byte();
    twi_send_nack();
    twi_stop();

    return value;
#endif
}

void eeprom_test(void)
{
    logi("%s", __func__);

    uint8_t data[3];

    data[0] = 0xa0;
    data[1] = 0xa1;
    data[2] = 0xa2;
    eeprom_write(0x00, data, 3);
    mdelay(10);
    logi("eeprom: %x,%x,%x", eeprom_read(0x00),eeprom_read(0x01),eeprom_read(0x02));
}


#if 0
//example:

void rda5807_read(uint8_t *data, uint8_t size)
{
    uint32_t i = 0;

    twi_start();
    twi_write_byte(0x21);
    twi_get_ack();

    for (i=0;i<size-1;i++) {
        data[i] = twi_read_byte();
        twi_send_ack();
    }

    data[i] = twi_read_byte();
    twi_send_nack();

    twi_stop();
}

void rda5807_write(const uint8_t *data, uint8_t size)
{
    twi_start();

    twi_write_byte(0x20);
    twi_get_ack();

    for (uint32_t i=0;i<size;i++) {
        twi_write_byte(data[i]);
        twi_get_ack();
    }

    twi_stop();
}
#endif

