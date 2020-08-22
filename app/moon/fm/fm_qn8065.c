#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "fm_api.h"


#if FM_QN8065_EN


#if (FM_CLK_SEL == FM_CLK_12M)
//crystal clock is 12MHZ
#define QN8065_XTAL_DIV0_VAL                   	0x6e
#define QN8065_XTAL_DIV1_VAL                   	0x69
#define QN8065_XTAL_DIV2_VAL                   	0x66
#endif


//if using pilot as CCA algorithm,macro QN8065_PILOT_CCA will be set to 1
#define QN8065_PILOT_CCA 		            0// 1

#if (FM_CLK_SEL == FM_CLK_12M)
//if using scan noise floor as CCA algorithm,macro QN8065_SCAN_NOISE_FLOOR_CCA will be set to 1
#define QN8065_SCAN_NOISE_FLOOR_CCA			0
//if want improving some special frequency SNR, macro IMPROVING_SNR will be set to 1
#define IMPROVING_SNR                       0
#else
#define QN8065_SCAN_NOISE_FLOOR_CCA		    1
#define IMPROVING_SNR                       1
#endif
uint8_t QN8065_PreNoiseFloor = 40,QN8065_NoiseFloor = 40;


//16级音量
AT(.fm_qn8065_rodata_seg)
const uint8_t volume_8065table[17] = {
    0,4,8,12,14,16,18,20,22,24,26,28,30,34,38,43,47
};

AT(.fm_qn8065_seg)
void QN8065_Delay(uint16_t n)
{
    while (n--)
        asm("nop");
}

AT(.fm_qn8065_seg)
uint8_t QN8065_ReadReg(uint8_t adr)
{
    uint8_t value;

    twi_write(FM_TWI_ID, FM_TWI_ADDR, &adr, 1);
    twi_read(FM_TWI_ID, FM_TWI_ADDR, &value, 1);

    return value;
}

AT(.fm_qn8065_seg)
void QN8065_WriteReg(uint8_t adr, uint8_t value)
{
    uint8_t data[2];

    data[0] = adr;
    data[1] = value;

    twi_write(FM_TWI_ID, FM_TWI_ADDR, data, 2);
}

AT(.fm_qn8065_seg)
uint16_t QN8065_FREQ2CHREG(uint16_t freq)
{
    return 	(freq-6000)/5;
}

/**********************************************************************
void QN8065_SetMute(uint8_t On)
**********************************************************************
Description: on or off mute chip
Parameters:
    On: 1:on mute chip;0:off mute chip
Return Value:
    None
**********************************************************************/
AT(.fm_qn8065_seg)
void QN8065_SetMute(uint8_t On)
{
	//mute or unmute by holding DAC
    //and set low voltage regulator output as 0.9v
    QN8065_WriteReg(QN8065_REG_REF, On?0x14:0x10);
}

/**********************************************************************
void QN8065_SetCh(uint16_t start,uint16_t stop,uint8_t step)
**********************************************************************
Description: set channel frequency
Parameters:
    freq:  channel frequency to be set,frequency unit is 10KHZ
Return Value:
    None
**********************************************************************/
AT(.fm_qn8065_seg)
void QN8065_SetCh(uint16_t start,uint16_t stop,uint8_t step)
{
    uint8_t temp;
    //TRACE("start:%d,stop:%d,step:%d \n",start,stop, step);
    start = QN8065_FREQ2CHREG(start);
    //fored the stop is bigger than start
    stop = QN8065_FREQ2CHREG(stop);
	//writing higher bits of channel frequency,CCA channel start,
	//stop and step index at first
	temp = (uint8_t) ((start >> 8) & QN8065_CH_CH);
	temp |= ((uint8_t)(start >> 6) & QN8065_CH_CH_START);
	temp |= ((uint8_t) (stop >> 4) & QN8065_CH_CH_STOP);
	temp |= (step << 6);
	QN8065_WriteReg(QN8065_CH_STEP, temp);
	//writing lower 8 bits of CCA channel start index
	QN8065_WriteReg(QN8065_CH_START, (uint8_t)start);
	//writing lower 8 bits of CCA channel stop index
	QN8065_WriteReg(QN8065_CH_STOP, (uint8_t)stop);
	//writing lower 8 bits of channel index
	QN8065_WriteReg(QN8065_CH, (uint8_t)start);
}

