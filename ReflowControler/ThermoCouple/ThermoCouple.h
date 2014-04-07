/*
 * ThermoCouple.h
 *
 * Created: 01.04.2014 23:08:14
 *  Author: Henry
 */ 


#ifndef THERMOCOUPLE_H_
#define THERMOCOUPLE_H_

#if __cplusplus
extern "C" {
#endif

typedef enum 
{ 
	ThermoCouple_Error_None = 0,
	ThermoCouple_Error_OpenCircuit = 1, 
	ThermoCouple_Error_ShortToGND = 2, 
	ThermoCouple_Error_ShortToVCC = 4, 
	ThermoCouple_Error_Communication = 8
} ThermoCoupleError_t;

extern void thermocouple_init(void);

extern int16_t thermocouple_get_temp(int16_t *internal_temp, ThermoCoupleError_t* error);


#if __cplusplus
};
#endif

#endif /* THERMOCOUPLE_H_ */