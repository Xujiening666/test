#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_

/***************************************************************************
 *               scene mode config
 ***************************************************************************/
#define MODE_BT_EN                          0       //蓝牙模式
#define MODE_MUSIC_EN                       1       //本地音乐模式
#define MODE_RECORD_EN                      1      //录音模式
#define MODE_FM_EN                          0       //FM收音模式
#define MODE_LINEIN_EN                      0       //音频输入模式
#define MODE_USBDEV_EN                      1       //USB从机模式
#define MODE_SPDIF_EN                       0       //光纤输入模式
#define MODE_CLOCK_EN                       0       //时钟模式
#define MODE_POWEROFF_EN                    1       //软关机模式
#define MODE_CHARGE_EN                      1       //充电模式
#define MODE_DISKUPDATE_EN                  0       //设备升级固件模式

/***************************************************************************
 *               audio config
 ***************************************************************************/
/* volume config */
#define VOLUME_MAX                          100     //最大音量值（NOTE:最大只能设置成100）
#define VOLUME_DEFAULT                      70      //默认音量值
#define VOLUME_MIN                          30      //开机最小音量值
#define VOLUME_STEP                         5       //加减音量的步进大小

/* audio effects for playback */
#define AUDIO_PLAYBACK_EQ                   0       //音乐（播放）通路EQ功能
#define AUDIO_PLAYBACK_DRC                  0       //音乐（播放）通路DRC功能
#define AUDIO_PLAYBACK_TSPS                 0       //音乐（播放）通路变调功能
#define AUDIO_PLAYBACK_VSS                  0       //音乐（播放）通路3D音效功能
#define AUDIO_PLAYBACK_SPECTRUM             0       //音乐（播放）频谱
#define AUDIO_PLAYBACK_DR                   0       //音乐（播放）延时功能
#define AUDIO_PLAYBACK_TBVC                 0       //音乐（播放）等响功能

/* audio effects for mic */
#define AUDIO_MIC_EQ                        0       //MIC通路EQ功能
#define AUDIO_MIC_DRC                       0       //MIC通路DRC功能
#define AUDIO_MIC_ECHO                      0       //MIC通路ECHO(回声)功能
#define AUDIO_MIC_REVERB                    0       //MIC通路REVERB(混响)功能
#define AUDIO_MIC_TSPS                      0       //MIC通路变调功能

#define AUDIO_PR_MIX_SPECTRUM               0
#define AUDIO_PR_MIX_DECORR                 0

#define AUDIO_KARAOKE_MODE                  1       //卡拉OK功能
#define AUDIO_KARAOKE_USE_FS                0       //移频防啸叫功能
#define AUDIO_KARAOKE_USE_NHS               0       //NHS防啸叫功能
#define AUDIO_KARAOKE_USE_DUCKER            0       //闪避功能
#define AUDIO_KARAOKE_AUXTRACK_EN           0       //辅助通路播放功能（此功能可以不打断音乐播放播放sbc提示音）

#define AUDIO_KARAOKE_START_WITH_REVERB     0       //开机默认打开混响
#define AUDIO_KARAOKE_START_WITH_ECHO       0       //开机默认打开回声

#define AUDIO_ADJUST_HPLR_LINEOUTLR         0       //HP和lineout左右声道独立调节音量功能

/*
 * 通话输出设备选择：0-扬声器（speaker); 1-耳机（headset)
 */
#define AUDIO_PHONE_OUTPUT_SEL              0

#define AUDIO_OUT_DEVICE_SEL                1       //0:i2s; 1:audiocodec
#define AUDIO_IN_DEVICE_SEL                 1       //0:i2s; 1:audiocodec

/* mute功放配置
 * 0: 不使用mute功放功能; 1: 使用gpio mute功放; 2: 使用micbias引脚mute功放
 */
#define AUDIO_USE_MUTE                      1
#define AUDIO_MUTE_PIN                      (PIN_D1)
/* high level is mute or low level is mute */
#define AUDIO_MUTE_PIN_STATE                (PIN_LOW)

/* DAC digital gain:-80~30; 0 is 0dB, 0.75dB/step */
#if (AUDIO_PHONE_OUTPUT_SEL == 1)
#define AUDIO_DAC_GAIN                      -8
#else
#define AUDIO_DAC_GAIN                      0
#endif

/* DMIC(digital microphone) config */
#define AUDIO_DMICDATA0_EN                  0
#define AUDIO_DMICDATA1_EN                  0
#define AUDIO_DMIC_CLK_PIN_SEL              0       //0:PB5; 1:PC5; 2:PB3
#define AUDIO_DMICDATA0_PIN_SEL             0       //0:PB6; 1:PC6; 2:PB4
#define AUDIO_DMICDATA1_PIN_SEL             0       //0:PB7; 1:PC7;

/* DMIC maximum digital gain:-80 ~ 30; 0 is 0dB, 0.75dB/step */
#define AUDIO_DMICDATA0F_DIGITAL_GAIN       0       //DMICDATA0 falling edge
#define AUDIO_DMICDATA0R_DIGITAL_GAIN       0       //DMICDATA0 rising edge
#define AUDIO_DMICDATA1F_DIGITAL_GAIN       0       //DMICDATA1 falling edge
#define AUDIO_DMICDATA1R_DIGITAL_GAIN       0       //DMICDATA1 rising edge

/* AMIC maximum digital gain:-80 ~ 30; 0 is 0dB, 0.75dB/step */
#define AUDIO_AMIC0_DIGITAL_GAIN            0
#define AUDIO_AMIC1_DIGITAL_GAIN            0
#define AUDIO_AMIC2_DIGITAL_GAIN            0