/**********************************************************************
void qn8065_init()
**********************************************************************
Description: Initialize device to make it ready to have all functionality ready for use.
Parameters:
    None
Return Value:
    None
**********************************************************************/
AT(.fm_qn8065_seg)
void qn8065_init(void)
{
    logi("%s", __func__);

    QN8065_WriteReg(QN8065_SYSTEM1, 0x81);
    mdelay(50);

    /*********User sets chip working clock **********/
    //Following is where change the input clock wave type,as sine-wave or square-wave.

#if ( (FM_CLK_SEL == FM_CLK_32K_HOSC) || (FM_CLK_SEL == FM_CLK_24M_HOSC)\
    || (FM_CLK_SEL == FM_CLK_12M) || (FM_CLK_SEL == FM_CLK_LOSC) )
    QN8065_WriteReg(QN8065_CCA,QN8065_DIGITAL_CLOCK);    //方波
#else
    QN8065_WriteReg(QN8065_CCA,QN8065_SINE_WAVE_CLOCK ); //正弦波
#endif
    //Following is where change the input clock frequency.

    QN8065_WriteReg(QN8065_XTAL_DIV0, QN8065_XTAL_DIV0_VAL);
    QN8065_WriteReg(QN8065_XTAL_DIV1, QN8065_XTAL_DIV1_VAL);
    QN8065_WriteReg(QN8065_XTAL_DIV2, QN8065_XTAL_DIV2_VAL);

    mdelay(50);

    /********User sets chip working clock end ********/
    QN8065_WriteReg(QN8065_CCA1, 0x96);
    QN8065_WriteReg(QN8065_CCOND1, 0xb6);
    ///select SNR for SM,SNC,HCC MPX
    QN8065_WriteReg(QN8065_SMSTART,0x80|QN8065_SMSTART_VAL); //set SMSTART
    QN8065_WriteReg(QN8065_SNCSTART,0x80|QN8065_SNCSTART_VAL); //set SNCSTART
    QN8065_WriteReg(QN8065_HCCSTART,0x80|QN8065_HCCSTART_VAL); //set HCCSTART
    //enter receiver mode directly
    QN8065_WriteReg(QN8065_SYSTEM1, 0x11);
    QN8065_SetMute(1);
}

/**********************************************************************
void qn8065_tune2ch(uint16_t ch)
**********************************************************************
Description: Tune to the specific channel.
Parameters:
	ch:Set the frequency (10kHz) to be tuned,
eg: 101.30MHz will be set to 10130.
Return Value:
	None
**********************************************************************/
AT(.fm_qn8065_seg)
void qn8065_tune2ch(uint16_t ch)
{
	uint8_t sysClk32kFlag = 0;
	uint8_t stereoFlag = 0;
	uint8_t reg;
#if !FM_SEEK_50K_STEP
    ch = ch * 10;
#endif
    QN8065_SetMute(1);

	if (QN8065_ReadReg(QN8065_XTAL_DIV0) == 0x01)
	{
		sysClk32kFlag = 1;
		stereoFlag = (QN8065_ReadReg(QN8065_STATUS1) & QN8065_ST_MO_RX) ? 0:1;
	}
#if IMPROVING_SNR
	//improving some special stations snr when working clock is 32768Hz
	if (ch == 10030 || ch == 10090 || ch == 10100 || ch == 10110
		|| ch == 10150 || ch == 10160 || ch == 10220)
	{
		//frequency is added 50k only system clock is 32K.
		if (sysClk32kFlag)
		{
			QN8065_WriteReg(QN8065_XTAL_DIV2, 0x67);
			//add 50K
			ch += 5;
		}
	}
		//restore XTAL_DIV2 right value if the station is not a special station
	else
	{
		QN8065_WriteReg(QN8065_XTAL_DIV2, QN8065_XTAL_DIV2_VAL);
	}
#endif
    QN8065_WriteReg(QN8065_REG_FLL,0x84); //enable 2k speed
    //enter idle mode and disable CCA, with user write into frequency
    QN8065_WriteReg(QN8065_SYSTEM1, 0x01);
    //step is 100K.
    QN8065_SetCh(ch, ch, 1);
    //enter rx mode and disable CCA, with user write into frequency
    QN8065_WriteReg(QN8065_SYSTEM1, 0x11);
    QN8065_Delay(QN8065_CH_SETUP_DELAY_TIME);
    QN8065_WriteReg(QN8065_REG_FLL, 0x04);//disable 2k speed
	if (sysClk32kFlag)
	{
		reg = QN8065_ReadReg(QN8065_CCA1);
		//disable bypass 32k spur rejection filter if is stereo
		if (stereoFlag)
		{
			reg &= ~0x20;
		}
		//enable bypass 32k spur rejection filter if is mono
		else
		{
			reg |= 0x20;
		}
		QN8065_WriteReg(QN8065_CCA1, reg);
	}
    QN8065_SetMute(0);
}

