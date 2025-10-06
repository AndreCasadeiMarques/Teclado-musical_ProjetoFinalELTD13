#ifndef __LCD__
#define __LCD__

#include "stm32f10x.h"

#define LCD_RS 15
#define LCD_EN 12
#define LCD4 8
#define LCD5 6
#define LCD6 5
#define LCD7 11

void lcd_init(void);
void lcd_command(unsigned char cmd);
void lcd_data(unsigned char data);
void lcd_print(char *str);
void lcd_putValue(unsigned char value);
void delay_us(uint16_t t);
void delay_ms(uint16_t t);

#endif