/* LINEIN maximum digital gain:-80 ~ 30; 0 is 0dB, 0.75dB/step */
#define AUDIO_LINEIN0_DIGITAL_GAIN          0
#define AUDIO_LINEIN1_DIGITAL_GAIN          0

/*
 * Default analog mic gain
 * AMIC(analog microphone) analog gain range :0 ~ 14;
 *   0:-4.5dB; 1:-3dB; 2:-1.5dB; 3:0dB; 4:1.5dB; 5:3dB; 6:4.5dB; 7:6dB;
 *   8:24dB; 9:27dB; 10:30dB; 11:33dB; 12:36dB; 13:39dB; 14:42dB;
 */
#define AUDIO_AMIC0_ANALOG_GAIN             14
#define AUDIO_AMIC1_ANALOG_GAIN             14
#define AUDIO_AMIC2_ANALOG_GAIN             14

/*
 * Default linein gain
 * linein analog gain range :0 ~ 7;
 *   0:-4.5dB; 1:-3dB; 2:-1.5dB; 3:0dB; 4:1.5dB; 5:3dB; 6:4.5dB; 7:6dB;
 */
#define AUDIO_LINEIN0_ANALOG_GAIN           7
#define AUDIO_LINEIN1_ANALOG_GAIN           7

/*
 * analog mic connection mode: 0:differential mode; 1:single-end mode
 */
#define AUDIO_AMIC0_SINGLE_END              1
#define AUDIO_AMIC1_SINGLE_END              1
#define AUDIO_AMIC2_SINGLE_END              1

/*
 * linein connection mode: 0:differential mode; 1:single-end mode
 */
#define AUDIO_LINEIN0_SINGLE_END            1
#define AUDIO_LINEIN1_SINGLE_END            1

/*
 * Default input source config
 * 0:AMIC0(analog microphone 0); 1:AMIC1; 2:AMIC2;
 * 3:DMIC0F(digital microphone 0 falling edge);
 * 4:DMIC0R(digital microphone 0 rising edge);
 * 5:AMIC0/1; 6:AMIC0/2; 7:AMIC1/2;
 * 8:DMIC0F_R(digital microphone 0 falling and rising edge)
 * 9:linein0, 10:linein1, 11:linein0/1, 12:amic all
 */
#define AUDIO_REC_INPUT_SEL                 12       //record and call input select
#define AUDIO_LINEIN_MODE_INPUT_SEL         11      //linein mode input select
#define AUDIO_FM_MODE_INPUT_SEL             5       //fm mode input select

/***************************************************************************
 *               bt config
 ***************************************************************************/
#define BT_BACKGROUND_EN                    0       //蓝牙后台（全模式支持蓝牙）功能
#define BT_PHONE_EN                         0       //蓝牙通话功能
#define BT_SPP_EN                           0       //SPP功能
#define BT_BLE_EN                           1       //BLE功能
#define BT_HID_EN                           0       //HID功能
#define BT_ADDR_USE_RANDOM                  0       //蓝牙设备地址使用随机值功能
#define BT_FCC_TEST_EN                      0       //FCC测试功能
#define BT_VOLUME_EN                        0       //音量与手机同步功能
#define BT_RECONNECT_SCAN_EN                0       //回连时可被搜索到功能
#define BT_SIRI_EN                          0       //苹果设备的siri功能
#define BT_BUFFER_SIZE                      180     //蓝牙utc

#define BT_EWS_EN                           0       //EWS功能
#if BT_EWS_EN
#undef BT_ADDR_USE_RANDOM
#define BT_ADDR_USE_RANDOM                  0
#endif

#define BT_ADDR_DEFAULT                     {0xF0,0xF1,0xF2,0xF3,0xF4,0xF5}     //蓝牙设备默认地址
#define BT_NAME_DEFAULT                     "SL0000"                            //蓝牙设备默认名字

#define BLE_ADDR_USE_RANDOM                 1                                   //BLE设备地址使用随机值功能
#define BLE_ADDR_DEFAULT                    {0xEA,0xEB,0xEC,0xED,0xEE,0xEF}     //BLE设备默认地址
#define BLE_NAME_DEFAULT                    "RV100" //BLE设备默认名字

#define BT_RFPOWER_FORCE_EN                 1       //RF发送功率固定
#define BT_RFPOWER_FORCE_VAL                19      //RF发送功率固定的档位，范围：0~18，功率值：(-34+2*BT_RFPOWER_FORCE_VAL) dbm

#define BT_MAXPOWER_SET_EN                  1       //RF发送最大功率限制（在BT_RFPOWER_FORCE_EN是0时有效）
#define BT_MAXPOWER_SET_VAL                 18      //RF发送最大功率的档位，范围：0~18，功率值：(-34+2*BT_MAXPOWER_SET_VAL) dbm

/*
* 回连时间说明：
* 在RECONNECT_SCAN_EN是0时，每次回连用时5.5s，
* 在RECONNECT_SCAN_EN是1时，每次回连用时11s。
*/
#define BT_TIMEOUT_RECONNECT_TIMES          20      //掉线回连次数
#define BT_POWERON_RECONNECT_TIMES          3       //上电回连次数

/***************************************************************************
 *               music config
 ***************************************************************************/