#if QN8065_SCAN_NOISE_FLOOR_CCA
 /***********************************************************************
Description: scan a noise floor from 87.5M to 108M by step 200K
Parameters:
Return Value:
 1: scan a noise floor successfully.
 0: chip would not normally work.
**********************************************************************/
AT(.fm_qn8065_seg)
uint8_t QN8065_ScanNoiseFloor(uint16_t start,uint16_t stop)
{
    uint8_t regValue;
    uint8_t timeOut = 255; //time out is 2.55S

    QN8065_WriteReg(QN8065_CCA_TIME,0x00);
    //config CCS from start frequency to  stop frequency by step 200KHZ
    QN8065_SetCh(start,stop,2);
    //enable CCS
    QN8065_WriteReg(QN8065_CCA2, 0x95);
    //enter CCA mode,channel index is decided by internal CCA
    QN8065_WriteReg(QN8065_SYSTEM1,0x12);
    while(1)
    {
        regValue = QN8065_ReadReg(QN8065_SYSTEM1);
        //if it seeks a potential channel, the loop will be quited
        if((regValue & QN8065_CHSC) == 0) break;
        mdelay(50);
        //if it was time out,chip would not normally work.
        if((timeOut--) == 0)
        {
            //disable CCS while exit
            QN8065_WriteReg(QN8065_CCA2,0x85);
            return 0;
        }
    }
    //disable CCS while exit
    QN8065_WriteReg(QN8065_CCA2,0x85);
    QN8065_NoiseFloor = QN8065_ReadReg(QN8065_CCS_NOISE)&0x7f;
    if(((QN8065_PreNoiseFloor-QN8065_NoiseFloor) > 2) ||((QN8065_NoiseFloor-QN8065_PreNoiseFloor) > 2))
    {
        QN8065_PreNoiseFloor = QN8065_NoiseFloor;
    }
    //TRACE("%d-%d noiseFloor:%d,timeElapsed:%dms\n",start,stop,QN8065_NoiseFloor,(255-timeOut)*10);
    return 1;
}
#endif

/***********************************************************************
void QN8065_RXSetTH(void)
***********************************************************************
Description: Setting the threshold value of automatic scan channel

Return Value:
  	None
***********************************************************************/
AT(.fm_qn8065_seg)
void QN8065_RXSetTH(void)
{
#if IMPROVING_SNR
    uint8_t rssiTH;
    rssiTH = (QN8065_PreNoiseFloor-28);//(QN8065_PreNoiseFloor-28)+3;
    //restore XTAL_DIV2 right value if the station is not a special station
    QN8065_WriteReg(QN8065_XTAL_DIV2, QN8065_XTAL_DIV2_VAL);
    //set IF counter error range value is 256 = 4*64
    QN8065_WriteReg(QN8065_CCA_CNT, 8);//4
#else
    //set IF counter error range value is 256 = 4*64
    QN8065_WriteReg(QN8065_CCA_CNT, 10);//8
#endif
    //set CCA time
    QN8065_WriteReg(QN8065_CCA_TIME, QN8065_CCA_NAGC_40MS|QN8065_CCA_NSNR_20MS|QN8065_CCA_NRSSI_10MS);
#if IMPROVING_SNR
    //set RSSI threshold for CCA
    QN8065_WriteReg(QN8065_CCA, (QN8065_ReadReg(QN8065_CCA)&~0x3f)|rssiTH);
#else
    //set RSSI threshold for CCA
    QN8065_WriteReg(QN8065_CCA, (QN8065_ReadReg(QN8065_CCA)&~0x3f) | 3);
#endif
    //set QN8065_SNR threshold for CCA
#if QN8065_PILOT_CCA
	QN8065_WriteReg(QN8065_CCA_SNR_TH, (QN8065_ReadReg(QN8065_CCA_SNR_TH)&~0x3f)|8);  //pilot CCA the QN8065_SNR thershold is 8
#else
	QN8065_WriteReg(QN8065_CCA_SNR_TH, (QN8065_ReadReg(QN8065_CCA_SNR_TH)&~0x3f)|9); 	//normall CCA the QN8065_SNR thereshold is 9
#endif
	//TRACE("rssiTH=%d, snrTH=%d\n", rssiTH,snrTH);
}


