/*
 * MainMenu.c
 *
 * Created: 25.03.2014 20:11:27
 *  Author: Henry
 */ 
#include <avr/common.h>
#include <avr/pgmspace.h>
#include <stddef.h>
#include "main.h"
#include "MainMenu.h"
#include "Menu/menu.h"
#include "LCD/lcd.h"
#include "PWM/pwm.h"
#include "BUZZER/buzzer.h"
#include "TASKS/DebugTask.h"
#include "TASKS/InfoTask.h"
#include "TASKS/ParamTask.h"
#include "TASKS/ReflowTask.h"


const char c_MainMenuTitle[] PROGMEM = "Main Menu";
const char c_MainMenu_InfoTitle[] PROGMEM = "Info";
const char c_MainMenu_LearnTitle[] PROGMEM = "Learn";
const char c_MainMenu_ParamTitle[] PROGMEM = "Parameter";
const char c_MainMenu_ReflowTitle[] PROGMEM = "Reflow";
const char c_MainMenu_HelpTitle[] PROGMEM = "Help";
const char c_MainMenu_Debug[] PROGMEM = "Debug";

const char c_Param_BankTitle[] PROGMEM		= "Select Bank";
const char c_Param_ActThrsTitle[] PROGMEM	= "Actv. Theshold";
const char c_Param_ToSoakTitle[] PROGMEM	= "(P1) To Soak";
const char c_Param_PreHeatTitle[] PROGMEM	= "(P2) PreHeat";
const char c_Param_ToPeakTitle[] PROGMEM	= "(P3) To Peak";
const char c_Param_ReflowTitle[] PROGMEM	= "(P4) Reflow";
const char c_Param_CoolingTitle[] PROGMEM	= "(P5) Cooling";

typedef enum
{
	MenuCmd_Invalid=0,
	MenuCmd_Reflow,
	MenuCmd_Info,
	MenuCmd_Help,
	MenuCmd_Debug,
	MenuCmd_Learn,
	
	MenuCmd_ParamP1,
	MenuCmd_ParamP2,
	MenuCmd_ParamP3,
	MenuCmd_ParamP4,
	MenuCmd_ParamP5,
	MenuCmd_ParamBank,
	MenuCmd_ActThrs
} MenuCommands_t;


/*									pNextMenu,			pPreviousMenu,  pFirstChild,		pParent */
MenuItem_t Menu_Param_Cooling	= { NULL,				NULL,			NULL,				NULL, (void*)MenuCmd_ParamP5, c_Param_CoolingTitle};
MenuItem_t Menu_Param_Reflow	= { &Menu_Param_Cooling,NULL,			NULL,				NULL, (void*)MenuCmd_ParamP4, c_Param_ReflowTitle};
MenuItem_t Menu_Param_ToPeak	= { &Menu_Param_Reflow, NULL,			NULL,				NULL, (void*)MenuCmd_ParamP3, c_Param_ToPeakTitle};
MenuItem_t Menu_Param_PreHeat	= { &Menu_Param_ToPeak, NULL,			NULL,				NULL, (void*)MenuCmd_ParamP2, c_Param_PreHeatTitle};
MenuItem_t Menu_Param_ToSoak	= { &Menu_Param_PreHeat,NULL,			NULL,				NULL, (void*)MenuCmd_ParamP1, c_Param_ToSoakTitle};
MenuItem_t Menu_Param_ActThrs   = { &Menu_Param_ToSoak, NULL,			NULL,				NULL, (void*)MenuCmd_ActThrs, c_Param_ActThrsTitle};
MenuItem_t Menu_Param_BankSel   = { &Menu_Param_ActThrs, NULL,			NULL,				NULL, (void*)MenuCmd_ParamBank, c_Param_BankTitle};
	
