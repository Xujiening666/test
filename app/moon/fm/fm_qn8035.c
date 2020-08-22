#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "fm_api.h"
#include "task_main.h"


#if	FM_QN8035_EN

//if antenna match circuit is used a inductor，macro USING_INDUCTOR will be set to 1
#define USING_INDUCTOR                  1

#define INVERSE_IMR						1

//if using san noise floor as CCA algorithm,macro SCAN_NOISE_FLOOR_CCA will be set to 1
#define SCAN_NOISE_FLOOR_CCA 	        1
//if using pilot as CCA algorithm,macro PILOT_CCA will be set to 1
#define PILOT_CCA					    1

//长天线配置:
uint8_t  qnd_PreNoiseFloor,qnd_NoiseFloor;
//bool qn8035_online;
//短天线配置:
//UINT8 _xdata qnd_PreNoiseFloor = 35,qnd_NoiseFloor = 35;

//16级音量
AT(.fm_qn8035_rodata_seg)
const uint8_t volume_8035table[17] = {
    0,4,8,12,14,16,18,20,22,24,26,28,30,34,38,43,47
};


AT(.fm_qn8035_seg)
uint8_t QN8035_ReadReg(uint8_t adr)
{
    uint8_t value;

    twi_write(FM_TWI_ID, FM_TWI_ADDR, &adr, 1);
    twi_read(FM_TWI_ID, FM_TWI_ADDR, &value, 1);

    return value;
}

AT(.fm_qn8035_seg)
void QN8035_WriteReg(uint8_t adr, uint8_t value)
{
    uint8_t data[2];

    data[0] = adr;
    data[1] = value;

    twi_write(FM_TWI_ID, FM_TWI_ADDR, data, 2);
}

AT(.fm_qn8035_seg)
void QNF_SetCh(uint16_t start,uint16_t stop,uint8_t step)
{
    uint8_t temp;

	 start = FREQ2CHREG(start);
	 stop = FREQ2CHREG(stop);
	//writing lower 8 bits of CCA channel start index
	QN8035_WriteReg(CH_START, (uint8_t)start);
	//writing lower 8 bits of CCA channel stop index
	QN8035_WriteReg(CH_STOP, (uint8_t)stop);
	//writing lower 8 bits of channel index
	QN8035_WriteReg(CH, (uint8_t)start);
	//writing higher bits of CCA channel start,stop and step index
	temp = (uint8_t) ((start >> 8) & CH_CH);
	temp |= ((uint8_t)(start >> 6) & CH_CH_START);
	temp |= ((uint8_t) (stop >> 4) & CH_CH_STOP);
	temp |= (step << 6);
	QN8035_WriteReg(CH_STEP, temp);
}

AT(.fm_qn8035_seg)
void QN8035_Delay(uint16_t n)
{
    while (n--)
        asm("nop");
}

AT(.fm_qn8035_seg)
void QNF_SetMute(uint8_t On)
{
	QN8035_WriteReg(REG_DAC, On ? 0x1B:0x10);
}

#if SCAN_NOISE_FLOOR_CCA
AT(.fm_qn8035_seg)
uint8_t QND_ScanNoiseFloor(uint16_t start,uint16_t stop)
{
	uint8_t regValue;
	uint8_t timeOut = 255; //time out is 2.55S

	QN8035_WriteReg(CCA_SNR_TH_1,0x00);
	QN8035_WriteReg(CCA_SNR_TH_2,0x05);
	QN8035_WriteReg(0x40,0xf0);
	//config CCS frequency rang by step 200KHZ
	QNF_SetCh(start,stop,2);
/*
	QN8035_WriteReg(CH_START,0x26);
	QN8035_WriteReg(CH_STOP,0xc0);
	QN8035_WriteReg(CH_STEP,0xb8);
*/
	//enter CCA mode,channel index is decided by internal CCA
	QN8035_WriteReg(SYSTEM1,0x12);
    while(1) {
        regValue = QN_IIC_read(SYSTEM1);
        //if it seeks a potential channel, the loop will be quited
        if((regValue & CHSC) == 0) break;
        mdelay(1);   //delay 10ms mdelay(50);
        //if it was time out,chip would not normally work.
        if((timeOut--) == 0) {
    		QN8035_WriteReg(0x40,0x70);
    		return 0;
        }
    }
	QN8035_WriteReg(0x40,0x70);
	qnd_NoiseFloor = QN_IIC_read(0x3f);
	if(((qnd_PreNoiseFloor-qnd_NoiseFloor) > 2) ||((qnd_NoiseFloor-qnd_PreNoiseFloor) > 2)) {
		qnd_PreNoiseFloor = qnd_NoiseFloor;
	}
	//TRACE("NF:%d,timeOut:%d\n",qnd_NoiseFloor,255-timeOut);
	return 1;
}
#endif