#define MUSIC_FOLDER_SELECT_EN              0       //文件夹选择功能
#define MUSIC_FAST_SKIP_EN                  1       //快进快退功能
#define MUSIC_REC_FOLDER_IGNORE             1       //忽略录音文件夹
#define MUSIC_NUM_STEP10                    0       //上下首时文件编号加减10
#define MUSIC_MUTE_FOR_FAST                 1       //快进快退时静音
#define MUSIC_DEVICE_SWITCH                 1       //0：U盘或SD卡内循环播放；1：自动切换U盘/SD卡播放
#define MUSIC_METADATA                      0       //metadata(ID3)信息获取

/*
 * 如果宏AUDIO_KARAOKE_MODE是1，仅支持WAV和MP3。
 */
#define MUSIC_WAV                           1
#define MUSIC_MP3                           1
#define MUSIC_WMA                           1
#define MUSIC_FLAC                          1
#define MUSIC_AAC                           1
#define MUSIC_APE                           1
#define MUSIC_OGG                           1
#define MUSIC_SBC                           0

/*
 * 0：保存本地音乐播放信息（曲目、时间）到RTC RAM，在宏PMU_VCC_RTC_ALWAYS_ON是1时有效，且在电池不断电时有效；
 * 1：保存本地音乐播放信息（曲目）到flash，在每个文件开始播放时写操作；
 * 2：保存本地音乐播放信息（曲目、时间）到flash，在软关机时写操作。
 */
#define MUSIC_INFO_SAVE_MODE                0

/***************************************************************************
 *               record config
 ***************************************************************************/
#define RECORD_LINEIN_FUNC                  0        //linein模式下录音; 0:disable; 1:录linein; 2:录mic(仅AUDIO_KARAOKE_MODE使能时有效); 3:录mic和linein混音(仅AUDIO_KARAOKE_MODE使能时有效)
#define RECORD_FM_FUNC                      0        //FM模式下录音; 0:disable; 1:录fm; 2:录mic(仅AUDIO_KARAOKE_MODE使能时有效); 3:录mic和fm混音(仅AUDIO_KARAOKE_MODE使能时有效)
#define RECORD_STREAM_TYPE                  0        //录音数据流保存类型： 0:文件系统（U盘/SD卡）保存; 1: spi flash保存
#define RECORD_NEW_DEVICE_FIRST             1        //0:不自动切换最新插入的设备； 1:自动切换最新插入的设备
#define RECORD_AUTO_PLAY                    1        //0:不自动播放录音文件； 1:自动播放录音文件
#define RECORD_PLAY_LOOP                    0        //0:全部循环播放; 1:单曲循环播放
#define RECORD_FORMAT_SEL                   0        //录音编码格式： 0:adpcm; 1:mp2; 2:sbc; 3:dump
#define RECORD_FILE_DIR                     "record" //录音文件夹名
#define RECORD_FILE_PREFIX                  "REC"    //录音文件名前缀
#define RECORD_FILE_COUNT_BITS              5        //录音文件序号支持多少位（e.g 支持到万位则配5)
#define RECORD_FILE_SUM_COUNT               19       //RECORD_FILE_DIR + RECORD_FILE_COUNT_BITS + RECORD_FILE_COUNT_BITS + 1 ("/") + 4 (.mp3)
#define RECORD_FILE_BIT_RATE                64000      //MP2,MP3 编码比特率
#define RECORD_EQ_SEL                       0        //0:none 1:pop 2:vocal
#define RECORD_MP2_HEAD_SIZE                28       //mp2文件头大小
#define TURN_ON_AUTO_REC                    0        //开机是否自动录音

/***************************************************************************
 *               linein config
 ***************************************************************************/
#define LINEIN_ALL_MODE_SUPPORT             0       //linein全模式功能，全模式下可播放linein(仅AUDIO_KARAOKE_MODE使能时有效)
#define LINEIN_DETECT_EN                    1       //借用linein插入/拔出做耳机检测
#define LINEIN_DETECT_PIN_NUM               PIN_C4  //耳机检测gpio


#if LINEIN_ALL_MODE_SUPPORT
#undef MODE_LINEIN_EN
#define MODE_LINEIN_EN                      0
#endif
/*
#if !MODE_LINEIN_EN
#undef LINEIN_DETECT_EN
#define LINEIN_DETECT_EN                    0
#endif
*/



/***************************************************************************
 *               fm config
 ***************************************************************************/
#define FM_CLK_SEL                          2       //0:32K_HOSC(32K方波); 1:HOSC(24M方波); 2:12M(方波); 3:LOSC(方波); 4:32768 OSC(32.768K正弦波)
#define FM_CLK_PIN_SEL                      5       //0:none; 1:PB12; 2:PB1; 3:PB8; 4:PB13; 5:PC4; 6:PD0;
#define FM_TWI_SEL                          1       //1:twi1; 2:twi2
#define FM_TO_CODEC                         1       //0:FM声音不经过主控，直接硬件直连播放；1:FM声音经过codec内部通路到DAC播放

#define FM_RDA5807_EN                       0
#define FM_QN8065_EN                        1
#define FM_QN8035_EN                        0
#define FM_BK1080_EN                        0

/***************************************************************************
 *               sd config
 ***************************************************************************/
#define SD_EN                               1       //SD卡
#define SD_DETECT_EN                        0       //SD检测
#define SD_D0_PIN_SEL                       0       //0:PB0; 1:PB3;
#define SD_CLK_PIN_SEL                      0       //0:PB1; 1:PB6;
#define SD_CMD_PIN_SEL                      0       //0:PB2; 1:PB4;
#define SD_CLKDIV                           2       //0:--; 1:24MHz; 2:12MHz; 4:6MHz; 8:3MHz; 16:1.5MHz; 32:750KHz; 64:375KHz;

#define SD_DM_SHARE_EN                      0       //SD_D0与USB_DM共用PB3（不支持USB从机模式）

