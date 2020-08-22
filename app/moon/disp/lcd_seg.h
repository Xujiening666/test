#ifndef _LCD_SEG_H_
#define _LCD_SEG_H_


#ifdef __cplusplus
 extern "C" {
#endif


#define LCD_COM_MAX             5
#define LCD_SEG_MAX             8

#define LCD_COM_LEVEL_SHIFT     (0)                         //2bit
#define LCD_COM_LEVEL_MASK      (0x3<<LCD_COM_LEVEL_SHIFT)
#define LCD_COM_NUM_SHIFT       (2)                         //4bit
#define LCD_COM_NUM_MASK        (0xF<<LCD_COM_NUM_SHIFT)

#define LCD_SEG_NUM0_SHIFT      (0)
#define LCD_SEG_NUM0_MASK       (0x1<<LCD_SEG_NUM0_SHIFT)
#define LCD_SEG_NUM1_SHIFT      (1)
#define LCD_SEG_NUM1_MASK       (0x1<<LCD_SEG_NUM1_SHIFT)
#define LCD_SEG_NUM2_SHIFT      (2)
#define LCD_SEG_NUM2_MASK       (0x1<<LCD_SEG_NUM2_SHIFT)
#define LCD_SEG_NUM3_SHIFT      (3)
#define LCD_SEG_NUM3_MASK       (0x1<<LCD_SEG_NUM3_SHIFT)
#define LCD_SEG_NUM4_SHIFT      (4)
#define LCD_SEG_NUM4_MASK       (0x1<<LCD_SEG_NUM4_SHIFT)
#define LCD_SEG_NUM5_SHIFT      (5)
#define LCD_SEG_NUM5_MASK       (0x1<<LCD_SEG_NUM5_SHIFT)
#define LCD_SEG_NUM6_SHIFT      (6)
#define LCD_SEG_NUM6_MASK       (0x1<<LCD_SEG_NUM6_SHIFT)
#define LCD_SEG_NUM7_SHIFT      (7)
#define LCD_SEG_NUM7_MASK       (0x1<<LCD_SEG_NUM7_SHIFT)

enum lcd_com_e {
    LCD_COM_0 = 0,
    LCD_COM_1,
    LCD_COM_2,
    LCD_COM_3,
    LCD_COM_4,

    LCD_A,
    LCD_B,
    LCD_C,
    LCD_D,
    LCD_E,
    LCD_F,
    LCD_G,
};

enum lcd_level_e {
    LCD_LEVEL_H = 0,
    LCD_LEVEL_L,
    LCD_LEVEL_M,

    LCD_LEVEL_Z,
};

#define _LCD_CYCLE(num, level) \
    ((num<<LCD_COM_NUM_SHIFT) | (level<<LCD_COM_LEVEL_SHIFT))

enum lcd_cycle_e {
    LCD_CYCLE_0H  = _LCD_CYCLE(LCD_COM_0,LCD_LEVEL_H),
    LCD_CYCLE_0L  = _LCD_CYCLE(LCD_COM_0,LCD_LEVEL_L),
    LCD_CYCLE_0M  = _LCD_CYCLE(LCD_COM_0,LCD_LEVEL_M),

    LCD_CYCLE_1H  = _LCD_CYCLE(LCD_COM_1,LCD_LEVEL_H),
    LCD_CYCLE_1L  = _LCD_CYCLE(LCD_COM_1,LCD_LEVEL_L),
    LCD_CYCLE_1M  = _LCD_CYCLE(LCD_COM_1,LCD_LEVEL_M),

    LCD_CYCLE_2H  = _LCD_CYCLE(LCD_COM_2,LCD_LEVEL_H),
    LCD_CYCLE_2L  = _LCD_CYCLE(LCD_COM_2,LCD_LEVEL_L),
    LCD_CYCLE_2M  = _LCD_CYCLE(LCD_COM_2,LCD_LEVEL_M),

    LCD_CYCLE_3H  = _LCD_CYCLE(LCD_COM_3,LCD_LEVEL_H),
    LCD_CYCLE_3L  = _LCD_CYCLE(LCD_COM_3,LCD_LEVEL_L),
    LCD_CYCLE_3M  = _LCD_CYCLE(LCD_COM_3,LCD_LEVEL_M),

    LCD_CYCLE_4H  = _LCD_CYCLE(LCD_COM_4,LCD_LEVEL_H),
    LCD_CYCLE_4L  = _LCD_CYCLE(LCD_COM_4,LCD_LEVEL_L),
    LCD_CYCLE_4M  = _LCD_CYCLE(LCD_COM_4,LCD_LEVEL_M),

    LCD_CYCLE_A   = _LCD_CYCLE(LCD_A,LCD_LEVEL_Z),
    LCD_CYCLE_B   = _LCD_CYCLE(LCD_B,LCD_LEVEL_Z),
    LCD_CYCLE_C   = _LCD_CYCLE(LCD_C,LCD_LEVEL_Z),
    LCD_CYCLE_D   = _LCD_CYCLE(LCD_D,LCD_LEVEL_Z),
    LCD_CYCLE_E   = _LCD_CYCLE(LCD_E,LCD_LEVEL_Z),
    LCD_CYCLE_F   = _LCD_CYCLE(LCD_F,LCD_LEVEL_Z),
    LCD_CYCLE_G   = _LCD_CYCLE(LCD_G,LCD_LEVEL_Z),
};


/*
    C0--C1--C2--C3--C4--S0--S1--S2--S3--S4--S5--S6--S7
    B5--B6--B7--B8--D0--B11-B12-B13-B0--B1--B2--B4--B3
*/
#define PIN_COM0        PIN_B5
#define PIN_COM1        PIN_B6
#define PIN_COM2        PIN_B7
#define PIN_COM3        PIN_B8
#define PIN_COM4        PIN_D0
#define PIN_SEG0        PIN_B11
#define PIN_SEG1        PIN_B12
#define PIN_SEG2        PIN_B13
#define PIN_SEG3        PIN_B0
#define PIN_SEG4        PIN_B1
#define PIN_SEG5        PIN_B2
#define PIN_SEG6        PIN_B4
#define PIN_SEG7        PIN_B3


#define LCD_COM0_INIT() {pin_config(PIN_COM0 | PIN_DRI3 | PIN_FUNCINPUT | PIN_PULL_UP2K_DOWN2K);}
#define LCD_COM1_INIT() {pin_config(PIN_COM1 | PIN_DRI3 | PIN_FUNCINPUT | PIN_PULL_UP2K_DOWN2K);}
#define LCD_COM2_INIT() {pin_config(PIN_COM2 | PIN_DRI3 | PIN_FUNCINPUT | PIN_PULL_UP2K_DOWN2K);}
#define LCD_COM3_INIT() {pin_config(PIN_COM3 | PIN_DRI3 | PIN_FUNCINPUT | PIN_PULL_UP2K_DOWN2K);}
#define LCD_COM4_INIT() {pin_config(PIN_COM4 | PIN_DRI3 | PIN_FUNCINPUT | PIN_PULL_UP2K_DOWN2K);}
#define LCD_SEG0_INIT() {pin_config(PIN_SEG0 | PIN_DRI3 | PIN_FUNCOUTPUT | PIN_LOW);}
#define LCD_SEG1_INIT() {pin_config(PIN_SEG1 | PIN_DRI3 | PIN_FUNCOUTPUT | PIN_LOW);}
#define LCD_SEG2_INIT() {pin_config(PIN_SEG2 | PIN_DRI3 | PIN_FUNCOUTPUT | PIN_LOW);}
#define LCD_SEG3_INIT() {pin_config(PIN_SEG3 | PIN_DRI3 | PIN_FUNCOUTPUT | PIN_LOW);}
#define LCD_SEG4_INIT() {pin_config(PIN_SEG4 | PIN_DRI3 | PIN_FUNCOUTPUT | PIN_LOW);}
#define LCD_SEG5_INIT() {pin_config(PIN_SEG5 | PIN_DRI3 | PIN_FUNCOUTPUT | PIN_LOW);}
#define LCD_SEG6_INIT() {pin_config(PIN_SEG6 | PIN_DRI3 | PIN_FUNCOUTPUT | PIN_LOW);}
#define LCD_SEG7_INIT() {pin_config(PIN_SEG7 | PIN_DRI3 | PIN_FUNCOUTPUT | PIN_LOW);}

#define LCD_COM0_H()    {pin_set_value(PIN_COM0,1); pin_set_func(PIN_COM0,PIN_FUNCOUTPUT);}
#define LCD_COM0_L()    {pin_set_value(PIN_COM0,0);}
#define LCD_COM0_M()    {pin_set_func(PIN_COM0,PIN_FUNCINPUT);}

#define LCD_COM1_H()    {pin_set_value(PIN_COM1,1); pin_set_func(PIN_COM1,PIN_FUNCOUTPUT);}
#define LCD_COM1_L()    {pin_set_value(PIN_COM1,0);}
#define LCD_COM1_M()    {pin_set_func(PIN_COM1,PIN_FUNCINPUT);}

#define LCD_COM2_H()    {pin_set_value(PIN_COM2,1); pin_set_func(PIN_COM2,PIN_FUNCOUTPUT);}
#define LCD_COM2_L()    {pin_set_value(PIN_COM2,0);}
#define LCD_COM2_M()    {pin_set_func(PIN_COM2,PIN_FUNCINPUT);}

#define LCD_COM3_H()    {pin_set_value(PIN_COM3,1); pin_set_func(PIN_COM3,PIN_FUNCOUTPUT);}
#define LCD_COM3_L()    {pin_set_value(PIN_COM3,0);}
#define LCD_COM3_M()    {pin_set_func(PIN_COM3,PIN_FUNCINPUT);}

#define LCD_COM4_H()    {pin_set_value(PIN_COM4,1); pin_set_func(PIN_COM4,PIN_FUNCOUTPUT);}
#define LCD_COM4_L()    {pin_set_value(PIN_COM4,0);}
#define LCD_COM4_M()    {pin_set_func(PIN_COM4,PIN_FUNCINPUT);}

#define LCD_SEG0_H()    {pin_set_value(PIN_SEG0,1); pin_set_func(PIN_SEG0,PIN_FUNCOUTPUT);}
#define LCD_SEG0_L()    {pin_set_value(PIN_SEG0,0); pin_set_func(PIN_SEG0,PIN_FUNCOUTPUT);}
#define LCD_SEG0_I()    {pin_set_func(PIN_SEG0,PIN_FUNCINPUT);}
#define LCD_SEG0_O()    {pin_set_func(PIN_SEG0,PIN_FUNCOUTPUT);}
#define LCD_SEG0_D()    {pin_set_func(PIN_SEG0,PIN_FUNC15);}

#define LCD_SEG1_H()    {pin_set_value(PIN_SEG1,1); pin_set_func(PIN_SEG1,PIN_FUNCOUTPUT);}
#define LCD_SEG1_L()    {pin_set_value(PIN_SEG1,0); pin_set_func(PIN_SEG1,PIN_FUNCOUTPUT);}
#define LCD_SEG1_I()    {pin_set_func(PIN_SEG1,PIN_FUNCINPUT);}
#define LCD_SEG1_O()    {pin_set_func(PIN_SEG1,PIN_FUNCOUTPUT);}
#define LCD_SEG1_D()    {pin_set_func(PIN_SEG1,PIN_FUNC15);}

#define LCD_SEG2_H()    {pin_set_value(PIN_SEG2,1); pin_set_func(PIN_SEG2,PIN_FUNCOUTPUT);}
#define LCD_SEG2_L()    {pin_set_value(PIN_SEG2,0); pin_set_func(PIN_SEG2,PIN_FUNCOUTPUT);}
#define LCD_SEG2_I()    {pin_set_func(PIN_SEG2,PIN_FUNCINPUT);}
#define LCD_SEG2_O()    {pin_set_func(PIN_SEG2,PIN_FUNCOUTPUT);}
#define LCD_SEG2_D()    {pin_set_func(PIN_SEG2,PIN_FUNC15);}

#define LCD_SEG3_H()    {pin_set_value(PIN_SEG3,1); pin_set_func(PIN_SEG3,PIN_FUNCOUTPUT);}
#define LCD_SEG3_L()    {pin_set_value(PIN_SEG3,0); pin_set_func(PIN_SEG3,PIN_FUNCOUTPUT);}
#define LCD_SEG3_I()    {pin_set_func(PIN_SEG3,PIN_FUNCINPUT);}
#define LCD_SEG3_O()    {pin_set_func(PIN_SEG3,PIN_FUNCOUTPUT);}
#define LCD_SEG3_D()    {pin_set_func(PIN_SEG3,PIN_FUNC15);}

#define LCD_SEG4_H()    {pin_set_value(PIN_SEG4,1); pin_set_func(PIN_SEG4,PIN_FUNCOUTPUT);}
#define LCD_SEG4_L()    {pin_set_value(PIN_SEG4,0); pin_set_func(PIN_SEG4,PIN_FUNCOUTPUT);}
#define LCD_SEG4_I()    {pin_set_func(PIN_SEG4,PIN_FUNCINPUT);}
#define LCD_SEG4_O()    {pin_set_func(PIN_SEG4,PIN_FUNCOUTPUT);}
#define LCD_SEG4_D()    {pin_set_func(PIN_SEG4,PIN_FUNC15);}

#define LCD_SEG5_H()    {pin_set_value(PIN_SEG5,1); pin_set_func(PIN_SEG5,PIN_FUNCOUTPUT);}
#define LCD_SEG5_L()    {pin_set_value(PIN_SEG5,0); pin_set_func(PIN_SEG5,PIN_FUNCOUTPUT);}
#define LCD_SEG5_I()    {pin_set_func(PIN_SEG5,PIN_FUNCINPUT);}
#define LCD_SEG5_O()    {pin_set_func(PIN_SEG5,PIN_FUNCOUTPUT);}
#define LCD_SEG5_D()    {pin_set_func(PIN_SEG5,PIN_FUNC15);}

#define LCD_SEG6_H()    {pin_set_value(PIN_SEG6,1); pin_set_func(PIN_SEG6,PIN_FUNCOUTPUT);}
#define LCD_SEG6_L()    {pin_set_value(PIN_SEG6,0); pin_set_func(PIN_SEG6,PIN_FUNCOUTPUT);}
#define LCD_SEG6_I()    {pin_set_func(PIN_SEG6,PIN_FUNCINPUT);}
#define LCD_SEG6_O()    {pin_set_func(PIN_SEG6,PIN_FUNCOUTPUT);}
#define LCD_SEG6_D()    {pin_set_func(PIN_SEG6,PIN_FUNC15);}

#define LCD_SEG7_H()    {pin_set_value(PIN_SEG7,1); pin_set_func(PIN_SEG7,PIN_FUNCOUTPUT);}
#define LCD_SEG7_L()    {pin_set_value(PIN_SEG7,0); pin_set_func(PIN_SEG7,PIN_FUNCOUTPUT);}
#define LCD_SEG7_I()    {pin_set_func(PIN_SEG7,PIN_FUNCINPUT);}
#define LCD_SEG7_O()    {pin_set_func(PIN_SEG7,PIN_FUNCOUTPUT);}
#define LCD_SEG7_D()    {pin_set_func(PIN_SEG7,PIN_FUNC15);}


void disp_lcd_seg_init(void);
void disp_lcd_seg_config(void);
void disp_lcd_seg_update(void);
void disp_lcd_seg(void);

#ifdef __cplusplus
}
#endif


#endif

