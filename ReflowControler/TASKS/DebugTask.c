/*
 * DebugTask.c
 *
 * Created: 29.03.2014 18:17:59
 *  Author: Henry
 */ 
#include <avr/common.h>
#include <avr/pgmspace.h>
#include "../LCD/lcd.h"
#include "DebugTask.h"
#include <stddef.h>
#include "../BUZZER/buzzer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../GPIO/gpio.h"
#include "../ThermoCouple/ThermoCouple.h"


const char c_debug_text[] PROGMEM = "Debug: T:%3d.%02dC\n      Ti: %2d.%02dC";


static char time_s[17];

static Event_t	msg;

const char s_Relais_On[] PROGMEM = "R:ON";
const char s_Relais_Off[] PROGMEM = "R:OFF";

static uint16_t temp_1, temp_int_1;
static uint16_t temp_2, temp_int_2;
static int16_t t_ext, t_int;
		

static uint32_t	tm;

void debug_task(MESSAGE_PUMP pMsgPump)
{
	 tm = get_tick() + 500*10;
		
	lcd_cls();
	
	while(1)
	{
		pMsgPump(&msg);
		
		if (msg.Click)
		{
			/* Toggle Relay */
			gpio_toggle_pin_level(RELAIS_OUT);
			buzz(Pwm_3906Hz, 10, 10, 0);
		}
		if (msg.Escape)
		{
			buzz(Pwm_3906Hz, 10, 10, 1);
			return;
		}		
		
			
		if (tm < get_tick())
		{
			
			/*  reload for next cycle */
			tm = get_tick() + 500*10;
		
			t_ext = thermocouple_get_temp(&t_int, NULL);
			temp_1 = t_ext >> 2;
			temp_2 = (t_ext & 0x03) * 25;
			temp_int_1 = t_int >> 4;
			temp_int_2 = 625 * (t_int&0x0f);
			
			/* and redisplay values */
			/* Get new measurement */
			lcd_locate(0,0);
			sprintf_P(time_s, c_debug_text, temp_1, temp_2, temp_int_1, temp_int_2);
			lcd_write(time_s);
			/* update relais status */
			lcd_locate(0, 1);
			if (gpio_get_pin_level(RELAIS_OUT) == 0)
			{
				lcd_write_s(s_Relais_Off);
			} else
			{
				lcd_write_s(s_Relais_On);
			}

		}
	}
}