#if !SD_EN
#undef SD_DETECT_EN
#undef SD_DM_SHARE_EN
#define SD_DETECT_EN                        0
#define SD_DM_SHARE_EN                      0
#endif

#define CHECK_SD_SN                         1
#define SD_SN_CID_0                         0x12345653
#define SD_SN_CID_1                         0x44000000
#define SD_SN_CID_2                         0x0046f034
#define SD_SN_CID_3                         0x4a014269

/***************************************************************************
 *               usb config
 ***************************************************************************/
#define USB_EN                              1       //USB
#define USB_DETECT_EN                       1       //USB检测
#define USB_DISK_EN                         1       //U盘（播放、录音）
#define USB_AUDIO_EN                        0       //USB声卡
#define USB_AUDIO_MIC_EN                    0       //使用这个控制时USB_AUDIO_EN务必为1, 该开关为0表示使用的是声卡以及USB MIC的复合设备:  ,  1:表示仅仅USB MIC
#define USB_AUDIO_VOLUME_SYNCHRONIZE        1       //0:音量不与PC同步；1:音量与PC同步调节
#define USB_AUDIO_ALL_MODE_SUPPORT          0       //usbaudio全模式功能，全模式下可使用usbaudio播放和录音(仅AUDIO_KARAOKE_MODE使能时有效)
#define USB_MSC_EN                          1       //USB大容量存储

#if USB_AUDIO_ALL_MODE_SUPPORT
#undef MODE_USBDEV_EN
#define MODE_USBDEV_EN                      0
#undef MODE_DISKUPDATE_EN
#define MODE_DISKUPDATE_EN                  0
#endif

#if (USB_AUDIO_EN == 0)
#undef USB_AUDIO_MIC_EN
#define USB_AUDIO_MIC_EN                    0
#endif

#if USB_AUDIO_MIC_EN                                //单独MIC的情况下不支持存储
#undef USB_MSC_EN
#define USB_MSC_EN                          0
#endif

#define USB_DM_PIN_SEL                      0       //0:PB3
#define USB_DP_PIN_SEL                      0       //0:PB4

#if !USB_EN
#undef USB_DETECT_EN
#undef SD_DM_SHARE_EN
#undef USB_DISK_EN
#undef USB_AUDIO_EN
#undef USB_MSC_EN
#undef MODE_USBDEV_EN
#define USB_DETECT_EN                       0
#define SD_DM_SHARE_EN                      0
#define USB_DISK_EN                         0
#define USB_AUDIO_EN                        0
#define USB_MSC_EN                          0
#define MODE_USBDEV_EN                      0
#endif

#if (!MODE_USBDEV_EN && !USB_AUDIO_ALL_MODE_SUPPORT)
#undef USB_AUDIO_EN
#undef USB_MSC_EN
#define USB_AUDIO_EN                        0
#define USB_MSC_EN                          0
#endif

/***************************************************************************
 *               eq config
 ***************************************************************************/
/*
 * 在宏MODE_USBDEV_EN、USB_AUDIO_EN、USB_MSC_EN是1时有效
 */
#define EQ_REALTIME                         0       //连接EQ工具调试EQ
#define EQ_ALL_MODE_SUPPORT                 0       //全模式支持连接EQ工具调试EQ
#define EQ_DEVICE_SEND_TO_PC                1       //主动把EQ配置发送给PC

#if (EQ_REALTIME==0 && EQ_ALL_MODE_SUPPORT==0)
#undef EQ_DEVICE_SEND_TO_PC
#define EQ_DEVICE_SEND_TO_PC                0
#endif

#if !USB_MSC_EN
#undef EQ_REALTIME
#define EQ_REALTIME                         0
#endif

/***************************************************************************
 *               twi config
 ***************************************************************************/
#define TWI1_EN                             0       //TWI1模块
#define TWI1_SCL_PIN_SEL                    0       //0:PB0; 1:PD0;
#define TWI1_SDA_PIN_SEL                    0       //0:PB2; 1:PD1;
#define TWI1_SD_SHARE_EN                    0       //TWI1与SD共用PB0/PB2
#if !TWI1_EN
#undef TWI1_SD_SHARE_EN
#define TWI1_SD_SHARE_EN                    0
#endif

#define TWI2_EN                             0       //TWI2模块
#define TWI2_SCL_PIN_SEL                    2       //0:PB3; 1:PB12; 2:PD1; 3:--; 4:PB6
#define TWI2_SDA_PIN_SEL                    2       //0:PB4; 1:PB13; 2:PD0; 3:--; 4:PB7
#define TWI2_SD_SHARE_EN                    0       //TWI2与SD共用PB3/PB4
#define TWI2_DM_SHARE_EN                    0       //TWI2_SCL与USB_DM共用PB3
#if !TWI2_EN
#undef TWI2_SD_SHARE_EN
#undef TWI2_DM_SHARE_EN
#define TWI2_SD_SHARE_EN                    0
#define TWI2_DM_SHARE_EN                    0
#endif

/***************************************************************************
 *               spi config
 ***************************************************************************/
#define SPI0_EN                             0       //SPI0模块
#define SPI0_CS_PIN_SEL                     0       //0:PB7
#define SPI0_CLK_PIN_SEL                    0       //0:PB0
#define SPI0_MOSI_PIN_SEL                   0       //0:PB1
#define SPI0_MISO_PIN_SEL                   0       //0:PB2

