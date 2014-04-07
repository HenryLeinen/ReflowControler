/*
 * menu.c
 *
 * Created: 24.03.2014 12:49:33
 *  Author: Henry
 */ 
#include <avr/common.h>
#include <avr/pgmspace.h>
#include <stddef.h>
#include <string.h>
#include "menu.h"


char menu_buffer[17];

static void menu_item_init(MenuItem_t *pParent, MenuItem_t *pItem)
{
	/* Iterate through all neighbours */	
	while( pItem )
	{
		pItem->pParent = pParent;
		if (pItem ->pFirstChild )
			menu_item_init(pItem, pItem->pFirstChild);
		
		if (pItem->pNextItem) {
			pItem->pNextItem->pPrevItem = pItem;
		}
		pItem = pItem->pNextItem;
	}
}

/************************************************************************/
/* This function iterates through the whole structure and fills the		*/
/* items for previous and parent										*/
/************************************************************************/
void menu_init(MenuHandle_t *menu, MenuItem_t *item)
{
	if (menu == NULL)
		return;
	
	menu->pMenuRoot = item;
	menu->pCurrentMenu = item;
	
	menu->pCurrentSubMenu = menu->pCurrentMenu->pFirstChild;
		
	menu_item_init(NULL, item);
}


/************************************************************************/
/* This function processes menu commands and allows navigation			*/
/* within the menu.														*/
/*	@returns : 0 if an action was performed which is not allowed        */
/*			   1 otherwise												*/
/************************************************************************/
uint8_t menu_process_command(MenuHandle_t *hMenu, MenuCommand_t cmd)
{
	uint8_t retval = 0;
	switch(cmd)
	{
		case Menu_Cmd_Left:
			if (hMenu->pCurrentSubMenu->pPrevItem != NULL)
			{
				hMenu->pCurrentSubMenu = hMenu->pCurrentSubMenu->pPrevItem;
				retval = 1;
			} else retval = 0;
			break;
		case Menu_Cmd_Right:
			if (hMenu->pCurrentSubMenu->pNextItem != NULL)
			{
				hMenu->pCurrentSubMenu = hMenu->pCurrentSubMenu->pNextItem;
				retval = 1;
			} else retval = 0;
			break;
		case Menu_Cmd_Click:
			if (hMenu->pCurrentSubMenu->pFirstChild != NULL)
			{
				hMenu->pCurrentMenu = hMenu->pCurrentSubMenu;
				hMenu->pCurrentSubMenu = hMenu->pCurrentMenu->pFirstChild;
				retval = 1;
			} else if (hMenu->pCurrentSubMenu->pMenuCookie != (void*)0)
			{
				retval = 2;
			}
			break;
		case Menu_Cmd_Escape:
			if (hMenu->pCurrentMenu->pParent != NULL)
			{
				hMenu->pCurrentMenu = hMenu->pCurrentMenu->pParent;
				hMenu->pCurrentSubMenu = hMenu->pCurrentMenu->pFirstChild;				
				retval = 1;
			} else retval = 0;
			break;
		default:
			retval = 0;
			break;
	}
	return retval;
}

/************************************************************************/
/* This function will generate the current menu headng text				*/
/************************************************************************/
const char* menu_get_menu_heading_text(MenuHandle_t *hMenu)
{
	strcpy_P(menu_buffer, hMenu->pCurrentMenu->pMenuItemText);
	return menu_buffer;	
}

/************************************************************************/
/* This function will generate the current submenu item text			*/
/************************************************************************/
extern const char* menu_get_current_submenu_text(MenuHandle_t *hMenu)
{
	memset(menu_buffer, 32, 16);
	menu_buffer[16] = 0;
	if (hMenu->pCurrentSubMenu->pPrevItem)
		menu_buffer[0] = '<';
	else
		menu_buffer[0] = ' ';
	menu_buffer[1] = ' ';
	const char *ptxt = hMenu->pCurrentSubMenu->pMenuItemText;
	char c;
	uint8_t x = 2;
	while( (c = pgm_read_byte((ptxt++))) != 0)
		menu_buffer[x++] = c;
	if (hMenu->pCurrentSubMenu->pNextItem)
		menu_buffer[15] = '>';
	return menu_buffer;
}
