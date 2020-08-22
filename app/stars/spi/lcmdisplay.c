//*****************************************************************
//CONTROL IC:ST7735S
//MODEL TYPE:80*160

//*****************************************************************
#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "spi.h"
#include "lcmconfig.h"

extern const unsigned char  gImage_test_33[];
extern  unsigned char ascii[];
#define RED    0xF800
#define GREEN  0x07E0
#define BLUE   0x001F
#define WHITE  0xFFFF
#define BLACK  0x0000
#define GRAY25 0xADB5

#define  FONT_W  8
#define  FONT_H  16


void st7735_led_bg_open()
{
    SPI1_LCD_HIGH();
}


void st7735_led_bg_close()
{
    SPI1_LCD_LOW();
}



//##########################dealy 1ms/per circle#################
void MsDelay(unsigned int i)
  {
		unsigned int j=0;
		while(i--){
      for(j=0;j<400;j++);
    }
  }


void WriteData(unsigned char data1, uint16_t di)
  {

    uint16_t ds;
	ds=!di;	
	SPI1_CS_LOW();
	udelay(20);
	pin_set_value(PIN_B13, di);
	spi_write(data1);
	pin_set_value(PIN_B13, ds);
	//udelay(10);
	SPI1_CS_HIGH();
  }

	void PauseKey()
	{
     MsDelay(10);
  }

/*************************************************/
unsigned char ToOrd(unsigned char ch)
{
	if(ch<32)
	{
		ch=95;
	}
	else if((ch>=32)&&(ch<=47)) //(32~47)�ո�~/
	{
		ch=(ch-32)+10+62;
	}
	else if((ch>=48)&&(ch<=57))//(48~57)0~9
	{
		ch=ch-48;
	}
	else if((ch>=58)&&(ch<=64))//(58~64):~@
	{
		ch=(ch-58)+10+62+16;
	}
	else if((ch>=65)&&(ch<=126))//(65~126)A~~
	{
		ch=(ch-65)+10;
	}
	else if(ch>126)
	{		
		ch=95;
	}

	return ch;
}


/*****************************************************************/
void DispOneChar(unsigned char ord,unsigned int Xstart,unsigned int Ystart,unsigned int TextColor,unsigned int BackColor)	 // ord:0~95
{													  
   unsigned char i,j,dat;
   unsigned char *p;
   unsigned int index;
   unsigned char Xend,Yend;

    Xend=Xstart+FONT_W-1;
	Yend=Ystart+FONT_H-1;

   index = ord;

   if(index>95)	   //95:ASCII CHAR NUM
   		index=95;

   index = index*((FONT_W/8)*FONT_H);	 //���ַ�����λ��

   p = ascii;
   p = p+index;

   WriteData(0x2a,0);
   WriteData(0x00,1);
   WriteData(0x1a+Xstart,1);
   WriteData(0x00,1);
   WriteData(0x1a+Xend,1);
   
   WriteData(0x2B,0);
   WriteData(0x00,1);
   WriteData(0x01+Ystart,1);
   WriteData(0x00,1);
   WriteData(Yend,1);
   WriteData(0x2C,0);

   for(i=0;i<FONT_H;i++)
	{
	   dat=*p++;   	   
	   for(j=0;j<FONT_W;j++)
	   {
	   	   if((dat<<j)&0x80)
             {
				WriteData((unsigned char)((TextColor&0xff00)>>8),1);
		        WriteData((unsigned char)(TextColor&0x00ff),1);
             }      
           else 
             {
				WriteData((unsigned char)((BackColor&0xff00)>>8),1);
		        WriteData((unsigned char)(BackColor&0x00ff),1); 
             }
	   }
	}	
}


//####################### DispStr####################
/*
刷字符串
*str 字符串数据

Xstart Ystart 开始坐标
TextColor 字符串 颜色
BackColor 字符串 背景颜色
*/
void DispStr(unsigned char *str,unsigned int Xstart,unsigned int Ystart,unsigned int TextColor,unsigned int BackColor)
{

	while(!(*str=='\0'))
	{
		DispOneChar(ToOrd(*str++),Xstart,Ystart,TextColor,BackColor);

		if(Xstart>((ROW_X-1)-FONT_W))
		{
			Xstart=0;
		    Ystart=Ystart+FONT_H;
		}
		else
		{
			Xstart=Xstart+FONT_W;
		}

		if(Ystart>((COL_Y-1)-FONT_H))
		{
			Ystart=0;
		}
	}	
}

