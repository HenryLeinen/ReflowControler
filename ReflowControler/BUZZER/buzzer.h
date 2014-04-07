/*
 * buzzer.h
 *
 * Created: 29.03.2014 18:32:04
 *  Author: Henry
 */ 


#ifndef BUZZER_H_
#define BUZZER_H_


#if __cplusplus
extern "C" {
#endif

#include "../PWM/pwm.h"

extern void buzz(Pwm_Frequency_t freq, uint8_t duty, uint8_t len, uint8_t twice);


#if __cplusplus
};
#endif

#endif /* BUZZER_H_ */