AT(.fm_qn8035_seg)
void QND_RXSetTH(void)
{
	uint8_t rssi_th;

	rssi_th = qnd_PreNoiseFloor + 8-28 ;   //越小台多
	///increase reference PLL charge pump current.
	QN8035_WriteReg(REG_REF,0x7a);
	//NFILT program is enabled
	QN8035_WriteReg(0x1b,0x78);
	//using Filter3
	QN8035_WriteReg(CCA1,0x75);
	//setting CCA IF counter error range value(768).
	QN8035_WriteReg(CCA_CNT2,0x03);  //0x01	  大台多 1-5
#if PILOT_CCA
	QN8035_WriteReg(PLT1,0x00);
#endif
	//selection the time of CCA FSM wait SNR calculator to settle:20ms
	//0x00:	    20ms(default)
	//0x40:	    40ms
	//0x80:	    60ms
	//0xC0:	    100m
	//    QNF_SetRegBit(CCA_SNR_TH_1 , 0xC0, 0x00);
	//selection the time of CCA FSM wait RF front end and AGC to settle:20ms
	//0x00:     10ms
	//0x40:     20ms(default)
	//0x80:     40ms
	//0xC0:     60ms
	//    QNF_SetRegBit(CCA_SNR_TH_2, 0xC0, 0x40);
	//    QNF_SetRegBit(CCA, 30);  //setting CCA RSSI threshold is 30
	QN8035_WriteReg(CCA_SNR_TH_2,0x85);
	QN8035_WriteReg(CCA,(QN_IIC_read(CCA)&0xc0)|rssi_th);
#if PILOT_CCA
	QN8035_WriteReg(CCA_SNR_TH_1,0x88); //setting SNR threshold for CCA
#else
	QN8035_WriteReg(CCA_SNR_TH_1,8); //小台多 8-12 //setting SNR threshold for CCA  9
#endif
}

AT(.fm_qn8035_seg)
void qn8035_init(void)
{
	/*Gobal variable initial*/
    qnd_PreNoiseFloor = 40;
    qnd_NoiseFloor = 40;
    QN8035_WriteReg(0x00, 0x81);
	mdelay(25);
    QNF_SetMute(1);
	QN8035_WriteReg(0x58,0x93);
    for (uint32_t i=0;i<100;i++) {
        if (MODE_FM != mode_get_sta()) {
            break;
        }
        mdelay(20);
    }
	//Following is where change the input clock frequency.

    QN8035_WriteReg(0x00, 0x81);
	mdelay(1);

	/*********User sets chip working clock **********/
	//Following is where change the input clock wave type,as sine-wave or square-wave.
	//default set is 32.768KHZ square-wave input.
	QN8035_WriteReg(0x01,QND_SINE_WAVE_CLOCK);
    //QN8035_WriteReg(0x01,QND_DIGITAL_CLOCK);

	QN8035_WriteReg(0x58,0x93);
	//mdelay(200);
	//Following is where change the input clock frequency.

	QN8035_WriteReg(XTAL_DIV0, QND_XTAL_DIV0);
	QN8035_WriteReg(XTAL_DIV1, QND_XTAL_DIV1);
	QN8035_WriteReg(XTAL_DIV2, QND_XTAL_DIV2);
	/********User sets chip working clock end ********/
    mdelay(50);
	QN8035_WriteReg(0x54, 0x47);//mod PLL setting
	//select SNR as filter3,SM step is 2db
	QN8035_WriteReg(0x19, 0xc4);
	QN8035_WriteReg(0x33, 0x9e);//set HCC and SM Hystersis 5db
	QN8035_WriteReg(0x2d, 0xd6);//notch filter threshold adjusting
	QN8035_WriteReg(0x43, 0x10);//notch filter threshold enable
	QN8035_WriteReg(0x47,0x39);
	//QN8035_WriteReg(0x57, 0x21);//only for qn8035B test
	//enter receiver mode directly
	QN8035_WriteReg(0x00, 0x11);
	//Enable the channel condition filter3 adaptation,Let ccfilter3 adjust freely
	QN8035_WriteReg(0x1d,0xa9);
	QN8035_WriteReg(0x4f, 0x40);//dsiable auto tuning
	QN8035_WriteReg(0x34,SMSTART_VAL); ///set SMSTART
	QN8035_WriteReg(0x35,SNCSTART_VAL); ///set SNCSTART
	QN8035_WriteReg(0x36,HCCSTART_VAL); ///set HCCSTART
	QNF_SetMute(1);

    // dac_channel_on(FM_IIC_CHANNEL, FADE_ON);
}

