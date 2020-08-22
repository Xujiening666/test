#define LOG_LEV 4
#include "sys_inc.h"
#include "user_config.h"
#include "rtc.h"
#include "event.h"
#include "task_main.h"


/* "rtc_base_seg":          resident,   flash
 * "rtc_base_sram_seg":     resident,   sram
 * "rtc_clock_seg":         clock,      flash
 * "rtc_clock_sram_seg":    clock,      sram
 * "rtc_alarm_seg":         alarm,      flash
 * "rtc_alarm_sram_seg":    alarm,      sram
 * "rtc_mem_seg":           mem,        flash
 * "rtc_api_seg":           api,        flash
 */


volatile uint32_t global_seconed = 0;
#if RTC_CLOCK_EN
struct rtc_t rtc;
#endif


extern bool startup_is_first(void);

AT(.rtc_base_seg)
void rtc_init(void)
{
    clk_dcxo_set(RTC_DCXO_CUR, RTC_DCXO_LP_EN, RTC_XO32K_EN);
    rtc_memory_init();

#if RTC_EN
    rtc_module_init();
#if 1
    rtc_clk_sel(RTC_CLK_LOSC);
#else
    rtc_clk_sel(RTC_CLK_HOSC);
#endif

#if 0
    if(startup_is_first()) {
		rtc.t.year = 0;     //the BASE_YEAR is defined 2018
		rtc.t.month = 1;
		rtc.t.day = 1;
		rtc.t.hour = 8;
		rtc.t.minute = 0;
		rtc.t.second = 0;
		rtc.t.weekday = MONDAY;
		rtc_set_time(&rtc.t);
    }
#endif
#if RTC_ALARM0_EN
    rtc_alarm_en(ALARMID_0);
#endif
#if RTC_ALARM1_EN
    rtc_alarm_en(ALARMID_1);
#endif
#if (RTC_ALARM0_EN || RTC_ALARM1_EN)
    rtc_alarm_get();
#endif

#endif
}



AT(.usbd_msc_seg)
void on_usb_rtc_set(uint8_t* rtcata)
{
    uint16_t year;
    uint8_t month,day, hour,minute, second;
    year = rtcata[0] + 2000;
    month = rtcata[1];
    day = rtcata[2];
    hour = rtcata[3];
    minute = rtcata[4];
    second = rtcata[5];
    logi("usb rtc set %d:%02d%02d - %02d:%02d:%02d ",year,month,day,hour,minute,second);
    rtc.t.year = year-2018;     //the BASE_YEAR is defined 2018
    rtc.t.month = month;
    rtc.t.day = day;
    rtc.t.hour = hour;
    rtc.t.minute = minute;
    rtc.t.second = second;
    rtc.t.weekday = MONDAY;
    rtc_set_time(&rtc.t);
}

#if 0
AT(.rtc_base_seg)
void rtc_deinit(void)
{
    rtc_module_deinit();
}
#endif

#if RTC_EN
AT(.rtc_base_sram_seg)
void rtc_second_cb(void)
{
    global_seconed++;
    event_put(SYSTEM_EVENT_SECOND);
#if RTC_CLOCK_EN
    //if (scene_mode.sta == MODE_CLOCK) {
        event_put(RTC_EVENT_SECOND);
    //}
#endif
}

AT(.rtc_base_sram_seg)
uint32_t rtc_get_second(void)
{
    return global_seconed;
}

AT(.rtc_alarm_seg)
void rtc_alarm_set(void)
{
    RTC_AlarmConfigDef config;

    if (rtc.alarm_enable) {
        config.status.weekday_mask =
            MONDAY_MASK
            | TUESDAY_MASK
            | WEDNESDAY_MASK
            | THURSDAY_MASK
            | FRIDAY_MASK
            | SATURDAY_MASK
            | SUNDAY_MASK;
    } else {
        config.status.weekday_mask = 0;
    }

    config.cfg.t = rtc.alarm_time;

    rtc_set_alarm(ALARMID_1, config);
}

AT(.rtc_alarm_seg)
void rtc_alarm_get(void)
{
    RTC_AlarmConfigDef config;

    rtc_get_alarm(ALARMID_1, &config);

    rtc.alarm_time = config.cfg.t;
    rtc.alarm_enable = config.status.weekday_mask ? true : false;
}

AT(.rtc_alarm_seg)
void rtc_alarm_switch(void)
{
    if (rtc.alarm_enable) {
        rtc.alarm_enable = false;
        rtc_alarm_set();
        logi("alarm off");
    } else {
        rtc.alarm_enable = true;
        rtc_alarm_set();
        logi("alarm on");
    }
}

AT(.rtc_api_seg)
void rtc_select_next_time_param(void)
{
    rtc.params_select = (rtc.params_select + 1) % 6;
}

