
#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "tas5711.h"
#include "twi.h"

#if I2S0_USE_TAS5711

#if (1 == I2S0_USE_TAS5711)
#define TWI_ID_TAS5711   TWI_ID_01
#else
#define TWI_ID_TAS5711   TWI_ID_02
#endif

//上电加载寄存器设置
AT(.text.bsp.i2c.rodata)
const cfg_reg registers[] =
{
    {{ 0x00, 0x6C }},
    {{ 0x01, 0x00 }},
    {{ 0x02, 0x86 }},
    {{ 0x03, 0xA0 }},
    {{ 0x04, 0x05 }},
    {{ 0x05, 0x84 }},
    {{ 0x06, 0x00 }},
    //{{ 0x07, 0x24 }},
    {{ 0x08, 0x27 }},
    {{ 0x09, 0x27 }},
    {{ 0x0A, 0x40 }},
    {{ 0x0B, 0x00 }},
    {{ 0x0C, 0xFF }},
    {{ 0x0D, 0xFF }},
    {{ 0x0E, 0xF1 }},
    {{ 0x0F, 0x55 }},
    {{ 0x10, 0x07 }},
    {{ 0x11, 0xAC }},
    {{ 0x12, 0x54 }},
    {{ 0x13, 0xAC }},
    {{ 0x14, 0x54 }},
    {{ 0x15, 0x20 }},
    {{ 0x16, 0x00 }},
    {{ 0x17, 0x00 }},
    {{ 0x18, 0x8F }},
    {{ 0x19, 0x30 }},
    {{ 0x1A, 0x8F }},
    {{ 0x1B, 0xC0 }},
    {{ 0x1C, 0x02 }},
    {{ 0x1D, 0x00 }},
    {{ 0x1E, 0x00 }},
    {{ 0x1F, 0x00 }},
};

// 音量表
AT(.text.bsp.i2c.rodata)
const u8 pa_vol_tbl[33] =
{
  MASTER_SOFT_MUTE,
  //MASTER_N39DB,
  //MASTER_N37DB,
  //MASTER_N35DB,
  //MASTER_N33DB,
  //MASTER_N31DB,
  0x84,
  0x7f,
  0x7b,
  0x78,
  0x75,
  0x72,
  0x6f,
  0x6c,
  0x69,
  0x66,
  0x63,
  0x60,
  0x5d,
  0x5a,
  0x57,
  0x54,
  0x51,
  0x4e,
  0x4b,
  0x48,
  0x45,
  0x42,
  0x3f,
  0x3c,
  0x39,
  0x36,
  0x33,
  0x30,
  0x2d,
  0x2a,
  0x27,
  0x24
};

// 高音
AT(.text.bsp.i2c.rodata)
const unsigned char TAS5711_TREBLEEQ[9][20]=
{
  {	//-5dB
    0x00,
    0x3F, 0x4E,
    0x0B, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x1F, 0x0D,
    0x92, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x21, 0xA4,
    0x63,
  },
  { //-4dB
    0x00,
    0x49, 0x5C,
    0x27, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x24, 0xB3,
    0xB6, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x11, 0xF0,
    0x22,
  },
  { //-3dB
    0x00,
    0x54, 0x10,
    0xEF, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x2B, 0x6D,
    0xF5, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x00, 0x81,
    0x1C,
  },
  { //-2dB
    0x00,
    0x5F, 0x3C,
    0x6F, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x33, 0xE4,
    0xF0, 0x00,
    0x00, 0x00,
    0x00, 0x03,
    0xEC, 0xDE,
    0xA1,
  },
  { //-1dB
    0x00,
    0x6A, 0xA9,
    0x02, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x3F, 0xC7,
    0x04, 0x00,
    0x00, 0x00,
    0x00, 0x03,
    0xD5, 0x8F,
    0xF9,
  },
  { //0dB
    0x00,
    0x79, 0x18,
    0xD8, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x43, 0xC6,
    0xEE, 0x00,
    0x00, 0x00,
    0x00, 0x03,
    0xC3, 0x20,
    0x3A,
  },
  { //+1dB
    0x00,
    0x88, 0xB0,
    0xB7, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x34, 0x2F,
    0x0F, 0x00,
    0x00, 0x00,
    0x00, 0x03,
    0xC3, 0x20,
    0x3A,
  },
  { //+2dB
    0x00,
    0x99, 0x9B,
    0xFD, 0x00,
    0x00, 0x00,
    0x00, 0x00,
    0x32, 0xED,
    0x9F, 0x00,
    0x00, 0x00,
    0x00, 0x03,
    0xB3, 0x76,
    0x64,
  },
  { //+3dB
    0x00,
    0xC2, 0xE4,
    0xE6, 0x00,
    0x00, 0x00,
    0x00, 0x03,
    0xFF, 0x3B,
    0x6B, 0x00,
    0x00, 0x00,
    0x00, 0x03,
    0xBD, 0xDF,
    0xAF,
  },
};

