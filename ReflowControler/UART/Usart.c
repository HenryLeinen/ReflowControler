/*
 * Usart.c
 *
 * Created: 25.03.2012 23:35:59
 *  Author: Henry
 */ 
#include <stddef.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "Usart.h"
#include <string.h>
#include "../main.h"

typedef uint8_t bool_t;
#define TRUE	(1==1)
#define FALSE	(1==0)

typedef enum { false = 0, true = 1 } bool;


#define SET_BIT(addr, bit)		((addr) |= (1<<(bit)))
#define CLR_BIT(addr, bit)		((addr) &= ~(1<<(bit)))
#define BIT_IS_SET(addr, bit)	(((addr) & (1<<(bit))) == (1<<(bit)))



/************************************************************************/
/* UART definition                                                      */
/************************************************************************/
#define UART_DEFAULT_RX_BUFFER_SIZE			50
#define UART_DEFAULT_TX_BUFFER_SIZE			50


/************************************************************************/
/* Clock Frequency definition                                           */
/************************************************************************/
#define F_OSC	8000000		//	clock Frequency is 3.6864MHz


/************************************************************************/
/* The receive Buffer for internal use                                                                     */
/************************************************************************/
volatile static char RxBuffer[UART_DEFAULT_RX_BUFFER_SIZE];
/************************************************************************/
/* The relative position of the next char in the rx buffer				*/
/************************************************************************/
volatile static uint8_t RxBufferPtr = 0;
/************************************************************************/
/* Flag for signalling a rx buffer overrun, means more characters have  */
/* been received than application has accepted.							*/
/************************************************************************/
volatile static bool_t RxBufferOverrun = false;

/************************************************************************/
/* The transmit buffer for internal use									*/
/************************************************************************/
static char TxBuffer[UART_DEFAULT_TX_BUFFER_SIZE];
/************************************************************************/
/* The relative position of the next char in the tx buffer				*/
/************************************************************************/
volatile static uint8_t TxBufferPtr = 0;
/************************************************************************/
/* The maximum number of characters to transmit							*/
/************************************************************************/
static uint8_t TxNumCharsToTransmit = 0;
/************************************************************************/
/* Flag indicating an active transmission								*/
/************************************************************************/
static bool_t TxBusy = false;

/************************************************************************/
/* RxFrame, DataOverrun and Parity Error Flags							*/
/************************************************************************/
volatile static bool_t RxFrameError = false;
volatile static bool_t RxDataOverrunError = false;
volatile static bool_t RxParityError = false;

/************************************************************************/
/* the following four items are the currently configured Uart params	*/
/************************************************************************/
static UsartBitConfig_t UartBitConfig;
static UsartParityConfig_t UartParityConfig;
static UsartStopBitConfig_t UartStopConfig;
static uint16_t UartBaudRate;



/************************************************************************/
/* When the DEBUG preprocessor directive is defined, UART activity		*/
/* will be indicated on the LEDs										*/
/************************************************************************/
//#define UART_DEBUG
#undef UART_DEBUG



/************************************************************************/
/* RX INTERRUPT.														*/
/* THe interrupt handler will first check whether an error has occured  */
/* afterwards the received character will be stored in the internal     */
/* buffer.																*/
/************************************************************************/
ISR(USART_RXC_vect)
{
	//	First read the status register to check for any errors detected by the device
	if (BIT_IS_SET(UCSRA, FE))
		RxFrameError = true;
	else
		RxFrameError = false;
		
	if (BIT_IS_SET(UCSRA, DOR))
		RxDataOverrunError = true;
	else
		RxDataOverrunError = false;
		
	if (BIT_IS_SET(UCSRA, PE))
		RxParityError = true;
	else
		RxParityError = false;
	
	//	Write the character received into the internal buffer
	RxBuffer[RxBufferPtr++] = UDR;
	
	//	Check for buffer overrun
	if (RxBufferPtr >= UART_DEFAULT_RX_BUFFER_SIZE)
	{
		RxBufferOverrun = true;
		RxBufferPtr = UART_DEFAULT_RX_BUFFER_SIZE-1; //	Overwrite last possible character in the array when receiving next character
	}		
	
#ifdef UART_DEBUG
	//	and invert pin 1
	if (PORTB&2)
		PORTB &= 0xFD;
	else
		PORTB |= 0x02;
	
	//	toggle pin 2 if error occured
	if (RxFrameError)
		{
			if (PORTB&4)
				PORTB &= 0xFB;
			else
				PORTB |= 0x04;
		}
	if ( RxDataOverrunError )
		{
			if (PORTB&8)
				PORTB &= 0xF7;
			else
				PORTB |= 0x08;
		}
	if ( RxParityError )
		{
			if (PORTB&16)
				PORTB &= 0xef;
			else
				PORTB |= 0x10;
		}
#endif
}