AT(.fm_qn8065_rodata_seg)
const uint16_t qn8065_start[] =
{
    8750,  8810,  9010,  9210,
    9410,  9610,  9810,  10010,
    10210, 10410, 10610
};

AT(.fm_qn8065_rodata_seg)
const uint16_t qn8065_stop[] =
{
    8800,  9000,  9200,  9400,
    9600,  9800,  10000, 10200,
	10400, 10600, 10800
};
//#pragma constseg=default
/***********************************************************************
INT8 QN8065_RXValidCH(uint16_t freq);
***********************************************************************
Description: to validate a ch (frequency)(if it's a valid channel)
Freq: specific channel frequency, unit: 10Khz
  eg: 108.00MHz will be set to 10800.
Return Value:
  0: it's not a valid channel
  1: it's a valid channel
 -1: chip does not normally work.
***********************************************************************/
AT(.fm_qn8065_seg)
uint8_t QN8065_RXValidCH(uint16_t freq)
{
    uint8_t regValue;
    uint8_t timeOut;
    uint8_t isValidChannelFlag;
    uint8_t CalValue;
#if QN8065_PILOT_CCA
    uint8_t snr,readCnt,stereoCount=0;
#endif

#if (FM_CLK_SEL != FM_CLK_12M)
#if QN8065_SCAN_NOISE_FLOOR_CCA
#if 1
    for(uint8_t i=0; i<11; i++) {
        if(freq == qn8065_start[i]) {
            QN8065_ScanNoiseFloor(qn8065_start[i], qn8065_stop[i]);
            QN8065_RXSetTH();
            break;
        }
    }
#else
    switch(freq)
    {
    case 8750:
        QN8065_ScanNoiseFloor(8750,8800);
        QN8065_RXSetTH();
        break;
    case 8810:
        QN8065_ScanNoiseFloor(8810,9000);
        QN8065_RXSetTH();
        break;
    case 9010:
        QN8065_ScanNoiseFloor(9010,9200);
        QN8065_RXSetTH();
        break;
    case 9210:
        QN8065_ScanNoiseFloor(9210,9400);
        QN8065_RXSetTH();
        break;
    case 9410:
        QN8065_ScanNoiseFloor(9410,9600);
        QN8065_RXSetTH();
        break;
    case 9610:
        QN8065_ScanNoiseFloor(9610,9800);
        QN8065_RXSetTH();
        break;
    case 9810:
        QN8065_ScanNoiseFloor(9810,10000);
        QN8065_RXSetTH();
        break;
    case 10010:
        QN8065_ScanNoiseFloor(10010,10200);
        QN8065_RXSetTH();
        break;
    case 10210:
        QN8065_ScanNoiseFloor(10210,10400);
        QN8065_RXSetTH();
        break;
    case 10410:
        QN8065_ScanNoiseFloor(10410,10600);
        QN8065_RXSetTH();
        break;
    case 10610:
        QN8065_ScanNoiseFloor(10610,10800);
        QN8065_RXSetTH();
        break;
    default:
        //mdelay(500); //this delay can improve user display effect
        break;
    }
#endif
#endif
    //enter idle mode and disable CCA, with user write into frequency
    QN8065_WriteReg(QN8065_SYSTEM1, 0x01);
    //only support 100K and 200K step.
    QN8065_SetCh(freq,freq+10,2);

#else
    QN8065_RXSetTH();

    //enter idle mode and disable CCA, with user write into frequency
    QN8065_WriteReg(QN8065_SYSTEM1, 0x01);

    //only support 100K and 200K step.
    QN8065_SetCh(freq,freq+5,1);
#endif

    QN8065_WriteReg(0x2D, 0x80);
    QN8065_WriteReg(0x2D, 0x00);
    // delay about 700us
    // there is no delay us function, so I delay 1ms here
    mdelay(1);
    CalValue = QN8065_ReadReg(0x2D);
    QN8065_WriteReg(0x2D, (CalValue | 0x40));

    //entering into RX mode and CCA mode,channels index decide by CCA.
    QN8065_WriteReg(QN8065_SYSTEM1, 0x12);
#if (FM_CLK_SEL != FM_CLK_12M)
    mdelay(50);
#endif
    timeOut = 60;  // time out is 300ms
    while(1)
    {
        regValue = QN8065_ReadReg(QN8065_SYSTEM1);
        //if it seeks a potential channel, the loop will be quited
        if((regValue & QN8065_CHSC) == 0) break;
        mdelay(5);
        //if it was time out,chip would not normally work.
        if((timeOut--) == 0) return 0;
    }
    //reading out the rxcca_fail flag of RXCCA status
    isValidChannelFlag = (QN8065_ReadReg(QN8065_STATUS1) & QN8065_RXCCA_FAIL ? 0:1);
    QN8065_WriteReg(0x2D, CalValue);
    if(isValidChannelFlag)
    {
        QN8065_SetMute(0);      //搜到真台后不MUTE，程序里让其自动播放一小段时间
#if QN8065_PILOT_CCA
        //set pilot threshold as 1/256
        QN8065_WriteReg(QN8065_CCOND2,0x91);
        mdelay(500);
        snr = QN8065_ReadReg(QN8065_SNR);
        logi("freq:%d,snr:%d", freq,snr);
#if (FM_CLK_SEL != FM_CLK_12M)
        if(snr< 25)  //25
#else
        if(snr< 13)  //13
#endif
        {
            isValidChannelFlag = 0;
            for(readCnt=10;readCnt>0;readCnt--)
            {
                mdelay(10);
                stereoCount += ((QN8065_ReadReg(QN8065_STATUS1) & QN8065_ST_MO_RX) ? 0:1);
                if(stereoCount >= 3)
                {
                    isValidChannelFlag =1;
                    break;
                }
            }
        }
        //restore pilot threshold as 2/256
        QN8065_WriteReg(QN8065_CCOND2,0x92);
        return (isValidChannelFlag);
#else
        return 1;
#endif
    }
    return 0;
}

