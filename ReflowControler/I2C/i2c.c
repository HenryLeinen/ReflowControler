/*
 * i2c.c
 *
 * Created: 25.03.2014 23:55:18
 *  Author: Henry
 */ 
#include <avr/common.h>
#include <avr/io.h>
#include <stddef.h>
#include <avr/interrupt.h>
#include "i2c.h"
#include "../main.h"


const char _i2c_s0[] PROGMEM = "NoError";
const char _i2c_s1[] PROGMEM = "Args";
const char _i2c_s2[] PROGMEM = "Busy";
const char _i2c_s3[] PROGMEM = "Start";
const char _i2c_s4[] PROGMEM = "AddrNACK";
const char _i2c_s5[] PROGMEM = "DataNACK";
const char _i2c_s6[] PROGMEM = "ArbLost";
const char _i2c_s7[] PROGMEM = "Unknown";

PROGMEM const char * const i2c_error_text[] =
{
	_i2c_s0,
	_i2c_s1,
	_i2c_s2,
	_i2c_s3,
	_i2c_s4,
	_i2c_s5,
	_i2c_s6,
	_i2c_s7
};





static uint8_t i2c_address = 0x00;
static volatile uint8_t i2c_data[8] = {0x00};
static uint8_t i2c_data_count = 0;
static volatile uint8_t i2c_data_ptr = 0;


static volatile I2CError_t i2c_last_error;

typedef enum { I2C_SendStart, I2C_SendAddress, I2C_SendData, I2C_SendStop, I2C_ReciveData, I2C_Ready } I2CStatus_t;
	
static volatile I2CStatus_t i2c_status;

ISR(TWI_vect)
{	
	switch (i2c_status) 
	{
		case I2C_SendStart		:
			/* Check if start bit was requested */
			if ( (TWCR & (1<<TWSTA )) == (1<<TWSTA))
			{
				/* Start bit was requested, check if has been sent correctly */
				if ((TWSR & 0xF8) == 0x08)
				{
					/* Start bit was successfully sent */
					i2c_status = I2C_SendAddress;
					i2c_last_error = I2C_NoError;
					/* load slave address and change into master transmission mode */
					TWDR = i2c_address;
					/* and request to send address */
					TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWIE);
				} else
				{
					/* Start bit was not sent properly */
					/* set error and terminate */
					i2c_last_error = I2C_Error_StartBit;
					i2c_status = I2C_Ready;
				}
			}
			break;
			
		case I2C_SendAddress	:
			/* The address should have been sent */
			if ( ((TWSR & 0xF8) == 0x18) || ((TWSR & 0xF8) == 0x40))
			{
				/* Address has been sent and an ACK has been received */
				/* Check if we ant to send or receive data */
				if ((i2c_address&1) == 1)
				{
					/* We want to receive data */
					i2c_status = I2C_ReciveData;
					i2c_last_error = I2C_NoError;
					/* Check if we need to receive only one byte */
					if (i2c_data_count == 1)
					{
						/* do not set the TWEN in order to react with NACK */
						TWCR = (1<<TWINT)|(1<<TWIE)|(1<<TWEN);
					}
					else
					{
						/* Respond with ACK after next byte */
						TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWIE) | (1<<TWEA);
					}
				} else
				{
					/* We want to send data */
					i2c_status = I2C_SendData;
					/* load first data byte and send */
					TWDR = i2c_data[i2c_data_ptr++];
					/* and request to send or receive */
					TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWIE);
				}
				i2c_last_error = I2C_NoError;
			} else if ( ((TWSR & 0xF8) == 0x20) || ((TWSR & 0xF8) == 0x48))
			{
				/* Address has been sent and a NACK has been received */
				/* Send Stop condition */
				i2c_last_error = I2C_Error_Nack_on_address;
				TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN) | (1<<TWIE);
				i2c_status = I2C_Ready;
			} else
			{
				/* unknown error */
				/* Send Stop condition */
				i2c_last_error = I2C_Error_StartBit;
				TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN) | (1<<TWIE);
				i2c_status = I2C_Ready;
			}
			break;
		case I2C_SendData		:
			/* Data has been sent, check if successful */
			if ((TWSR & 0xF8) == 0x28)
			{
				/* Data was acknowledged */
				i2c_last_error = I2C_NoError;
				/* check if more data to send */
				if (i2c_data_ptr >= i2c_data_count)
				{
					/* we are done, so send stop condition */
					TWCR = (1<<TWINT) | (1<<TWSTO) |(1<<TWEN) | (1<<TWIE);
					i2c_status = I2C_Ready;
 				} else
				{
					i2c_status = I2C_SendData;
					/* load next data byte to send */
					TWDR = i2c_data[i2c_data_ptr++];
					/* and request to send */
					TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWIE);
				}
			} else if ((TWSR & 0xF8) == 0x30)
			{
				/* Data was NOT acknowledged */
				i2c_last_error = I2C_Error_Nack_on_data;
				/* Send stop condition */
				TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN) | (1<<TWIE);
				i2c_status = I2C_Ready;
			} else if ((TWSR & 0xF8) == 0x38)
			{
				/* Arbitration was lost */
				i2c_last_error = I2C_Error_Arbitration_lost;
				/* Send stop condition */
				TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN) | (1<<TWIE);
				i2c_status = I2C_Ready;
			} else 
			{
				/* Unknown error */
				i2c_last_error = I2C_Error_Unknown;
				/* Send stop condition */
				TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN) | (1<<TWIE);
				i2c_status = I2C_Ready;
			}
			break;
		case I2C_ReciveData		:
			/* Check if receiving was successfull */
			if ((TWSR & 0xF8) == 0x50)
			{
				/* Data byte was successfully received with ACK */
				i2c_data[i2c_data_ptr++] = TWDR;
				/* check if next byte is last data byte */
				if (i2c_data_ptr < i2c_data_count-1)
				{
					/* next time send an ACK */
					i2c_last_error = I2C_NoError;
					TWCR = (1<<TWINT) | (1<<TWIE) | (1<<TWEN) | (1<<TWEA);
				} else if (i2c_data_ptr == i2c_data_count-1)
				{
					/* next time send a NACK */
					i2c_last_error = I2C_NoError;
					TWCR = (1<<TWINT) | (1<<TWIE) | (1<<TWEN);
				} else
				{
					/* Finished */
					i2c_last_error = I2C_Error_Unknown;
					/* Send stop condition */
					TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN) | (1<<TWIE);
					i2c_status = I2C_Ready;
				}
			} else if ((TWSR & 0xF8) == 0x58)
			{
				/* Data byte was received but a NACK has been returned, means : last byte received */
				/* Copy last byte into buffer */
				i2c_data[i2c_data_ptr++] = TWDR;
				/* check if number of received bytes matches */
				if (i2c_data_ptr == i2c_data_count)
				{
					/* everything went well, so request stop condition and terminate */
					i2c_last_error = I2C_NoError;
					TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN) | (1<<TWIE);
					i2c_status = I2C_Ready;
				} else
				{
					i2c_last_error = I2C_Error_Nack_on_data;
					TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO) | (1<<TWIE);
					i2c_status = I2C_Ready;
				}
			} else
			{
				/* An unknwon error occured */
				i2c_last_error = I2C_Error_Unknown;
				TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN) | (1<<TWIE);
				i2c_status = I2C_Ready;
			}
			break;
			
		default:
			i2c_last_error = I2C_Error_Unknown;
			i2c_status = I2C_Ready;
			TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWIE);
			break;
	}
}