//####################### displayPattern####################
/*
刷颜色
x_s y_s 开始坐标
x_len  y_len 颜色块大小
dat1 dat2 颜色选择
*/
void displayPattern(uint8_t x_s,uint8_t y_s,uint8_t x_len, uint8_t y_len,unsigned char dat1,unsigned char dat2)
{
    unsigned char j,k;
	uint8_t colum,line;
	colum = 0x1a+x_s+x_len;
	line = 0x01+y_s+y_len;

	if (colum > 0x69) {
		colum = 0x69;
	}

	if (line > 0xa0) {
		line = 0xa0;
	}

				WriteData(0x2a,0);//horizontal window address 
				WriteData(0x00,1);
				WriteData((0x1a+x_s),1);//x
				WriteData(0x00,1);
				WriteData(colum,1);//(80-1)+X
				
				WriteData(0x2b,0);//vertical window address
				WriteData(0x00,1);
				WriteData((0x01+y_s),1);//y
				WriteData(0x00,1);
				WriteData(line,1);//(160-1)+X
				
				WriteData(0x2c,0);
		for(j=y_s;j<line;j++)
		 {
				for(k=x_s;k<80;k++)
				{
			   		WriteData(dat1,1);
			   		WriteData(dat2,1);
				}	
			}		 
}


//####################### initial code####################

void Init_lcd(void)
  {
		logi("Init_lcd");
		
		SPI1_LCD_LOW();	
		SPI1_RST_HIGH();
		MsDelay(5);
		SPI1_RST_LOW();
		MsDelay(50);
		SPI1_RST_HIGH();
		MsDelay(150);
		
	
		WriteData(0x11,0); //sleep out    
		
		MsDelay(120);                //ms                                                              
		
		WriteData(0x21,0); //display inversion on 
		
		WriteData(0xB1,0); //frame rate control     
		WriteData(0x05,1);   
		WriteData(0x3A,1);   
		WriteData(0x3A,1);   
		
		WriteData(0xB2,0);  //frame rate control   
		WriteData(0x05,1);   
		WriteData(0x3A,1);   
		WriteData(0x3A,1);   
		
		WriteData(0xB3,0);   //frame rate control   
		WriteData(0x05,1);   
		WriteData(0x3A,1);   
		WriteData(0x3A,1);   
		WriteData(0x05,1);   
		WriteData(0x3A,1);   
		WriteData(0x3A,1);   
		
		WriteData(0xB4,0);  //display inversion control
		WriteData(0x03,1);   
		
		WriteData(0xC0,0);  //power control1   
		WriteData(0x62,1);   
		WriteData(0x02,1);   
		WriteData(0x04,1);   
		
		WriteData(0xC1,0);  //power control2     
		WriteData(0xC0,1);   
		
		WriteData(0xC2,0);  //power control3    
		WriteData(0x0D,1);   
		WriteData(0x00,1);   
		
		WriteData(0xC3,0);   //power control4  
		WriteData(0x8D,1);   
		WriteData(0x6A,1);   
		
		WriteData(0xC4,0);   //power control5   
		WriteData(0x8D,1);   
		WriteData(0xEE,1);   
		
		WriteData(0xC5,0);  //VCOM
		WriteData(0x0E,1);   
		
		WriteData(0xE0,0); //gamma
		WriteData(0x10,1);
		WriteData(0x0E,1);
		WriteData(0x02,1);
		WriteData(0x03,1);
		WriteData(0x0E,1);
		WriteData(0x07,1);
		WriteData(0x02,1);
		WriteData(0x07,1);
		WriteData(0x0A,1);
		WriteData(0x12,1);
		WriteData(0x27,1);
		WriteData(0x37,1);
		WriteData(0x00,1);
		WriteData(0x0D,1);
		WriteData(0x0E,1);
		WriteData(0x10,1);
		
		
		WriteData(0xE1,0); //gamma
		WriteData(0x10,1);
		WriteData(0x0E,1);
		WriteData(0x03,1);
		WriteData(0x03,1);
		WriteData(0x0F,1);
		WriteData(0x06,1);
		WriteData(0x02,1);
		WriteData(0x08,1);
		WriteData(0x0A,1);
		WriteData(0x13,1);
		WriteData(0x26,1);
		WriteData(0x36,1);
		WriteData(0x00,1);
		WriteData(0x0D,1);
		WriteData(0x0E,1);
		WriteData(0x10,1);
		
		WriteData(0x3A,0); //color mode  
		WriteData(0x05,1);
		
		WriteData(0x36,0); //Memory Data Access Control    
		WriteData(0xC8,1);
		
		WriteData(0x29,0); //Display On

	    WriteData(0x2C,0);//Memory Write
		displayPattern(0,0,80,160,0x00,0x00);//黑色		
		mdelay(100);
		st7735_led_bg_open();
  }

