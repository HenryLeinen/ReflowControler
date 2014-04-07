/*
 * ReflowTask.c
 *
 * Created: 31.03.2014 16:54:29
 *  Author: Henry
 */ 
#include <avr/common.h>
#include <avr/pgmspace.h>
#include <stddef.h>
#include "../BUZZER/buzzer.h"
#include "../LCD/lcd.h"
#include "../Parameter.h"
#include "ReflowTask.h"
#include <stdio.h>
#include "../UART/Usart.h"
#include <string.h>
#include "../GPIO/gpio.h"
#include "../ThermoCouple/ThermoCouple.h"

const char s_ConfirmMsg[] PROGMEM = "Ready for soldering ?\nYES          NO";
const char s_CancelMsg[] PROGMEM = "ESC to cancel !";

const char s_ReadyMsg[] PROGMEM = "Reflow finished !\nCAUTION HOT    C";
const char s_ErrorMsg[] PROGMEM = "Error occured !\nCAUTION HOT    C";

const char s_ReportMsg[] PROGMEM = "Phase\tZeit\tIst Temp\tSoll Temp\tHeizung\n";

const char s_ActivationMsg[] PROGMEM = "Waiting T=%3d\nActual T=%3d";

typedef enum 
{ 
	ReflowState_Activation,
	ReflowState_Running, 
	ReflowState_Error, 
	ReflowState_Confirm, 
	ReflowState_Ready,
	ReflowState_Quit
} ReflowState_t;
static uint8_t			reflow_phase;

static ReflowState_t	reflow_state;
static uint32_t			start_time;
static uint32_t			next_phase_time;
static uint32_t			next_screen_update_time;
static int16_t			target_temp;
static int16_t			actual_temp;
static uint16_t			actual_duration;
static int16_t			temp_tracker;
static int16_t			start_temp;
static char Buffer[45];
static ThermoCoupleError_t	th_error;
static int16_t nLastTemperature = 20*4;
static uint32_t		absolute_time;

static void reflow_init()
{
	start_time = get_tick();
	target_temp = (int16_t)param_get_value(reflow_phase, Param_Temperature);
	actual_duration = param_get_value(reflow_phase, Param_Time);
	next_phase_time = start_time + (uint32_t)actual_duration*10000;
	next_screen_update_time = start_time;

	if (reflow_phase == 0)
	{
		start_temp = 20;
		absolute_time = start_time;
		UsartSendString_S(s_ReportMsg, NULL);
	}
	else
		start_temp = (int16_t)param_get_value(reflow_phase-1, Param_Temperature);
			
	lcd_cls();
	lcd_enable_display(Lcd_Display_On, Lcd_Cursor_Off);
}

static uint16_t time_min, time_sec, total_sec;
static uint16_t percent;
static uint8_t		user_interrupt = 0;
static uint32_t		tm;
static uint8_t done = 0;

static uint32_t		reflow_temp_start_time = 0;

void reflow_wait_activation(Event_t *msg)
{
	tm = get_tick();
	
	gpio_set_pin_high(LED_RED);
	gpio_set_pin_low(LED_GRN);
	gpio_set_pin_high(RELAIS_OUT);
	
	if (tm >= next_screen_update_time)
	{
		/* DO MEASURE TEMPERATURE */
		actual_temp = thermocouple_get_temp(NULL, &th_error)/4;

		lcd_locate(0,0);
		if (th_error == ThermoCouple_Error_None)
		{
			sprintf_P(Buffer, s_ActivationMsg, actual_temp, param_get_activation_threshold());
			lcd_write(Buffer);
		
			if (actual_temp >= param_get_activation_threshold())
			{
				reflow_state = ReflowState_Running;
				reflow_init();
				start_temp = actual_temp;
			}
			
		} else
		{
			lcd_write("Error : ");
			switch(th_error)
			{
				case ThermoCouple_Error_Communication:
					lcd_write("Comm");
					break;
				case ThermoCouple_Error_OpenCircuit:
					lcd_write("OL");
					break;
				case ThermoCouple_Error_ShortToGND:
					lcd_write("S2G");
					break;
				case ThermoCouple_Error_ShortToVCC:
					lcd_write("S2B");
					break;
				default:
					break;
			}
		}
		
		next_screen_update_time = tm + 5000;
	}
	
	if (msg->Click || msg->Escape)
	{
		/* user intends to cancel */
		buzz(Pwm_488Hz, 50, 250, 1);
		reflow_state = ReflowState_Ready;
		reflow_temp_start_time = 0;
		user_interrupt = 0;
	}
}


