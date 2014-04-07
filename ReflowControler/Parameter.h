/*
 * Parameter.h
 *
 * Created: 31.03.2014 12:59:49
 *  Author: Henry
 */ 


#ifndef PARAMETER_H_
#define PARAMETER_H_

#if __cplusplus
extern "C" {
#endif


typedef enum
{
	Param_Time,
	Param_Temperature
} ParameterName_t;


extern uint8_t param_get_bank();
extern void param_set_bank(uint8_t bank);

extern void param_get_bank_name(char *pName);
extern void param_set_bank_name(char *pName);

extern uint16_t param_get_value(uint8_t p_no, ParameterName_t p_name);
extern void param_set_value(uint8_t p_no, ParameterName_t p_name, uint16_t p_value);

extern uint16_t param_get_activation_threshold(void);
extern void param_set_activation_threshold(uint16_t);

#if __cplusplus
};
#endif

#endif /* PARAMETER_H_ */