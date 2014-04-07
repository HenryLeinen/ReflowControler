/*
 * lcd.c
 *
 * Created: 23.03.2014 16:28:44
 *  Author: Henry
 */ 
#include <avr/common.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "../GPIO\gpio.h"
#include "lcd.h"
#include "../main.h"

static uint8_t pos_x, pos_y;

/****************************************************/
/*													*/
/* Private lcd functions							*/
/*													*/
/****************************************************/
static void lcd_write_data8(uint8_t data)
{
	/* Instruction code */
	gpio_set_pin_level(LCD_RS, 0);
	/* Read / Write code */
	gpio_set_pin_level(LCD_RW, 0);
	/* unselect chip */
	gpio_set_pin_level(LCD_CS, 1);
	_delay_us(10);
	/* write data */
	gpio_set_pin_level(LCD_DB7, ((data&0x80)>>7));
	gpio_set_pin_level(LCD_DB7, ((data&0x80)>>7));
	gpio_set_pin_level(LCD_DB6, ((data&0x40)>>6));
	gpio_set_pin_level(LCD_DB5, ((data&0x20)>>5));
	gpio_set_pin_level(LCD_DB4, ((data&0x10)>>4));
	/* select chip */
	_delay_us(10);
	gpio_set_pin_level(LCD_CS, 0);
	
	_delay_us(10);
	
	/* done */
}

static void lcd_write_data(uint8_t data, uint8_t wr_dta)
{
	/* Instruction code */
	gpio_set_pin_level(LCD_RS, wr_dta );			// Instruction
	/* Read / Write code */
	gpio_set_pin_level(LCD_RW, 0);			// write
	/* unselect chip */
	gpio_set_pin_level(LCD_CS, 1);				//	unselect
	/* write data */
	gpio_set_pin_level(LCD_DB7, ((data&0x80)>>7));
	gpio_set_pin_level(LCD_DB6, ((data&0x40)>>6));
	gpio_set_pin_level(LCD_DB5, ((data&0x20)>>5));
	gpio_set_pin_level(LCD_DB4, ((data&0x10)>>4));
	_delay_us(10);
	/* select chip */
	gpio_set_pin_level(LCD_CS, 0);
	
	_delay_us(10);
	
	/* unselect chip */
	gpio_set_pin_level(LCD_CS, 1);
	_delay_us(10);
	/* write data */
	gpio_set_pin_level(LCD_DB7, ((data&0x08)>>3));
	gpio_set_pin_level(LCD_DB6, ((data&0x04)>>2));
	gpio_set_pin_level(LCD_DB5, ((data&0x02)>>1));
	gpio_set_pin_level(LCD_DB4, ((data&0x01)>>0));
	/* select chip */
	_delay_us(10);
	gpio_set_pin_level(LCD_CS, 0);
	
	_delay_us(10);
	/* done */
}

void lcd_init(void)
{
	pos_x = pos_y = 0;
	
	_delay_ms(15);
	/* Set 4-bit interface, 2-line display, 5x7 dots */
	lcd_write_data8(0x28);
	_delay_us(4100);
	/* Set 4-bit interface, 2-line display, 5x7 dots */
	lcd_write_data(0x28, 0);
	_delay_us(100);
	/* Set display on, cursor on, cursor blinking off */
	lcd_write_data(0x0e, 0);
	_delay_us(42);
	/* Set shift mode, cursor move and right shift */
	lcd_write_data(0x06, 0);
	_delay_us(42);
	/* Clear screen */
	lcd_write_data(0x01, 0);
	_delay_us(1640);
}

void lcd_cls(void)
{
	lcd_write_data(0x01, 0);
	_delay_us(1640);
	pos_x = pos_y = 0;
}

void lcd_locate(uint8_t x, uint8_t y)
{
	/* Set DD address */
	lcd_write_data(0x80 | (x + y*0x40), 0);
	_delay_us(42);
	pos_x = x;
	pos_y = y;
}

void lcd_write(const char *txt)
{
	while(*txt) {
		if (*txt == '\n') {
			if (pos_y == 1)
				lcd_locate(0, 0);
			else
				lcd_locate(0,1);
			txt++;
		} else {
			lcd_write_data(*txt++, 1);
			_delay_us(46);
			pos_x++;
		}
	}
}

void lcd_write_n(const char *txt, uint8_t len)
{
	while(len--)
	{
		if (*txt == '\n') {
			if (pos_y == 1)
				lcd_locate(0, 0);
			else
				lcd_locate(0,1);
			txt++;
		} else {
			lcd_write_data(*txt++, 1);
			_delay_us(46);
			pos_x++;
		}
	}
}

void lcd_write_c(char c)
{
	if (c == '\n')
	{
		if (pos_y == 1)
			lcd_locate(0,0);
		else
			lcd_locate(0,1);
	} else
	{
		lcd_write_data(c, 1);
		_delay_us(46);
		pos_x++;
	}
}

void lcd_write_s(const char *txt)
{
	char c;
	while((c = pgm_read_byte(txt)) != 0) {
		if (c == '\n') {
			if (pos_y == 1)
				lcd_locate(0,0);
			else
				lcd_locate(0,1);
			txt++;
		} else {
			lcd_write_data(c, 1);
			txt++;
			_delay_us(46);
			pos_x++;
		}
	}	
}

void lcd_set_font( LcdFont_t fnt )
{
	lcd_write_data(0x28 | (fnt == Lcd_Font_5x7 ? 0 : 1),0);
	_delay_us(42);
}

void lcd_enable_display( LcdDisplay_t dspl, LcdCursor_t crsr)
{
	lcd_write_data(0x08 | (dspl == Lcd_Display_On ? 4 : 0) | (crsr == Lcd_Cursor_Blink ? 3 : (crsr == Lcd_Cursor_On ? 2 : 0)),0);
	_delay_us(42);	
}
