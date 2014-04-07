/*
 * i2c.h
 *
 * Created: 25.03.2014 23:55:08
 *  Author: Henry
 */ 


#ifndef I2C_H_
#define I2C_H_

#include <avr/pgmspace.h>

#if __cplusplus
extern "C"
{
#endif

extern PROGMEM const char * const i2c_error_text[];


typedef enum { 
	I2C_NoError = 0, 
	I2C_Error_Args, 
	I2C_Error_Busy, 
	I2C_Error_StartBit, 
	I2C_Error_Nack_on_address, 
	I2C_Error_Nack_on_data, 
	I2C_Error_Arbitration_lost, 
	I2C_Error_Timeout,
	I2C_Error_Unknown 
} I2CError_t;

extern void i2c_init(void);
extern I2CError_t i2c_send_bytes(uint8_t addr, uint8_t *data, uint8_t count);
extern I2CError_t i2c_recv_bytes(uint8_t addr, uint8_t *pData, uint8_t count);



#if __cplusplus
};
#endif

#endif /* I2C_H_ */