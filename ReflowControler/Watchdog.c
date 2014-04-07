/*
 * Watchdog.c
 *
 * Created: 25.03.2012 23:11:54
 *  Author: Henry
 */ 
#include <avr/wdt.h>
#include "Watchdog.h"

void WatchdogEnable(WatchdogTimeout_t wdtTimeout)
{
	wdt_enable(wdtTimeout);
}

void WatchdogDisable(void)
{
	wdt_disable();
}

void WatchdogTrigger(void)
{
	wdt_reset();
}