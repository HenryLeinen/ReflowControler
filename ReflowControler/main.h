/*
 * main.h
 *
 * Created: 25.03.2014 20:14:15
 *  Author: Henry
 */ 
#ifndef __MAIN_H__
#define __MAIN_H__

#if __cplusplus
extern "C"
{
#endif


#define F_CPU			8000000
#include <util/delay.h>


/*
 * Recipe for a typical application's cyclic function:
 * 
 * The prototype looks like follows:
 *
 *	extern State_t    <app_name>_cyclic( MESSAGE_PUMP pMsgPump );
 *
 *	The application's or main function shall ensure that the message
 *  pumping main function @ref pMsgPump is called cyclically to ensure
 *  user events are being detected. The actual user event is returned 
 *  in the pointer provided to the message_pump function.
 *  This way, the application is responsible for message processing and
 *  may decide whether or not to use blocking calls.
 *
 *  The following example shows a typical scenario for an example app :
 *
 *  void example_cyclic( MESSAGE_PUMP pMsgPump )
 *  {
 *		Event_t	msg;
 *
 *		example_initialize();
 *
 *		while(1)
 *		{
 *			pMsgPump(&msg);
 *			if (msg.Left)
 *			{
 *				...
 *			}
 *			if (msg.Right)
 *			{
 *				...
 *			}
 *			if (msg.Click)
 *			{
 *				...
 *			}
 *		}
 *	}
 */

/************************************************************************/
/* State variable which is used to control the actual program status	*/
/* All main programs' cyclic functions have to return the new status.	*/
/************************************************************************/
typedef enum { State_MainMenu, State_Reflow, State_Learn, State_Parameterize, State_Info } State_t;


/************************************************************************/
/* Event definition which has to be used by cyclic functions. When the  */
/* Event has been consumed it shall be reset to 0.						*/
/************************************************************************/
typedef union {
	struct {
		uint8_t		Left:1;
		uint8_t		Right:1;
		uint8_t		Click:1;
		uint8_t		Escape:1;
		uint8_t		Reserved:4;
	};
	uint8_t		b;
} Event_t;


typedef void (*MESSAGE_PUMP)(Event_t *pResult);


/************************************************************************/
/* get_tick function returns the number of ticks elapsed since start-up	*/
/*  This function is implemented in init.c								*/
/************************************************************************/
extern uint32_t get_tick(void);

/************************************************************************/
/* Board initialization function. Implemented in board_init.c			*/
/************************************************************************/
extern void board_init(void);

/************************************************************************/
/* The function get_seconds_between_ticks delivers the number of seconds*/
/* elapsed between the two given tick values. Please keep in mind that  */
/*																		*/
/************************************************************************/
extern uint16_t get_seconds_between_ticks(uint16_t, uint16_t);



extern const char c_GreetingMsg[];

#if __cplusplus	
};
#endif



#endif