#define SPI1_EN                             0       //SPI1模块
#define SPI1_MODE_SEL                       1       //0:使用gpio软件模拟; 1:使用spi1硬件模块
#define SPI1_DIRECT_SEL                     1       //0:四线模式:CS/CLK/MOSI/MISO,支持读/写; 1:三线模式:CS/CLK/MOSI,支持读/写(spiflash的SO通过电阻接到SI)
#define SPI1_CS_PIN_SEL                     1       //0:--; 1:PB5
#define SPI1_CLK_PIN_SEL                    1       //0:--; 1:PB6
#define SPI1_MOSI_PIN_SEL                   1       //0:--; 1:PB7
#define SPI1_MISO_PIN_SEL                   0       //0:--; 1:PB8; 2:PB10; (仅在四线模式时用到)

#define SPI_FLASH_TONE_SEL                  0       //0:内置spiflash; 1:外部spiflash（使用SPI0）; 2:外部spiflash（使用SPI1）
#define SPI_FLASH_AUXTRACK_SEL              2       //0:内置spiflash; 1:外部spiflash（使用SPI0）; 2:外部spiflash（使用SPI1）
/* 外部spi-flash容量：SPI0支持最大1.5Mbyte，SPI1支持最大16Mbyte */
#define SPI_FLASH_SIZE                      (1*1024*1024) //外置flash容量(单位:byte)

/***************************************************************************
 *               timer config
 ***************************************************************************/
#define TIMER0_EN                           0       //定时器0模块
#define TIMER0_PERIOD                       5000    //us

#define TIMER1_EN                           0        //定时器1模块 //自动关机定时器
#define TIMER1_PERIOD                       600000000//us 10分钟

#define TIMER2_EN                           0       //定时器2模块
#define TIMER2_PERIOD                       100000  //us

#define TIMER3_EN                           0       //定时器3模块
#define TIMER3_PERIOD                       100000  //us

/***************************************************************************
 *               pmw config
 ***************************************************************************/
#define PWM_EN                              0       //PWM模块

#define PWM_CH0_EN                          0       //PWM通道0
#define PWM_CH1_EN                          0       //PWM通道1
#define PWM_CH2_EN                          0       //PWM通道2
#define PWM_CH3_EN                          0       //PWM通道3
#define PWM_CH4_EN                          0       //PWM通道4
#define PWM_CH5_EN                          0       //PWM通道5

#define PWM_CH0_PIN_SEL                     1       //0:--; 1:PB5; 2:PB12
#define PWM_CH1_PIN_SEL                     1       //0:--; 1:PB6; 2:PB13
#define PWM_CH2_PIN_SEL                     1       //0:--; 1:PB7
#define PWM_CH3_PIN_SEL                     1       //0:--; 1:PB8; 2:PB10;
#define PWM_CH4_PIN_SEL                     1       //0:--; 1:PB9; 2:PB11; 3:PD1;
#define PWM_CH5_PIN_SEL                     1       //0:--; 1:PD0;

/***************************************************************************
 *               i2s config
 ***************************************************************************/
#define I2S0_EN                             0       //I2S模块
#define I2S0_BCLK_PIN_SEL                   0       //0:PB7;
#define I2S0_LRCK_PIN_SEL                   0       //0:PB6;
#define I2S0_TX_EN                          1
#define I2S0_TX_PIN_SEL                     0       //0:PB8; 1:--; 2:--; 3:PB10
#define I2S0_RX_EN                          0
#define I2S0_RX_PIN_SEL                     1       //0:PB2; 1:PB10;
#define I2S0_MCLK_EN                        0
#define I2S0_MCLK_PIN_SEL                   1       //0:PB5; 1:PB11;
#define I2S0_MCLK_FACTOR                    256     //mclk分频比（mclk频率是采样率的多少倍）
#define I2S0_FORMAT                         0       //0:标准I2S; 1:左对齐模式（left-justified）; 2:右对齐模式（right-justified）
#define I2S0_SLOT_WIDTH                     1       //0:16; 1:32; (LRCK周期 = slot_num * slot_width * BCLK周期)
#define I2S0_SLOT_NUM                       1       //0:1-slot, 1:2-slot; 2:4-slot; 3:8-slot; 4:16-slot;(2ch 固定配成2-slot即可)
#define I2S0_SAMPLE_RESOLUTION              3       //采样精度（采样精度必须小于等于slot_width) 0:16bit; 1:20bit; 2:24bit; 3:32bit
#define I2S0_USE_WM8978                     0       //0:不使用WM8978芯片； 1：I2S接WM8978芯片，使用TWI1控制； 2：I2S接WM8978芯片，使用TWI2控制
#define I2S0_USE_TAS5711                    0       //0:不使用TAS5711芯片； 1：I2S接TAS5711芯片，使用TWI1控制； 2：I2S接TAS5711芯片，使用TWI2控制

/***************************************************************************
 *               spdif config
 ****************************************************************************/
#define SPDIF_EN                            0       //SPDIF模块
#define SPDIF_PIN_SEL                       0       //0:PD0; 1:PD1

/***************************************************************************
 *               uart config
 ***************************************************************************/
#define UART0_EN                            1       //UART0模块
#define UART0_BAUDRATE                      115200 //波特率
#define UART0_SCLK_FREQ                     48000000
#define UART0_TX_PIN_SEL                    8       //0:PB1; 1:PB3; 2:PB6; 3:PB8; 4:PB9; 5:PD0; 6:--; 7:--; 8:PB11; 9:PB13;
#define UART0_RX_PIN_SEL                    3       //0:PB4; 1:PB10; 2:PD1; 3:--; 4:PB12;

