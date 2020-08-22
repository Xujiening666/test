#ifndef _HAL_SPI_LIB_H_
#define _HAL_SPI_LIB_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "sys_hal_def.h"


typedef struct {
    uint32_t datasize;      //data size
    uint32_t casize;        //cmd addr size
    uint32_t clkdiv;        //clock divider
    uint32_t direct;        //direct mode
    uint32_t first;         //MSB/LSB first
    uint32_t cpha;          //phase 0/1
    uint32_t cpol;          //polarity high/low
} spi_param_t;

enum spi_polarity_e {
    SPI_POLARITY_LOW        = (0x0<<0),
    SPI_POLARITY_HIGH       = (0x1<<0),
};

enum spi_phase_e {
    SPI_PHASE_0             = (0x0<<1),
    SPI_PHASE_1             = (0x1<<1),
};

enum spi_first_e {
    SPI_FIRST_MSB           = (0x0<<2),
    SPI_FIRST_LSB           = (0x1<<2),
};

enum spi_direct_e {
    SPI_2LINE_UNIDIRECTIONAL= (0x0<<3),
    SPI_1LINE_BIDIRECTIONAL = (0x1<<3),
};

enum spi_clockdiv_e {
    SPI_CLOCKDIV_2          = (0x0<<8),
    SPI_CLOCKDIV_4          = (0x1<<8),
    SPI_CLOCKDIV_8          = (0x2<<8),
    SPI_CLOCKDIV_16         = (0x3<<8),
    SPI_CLOCKDIV_32         = (0x4<<8),
    SPI_CLOCKDIV_64         = (0x5<<8),
    SPI_CLOCKDIV_128        = (0x6<<8),
    SPI_CLOCKDIV_256        = (0x7<<8),
};

enum spi_casize_e {
    SPI_CASIZE_0BIT         = (0x20<<16),
    SPI_CASIZE_8BIT         = (0x07<<16),
    SPI_CASIZE_16BIT        = (0x0F<<16),
    SPI_CASIZE_32BIT        = (0x1F<<16),
    SPI_CASIZE_MASK         = (0x3F<<16),
};

enum spi_datasize_e {
    SPI_DATASIZE_0BIT       = (0x20<<24),
    SPI_DATASIZE_8BIT       = (0x07<<24),
    SPI_DATASIZE_16BIT      = (0x0F<<24),
    SPI_DATASIZE_32BIT      = (0x1F<<24),
    SPI_DATASIZE_MASK       = (0x3F<<24),
};

enum spi_rw_type_e {
    SPI_RW_TYPE_RD          = (0x0<<1),
    SPI_RW_TYPE_WR          = (0x1<<1),
};


void spi_module_init(void);
void spi_module_size_set(uint32_t ca_size, uint32_t data_size);
void spi_module_write(uint32_t data);
uint32_t spi_module_read(void);
void spi_module_write_with_ca(uint32_t ca, uint32_t data);
uint32_t spi_module_read_with_ca(uint32_t ca);


#ifdef __cplusplus
}
#endif

#endif
