/*
 * ParamTask.c
 *
 * Created: 30.03.2014 19:10:38
 *  Author: Henry
 */ 
#include <avr/common.h>
#include <avr/pgmspace.h>
#include "../LCD/lcd.h"
#include "../BUZZER/buzzer.h"
#include "../main.h"
#include "../Parameter.h"
#include <string.h>
#include <stdio.h>
#include <stddef.h>


extern const char c_Param_ToSoakTitle[];
extern const char c_Param_PreHeatTitle[];
extern const char c_Param_ToPeakTitle[];
extern const char c_Param_ReflowTitle[];
extern const char c_Param_CoolingTitle[];

const char s_SelectBank[] PROGMEM = "Bank: \nBank 0";
const char s_ActvThrshld[] PROGMEM = "Act. Threshold\nT=%3d";


PGM_P const c_Titles[5] PROGMEM = { c_Param_ToSoakTitle, c_Param_PreHeatTitle, c_Param_ToPeakTitle, c_Param_ReflowTitle, c_Param_CoolingTitle };

char Buffer[17];

typedef enum { Scroll_Parameters, Scroll_Item, Scroll_Number } ScrollType_t;
typedef enum { Update_none, Update_All, Update_Time, Update_Temp } UpdateType_t;
typedef enum { Select_Time, Select_Temp, Select_Ctrl } SelectionType_t;
	
static ScrollType_t scroll_type = Scroll_Parameters;
static UpdateType_t update_type = Update_All;
static SelectionType_t sel_type = Select_Time;

void param_task(MESSAGE_PUMP pMsgPump, uint8_t ParamNo)
{
	Event_t msg;
	
	update_type = Update_All;
	sel_type = Select_Time;
	scroll_type = Scroll_Parameters;
	
	while(1)
	{
		pMsgPump(&msg);
		
		switch(scroll_type)
		{
			case Scroll_Parameters: 
				if (msg.Left)
				{
					buzz(Pwm_488Hz, 10, 10, 0);
					if (ParamNo == 0)
						ParamNo = 4;
					else
						ParamNo--;
					update_type = Update_All;
				}
				if (msg.Right)
				{
					buzz(Pwm_488Hz, 10, 10, 0);
					if (ParamNo == 4)
						ParamNo = 0;
					else
						ParamNo++;
					update_type = Update_All;
				}
				break;
			case Scroll_Item : 
				if (msg.Left)
				{
					if (sel_type == Select_Time)
					{
						sel_type = Select_Temp;
						update_type = Update_Temp;
					}
					else
					{
						sel_type = Select_Time;
						update_type = Update_Time;
					}
					buzz(Pwm_3906Hz, 10, 10, 0);
				}
				if (msg.Right)
				{
					if (sel_type == Select_Time)
					{
						sel_type = Select_Temp;
						update_type = Update_Temp;
					}
					else
					{
						sel_type = Select_Time;
						update_type = Update_Time;
					}
					buzz(Pwm_3906Hz, 10, 10, 0);
				}
				break;
			case Scroll_Number : 
				if (msg.Left)
				{
					if (sel_type == Select_Time)
					{
						update_type = Update_Time;
						uint16_t p = param_get_value(ParamNo, Param_Time);
						if (p == 0)
						{
							buzz(Pwm_122Hz, 50, 20, 0);
						} else 
						{
							p--;
							buzz(Pwm_488Hz, 5, 5, 0);
							param_set_value(ParamNo, Param_Time, p);
						}
					} else if (sel_type == Select_Temp)
					{
						update_type = Update_Temp;
						uint16_t p = param_get_value(ParamNo, Param_Temperature);
						if (p == 0)
						{
							buzz(Pwm_122Hz, 50, 20, 0);
						} else
						{
							p--;
							buzz(Pwm_488Hz, 5, 5, 0);
							param_set_value(ParamNo, Param_Temperature, p);
						}
					}
				}
				if (msg.Right)
				{
					if (sel_type == Select_Time)
					{
						update_type = Update_Time;
						uint16_t p = param_get_value(ParamNo, Param_Time);
						if (p >= 600)
						{
							buzz(Pwm_122Hz, 50, 20, 0);
						} else
						{
							p++;
							buzz(Pwm_488Hz, 5, 5, 0);
							param_set_value(ParamNo, Param_Time, p);
						}
					} else if (sel_type == Select_Temp)
					{
						update_type = Update_Temp;
						uint16_t p = param_get_value(ParamNo, Param_Temperature);
						if (p >= 250)
						{
							buzz(Pwm_122Hz, 50, 20, 0);
						} else
						{
							p++;
							buzz(Pwm_488Hz, 5, 5, 0);
							param_set_value(ParamNo, Param_Temperature, p);
						}
					}
				}
				break;
		}
		
		if (msg.Escape)
		{
			buzz(Pwm_3906Hz, 10, 10, 1);
			switch(scroll_type)
			{
				case Scroll_Parameters:
					lcd_enable_display(Lcd_Cursor_On, Lcd_Cursor_Off);
					return;
				case Scroll_Item:
					scroll_type = Scroll_Parameters;
					update_type = Update_All;
					break;
				case Scroll_Number:
					scroll_type = Scroll_Item;
					if (sel_type == Select_Temp)
						update_type = Update_Temp;
					else
						update_type = Update_Time;
					break;
			}
		}
		
		if (msg.Click)
		{
			switch(scroll_type)
			{
				case Scroll_Parameters:
					buzz(Pwm_3906Hz, 10, 10, 0);
					scroll_type = Scroll_Item;
					sel_type = Select_Time;
					update_type = Update_Time;
					break;
				case Scroll_Item:
					buzz(Pwm_3906Hz, 5, 5, 0);
					scroll_type = Scroll_Number;
					if (sel_type == Select_Time)
						update_type = Update_Time;
					else
						update_type = Update_Temp;
					break;
				case Scroll_Number:
					break;
			}
		}
		
		if (update_type == Update_All)
		{
			update_type = Update_none;
			lcd_enable_display(Lcd_Display_On, Lcd_Cursor_Off);
			lcd_cls();
	
			lcd_locate(0,0);
			lcd_write_s((PGM_P)pgm_read_ptr(&c_Titles[ParamNo]));
	
			lcd_locate(0,1);
			Buffer[0] = 't'; Buffer[1] = '=';
			sprintf(Buffer, "t=%3dsec  T=%3dC", param_get_value(ParamNo, Param_Time), param_get_value(ParamNo, Param_Temperature));
			lcd_write(Buffer);
	
			lcd_locate(2,1);
		} else if (update_type == Update_Temp)
		{
			update_type = Update_none;
			lcd_enable_display(Lcd_Display_On, (scroll_type == Scroll_Number ? Lcd_Cursor_Blink : Lcd_Cursor_On) );
			sprintf(&Buffer[12], "%3dC", param_get_value(ParamNo, Param_Temperature));
			lcd_locate(12, 1);
			lcd_write(&Buffer[12]);
			lcd_locate(14,1);
		} else if (update_type == Update_Time)
		{
			update_type = Update_none;
			lcd_enable_display(Lcd_Display_On, (scroll_type == Scroll_Number ? Lcd_Cursor_Blink : Lcd_Cursor_On) );
			sprintf(&Buffer[2], "%3d", param_get_value(ParamNo, Param_Time));
			lcd_locate(2, 1);
			lcd_write(&Buffer[2]);
			lcd_locate(4,1);
		}
	}
}