AT(.rtc_rodata_seg)
const int rtc_max_list[] = {
    0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

AT(.rtc_api_seg)
static inline int rtc_max_day(int year, int month)
{
    if (IS_RTC_LEAP_YEAR(year) && (2 == month)) {
        return 29;
    }

    return rtc_max_list[month];
}

AT(.rtc_api_seg)
void rtc_config_time_param(int inc)
{
    int tmp;

    switch (rtc.params_select) {
    case SEL_YEAR:      //year
        if (IS_RTC_YEAR(tmp = rtc.cfg_t.year + inc)) {
            rtc.cfg_t.year = tmp;
        }
        logv("config time Year:%d", rtc.cfg_t.year);
        break;

    case SEL_MONTH:     //month
        if (IS_RTC_MONTH(tmp = rtc.cfg_t.month + inc)) {
            rtc.cfg_t.month = tmp;
        }
        logv("config time month:%d", rtc.cfg_t.month);
        break;

    case SEL_DAY:       //day
        tmp = rtc.cfg_t.day + inc;
        if (IS_RTC_DAY(tmp)
            && tmp <= rtc_max_day(rtc.cfg_t.year, rtc.cfg_t.month)) {
            rtc.cfg_t.day = tmp;
        }
        logv("config time date:%d", rtc.cfg_t.day);
        break;

    case SEL_HOUR:      //hour
        if (IS_RTC_HOUR24(tmp = rtc.cfg_t.hour + inc)) {
            rtc.cfg_t.hour = tmp;
        }
        logv("config time hour:%d", rtc.cfg_t.hour);
        break;

    case SEL_MINUTE:    //minute
        if (IS_RTC_MINUTE(tmp = rtc.cfg_t.minute + inc)) {
            rtc.cfg_t.minute = tmp;
        }
        logv("config time minute:%d", rtc.cfg_t.minute);
        break;

    case SEL_SECOND:    //second
        if (IS_RTC_SECOND(tmp = rtc.cfg_t.second + inc)) {
            rtc.cfg_t.second = tmp;
        }
        logv("config time second:%d", rtc.cfg_t.second);
        break;
    }
}

AT(.rtc_alarm_seg)
void rtc_select_next_alarm_param(void)
{
    switch (rtc.params_select) {
    case SEL_HOUR:
        rtc.params_select = SEL_MINUTE;
        break;

    case SEL_MINUTE:
        rtc.params_select = SEL_HOUR;
        break;
    }
}

AT(.rtc_alarm_seg)
void rtc_config_alarm_param(int inc)
{
    int tmp;

    switch (rtc.params_select) {
    case SEL_HOUR:      //hour
        if (IS_RTC_HOUR24(tmp = rtc.cfg_alarm_t.hour + inc)) {
            rtc.cfg_alarm_t.hour = tmp;
        }
        logv("config alarm Hours:%d", rtc.cfg_alarm_t.hour);
        break;

    case SEL_MINUTE:    //minute
        if (IS_RTC_MINUTE(tmp = rtc.cfg_alarm_t.minute + inc)) {
            rtc.cfg_alarm_t.minute = tmp;
            logv("config alarm minute:%d", rtc.cfg_alarm_t.minute);
        }
        break;
    }
}
#endif

#if (RTC_ALARM0_EN || RTC_ALARM1_EN)
AT(.rtc_alarm_sram_seg)
void rtc_alarm0_cb(void)
{
    event_put(RTC_EVENT_ALM0);
}

AT(.rtc_alarm_sram_seg)
void rtc_alarm1_cb(void)
{
    event_put(RTC_EVENT_ALM1);
}

#else
void rtc_alarm0_cb(void)
{
}

void rtc_alarm1_cb(void)
{
}
#endif
#if 0
static int DAYS = 24*3600;
static int FOURYEARS = 365*3+366;
static uint8_t norMoth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static uint8_t leapMoth[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

void getHourMinSec(int nDecyear,int nDecmoth,int nDecday,uint32_t nSecond)
{
    int nHour = nSecond/3600;
    int nMin = (nSecond%3600)/60;
    int nSec = (nSecond%3600)%60;
	logi("%d:%d:%d\n", nDecyear, nDecmoth, nDecday);
    logi("%d:%d:%d\n", nHour, nMin, nSec);
	rtc.t.year = nDecyear - BASE_YEAR;     //the BASE_YEAR is defined 2018
	rtc.t.month = nDecmoth;
	rtc.t.day = nDecday;
	rtc.t.hour = nHour;
	rtc.t.minute = nMin;
	rtc.t.second = nSec;
	rtc_set_time(&rtc.t);
}

void getMothAndDay(bool bLeapYear, int nDays, int *nMoth, int *nDay)
{
    int i = 0;
    int nTmp = 0;
    uint8_t *pMoth = bLeapYear?leapMoth:norMoth;


    for (i=0; i<12; i++)
    {
        nTmp = nDays-pMoth[i];
        if (nTmp <= 0)
        {
            *nMoth = i+1;
            if (nTmp == 0)
            {
                *nDay = pMoth[i];
            }
            else
            {
                *nDay = nDays;
            }
            break;
        }
        nDays = nTmp;
    }
}

void set_time(uint32_t nTime)
{
	nTime += 28800;//时区的八个小时
	int nDays = nTime/DAYS + ((nTime%DAYS)?1:0);
	int nYear4 = nDays/FOURYEARS;
	int nRemain = nDays%FOURYEARS;
	int nDecyear = 1970 + nYear4*4;
	int nDecmoth = 0;
	int nDecday = 0;
	bool bLeapyear = false;


	if (nRemain < 365)
	{
		;
	}
	else if (nRemain < 365*2)
	{
		nDecyear += 1;
		nRemain -= 365;
	}
	else if (nRemain < 365*3)
	{
		nDecyear += 2;
		nRemain -= 365*2;
	}
	else
	{
		nDecyear += 3;
		nRemain -= 365*3;
		bLeapyear = true;
	}
	getMothAndDay(bLeapyear, nRemain, &nDecmoth, &nDecday);
	getHourMinSec(nDecyear,nDecmoth,nDecday,nTime % DAYS);

}
#endif

#define SECS_PER_HOUR        (60 * 60)
#define SECS_PER_DAY         (SECS_PER_HOUR * 24)
#define DIV(a, b)            ((a) / (b) - ((a) % (b) < 0))
#define LEAPS_THRU_END_OF(y) (DIV (y, 4) - DIV (y, 100) + DIV (y, 400))

#define __isleap(year) \
    ((year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0))
    
/***************************************************************************//**
 * \brief       Convert UTC seconds to tm struct.
 * \param       t : seconds from 1970-1-1 0:0:0
 * \param       tp : tm struct pointer
 * \return      0 : overflow error
 * \return      1 : success
 * \author      glic __offtime
 * \note        timezone is ignored
 *               struct tm
 *               {
 *               	int tm_sec;            Seconds. [0-60] (1 leap second) 
 *               	int tm_min;            Minutes. [0-59] 
 *               	int tm_hour;           Hours.   [0-23] 
 *               	int tm_mday;           Day.     [1-31] 
 *               	int tm_mon;            Month.   [0-11] 
 *               	int tm_year;           Year - 1900.  
 *               	int tm_wday;           Day of week. [0-6] 
 *               	int tm_yday;           Days in year.[0-365] 
 *               }
 ******************************************************************************/
void set_time(uint32_t t) 
{
	uint16_t __mon_yday[2][13] =
	{
		/* Normal years.  */
		{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 },
		/* Leap years.	*/
		{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 }
	};
	uint16_t days, rem, y;
	uint16_t *ip;
	
	t += 28800;//时区的八个小时
	days = t / SECS_PER_DAY;
	rem = t % SECS_PER_DAY;
	while (rem < 0)
	{
		rem += SECS_PER_DAY;
		--days;
	}
	while (rem >= SECS_PER_DAY)
	{
		rem -= SECS_PER_DAY;
		++days;
	}

	uint8_t tm_hour = rem / SECS_PER_HOUR;
	rem %= SECS_PER_HOUR;
	uint8_t tm_min = rem / 60;
	uint8_t tm_sec = rem % 60;
	/* January 1, 1970 was a Thursday.	*/
	uint8_t tm_wday = (4 + days) % 7;
	if (tm_wday < 0)
	    tm_wday += 7;
	y = 1970;
	
	while (days < 0 || days >= (__isleap (y) ? 366 : 365))
	{
		/* Guess a corrected year, assuming 365 days per year.	*/
		long int yg = y + days / 365 - (days % 365 < 0);
		/* Adjust DAYS and Y to match the guessed year.  */
		days -= ((yg - y) * 365
			   + LEAPS_THRU_END_OF (yg - 1)
			   - LEAPS_THRU_END_OF (y - 1));
		y = yg;
	}

	uint16_t tm_year = y - 1900;
	if (tm_year != y - 1900)
	{
		loge("error");
	}
	
	ip = __mon_yday[__isleap(y)];
	for (y = 11; days < (long int) ip[y]; --y)
		continue;
	
	days -= ip[y];
	uint8_t tm_mon = y + 1;
	uint8_t tm_mday = days + 1;
	rtc.t.year = tm_year + 1900 - BASE_YEAR;     //the BASE_YEAR is defined 2018
	rtc.t.month = tm_mon;
	rtc.t.day = tm_mday;
	rtc.t.hour = tm_hour;
	rtc.t.minute = tm_min;
	rtc.t.second = tm_sec;
	rtc.t.weekday = tm_wday;
	rtc_set_time(&rtc.t);
	loge("%d/%d/%d %d:%d:%d %d",tm_year + 1900, tm_mon, tm_mday, tm_hour, tm_min, tm_sec, tm_wday);
}



