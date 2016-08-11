// UTFT_Demo_320x240 (C)2014 Henning Karlsen
// web: http://www.henningkarlsen.com/electronics
//
// This program is a demo of how to use most of the functions
// of the library with a supported display modules.
//
// This demo was made for modules with a screen resolution 
// of 320x240 pixels.
//
// This program requires the UTFT library.
//

#include <math.h>
#include <time.h>
#include <stdlib.h>

#include "UTFT.h"

// Declare which fonts we will be using
//extern void Delay(__IO uint32_t nCount);

// Set the pins to the correct ones for your development shield
// ------------------------------------------------------------
// Standard Arduino Mega/Due shield            : <display model>,38,39,40,41
// CTE TFT LCD/SD Shield for Arduino Due       : <display model>,25,26,27,28
// Teensy 3.x TFT Test Board                   : <display model>,23,22, 3, 4
// ElecHouse TFT LCD/SD Shield for Arduino Due : <display model>,22,23,31,33
//
// Remember to change the model parameter to suit your display module!


void loop(void)
{
  int buf[318];
  int x;// x2;
  int y;// y2;
  int r,i;

	setFont(SmallFont);
// Clear the screen and draw the frame
  clrScr();
	
	setColorRGB(0,0,0);
  fillRect(1,15,238,305);

	setColorRGB(64, 64, 64);
  //fillRect(0, 0, 319, 13);
	fillRect(0, 0, 239, 13);
  setColorRGB(255, 0, 0);  
  //fillRect(0, 226, 319, 239);
	fillRect(0, 306, 239, 319);
	

  setColorRGB(255, 255, 255);
  setBackColorRGB(255, 0, 0);
	//setBackColor(VGA_RED);
  print((uint8_t*)"---TFT Lib---*", 0,306, 0);
  setBackColorRGB(64, 64, 64);
  setColorRGB(255,255,0);
  print((uint8_t*)"DVP by Jason", 0,0, 0);

  setColorRGB(0, 0, 255);
  drawRect(0, 14, 239, 305);

// Draw crosshairs
  setColorRGB(0, 0, 255);
  setBackColorRGB(0, 0, 0);
  drawLine(119, 14, 119, 305);
  drawLine(1, 159, 238, 159);

  for (i=9; i<310; i+=10)
    drawLine(i, 157, i, 161);
  for (i=19; i<300; i+=10)
    drawLine(117, i, 121, i);
		
// Draw sin-, cos- and tan-lines  
  setColorRGB(0,255,255);
  print((uint8_t*)"Sin", 5, 15,0);
  for (i=1; i<238; i++)
  {
    drawPixel(i,159+(sin(((i*1.13)*3.14)/180)*95));
  }
  
  setColorRGB(255,0,0);
  print((uint8_t*)"Cos", 5, 27,0);
  for (i=1; i<238; i++)
  {
    drawPixel(i,159+(cos(((i*1.13)*3.14)/180)*95));
  }

  setColorRGB(255,255,0);
  print((uint8_t*)"Tan", 5, 39,0);
  for (i=1; i<238; i++)
  {
    drawPixel(i,159+(tan(((i*1.13)*3.14)/180)));
  }

  Delay(2000);
	
  setColorRGB(0,0,0);
  fillRect(1,15,238,305);
  setColorRGB(0, 0, 255);
  setBackColorRGB(0, 0, 0);
  //drawLine(159, 15, 159, 224);
  //drawLine(1, 119, 318, 119);	
  drawLine(119, 14, 119, 305);
  drawLine(1, 159, 238, 159);

// Draw a moving sinewave
  x=1;
  for (i=1; i<(238*20); i++) 
  {
    x++;
    if (x==239)
      x=1;
    if (i>239)
    {
      if ((x==119)||(buf[x-1]==159))
        setColorRGB(0,0,255);
      else
        setColorRGB(0,0,0);
      drawPixel(x,buf[x-1]);
    }
    setColorRGB(0,255,255);
    y=159+(sin(((i*1.1)*3.14)/180)*(90-(i / 100)));
    drawPixel(x,y);
    buf[x-1]=y;
  }

  Delay(2000);
  
  setColorRGB(0,0,0);
  fillRect(1,15,238,305);

// Draw some filled rectangles
  for (i=1; i<6; i++)
  {
    switch (i)
    {
      case 1:
        setColorRGB(255,0,255);
        break;
      case 2:
        setColorRGB(255,0,0);
        break;
      case 3:
        setColorRGB(0,255,0);
        break;
      case 4:
        setColorRGB(0,0,255);
        break;
      case 5:
        setColorRGB(255,255,0);
        break;
    }
    fillRect(15+(i*20), 50+(i*20), 115+(i*20), 150+(i*20));
		Delay(500);
  }

  Delay(2000);
  
  setColorRGB(0,0,0);
  fillRect(1,15,238,305);

// Draw some filled, rounded rectangles
  for (i=1; i<6; i++)
  {
    switch (i)
    {
      case 1:
        setColorRGB(255,0,255);
        break;
      case 2:
        setColorRGB(255,0,0);
        break;
      case 3:
        setColorRGB(0,255,0);
        break;
      case 4:
        setColorRGB(0,0,255);
        break;
      case 5:
        setColorRGB(255,255,0);
        break;
    }
    fillRoundRect(150-(i*20), 50+(i*20), 250-(i*20), 150+(i*20));
		Delay(500);
  }
 
  Delay(2000);
  
  setColorRGB(0,0,0);
  fillRect(1,15,238,305);

// Draw some filled circles
  for (i=1; i<6; i++)
  {
    switch (i)
    {
      case 1:
        setColorRGB(255,0,255);
        break;
      case 2:
        setColorRGB(255,0,0);
        break;
      case 3:
        setColorRGB(0,255,0);
        break;
      case 4:
        setColorRGB(0,0,255);
        break;
      case 5:
        setColorRGB(255,255,0);
        break;
    }
    fillCircle(60+(i*20),100+(i*20), 50);
  }
	  
  Delay(2000);
  
  setColorRGB(0,0,0);
  fillRect(1,15,238,305);

// Draw some lines in a pattern
  setColorRGB(255,0,0);
  for (i=15; i<224; i+=5)
  {
    drawLine(1, i, (i*1.44)-10, 305);
  }
  setColorRGB (255,0,0);
  for (i=305; i>15; i-=5)
  {
    drawLine(228, i, (i*1.44)-11, 15);
  }

  setColorRGB (0,255,255);
  for (i=305; i>15; i-=5)
  {
    drawLine(1, i, 240-19-(i*1.44), 15);
  }
  setColorRGB (0,255,255);
  for (i=15; i<240-16; i+=5)
  {
    drawLine(318, i, 330-(i*1.44), 320-16);
  }

		  
  Delay(2000);
 
  setColorRGB(0,0,0);
  fillRect(1,15,238,305);

// Draw some random circles
  for (i=0; i<100; i++)
  {
    //setColorRGB(random(255), random(255), random(255));
		setColorRGB(rand(), rand(), rand());
    //x=32+random(256);
		x=32+rand();
    //y=45+random(146);
		y=45+rand();
    //r=random(30);
		r = 50;//rand();
    drawCircle(x, y, r);
  }

  Delay(2000);
  
  setColorRGB(0,0,0);
  fillRect(1,15,238,305);

// Draw some random rectangles
  for (i=0; i<100; i++)
  {
    //setColorRGB(random(255), random(255), random(255));
		setColorRGB(rand(), rand(), rand());
    //x=2+random(316);
		x=2+rand();
    //y=16+random(207);
		y=16+rand();
    //x2=2+random(316);
    //x2=2+rand();		
    //y2=16+random(207);
		//y2=16+rand();
    drawRect(x, y, x + 50, y + 100);
  }

  Delay(2000);
  
  setColorRGB(0,0,0);
  fillRect(1,15,238,305);

// Draw some random rounded rectangles
  for (i=0; i<100; i++)
  {
    //setColorRGB(random(255), random(255), random(255));
		setColorRGB(rand(), rand(), rand());
    //x=2+random(316);
		x=2+rand();
    //y=16+random(207);
		y=16+rand();
    //x2=2+random(316);
		//x2=2+rand();
    //y2=16+random(207);
		//y2=16+rand();
    drawRoundRect(x, y, x + 50, y + 50);
  }

  Delay(2000);
  
  setColorRGB(0,0,0);
  fillRect(1,15,238,305);

  for (i=0; i<100; i++)
  {
    //setColorRGB(random(255), random(255), random(255));
		setColorRGB(rand(), rand(), rand());
    //x=2+random(316);
    //y=16+random(209);
    //x2=2+random(316);
    //y2=16+random(209);
    x=2+rand();
    y=16+rand();
    //x2=2+rand();
    //y2=16+rand();		
    drawLine(x, y, x + 50, y + 50);
  }

  Delay(2000);
 
  setColorRGB(0,0,0);
  fillRect(1,15,238,305);

  for (i=0; i<10000; i++)
  {
    //setColorRGB(random(255), random(255), random(255));
    //drawPixel(2+random(316), 16+random(209));
    setColorRGB(rand(), rand(), rand());
    drawPixel(2+rand(), 16+rand());		
  }

  Delay(2000);

  fillScrRGB(0, 0, 255);
  setColorRGB(255, 0, 0);
  fillRoundRect(10, 100, 229, 199);
 

  setColorRGB(255, 255, 255);
  setBackColorRGB(255, 0, 0);
  print((uint8_t*)"That's it!",      11,190, 0);
  print((uint8_t*)"Restarting in a", 11,170, 0);
  print((uint8_t*)"few seconds...",  11,150, 0);
#if 1  
  setColorRGB(0, 255, 0);
  setBackColorRGB(0, 0, 255);
  print((uint8_t*)"Runtime: (msecs)", 11,120, 0);
  //printNumI(millis(), CENTER, 225);
#endif
  Delay (2000);
	setColorRGB(0,0,0);
  fillRect(1,15,238,305);
	Delay (2000);
}

