/*
  UTFT.cpp - Arduino/chipKit library support for Color TFT LCD Boards
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
#include <math.h>
#include "TLC2000_SPI_LCD_TFT.h"
#include "UTFT.h"
//#include "memorysaver.h"


/*
The functions and variables below should not normally be used.
They have been left publicly available for use in add-on libraries
that might need access to the lower level functions of UTFT.

Please note that these functions and variables are not documented
and I do not provide support on how to use them.
*/
uint8_t			fch, fcl, bch, bcl;
uint8_t			orient;
uint32_t			disp_x_size, disp_y_size;
uint8_t			display_model, display_transfer_mode, display_serial_mode;
__IO uint8_t *P_RS, *P_WR, *P_CS, *P_RST, *P_SDA, *P_SCL, *P_ALE;
//regsize			B_RS, B_WR, B_CS, B_RST, B_SDA, B_SCL, B_ALE;
//byte			__p1, __p2, __p3, __p4, __p5;
CURRENT_FONT	cfont;
uint8_t			_transparent;

void InitLCD(void)
{
	orient = 0;//orientation;
	
	disp_x_size =			239;
  disp_y_size =			319;

  sLCD_init_TFT();
	
	setColorRGB(255, 255, 255);
	setBackColorRGB(0, 0, 0);
	cfont.font=0;
	_transparent = 0;
}

void setFont(uint8_t* font)
{
	cfont.font = &font[0];
	cfont.x_size=font[0];
	cfont.y_size=font[1];
	cfont.offset=font[2];
	cfont.numchars=font[3];
}

void setXY(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
   sLCD_Write_Command_TFT(0x2A); //column address set
   sLCD_Write_Data_TFT(x1>>8);
   sLCD_Write_Data_TFT(x1>>0); //start 0x0000
   sLCD_Write_Data_TFT(x2>>8);
   sLCD_Write_Data_TFT(x2>>0); //end 0x00EF
	 
   sLCD_Write_Command_TFT(0x2B); //page address set
   sLCD_Write_Data_TFT(y1>>8);
   sLCD_Write_Data_TFT(y1>>0); //start 0x0000
   sLCD_Write_Data_TFT(y2>>8);
   sLCD_Write_Data_TFT(y2>>0); //end 0x013F	
	
	 sLCD_Write_Command_TFT(0x2C); //Write
}

void clrXY(void)
{
	setXY(0,0,disp_x_size,disp_y_size);
}

void drawHLine(int x, int y, int l)
{
	int i;
	if (l<0)
	{
		l = -l;
		x -= l;
	}
	setXY(x, y, x+l, y);
	for (i=0; i<l+1; i++)
	{
#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
      /* Reload IWDG counter */
      IWDG_ReloadCounter();  
#endif  				
  	sLCD_Write_Data_TFT((uint8_t)((foreground_color >> 8)&0xFF));		
		sLCD_Write_Data_TFT((uint8_t)((foreground_color >> 0)&0xFF));					
	}

	clrXY();
}

void drawVLine(int x, int y, int l)
{
	int i;
	if (l<0)
	{
		l = -l;
		y -= l;
	}
	setXY(x, y, x, y+l);
	for (i=0; i<l+1; i++)
	{
#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
      /* Reload IWDG counter */
      IWDG_ReloadCounter();  
#endif  				
  	sLCD_Write_Data_TFT((uint8_t)((foreground_color >> 8)&0xFF));		
		sLCD_Write_Data_TFT((uint8_t)((foreground_color >> 0)&0xFF));					
	}
	clrXY();
}

void drawRect(int x1, int y1, int x2, int y2)
{
	if (x1>x2)
	{
		//swap(int, x1, x2);
	}
	if (y1>y2)
	{
		//swap(int, y1, y2);
	}

	drawHLine(x1, y1, x2-x1);
	drawHLine(x1, y2, x2-x1);
	drawVLine(x1, y1, y2-y1);
	drawVLine(x2, y1, y2-y1);
}