// 低音
#if 0
AT(.text.bsp.i2c.rodata)
const unsigned char TAS5711_BASSEQ[11][20]=
{
	{      //-5dB
    0x00, 0x7F, 0x8C, 0xC5, 0x03,
    0x02, 0x12, 0x22, 0x00, 0x7E,
    0x64, 0xA9, 0x00, 0xFD, 0xED,
    0xDE, 0x03, 0x82, 0x0E, 0x91
	},
	{     //-4dB
    0x00, 0x7F, 0xBF, 0x2E, 0x03,
    0x01, 0x62, 0xDF, 0x00, 0x7E,
    0xE1, 0x87, 0x00, 0xFE, 0x9D,
    0x21, 0x03, 0x81, 0x5F, 0x4C
	},
	{     //-3dB
    0x00, 0x7F, 0xB8, 0xF2, 0x03,
    0x01, 0xEA, 0x25, 0x00, 0x7E,
    0x60, 0x7A, 0x00, 0xFE, 0x15,
    0xDB, 0x03, 0x81, 0xE6, 0x94
	},
	{     //-2dB
    0x00, 0x7F, 0xCD, 0xF6, 0x03,
    0x01, 0xEA, 0x25, 0x00, 0x7E,
    0x4B, 0x76, 0x00, 0xFE, 0x15,
    0xDB, 0x03, 0x81, 0xE6, 0x94
	},
	{    //-1dB
    0x00, 0x7F, 0xE5, 0x8B, 0x03,
    0x01, 0xEA, 0x25, 0x00, 0x7E,
    0x33, 0xE1, 0x00, 0xFE, 0x15,
    0xDB, 0x03, 0x81, 0xE6, 0x94
	},
	{   //0dB
/*    0x00, 0x80, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00*/
		0x00, 0x80, 0x00, 0x00, 0x03,
		0x01, 0xEA, 0x25, 0x00, 0x7E,
		0x19, 0x6C, 0x00, 0xFE, 0x15,
		0xDB, 0x03, 0x81, 0xE6, 0x94
	},
	{   //+1dB
    0x00, 0x80, 0x1D, 0xB0, 0x03,
    0x01, 0xEA, 0x25, 0x00, 0x7D,
    0xFB, 0xBD, 0x00, 0xFE, 0x15,
    0xDB, 0x03, 0x81, 0xE6, 0x94
	},
	{     //+2dB
    0x00, 0x80, 0x3E, 0xFE, 0x03,
    0x01, 0xEA, 0x25, 0x00, 0x7D,
    0xDA, 0x6E, 0x00, 0xFE, 0x15,
    0xDB, 0x03, 0x81, 0xE6, 0x94
	},
	{     //+3dB
    0x00, 0x80, 0x64, 0x5E, 0x03,
    0x01, 0xEA, 0x25, 0x00, 0x7D,
    0xB5, 0x0E, 0x00, 0xFE, 0x15,
    0xDB, 0x03, 0x81, 0xE6, 0x94
	},
	{     //+4dB
    0x00, 0x80, 0x40, 0xF3, 0x03,
    0x00, 0xE1, 0xAC, 0x00, 0x7E,
    0xE0, 0xF5, 0x00, 0xFF, 0x1E,
    0x54, 0x03, 0x80, 0xDE, 0x18
	},
	{      //+5dB
    0x00, 0x80, 0x73, 0xA3, 0x03,
    0x01, 0x2C, 0xBC, 0x00, 0x7E,
    0x63, 0x35, 0x00, 0xFE, 0xD3,
    0x44, 0x03, 0x81, 0x29, 0x28
	}
};
#else
AT(.text.bsp.i2c.rodata)
const unsigned char TAS5711_BASSEQ[MAX_BAS_TAS5711+1]=
{
    0x40+16, 0x40+12, 0x40+8, 0x40+4,
    0x40,
    0x40-8, 0x40-16, 0x40-24, 0x40-32,
};
#endif

/*----------------------------------------------------------------------------*/
/***@brief iic_read_byte
    @details  模拟iic时序,发送n-byte数据
    @param  无
    @return 读取的数据
    @note 发送完成后,clk为0
*/
/*----------------------------------------------------------------------------*/
AT(.text.bsp.i2c)
void i2c_write(u8 *pd, u16 n)
{
    int ret;
    ret = twi_write(TWI_ID_TAS5711, TAS5711_SLAVE_ADR >> 1, pd, n);
    if (ret != HAL_OK) {
        logw("i2c write tas5711 reg fail");
    } else {
        logd("i2c write ok");
    }
}