/************************************************************************/
/* TX INTERRUPT															*/
/* The interrupt handler will be enabled by the Data Register Empty     */
/* interrupt handler only after sending the last character. This is done*/
/* so that the TX complete interrupt handler can finalize the sending   */
/* process.																*/
/************************************************************************/
ISR(USART_TXC_vect)
{
	//	Disable transmitter, transmit complete interrupt and data register empty interrupt handler
	UCSRB = UCSRB & (~((1<<TXEN)|(1<<UDRIE)|(1<<TXCIE)));
	
	//	Reset counter and transmit buffer pointer
	TxBufferPtr = 0;
	TxNumCharsToTransmit = 0;
	
	//	Indicate that no transmission is currently in progress.
	TxBusy = false;
}


/************************************************************************/
/* UDRE INTERRUPT														*/
/* Will put the next character into the data register and in case this  */
/* is the last character to send, the data register emtpy interrupt will*/
/* be disabled and instead the transmit complete interrupt will be en-  */
/* abled to finalize the sending process.								*/
/************************************************************************/
ISR(USART_UDRE_vect)
{
	//	Send next character and advance buffer pointer
	UDR = TxBuffer[TxBufferPtr++];
	
	if (TxBufferPtr >= TxNumCharsToTransmit)
	{
		//	Transmission is over after the current character
		//	Enable Transmission complete interrupt to completely disable the transmitter
		UCSRB = (UCSRB | (1<<TXEN) | (1<<TXCIE)) &  (~(1<<UDRIE));
	}
}



/************************************************************************/
/* Function UsartSetup													*/
/*	Allows to setup the UART accordingly using the given parameters     */
/*  This function will return UsartOk if successfull or UsartBusy if    */
/*  a sending process is currently ongoing								*/
/************************************************************************/
UsartStatus_t UsartSetup(UsartBitConfig_t ubitconfig, UsartParityConfig_t uparityconfig, UsartStopBitConfig_t ustopconfig, uint32_t baudrate)
{
	uint32_t baudrateregister;
	
	//	Check whether the device is idle
	if (!TxBusy)
	{
		//	Store the parameters locally
		UartBaudRate = baudrate;
		UartParityConfig = uparityconfig;
		UartBitConfig = ubitconfig;
		UartStopConfig = ustopconfig;
	
		//	Wait for any transmissions to be completed
		//	Set the baudrate
		baudrateregister = (F_OSC >> 4)/baudrate -1;
		UBRRH = (uint8_t)(baudrateregister>>8);
		UBRRL = (uint8_t)(baudrateregister);

		//	Enable transmitter and receiver, also enable the receive interrupt
		UCSRB = (1<<RXEN) | (1<<TXEN) | (1<<RXCIE) | (ubitconfig&0x04);
	
		//	Set the frame format
		UCSRC =  (1<<URSEL) | ((ubitconfig&0x03)<<UCSZ0) | (uparityconfig<<UPM0) | (ustopconfig<<USBS);
		return UsartOk;
	}
	return UsartBusy;
}


