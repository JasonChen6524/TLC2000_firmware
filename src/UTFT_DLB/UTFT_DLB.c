/*
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <math.h>
#include <string.h>
#include "UTFT_DLB.h"


//*****************************************************************************************************************
// DLB
//*****************************************************************************************************************

propFont fontChar;    

// draw a proportional (or other ttf converted) font character on an angle
int rotatePropChar(uint8_t c, int x, int y, int offset, int deg)
{
   //propFont* fontChar;
	 float cos_radian,sin_radian;
   uint8_t i,j,ch,mask;
   //uint16_t temp; 
	 int newX,newY;
   uint8_t *tempPtr = fontChar.dataPtr;
	 double radian = deg * 0.0175;  
   uint16_t fcolor = getColor();
	
   if (!getCharPtr(c))//, fontChar))
   {
       return 0;
   }
      
   if (fontChar.width != 0)
   {
      mask = 0x80;
      cos_radian = cos(radian);
      sin_radian = sin(radian);
      for (j=0; j < fontChar.height; j++)
      {
         //ch=pgm_read_byte(tempPtr++);
         for (i=0; i < fontChar.width; i++)
         {
            if (((i + (j*fontChar.width)) % 8) == 0)
            {
                mask = 0x80;
              //ch = pgm_read_byte(tempPtr++);
							  ch=*tempPtr++;
            }
            
            newX = x + ((offset + i) * cos_radian - (j+fontChar.adjYOffset)*sin_radian);
            newY = y + ((j+fontChar.adjYOffset) * cos_radian + (offset + i) * sin_radian);
            if ((ch & mask) !=0)
            {
                setXY(newX,newY,newX,newY);
                setPixel(fcolor);
            } 
            else
            {
                if (!_transparent)
                {
                    setXY(newX,newY,newX,newY);
                    setPixel(getBackColor());
                }                
            }
            mask >>= 1;
         }
      }
   }
	 clrXY();
      
   return fontChar.xDelta;
}

// override UTFT::print to handle proportional and fixed-width fonts
void print_ttf(char *st, int x, int y, int deg)
{
	int i;
	int offset = 0;
	int stl;
	//char *stringPtr = st;

	stl = strlen(st);
#if 0
	if (orient==PORTRAIT)
	{
       if (x==RIGHT)
          x=240-(stl*cfont.x_size);
       if (x==CENTER)
          x=(240-(stl*cfont.x_size))/2;
	}
	else
	{
       if (x==RIGHT)
          x=(disp_y_size+1)-(stl*cfont.x_size);
       if (x==CENTER)
          x=((disp_y_size+1)-(stl*cfont.x_size))/2;
	}
#endif
	
  offset = 0;
	for (i=0; i < stl; i++)
	//while(*stringPtr++)
   {
#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
      /* Reload IWDG counter */
      IWDG_ReloadCounter();  
#endif  				 
		if (deg==0)
    {
        // DLB Added this stuff...
        if (cfont.x_size == 0)
        {
            x += printProportionalChar(*st++, x, y)+1;
        }
        else
        {          
            printChar(*st++, x, y);
            x += cfont.x_size;
        }
    }
		else
      {
        // DLB Added this stuff...
        if (cfont.x_size == 0)
        {
            offset += rotatePropChar(*st++, x, y, offset, deg);
        }
        else
        {
            rotateChar(*st++, x, y, i, deg);
        }
      }
  }
}

// private method to return the Glyph data for an individual character in the ttf font
uint8_t getCharPtr(uint8_t c)//, propFont* fontChar)
{
    uint8_t* tempPtr = (uint8_t*)(cfont.font + 4); // point at data
    
    do
    {
#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
      /* Reload IWDG counter */
      IWDG_ReloadCounter();  
#endif  					
        //fontChar.charCode = pgm_read_byte(tempPtr++);
			  fontChar.charCode   = *tempPtr++;
        fontChar.adjYOffset = *tempPtr++;
        fontChar.width      = *tempPtr++;
        fontChar.height     = *tempPtr++;
        fontChar.xOffset    = *tempPtr++;
        fontChar.xOffset    = fontChar.xOffset < 0x80 ? fontChar.xOffset : (0x100 - fontChar.xOffset);
        fontChar.xDelta     = *tempPtr++;
        if (c != fontChar.charCode && fontChar.charCode != 0xFF)
        {
            if (fontChar.width != 0)
            {
                // packed bits
                tempPtr += (((fontChar.width * fontChar.height)-1) / 8) + 1;
            }
        }
    } while (c != fontChar.charCode && fontChar.charCode != 0xFF);
    
    fontChar.dataPtr = tempPtr;

    return (fontChar.charCode != 0xFF);
}


