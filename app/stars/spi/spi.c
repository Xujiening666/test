#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "norflash.h"
#include "spi.h"


#if (SPI0_EN || SPI1_EN)

extern spi_param_t spi_param;
volatile uint8_t spi_lock;


AT(.spi_init_seg)
void spi_init(void)
{
#if SPI0_EN
    pin_config(SPI0_CS_PIN_CFG);
    pin_config(SPI0_CLK_PIN_CFG);
    pin_config(SPI0_MOSI_PIN_CFG);
    pin_config(SPI0_MISO_PIN_CFG);
    norflash_chip1_mapping(0x1);             //MISO select PB2
#endif

#if SPI1_EN
#if SPI1_MODE_SEL
    module_ccu_enable(CCU_SPI);
	pin_config(SPI1_RST_PIN_CFG);
	pin_config(SPI1_LCD_PIN_CFG);
	pin_config(SPI1_LCD_DC_PIN_CFG);
    SPI1_CS_HIGH();
    SPI1_CS_INIT();
    pin_config(SPI1_CLK_PIN_CFG);
    pin_config(SPI1_MOSI_PIN_CFG);	
#if (SPI1_DIRECT_SEL == 0)
    pin_config(SPI1_MISO_PIN_CFG);
#endif

    spi_param.datasize = SPI_DATASIZE_8BIT;
    spi_param.casize = SPI_CASIZE_0BIT;
#if SPI1_DIRECT_SEL
    spi_param.clkdiv = SPI_CLOCKDIV_8;
    spi_param.direct = SPI_1LINE_BIDIRECTIONAL;
#else
    spi_param.clkdiv = SPI_CLOCKDIV_4;
    spi_param.direct = SPI_2LINE_UNIDIRECTIONAL;
#endif
    spi_param.first = SPI_FIRST_MSB;
    spi_param.cpha = SPI_PHASE_0;
    spi_param.cpol = SPI_POLARITY_LOW;

    spi_module_init();

#else
	pin_config(SPI1_RST_PIN_CFG);
	pin_config(SPI1_LCD_PIN_CFG);
	pin_config(SPI1_LCD_DC_PIN_CFG);
    SPI1_CS_HIGH();
    SPI1_CLK_LOW();
    SPI1_MOSI_LOW();

    SPI1_CS_INIT();
    SPI1_CLK_INIT();
    SPI1_MOSI_INIT();
#if SPI1_DIRECT_SEL
    SPI1_DAT_RD();              //3线默认配置为读(输入)，在写之前需要配置为输出，写完后还原为输入
#else
    SPI1_MISO_INIT();
#endif
#endif
#endif

    //spi_test();
}

AT(.spi_seg)
void spi_delay(void)
{
    //udelay(1);
}

AT(.spi_seg)
void spi_write(uint32_t data)
{
#if SPI1_EN
#if SPI1_MODE_SEL
    spi_module_write(data);

#else
#if SPI1_DIRECT_SEL
    SPI1_DAT_WR();
#endif
    SPI1_CLK_LOW();
    for (uint8_t i=0;i<8;i++) {
        if (data & 0x80) {
            SPI1_MOSI_HIGH();
        } else {
            SPI1_MOSI_LOW();
        }
        spi_delay();
        SPI1_CLK_HIGH();
        spi_delay();
        SPI1_CLK_LOW();
        data <<= 1;
    }
#if SPI1_DIRECT_SEL
    SPI1_DAT_RD();              //默认配置为读(输入)
#endif
#endif
#endif
}

AT(.spi_seg)
void spi_write_9bit(uint32_t data)
{
#if SPI1_EN
#if SPI1_MODE_SEL
    spi_module_write(data);

#else
#if SPI1_DIRECT_SEL
    SPI1_DAT_WR();
#endif
    SPI1_CLK_LOW();
    for (uint8_t i=0;i<9;i++) {
        if (data & 0x100) {
            SPI1_MOSI_HIGH();
        } else {
            SPI1_MOSI_LOW();
        }
        spi_delay();
        SPI1_CLK_HIGH();
        spi_delay();
        SPI1_CLK_LOW();
        data <<= 1;
    }
#if SPI1_DIRECT_SEL
    SPI1_DAT_RD();              //默认配置为读(输入)
#endif
#endif
#endif
}


