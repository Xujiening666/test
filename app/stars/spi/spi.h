#ifndef _SPI_H_
#define _SPI_H_

//SPI0
#if (SPI0_CS_PIN_SEL == 0)
#define SPI0_CS_PIN_CFG     (PIN_B7 | PIN_FUNC3 | PIN_DRI1 | PIN_PULL_UP100K)
#endif

#if (SPI0_CLK_PIN_SEL == 0)
#define SPI0_CLK_PIN_CFG    (PIN_B0 | PIN_FUNC5 | PIN_DRI1)
#endif

#if (SPI0_MOSI_PIN_SEL == 0)
#define SPI0_MOSI_PIN_CFG   (PIN_B1 | PIN_FUNC5 | PIN_DRI1)
#endif

#if (SPI0_MISO_PIN_SEL == 0)
#define SPI0_MISO_PIN_CFG   (PIN_B2 | PIN_FUNC5 | PIN_DRI1)
#endif


//SPI1
#if SPI1_MODE_SEL
//使用SPI1硬件模块
#if (SPI1_CS_PIN_SEL == 0)
#define SPI1_CS_PIN_CFG     (PIN_A7 | PIN_FUNC2)
#define SPI1_CS_PIN         (PIN_A7)
#define SPI1_CS_FUNC        (PIN_FUNC2)
#elif (SPI1_CS_PIN_SEL == 1)
#define SPI1_CS_PIN_CFG     (PIN_B5 | PIN_FUNC6)
#define SPI1_CS_PIN         (PIN_B5)
#define SPI1_CS_FUNC        (PIN_FUNC6)
#endif

#if (SPI1_CLK_PIN_SEL == 0)
#define SPI1_CLK_PIN_CFG    (PIN_A6 | PIN_FUNC2)
#elif (SPI1_CLK_PIN_SEL == 1)
#define SPI1_CLK_PIN_CFG    (PIN_B6 | PIN_FUNC6)
#endif

#if (SPI1_MOSI_PIN_SEL == 0)
#define SPI1_MOSI_PIN_CFG   (PIN_A8 | PIN_FUNC2 | PIN_PULL_UP100K)
#elif (SPI1_MOSI_PIN_SEL == 1)
#define SPI1_MOSI_PIN_CFG   (PIN_B7 | PIN_FUNC6 | PIN_PULL_UP100K)
#endif

#if (SPI1_MISO_PIN_SEL == 0)
#define SPI1_MISO_PIN_CFG   (PIN_A9 | PIN_FUNC2 | PIN_PULL_UP100K)
#elif (SPI1_MISO_PIN_SEL == 1)
#define SPI1_MISO_PIN_CFG   (PIN_B8 | PIN_FUNC6 | PIN_PULL_UP100K)
#elif (SPI1_MISO_PIN_SEL == 2)
#define SPI1_MISO_PIN_CFG   (PIN_B10 | PIN_FUNC4 | PIN_PULL_UP100K)
#endif

#define SPI1_CS_INIT()      pin_set_func(SPI1_CS_PIN, PIN_FUNCOUTPUT)
#define SPI1_CS_DEINIT()    pin_set_func(SPI1_CS_PIN, SPI1_CS_FUNC)
#define SPI1_CS_LOW()       pin_set_value(SPI1_CS_PIN, 0)
#define SPI1_CS_HIGH()      pin_set_value(SPI1_CS_PIN, 1)



#define SPI1_RST_PIN_CFG   (PIN_A4 | PIN_FUNCOUTPUT | PIN_DRI1)

#define SPI1_RST_HIGH()    pin_set_value(PIN_A4, 1)
#define SPI1_RST_LOW()     pin_set_value(PIN_A4, 0)


#define SPI1_LCD_PIN_CFG   (PIN_B12 | PIN_FUNCOUTPUT | PIN_DRI1)

#define SPI1_LCD_HIGH()    pin_set_value(PIN_B12, 1)
#define SPI1_LCD_LOW()     pin_set_value(PIN_B12, 0)

void spi_write_9bit(uint32_t data);


#else
//使用GPIO软件模拟
#define SPI1_CS_PIN         (PIN_B5)
#define SPI1_CLK_PIN        (PIN_B6)
#define SPI1_MOSI_PIN       (PIN_B7)
#define SPI1_MISO_PIN       (PIN_B8)

#define SPI1_CS_INIT()      pin_config(SPI1_CS_PIN | PIN_FUNCOUTPUT | PIN_DRI1)
#define SPI1_CLK_INIT()     pin_config(SPI1_CLK_PIN | PIN_FUNCOUTPUT | PIN_DRI1)
#define SPI1_MOSI_INIT()    pin_config(SPI1_MOSI_PIN | PIN_FUNCOUTPUT | PIN_DRI1 | PIN_PULL_UP2K)
#define SPI1_MISO_INIT()    pin_config(SPI1_MISO_PIN | PIN_FUNCINPUT | PIN_DRI1 | PIN_PULL_UP2K)

#define SPI1_CS_LOW()       pin_set_value(SPI1_CS_PIN, 0)
#define SPI1_CS_HIGH()      pin_set_value(SPI1_CS_PIN, 1)

#define SPI1_CLK_LOW()      pin_set_value(SPI1_CLK_PIN, 0)
#define SPI1_CLK_HIGH()     pin_set_value(SPI1_CLK_PIN, 1)

#define SPI1_MOSI_LOW()     pin_set_value(SPI1_MOSI_PIN, 0)
#define SPI1_MOSI_HIGH()    pin_set_value(SPI1_MOSI_PIN, 1)

#if SPI1_DIRECT_SEL
//3线
#define SPI1_DAT_WR()       pin_set_func(SPI1_MOSI_PIN, PIN_FUNCOUTPUT)
#define SPI1_DAT_RD()       pin_set_func(SPI1_MOSI_PIN, PIN_FUNCINPUT)
#define SPI1_DAT_VAL()      pin_get_value(SPI1_MOSI_PIN)
#else
//4线
#define SPI1_DAT_VAL()      pin_get_value(SPI1_MISO_PIN)
#endif

#endif

#define SPI1_RST_PIN_CFG      (PIN_A4 | PIN_FUNCOUTPUT | PIN_DRI1)
#define SPI1_LCD_DC_PIN_CFG   (PIN_B13 | PIN_FUNCOUTPUT | PIN_DRI1)

#define SPI1_RST_HIGH()    pin_set_value(PIN_A4, 1)
#define SPI1_RST_LOW()     pin_set_value(PIN_A4, 0)

#define SPI1_LCD_PIN_CFG   (PIN_B12 | PIN_FUNCOUTPUT | PIN_DRI1)

#define SPI1_LCD_HIGH()    pin_set_value(PIN_B12, 1)
#define SPI1_LCD_LOW()     pin_set_value(PIN_B12, 0)


void spi_write_9bit(uint32_t data);

void spi_init(void);
void spi_flash_erase(uint32_t flash_addr, uint32_t chip_erase, uint32_t wait_busy);
void spi_flash_write(uint8_t *ram_addr, uint32_t flash_addr, uint32_t bytes, uint32_t wait_busy);
void spi_flash_read(uint8_t *ram_addr, uint32_t flash_addr, uint32_t bytes);

void spi_test(void);
void spi_write(uint32_t data);


#endif

