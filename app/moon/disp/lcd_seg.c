#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "disp.h"
#include "key.h"


uint8_t lcd_cycle;
uint8_t lcd_seg_buf[LCD_COM_MAX];   //if the bit is 1, on


AT(.disp_init_seg)
void disp_lcd_seg_init(void)
{
    LCD_COM0_INIT();
    LCD_COM1_INIT();
    LCD_COM2_INIT();
    LCD_COM3_INIT();
    LCD_COM4_INIT();
    LCD_SEG0_INIT();
    LCD_SEG1_INIT();
    LCD_SEG2_INIT();
    LCD_SEG3_INIT();
    LCD_SEG4_INIT();
    LCD_SEG5_INIT();
    LCD_SEG6_INIT();
    LCD_SEG7_INIT();

    lcd_cycle = LCD_CYCLE_0H;

    lcd_seg_buf[0] = 0xFF;
    lcd_seg_buf[1] = 0xFF;
    lcd_seg_buf[2] = 0xFF;
    lcd_seg_buf[3] = 0xFF;
    lcd_seg_buf[4] = 0xFF;
}

#if LCD_SEG_REUSE
AT(.disp_seg)
void disp_lcd_seg_detect(void)
{
    LCD_SEG1_I();
    LCD_SEG2_I();
    udelay(5);

    //TODO

    LCD_SEG1_O();
    LCD_SEG2_O();
}
#endif

AT(.disp_seg)
void disp_lcd_seg_config(void)
{
    uint8_t com_num;
    uint8_t com_level;

    com_num = (lcd_cycle & LCD_COM_NUM_MASK) >> LCD_COM_NUM_SHIFT;
    com_level = (lcd_cycle & LCD_COM_LEVEL_MASK) >> LCD_COM_LEVEL_SHIFT;

    if (lcd_seg_buf[com_num] & LCD_SEG_NUM0_MASK) {
        if (com_level == LCD_LEVEL_H) {
            LCD_SEG0_L();
        } else if (com_level == LCD_LEVEL_L) {
            LCD_SEG0_H();
        }
    }

    if (lcd_seg_buf[com_num] & LCD_SEG_NUM1_MASK) {
        if (com_level == LCD_LEVEL_H) {
            LCD_SEG1_L();
        } else if (com_level == LCD_LEVEL_L) {
            LCD_SEG1_H();
        }
    }

    if (lcd_seg_buf[com_num] & LCD_SEG_NUM2_MASK) {
        if (com_level == LCD_LEVEL_H) {
            LCD_SEG2_L();
        } else if (com_level == LCD_LEVEL_L) {
            LCD_SEG2_H();
        }
    }

    if (lcd_seg_buf[com_num] & LCD_SEG_NUM3_MASK) {
        if (com_level == LCD_LEVEL_H) {
            LCD_SEG3_L();
        } else if (com_level == LCD_LEVEL_L) {
            LCD_SEG3_H();
        }
    }

    if (lcd_seg_buf[com_num] & LCD_SEG_NUM4_MASK) {
        if (com_level == LCD_LEVEL_H) {
            LCD_SEG4_L();
        } else if (com_level == LCD_LEVEL_L) {
            LCD_SEG4_H();
        }
    }

    if (lcd_seg_buf[com_num] & LCD_SEG_NUM5_MASK) {
        if (com_level == LCD_LEVEL_H) {
            LCD_SEG5_L();
        } else if (com_level == LCD_LEVEL_L) {
            LCD_SEG5_H();
        }
    }

    if (lcd_seg_buf[com_num] & LCD_SEG_NUM6_MASK) {
        if (com_level == LCD_LEVEL_H) {
            LCD_SEG6_L();
        } else if (com_level == LCD_LEVEL_L) {
            LCD_SEG6_H();
        }
    }

    if (lcd_seg_buf[com_num] & LCD_SEG_NUM7_MASK) {
        if (com_level == LCD_LEVEL_H) {
            LCD_SEG7_L();
        } else if (com_level == LCD_LEVEL_L) {
            LCD_SEG7_H();
        }
    }

}

AT(.disp_seg)
void disp_lcd_seg_update(void)
{
    lcd_seg_buf[0] = 0xFF;
    lcd_seg_buf[1] = 0xFF;
    lcd_seg_buf[2] = 0xFF;
    lcd_seg_buf[3] = 0xFF;
    lcd_seg_buf[4] = 0xFF;
}

