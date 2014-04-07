/*
 * encoder.c
 *
 * Created: 24.03.2014 10:54:55
 *  Author: Henry
 */ 
#include <avr/common.h>
#include <stddef.h>
#include <avr/io.h>
#include "..\GPIO\gpio.h"
#include "..\main.h"
#include "encoder.h"



#define		SHORT_CLICK_TIME_DELAY				(10*(500))		/* corresponds to 500 ms */


static uint8_t encoder_last_switch_status = 0xff;
static uint8_t encoder_switch_status = 0xff;

static uint8_t encoder_last_enc_a_status = 0xff;
static uint8_t encoder_enc_a_status = 0xff;

static uint8_t encoder_counter = 0;

static ENCODER_EVENT	Encoder_Callback_Function = NULL;


static void encoder_fire_event(Encoder_Event_t evt)
{
	if (Encoder_Callback_Function != NULL)
		Encoder_Callback_Function(evt);
}


void encoder_init()
{
	Encoder_Callback_Function = NULL;
	
	/* initialize debounce values */
	if (gpio_get_pin_level(ENC_A))
		encoder_last_enc_a_status = encoder_enc_a_status = 0x00;
	else
		encoder_last_enc_a_status = encoder_enc_a_status = 0xFF;
	if (gpio_get_pin_level(ENC_SW) == 0)
		encoder_last_switch_status = encoder_switch_status = 0x00;
	else
		encoder_last_switch_status = encoder_switch_status = 0xFF;
	
}


void encoder_register_event( ENCODER_EVENT evt )
{
	Encoder_Callback_Function = evt;
}


void encoder_cyclic()
{
	/* debounce switch */
	static uint32_t negative_slope_time = 0;
	uint8_t mask = 1<<(encoder_counter++%8);
	if (gpio_get_pin_level(ENC_SW) == 1)
		encoder_switch_status |= mask;
	else 
		encoder_switch_status &= ~mask;

	gpio_set_pin_level(DEBUG, gpio_get_pin_level(ENC_SW));
	/* check if there is a stable state */
	if ((encoder_switch_status == 0) || (encoder_switch_status == 0xff)) {
		/* check if there is a transition */
		if (encoder_switch_status != encoder_last_switch_status) {
			/* transition detected */
			if (encoder_switch_status == 0x00) {
				/* remember when this event occured, so we can distinguish between long and short button press */
				negative_slope_time = get_tick();
			} else 
			{
				/* positive transition detected, check if the time elapsed is small enough to report a short button click */
				if ((negative_slope_time != -1) && ((get_tick() - negative_slope_time) <= SHORT_CLICK_TIME_DELAY))
				{
					encoder_fire_event(Enc_Click);
				}
			}
			encoder_last_switch_status = encoder_switch_status;
		} else
		{
			/* No transition currently. Check if button is pressed down */
			if ((encoder_switch_status == 0x00) && (negative_slope_time != -1))
			{
				/* button is pressed down so check if time elapsed since pressing it is longer than the threshold for a short click */
				if ((get_tick() - negative_slope_time) >= SHORT_CLICK_TIME_DELAY)
				{
					/*  yep, so report an escape as long button press */
					encoder_fire_event(Enc_Escape);
					/* do not send the notification again and again */
					negative_slope_time = -1;
				}
			}
		}
	}
	
	/* debounce ENC_A and ENC_B signals */
	if (gpio_get_pin_level(ENC_A) == 1)
		encoder_enc_a_status |= mask;
	else
		encoder_enc_a_status &= ~mask;
	
	/* for ENC_A check if there is a stable state */
	if ((encoder_enc_a_status == 0) || (encoder_enc_a_status == 0xFF)) {
		/* check if there is a transition */
		if (encoder_enc_a_status != encoder_last_enc_a_status) {
			/* transition detected */
			if (encoder_enc_a_status == 0x00) {
				/* transition high -> low detected */
				/* in case ENC_B is low, we have a turn to left detected, otherwise turn to right */
				if (gpio_get_pin_level(ENC_B) == 0) {
					encoder_fire_event(Enc_Left);
				} else {
					encoder_fire_event(Enc_Right);
				}
			} else {
				/* transition low -> high detected */
				/* in case ENC_B is high, we have a turn to left detected, otherwise turn to right */
				if (gpio_get_pin_level(ENC_B) == 1) {
					encoder_fire_event(Enc_Left);
				} else {
					encoder_fire_event(Enc_Right);
				}
			}
		}
		encoder_last_enc_a_status = encoder_enc_a_status;
	}
}