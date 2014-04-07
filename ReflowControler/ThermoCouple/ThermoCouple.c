/*
 * ThermoCouple.c
 *
 * Created: 01.04.2014 23:08:03
 *  Author: Henry
 */ 
#include <avr/common.h>
#include <avr/io.h>
#include <stddef.h>
#include "../main.h"
#include "../GPIO/gpio.h"
#include "ThermoCouple.h"


void thermocouple_init(void)
{
	/* Set data direction of ports */
	gpio_set_pin_high(TEMP_SENSE_CS);
	gpio_set_pin_dir(TEMP_SENSE_CS, GPIO_DIR_OUTPUT);
	gpio_set_pin_high(TEMP_SENSE_CS);
	gpio_set_pin_dir(TEMP_SENSE_MISO, GPIO_DIR_INPUT);
	gpio_set_pin_dir(TEMP_SENSE_SCK, GPIO_DIR_OUTPUT);
	/* Enable the SPI, set to master mode and set proper baud rate (500kHZ), MSB comes first */
	/* SCK is idle when low, Sample on leading (rising) edge of the clock */
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0);
	SPSR = 0; //(1<<SPI2X);
}

static uint8_t raw_data[4];
static int16_t raw_temp;
static uint8_t i;
static uint32_t ms;
int16_t thermocouple_get_temp(int16_t *internal_temp, ThermoCoupleError_t* error)
{
	
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0) | (1<<SPR1);
//	SPSR = (1<<SPI2X);
	/* Initiate CS */
	gpio_set_pin_low(TEMP_SENSE_CS);
	
	i = SPSR;
	i = SPDR;
	if (error != NULL)
		*error = ThermoCouple_Error_Communication;

	/* Get four bytes */
	for (i = 0 ; i < 4 ; i++)
	{
		SPDR = 0xff;
		ms = get_tick();
		while(!(SPSR & (1<<SPIF)))
			if (get_tick() >= ms+1000)
			{
				gpio_set_pin_high(TEMP_SENSE_CS);
				return -1;
			}
		raw_data[i] = SPDR;
	}
	
	/* Terminate CS */
	gpio_set_pin_high(TEMP_SENSE_CS);
	
	raw_temp = ((int16_t)raw_data[0]<<8) + raw_data[1];
	raw_temp /=4;
	
	if (internal_temp != NULL)
	{
		*internal_temp = (int16_t)(((uint16_t)(raw_data[2]&0x7f)<<4) + (raw_data[3]>>4));
	}
	if (error != NULL)
	{
		if (raw_data[2]&0x80)
		{
			*error = (ThermoCoupleError_t) (raw_data[3]&0x07);
		} else 
		{
			*error = ThermoCouple_Error_None;
		}
	}
	return raw_temp;
}