void setPixel(uint16_t color)
{
  	sLCD_Write_Data_TFT((uint8_t)((color >> 8)&0xFF));		
		sLCD_Write_Data_TFT((uint8_t)((color >> 0)&0xFF));					
}


void drawPixel(int x, int y)
{
	setXY(x, y, x, y);
	setPixel((fch<<8)|fcl);
	clrXY();
}

void drawRoundRect(int x1, int y1, int x2, int y2)
{
	if (x1>x2)
	{
		//swap(int, x1, x2);
	}
	if (y1>y2)
	{
		//swap(int, y1, y2);
	}
	if ((x2-x1)>4 && (y2-y1)>4)
	{
		drawPixel(x1+1,y1+1);
		drawPixel(x2-1,y1+1);
		drawPixel(x1+1,y2-1);
		drawPixel(x2-1,y2-1);
		drawHLine(x1+2, y1, x2-x1-4);
		drawHLine(x1+2, y2, x2-x1-4);
		drawVLine(x1, y1+2, y2-y1-4);
		drawVLine(x2, y1+2, y2-y1-4);
	}
}

void fillRect(int x1, int y1, int x2, int y2)
{
	int i;
	if (x1>x2)
	{
		//swap(int, x1, x2);
	}
	if (y1>y2)
	{
		//swap(int, y1, y2);
	}
	for (i=0; i<((y2-y1)/2)+1; i++)
	{
		drawHLine(x1, y1+i, x2-x1);
		drawHLine(x1, y2-i, x2-x1);
	}
	//for (int i=0; i<((x2-x1)/2)+1; i++)
	//{
	//	drawVLine(x1+i, y1, y2-y1);
	//	drawVLine(x2-i, y1, y2-y1);
	//}
}

void fillRoundRect(int x1, int y1, int x2, int y2)
{
	int i;
	if (x1>x2)
	{
		//swap(int, x1, x2);
	}
	if (y1>y2)
	{
		//swap(int, y1, y2);
	}

	if ((x2-x1)>4 && (y2-y1)>4)
	{
		for (i=0; i<((y2-y1)/2)+1; i++)
		{
			switch(i)
			{
			case 0:
				drawHLine(x1+2, y1+i, x2-x1-4);
				drawHLine(x1+2, y2-i, x2-x1-4);
				break;
			case 1:
				drawHLine(x1+1, y1+i, x2-x1-2);
				drawHLine(x1+1, y2-i, x2-x1-2);
				break;
			default:
				drawHLine(x1, y1+i, x2-x1);
				drawHLine(x1, y2-i, x2-x1);
			}
		}
	}
}

void LCD_Write_DATA(uint8_t VH,uint8_t VL)
{
	 sLCD_Write_Data_TFT(VH);		
	 sLCD_Write_Data_TFT(VL);
}

void drawCircle(int x, int y, int radius)
{
	int f = 1 - radius;
	int ddF_x = 1;
	int ddF_y = -2 * radius;
	int x1 = 0;
	int y1 = radius;
 
	setXY(x, y + radius, x, y + radius);
	LCD_Write_DATA(fch,fcl);
	setXY(x, y - radius, x, y - radius);
	LCD_Write_DATA(fch,fcl);
	setXY(x + radius, y, x + radius, y);
	LCD_Write_DATA(fch,fcl);
	setXY(x - radius, y, x - radius, y);
	LCD_Write_DATA(fch,fcl);
 
	while(x1 < y1)
	{
#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
      /* Reload IWDG counter */
      IWDG_ReloadCounter();  
#endif  				
		if(f >= 0) 
		{
			y1--;
			ddF_y += 2;
			f += ddF_y;
		}
		x1++;
		ddF_x += 2;
		f += ddF_x;    
		setXY(x + x1, y + y1, x + x1, y + y1);
		LCD_Write_DATA(fch,fcl);
		setXY(x - x1, y + y1, x - x1, y + y1);
		LCD_Write_DATA(fch,fcl);
		setXY(x + x1, y - y1, x + x1, y - y1);
		LCD_Write_DATA(fch,fcl);
		setXY(x - x1, y - y1, x - x1, y - y1);
		LCD_Write_DATA(fch,fcl);
		setXY(x + y1, y + x1, x + y1, y + x1);
		LCD_Write_DATA(fch,fcl);
		setXY(x - y1, y + x1, x - y1, y + x1);
		LCD_Write_DATA(fch,fcl);
		setXY(x + y1, y - x1, x + y1, y - x1);
		LCD_Write_DATA(fch,fcl);
		setXY(x - y1, y - x1, x - y1, y - x1);
		LCD_Write_DATA(fch,fcl);
	}
	clrXY();
}