AT(.fm_qn8035_seg)
void qn8035_powerdown(void)
{
    //QND_SetSysMode(0);
    //QNF_SetMute(1);
    logi("qn8035_powerdown:");
    QN8035_WriteReg(SYSTEM1, 0x21);
    mdelay(5);

    //dac_channel_off(FM_IIC_CHANNEL, FADE_ON);
}

AT(.fm_qn8035_seg)
void QND_TuneToCH(uint16_t ch)
{
	uint8_t reg;
	//increase reference PLL charge pump current.
	QN8035_WriteReg(REG_REF,0x7a);

	/********** QNF_RXInit ****************/
	QN8035_WriteReg(0x1b,0x70);	//Let NFILT adjust freely
	QN8035_WriteReg(0x2c,0x52);
	QN8035_WriteReg(0x45,0x50);	//Set aud_thrd will affect ccfilter3's tap number
	QN8035_WriteReg(0x40,0x70);	//set SNR as SM,SNC,HCC MPX
	QN8035_WriteReg(0x41,0xca);
	/********** End of QNF_RXInit ****************/
#if INVERSE_IMR
	reg = QN_IIC_read(CCA)&~0x40;
	if((ch == 9340)||(ch == 9390)||(ch == 9530)||(ch == 9980)||(ch == 10480)) {
		reg |= 0x40;	// inverse IMR.
	} else {
		reg &= ~0x40;
	}
	QN8035_WriteReg(CCA, reg);
#endif
	QNF_SetMute(1);
	QNF_SetCh(ch,ch,1);
	//enable CCA mode with user write into frequency
	QN8035_WriteReg(0x00, 0x13);
#if USING_INDUCTOR
	//Auto tuning
	QN8035_WriteReg(0x4f, 0x80);
	reg = QN_IIC_read(0x4f);
	reg >>= 1;
	QN8035_WriteReg(0x4f, reg);
#endif
	///avoid the "POP" noise.
	//QND_Delay(CH_SETUP_DELAY_TIME);
	//QND_Delay(500);
    for (uint32_t i=0;i<25;i++) {
        if (MODE_FM != mode_get_sta()) {
            break;
        }
        mdelay(20);
    }
	///decrease reference PLL charge pump current.
	QN8035_WriteReg(REG_REF,0x70);
	QNF_SetMute(0);
}

