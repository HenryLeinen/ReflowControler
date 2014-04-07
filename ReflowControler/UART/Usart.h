/*
 * Usart.h
 *
 * Created: 25.03.2012 23:35:45
 *  Author: Henry
 */ 


#ifndef USART_H_
#define USART_H_

typedef enum
{
	UsartConfig5Bits = 0x00,
	UsartConfig6Bits = 0x01,
	UsartConfig7Bits = 0x02,
	UsartConfig8Bits = 0x03,
	UsartConfig9Bits = 0x07
} UsartBitConfig_t;

typedef enum
{
	UsartConfigNoParity = 0x00,
	UsartConfigEvenParity = 0x02,
	UsartConfigOddParity = 0x03
} UsartParityConfig_t;

typedef enum
{
	UsartConfig1Stop = 0x00,
	UsartConfig2Stop = 0x01
} UsartStopBitConfig_t;

typedef enum
{
	UsartOk,
	UsartBusy,
	UsartTooLong,
	UsartInvalidParameter
} UsartStatus_t;

extern UsartStatus_t UsartSetup(UsartBitConfig_t, UsartParityConfig_t , UsartStopBitConfig_t , uint32_t );
extern UsartStatus_t UsartSendChar(char);
extern UsartStatus_t UsartSendString(char*, uint8_t*);
extern UsartStatus_t UsartSendString_S(const char*, uint8_t*);
#endif /* USART_H_ */