void param_bank_task(MESSAGE_PUMP pMsgPump)
{
	Event_t msg;
	
	lcd_cls();
	lcd_locate(0,0);
	lcd_write_s(s_SelectBank);
	param_get_bank_name(Buffer);
	lcd_locate(5, 0);
	lcd_write_n(Buffer, 10);
	lcd_locate(5,1);
	lcd_enable_display(Lcd_Display_On, Lcd_Cursor_Blink);
	lcd_write_c(48+param_get_bank());
	lcd_locate(5,1);
		
	while(1)
	{
		pMsgPump(&msg);
	
		if(msg.Left || msg.Right)
		{
			if (param_get_bank() == 0)
				param_set_bank(1);
			else
				param_set_bank(0);
			
			param_get_bank_name(Buffer);
			lcd_locate(5, 0);
			lcd_write_n(Buffer, 10);
			lcd_locate(5,1);
			lcd_write_c(48+param_get_bank());
			lcd_locate(5,1);
			
			buzz(Pwm_3906Hz, 5, 5, 0);
		}	
		
		
		if (msg.Click || msg.Escape)
		{
			buzz(Pwm_3906Hz, 10, 10, 6);
			return;
		}
	}
}







void param_act_thrsh(MESSAGE_PUMP pMsgPump)
{
	Event_t msg;
	
	lcd_cls();
	lcd_locate(0,0);
	sprintf_P(Buffer, s_ActvThrshld, param_get_activation_threshold());
	lcd_write(Buffer);
	lcd_enable_display(Lcd_Display_On, Lcd_Cursor_Blink);
	lcd_locate(2,1);
	while(1)
	{
		pMsgPump(&msg);
		
		if(msg.Right)
		{
			uint16_t t = param_get_activation_threshold();
			t++;
			if (t > 200)
				t = 200;
			else if (t < 0)
				t = 0;
			param_set_activation_threshold(t);
			lcd_locate(2,1);
			lcd_locate(0,0);
			sprintf_P(Buffer, s_ActvThrshld, param_get_activation_threshold());
			lcd_write(Buffer);
			
			buzz(Pwm_3906Hz, 5, 5, 0);
		}
		if (msg.Left)
		{
			uint16_t t = param_get_activation_threshold();
			t--;
			if (t > 200)
				t = 200;
			else if (t < 0)
				t = 0;
			param_set_activation_threshold(t);
			lcd_locate(2,1);
			lcd_locate(0,0);
			sprintf_P(Buffer, s_ActvThrshld, param_get_activation_threshold());
			lcd_write(Buffer);
			
			buzz(Pwm_3906Hz, 5, 5, 0);
		}
		
		
		if (msg.Click || msg.Escape)
		{
			buzz(Pwm_3906Hz, 10, 10, 6);
			return;
		}
	}
}