AT(.fm_qn8035_seg)
bool QND_RXValidCH(uint16_t freq)
{
	uint8_t regValue;
	uint8_t timeOut;
	uint8_t isValidChannelFlag;
    //UINT8 snr,snc,temp1,temp2;
#if PILOT_CCA
	uint8_t snr,readCnt,stereoCount=0;
#endif
	freq = freq * 10;

#if SCAN_NOISE_FLOOR_CCA
	switch(freq) {
	case 8750:
		QND_ScanNoiseFloor(8750,8800);
		QND_RXSetTH();
		break;
	case 8810:
		QND_ScanNoiseFloor(8810,9200);
		QND_RXSetTH();
		break;
	case 9210:
		QND_ScanNoiseFloor(9210,9600);
		QND_RXSetTH();
		break;
	case 9610:
		QND_ScanNoiseFloor(9610,10000);
		QND_RXSetTH();
		break;
	case 10010:
		QND_ScanNoiseFloor(10010,10400);
		QND_RXSetTH();
		break;
	case 10410:
		QND_ScanNoiseFloor(10410,10800);
		QND_RXSetTH();
		break;
	default:
		//QND_Delay(350);
		break;
	}
#endif

	QNF_SetCh(freq,freq,1);
#if USING_INDUCTOR
	//Auto tuning
	QN8035_WriteReg(0x00, 0x11);
	QN8035_WriteReg(0x4f, 0x80);
	regValue = QN_IIC_read(0x4f);
	regValue = (regValue >> 1);
	QN8035_WriteReg(0x4f, regValue);
#endif
	//entering into RX mode and CCA mode,channels index decide by CCA.
	QN8035_WriteReg(0x00, 0x12);
	timeOut = 20;  // time out is 100ms
	while(1) {
		regValue = QN_IIC_read(SYSTEM1);
		//if it seeks a potential channel, the loop will be quited
		if((regValue & CHSC) == 0) break;
		mdelay(10);   //delay 5ms
		//if it was time out,chip would not normally work.
		if((timeOut--) == 0) return 0;
	}
	//reading out the rxcca_fail flag of RXCCA status
	isValidChannelFlag = (QN_IIC_read(STATUS1) & RXCCA_FAIL ? 0:1);
	if(isValidChannelFlag) {
#if PILOT_CCA
        for (uint32_t i=0;i<5;i++) {
            mdelay(20);
            if (MODE_FM != mode_get_sta()) {
                break;
            }
            mdelay(20);
        }
		snr = QN_IIC_read(SNR);
		if(snr> 25) return 1;
		for(readCnt=10;readCnt>0;readCnt--) {
			mdelay(10);
			stereoCount += ((QN_IIC_read(STATUS1) & ST_MO_RX) ? 0:1);
			if(stereoCount >= 3) return 1;
		}
#else
		return 1;
#endif
	}
	return 0;
}

AT(.fm_qn8035_seg)
bool qn8035_set_fre(uint16_t fre)
{
	if (QND_RXValidCH(fre)) {
        QND_TuneToCH(fre*10);
        return TRUE;
    }
    return FALSE;
}

AT(.fm_qn8035_seg)
uint8_t qn8035_seek(uint16_t freq)
{
    QNF_SetMute(1);
    //logi("qn8035_seek freq==%d\n",freq);
    if (QND_RXValidCH(freq)) {
        QND_TuneToCH(freq*10);
        return TRUE;
    }
    return FALSE;
}

AT(.fm_qn8035_seg)
void QND_SetVol(uint8_t vol)
{
    uint8_t sVol;
    uint8_t regVal;
    sVol=vol*3+2;
    regVal = QN_IIC_read(VOL_CTL);
    regVal = (regVal&0xC0)|(sVol/6)|((5-(sVol%6))<<3);
    QN8035_WriteReg(VOL_CTL,regVal);
}

AT(.fm_qn8035_seg)
void QN8035_RXConfigAudio(uint8_t option)
{
	uint8_t regVal;
	option = option>47 ? 47:option;
	regVal = QN8035_ReadReg(VOL_CTL);
	regVal = (regVal&0xc0)|(option/6)|((5-option%6)<<3);
	QN8035_WriteReg(VOL_CTL,regVal);
}

AT(.fm_qn8035_seg)
void qn8035_set_vol(uint8_t volume)
{
    if(volume == 0) {
        QNF_SetMute(1);
    } else {
        QNF_SetMute(0);
    }
    QN8035_RXConfigAudio(volume_8035table[volume]);
}

AT(.fm_qn8035_seg)
uint8_t qn8035_online(void)
{
    uint8_t id;
    for(uint8_t i = 0; i < 3; i++) {
        id = QN_IIC_read(0x06) & 0xFC;
        logi("qn8035id=%x", id);
        if(id == 0x84) {
            return 1;
        }
        mdelay(20);
    }
    return 0;
}

AT(.fm_qn8035_seg)
void qn8035_setch(uint8_t db)
{
	db = db;
    QND_RXSetTH();
}

#endif