#define UART1_EN                            0       //UART1模块
#define UART1_BAUDRATE                      3000000  //波特率
#define UART1_HWFLOWCTL                     UART_HWCONTROL_NONE //流控
#define UART1_SCLK_FREQ                     48000000
#define UART1_FIFO_EN                       0
#define UART1_DMA_EN                        0
#define UART1_CTS_PIN_SEL                   5       //0:PB4; 1:--; 2:--; 3:--; 4:--; 5:PB13
#define UART1_RTS_PIN_SEL                   5       //0:PB3; 1:--; 2:--; 3:--; 4:--; 5:PB12
#define UART1_TX_PIN_SEL                    5       //0:--; 1:PD0; 2:--; 3:--; 4:--; 5:PB10
#define UART1_RX_PIN_SEL                    5       //0:--; 1:PD1; 2:--; 3:--; 4:--; 5:PB11

#define UART2_EN                            0       //UART2模块
#define UART2_BAUDRATE                      115200  //波特率
#define UART2_HWFLOWCTL                     UART_HWCONTROL_NONE //流控
#define UART2_SCLK_FREQ                     48000000
#define UART2_FIFO_EN                       0
#define UART2_DMA_EN                        0
#define UART2_CTS_PIN_SEL                   0       //0:PB13
#define UART2_RTS_PIN_SEL                   0       //0:PB12
#define UART2_TX_PIN_SEL                    0       //0:PB0; 1:PB3
#define UART2_RX_PIN_SEL                    0       //0:PB2; 1:PB4

/***************************************************************************
 *               tone config
 ***************************************************************************/
#define TONE_EN                             1       //提示音总开关
#define TONE_VOLUME                         40      //提示音音量

#define TONE_WELCOME_EN                     0       //开机提示音
#define TONE_BT_MODE_EN                     0       //蓝牙模式提示音
#define TONE_MUSIC_MODE_EN                  0       //音乐模式提示音
#define TONE_CLOCK_MODE_EN                  0       //clock模式提示音
#define TONE_FM_MODE_EN                     0       //FM模式提示音
#define TONE_LINEIN_MODE_EN                 0       //linein模式提示音
#define TONE_PC_MODE_EN                     0       //PC模式提示音
#define TONE_POWER_OFF_EN                   0       //关机提示音
#define TONE_RECORDING_MODE_EN              0       //录音模式提示音
#define TONE_CONNECTED_EN                   0       //蓝牙连接提示音
#define TONE_DISCONNECTED_EN                0       //蓝牙断开连接提示音
#define TONE_INCOMING_NUM_EN                0       //来电报号提示音
#define TONE_CALL_EN                        0       //来电铃声
#define TONE_ALARM_EN                       0       //闹钟提示音
#define TONE_MAX_VOLUME_EN                  0       //最大音量提示音
#define TONE_MIN_VOLUME_EN                  0       //最小音量提示音
#define TONE_LOW_POWER_EN                   0       //低电量提示音
#define TONE_BT_NEXT_EN                     0       //蓝牙模式下一首提示音
#define TONE_BT_PREV_EN                     0       //蓝牙模式上一首提示音
#define TONE_MUSIC_NEXT_EN                  0       //音乐模式下一首提示音
#define TONE_MUSIC_PREV_EN                  0       //音乐模式上一首提示音

#define SUNCHIP_DEVICE_UNBINDER             1       //设备解绑
#define SUNCHIP_LOW_POWER_EN                1       //低电
#define SUNCHIP_MUISC_MODE                  1       //音乐模式
#define SUNCHIP_NO_FILES                    1       //没有文件
#define SUNCHIP_POWER_ON                    1       //开机
#define SUNCHIP_RECORD_MODE                 1       //录音模式
#define SUNCHIP_SHUTDOWN                    1       //关机

/***************************************************************************
 *               rtc config
 ***************************************************************************/
/*
 * 如果MODE_CLOCK_EN是1，请保持RTC_EN是1；
 * 如果BAT_EXIT是1，请保持RTC_EN是1，基础系统用到了RTC的秒中断；
 * RTC模块的功耗特别低，不必在意模块常开。
 */
#define RTC_EN                              1       //RTC模块
#define RTC_CLOCK_EN                        1       //时钟
#define RTC_ALARM0_EN                       0       //闹钟0，可设置在一定时间（例如10秒）后闹铃响
#define RTC_ALARM1_EN                       0       //闹钟1，可设置在某个时间点（例如07:05:30）闹铃响，可分别配置周一/二/三/四/五/六/日
/*
#if (MODE_CLOCK_EN == 0)
#undef RTC_CLOCK_EN
#undef RTC_ALARM0_EN
#undef RTC_ALARM1_EN
#define RTC_CLOCK_EN                        0
#define RTC_ALARM0_EN                       0
#define RTC_ALARM1_EN                       0
#endif
*/
#define RTC_DCXO_CUR                        0x18
#define RTC_DCXO_LP_EN                      1
#define RTC_XO32K_EN                        0

/***************************************************************************
 *               key config
 ***************************************************************************/
#define KEY_AD_EN                           1       //AD按键
#define KEY_IO_EN                           0       //IO按键
#define KEY_IR_EN                           0       //红外遥控按键
#define KEY_TOUCH_EN                        0       //触摸按键
#define KEY_CODING_EN                       0       //编码开关旋钮
#define KEY_UART_EN                         0       //从UART接收按键
#define KEY_PMU_EN                          1       //ONOFF用作p/p键
#define KEY_PMU_MULTFUC_EN                  0       //ONOFF用作多功能键（包括单击、双击、长按）

/***************************************************************************
 *               keyadc config
 ***************************************************************************/