void fillCircle(int x, int y, int radius)
{
	int y1,x1;
	for(y1=-radius; y1<=0; y1++) 
		for(x1=-radius; x1<=0; x1++)
			if(x1*x1+y1*y1 <= radius*radius) 
			{
				drawHLine(x+x1, y+y1, 2*(-x1));
				drawHLine(x+x1, y-y1, 2*(-x1));
				break;
			}
}

void clrScr(void)
{
	//long i;
	clrXY();
  sLCD_background_TFT(background_color, foreground_color);  	
}

void fillScr(uint16_t color)
{
  sLCD_background_TFT(color, foreground_color);  	
}

void fillScrRGB(uint8_t r, uint8_t g, uint8_t b)
{
	uint16_t color = ((r&248)<<8 | (g&252)<<3 | (b&248)>>3);
	fillScr(color);
}

void setColorRGB(uint8_t r, uint8_t g, uint8_t b)
{
	fch=((r&0xF8)|g>>5);//((r&248)|g>>5);
	fcl=((g&0x1C)<<3|b>>3);  //((g&28)<<3|b>>3);
	
	foreground_color = fch;
	foreground_color = (foreground_color << 8)&0xFF00;
	foreground_color |= fcl;
}

void setColor(uint16_t color)
{
	fch=(uint8_t)(color>>8);
	fcl=(uint8_t)(color & 0xFF);
	foreground_color = color;
}

uint16_t getColor(void)
{
	return (fch<<8) | fcl;
}

void setBackColorRGB(uint8_t r, uint8_t g, uint8_t b)
{
	bch=((r&248)|g>>5);
	bcl=((g&28)<<3|b>>3);
	
	background_color = bch;
	background_color = (foreground_color << 8)&0xFF00;
	background_color |= bcl;
	_transparent=0;
}

void setBackColor(uint16_t color)
{
	//if (color == VGA_TRANSPARENT)
	//	_transparent=1;
	//else
	{
		bch=(uint8_t)(color>>8);
		bcl=(uint8_t)(color & 0xFF);
		background_color = color;
		_transparent=0;
	}
}

uint16_t getBackColor(void)
{
	return (bch<<8) | bcl;
}

void drawLine(int x1, int y1, int x2, int y2)
{
	if (y1==y2)
		drawHLine(x1, y1, x2-x1);
	else if (x1==x2)
		drawVLine(x1, y1, y2-y1);
	else
	{
		unsigned int	dx = (x2 > x1 ? x2 - x1 : x1 - x2);
		short			xstep =  x2 > x1 ? 1 : -1;
		unsigned int	dy = (y2 > y1 ? y2 - y1 : y1 - y2);
		short			ystep =  y2 > y1 ? 1 : -1;
		int				col = x1, row = y1;

		if (dx < dy)
		{
			int t = - (dy >> 1);
			while (1)
			{
#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
      /* Reload IWDG counter */
      IWDG_ReloadCounter();  
#endif  						
				setXY (col, row, col, row);
				LCD_Write_DATA (fch, fcl);
				if (row == y2)
					return;
				row += ystep;
				t += dx;
				if (t >= 0)
				{
					col += xstep;
					t   -= dy;
				}
			} 
		}
		else
		{
			int t = - (dx >> 1);
			while (1)
			{
#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
      /* Reload IWDG counter */
      IWDG_ReloadCounter();  
#endif  						
				setXY (col, row, col, row);
				LCD_Write_DATA (fch, fcl);
				if (col == x2)
					return;
				col += xstep;
				t += dy;
				if (t >= 0)
				{
					row += ystep;
					t   -= dx;
				}
			} 
		}
	}
	clrXY();
}

