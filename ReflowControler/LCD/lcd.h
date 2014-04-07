/*
 * lcd.h
 *
 * Created: 23.03.2014 16:28:19
 *  Author: Henry
 */ 
#ifndef __LCD_H__
#define __LCD_H__

#if __cplusplus
extern "C" {
#endif

typedef enum { Lcd_Display_Off, Lcd_Display_On } LcdDisplay_t;
typedef enum { Lcd_Cursor_Off, Lcd_Cursor_On, Lcd_Cursor_Blink } LcdCursor_t;
typedef enum { Lcd_Font_5x7, Lcd_Font_5x10} LcdFont_t;
	
extern void lcd_init(void);
extern void lcd_cls(void);
extern void lcd_locate(uint8_t x, uint8_t y);
extern void lcd_write(const char * txt);
extern void lcd_write_s(const char * txt);
extern void lcd_write_c(char c);
extern void lcd_write_n(const char *txt, uint8_t len);

extern void lcd_set_font( LcdFont_t );
extern void lcd_enable_display( LcdDisplay_t , LcdCursor_t);

#if __cplusplus
}
#endif

#endif