#define KADC0_EN                            1       //PB9
#define KADC1_EN                            0       //PB10
#define KADC2_EN                            0       //PB11
#define KADC3_EN                            0       //PB12
#define KADC4_EN                            0       //PB13
#define KADC5_EN                            0       //PB8

/*
* 0: 查询方式（硬件上需确保adc输入电压小于VCCIO，不可以直接上拉到VCCIO）;
* 1: 中断方式;
*/
#define KADC0_IRQ                           0
#define KADC1_IRQ                           0
#define KADC2_IRQ                           0
#define KADC3_IRQ                           0
#define KADC4_IRQ                           0
#define KADC5_IRQ                           0

/*
* 0: 按键;
* 1: 电位器旋钮;
*/
#define KADC0_KNOB                          0
#define KADC1_KNOB                          0
#define KADC2_KNOB                          0
#define KADC3_KNOB                          0
#define KADC4_KNOB                          0
#define KADC5_KNOB                          0

#define KADC_SHARE1                         0       //2个旋钮共用一个adc通道

/***************************************************************************
 *               keyio config
 ***************************************************************************/
#define KIO_GPIOB                           0       //使用PB组GPIO
#define KIO_GPIOC                           0       //使用PC组GPIO

#define ONE_KEY_MULTI_FUNC                  0       //多功能
#define KEY_SWITCH_EN                       0


#if !KEY_IO_EN
#undef KIO_GPIOB
#undef KIO_GPIOC
#undef ONE_KEY_MULTI_FUNC
#define ONE_KEY_MULTI_FUNC                  0
#define KIO_GPIOB                           0
#define KIO_GPIOC                           0
#endif

/***************************************************************************
 *               irrx config
 ***************************************************************************/
#define IRRX_PIN_SEL                        1       //0:--; 1:PD1; 2:PC3;

/***************************************************************************
 *               disp config
 ***************************************************************************/
#define DISP_EN                             1      //显示
#define LED_DIGITAL_EN                      0       //数码管
#define LED_DIODE_EN                        1      //单个二极管
#define LCD_SEG_EN                          0       //段码液晶屏

//LED_DIGITAL:
#define LED0_PIN_SEL                        0       //0:B5;
#define LED1_PIN_SEL                        0       //0:B6;
#define LED2_PIN_SEL                        0       //0:B7;
#define LED3_PIN_SEL                        0       //0:B8;
#define LED4_PIN_SEL                        0       //0:B10;
#define LED5_PIN_SEL                        1       //0:B0; 1:B11;
#define LED6_PIN_SEL                        1       //0:B2; 1:B12;
#define LED7_PIN_SEL                        0       //0:B13;
#define LED8_PIN_SEL                        0       //0:D0;

//LED_DIODE:
#define LED_RED_PIN_NUM                     (PIN_A5)
#define LED_BLUE_PIN_NUM                    (PIN_B10)

//LCD_SEG:
#define LCD_SEG_REUSE                       0       //复用做输入检测功能

#if !DISP_EN
#undef LED_DIGITAL_EN
#undef LED_DIODE_EN
#undef LCD_SEG_EN
#define LED_DIGITAL_EN                      0
#define LED_DIODE_EN                        0
#define LCD_SEG_EN                          0
#endif

/***************************************************************************
 *               motor config
 ***************************************************************************/
#define MOTOR_EN                            0

/***************************************************************************
 *               pmu startup mode config
***************************************************************************/
/*
 * 0: ONOFF按键开机/板上硬开关/板上软开关
 * 1: ONOFF高开机（HSW）
 */
#define PMU_POWER_ON_MODE                   0

/***************************************************************************
 *               pmu config
 ***************************************************************************/
#define PMU_CORE_USE_DCDC                   1       //HS系列使用DCDC给CORE供电，WS/CA系列使用LDO给CORE供电
#define PMU_VBUS_POWERON_EN                 1       //使能VBUS插入开机
#define PMU_SHORT_PRESS_POWERON_EN          0       //使能短按开机
#define PMU_ULTRA_SHORT_PRESS_POWERON_EN    0       //使能超短按开机
#define PMU_POWEROFF_WHEN_BAT_FIRST_IN      0       //使能第一次插入电池时关机
#define PMU_ULTRA_LONG_PRESS_TIME           10000    //设置超长按时长，死机时超长按可以复位，单位:ms; 只能选择6000/8000/10000/12000ms
#define PMU_LONG_PRESS_TIME                 2000    //设置长按时长，长按开机或者长按关机，单位:ms; 只能选择1000/2000/3000/4000ms
#define PMU_CHARGE_CURRENT                  100//75//200     //设置充电电流，是充电时芯片耗电和充到电池的电流总和，单位:mA; 25-450mA,步进:25mA
#define PMU_VBUSIN_RESTART                  0       //使能VBUS插入后重启，常用于耳机方案
#define PMU_VCC_RTC_ALWAYS_ON               1       //设置RTC是否常在电，如果需要RTC时间在关机时继续走，应该设置RTC为常在电
#define PMU_VCC_IO_VOLT                     3300    //设置VCC-IO电压，单位:mV，2700~3400mV，步进:100mV，会影响外设GPIO电压
#define PMU_AVCC_VOLT                       3300    //设置AVCC电压，单位:mV， 2700~3400mV， 步进:100mV，会影响音频最大音量
#define PMU_NOR_SEC_ERASE_PRO_EN            0       //使能NOR扇区擦除保护，默认不使能，如果在车机或者硬开关会出现突然掉电的产品，则需要使能
#define PMU_NOR_SEC_ERASE_PRO_VOLT          2900    //设置NOR扇区擦除保护电压，低于这个电压，不会对NOR进行擦除，单位:mV，2900/3000/3100/3300mV

