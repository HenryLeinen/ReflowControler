/*
 * buzzer.c
 *
 * Created: 29.03.2014 18:31:55
 *  Author: Henry
 */ 
#include <avr/common.h>
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <stddef.h>
#include "buzzer.h"
#include "../main.h"


void buzz(Pwm_Frequency_t freq, uint8_t duty, uint8_t len, uint8_t twice)
{
	pwm_set_frequency(freq);
	pwm_set_duty(duty);
	_delay_ms(len);
	pwm_set_duty(0);
	for (uint8_t t = 0 ; t < twice ; t++)
	{
		_delay_ms(50);
		pwm_set_duty(duty);
		_delay_ms(len);
		pwm_set_duty(0);
	}
}