MenuItem_t Menu_Sub_Debug		= { NULL,				NULL,			NULL,				NULL, (void*)MenuCmd_Debug, c_MainMenu_Debug};
MenuItem_t Menu_Sub_Help		= { &Menu_Sub_Debug,	NULL,			NULL,				NULL, (void*)MenuCmd_Help, c_MainMenu_HelpTitle};
MenuItem_t Menu_Sub_Info		= { &Menu_Sub_Help,		NULL,			NULL,				NULL, (void*)MenuCmd_Info, c_MainMenu_InfoTitle};
MenuItem_t Menu_Sub_Reflow		= { &Menu_Sub_Info,		NULL,			NULL,				NULL, (void*)MenuCmd_Reflow, c_MainMenu_ReflowTitle};
MenuItem_t Menu_Sub_Param		= { &Menu_Sub_Reflow,	NULL,			&Menu_Param_BankSel,NULL, (void*)MenuCmd_Invalid, c_MainMenu_ParamTitle};
MenuItem_t Menu_Sub_Learn		= { &Menu_Sub_Param,	NULL,			NULL,				NULL, (void*)MenuCmd_Learn, c_MainMenu_LearnTitle};

MenuItem_t Menu_Main_Item		= { NULL,				NULL,			&Menu_Sub_Learn,	NULL, (void*)MenuCmd_Invalid, c_MainMenuTitle };

static MenuHandle_t hMenu;



static void main_menu_process_menu_item(MESSAGE_PUMP pMsgPump, MenuCommands_t cmd)
{
	switch(cmd)
	{
		case MenuCmd_Debug:
			debug_task(pMsgPump);
			break;
		case MenuCmd_Info:
			info_task(pMsgPump);
			break;
		case MenuCmd_Reflow:
			reflow_task(pMsgPump);
			break;
		case MenuCmd_Help:
		case MenuCmd_Learn:
			break;
		case MenuCmd_ParamP1:
		case MenuCmd_ParamP2:
		case MenuCmd_ParamP3:
		case MenuCmd_ParamP4:
		case MenuCmd_ParamP5:
			param_task(pMsgPump, (uint8_t)cmd - (uint8_t)MenuCmd_ParamP1);
			break;
		case MenuCmd_ParamBank:
			param_bank_task(pMsgPump);
			break;
		case MenuCmd_ActThrs:
			param_act_thrsh(pMsgPump);
			break;
		default:
		break;
	}
}



State_t main_menu_cyclic(MESSAGE_PUMP pMsgPump)
{
	Event_t		msg;
	uint8_t		retVal;
	
	/* Perform initialization here */
	menu_init(&hMenu, &Menu_Main_Item);
	
	/* redraw display and give audible feedback */
	lcd_cls();
	lcd_write_s(/*menu_get_menu_heading_text(&hMenu)*/hMenu.pCurrentMenu->pMenuItemText);
	lcd_locate(0,1);
	lcd_write(menu_get_current_submenu_text(&hMenu));
	/* Start the application */
	while(1)
	{
		pMsgPump(&msg);
		
		if (msg.b != 0) 
		{
			if (msg.Left)
			{
				if (menu_process_command(&hMenu, Menu_Cmd_Left) == 1)
					buzz(Pwm_488Hz, 5, 20, 0);
				else
					buzz(Pwm_122Hz, 5, 100,0);
			}
			if (msg.Right)
			{
				if (menu_process_command(&hMenu, Menu_Cmd_Right) == 1)
					buzz(Pwm_488Hz, 5, 20, 0);
				else
					buzz(Pwm_122Hz, 5, 100,0);
			}
			if (msg.Click)
			{
				retVal = menu_process_command(&hMenu, Menu_Cmd_Click);
				if (retVal == 1)
				{
					buzz(Pwm_3906Hz, 55, 10, 0);
				} else if (retVal == 2)
				{
					buzz(Pwm_3906Hz, 55, 10, 0);
					main_menu_process_menu_item(pMsgPump, (MenuCommands_t)hMenu.pCurrentSubMenu->pMenuCookie);
				} else 
					buzz(Pwm_122Hz, 5, 100,0);
			}
			if (msg.Escape)
			{
				if (menu_process_command(&hMenu, Menu_Cmd_Escape) == 1)
					buzz(Pwm_3906Hz, 55, 10, 1);
				else
					buzz(Pwm_122Hz, 5, 100,0);
			}
			
			/* redraw display and give audible feedback */
			lcd_cls();
			lcd_write(menu_get_menu_heading_text(&hMenu));
			lcd_locate(0,1);
			lcd_write(menu_get_current_submenu_text(&hMenu));
		}
	}
	
	return State_MainMenu;
}