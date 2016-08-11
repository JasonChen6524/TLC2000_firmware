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


#ifndef UTFT_DLB_H
#define UTFT_DLB_H

/*#include "WProgram.h"*/

#include "UTFT.h"

typedef struct _propFont
{
  uint8_t charCode;
	
  //uint8_t adjYOffset;
  //uint8_t width;
  //uint8_t height;
  //uint8_t xOffset;
  //uint8_t xDelta;	
  int adjYOffset;
  int width;
  int height;
  int xOffset;
  int xDelta;	
  uint8_t* dataPtr;
}propFont;
    

void print_ttf(char *st, int x, int y, int deg);   
void print_ttf_1(char *st, int x, int y, int deg);
int getStringWidth(char* str);
int getFontHeight(void);
uint8_t getOrientation(void);

int printProportionalChar(uint8_t c, int x, int y);
int rotatePropChar(uint8_t c, int x, int y, int offset, int deg);
uint8_t getCharPtr(uint8_t c);//, propFont* font);

#endif