//----------------------------------------------------------
/*
    显示一张图片
    *p 图片数据
    x_s，y_s 图片开始显示坐标
    row 图片宽度
    col 图片高度
*/
void displayPicture(const unsigned char *p,unsigned char x_s,unsigned char y_s,unsigned char row,unsigned char col)
{
   unsigned int i;
   unsigned char len,hei;
   len= 0x1a + x_s + row - 1;
   hei = 0x01 + y_s + col -1;

   //logi("len=0x%x,x_s=0x%x,row=0x%x",len,x_s,row);
   //logi("hen=0x%x,x_s=0x%x,row=0x%x",hei,y_s,col);

        WriteData(0x2a,0);//horizontal window address heng
		WriteData(0x00,1);
		WriteData((0x1a + x_s),1);
		WriteData(0x00,1);
		WriteData(len,1);

		WriteData(0x2b,0);//vertical window address shu
		WriteData(0x00,1);
		WriteData((0x01 + y_s),1);
		WriteData(0x00,1);
		WriteData(hei,1);
		WriteData(0x2c,0);
   
    for (i=0;i < row*col; i++) {
        WriteData(p[2*i],1); 
        WriteData(p[2*i + 1],1);
    }
}

void disp_frame(void)
{
	unsigned int ii,jj;
	
      	        WriteData(0x2a,0);//horizontal window address 
				WriteData(0x00,1);
				WriteData(0x1a,1);
				WriteData(0x00,1);
				WriteData(0x69,1);
				WriteData(0x2b,0);//vertical window address
				WriteData(0x00,1);
				WriteData(0x01,1);
				WriteData(0x00,1);
				WriteData(0xa0,1);
				WriteData(0x2c,0); 
      for(ii=0;ii<160;ii++)
      {
   	     	if(ii==0) 
   	     	{
   	     		for(jj=0;jj<80;jj++)	 {WriteData(0xff,1);WriteData(0xff,1);}
   	     	}
   	     	else if(ii== (159))
   	     	{
   	     		for(jj=0;jj<80;jj++)	 {WriteData(0xff,1);WriteData(0xff,1);}
   	     	}
    	      else
    	      {
    	      	for(jj=0;jj<80;jj++)	 
    	      	{
    	      		if(jj==0 )        	{WriteData(0xff,1);WriteData(0xff,1);}
    	      		else if(jj==79)    {WriteData(0xff,1);WriteData(0xff,1);}   	      		
    	      		else             		{WriteData(0x00,1);WriteData(0x00,1);}
    	      	}	
    	      }
      }
}

unsigned char hello_str[]={"HELLO"};
unsigned char sl_str[]={"SmartLink"};

void st7735_led_init(void)
{
	 logi("st7735_led_init");
	
	Init_lcd();		
	//displayPattern(10,10,10,10,0xff,0xff);//白色
	//PauseKey();
	//displayPattern(20,20,40,40,0xf8,0x00);//红色
    //PauseKey();
	//displayPattern(10,60,10,10,0x07,0xe0);//绿色
     displayPicture(gImage_test_33,20,10,40,22);
     DispStr(hello_str,20,80,WHITE,BLACK);
     DispStr(sl_str,5,96,BLACK,GRAY25);
}
