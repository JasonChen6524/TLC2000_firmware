/*
  UTFT.h - Arduino/chipKit library support for Color TFT LCD Boards
  Copyright (C)2010-2014 Henning Karlsen. All right reserved
  
  This library is the continuation of my ITDB02_Graph, ITDB02_Graph16
  and RGB_GLCD libraries for Arduino and chipKit. As the number of 
  supported display modules and controllers started to increase I felt 
  it was time to make a single, universal library as it will be much 
  easier to maintain in the future.

  Basic functionality of this library was origianlly based on the 
  demo-code provided by ITead studio (for the ITDB02 modules) and 
  NKC Electronics (for the RGB GLCD module/shield).

  This library supports a number of 8bit, 16bit and serial graphic 
  displays, and will work with both Arduino and chipKit boards. For a 
  full list of tested display modules and controllers, see the 
  document UTFT_Supported_display_modules_&_controllers.pdf.

  When using 8bit and 16bit display modules there are some 
  requirements you must adhere to. These requirements can be found 
  in the document UTFT_Requirements.pdf.
  There are no special requirements when using serial displays.

  You can always find the latest version of the library at 
  http://electronics.henningkarlsen.com/

  If you make any modifications or improvements to the code, I would 
  appreciate that you share the code with me so that I might include 
  it in the next release. I can be contacted through 
  http://electronics.henningkarlsen.com/contact.php.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the CC BY-NC-SA 3.0 license.
  Please see the included documents for further information.

  Commercial use of this library requires you to buy a license that
  will allow commercial use. This includes using the library,
  modified or not, as a tool to sell products.

  The license applies to all part of the library including the 
  examples and tools supplied with the library.
*/

#ifndef __UTFT_H
#define __UTFT_H

#include "stm32f10x.h"

#define UTFT_VERSION	279

#define LEFT 0
#define RIGHT 9999
#define CENTER 9998

#define PORTRAIT    0
#define LANDSCAPE   1

#define ILI9341_S5P		25
#define ILI9341_S4P		26

#define TFT01_22SP		25	// ILI9341	(Serial 5Pin)
#define TFT01_24SP		25	// ILI9341	(Serial 5Pin)

#define SERIAL_4PIN		4
#define SERIAL_5PIN		5
#define LATCHED_16		17

#define NOTINUSE		255

//*********************************
// COLORS
//*********************************
// VGA color palette
#define VGA_BLACK		    0x0000
#define VGA_WHITE		    0xFFFF
#define VGA_RED			    0xF800
#define VGA_GREEN		    0x0400
#define VGA_BLUE		    0x001F
#define VGA_SILVER		  0xC618
#define VGA_GRAY		    0x8410
#define VGA_MAROON		  0x8000
#define VGA_YELLOW		  0xFFE0
#define VGA_OLIVE		    0x8400
#define VGA_LIME		    0x07E0
#define VGA_AQUA		    0x07FF
#define VGA_TEAL		    0x0410
#define VGA_NAVY		    0x0010
#define VGA_FUCHSIA		  0xF81F
#define VGA_PURPLE		  0x8010
#define VGA_TRANSPARENT	0xFFFFFFFF

typedef struct _current_font
{
	uint8_t* font;
	uint8_t x_size;
	uint8_t y_size;
	uint8_t offset;
	uint8_t numchars;
} CURRENT_FONT;

extern uint8_t	 orient;
extern uint8_t	 _transparent;
extern uint32_t	 disp_x_size, disp_y_size;
extern void Delay(__IO uint32_t nCount);
extern CURRENT_FONT	cfont;
extern void DrawSmallLogoFlip(uint16_t x, uint16_t y);

void InitLCD(void);
void setFont(uint8_t* font);
void setXY(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void clrXY(void);
void drawHLine(int x, int y, int l);
void drawVLine(int x, int y, int l);
void	drawRect(int x1, int y1, int x2, int y2);
void	drawRoundRect(int x1, int y1, int x2, int y2);
void	drawPixel(int x, int y);
void setPixel(uint16_t color);
void	fillRect(int x1, int y1, int x2, int y2);
void	fillRoundRect(int x1, int y1, int x2, int y2);
void	fillCircle(int x, int y, int radius);
void	clrScr(void);
void	fillScrRGB(uint8_t r, uint8_t g, uint8_t b);
void	fillScr(uint16_t color);
void	setColorRGB(uint8_t r, uint8_t g, uint8_t b);
void	setColor(uint16_t color);
uint16_t	getColor(void);
void	setBackColorRGB(uint8_t r, uint8_t g, uint8_t b);
void	setBackColor(uint16_t color);
uint16_t	getBackColor(void);
void	drawLine(int x1, int y1, int x2, int y2);

void LCD_Write_DATA(uint8_t VH,uint8_t VL);
void	drawCircle(int x, int y, int radius);

void printChar(uint8_t c, int x, int y);
void rotateChar(uint8_t c, int x, int y, int pos, int deg);
void	print(uint8_t *st, int x, int y, int deg);
void	printNumI(long num, int x, int y, int length, char filler);
//void	printNumF(double num, uint8_t dec, int x, int y, char divider='.', int length=0, char filler=' ');
void	printNumF(double num, uint8_t dec, int x, int y, char divider, int length, char filler);
uint8_t* getFont(void);
uint8_t	getFontXsize(void);
uint8_t	getFontYsize(void);

void	drawBitmap1(int x, int y, int sx, int sy, uint16_t *data, int scale);
void	drawBitmap2(int x, int y, int sx, int sy, uint16_t *data, int deg, int rox, int roy);

void	lcdOff(void);
void	lcdOn(void);

void	setContrast(char c);
int		getDisplayXSize(void);
int		getDisplayYSize(void);

void	setBrightness(uint8_t br);
void	setDisplayPage(uint8_t page);
void	setWritePage(uint8_t page);

//void LCD_Writ_Bus(char VH,char VL, byte mode);
//void LCD_Write_COM(char VL);

//void LCD_Write_DATA(char VL);
//void LCD_Write_COM_DATA(char com1,int dat1);
//void _hw_special_init();

//void _set_direction_registers(uint8_t mode);
//void _fast_fill_16(int ch, int cl, long pix);
//void _fast_fill_8(int ch, long pix);
//void _convert_float(char *buf, double num, int width, uint8_t prec);

#endif