/************************************************************************/
/* Function UsartSendChar												*/
/*	Allows to send a single character on UART							*/
/*	FUnction returns UsartOk if ok, or UsartBusy if another sending     */
/*	process is ongoing.													*/
/************************************************************************/
UsartStatus_t UsartSendChar(char c)
{
	if (!TxBusy)
	{
		/*	Wait for the transmit buffer to be empty */
		while ( !(UCSRA & (1<<UDRE)))
			;	
	
		//	Put data in buffer
		UDR = c;
	
		//	Enable Transmitter and transmitter interrupt
	//	UCSRB = UCSRB | (1<<TXEN);
	
		return UsartOk;
	}
	return UsartBusy;
}


/************************************************************************/
/* Function UsartSendString												*/
/*	Allows to send a string of data characters over Uart.				*/
/*	FUnction will return UsartBusy if another sending process is on-	*/
/*	going or UsartOk if sending process has been started successfully.  */
/*  It returns UsartTooLong if the given data does not fit into the     */
/*	internal data buffer. In this particular case, the number of char-	*/
/*	acters that could be successfully copied into the buffer will be	*/
/*	written back into the parameter len.								*/
/************************************************************************/
UsartStatus_t UsartSendString(char* s, uint8_t* len)
{
	UsartStatus_t	retVal = UsartOk;
	
	if (TxBusy)
		return UsartBusy;

	if ((s == NULL) || (len == NULL))
		return UsartInvalidParameter;
		
	//	Check if length is ok
	if (*len >= UART_DEFAULT_TX_BUFFER_SIZE)
	{
		*len = UART_DEFAULT_TX_BUFFER_SIZE;
		retVal = UsartTooLong;
	}		
	
	//	Just one char to send ?
	if (1 == *len)
	{
		//	Then use the single char function
		UsartSendChar(s[0]);
	}
	else
	{
		//	Copy the string into the internal buffer
		memcpy(TxBuffer, s, *len);

		//	Indicate we're busy
		TxBusy = true;
	
		//	Reset internal values
		TxBufferPtr = 0;
		TxNumCharsToTransmit = *len;
	
		//	Copy first character into the transmit buffer
		UDR = TxBuffer[TxBufferPtr++];
		
		//	Enable transmitter and data register empty interrupt
		UCSRB = UCSRB | (1<<TXEN) | (1<<UDRIE);
	}		
	
	
	return retVal;
}



/************************************************************************/
/* Function UsartSendString_S												*/
/*	Allows to send a string of data characters over Uart.				*/
/*	FUnction will return UsartBusy if another sending process is on-	*/
/*	going or UsartOk if sending process has been started successfully.  */
/*  It returns UsartTooLong if the given data does not fit into the     */
/*	internal data buffer. In this particular case, the number of char-	*/
/*	acters that could be successfully copied into the buffer will be	*/
/*	written back into the parameter len.								*/
/************************************************************************/
UsartStatus_t UsartSendString_S(const char* s, uint8_t* len)
{
	UsartStatus_t	retVal = UsartOk;
	
	if (TxBusy)
	return UsartBusy;

	if ((s == NULL) || (len == NULL))
	return UsartInvalidParameter;
	
	//	Check if length is ok
	if (*len >= UART_DEFAULT_TX_BUFFER_SIZE)
	{
		*len = UART_DEFAULT_TX_BUFFER_SIZE;
		retVal = UsartTooLong;
	}
	
	//	Just one char to send ?
	if (1 == *len)
	{
		//	Then use the single char function
		UsartSendChar(pgm_read_byte(s));
	}
	else
	{
		//	Copy the string into the internal buffer
		memcpy_P(TxBuffer, s, *len);

		//	Indicate we're busy
		TxBusy = true;
		
		//	Reset internal values
		TxBufferPtr = 0;
		TxNumCharsToTransmit = *len;
		
		//	Copy first character into the transmit buffer
		UDR = TxBuffer[TxBufferPtr++];
		
		//	Enable transmitter and data register empty interrupt
		UCSRB = UCSRB | (1<<TXEN) | (1<<UDRIE);
	}
	
	
	return retVal;
}