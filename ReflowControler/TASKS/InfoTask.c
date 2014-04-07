/*
 * InfoTask.c
 *
 * Created: 29.03.2014 18:17:48
 *  Author: Henry
 */ 
#include <avr/common.h>
#include <avr/pgmspace.h>
#include "../LCD/lcd.h"
#include "InfoTask.h"
#include "../BUZZER/buzzer.h"


void info_task(MESSAGE_PUMP pMsgPump)
{
	Event_t msg;
	
	lcd_cls();
	lcd_locate(0,0);
	lcd_write_s(c_GreetingMsg);

	while(msg.Escape != 1)
	{
		pMsgPump(&msg);		
	}
	
	buzz(Pwm_3906Hz, 10, 20, 1);
}