AT(.spi_seg)
uint8_t spi_read(void)
{
    uint8_t value;
    value = 0;

#if SPI1_EN
#if SPI1_MODE_SEL
    value = spi_module_read();

#else
    SPI1_CLK_LOW();
    spi_delay();
    for (uint8_t i=0;i<8;i++) {
        SPI1_CLK_HIGH();
        spi_delay();
        value <<= 1;
        if (SPI1_DAT_VAL()) {
            value |= 0x01;
        }
        SPI1_CLK_LOW();
        spi_delay();
    }
#endif
#endif

    return value;
}

/*
AT(.spi_seg)
int32_t spi_flash_is_busy(void)
{
    int32_t ret;

#if SPI0_EN
    norflash_chip_sel(FLASH_CHIP1);
    ret = norflash_waitbusy();
    norflash_chip_sel(FLASH_CHIP0);

#elif SPI1_EN
    //(0x05) read status register
    SPI1_CS_LOW();
    spi_write(0x05);
    ret = spi_read() & 0x01;
    SPI1_CS_HIGH();

#endif

    return ret;
}
*/

AT(.spi_seg)
void spi_flash_waitbusy(void)
{
#if SPI0_EN
    uint32_t ret;
    while(1) {
        mdelay(10);
        norflash_chip_sel(FLASH_CHIP1);
        ret = norflash_waitbusy();
        norflash_chip_sel(FLASH_CHIP0);
        if (ret == 0) {
            break;
        }
    }

#elif SPI1_EN
        uint8_t val;
        //(0x05) read status register
        SPI1_CS_LOW();
        spi_write(0x05);
        do {
            val = spi_read();
        } while(val & 0x01);
        SPI1_CS_HIGH();

#endif
}


AT(.spi_init_seg)
void spi_flash_erase(uint32_t flash_addr, uint32_t chip_erase, uint32_t wait_busy)
{
    int count = 500;
    while (spi_lock && count) {
        --count;
        msleep(1);
    }

    if (spi_lock) {
        loge("spi_lock err!!!");
    }

    spi_lock = 1;

#if SPI0_EN
    norflash_chip_sel(FLASH_CHIP1);
    norflash_erase(flash_addr, wait_busy);
    norflash_chip_sel(FLASH_CHIP0);

#elif SPI1_EN
    uint8_t val;

    //(0x06) write enable
    SPI1_CS_LOW();
    spi_write(0x06);
    SPI1_CS_HIGH();

    if (chip_erase) {
        //(0xC7) chip erase
        SPI1_CS_LOW();
        spi_write(0xC7);
        SPI1_CS_HIGH();
    } else {
        //(0x20) sector erase
        SPI1_CS_LOW();
        spi_write(0x20);
        spi_write((uint8_t)(flash_addr>>16));
        spi_write((uint8_t)(flash_addr>>8));
        spi_write((uint8_t)(flash_addr>>0));
        SPI1_CS_HIGH();
    }

    if (wait_busy) {
        //(0x05) read status register
        SPI1_CS_LOW();
        spi_write(0x05);
        do {
            val = spi_read();
        } while(val & 0x01);
        SPI1_CS_HIGH();
    }
#endif

    spi_lock = 0;
}

