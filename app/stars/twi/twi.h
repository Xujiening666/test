#ifndef _TWI_H_
#define _TWI_H_


/* minimum clk: 24M/256=93.75K
 *
 * divider = 2^(n+1)
 * [0x7]: 24M / 256 = 93.75K
 * [0x6]: 24M / 128 = 187.5K
 * [0x5]: 24M / 64 = 375K
 */

#define TWI1_CLK_FREQ           100000
#define TWI2_CLK_FREQ           100000

#if (TWI1_SCL_PIN_SEL == 0)
#define TWI1_SCL_PIN_CFG        (PIN_B0 | PIN_FUNC3 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SCL_PIN_SEL == 1)
#define TWI1_SCL_PIN_CFG        (PIN_D0 | PIN_FUNC7 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SCL_PIN_SEL == 2)
#define TWI1_SCL_PIN_CFG        (PIN_C1 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SCL_PIN_SEL == 3)
#define TWI1_SCL_PIN_CFG        (PIN_A5 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP2K)
#endif

#if (TWI1_SDA_PIN_SEL == 0)
#define TWI1_SDA_PIN_CFG        (PIN_B2 | PIN_FUNC3 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SDA_PIN_SEL == 1)
#define TWI1_SDA_PIN_CFG        (PIN_D1 | PIN_FUNC7 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SDA_PIN_SEL == 2)
#define TWI1_SDA_PIN_CFG        (PIN_C0 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI1_SDA_PIN_SEL == 3)
#define TWI1_SDA_PIN_CFG        (PIN_A4 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP2K)
#endif


#if (TWI2_SCL_PIN_SEL == 0)
#define TWI2_SCL_PIN_CFG        (PIN_B3 | PIN_FUNC9 | PIN_DRI1 | PIN_PULL_UP2K)
#define TWI2_SCL_PIN_NUM        (PIN_B3)
#define TWI2_SCL_PIN_FUNC       (PIN_FUNC9)
#define TWI2_SCL_PIN_PULL       (PIN_PULL_UP2K)
#elif (TWI2_SCL_PIN_SEL == 1)
#define TWI2_SCL_PIN_CFG        (PIN_B12 | PIN_FUNC4 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI2_SCL_PIN_SEL == 2)
#define TWI2_SCL_PIN_CFG        (PIN_D1 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI2_SCL_PIN_SEL == 3)
#define TWI2_SCL_PIN_CFG        (PIN_B1 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI2_SCL_PIN_SEL == 4)
#define TWI2_SCL_PIN_CFG        (PIN_B6 | PIN_FUNC8 | PIN_DRI1 | PIN_PULL_UP2K)
#endif

#if (TWI2_SDA_PIN_SEL == 0)
#define TWI2_SDA_PIN_CFG        (PIN_B4 | PIN_FUNC9 | PIN_DRI1 | PIN_PULL_UP2K)
#define TWI2_SDA_PIN_NUM        (PIN_B4)
#define TWI2_SDA_PIN_FUNC       (PIN_FUNC9)
#define TWI2_SDA_PIN_PULL       (PIN_PULL_UP2K)
#define TWI2_SDA_PIN_PULL_100K  (PIN_PULL_UP100K)
#elif (TWI2_SDA_PIN_SEL == 1)
#define TWI2_SDA_PIN_CFG        (PIN_B13 | PIN_FUNC4 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI2_SDA_PIN_SEL == 2)
#define TWI2_SDA_PIN_CFG        (PIN_D0 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI2_SDA_PIN_SEL == 3)
#define TWI2_SDA_PIN_CFG        (PIN_B2 | PIN_FUNC2 | PIN_DRI1 | PIN_PULL_UP2K)
#elif (TWI2_SDA_PIN_SEL == 4)
#define TWI2_SDA_PIN_CFG        (PIN_B7 | PIN_FUNC8 | PIN_DRI1 | PIN_PULL_UP2K)
#endif


/***************************************************************************
                 use gpio software mode
***************************************************************************/
#define TWI_SCL_PIN             PIN_B0
#define TWI_SDA_PIN             PIN_B2

#define TWI_SCL_INIT()          pin_config(TWI_SCL_PIN | PIN_FUNCOUTPUT | PIN_DRI3)
#define TWI_SCL_FUNC_OUT()      pin_set_func(TWI_SCL_PIN, PIN_FUNCOUTPUT)
#define TWI_SCL_VAL_HIGH()      pin_set_value(TWI_SCL_PIN, 1)
#define TWI_SCL_VAL_LOW()       pin_set_value(TWI_SCL_PIN, 0)

#define TWI_SDA_INIT()          pin_config(TWI_SDA_PIN | PIN_FUNCOUTPUT | PIN_DRI3 | PIN_PULL_UP2K)
#define TWI_SDA_FUNC_OUT()      pin_set_func(TWI_SDA_PIN, PIN_FUNCOUTPUT)
#define TWI_SDA_FUNC_IN()       pin_set_func(TWI_SDA_PIN, PIN_FUNCINPUT)
#define TWI_SDA_VAL_HIGH()      pin_set_value(TWI_SDA_PIN, 1)
#define TWI_SDA_VAL_LOW()       pin_set_value(TWI_SDA_PIN, 0)
#define TWI_SDA_VAL_GET()       pin_get_value(TWI_SDA_PIN)


void twi_init(void);
void twi_deinit(void);
int twi_write(uint32_t id, uint32_t addr, const uint8_t *data, uint32_t bytes);
int twi_read(uint32_t id, uint32_t addr, uint8_t *data, uint32_t bytes);

void twi_delay(void);
void twi_io_init(void);
void twi_io_deinit(void);
void twi_start(void);
void twi_stop(void);
void twi_write_byte(uint8_t data);
uint8_t twi_read_byte(void);
void twi_send_ack(void);
void twi_send_nack(void);
uint8_t twi_get_ack(void);

#endif

