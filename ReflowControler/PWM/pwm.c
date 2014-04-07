/*
 * pwm.c
 *
 * Module which (currently) implements a very simple 8-bit PWM driver (especially for ATMega48/88&168) and for 8-bit timer devices
 * Currently only OC0A us used, changing to OC0B is very easy.
 * Created: 23.03.2014 19:03:05
 *  Author: Henry
 */ 
#include <avr/common.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stddef.h>
#include "pwm.h"


static volatile uint8_t	PwmDutyCycle = 0;
static volatile Pwm_Frequency_t	PwmFrequency = Pwm_Off;

/*************************************************/
/* ISR TIMER0_OVF_vect							 */
/*  Implements setting the new PwmDutyCycle on   */
/*  overflow of the timer. this is to make sure  */
/*  we get a clean PWM and do not lose any cycles*/
/*************************************************/
ISR(TIMER0_OVF_vect)
{
	
	if (PwmDutyCycle == 0) {
		/* In case the duty cycle is zero, simply switch off the clock */
#if defined(__AVR_ATmega48__)
		TCCR0B = (TCCR0B & 0xF8) | (uint8_t)Pwm_Off;
#elif defined(__AVR_ATmega16__)
		TCCR0 = (TCCR0 & 0xF8) | (uint8_t)Pwm_Off;
#endif
	}
#if defined(__AVR_ATmega48__)
	OCR0A = PwmDutyCycle;
#elif defined(__AVR_ATmega16__)
	OCR0 = PwmDutyCycle;
#endif
}

/************************************************************************/
/* Public Function pwm_init                                             */
/*  Initializes the HW and internal data, disables PWM					*/
/************************************************************************/
void pwm_init(void)
{
	PwmFrequency = Pwm_Off;
#if defined(__AVR_ATmega48__)
	TCCR0A = 0x83;
	TCCR0B = 0x01;
	OCR0A = 128;
	TIMSK0 = TIMSK0 | 1;
#elif defined(__AVR_ATmega16__)
	TCCR0 = (1<<WGM00) | (1<<WGM01) | (1<<COM01) | (1<<COM00);
	OCR0 = 128;
	TIMSK = TIMSK | (1<<TOIE0);
#endif
	sei();
}

/************************************************************************/
/* Public function pwm_set_frequency									*/
/*  Sets the new frequency immediately									*/
/************************************************************************/
void pwm_set_frequency(Pwm_Frequency_t freq)
{
	if (PwmFrequency != Pwm_Off) {
		/* update on the fly */
#if defined(__AVR_ATmega48__)
		TCCR0B = (TCCR0B & 0xF8) | (uint8_t)freq;
#elif defined(__AVR_ATmega16__)
		TCCR0 = (TCCR0 & 0xF8) |(uint8_t)freq;
#endif
	} 
	PwmFrequency = freq;
}

/************************************************************************/
/* Public function pwm_set_duty											*/
/*  Sets the given duty cycle. If the duty cycle is 0%, disables the    */
/*  PWM frequency.														*/
/*  The new duty cycle will be written in the overflow interrupt		*/
/************************************************************************/
void pwm_set_duty(uint8_t percent)
{
	PwmDutyCycle = (uint8_t)((uint16_t)percent * 256 / 100);
#if defined(__AVR_ATmega48__)
	TCCR0B = (TCCR0B & 0xF8) | (uint8_t)PwmFrequency;
#elif defined(__AVR_ATmega16__)
	if (percent == 0)
		TCCR0 = (TCCR0 & 0xf8) | (uint8_t)Pwm_Off;
	else
		TCCR0 = (TCCR0 & 0xf8) | (uint8_t)PwmFrequency;
#endif
}