/*   -5com-8seg-
 *       0--C0H--------------------C4M--
 *       1--C0M--C1H--------------------
 *       2--------C1M--C2H--------------
 *       3--------------C2M--C3H--------
 *       4--------------------C3M--C4H--
 *       5--C0L--------------------C4M--
 *       6--C0M--C1L--------------------
 *       7--------C1M--C2L--------------
 *       8--------------C2M--C3L--------
 *       9--------------------C3M--C4L--
 *
 *       0H -> 1H -> 2H -> 3H -> 4H -> 0L -> 1L -> 2L -> 3L -> 4L -> 0H
 *
 *
 *       if you need reuse SEG pin to do other functions, following this mode, and you can do something when the COM is M.
 *       0--C0H---------------------------------
 *       1--C0M--C1H----------------------------
 *       2--------C1M--------------------(detect)-
 *       3-------------C2H----------------------
 *       4-------------C2M--C3H-----------------
 *       5-------------------C3M---------(detect)-
 *       6-------------------------C4H----------
 *       7--C0L--------------------C4M----------
 *       8--C0M--------------------------(detect)-
 *       9--------C1L---------------------------
 *       10-------C1M--C2L----------------------
 *       11-------------C2M--------------(detect)-
 *       12-------------------C3L---------------
 *       13-------------------C3M--C4L----------
 *       14-------------------------C4M--(detect)-
 *
 *       0H -> 1H -> 1M -> 2H -> 3H -> 3M -> 4H -> 0L ->0M -> 1L -> 2L ->2M -> 3L -> 4L -> 4M -> 0H
 *
 */

AT(.disp_seg)
void disp_lcd_seg(void)
{
    switch(lcd_cycle) {
    /*0*/
    case LCD_CYCLE_0H:
        disp_lcd_seg_config();
        LCD_COM0_H();
        lcd_cycle = LCD_CYCLE_1H;
        break;

    /*1*/
    case LCD_CYCLE_1H:
        disp_lcd_seg_config();
        LCD_COM0_M();
        LCD_COM1_H();
        lcd_cycle = LCD_CYCLE_1M;
        break;

    /*2*/
    case LCD_CYCLE_1M:
        LCD_COM1_M();
#if LCD_SEG_REUSE
        disp_lcd_seg_detect();
#endif
        lcd_cycle = LCD_CYCLE_2H;
        break;

    /*3*/
    case LCD_CYCLE_2H:
        disp_lcd_seg_config();
        LCD_COM2_H();
        lcd_cycle = LCD_CYCLE_3H;
        break;

    /*4*/
    case LCD_CYCLE_3H:
        disp_lcd_seg_config();
        LCD_COM2_M();
        LCD_COM3_H();
        lcd_cycle = LCD_CYCLE_3M;
        break;

    /*5*/
    case LCD_CYCLE_3M:
        LCD_COM3_M();
#if LCD_SEG_REUSE
        disp_lcd_seg_detect();
#endif
        lcd_cycle = LCD_CYCLE_4H;
        break;

    /*6*/
    case LCD_CYCLE_4H:
        disp_lcd_seg_config();
        LCD_COM4_H();
        lcd_cycle = LCD_CYCLE_0L;
        break;

    /*7*/
    case LCD_CYCLE_0L:
        disp_lcd_seg_config();
        LCD_COM4_M();
        LCD_COM0_L();
        lcd_cycle = LCD_CYCLE_0M;
        break;

    /*8*/
    case LCD_CYCLE_0M:
        LCD_COM0_M();
#if LCD_SEG_REUSE
        disp_lcd_seg_detect();
#endif
        lcd_cycle = LCD_CYCLE_1L;
        break;

    /*9*/
    case LCD_CYCLE_1L:
        disp_lcd_seg_config();
        LCD_COM1_L();
        lcd_cycle = LCD_CYCLE_2L;
        break;

    /*10*/
    case LCD_CYCLE_2L:
        disp_lcd_seg_config();
        LCD_COM1_M();
        LCD_COM2_L();
        lcd_cycle = LCD_CYCLE_2M;
        break;

    /*11*/
    case LCD_CYCLE_2M:
        LCD_COM2_M();
#if LCD_SEG_REUSE
        disp_lcd_seg_detect();
#endif
        lcd_cycle = LCD_CYCLE_3L;
        break;

    /*12*/
    case LCD_CYCLE_3L:
        disp_lcd_seg_config();
        LCD_COM3_L();
        lcd_cycle = LCD_CYCLE_4L;
        break;

    /*13*/
    case LCD_CYCLE_4L:
        disp_lcd_seg_config();
        LCD_COM3_M();
        LCD_COM4_L();
        lcd_cycle = LCD_CYCLE_4M;
        break;

    /*14*/
    case LCD_CYCLE_4M:
        LCD_COM4_M();
#if LCD_SEG_REUSE
        disp_lcd_seg_detect();
#endif
        lcd_cycle = LCD_CYCLE_0H;
        break;
    }

    for(uint8_t i=0;i<10;i++) {
        pin_set_value(PIN_B10, 0);
        pin_set_value(PIN_B10, 1);
    }
}

