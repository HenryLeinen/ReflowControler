/*
 * board_config.h
 *
 * Created: 28.03.2014 01:26:52
 *  Author: Henry
 */ 


#ifndef BOARD_CONFIG_H_
#define BOARD_CONFIG_H_

#define LCD_DB4_PORT	(PORTA)
#define LCD_DB4_PORTIN	(PINA)
#define LCD_DB4_DDR		(DDRA)
#define LCD_DB4_PIN		4
#define LCD_DB5_PORT	(PORTA)
#define LCD_DB5_PORTIN	(PINA)
#define LCD_DB5_DDR		(DDRA)
#define LCD_DB5_PIN		5
#define LCD_DB6_PORT	(PORTA)
#define LCD_DB6_PORTIN	(PINA)
#define LCD_DB6_DDR		(DDRA)
#define LCD_DB6_PIN		6
#define LCD_DB7_PORT	(PORTA)
#define LCD_DB7_PORTIN	(PINA)
#define LCD_DB7_DDR		(DDRA)
#define LCD_DB7_PIN		7

#define LCD_RS_PORT		(PORTB)
#define LCD_RS_PORTIN	(PINB)
#define LCD_RS_DDR		(DDRB)
#define LCD_RS_PIN		0
#define LCD_CS_PORT		(PORTA)
#define LCD_CS_PORTIN	(PINA)
#define LCD_CS_DDR		(DDRA)
#define LCD_CS_PIN		3
#define LCD_RW_PORT		(PORTA)
#define LCD_RW_PORTIN	(PINA)
#define LCD_RW_DDR		(DDRA)
#define LCD_RW_PIN		2

#define TEMP_SENSE_P_PORT		(PORTA)
#define TEMP_SENSE_P_PORTIN	(PINA)
#define TEMP_SENSE_P_DDR		(DDRA)
#define TEMP_SENSE_P_PIN		0
#define TEMP_SENSE_N_PORT		(PORTA)
#define TEMP_SENSE_N_PORTIN		(PINA)
#define TEMP_SENSE_N_DDR		(DDRA)
#define TEMP_SENSE_N_PIN		1

#define RELAIS_OUT_PORT		(PORTD)
#define RELAIS_OUT_PORTIN	(PIND)
#define RELAIS_OUT_DDR		(DDRD)
#define RELAIS_OUT_PIN		6
#define BUZZER_OUT_PORT		(PORTB)
#define BUZZER_OUT_PORTIN	(PINB)
#define BUZZER_OUT_DDR		(DDRB)
#define BUZZER_OUT_PIN		3

#define ENC_A_PORT			(PORTC)
#define ENC_A_PORTIN		(PINC)
#define ENC_A_DDR			(DDRC)
#define ENC_A_PIN			2
#define ENC_B_PORT			(PORTC)
#define ENC_B_PORTIN		(PINC)
#define ENC_B_DDR			(DDRC)
#define ENC_B_PIN			3
#define ENC_SW_PORT			(PORTC)
#define ENC_SW_PORTIN		(PINC)
#define ENC_SW_DDR			(DDRC)
#define ENC_SW_PIN			4

#define DEBUG_PORT			(PORTD)
#define DEBUG_PORTIN		(PIND)
#define DEBUG_DDR			(DDRD)
#define DEBUG_PIN			7

#define SCL_PORT			(PORTC)
#define SCL_PORTIN			(PINC)
#define SCL_DDR				(DDRC)
#define SCL_PIN				0
#define SDA_PORT			(PORTC)
#define SDA_PORTIN			(PINC)
#define SDA_DDR				(DDRC)
#define SDA_PIN				1

#define LED_RED_PORT		(PORTD)
#define LED_RED_DDR			(DDRD)
#define LED_RED_PORTIN		(PIND)
#define LED_RED_PIN			2

#define LED_GRN_PORT		(PORTD)
#define LED_GRN_DDR			(DDRD)
#define LED_GRN_PORTIN		(PIND)
#define LED_GRN_PIN			3

#define TEMP_SENSE_SCK_PORT		(PORTB)
#define TEMP_SENSE_SCK_DDR		(DDRB)
#define TEMP_SENSE_SCK_PIN		7
#define TEMP_SENSE_MISO_PORT	(PORTB)
#define TEMP_SENSE_MISO_DDR		(DDRB)
#define TEMP_SENSE_MISO_PIN		6
#define TEMP_SENSE_CS_PORT		(PORTB)
#define TEMP_SENSE_CS_DDR		(DDRB)
#define TEMP_SENSE_CS_PORTIN	(PINB)
#define TEMP_SENSE_CS_PIN		1


#endif /* BOARD_CONFIG_H_ */