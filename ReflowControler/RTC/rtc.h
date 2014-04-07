/*
 * rtc.h
 *
 * Created: 26.03.2014 04:14:59
 *  Author: Henry
 */ 


#ifndef RTC_H_
#define RTC_H_

#if __cplusplus
extern "C"
{
#endif


typedef struct
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t day;
	uint8_t date;
	uint8_t month;
	uint16_t year;
} RtcDateTime_t;

typedef enum { Rtc_Sqw_1Hz, Rtc_Sqw_4096Hz, Rtc_Sqw_8192Hz, Rtc_Sqw_32768Hz , Rtc_Sqw_Disable} RtcSqwStatus_t;

extern void rtc_init(void);

extern void rtc_activate_sqw(RtcSqwStatus_t);

extern void rtc_get_date_time(RtcDateTime_t*);
extern void rtc_get_date_time_string(char *, uint8_t, char *, uint8_t);
extern void rtc_set_date_time(RtcDateTime_t*);


#if __cplusplus
};
#endif

#endif /* RTC_H_ */