void reflow_control(Event_t *msg)
{
	tm = get_tick();
	
	if (tm >= next_screen_update_time)
	{
		if (gpio_get_pin_level(RELAIS_OUT))
			gpio_toggle_pin_level(LED_RED);
		else
			gpio_toggle_pin_level(LED_GRN);
		
		total_sec = ((tm-start_time)/10000);
		time_sec = total_sec%60;
		time_min = total_sec/60;
		percent = total_sec*100/actual_duration;
		
		/* DO MEASURE TEMPERATURE */
		actual_temp = thermocouple_get_temp(NULL, &th_error);
			/* check for unplausible temperature */
		if ((actual_temp < (15*4)) || (actual_temp > (270*4)))
		{
			/* if unplausible, reuse last measured temperature */
			actual_temp = (int16_t)nLastTemperature;
		} else 
		{
			nLastTemperature = actual_temp;
			actual_temp >>= 2;
			if ((actual_temp > 218) && (reflow_temp_start_time == 0))
				reflow_temp_start_time = tm;		/* Start the counting of the critical time */
			else if (actual_temp < 218)
				reflow_temp_start_time = 0;
		}
		
		temp_tracker = start_temp + (target_temp-start_temp)*(int16_t)total_sec/(int16_t)actual_duration;
		/* update screen */
		lcd_locate(0,0);
		lcd_write_c('P');
		lcd_write_c(48+reflow_phase+1);
		lcd_write_c(':');
		sprintf(Buffer, " T=%3dC->%3dC", actual_temp, temp_tracker);
		lcd_write(Buffer);
		lcd_locate(0,1);
		if (reflow_temp_start_time != 0)
		{
			uint16_t total_reflow_sec = ((tm-reflow_temp_start_time)/10000);
			uint16_t time_reflow_sec = total_reflow_sec%60;
			uint16_t time_reflow_min = total_reflow_sec/60;
			sprintf(Buffer, "Reflow %02d:%02d", time_reflow_min, time_reflow_sec);
			
			if (total_reflow_sec > 60)
			{
				/* inform user and ask to open door */
				/* end is reached !*/
				buzz(Pwm_3906Hz, 50, 250, 0);
				_delay_ms(500);
				buzz(Pwm_3906Hz, 50, 250, 0);
				_delay_ms(500);
				buzz(Pwm_3906Hz, 50, 250, 0);
				reflow_state = ReflowState_Ready;
				done = 0;
			}
		} else 
		{
			for(uint8_t i = 0 ; i < 16 ; i++)
			{
				if ( i < percent*16/100 )
				Buffer[i] = 255;
				else
				Buffer[i] = 32;
			}
			if (time_sec&1)
			{
				sprintf(&Buffer[6], "%02d:%02d", time_min, time_sec);
			}
		}
		lcd_write(Buffer);
		/* Send data via uart */
		sprintf(Buffer, "P%1d\t%4d\t%3d,%02d\t%3d\t%u\n", reflow_phase+1, (uint16_t)((tm-absolute_time)/10000), actual_temp, (nLastTemperature&3)*25, temp_tracker, gpio_get_pin_level(RELAIS_OUT));
		uint8_t len = strlen(Buffer);
		UsartSendString(Buffer, &len);
		
		if (user_interrupt == 1)
		{
			lcd_locate(0,1);
			lcd_write_s(s_CancelMsg);
		}
		
		/* PERFORM PID or 2 point control */
		/* Control output */
		if (gpio_get_pin_level(RELAIS_OUT) && (actual_temp > temp_tracker-1))
		{
			/* switch off relais */
			gpio_set_pin_low(RELAIS_OUT);
			gpio_set_pin_low(LED_RED);
		} else if (!gpio_get_pin_level(RELAIS_OUT) && (actual_temp <= temp_tracker))
		{
			gpio_set_pin_high(RELAIS_OUT);
			gpio_set_pin_low(LED_GRN);
		}
		next_screen_update_time = tm + 5000;
	}
	if (tm >= next_phase_time)
	{
		/* Next phase has been reached */
		reflow_phase++;
		
		if (reflow_phase == 5)
		{
			/* end is reached !*/
			buzz(Pwm_3906Hz, 50, 250, 0);
			_delay_ms(500);
			buzz(Pwm_3906Hz, 50, 250, 0);
			_delay_ms(500);
			buzz(Pwm_3906Hz, 50, 250, 0);
			done = 0;
			reflow_state = ReflowState_Ready;
		} else
		{
			buzz(Pwm_488Hz, 50, 250,0);
			reflow_init();
		}
	}
	if (msg->Click)
	{
		if (user_interrupt == 0)
		{
			/* ask user about cancelling request, while not stopping the procedure */
			user_interrupt = 1;
		} else 
		{
			/* user does NOT want to cancel */
			user_interrupt = 0;
		}
		buzz(Pwm_3906Hz, 10, 10, 0);
	}
	if (msg->Escape)
	{
		if (user_interrupt !=0)
		{
			/* user intends to cancel */
			buzz(Pwm_488Hz, 50, 250, 1);
			reflow_state = ReflowState_Ready;
			user_interrupt = 0;
		} else
		{
			buzz(Pwm_488Hz, 20, 50, 0);
		}
	}
}

