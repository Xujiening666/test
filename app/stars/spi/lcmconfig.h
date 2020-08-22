#ifndef __LCMCONFIG_H__
#define __LCMCONFIG_H__

/*#define dataport P1      //优蓝测试板接口 
sbit CS1=P3^0;
sbit RES=P3^4;
sbit RS=P3^1;
sbit SCLK=P1^6;
sbit SDA=P1^7;

sbit STEP_KEY =P2^0;
sbit NEXT_KEY =P3^7;
sbit DN_KEY =  P3^5;
sbit UP_KEY =  P3^6;*/

/*
#define dataport P0    //U-LEADER测试版接口
sbit CS1=P3^5;
sbit RES=P3^3;
sbit RS=P3^4;
sbit E_RD=P3^6;
sbit RW=P3^7;
sbit STEP_KEY =P1^4;
sbit NEXT_KEY =P1^5;
sbit DN_KEY =  P1^6;
sbit UP_KEY =  P1^7;
*/

#define COL_Y      160
#define ROW_X      80


void st7735_led_init(void);
void st7735_led_bg_open();
void st7735_led_bg_close();

#endif
