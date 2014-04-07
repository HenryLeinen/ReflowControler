/*
 * pwm.h
 *
 * Created: 23.03.2014 19:03:20
 *  Author: Henry
 */ 
#ifndef __PWM_H__
#define __PWM_H__

#if __cplusplus
extern "C" {
#endif

typedef enum { Pwm_Off = 0, Pwm_31250Hz = 1, Pwm_3906Hz = 2, Pwm_488Hz = 3, Pwm_122Hz = 4, Pwm_30Hz = 5 } Pwm_Frequency_t;
	
extern void pwm_init(void);
extern void pwm_set_frequency(Pwm_Frequency_t);
extern void pwm_set_duty(uint8_t);

#if __cplusplus
}
#endif
#endif