/*----------------------------------------------------------------------------*/
/***@brief iic_read_byte
    @details  模拟iic时序,发送n-byte数据
    @param  无
    @return 读取的数据
    @note 发送完成后,clk为0
*/
/*----------------------------------------------------------------------------*/
AT(.text.bsp.i2c)
void transmit_registers(cfg_reg const *r, u16 n)
{
    u16 i = 0;
    while (i < n) {
        switch (r[i].command) {

        case CFG_META_BURST:
            i2c_write((u8 *)&r[i+1], r[i].param);
            i += (r[i].param + 1)/2;
            break;
        default:
            i2c_write((u8 *)&r[i], 2);
            break;
        }
        i++;
    }
}

/*----------------------------------------------------------------------------*/
/***@brief iic_read_byte
    @details  模拟iic时序,发送n-byte数据
    @param  无
    @return 读取的数据
    @note 发送完成后,clk为0
*/
/*----------------------------------------------------------------------------*/
AT(.text.bsp.i2c)
void TAS5711_WriteReg(u8 adr, u8 value)
{
    int ret;
    u8 tmp[2];
    tmp[0] = adr;
    tmp[1] = value;

    ret = twi_write(TWI_ID_TAS5711, TAS5711_SLAVE_ADR >> 1, tmp, 2);
    if (ret != HAL_OK) {
        logw("i2c write tas5711 reg fail");
    } else {
        logd("i2c write ok");
    }
}

AT(.text.bsp.i2c)
void tas5711_Init(void)
{
#if 1
    // RESET DEVICE
    STANDBY_HI();
    PA_RES_LOW();
    PA_PDN_LOW();

	//delay_5ms(2);
	mdelay(10);

    PA_PDN_HIGH();
    //delay_5ms(10);
    mdelay(50);
    PA_RES_HIGH();
    //delay_5ms(10);    //at least 13.5ms
    mdelay(50);
#endif

    //Load base data
    //transmit_registers(registers, sizeof(registers)/sizeof(registers[0]));
}

AT(.text.bsp.i2c)
void tas5711_SetVol(u8 vol)
{
    u8 val=pa_vol_tbl[vol];

#if 0
    if( (val != 0xff) && (func_cb.sta == FUNC_HDMI) ) {
        val += 15;
    }
#endif

    TAS5711_WriteReg(0x07,val);

#if USER_SAVE_FLASH
      param_user_amp_vol_write();
      sys_cb.cm_times = 0;
      sys_cb.cm_vol_change = 1;
#endif
}

AT(.text.bsp.i2c)
void tas5711_Mute(void)
{
    TAS5711_WriteReg(0x06,0x07);
}

AT(.text.bsp.i2c)
void tas5711_Unmute(void)
{
    TAS5711_WriteReg(0x06,0x00);
}

AT(.text.bsp.i2c)
void load_tas5711_reg(void)
{
    transmit_registers(registers, sizeof(registers)/sizeof(registers[0]));
}


AT(.text.bsp.i2c)
void load_music_coef(void)
{
    //transmit_registers(music_m, sizeof(music_m)/sizeof(music_m[0]));
}


AT(.text.bsp.i2c)
void load_movie_coef(void)
{
    //transmit_registers(movie_m, sizeof(movie_m)/sizeof(movie_m[0]));
}

AT(.text.bsp.i2c)
void load_voice_coef(void)
{
    //transmit_registers(voice_m, sizeof(voice_m)/sizeof(voice_m[0]));
}

AT(.text.bsp.i2c)
void load_def_coef(void)
{
    //transmit_registers(def_m, sizeof(def_m)/sizeof(def_m[0]));
}

#define TREBLEEQadd1 0x59 //CH1 BQ1
#define TREBLEEQadd2 0x5d	//CH2 BQ1
#define BASSEQadd1   0x59
#define BASSEQadd2   0x5d

AT(.text.bsp.i2c)
void tas5711_SetTre(u8 trb)
{
    u8 *ptr;
    ptr = (unsigned char *)TAS5711_TREBLEEQ[trb];


    //i2c_write_x(TREBLEEQadd1, ptr, 20);
    //i2c_write_x(TREBLEEQadd2, ptr, 20);
    u8 tmp[21];
    tmp[0] = TREBLEEQadd1;
    memcpy(tmp+1, ptr, 20);
    i2c_write(tmp, 21);
    tmp[0] = TREBLEEQadd2;
    i2c_write(tmp, 21);
}

AT(.text.bsp.i2c)
void tas5711_SetBas(u8 bass)
{
   TAS5711_WriteReg(0x0a, TAS5711_BASSEQ[bass]);
#if 0
    //TAS5711_WriteReg(0x0a, 0xff);
    u8 *ptr;
    //TAS5711_MUTE();
    //delay_5ms(1);
    ptr = (unsigned char *)TAS5711_BASSEQ[bass];
    i2c_write_x(BASSEQadd1, ptr, 20);
    i2c_write_x(BASSEQadd2, ptr, 20);
    //delay_5ms(1);
    //TAS5711_UN_MUTE();
#endif
}

AT(.text.bsp.i2c)
void tas5711_SetEQ(u8 eq)
{
}
#endif