void i2c_init()
{
	cli();
	/* Setup baud rate of 100kHz */
	TWBR = 2;
	TWSR = 2;
	/* Enable the TWI interface and the interrupt, while clearing pending TWINTs*/
	TWCR = 0x85;
	sei();
	i2c_last_error = I2C_NoError;
	i2c_status = I2C_Ready;
}

I2CError_t i2c_send_bytes(uint8_t addr, uint8_t *data, uint8_t count)
{
	uint16_t t;
	uint8_t i;
	if (i2c_status != I2C_Ready)
		return I2C_Error_Busy;
	if (count > 8)
		return I2C_Error_Args;
	if (data == NULL)
		return I2C_Error_Args;
					
	i2c_last_error = I2C_NoError;
	i2c_address = addr;
	i2c_data_ptr = 0;
	i2c_data_count = count;

	for(i = 0 ; i < count ; i++) 
		i2c_data[i] = data[i];
		
	i2c_last_error = I2C_NoError;
	i2c_status = I2C_SendStart;
	
	/* Initiate the communication with the start bit */
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN) | (1<<TWIE);
	
	t = get_tick();
	
	/* And wait until it is finished */
	while(i2c_status != I2C_Ready)
	{
		if (get_tick() -t > 10000) /* timeout of 1 second */
		{
			/* Send stop and reset status */
			i2c_status = I2C_Ready;
			TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);
			return I2C_Error_Timeout;
		}
	}
	
	return i2c_last_error;
}

I2CError_t i2c_recv_bytes(uint8_t addr, uint8_t *pData, uint8_t count)
{
	uint16_t t;
	uint8_t  i;
	if (i2c_status != I2C_Ready)
		return I2C_Error_Busy;
	if (count > 8)
		return I2C_Error_Args;
	if (pData == NULL)
		return I2C_Error_Args;

	i2c_last_error = I2C_NoError;
	i2c_address = addr | 1;		// Ensure we are going into read mode
	i2c_data_ptr = 0;
	i2c_data_count = count;

	i2c_last_error = I2C_NoError;
	i2c_status = I2C_SendStart;
	
	/* Initiate the communication with the start bit */
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN) | (1<<TWIE);
	
	t = get_tick();
	
	/* And wait until it is finished */
	while(i2c_status != I2C_Ready)
	{
		if (get_tick()-t >10000)
		{
			/* Send stop and reset status */
			i2c_status = I2C_Ready;
			TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);
			return I2C_Error_Timeout;
		}
	}

	/* Copy read bytes */
	for(i = 0 ; i < count ; i++)
		pData[i] = i2c_data[i];

	return i2c_last_error;
}