// print a ttf based character
//uint8_t *tempPtr;
int printProportionalChar(uint8_t c, int x, int y)
{
	  uint8_t i,ch,mask;
	  int j,h;
	  uint16_t fcolor,bcolor;//temp,; 
    uint8_t *tempPtr;
	  int fontHeight;

    //propFont* fontChar;    
    if (!getCharPtr(c))//, fontChar))
    {
        return 0;
    }
        
    // fill background
    // VGA_TRANSPARENT?
    fcolor = getColor();
		bcolor = getBackColor();
		//_transparent = 1;
		fontHeight = getFontHeight();
    if (!_transparent)
    {
        //fontHeight = getFontHeight();
        setColor(bcolor);//getBackColor());
        fillRect(x, y, x + fontChar.xDelta+1, y + fontHeight);
        setColor(fcolor);
    }
    tempPtr = fontChar.dataPtr;
    
    // draw Glyph    
    if (fontChar.width != 0)
    {
        mask = 0x80;
        for (j=0; j < fontChar.height; j++)
        {
#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
      /* Reload IWDG counter */
      IWDG_ReloadCounter();  
#endif  												
           for (i=0; i < fontChar.width; i++)
           {	 
              if (((i + (j*fontChar.width)) % 8) == 0)
              {
                 mask = 0x80;
               //ch = pgm_read_byte(tempPtr++);
								 ch = *tempPtr++;								 
              }
                
              if ((ch & mask) !=0)
              {
							 //h = y+fontChar.adjYOffset+fontHeight-j;
							 //h = y + (24-fontChar.adjYOffset+j); 		
						     h = y + fontHeight - (fontChar.adjYOffset+j);								
                 setXY(x+fontChar.xOffset+i, h, x+fontChar.xOffset+i, h);																
                 setPixel(fcolor);
              } 
              else
              {
                  //setPixel(bcolorr, bcolorg, bcolorb);
								 //setPixel(bcolor);
								 //h = y+fontChar.adjYOffset+j;
              }
              mask >>= 1;
           }
        }
    }           
    return fontChar.xDelta;
}

int printProportionalChar_1(uint8_t c, int x, int y)
{
	  uint8_t i,ch,mask;
	  int j,h;
	  uint16_t fcolor;//temp,; 
    uint8_t *tempPtr;
	  int fontHeight;

    //propFont* fontChar;    
    if (!getCharPtr(c))//, fontChar))
    {
        return 0;
    }
        
    // fill background
    // VGA_TRANSPARENT?
    fcolor = getColor();
    if (!_transparent)
    {
        fontHeight = getFontHeight();
        setColor(getBackColor());
        fillRect(x, y, x + fontChar.xDelta+1, y + fontHeight);
        setColor(fcolor);
    }
    
    tempPtr = fontChar.dataPtr;
    
    // draw Glyph    
    if (fontChar.width != 0)
    {
        mask = 0x80;
        for (j=0; j < fontChar.height; j++)
        {
#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
      /* Reload IWDG counter */
      IWDG_ReloadCounter();  
#endif  											
           //ch=pgm_read_byte(tempPtr++);					
           for (i=0; i < fontChar.width; i++)
           {		 
              if (((i + (j*fontChar.width)) % 8) == 0)
              {
                 mask = 0x80;
                 //ch = pgm_read_byte(tempPtr++);
								 ch = *tempPtr++;								 
              }
                
              if ((ch & mask) !=0)
              {
								 //h = y+fontChar.adjYOffset+fontChar.height-j;
								 h = y+fontChar.adjYOffset+j;
                 setXY(x+fontChar.xOffset+i, h, x+fontChar.xOffset+i, h);																
                 setPixel(fcolor);
              } 
              else
              {
                  //setPixel(bcolorr, bcolorg, bcolorb);
								 //h = y+fontChar.adjYOffset+j;
                 //setXY(x+fontChar.xOffset+i, h, x+fontChar.xOffset+i, h);			
								 //setPixel(getBackColor());
              }
              mask >>= 1;
           }
        }
    }           
    return fontChar.xDelta;
}

void print_ttf_1(char *st, int x, int y, int deg)
{
	int i;
	int offset = 0;
	int stl;
	//char *stringPtr = st;

	stl = strlen(st);
#if 1
	if (orient==PORTRAIT)
	{
       if (x==RIGHT)
          x=240-(stl*cfont.x_size);
       if (x==CENTER)
          x=(240-(stl*cfont.x_size))/2;
	}
	else
	{
       if (x==RIGHT)
          x=(disp_y_size+1)-(stl*cfont.x_size);
       if (x==CENTER)
          x=((disp_y_size+1)-(stl*cfont.x_size))/2;
	}
#endif
	
  offset = 0;
	for (i=0; i < stl; i++)
	//while(*stringPtr++)
   {
#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
      /* Reload IWDG counter */
      IWDG_ReloadCounter();  
#endif  				 
		if (deg==0)
    {
        // DLB Added this stuff...
        if (cfont.x_size == 0)
        {
            x += printProportionalChar_1(*st++, x, y)+1;
        }
        else
        {          
            printChar(*st++, x, y);
            x += cfont.x_size;
        }
    }
		else
      {
        // DLB Added this stuff...
        if (cfont.x_size == 0)
        {
            offset += rotatePropChar(*st++, x, y, offset, deg);
        }
        else
        {
            rotateChar(*st++, x, y, i, deg);
        }
      }
  }
}


// returns the string width in pixels. Useful for positions strings on the screen.
int getStringWidth(char* str)
{
	  uint8_t found;
    //char* tempStrptr = str;
    
    // is it a fixed width font?
    if (cfont.x_size != 0)
    {
        return (strlen(str) * cfont.x_size);
    }
    else
    {
        // calculate the string width
        int strWidth = 0;
        while (*str != 0)
        {
#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
      /* Reload IWDG counter */
      IWDG_ReloadCounter();  
#endif  							
            //propFont* fontChar;    
            found = getCharPtr(*str);//, fontChar);
            
            if (found && *str == fontChar.charCode)
            {
                strWidth += fontChar.xDelta + 1;
            }
            
            str++;            
        }

        return strWidth;
    }
}

int getFontHeight()
{
    return (cfont.y_size);
}

//uint8_t getOrientation()
//{
//    return orient;
//}