AT(.spi_seg)
void spi_flash_write(uint8_t *ram_addr, uint32_t flash_addr, uint32_t bytes, uint32_t wait_busy)
{
    int count = 500;
    while (spi_lock && count) {
        --count;
        msleep(1);
    }

    if (spi_lock) {
        loge("spi_lock err!!!");
    }

    spi_lock = 1;

#if SPI0_EN
    norflash_chip_sel(FLASH_CHIP1);
    norflash_write((uint32_t)ram_addr, flash_addr, bytes, 0, wait_busy);
    norflash_chip_sel(FLASH_CHIP0);

#elif SPI1_EN
    uint8_t val;

    //(0x06) write enable
    SPI1_CS_LOW();
    spi_write(0x06);
    SPI1_CS_HIGH();

    //(0x02) page program
    SPI1_CS_LOW();
    spi_write(0x02);
    spi_write((uint8_t)(flash_addr>>16));
    spi_write((uint8_t)(flash_addr>>8));
    spi_write((uint8_t)(flash_addr>>0));
    for (uint32_t i=0;i<bytes;i++) {
        spi_write((uint32_t)(*(ram_addr+i)));
    }
    SPI1_CS_HIGH();

    if (wait_busy) {
        //(0x05) read status register
        SPI1_CS_LOW();
        spi_write(0x05);
        do {
            val = spi_read();
        } while(val & 0x01);
        SPI1_CS_HIGH();
    }
#endif

    spi_lock = 0;
}

AT(.spi_seg)
void spi_flash_read(uint8_t *ram_addr, uint32_t flash_addr, uint32_t bytes)
{
    int count = 500;
    while (spi_lock && count) {
        --count;
        msleep(1);
    }


    if (spi_lock) {
        loge("spi_lock err!!!");
    }

    spi_lock = 1;

#if SPI0_EN
    norflash_read((uint32_t)ram_addr, FLASH_CHIP0_SIZE+flash_addr, bytes);

#elif SPI1_EN
    //(0x03) read data
    SPI1_CS_LOW();
    spi_write(0x03);
    spi_write((uint8_t)(flash_addr>>16));
    spi_write((uint8_t)(flash_addr>>8));
    spi_write((uint8_t)(flash_addr>>0));
    for (uint32_t i=0;i<bytes;i++) {
        *(ram_addr+i) = (uint8_t)spi_read();
    }
    SPI1_CS_HIGH();
#endif

    spi_lock= 0;
}

uint32_t get_spi_flash_size(void)
{
    return SPI_FLASH_SIZE;
}

#define SPI_TEST_EN     0

#if SPI_TEST_EN
uint8_t ttbuf[0x100];

