/*
 * rtc.c
 *
 * Created: 26.03.2014 04:14:46
 *  Author: Henry
 */ 
#include <avr/common.h>
#include <stddef.h>
#include "../I2C/i2c.h"
#include "rtc.h"


static uint8_t rtc_buffer[8];

void rtc_init(void)
{
	i2c_init();
}

void rtc_activate_sqw(RtcSqwStatus_t s)
{
	rtc_buffer[0] = 7;
	if (s != Rtc_Sqw_Disable)
	{
		rtc_buffer[1] = (uint8_t)s | 9;
	}
	else 
	{
		rtc_buffer[1] = 0;
	}
	i2c_send_bytes(0xD0, rtc_buffer, 2);
}

void rtc_get_date_time(RtcDateTime_t* dt)
{
	rtc_buffer[0] = 0;
	I2CError_t err = i2c_send_bytes(0xD0, rtc_buffer, 1);
	if (err == I2C_NoError)
	{
		err = i2c_recv_bytes(0xD0, rtc_buffer, 6);
		if (err == I2C_NoError)
		{
			dt->seconds = ((rtc_buffer[0]>>4)&0x07) * 10 + (rtc_buffer[0]&0x0f);
			dt->minutes = ((rtc_buffer[1]>>4)&0x07) * 10 + (rtc_buffer[1]&0x0f);
			if (rtc_buffer[2] & 0x40)
			{
				/* 12 hour format with PM/AM */
				dt->hours   = ((rtc_buffer[2]>>4)&0x01) * 10 + (rtc_buffer[2]&0x0f);
				if (rtc_buffer[2] & 0x20)
				{
					/* PM */
					dt->hours += 12;
				}
				else
				{
					/* AM */
				}
				
			}
			else
			{
				/* 24 hour format */
				dt->hours   = ((rtc_buffer[2]>>4)&0x03) * 10 + (rtc_buffer[2]&0x0f);
			}
			dt->date = ((rtc_buffer[4]>>4)&0x03) * 10 + (rtc_buffer[4]&0x0f);
			dt->month= ((rtc_buffer[5]>>4)&0x01) * 10 + (rtc_buffer[5]&0x0f);
			dt->year = 2000+ ((rtc_buffer[6]>>4)&0x07) * 10 + (rtc_buffer[6]&0x0f);
		}
	}
}

void rtc_get_date_time_string(char *sTime, uint8_t cTime, char *sDate, uint8_t cDate)
{
	rtc_buffer[0] = 0;
	I2CError_t err = i2c_send_bytes(0xD0, rtc_buffer, 1);
	if (err == I2C_NoError)
	{
		err = i2c_recv_bytes(0xD0, rtc_buffer, 6);
		if (err == I2C_NoError)

		if (sTime != NULL)
		{
			if (cTime >= 12)
			{
				sTime[0] = sTime[1] = sTime[2] = 32;
				sTime[5] = sTime[8] = ':';
				sTime[11] = 0;
				if (rtc_buffer[2]&0x40)
				{
					/* 12 hour format */
					if (rtc_buffer[2]&0x20)
					{
						sTime[0] = 'P';
					} else
					{
						sTime[0] = 'A';
					}
					sTime[1] = 'M';
					if (rtc_buffer[2]&0x10)
						sTime[3] = '1';
					else
						sTime[3] = '0';
				} else
				{
					/* 24 hour format */
					sTime[3] = 48+ ((rtc_buffer[2]>>4)&3);
				}
				sTime[4] = 48 + (rtc_buffer[2]&0x0f);
				/* Now write the minutes */
				sTime[6] = 48 + ((rtc_buffer[1]>>4)&0x07);
				sTime[7] = 48 + (rtc_buffer[1]&0x0f);
				/* Now write the seconds */
				sTime[9] = 48 + ((rtc_buffer[0]>>4)&0x07);
				sTime[10]= 48 + (rtc_buffer[0]&0x0f);
			}
		}
		if (sDate != NULL)
		{
			if (cDate >= 11)
			{
				sDate[2] = sDate[5] = '.';
				sDate[10] = 0;
				sDate[0] = 48 + ((rtc_buffer[4]>>4)&0x03);
				sDate[1] = 48 + (rtc_buffer[4]&0x0f);
				sDate[3] = 48 + ((rtc_buffer[5]>>4)&0x01);
				sDate[4] = 48 + (rtc_buffer[5]&0x0f);
				sDate[6] = '2';
				sDate[7] = '0';
				sDate[8] = 48 + ((rtc_buffer[6]>>4)&0x0f);
				sDate[9] = 48 + (rtc_buffer[6]&0x0f);
			}
		}
	}
}

void rtc_set_date_time(RtcDateTime_t* dt)
{
	
}