/************************************************************************
void QN8065_RXConfigAudio(uint8_t volume )
*************************************************************************
Description: config audio
Parameters:
  volume:control the volume gain,range : 0~47(-47db~0db)
Return Value:
    None
**********************************************************************/
AT(.fm_qn8065_seg)
void QN8065_RXConfigAudio(uint8_t option)
{
	uint8_t regVal;

	option = option>47 ? 47:option;
	regVal = QN8065_ReadReg(QN8065_VOL_CTL);
	regVal = (regVal&0xc0)|(option/6)|((5-option%6)<<3);
	QN8065_WriteReg(QN8065_VOL_CTL,regVal);
}

AT(.fm_qn8065_seg)
void qn8065_set_vol(uint8_t volume)
{
    if(volume == 0) {
        QN8065_SetMute(1);
    } else {
        QN8065_SetMute(0);
    }
    QN8065_RXConfigAudio(volume_8065table[volume]);
}

AT(.fm_qn8065_seg)
void qn8065_off(void)
{
    QN8065_WriteReg(QN8065_SYSTEM1, 0x20);
    mdelay(50);
}

AT(.fm_qn8065_seg)
uint8_t qn8065_seek(uint16_t freq)
{
    QN8065_SetMute(1);
#if FM_SEEK_50K_STEP
    return QN8065_RXValidCH(freq);
#else
    return QN8065_RXValidCH(freq*10);//qn8035 step frequency unit is 10KHZ
#endif
}

AT(.fm_qn8065_seg)
uint8_t qn8065_online(void)
{
    uint8_t id;
    for(uint8_t i = 0; i < 3; i++) {
        id = QN8065_ReadReg(0x06) & 0xFC;
        logi("qn8065id=%x", id);
        if(id == CHIPID_QN8065) {
            return 1;
        }
        mdelay(10);
    }
    return 0;
}

AT(.fm_qn8065_seg)
uint16_t GetIFCount(void)
{
	uint8_t r_temp;
	uint8_t IF_CNT1;
	uint16_t IF_CNT;
    IF_CNT1 = QN8065_ReadReg(0x1D);
	IF_CNT = QN8065_ReadReg(0x1E);
    IF_CNT = IF_CNT<<8;
    IF_CNT = IF_CNT + IF_CNT1;
	r_temp = QN8065_ReadReg(0x11);
	r_temp= r_temp & 0xfd;
	QN8065_WriteReg(0x11, r_temp);
	r_temp= r_temp | 0x02;
	QN8065_WriteReg(0x11, r_temp);
    return IF_CNT;
}

#endif