static void reflow_state_confirm(Event_t *msg)
{
	static uint8_t done = 2;
	
	if (done == 2)
	{
		lcd_cls();
		lcd_locate(0,0);
		lcd_write_s(s_ConfirmMsg);
		lcd_enable_display(Lcd_Display_On, Lcd_Cursor_Blink);
		lcd_locate(13,1);
		done = 1;
		gpio_set_pin_low(LED_GRN);
		gpio_set_pin_high(LED_RED);
	}
	
	if (msg->Left || msg->Right)
	{
		if (done == 1)
		{
			done = 0;
			lcd_locate(0, 1);
		}
		else if (done == 0)
		{
			done = 1;
			lcd_locate(13,1);
		}
		
		buzz(Pwm_488Hz, 5, 5, 0);
	}
	
	if (msg->Click || msg->Escape)
	{
		if (done == 0)
		{
			/* User clicked CANCEL */
			reflow_state = ReflowState_Activation;
			buzz(Pwm_3906Hz, 50, 250, 0);
			/* PERFORM INITIALIZATION FOR REFLOW SOLDERING */
			reflow_phase = 0;
			reflow_init();
			done = 2;
			gpio_set_pin_low(LED_GRN);
			gpio_set_pin_high(LED_RED);
		} else
		{
			/* user cancelled */
			reflow_state = ReflowState_Quit;
			buzz(Pwm_122Hz, 50, 50, 5);			
			done = 2;
			gpio_set_pin_high(LED_GRN);
			gpio_set_pin_low(LED_RED);
		}
	}
	
}


static void reflow_error(Event_t *msg)
{
	static uint8_t done = 0;
	static uint32_t old_time = 0;
	
	if (done == 0)
	{
		gpio_set_pin_low(RELAIS_OUT);
		gpio_set_pin_low(LED_RED);
		gpio_set_pin_high(LED_GRN);
		lcd_cls();
		lcd_write_s(s_ErrorMsg);
		sprintf(Buffer, "%3d", actual_temp);
		lcd_locate(12,1);
		lcd_write(Buffer);
		done = 1;
		old_time = get_tick();
	}
	
	if (get_tick() > old_time+10000)
	{
		sprintf(Buffer, "%3d", actual_temp);
		lcd_locate(12,1);
		lcd_write(Buffer);
		old_time = get_tick();
	}
	if (msg->Click || msg->Escape)
	{
		buzz(Pwm_3906Hz, 10, 10, 1);
		done = 0;
		reflow_state = ReflowState_Quit;
	}
}

static uint32_t old_time = 0;

static void reflow_ready(Event_t *msg)
{
	
	if (done == 0)
	{
		gpio_set_pin_low(RELAIS_OUT);
		gpio_set_pin_low(LED_RED);
		gpio_set_pin_high(LED_GRN);
		lcd_cls();
		lcd_write_s(s_ReadyMsg);
		sprintf(Buffer, "%3d", actual_temp);
		lcd_locate(12,1);
		lcd_write(Buffer);
		done = 1;
		old_time = get_tick();
	}
	
	if (get_tick() > old_time+10000)
	{
		sprintf(Buffer, "%3d", actual_temp);
		lcd_locate(12,1);
		lcd_write(Buffer);
		old_time = get_tick();
	}
	if (msg->Click || msg->Escape)
	{
		buzz(Pwm_3906Hz, 10, 10, 1);
		done = 0;
		reflow_state = ReflowState_Quit;
	}
}


void reflow_task(MESSAGE_PUMP pMsgPump)
{
	Event_t msg;
	
	reflow_state = ReflowState_Confirm;
	
	while(1)
	{
		pMsgPump(&msg);
	
		if (reflow_state == ReflowState_Confirm)
		{
			reflow_state_confirm(&msg);
		} else if (reflow_state == ReflowState_Activation)
		{
			reflow_wait_activation(&msg);
		} else if (reflow_state == ReflowState_Running)
		{
			reflow_control(&msg);
		} else if (reflow_state == ReflowState_Ready)
		{
			reflow_ready(&msg);
		} else if (reflow_state == ReflowState_Error)
		{
			reflow_error(&msg);
		} else if (reflow_state == ReflowState_Quit)
		{
			return ;
		}
	}
}