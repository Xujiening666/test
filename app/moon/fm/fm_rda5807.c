#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "fm_api.h"

#if FM_RDA5807_EN


static uint16_t chip_id;
static uint8_t wbuffer[12];
static uint8_t rbuffer[10];


#if RDA5807_PE_SP

/*RDA5807PE  RDA5807SP */
AT(.fm_rda5807_rodata_seg)
static const uint8_t RAD5807_INIT_TAB[104] = {
#if (FM_CLK_SEL == FM_CLK_32768_OSC)
    0xc0, 0x01, //internal 32768Hz, sine
#elif (FM_CLK_SEL == FM_CLK_24M_HOSC)
    0xc4, 0x51, //24M, square
#elif (FM_CLK_SEL == FM_CLK_12M)
    0xc4, 0x11, //12M, square
#elif (FM_CLK_SEL == FM_CLK_LOSC)
    0xc4, 0x01, //32768, square
#else
    #error "rda5807: please sel fm clk src"
#endif

#if FM_SEEK_50K_STEP
    0x1b, 0x92,
    0x0C, 0x00,
#else           //Step 100K
    0x00, 0x10,
    0x04, 0x00,
#endif
    0x86, 0xad,
    0x80, 0x00,
    0x5F, 0x1A,
    0x5e, 0xc6,
    0x00, 0x00,
    0x40, 0x6e,
    0x2d, 0x80,
    0x58, 0x03,
    0x58, 0x04,
    0x58, 0x04,
    0x58, 0x04,
    0x00, 0x47,
    0x90, 0x00,
    0xF5, 0x87,
    0x7F, 0x0B,
    0x04, 0xF1,
    0x5E, 0xc0,
    0x50, 0x6f,
    0x55, 0x92,
    0x00, 0x7d,
    0x10, 0xC0,
    0x07, 0x80,
    0x41, 0x1d,
    0x40, 0x06,
    0x1f, 0x9B,
    0x4c, 0x2b,
    0x81, 0x10,
    0x45, 0xa0,
#if FM_SEEK_50K_STEP
    0x55, 0x3F,
#else
    0x19, 0x3F,
#endif
    0xaf, 0x40,
    0x06, 0x81,
    0x1b, 0x2a,
    0x0D, 0x04,
    0x80, 0x9F,
    0x17, 0x8A,
    0xD3, 0x49,
    0x11, 0x42,
    0xA0, 0xC4,
    0x3E, 0xBB,
    0x00, 0x00,
    0x58, 0x04,
    0x58, 0x04,
    0x58, 0x04,
    0x00, 0x74,
    0x3D, 0x00,
    0x03, 0x0C,
    0x2F, 0x68,
    0x38, 0x77,
};
#endif

#if RDA5807_HS_HP
/* init reg for RDA5807HS  RDA5807HP */
AT(.fm_rda5807_rodata_seg)
static const uint8_t RAD5807H_INIT_TAB[110] = {
#if (FM_CLK_SEL == FM_CLK_32768_OSC)
    0xc0, 0x01, //internal 32768Hz, sine
#elif (FM_CLK_SEL == FM_CLK_24M_HOSC)
    0xc4, 0x51, //24M, square
#elif (FM_CLK_SEL == FM_CLK_12M)
    0xc4, 0x11, //12M, square
#elif (FM_CLK_SEL == FM_CLK_LOSC)
    0xc4, 0x01, //32768, square
#else
    #error "rda5807h: please sel fm clk src"
#endif

#if FM_SEEK_50K_STEP
    0x00,0x12,
#else
    0x00,0x10,
#endif
    0x04,0x00,
    0x86,0xBF,
    0x40,0x00,
    0x46,0xc6,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x00,
    0x00,0x06,
    0x00,0x19,
    0x2A,0x11,
    0x00,0x2E,
    0x2A,0x30,
    0xB8,0x3C,
    0x90,0x00,
    0x2A,0x91,
    0x84,0x12,
    0x00,0xA8,
    0xC4,0x00,
    0xE0,0x00,
    0x30,0x1D,
    0x81,0x6A,
    0x46,0x08,
    0x00,0x86,
    0x06,0x61,
    0x00,0x00,
    0x10,0x9E,
    0x24,0xC9,
    0x04,0x08,
    0x06,0x08,
    0xE1,0x05,
    0x3B,0x6C,
    0x2B,0xEC,
    0x09,0x0F,
    0x34,0x14,
    0x14,0x50,
    0x09,0x6D,
    0x2D,0x96,
    0x01,0xDA,
    0x2A,0x7B,
    0x08,0x21,
    0x13,0xD5,
    0x48,0x91,
    0x00,0xbc,
    0x08,0x96,
    0x15,0x3C,
    0x0B,0x80,
    0x25,0xC7,
    0x00,0x00,
};
#endif