AT(.spi_init_seg)
void spi_test(void)
{
    /*
        spi flash:
        All instructions,addresses and data are shifted in and out of the device, beginning with the most
        significant bit on the the first rising edge of SCLK after /CS is driven low.
      */
    logi("%s", __func__);

#if SPI0_EN
    logi("spi erase");
    spi_flash_erase(0x00, 0, 0);
    spi_flash_waitbusy();
    logi("%s,%d", __func__,__LINE__);

    for (uint32_t i=0;i<0x100;i++) {
        ttbuf[i] = i;
    }

    logi("spi write");
    spi_flash_write((uint8_t *)ttbuf, 0x00, 0x100, 1);
    spi_flash_waitbusy();
    memset(ttbuf, 0x55, 0x100);

    logi("spi read");
    spi_flash_read((uint8_t *)ttbuf, 0x00, 0x100);
    printf_array(ttbuf, 0x100);
#endif

#if SPI1_EN
#if SPI1_MODE_SEL
    volatile uint32_t val;

    //(0x9F) read identification
    SPI1_CS_LOW();
    spi_module_size_set(SPI_CASIZE_8BIT, SPI_DATASIZE_32BIT);
    val = spi_module_read_with_ca(0x9F);
    SPI1_CS_HIGH();
    logi("id = %x", val);

    //(0x90) read manufacturer/device id
    SPI1_CS_LOW();
    spi_module_size_set(SPI_CASIZE_32BIT, SPI_DATASIZE_32BIT);
    val = spi_module_read_with_ca((0x90<<24) | 0x000000);
    SPI1_CS_HIGH();
    logi("id = %x", val);

    //(0x06) write enable
    SPI1_CS_LOW();
    spi_module_size_set(SPI_CASIZE_8BIT, SPI_DATASIZE_0BIT);
    spi_module_write_with_ca(0x06, 0);
    SPI1_CS_HIGH();

    //(0x20) sector erase
    SPI1_CS_LOW();
    spi_module_size_set(SPI_CASIZE_32BIT, SPI_DATASIZE_0BIT);
    spi_module_write_with_ca((0x20<<24) | 0x000000, 0);
    SPI1_CS_HIGH();

    //(0x05) read status register
    SPI1_CS_LOW();
    do {
        mdelay(10);
        spi_module_size_set(SPI_CASIZE_8BIT, SPI_DATASIZE_8BIT);
        val = (uint8_t)spi_module_read_with_ca(0x05);
    } while(val & 0x01);
    SPI1_CS_HIGH();

    for (uint8_t n=0;n<0x10;n++) {
        //(0x06) write enable
        SPI1_CS_LOW();
        spi_module_size_set(SPI_CASIZE_0BIT, SPI_DATASIZE_8BIT);
        spi_module_write(0x06);
        SPI1_CS_HIGH();

        //(0x02) page program
        SPI1_CS_LOW();
        spi_module_size_set(SPI_CASIZE_32BIT, SPI_DATASIZE_8BIT);
        spi_module_write_with_ca((0x02<<24) | (0x000000+n), 0xa0+n);
        SPI1_CS_HIGH();

        //(0x05) read status register
        SPI1_CS_LOW();
        do {
            mdelay(10);
            spi_module_size_set(SPI_CASIZE_8BIT, SPI_DATASIZE_8BIT);
            val = (uint8_t)spi_module_read_with_ca(0x05);
        } while(val & 0x01);
        SPI1_CS_HIGH();

        //(0x03) read data
        SPI1_CS_LOW();
        spi_module_size_set(SPI_CASIZE_32BIT, SPI_DATASIZE_8BIT);
        val = (uint8_t)spi_module_read_with_ca((0x03<<24) | (0x000000+n));
        SPI1_CS_HIGH();
        logi("val = %x", val);
    }

    logi("---------");

    SPI1_CS_INIT();

    //spi_size_set(SPI_CASIZE_32BIT, SPI_DATASIZE_8BIT);
    spi_module_size_set(SPI_CASIZE_0BIT, SPI_DATASIZE_8BIT);

    SPI1_CS_LOW();
    //val = (uint8_t)spi_read_with_ca((0x03<<24) | (0x000000+0));
    //logi("val = %x", val);

    spi_module_write(0x03);
    spi_module_write(0x00);
    spi_module_write(0x00);
    spi_module_write(0x00);

    for (uint8_t n=0;n<0x10;n++) {
        val = (uint8_t)spi_module_read();
        logi("val = %x", val);
    }

    SPI1_CS_HIGH();

    logi("---------");
#endif

    logi("spi erase");
    spi_flash_erase(0x7D000, 0, 1);
    for (uint32_t i=0;i<0x100;i++) {
        ttbuf[i] = i;
    }

    logi("spi write");
    spi_flash_write((uint8_t *)ttbuf, 0x7D000, 0x100, 1);
    memset(ttbuf, 0x55, 0x100);

    logi("spi read");
    spi_flash_read((uint8_t *)ttbuf, 0x7D000, 0x100);
    printf_array(ttbuf, 0x100);
#endif

    while(1);
}

#endif

#else
void spi_init(void)
{
}

void spi_flash_read(uint8_t *ram_addr, uint32_t flash_addr, uint32_t bytes)
{
}

void spi_flash_write(uint8_t *ram_addr, uint32_t flash_addr,
    uint32_t bytes, uint32_t wait_busy)
{
}

void spi_flash_erase(uint32_t flash_addr, uint32_t chip_erase, uint32_t wait_busy)
{
}

#endif

