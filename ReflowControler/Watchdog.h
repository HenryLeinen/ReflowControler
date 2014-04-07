/*
 * Watchdog.h
 *
 * Created: 25.03.2012 23:12:07
 *  Author: Henry
 */ 


#ifndef WATCHDOG_H_
#define WATCHDOG_H_

#if __cplusplus
extern "C" {
#endif

#include <avr/wdt.h>

typedef enum
{
	WatchdogTimeout16ms = WDTO_15MS,
	WatchdogTimeout32ms = WDTO_30MS,
	WatchdogTimeout65ms = WDTO_60MS,
	WatchdogTimeout130ms = WDTO_120MS,
	WatchdogTimeout260ms = WDTO_250MS,
	WatchdogTimeout520ms = WDTO_500MS,
	WatchdogTimeout1s = WDTO_1S,
	WatchdogTimeout2s = WDTO_2S
} WatchdogTimeout_t;

extern void WatchdogEnable(WatchdogTimeout_t);
extern void WatchdogDisable(void);
extern void WatchdogTrigger(void);


#if __cplusplus
};
#endif


#endif /* WATCHDOG_H_ */