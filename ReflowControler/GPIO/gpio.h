/*
 * gpio.h
 *
 * Created: 28.03.2014 01:26:31
 *  Author: Henry
 */ 


#ifndef GPIO_H_
#define GPIO_H_

#if __cplusplus
extern "C"
{
#endif

#include "board_config.h"

#define DECLARE_PIN(port, pin)				(port << 8) | (1<<(pin))

#define GPIO_DIR_INPUT				0
#define GPIO_DIR_OUTPUT				1

#define GPIO_MODE_PULLUP			(0x01)
#define GPIO_MODE_PULLDOWN			(0x00)


#define gpio_set_pin_level(p, l)		(p ## _PORT) = (l != 0 ? (p ## _PORT) | (1<<(p ## _PIN)) : (p ## _PORT) & ~(1<<(p ## _PIN)))
#define gpio_set_pin_high(p)			(p ## _PORT) |= (1<<(p ## _PIN))
#define gpio_set_pin_low(p)				(p ## _PORT) &= ~(1<<(p ## _PIN))
#define gpio_toggle_pin_level(p)		(p ## _PORT) ^= (1<<(p ## _PIN))

#define gpio_set_pin_dir(p, d)			(p ## _DDR) = (d == GPIO_DIR_OUTPUT ? (p ## _DDR) | (1<<(p ## _PIN)) : (p ## _DDR) & ~(1<<(p ## _PIN)))
#define gpio_set_pin_mode(p, m)			(p ## _PORT) = (m != 0 ? (p ## _PORT) | (1<<(p ## _PIN)) : (p ## _PORT) & ~(1<<(p ## _PIN)))

#define gpio_get_pin_level(p)			(((p ## _PORTIN) >> (p ## _PIN)) & 0x01)



#if __cplusplus
};
#endif

#endif /* GPIO_H_ */