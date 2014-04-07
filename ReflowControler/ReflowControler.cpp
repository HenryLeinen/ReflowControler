/*
 * ReflowControler.cpp
 *
 * Created: 29.03.2014 17:53:52
 *  Author: Henry
 */ 
#include <avr/common.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stddef.h>
#include "LCD/lcd.h"
#include "PWM/pwm.h"
#include "Encoder/encoder.h"
#include "Menu/menu.h"
#include "main.h"
#include "MainMenu.h"
#include "RTC/rtc.h"

const char c_GreetingMsg[] PROGMEM = "Reflow oven V1.0\n(c) Henry Leinen";

static State_t	CurrentState = State_MainMenu;

static Event_t CurrentEventMask;


static void OnEncoderEvent(Encoder_Event_t evt)
{
	if (evt == Enc_Left) {
		CurrentEventMask.Left = 1;
		} else if (evt == Enc_Right) {
		CurrentEventMask.Right = 1;
		} else if (evt == Enc_Click) {
		CurrentEventMask.Click = 1;
		} else if (evt == Enc_Escape) {
		CurrentEventMask.Escape = 1;
	}
}

static void MainMessagePump( Event_t *pEvent )
{
	CurrentEventMask.b = 0;
	
	encoder_cyclic();
	
	if (pEvent != NULL)
	{
		pEvent->b = CurrentEventMask.b;
	}
}

int main (void)
{
	board_init();
	
	// Insert application code here, after the board has been initialized.
	lcd_init();
	lcd_cls();
	lcd_locate(0,0);
	lcd_set_font(Lcd_Font_5x7);
	lcd_write_s(c_GreetingMsg);

	/* Initialize the PWM driver and set proper frequency */
	pwm_init();
	pwm_set_duty(0);
	pwm_set_frequency(Pwm_31250Hz);

	/* Initialize the encoder and register a callback function */
	encoder_init();
	encoder_register_event(OnEncoderEvent);
	
	while(1)
	{
		encoder_cyclic();
		
		switch(CurrentState)
		{
			case State_Info:
			//				info_cyclic(&CurrentEventMask);
			CurrentState = State_MainMenu;
			break;
			
			case State_MainMenu:
			CurrentState = main_menu_cyclic(MainMessagePump);
			break;
			
			case State_Reflow:
			//				refow_cyclic();
			CurrentState = State_MainMenu;
			break;
			
			case State_Learn:
			//				learn_cyclic();
			CurrentState = State_MainMenu;
			break;
			
			case State_Parameterize:
			//				parameterize_cyclic();
			CurrentState = State_MainMenu;
			break;
			
			default:
			CurrentState = State_MainMenu;
			break;
		}
	}
}