//uint8_t chp;
//uint16_t tempp;
void printChar(uint8_t c, int x, int y)
{
	uint8_t i,ch;
	uint16_t j,kk;
	uint16_t temp; 
	int zz;
  
	if (!_transparent)
	{
		if (orient==PORTRAIT)
		{
			setXY(x,y,x+cfont.x_size-1,y+cfont.y_size-1);
  
			temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;
			kk = (cfont.x_size/8)*cfont.y_size;
			for(j=0;j<kk;j++)
			{
#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
      /* Reload IWDG counter */
      IWDG_ReloadCounter();  
#endif  					
				//ch=pgm_read_byte(&cfont.font[temp]);
				ch=cfont.font[temp+kk-j];
				for(i=0;i<8;i++)
				{   						
					//if((chp&(1<<(7-i)))!=0)   
					if((ch&(0x80>>i))!=0)   
					{
						setPixel((fch<<8)|fcl);
					} 
					else
					{
						setPixel((bch<<8)|bcl);
					}   
				}
				//temp++;
			}
		}
		else
		{
			temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;

			for(j=0;j<((cfont.x_size/8)*cfont.y_size);j+=(cfont.x_size/8))
			{
#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
      /* Reload IWDG counter */
      IWDG_ReloadCounter();  
#endif  					
				setXY(x,y+(j/(cfont.x_size/8)),x+cfont.x_size-1,y+(j/(cfont.x_size/8)));
				for (zz=(cfont.x_size/8)-1; zz>=0; zz--)
				{
					//ch=pgm_read_byte(&cfont.font[temp+zz]);
					ch=cfont.font[temp+zz];
					for(i=0;i<8;i++)
					{   							
						if((ch&(1<<i))!=0)   
						{
							setPixel((fch<<8)|fcl);
						} 
						else
						{
							setPixel((bch<<8)|bcl);
						}   
					}
				}
				temp+=(cfont.x_size/8);
			}
		}
	}
	else
	{
		temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;
		for(j=0;j<cfont.y_size;j++) 
		{
			for (zz=0; zz<(cfont.x_size/8); zz++)
			{
#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
      /* Reload IWDG counter */
      IWDG_ReloadCounter();  
#endif  					
				//ch=pgm_read_byte(&cfont.font[temp+zz]); 
				ch=cfont.font[temp+zz];
				for(i=0;i<8;i++)
				{   						
					setXY(x+i+(zz*8),y+j,x+i+(zz*8)+1,y+j+1);
				
					if((ch&(1<<(7-i)))!=0)   
					{
						setPixel((fch<<8)|fcl);
					} 
				}
			}
			temp+=(cfont.x_size/8);
		}
	}

  clrXY();
}

void rotateChar(uint8_t c, int x, int y, int pos, int deg)
{
	uint8_t i,j,ch;
	uint16_t temp; 
	int newx,newy,zz;
	double radian;
	radian=deg*0.0175;  

	temp=((c-cfont.offset)*((cfont.x_size/8)*cfont.y_size))+4;
	for(j=0;j<cfont.y_size;j++) 
	{
#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
      /* Reload IWDG counter */
      IWDG_ReloadCounter();  
#endif		
		for (zz=0; zz<(cfont.x_size/8); zz++)
		{
			//ch=pgm_read_byte(&cfont.font[temp+zz]); 
			ch=cfont.font[temp+zz];
			for(i=0;i<8;i++)
			{    						
				newx=x+(((i+(zz*8)+(pos*cfont.x_size))*cos(radian))-((j)*sin(radian)));
				newy=y+(((j)*cos(radian))+((i+(zz*8)+(pos*cfont.x_size))*sin(radian)));

				setXY(newx,newy,newx+1,newy+1);
				
				if((ch&(1<<(7-i)))!=0)   
				{
					setPixel((fch<<8)|fcl);
				} 
				else  
				{
					if (!_transparent)
						setPixel((bch<<8)|bcl);
				}   
			}
		}
		temp+=(cfont.x_size/8);
	}
	clrXY();
}

