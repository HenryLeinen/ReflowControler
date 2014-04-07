/*
 * menu.h
 *
 * Created: 24.03.2014 12:49:23
 *  Author: Henry
 */ 
#ifndef __MENU_H__
#define __MENU_H__

#if __cplusplus
extern "C"
{
#endif

struct MenuItem_tag;

struct MenuItem_tag {
	struct MenuItem_tag*				pNextItem;
	struct MenuItem_tag*				pPrevItem;
	struct MenuItem_tag*				pFirstChild;
	struct MenuItem_tag*				pParent;
	
	void *								pMenuCookie;
	
	const char *  						pMenuItemText;
};

typedef struct MenuItem_tag MenuItem_t;

typedef struct Menu_tag {
	MenuItem_t*	pMenuRoot;
	MenuItem_t* pCurrentMenu;
	MenuItem_t* pCurrentSubMenu;
} MenuHandle_t;

typedef enum { Menu_Cmd_None, Menu_Cmd_Left, Menu_Cmd_Right, Menu_Cmd_Click, Menu_Cmd_Escape } MenuCommand_t;
	
extern void menu_init(MenuHandle_t*menu, MenuItem_t *item);
extern const char* menu_get_menu_heading_text(MenuHandle_t*);
extern const char* menu_get_current_submenu_text(MenuHandle_t*);
extern uint8_t menu_process_command(MenuHandle_t*, MenuCommand_t);

#if __cplusplus
};
#endif

#endif
