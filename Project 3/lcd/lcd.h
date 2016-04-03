#ifndef _lcd_h_
#define _lcd_h_

#include <avr/io.h>
#include <stdio.h>
#include <avr/sfr_defs.h>

#include <util/delay.h>

// Access bits like variables
struct bits {
    uint8_t b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1;
} __attribute__((__packed__));
#define SBIT_(port,pin) ((*(volatile struct bits*)&port).b##pin)
#define SBIT(x,y)       SBIT_(x,y)

// force access of interrupt variables
#define IVAR(x)         (*(volatile typeof(x)*)&(x))


// always inline function x
#define AIL(x)          static x __attribute__ ((always_inline)); static x


// NOP
#define nop()           __asm__ volatile("nop"::)

#define LCD_2X16

/*                       define helper functions:                     */
#define     sbi(port, bit)   (port) |= (1 << (bit))
#define     cbi(port, bit)   (port) &= ~(1 << (bit))

#ifdef LCD_2X16
#define LCD_COLUMN      16
#define LCD_LINE        2
#define LCD_LINE1       0x80
#define LCD_LINE2       (0x80 + 0x40)
#endif

#define LCD_TIME_ENA    1.0             // 1µs
#define LCD_TIME_DAT    50.0            // 50µs
#define LCD_TIME_CLR    2000.0          // 2ms

/*                       define the button values:                         */
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5

/*                       define the LCD connections:                       */
#define LCD_D4      SBIT(PORTG, 5)
#define LCD_DDR_D4  SBIT(DDRG, 5)

#define LCD_D5      SBIT(PORTE, 3)
#define LCD_DDR_D5  SBIT(DDRE, 3)

#define LCD_D6      SBIT(PORTH, 3)
#define LCD_DDR_D6  SBIT(DDRH, 3)

#define LCD_D7      SBIT(PORTH, 4)
#define LCD_DDR_D7  SBIT(DDRH, 4)

#define LCD_RS      SBIT(PORTH, 5)
#define LCD_DDR_RS  SBIT(DDRH, 5)

#define LCD_E0      SBIT(PORTH, 6)
#define LCD_DDR_E0  SBIT(DDRH, 6)

void lcd_putchar(uint8_t d);
void lcd_init(void);
void lcd_puts(void *s);
void lcd_blank(uint8_t len);          // blank n digits
void lcd_command(uint8_t d);


#if 0
AIL( void lcd_xy( uint8_t x, uint8_t y ))       // always inline function 
{
#ifdef LCD_LINE2
  if( y == 1 )
    x += LCD_LINE2 - LCD_LINE1;  
#endif  
  lcd_command( x + LCD_LINE1 );
}
#else
#ifdef LCD_LINE2
#define lcd_xy(x, y)    lcd_command((x) + ((y==1) ? LCD_LINE2 : LCD_LINE1 ))
#else
#define lcd_xy(x, y)    lcd_command((x) + LCD_LINE1 ) 
#endif  

#endif

#endif
