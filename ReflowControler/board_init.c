/*
 * board_init.c
 *
 * Created: 28.03.2014 01:58:25
 *  Author: Henry
 */ 
#include <avr/common.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "GPIO/gpio.h"
#include "main.h"
#include "ADC/adc.h"
#include "UART/Usart.h"
#include "ThermoCouple/ThermoCouple.h"


static volatile uint32_t SystemCounter = 0;

uint32_t get_tick() { return SystemCounter; }


#if defined(__AVR_ATmega48__)
ISR(TIMER2_COMPA_vect)
#elif defined(__AVR_ATmega16__)
ISR(TIMER2_COMP_vect)
{
	SystemCounter++;
}
#endif

void board_init(void)
{
	/* Generate the timebase for the system */
	/* Use Timer 2A in CTC (Clear Timer on Compare match) mode */
	/* we are running with 8 MHz */
	cli();
#if defined(__AVR_ATmega48__)
	TCCR2A = 0x02;			//	CTC mode + non-PWM
	TCCR2B = 0x03;			//	Use a prescaler of 3 (8MHz/32 = 250kHz) to achieve a timer clock every 4µs
	OCR2A = 250;			//	produce an overflow every 1ms
	TIMSK2 = 2;				//	Enable Output compare interrupt
#elif defined(__AVR_ATmega16__)
	TCCR2 = (1<<WGM21) | (1<<CS21);			//	CTC mode + non-PWM AND use a prescaler of 3 (8MHz/32 = 250kHz) to achieve a timer clock every 4µs
	OCR2 = 100;								//	produce an overflow every 1ms
	TIMSK = (TIMSK &0xFF) | (1<<OCIE2);		//	Enable Output compare interrupt
#endif
	sei();
	/* Initialize the LCD display ports */
	gpio_set_pin_dir(LCD_DB4, GPIO_DIR_OUTPUT);
	gpio_set_pin_dir(LCD_DB5, GPIO_DIR_OUTPUT);
	gpio_set_pin_dir(LCD_DB6, GPIO_DIR_OUTPUT);
	gpio_set_pin_dir(LCD_DB7, GPIO_DIR_OUTPUT);
	
	gpio_set_pin_dir(LCD_RS, GPIO_DIR_OUTPUT);
	gpio_set_pin_dir(LCD_RW, GPIO_DIR_OUTPUT);
	gpio_set_pin_dir(LCD_CS, GPIO_DIR_OUTPUT);

	gpio_set_pin_level(LCD_RS, 0);
	gpio_set_pin_level(LCD_RW, 0),
	gpio_set_pin_level(LCD_CS, 0);	

	gpio_set_pin_dir(RELAIS_OUT, GPIO_DIR_OUTPUT);
	gpio_set_pin_dir(BUZZER_OUT, GPIO_DIR_OUTPUT);

	gpio_set_pin_level(RELAIS_OUT, 0);

	gpio_set_pin_dir(ENC_A, GPIO_DIR_INPUT);
	gpio_set_pin_dir(ENC_B, GPIO_DIR_INPUT);
	gpio_set_pin_dir(ENC_SW, GPIO_DIR_INPUT);
	gpio_set_pin_mode(ENC_A, GPIO_MODE_PULLUP);
	gpio_set_pin_mode(ENC_B, GPIO_MODE_PULLUP);
	gpio_set_pin_mode(ENC_SW, GPIO_MODE_PULLUP);

	gpio_set_pin_dir(DEBUG, GPIO_DIR_OUTPUT);
	
	gpio_set_pin_dir(TEMP_SENSE_P, GPIO_DIR_INPUT);
	gpio_set_pin_dir(TEMP_SENSE_N, GPIO_DIR_INPUT);

	gpio_set_pin_dir(LED_RED, GPIO_DIR_OUTPUT);
	gpio_set_pin_dir(LED_GRN, GPIO_DIR_OUTPUT);
	gpio_set_pin_high(LED_GRN);
	gpio_set_pin_low(LED_RED);
	
	adc_init();
	UsartSetup(UsartConfig8Bits, UsartConfigNoParity, UsartConfig1Stop, 250000);
	
	thermocouple_init();
}