#if RDA5807_M_MP
/* init reg for RDA5807MP  RDA5807M */
AT(.fm_rda5807_rodata_seg)
static const uint8_t RAD5807M_INIT_TAB[72] = {
#if (FM_CLK_SEL == FM_CLK_32768_OSC)
    0xc0, 0x01, //internal 32768Hz, sine
#elif (FM_CLK_SEL == FM_CLK_24M_HOSC)
    0xc4, 0x51, //24M, square
#elif (FM_CLK_SEL == FM_CLK_12M)
    0xc4, 0x11, //12M, square
#elif (FM_CLK_SEL == FM_CLK_LOSC)
    0xc4, 0x01, //32768, square
#else
    #error "rda5807m: please sel fm clk src"
#endif

    0x00, 0x00,
    0x04, 0x00,
    0xC3, 0xad,
    0x60, 0x00,
    0x42, 0x12,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x19,
    0x2a, 0x11,
    0xB0, 0x42,
    0x2A, 0x11,
    0xb8, 0x31,
    0xc0, 0x00,
    0x2a, 0x91,
    0x94, 0x00,
    0x00, 0xa8,
    0xc4, 0x00,
    0xF7, 0xcF,
    0x12, 0x14,
    0x80, 0x6F,
    0x46, 0x08,
    0x00, 0x86,
    0x06, 0x61,
    0x00, 0x00,
    0x10, 0x9E,
    0x23, 0xC8,
    0x04, 0x06,
    0x0E, 0x1C,
};
#endif

AT(.fm_rda5807_seg)
void rda5807_read(uint8_t *data, uint8_t size)
{
    twi_read(FM_TWI_ID, FM_TWI_ADDR, data, size);
}

AT(.fm_rda5807_seg)
void rda5807_write(const uint8_t *data, uint8_t size)
{
    twi_write(FM_TWI_ID, FM_TWI_ADDR, data, size);
}

AT(.fm_rda5807_seg)
void rda5807_fill(const uint8_t *pSrc)
{
    uint8_t i;

    for (i = 0; i <= 11; i++) {
        wbuffer[i] = *pSrc++;
    }
}

AT(.fm_rda5807_seg)
void rda5807_seek_tune(uint16_t freq)
{
    uint16_t ch = 0;

    ch = (freq - 870);
    wbuffer[0] |= 0x40;
    wbuffer[2] = ch >> 2;
    wbuffer[3] = ((ch & 0x0003) << 6) | 0x10;

    rda5807_write(wbuffer, 4);
    mdelay(10);

    rda5807_write(wbuffer, 4);   //set again
    mdelay(10);
}

AT(.fm_rda5807_seg)
uint8_t rda5807_online(void)
{
    logi("rda5807 online.");

    for(uint8_t i = 0; i < 3; i++) {
        wbuffer[0] = 0x00;
        wbuffer[1] = 0x02;
        rda5807_write(wbuffer, 2);

        //read ID
        rda5807_read(rbuffer, 10);
        chip_id = (rbuffer[8] << 8) | rbuffer[9];
        logi("rda5807 id:%04x", chip_id);

        if ((chip_id == 0x5801) || (chip_id == 0x5804) || (chip_id == 0x5808)) {
            return 1;
        }
    }

    return 0;
}

AT(.fm_rda5807_seg)
void rda5807_init(void)
{
#if RDA5807_PE_SP
    if (0x5804 == chip_id) {
        rda5807_fill(RAD5807_INIT_TAB);
        rda5807_write(wbuffer, 2);
        mdelay(120);

        rda5807_write(RAD5807_INIT_TAB, 104);
        mdelay(1);
    }
#endif

#if RDA5807_HS_HP
    if (0x5801 == chip_id) {
        rda5807_fill(RAD5807H_INIT_TAB);
        rda5807_write(wbuffer, 2);
        mdelay(120);

        rda5807_write(RAD5807H_INIT_TAB, 110);
        mdelay(1);
    }
#endif

#if RDA5807_M_MP
    if (0x5808 == chip_id) {
        rda5807_fill(RAD5807M_INIT_TAB);
        rda5807_write(wbuffer, 2);
        mdelay(120);

        rda5807_write(RAD5807M_INIT_TAB, 72);
        mdelay(1);
    }
#endif
}

AT(.fm_rda5807_seg)
void rda5807_set_vol(uint8_t vol)
{
    if (vol > 15)
        vol = 15;

    wbuffer[7] &= ~0x0F;
    wbuffer[7] |= vol;

    wbuffer[3] &= ~BIT(4);  //03H.4=0,disable tune
    rda5807_write(wbuffer, 8);
}

AT(.fm_rda5807_seg)
uint8_t rda5807_seek(uint16_t freq)
{
    rda5807_seek_tune(freq);
    mdelay(10);

    do {
        rda5807_read(rbuffer, 4);
        mdelay(5);
    } while (!((rbuffer[3] >> 7) & 0x01));

    return (rbuffer[2] & 0x01) ? true : false;
}

AT(.fm_rda5807_seg)
void rda5807_mute(void)
{
    wbuffer[0] &= ~(1 << 6);//MUTE ENABLE

    rda5807_write(wbuffer, 2);
    mdelay(5);
}

AT(.fm_rda5807_seg)
void rda5807_unmute(void)
{
    wbuffer[0] |= (1 << 6) | (1 << 7);
    wbuffer[1] |= (1 << 0);

    rda5807_write(wbuffer, 2);
    mdelay(5);
}

AT(.fm_rda5807_seg)
void rda5807_off(void)
{
    wbuffer[1] &= ~(1 << 0);
    rda5807_write(wbuffer, 2);
    mdelay(100);
}

#endif