uint8_t *pStrTFT;
void print(uint8_t *st, int x, int y, int deg)
{
	int i;//stl;
	pStrTFT = st;

	//stl = strlen(st);
	//stl = sizeof(st);
	//for (i=0; i<stl; i++)
	i = 0;
	while(*pStrTFT)
	{
		if (deg==0)
			printChar(*pStrTFT, x + (i*(cfont.x_size)), y);
		else
			rotateChar(*st++, x, y, i, deg);
		
		pStrTFT++;
		i++;
	}
}


void printNumI(long num, int x, int y, int length, char filler)
{
	char buf[25];
	uint8_t st[27];
	uint8_t neg=0;
	int c=0, f=0;
	int i;
  
	if (num==0)
	{
		if (length!=0)
		{
			for (c=0; c<(length-1); c++)
				st[c]=filler;
			st[c]=48;
			st[c+1]=0;
		}
		else
		{
			st[0]=48;
			st[1]=0;
		}
	}
	else
	{
		if (num<0)
		{
			neg=1;
			num=-num;
		}
	  
		while (num>0)
		{
			buf[c]=48+(num % 10);
			c++;
			num=(num-(num % 10))/10;
		}
		buf[c]=0;
	  
		if (neg)
		{
			st[0]=45;
		}
	  
		if (length>(c+neg))
		{
			for (i=0; i<(length-c-neg); i++)
			{
				st[i+neg]=filler;
				f++;
			}
		}

		for (i=0; i<c; i++)
		{
			st[i+neg+f]=buf[c-i-1];
		}
		st[c+neg+f]=0;

	}

	print(st,x,y,0);
}

void printNumF(double num, uint8_t dec, int x, int y, char divider, int length, char filler)
{
	uint8_t st[27];
	uint8_t neg=0;
	int i;

	if (dec<1)
		dec=1;
	else if (dec>5)
		dec=5;

	if (num<0)
		neg = 1;

	//_convert_float(st, num, length, dec);

	if (divider != '.')
	{
		for (i=0; i<sizeof(st); i++)
			if (st[i]=='.')
				st[i]=divider;
	}

	if (filler != ' ')
	{
		if (neg)
		{
			st[0]='-';
			for (i=1; i<sizeof(st); i++)
				if ((st[i]==' ') || (st[i]=='-'))
					st[i]=filler;
		}
		else
		{
			for (i=0; i<sizeof(st); i++)
				if (st[i]==' ')
					st[i]=filler;
		}
	}

	print(st,x,y,0);
}



uint8_t* getFont(void)
{
	return cfont.font;
}

uint8_t getFontXsize(void)
{
	return cfont.x_size;
}

uint8_t getFontYsize(void)
{
	return cfont.y_size;
}