/***************************************************************************
 *               battery config
 ***************************************************************************/
#define BAT_EXIST                           1       //使用内部PMU管理电池
#define BAT_VOLT_DET                        1       //使能内部PMU电池电池电压检测

#if !BAT_EXIST
#undef MODE_CHARGE_EN
#define MODE_CHARGE_EN                      0       //如果没有电池，则不需要使能充电模式
#undef BAT_VOLT_DET
#define BAT_VOLT_DET                        0       //如果没有电池，则不需要使能电池电压检测功能
#endif

#define BAT_QUANTITY_STEP_BIG               0       //使能大步进，大步进有5个步进，小步进有10个步进，根据产品需求选择，建议使用大步进
#define BAT_HW_LB_SHUT_EN                   1       //使能硬件低电关机
#define BAT_HW_LB_SHUT_VOLT                 2700    //设置硬件低电关机电压，只能选择2700/2800/2900/3100mV
#define BAT_HW_OK_VOLT                      2900    //设置硬件允许开机电压，只能选择2900/3000/3100/3300mV
#define BAT_SW_LB_WRN_VOLT                  3300    //设置软件低电提醒电压，单位:mV，2900~4400mV, 步进:100mV
#define BAT_SW_LB_SHUT_VOLT                 3100    //设置软件低电关机电压，单位:mV，2900~4400mV, 步进:100mV
#define BAT_SW_LB_WARNING_PERIOD            120000  //设置软件低电提醒周期，单位:ms, 步进:1000ms

//Twait = 20min
//Tcal = 82ms
#define RC_CAL_PRD_BASE                     60000
#define RC_CAL_PRD                          4
#define RC_CAL_NUM                          1023
#define RC_CAL_CON_MODE                     1
#define RC_POWEROFF_CAL_EN                  1
//#if ((MODE_CLOCK_EN == 0) || (BAT_EXIST == 0))
//#undef RC_CAL_CON_MODE
//#undef RC_POWEROFF_CAL_EN
//#define RC_CAL_CON_MODE                     0
//#define RC_POWEROFF_CAL_EN                  0
//#endif

/***************************************************************************
 *               power/sleep config
 ***************************************************************************/
#define SLEEP_EN                            0       //使能睡眠
#if AUDIO_KARAOKE_MODE || EQ_ALL_MODE_SUPPORT || AUDIO_ADJUST_HPLR_LINEOUTLR
#undef SLEEP_EN
#define SLEEP_EN                            0
#endif

#define SLEEP_DELAY                         10      //睡眠延时，当允许睡眠时延迟10s才进入睡眠，例如暂停本地播放10s后进入睡眠
#define TIME_BEFORE_WFI                     3       //进入WFI阈值，单位:ms

#define POWER_OFF_DELAY                     0       //关机延时，单位:ms

#define POWER_OFF_FIRST_EN                  0       //第一次开机，马上关机，如果电池接到VBUS，需要使能该功能，该功能依赖于PMU_VCC_RTC_ALWAYS_ON
#if (PMU_VCC_RTC_ALWAYS_ON == 0)                    //如果RTC关机会掉电，则需要同时关闭 POWER_OFF_FIRST_EN
#undef POWER_OFF_FIRST_EN
#define POWER_OFF_FIRST_EN                  0
#endif

#define AUTO_POWER_OFF                      0       //使能蓝牙断开自动关机，当蓝牙断开一段时间就会自动关机
#define AUTO_POWER_OFF_PERIOD               300     //蓝牙断开自动关机时间阈值，单位:s，e.g.300s = 5min

/***************************************************************************
 *               cpu freq config
 ***************************************************************************/
#define DVFS_KARAOKE_FREQ                   96000000
#define DVFS_EFFECTS_FREQ                   192000000
#define DVFS_TONEMIXER_FREQ                 64000000
#define DVFS_A2DPAUDIO_FREQ                 64000000
#define DVFS_SCOAUDIO_FREQ                  64000000
#define DVFS_SCOAENC_FREQ                   64000000
#define DVFS_USBAUDIO_FREQ                  192000000
#define DVFS_LINEINAUDIO_FREQ               64000000
#define DVFS_LOCAL_ENCODE_WAV_FREQ          192000000
#define DVFS_LOCAL_ENCODE_MP3_FREQ          192000000
#define DVFS_LOCAL_DECODE_WAV_FREQ          96000000
#define DVFS_LOCAL_DECODE_MP3_FREQ          192000000
#define DVFS_DEFAULT_FREQ                   64000000

/***************************************************************************
 *               debug config
 ***************************************************************************/
#define CPU_LOAD_EN                         0       //使能CPU负载统计功能
#if EQ_REALTIME
#undef  CPU_LOAD_EN
#define CPU_LOAD_EN                         1
#endif
#define CPU_LOAD_SHOW_STUS                  0
#define CPU_LOAD_PERIOD                     1000    //设置统计周期

/***************************************************************************
 *               eq auto load config
 ***************************************************************************/
#define EQ_LOAD_EN                          0
#define EQ_LOAD_SHOW_STATUS                 0
#define EQ_LOAD_PERIOD                      2000
/* Modify by arunboy 20200528 sunchip define*/
#define maikefeng     0 
#define AUTO_SHUNTDOWN                      1        //自动关机
#if AUTO_SHUNTDOWN
#define AUTO_SHUNTDOWN_TIME                 60*60000  //自动关机的时间(单位:分钟)
#endif
#endif

