/*
 * adc.c
 *
 * Created: 29.03.2014 19:27:34
 *  Author: Henry
 */ 
#include <avr/common.h>
#include <avr/io.h>
#include <stddef.h>
#include "../main.h"
#include "adc.h"

void adc_init(void)
{
	ADMUX = (1<<REFS1) | (1<<REFS0) | (1<<ADLAR);		//	Use internal 2.56V Reference with capacitor on AREF pin and left adjust the result
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS0);						// Enable ADC and use prescaler of 32
}



uint16_t adc_get_value(AdcChannel_t chn)
{
	ADMUX = (1<<REFS1) | (1<<REFS0) | /*(1<<ADLAR) |*/ (uint8_t)chn;		//	Use internal 2.56V Reference with capacitor on AREF pin and left adjust the result
	ADCSRA = (1<<ADEN) | (1<<ADSC) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);						// Enable ADC and use prescaler of 32
	
	/* Wait for conversion complete */
	while((ADCSRA&(1<<ADSC)) !=0);
	
	/* Perform a second conversion */
	ADCSRA = (1<<ADEN) | (1<<ADSC) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);						// Enable ADC and use prescaler of 32
	
	/* Wait for conversion complete */
	while((ADCSRA&(1<<ADSC)) != 0);
	
	return (uint16_t)ADC;
}