void drawBitmap1(int x, int y, int sx, int sy, uint16_t* data, int scale)
{
	unsigned int col;
	int tx, ty, tc, tsx, tsy;

	if (scale==1)
	{
		if (orient==PORTRAIT)
		{
			setXY(x, y, x+sx-1, y+sy-1);
			for (tc=0; tc<(sx*sy); tc++)
			{
#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
      /* Reload IWDG counter */
      IWDG_ReloadCounter();  
#endif  						
				//col=pgm_read_word(&data[tc]);
				col=data[tc];
				LCD_Write_DATA(col>>8,col & 0xff);
			}
		}
		else
		{
			for (ty=0; ty<sy; ty++)
			{
#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
      /* Reload IWDG counter */
      IWDG_ReloadCounter();  
#endif  						
				setXY(x, y+ty, x+sx-1, y+ty);
				for (tx=sx-1; tx>=0; tx--)
				{
					//col=pgm_read_word(&data[(ty*sx)+tx]);
					col=data[(ty*sx)+tx];
					LCD_Write_DATA(col>>8,col & 0xff);
				}
			}
		}
	}
	else
	{
		if (orient==PORTRAIT)
		{
			for (ty=0; ty<sy; ty++)
			{
#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
      /* Reload IWDG counter */
      IWDG_ReloadCounter();  
#endif				
				setXY(x, y+(ty*scale), x+((sx*scale)-1), y+(ty*scale)+scale);
				for (tsy=0; tsy<scale; tsy++)
					for (tx=0; tx<sx; tx++)
					{  								
						//col=pgm_read_word(&data[(ty*sx)+tx]);
						col=data[(ty*sx)+tx];
						for (tsx=0; tsx<scale; tsx++)
							LCD_Write_DATA(col>>8,col & 0xff);
					}
			}
		}
		else
		{
  		for (ty=0; ty<sy; ty++)
			{
#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
      /* Reload IWDG counter */
      IWDG_ReloadCounter();  
#endif				
				for (tsy=0; tsy<scale; tsy++)
				{
					setXY(x, y+(ty*scale)+tsy, x+((sx*scale)-1), y+(ty*scale)+tsy);
					for (tx=sx-1; tx>=0; tx--)
					{  								
						//col=pgm_read_word(&data[(ty*sx)+tx]);
						col=data[(ty*sx)+tx];
						for (tsx=0; tsx<scale; tsx++)
							LCD_Write_DATA(col>>8,col & 0xff);
					}
				}
			}
		}
	}
	clrXY();
}

void drawBitmap2(int x, int y, int sx, int sy, uint16_t* data, int deg, int rox, int roy)
{
	unsigned int col;
	int tx, ty, newx, newy;
	double radian;
	radian=deg*0.0175;  

	if (deg==0)
		drawBitmap1(x, y, sx, sy, data,0);
	else
	{
		for (ty=0; ty<sy; ty++)
		{
#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
      /* Reload IWDG counter */
      IWDG_ReloadCounter();  
#endif			
			for (tx=0; tx<sx; tx++)
			{  						
				//col=pgm_read_word(&data[(ty*sx)+tx]);
				col=data[(ty*sx)+tx];

				newx=x+rox+(((tx-rox)*cos(radian))-((ty-roy)*sin(radian)));
				newy=y+roy+(((ty-roy)*cos(radian))+((tx-rox)*sin(radian)));

				setXY(newx, newy, newx, newy);
				LCD_Write_DATA(col>>8,col & 0xff);
			}
		}
	}
	clrXY();
}

void lcdOff(void)
{

}

void lcdOn(void)
{

}

void setContrast(char c)
{

}

int getDisplayXSize(void)
{
		return disp_x_size+1;
}

int getDisplayYSize(void)
{
		return disp_y_size+1;
}

void setBrightness(uint8_t br)
{

}

void setDisplayPage(uint8_t page)
{

}

void setWritePage(uint8_t page)
{

}

#if 0
void LCD_Write_COM(char VL)  
{   
	if (display_transfer_mode!=1)
	{
		cbi(P_RS, B_RS);
		LCD_Writ_Bus(0x00,VL,display_transfer_mode);
	}
	else
		LCD_Writ_Bus(0x00,VL,display_transfer_mode);
}

void UTFT::LCD_Write_DATA(char VH,char VL)
{
	if (display_transfer_mode!=1)
	{
		sbi(P_RS, B_RS);
		LCD_Writ_Bus(VH,VL,display_transfer_mode);
	}
	else
	{
		LCD_Writ_Bus(0x01,VH,display_transfer_mode);
		LCD_Writ_Bus(0x01,VL,display_transfer_mode);
	}
}

void UTFT::LCD_Write_DATA(char VL)
{
	if (display_transfer_mode!=1)
	{
		sbi(P_RS, B_RS);
		LCD_Writ_Bus(0x00,VL,display_transfer_mode);
	}
	else
		LCD_Writ_Bus(0x01,VL,display_transfer_mode);
}

void UTFT::LCD_Write_COM_DATA(char com1,int dat1)
{
     LCD_Write_COM(com1);
     LCD_Write_DATA(dat1>>8,dat1);
}
#endif
