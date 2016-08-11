/**
  ******************************************************************************
  * @file    TLC2000_spi_LCD_TFT.c
  * @author  Jason Chen
  * @version V1.0
  * @date    18-Aug-2014
  * @brief   
  *            
  *          +-----------------------------------------------------------+
  *          |                     Pin assignment                        |
  *          +-----------------------------+---------------+-------------+
  *          |  STM32 SPI Pins             |     sLCD    |    Pin      |
  *          +-----------------------------+---------------+-------------+
  *          | sFLASH_CS_PIN               | ChipSelect(/S)|    1        |
  *          | sFLASH_SPI_MISO_PIN / MISO  |   DataOut(Q)  |    2        |
  *          |                             |   VCC         |    3 (3.3 V)|
  *          |                             |   GND         |    4 (0 V)  |
  *          | sFLASH_SPI_MOSI_PIN / MOSI  |   DataIn(D)   |    5        |
  *          | sFLASH_SPI_SCK_PIN / SCLK   |   Clock(C)    |    6        |
  *          |                             |    VCC        |    7 (3.3 V)|
  *          |                             |    VCC        |    8 (3.3 V)|  
  *          +-----------------------------+---------------+-------------+  
  */ 

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "TLC2000_SPI_LCD_TFT.h"
#include "stm3210e_TLC2000.h"
//#include "TLC2000_SPI_LCD.h"
#include "fnt08x08.h"
#include "fonts.h"
#include "UTFT.h"
#include "UTFT_DLB.h"
#include "calibration.h"

extern uint8_t displayBuff[];

uint16_t ColorArray[23] = {COLOR_BLACK1,  COLOR_WHITE1, COLOR_BLUE1,     COLOR_LIME1,   COLOR_RED1,
                           COLOR_DEEPRED1,COLOR_YELLOW1,COLOR_LIGHTBLUE1,COLOR_1PURPLE1,COLOR_DARKGREEN1, 
                           COLOR_MINE1,	  COLOR_GREEN1, COLOR_SILVER1,   COLOR_GRAY1,   COLOR_MAROON1,
                           COLOR_OLIVE1,  COLOR_AQUA1,  COLOR_TEAL1,     COLOR_NAVY1,   COLOR_FUCHSIA1,
                           COLOR_2PURPLE1,COLOR_THERALASE1,COLOR_ORANGE1};

uint16_t background_color = COLOR_BLACK1;
uint16_t foreground_color = COLOR_DARKGREEN1;
													 
uint16_t probe_color = COLOR_SILVER1;                           // Added by Jason Chen for probe, 2016_0627
													 
uint8_t armed_flag = 0;
int16_t RSSI = 0;
/**
  * @brief  DeInitializes the peripherals used by the SPI FLASH driver.
  * @param  None
  * @retval None
  */
void sLCD_SPI_DeInit_TFT(void)
{
  sLCD_LowLevel_DeInit();
}

/**
  * @brief  Initializes the peripherals used by the SPI FLASH driver.
  * @param  None
  * @retval None
  */
void sLCD_SPI_Init_TFT(void)
{
  SPI_InitTypeDef  SPI_InitStructure;

  sLCD_LowLevel_Init();
    
  /*!< Deselect the FLASH: Chip Select high */
  sLCD_CS_HIGH_TFT();

  /*!< SPI configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;//SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;

  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;  //--> 72MHz/4 = 18MHz

  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(sLCD_SPI, &SPI_InitStructure);

  /*!< Enable the sFLASH_SPI  */
  SPI_Cmd(sLCD_SPI, ENABLE);
	
//sLCD_EN_VP_HIGH_TFT();
	sLCD_EN_VP_LOW_TFT();
}

/**
  * @brief  Reads a byte from the SPI Flash.
  * @note   This function must be used only if the Start_Read_Sequence function
  *         has been previously called.
  * @param  None
  * @retval Byte Read from the SPI Flash.
  */
//uint8_t sLCD_ReadByte(void)
//{
//  return (sLCD_SendByte(sLCD_DUMMY_BYTE));
//}

/**
  * @brief  Sends a byte through the SPI interface and return the byte received
  *         from the SPI bus.
  * @param  byte: byte to send.
  * @retval The value of the received byte.
  */
/*uint8_t*/void sLCD_SendByte_TFT(uint8_t byte)
{
  /*!< Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(sLCD_SPI, SPI_I2S_FLAG_TXE) == RESET);

  /*!< Send byte through the SPI1 peripheral */
  SPI_I2S_SendData(sLCD_SPI, byte);

	  /*!< Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(sLCD_SPI, SPI_I2S_FLAG_TXE) == RESET);
	
  /*!< Wait to receive a byte */
  //while (SPI_I2S_GetFlagStatus(sLCD_SPI, SPI_I2S_FLAG_RXNE) == RESET);

  /*!< Return the byte read from the SPI bus */
  //return SPI_I2S_ReceiveData(sLCD_SPI);
}

/**
  * @brief  Sends a Half Word through the SPI interface and return the Half Word
  *         received from the SPI bus.
  * @param  HalfWord: Half Word to send.
  * @retval The value of the received Half Word.
  */
uint16_t sLCD_SendHalfWord_TFT(uint16_t HalfWord)
{
  /*!< Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(sLCD_SPI, SPI_I2S_FLAG_TXE) == RESET);

  /*!< Send Half Word through the sFLASH peripheral */
  SPI_I2S_SendData(sLCD_SPI, HalfWord);

  /*!< Wait to receive a Half Word */
  while (SPI_I2S_GetFlagStatus(sLCD_SPI, SPI_I2S_FLAG_RXNE) == RESET);

  /*!< Return the Half Word read from the SPI bus */
  return SPI_I2S_ReceiveData(sLCD_SPI);
}

/*************************************************/
void sLCD_Write_Command_TFT(uint8_t command)
{
	  sLCD_DC_LOW_TFT();          // command mode
	
	  //sLCD_SendByte_TFT((uint8_t)(command>>8)); // 
	  //sLCD_EN_VP_HIGH_TFT();                    // latch data on bus
	  //sLCD_CS_LOW_TFT();          // chip selected	  
	  //sLCD_CS_HIGH_TFT();                       // WR signal

   	sLCD_EN_VP_LOW_TFT();                     //	  
	  sLCD_SendByte_TFT(command);      // 
	  sLCD_EN_VP_HIGH_TFT();                    // latch data on bus	  
	  sLCD_CS_LOW_TFT();                        // chip selected
	  sLCD_CS_HIGH_TFT();                       // WR signal == deselect chip
	
	  //Delay(1);
	  __NOP();	__NOP();//	__NOP();	__NOP();	__NOP();
	  //sLCD_EN_VP_LOW_TFT();
}

/*************************************************/
void sLCD_Write_Data_TFT(uint8_t data)
{
    sLCD_DC_HIGH_TFT();          // SET_CD;   // data mode
		  
	  //sLCD_SendByte_TFT((uint8_t)(data>>8));    // 
	  //sLCD_EN_VP_HIGH_TFT();                    // latch data on bus	  
	  //sLCD_CS_LOW_TFT();          // chip selected	  
	  //sLCD_CS_HIGH_TFT();                       // WR signal

   	sLCD_EN_VP_LOW_TFT();                     //	  
	  sLCD_SendByte_TFT(data);         // 
	  sLCD_EN_VP_HIGH_TFT();                    // latch data on bus	  
	  sLCD_CS_LOW_TFT();                        // chip selected
	  sLCD_CS_HIGH_TFT();                       // WR signal == deselect chip
	
	  //Delay(1);
	__NOP();	__NOP();//	__NOP();	__NOP();	__NOP();
	  //sLCD_EN_VP_LOW_TFT();
}
/*************************************************/

/*************************************************/
void sLCD_init_TFT(void)
{
	 sLCD_RST_LOW_TFT();              // CLR_RESET;
	 Delay(100);
	 sLCD_RST_HIGH_TFT();             // SET_RESET;
	 Delay(100);

   sLCD_Write_Command_TFT(0x28); //display OFF
#if 1
   sLCD_Write_Command_TFT(0x11); //exit SLEEP mode
   sLCD_Write_Data_TFT(0x00);
	
   sLCD_Write_Command_TFT(0xCB); //Power Control A
   sLCD_Write_Data_TFT(0x39); //always 0x39
   sLCD_Write_Data_TFT(0x2C); //always 0x2C
   sLCD_Write_Data_TFT(0x00); //always 0x00
   sLCD_Write_Data_TFT(0x34); //Vcore = 1.6V
   sLCD_Write_Data_TFT(0x02); //DDVDH = 5.6V
	 Delay(5);
   sLCD_Write_Command_TFT(0xCF); //Power Control B
   sLCD_Write_Data_TFT(0x00); //always 0x00
   sLCD_Write_Data_TFT(0x81); //PCEQ off
   sLCD_Write_Data_TFT(0x30); //ESD protection
	 
   sLCD_Write_Command_TFT(0xE8); //Driver timing control A
   sLCD_Write_Data_TFT(0x85); //non-overlap
   sLCD_Write_Data_TFT(0x01); //EQ timing
   sLCD_Write_Data_TFT(0x79); //Pre-charge timing
	 Delay(5);
   sLCD_Write_Command_TFT(0xEA); //Driver timing control B
   sLCD_Write_Data_TFT(0x00); //Gate driver timing
   sLCD_Write_Data_TFT(0x00); //always 0x00[10]
	 
   sLCD_Write_Command_TFT(0xED); //Power-On sequence control
   sLCD_Write_Data_TFT(0x64); //soft start
   sLCD_Write_Data_TFT(0x03); //power on sequence
   sLCD_Write_Data_TFT(0x12); //power on sequence
   sLCD_Write_Data_TFT(0x81); //DDVDH enhance on
	 Delay(5);
   sLCD_Write_Command_TFT(0xF7); //Pump ratio control
   sLCD_Write_Data_TFT(0x20); //DDVDH=2xVCI
	 
   sLCD_Write_Command_TFT(0xC0); //power control 1
   sLCD_Write_Data_TFT(0x26);
   sLCD_Write_Data_TFT(0x04); //second parameter for ILI9340 (ignored by ILI9341)
	 Delay(5);
   sLCD_Write_Command_TFT(0xC1); //power control 2
   sLCD_Write_Data_TFT(0x11);
	 
   sLCD_Write_Command_TFT(0xC5); //VCOM control 1
   sLCD_Write_Data_TFT(0x35);
   sLCD_Write_Data_TFT(0x3E);
	 Delay(5);
   sLCD_Write_Command_TFT(0xC7); //VCOM control 2
   sLCD_Write_Data_TFT(0xBE);
	 
   sLCD_Write_Command_TFT(0x36); //memory access control = BGR
   sLCD_Write_Data_TFT(0x88);
	 Delay(5);
   sLCD_Write_Command_TFT(0xB1); //frame rate control
   sLCD_Write_Data_TFT(0x00);
   sLCD_Write_Data_TFT(0x10);
	 
   sLCD_Write_Command_TFT(0xB6); //display function control
   sLCD_Write_Data_TFT(0x0A);
   sLCD_Write_Data_TFT(0xA2);
	 Delay(5);
   sLCD_Write_Command_TFT(0x3A); //pixel format = 16 bit per pixel
   sLCD_Write_Data_TFT(0x55);
	 
   sLCD_Write_Command_TFT(0xF2); //3G Gamma control
   sLCD_Write_Data_TFT(0x02); //off
	 
   sLCD_Write_Command_TFT(0x26); //Gamma curve 3
   sLCD_Write_Data_TFT(0x01);
	 Delay(5);
   sLCD_Write_Command_TFT(0x2A); //column address set
   sLCD_Write_Data_TFT(0x00);
   sLCD_Write_Data_TFT(0x00); //start 0x0000
   sLCD_Write_Data_TFT(0x00);
   sLCD_Write_Data_TFT(0xEF); //end 0x00EF
	 Delay(5);
   sLCD_Write_Command_TFT(0x2B); //page address set
   sLCD_Write_Data_TFT(0x00);
   sLCD_Write_Data_TFT(0x00); //start 0x0000
   sLCD_Write_Data_TFT(0x01);
   sLCD_Write_Data_TFT(0x3F); //end 0x013F
	 
	 sLCD_Write_Command_TFT(0x29); //Display On
	 
#endif	 
   Delay(100);
}

void display_off(void)
{
	  sLCD_Write_Command_TFT(0x28); //display OFF
}

void display_on(void)
{
	sLCD_Write_Command_TFT(0x29); //Display On
}

void Display_workingMode_old(uint8_t mode)
{
	if(mode == PATIENT_TREATMENT_MODE)
	{
	  //print_ttf("->Treatment",120,3,0);	
		//sLCD_putString_TFT_ext(120,3,(uint8_t*)"->Treat", Arial_Normal16x16);
	//sLCD_putString_TFT(10,25,(uint8_t*)"---Treating---",Font16x24);//displayBuff);	
		sLCD_putString_TFT(10,25,(uint8_t*)"Treatment Mode",Font16x24);//displayBuff);	
	  //working_mode = 0;
	}
	else if(mode == PATIENT_MEASUREMENT_MODE)
	{
		//print_ttf("->Measurement",120,3,0);	
  //sLCD_putString_TFT_ext(120,3,(uint8_t*)"->Measu", Arial_Normal16x16);
  //sLCD_putString_TFT(10,104+24+12,(uint8_t*)"---Measuring---", Font16x24);    // Changed by Jason Chen, 2015.02.25		
		sLCD_putString_TFT(10+16+8,104+24+24,(uint8_t*)"Measurement", Font16x24);    // Changed by Jason Chen, 2015.02.25		
		sLCD_putString_TFT(10+32,104+24,(uint8_t*)"   Mode", Font16x24);    // Changed by Jason Chen, 2015.02.25		
    //print_ttf("-->Measurement", 10,80-24-12,0);	
	  //working_mode = 1;
	}
}

void Display_workingMode(uint8_t mode)      // Created newly by Jason Chen, 2015.03.24
{
		if(mode == PATIENT_TREATMENT_MODE)
		{							
			//Display_TensPower(userProfile.user_profile.pTensDACVoltage  & 0x0FFF);
			if(((userProfile.user_profile.p905DAC0Voltage > 0)||
				  (userProfile.user_profile.p905DAC1Voltage > 0)||
			    (userProfile.user_profile.p905DAC2Voltage > 0)||
			    (userProfile.user_profile.p905DAC3Voltage > 0)||
			    (userProfile.user_profile.p905DAC4Voltage > 0)||
			    (userProfile.user_profile.p905DAC5Voltage > 0)||(userProfile.user_profile.pTensDACVoltage  > 0))&&
			    (userProfile.user_profile.treatment_time > 0))
			{
		  //sLCD_putString_TFT(10,25,(uint8_t*)"Treatment Mode",Font16x24);//displayBuff);	
		  //sLCD_putString_TFT(10,25,(uint8_t*)"  Treatment   ",Font16x24);//displayBuff);	
			}
			else
			{
		  //sLCD_putString_TFT(10,25,(uint8_t*)"  Idle Mode   ",Font16x24);//displayBuff);	
			}
			
			//Display_LaserOff();
			//Display_TENsOff();                   // Added by Jason Chen, 2014.11.11
			
			//print_ttf("-->Treatment",120,3,0);	
			//sLCD_putString_TFT_ext(120,3,(uint8_t*)"->Treatment", Arial_Normal16x16);
		
	  }
  //else if(userProfile.user_profile.mode == PATIENT_MEASUREMENT_MODE)
		else if(mode == PATIENT_MEASUREMENT_MODE)
		{
			if(((userProfile.user_profile.p905DAC0Voltage > 0)||(userProfile.user_profile.p905DAC1Voltage > 0)||
			    (userProfile.user_profile.p905DAC2Voltage > 0)||(userProfile.user_profile.p905DAC3Voltage > 0)||
			    (userProfile.user_profile.p905DAC4Voltage > 0)||(userProfile.user_profile.p905DAC5Voltage > 0))&&
			    (userProfile.user_profile.treatment_time > 0))
			{
    		//sLCD_putString_TFT(10+16+8,104+24+24,(uint8_t*)"Measurement", Font16x24);    // Changed by Jason Chen, 2015.02.25		
		    //sLCD_putString_TFT(10+32,104+24,(uint8_t*)"   Mode", Font16x24);    // Changed by Jason Chen, 2015.02.25						
			}
			else
			{
				//sLCD_putString_TFT(10+16+8,104+24+24,(uint8_t*)" Idle Mode ", Font16x24);    // Changed by Jason Chen, 2015.02.25		
			}
			//Display_LaserOff();		
		}
}

/*************************************************/
void sLCD_Display_Home_TFT(void)
{
   sLCD_Write_Command_TFT(0x2A); //column address set
   sLCD_Write_Data_TFT(0x00);
   sLCD_Write_Data_TFT(0x00); //start 0x0000
   sLCD_Write_Data_TFT(0x00);
   sLCD_Write_Data_TFT(0xEF); //end 0x00EF
	 
   sLCD_Write_Command_TFT(0x2B); //page address set
   sLCD_Write_Data_TFT(0x00);
   sLCD_Write_Data_TFT(0x00); //start 0x0000
   sLCD_Write_Data_TFT(0x01);
   sLCD_Write_Data_TFT(0x3F); //end 0x013F
	 sLCD_Write_Command_TFT(0x2C);              //command to begin writing to frame memory
}

/*************************************************/
void sLCD_Display_RGB_TFT(void)
{
	int16_t i,j;
	
	sLCD_Display_Home_TFT();

// white, red, green, blue screen
	for(i=0;i<64;i++)
	{
		for(j=0;j<96;j++)
		{
			if(j>=0 && j<24)
			{
				sLCD_Write_Data_TFT((uint8_t)(COLOR_WHITE >> 8));
				sLCD_Write_Data_TFT((uint8_t)(COLOR_WHITE >> 0));;		//white display
			}
			if(j>23 && j<48)
			{
				sLCD_Write_Data_TFT((uint8_t)(COLOR_BLUE >> 8));
				sLCD_Write_Data_TFT((uint8_t)(COLOR_BLUE >> 0));		//blue display
			}
			if(j>47 && j<72)
			{
				sLCD_Write_Data_TFT((uint8_t)(COLOR_LIME >> 8));		//green display
				sLCD_Write_Data_TFT((uint8_t)(COLOR_LIME >> 0));
			}
			if(j>71 && j<96)
			{
				sLCD_Write_Data_TFT((uint8_t)(COLOR_RED >> 8));		//red display
				sLCD_Write_Data_TFT((uint8_t)(COLOR_RED >> 0));
			}
		}
	}
}

/*************************************************/
void sLCD_background_TFT(uint16_t back_color,uint16_t fore_color)
{
	uint16_t j,i;
#if 1
   sLCD_Write_Command_TFT(0x2A); //column address set
   sLCD_Write_Data_TFT(0x00);
   sLCD_Write_Data_TFT(0x00); //start 0x0000
   sLCD_Write_Data_TFT(0x00);
   sLCD_Write_Data_TFT(0xEF); //end 0x00EF
	 
   sLCD_Write_Command_TFT(0x2B); //page address set
   sLCD_Write_Data_TFT(0x00);
   sLCD_Write_Data_TFT(0x00); //start 0x0000
   sLCD_Write_Data_TFT(0x01);
   sLCD_Write_Data_TFT(0x3F); //end 0x013F
#endif
	 sLCD_Write_Command_TFT(0x2C);              //command to begin writing to frame memory
	for(i=0;i<720;i++)
	{
#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
      /* Reload IWDG counter */
        IWDG_ReloadCounter();  
#endif 		
		for(j=0;j<240;j++)
		{ 		
				background_color = back_color;
				foreground_color = fore_color;				
				sLCD_Write_Data_TFT((uint8_t)(background_color >> 8));
				sLCD_Write_Data_TFT((uint8_t)(background_color >> 0));;		//white display
				//background_color = back_color;
				//foreground_color = fore_color;
		}       
	}
}

uint8_t *pStr_TFT;// = TestStr;
void Display_square(uint16_t x, uint16_t y, uint16_t itscolor, sFONT font)
{
	uint16_t i,k,yy;//kk;
	int16_t j,h;
	uint16_t tempChar;
	
	if(font.Height> 16)
		h = 16;
	else
		h = font.Height;
	
	sLCD_Write_Command_TFT(0x2B);           //page address set
	yy = y;// + font.Height - i-1;
	sLCD_Write_Data_TFT((uint8_t)(yy >> 8));
	sLCD_Write_Data_TFT((uint8_t)yy);        //start 0x0000
	yy = y + font.Height - 1;
	sLCD_Write_Data_TFT((uint8_t)(yy >> 8));
	sLCD_Write_Data_TFT((uint8_t)yy);        //start 0x0000
	
	sLCD_Write_Command_TFT(0x2A);           //column address set
	yy = x;
	sLCD_Write_Data_TFT((uint8_t)(yy >> 8));
	sLCD_Write_Data_TFT((uint8_t)yy);        //start 0x0000
	yy = x + font.Width - 1;
	sLCD_Write_Data_TFT((uint8_t)(yy >> 8));
	sLCD_Write_Data_TFT((uint8_t)yy);        //start 0x0000
											
	sLCD_Write_Command_TFT(0x2C);              //command to begin writing to frame memory						

	for(i=0;i<h;i++)
	{			
#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
          /* Reload IWDG counter */
    IWDG_ReloadCounter();  
#endif  		
		tempChar = 0x3FFF;//font.table[('E'-0x20) * font.Height + i];
		if((font.Width == 12)||(font.Width == 8))
		{
			if(font.Width == 12)
				k = 0x8000;
			else
				k = 0x80;
			for(j=0;j<font.Width;j++)
			{ 				
				if(tempChar & (k>>j))
				{
					sLCD_Write_Data_TFT((uint8_t)(itscolor >> 8));		//green display
					sLCD_Write_Data_TFT((uint8_t)(itscolor >> 0));					
				}
				else
				{
					sLCD_Write_Data_TFT((uint8_t)(background_color >> 8));		//black display
					sLCD_Write_Data_TFT((uint8_t)(background_color >> 0));					
				}				
			}
		}
		else
		{
			k = 0x01;
			for(j=0;j<font.Width;j++)
			{			
				if(tempChar & (k<<j))
				{
					sLCD_Write_Data_TFT((uint8_t)(itscolor >> 8));		//green display
					sLCD_Write_Data_TFT((uint8_t)(itscolor >> 0));					
				}
				else
				{
					sLCD_Write_Data_TFT((uint8_t)(background_color >> 8));		//black display
					sLCD_Write_Data_TFT((uint8_t)(background_color >> 0));					
				}				
			}
		}
	}	
}

void sLCD_putChar_TFT(uint16_t x, uint16_t y, uint8_t TestChar, sFONT font)
{
	int i;
	uint16_t k,yy;//kk;
	int16_t j;
	uint16_t tempChar;
	
    sLCD_Write_Command_TFT(0x2B); 
  	yy = y;// + font.Height - i-1;
    sLCD_Write_Data_TFT((uint8_t)(yy >> 8));
    sLCD_Write_Data_TFT((uint8_t)yy);
	  yy = y + font.Height-1;
    sLCD_Write_Data_TFT((uint8_t)(yy >> 8));
    sLCD_Write_Data_TFT((uint8_t)yy);
	
	  sLCD_Write_Command_TFT(0x2A);           //column address set
	  yy = x;
    sLCD_Write_Data_TFT((uint8_t)(yy >> 8));
    sLCD_Write_Data_TFT((uint8_t)yy);
    yy = x + font.Width - 1;
    sLCD_Write_Data_TFT((uint8_t)(yy >> 8));
    sLCD_Write_Data_TFT((uint8_t)yy);
													
		sLCD_Write_Command_TFT(0x2C);              //command to begin writing to frame memory														
		
  //for(i=0;i<font.Height;i++)
		for(i=font.Height-1 ;i>=0;i--)
		{	

#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
          /* Reload IWDG counter */
    IWDG_ReloadCounter();  
#endif  
			
		  tempChar = font.table[(TestChar-0x20) * font.Height + i];
			//if(font.Width == 12)
			//	tempChar = tempChar >> 4;
			
			//k = (1 << (font.Width-1));
			if((font.Width == 12)||(font.Width == 8))
			{
				if(font.Width == 12)
					k = 0x8000;
				else
					k = 0x80;
				for(j=0;j<font.Width;j++)
				{
					if(tempChar & (k>>j))
					{
						sLCD_Write_Data_TFT((uint8_t)(foreground_color >> 8));		//green display
						sLCD_Write_Data_TFT((uint8_t)(foreground_color >> 0));					
					}
					else
					{
						sLCD_Write_Data_TFT((uint8_t)(background_color >> 8));		//black display
						sLCD_Write_Data_TFT((uint8_t)(background_color >> 0));					
					}				
				}
			}
			else
			{
				k = 0x01;
				for(j=0;j<font.Width;j++)
				{						
					if(tempChar & (k<<j))
					{
						sLCD_Write_Data_TFT((uint8_t)(foreground_color >> 8));		//green display
						sLCD_Write_Data_TFT((uint8_t)(foreground_color >> 0));					
					}
					else
					{
						sLCD_Write_Data_TFT((uint8_t)(background_color >> 8));		//black display
						sLCD_Write_Data_TFT((uint8_t)(background_color >> 0));					
					}				
				}
		  }
		}	
}

void sLCD_putChar_TFT_ext(uint16_t x, uint16_t y, uint8_t TestChar, sFONT_ext font)
{
	int i;
	uint16_t yy;
	int16_t j,jj;
	uint8_t tempChar,k,byte_count;

	byte_count = font.Width/8;
	if(font.Width > byte_count*8)
		byte_count +=1;
	
	sLCD_Write_Command_TFT(0x2B);           //page address set
	yy = y;
	sLCD_Write_Data_TFT((uint8_t)(yy >> 8));
	sLCD_Write_Data_TFT((uint8_t)yy);        
	yy = y + font.Height - 1;
	sLCD_Write_Data_TFT((uint8_t)(yy >> 8));
	sLCD_Write_Data_TFT((uint8_t)yy); 
	
	sLCD_Write_Command_TFT(0x2A);           //column address set
	yy = x;
	sLCD_Write_Data_TFT((uint8_t)(yy >> 8));
	sLCD_Write_Data_TFT((uint8_t)yy); 
	yy = x + font.Width - 1;
	sLCD_Write_Data_TFT((uint8_t)(yy >> 8));
	sLCD_Write_Data_TFT((uint8_t)yy); 
					
	sLCD_Write_Command_TFT(0x2C);           //command to begin writing to frame memory
	
	//for(i=0;i<font.Height;i++)
	for(i=font.Height - 1;i>=0;i--)
	{	
#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
	  IWDG_ReloadCounter();                            /* Reload IWDG counter */  
    #endif 	

		for(jj = 0; jj<byte_count;jj++)
		{
				tempChar = font.table[(TestChar-font.start) * font.Height * byte_count + i*byte_count + jj];
				//tempChar1 = font.table[(TestChar-font.start) * font.Height + jj*8];
				k = 0x80;
				for(j=0;j<8/*font.Width*/;j++)
				{									
					if(tempChar & (k>>j))
					{
						sLCD_Write_Data_TFT((uint8_t)(foreground_color >> 8));		//green display
						sLCD_Write_Data_TFT((uint8_t)(foreground_color >> 0));					
					}
					else
					{
						sLCD_Write_Data_TFT((uint8_t)(background_color >> 8));		//black display
						sLCD_Write_Data_TFT((uint8_t)(background_color >> 0));					
					}			
				}
		}
	}	
}

void sLCD_putString_TFT(uint16_t x, uint16_t y, uint8_t * Str, sFONT font)
{
//	uint8_t i,j;//,k;
	//uint8_t tempChar;//,count=0;
	pStr_TFT = Str;

	while(*pStr_TFT)
	{
		sLCD_putChar_TFT(x, y, *pStr_TFT, font);
    //count +=8;
		x +=font.Width;
		pStr_TFT++;		
	}
}

void sLCD_putString_TFT_D(uint16_t x, uint16_t y, uint8_t * Str, sFONT font, uint32_t delay)
{
//	uint8_t i,j;//,k;
	//uint8_t tempChar;//,count=0;
	pStr_TFT = Str;

	while(*pStr_TFT)
	{
		sLCD_putChar_TFT(x, y, *pStr_TFT, font);
    //count +=8;
		x +=font.Width;
		pStr_TFT++;		
		Delay(delay);
	}
}

void sLCD_putString_TFT_ext(uint16_t x, uint16_t y, uint8_t * Str, sFONT_ext font)
{
//	uint8_t i,j;//,k;
	//uint8_t tempChar;//,count=0;
	pStr_TFT = Str;

	while(*pStr_TFT)
	{
		sLCD_putChar_TFT_ext(x, y, *pStr_TFT, font);
    //count +=8;
		x +=font.Width;
		pStr_TFT++;		
	}
}

#define SHIFT_DOWN    12
void Display_Armed(void)
{
//sLCD_putString_TFT(5,104,(uint8_t*)"PROBE: Armed  ",Font16x24);
	sLCD_putString_TFT(5,104-SHIFT_DOWN,(uint8_t*)"    Armed     ",Font16x24);
	armed_flag = 1;
	
	Display_workingMode(userProfile.user_profile.mode);//Added by Jason Chen, 2015.03.24
	if(userProfile.user_profile.mode == PATIENT_TREATMENT_MODE)
		probeMode =BLE_RPT_MODE_TREATMENT_ARMED;
	else
		probeMode =BLE_RPT_MODE_MEASUREMENT_ARMED;
}

void Display_Idle(void)
{
	//sLCD_putString_TFT(5,104,(uint8_t*)"PROBE: Idle   ",Font16x24);
    sLCD_putString_TFT(5,104-SHIFT_DOWN,(uint8_t*)"     IDLE     ",Font16x24);	
	armed_flag = 0;	
	Display_workingMode(userProfile.user_profile.mode);//Added by Jason Chen, 2015.03.24
	 
}

void Display_SlefTestMode(void)                       // Added by Jason Chen, 2015.10.07
{
	sLCD_putString_TFT(5,104-SHIFT_DOWN,(uint8_t*)"Self-Test Mode",Font16x24);	
	armed_flag = 0;	
}


void Display_SlefCaliMode(void)                       // Added by Jason Chen, 2016.08.05
{
	sLCD_putString_TFT(5,104-SHIFT_DOWN,(uint8_t*)"Self-Cali.....",Font16x24);	
	armed_flag = 0;	
}

void Display_Treat(void)
{
	//sLCD_putString_TFT(5,104,(uint8_t*)"PROBE:TREATING",Font16x24);
	sLCD_putString_TFT(5,104-SHIFT_DOWN,(uint8_t*)"   TREATING   ",Font16x24);
	armed_flag = 1;
	
	Display_workingMode(userProfile.user_profile.mode);//Added by Jason Chen, 2015.03.24
}

void Display_Pause(void)                  //Added by Jason Chen, 2015.04.09
{
	sLCD_putString_TFT(5,104-SHIFT_DOWN,(uint8_t*)"   PAUSED     ",Font16x24);
	armed_flag = 1;	
	Display_workingMode(userProfile.user_profile.mode);
}

void Display_Measure(void)
{
	//sLCD_putString_TFT(5,104,(uint8_t*)"PROBE:MEASURE.",Font16x24);	
	sLCD_putString_TFT(5,104-SHIFT_DOWN,(uint8_t*)"  MEASURING   ",Font16x24);	
	armed_flag = 1;
	
	Display_workingMode(userProfile.user_profile.mode);//Added by Jason Chen, 2015.03.24
}

void Display_Active(void)
{
	//sLCD_putString_TFT(5,104,(uint8_t*)"PROBE: ACTIVE ",Font16x24);
	sLCD_putString_TFT(5,104-SHIFT_DOWN,(uint8_t*)"    ACTIVE    ",Font16x24);
	armed_flag = 1;
	
	Display_workingMode(userProfile.user_profile.mode);//Added by Jason Chen, 2015.03.24
}

void Display_Halted(void)
{
  //sLCD_putString_TFT(5,104,(uint8_t*)"PROBE: HALTED ",Font16x24);
	sLCD_putString_TFT(5,104-SHIFT_DOWN,(uint8_t*)"    HALTED    ",Font16x24);
	//armed_flag = 1;
	
	Display_workingMode(userProfile.user_profile.mode);//Added by Jason Chen, 2015.03.24
}

void Display_LaserOn(void)
{
	sLCD_putString_TFT(5,104-24-SHIFT_DOWN,(uint8_t*)"LASERS:ON     ",Font16x24);	
}

void Display_LaserOff(void)
{
	sLCD_putString_TFT(5,104-24-SHIFT_DOWN,(uint8_t*)"LASERS:OFF    ",Font16x24);	
}

void Display_TENsOn(void)
{	
	if(userProfile.user_profile.pTensDACVoltage>0)
	  sLCD_putString_TFT(5,80-24-SHIFT_DOWN,(uint8_t*)"TENS:  ON ",Font16x24);	
	else
		sLCD_putString_TFT(5,80-24-SHIFT_DOWN,(uint8_t*)"          ",Font16x24);	
}

void Display_Prompt(uint8_t prompt)               // Added by Jason Chen, 2015.04.09
{	
	if(prompt == PROMPT_WAIT)
	  sLCD_putString_TFT(28,80-48-SHIFT_DOWN,(uint8_t*)"   WAIT    ",Font16x24);	
	else if(prompt == PROMPT_START)
		sLCD_putString_TFT(28,80-48-SHIFT_DOWN,(uint8_t*)"   START   ",Font16x24);	  
	else if(prompt == PROMPT_PAUSE)
		sLCD_putString_TFT(28,80-48-SHIFT_DOWN,(uint8_t*)"   PAUSE   ",Font16x24);	  	
  else if(prompt == PROMPT_CALI)
		sLCD_putString_TFT(28,80-48-SHIFT_DOWN,(uint8_t*)"Cali Ready ",Font16x24);	  	
	else 
		sLCD_putString_TFT(28,80-48-SHIFT_DOWN,(uint8_t*)"           ",Font16x24);	
}


void Display_2s_count(uint32_t count)                              // Added by Jason Chen, 2015.04.09
{
	uint16_t tempColor = foreground_color;	
	foreground_color = COLOR_BLUE1;
	if(count > 99)
		count = 99;
	if(count > 0)
	{		
		sprintf((char*)displayBuff,"%d", count);
		if(count < 10)
		{			
			//sLCD_putString_TFT_ext(180,3,displayBuff, hallfetica_normal16x16);
			sLCD_putString_TFT_ext(194,3+16,displayBuff, SevenSegNumFont);			
		}
		else
		{			
			//sLCD_putString_TFT_ext(180,3,displayBuff, hallfetica_normal16x16);
			sLCD_putString_TFT_ext(170,3+16,displayBuff, SevenSegNumFont);
		}
	}
	else
	{
		sLCD_putString_TFT(170,48+16,(uint8_t*)"    ", Font16x24);
		sLCD_putString_TFT(170,24+16,(uint8_t*)"    ", Font16x24);
		sLCD_putString_TFT(170,3+16, (uint8_t*)"    ", Font16x24);
	}
	foreground_color = tempColor;		
}

void Display_TENsOff(void)
{
	if(userProfile.user_profile.pTensDACVoltage>0)
	  sLCD_putString_TFT(5,80-24-SHIFT_DOWN,(uint8_t*)"TENS:  OFF",Font16x24);	
	else
		sLCD_putString_TFT(5,80-24-SHIFT_DOWN,(uint8_t*)"          ",Font16x24);	
}

void Display_Power(uint16_t powerValue)
{
	float percentage = powerValue;
	uint16_t perValue;
	percentage = (percentage*100.0f)/4095.0f;
	perValue = (uint16_t)percentage ;
	percentage = percentage - (float)perValue;
	if(percentage > 0.5f)
		perValue +=1;
	if(perValue >= 100)		
	//sprintf((char*)displayBuff,"Power:%d%%   ", perValue);
	  sprintf((char*)displayBuff,"Power:%d  mW ", perValue);	                             
	else if((perValue >= 10)&&(perValue < 100))
	//sprintf((char*)displayBuff,"Power: %d%%   ", perValue);
	  sprintf((char*)displayBuff,"Power: %d  mW ", perValue);
	else
	//sprintf((char*)displayBuff,"Power:  %d%%   ", perValue);
	  sprintf((char*)displayBuff,"Power:  %d  mW ", perValue);
	sLCD_putString_TFT(5,     104+2*24-SHIFT_DOWN,(uint8_t*)displayBuff,Font16x24);
}

void DisplayTestOffset(uint16_t row, int16_t value)
{
  sprintf((char*)displayBuff,"offset%d: %d", row, value);
	sLCD_putString_TFT(5,     24+ row*24,(uint8_t*)displayBuff,Font16x24);
}

void DisplaySelfCali_backup(uint16_t row, float value)
{
	if(value >= 0.0f)
		sprintf((char*)displayBuff,"L%d: %2.3fV   ", row, value);
	else
		sprintf((char*)displayBuff,"L%d:%2.3fV   ", row, value);
  sLCD_putString_TFT(15,     30+ (6 - row)*24,(uint8_t*)displayBuff,Font16x24);	
}

#define REF_VOLT       3.0f
#define REF30_V        30.0f
#define FULL_SCALE     4095.0f
#define FONT           Font8x12
#define F_HIGHTH       12
#define F_WIDTH        8
void DisplaySelfCali(uint16_t row, float value, uint8_t flag)
{
	if(value >= 0.0f)
		sprintf((char*)displayBuff,"%d: %2.3fV   ", row, value);
	else
		sprintf((char*)displayBuff,"%d:%2.3fV   ", row, value);
	sLCD_putString_TFT(15,     24*4 + (6 - row)*F_HIGHTH,(uint8_t*)displayBuff,FONT);
	
	if(row == 1)
	{
		if(L_offset[0] >= 0.0f)
  		sprintf((char*)displayBuff,"  %2.3fV ", (REF30_V*L_offset[0])/FULL_SCALE);
	  else
		  sprintf((char*)displayBuff," %2.3fV ", (REF30_V*L_offset[0])/FULL_SCALE);
		sLCD_putString_TFT(15 + 9*8,     24*4 + (6 - row)*F_HIGHTH,(uint8_t*)displayBuff,FONT);
	}
	else if(row == 2)
	{
		if(L_offset[1] >= 0.0f)
  		sprintf((char*)displayBuff,"  %2.3fV ", (REF30_V*L_offset[1])/FULL_SCALE);
	  else
		  sprintf((char*)displayBuff," %2.3fV ", (REF30_V*L_offset[1])/FULL_SCALE);
		sLCD_putString_TFT(15 + 9*8,     24*4 + (6 - row)*F_HIGHTH,(uint8_t*)displayBuff,FONT);		
	}
	else if(row == 3)
	{
		if(L_offset[2] >= 0.0f)
  		sprintf((char*)displayBuff,"  %2.3fV ", (REF30_V*L_offset[2])/FULL_SCALE);
	  else
		  sprintf((char*)displayBuff," %2.3fV ", (REF30_V*L_offset[2])/FULL_SCALE);
		sLCD_putString_TFT(15 + 9*8,     24*4 + (6 - row)*F_HIGHTH,(uint8_t*)displayBuff,FONT);				
	}
	else if(row == 4)
	{
		if(L_offset[3] >= 0.0f)
  		sprintf((char*)displayBuff,"  %2.3fV ", (REF30_V*L_offset[3])/FULL_SCALE);
	  else
		  sprintf((char*)displayBuff," %2.3fV ", (REF30_V*L_offset[3])/FULL_SCALE);
		sLCD_putString_TFT(15 + 9*8,     24*4 + (6 - row)*F_HIGHTH,(uint8_t*)displayBuff,FONT);				
	}
	else if(row == 5)
	{
		if(L_offset[4] >= 0.0f)
  		sprintf((char*)displayBuff,"  %2.3fV ", (REF30_V*L_offset[4])/FULL_SCALE);
	  else
		  sprintf((char*)displayBuff," %2.3fV ", (REF30_V*L_offset[4])/FULL_SCALE);
		sLCD_putString_TFT(15 + 9*8,     24*4 + (6 - row)*F_HIGHTH,(uint8_t*)displayBuff,FONT);				
	}	

	if(flag)
	{
		if(row == 1)
		{
			if(O_offset[3] >= 0.0f)
				sprintf((char*)displayBuff,"  %2.3fV", (REF_VOLT*O_offset[3])/FULL_SCALE);
			else
				sprintf((char*)displayBuff," %2.3fV", (REF_VOLT*O_offset[3])/FULL_SCALE);
			sLCD_putString_TFT(15 + 10*8 + 8*8,     24*4 + (6 - row)*F_HIGHTH,(uint8_t*)displayBuff,FONT);		
		}
		else if(row == 2)
		{
			if(O_offset[2] >= 0.0f)
				sprintf((char*)displayBuff,"  %2.3fV", (REF_VOLT*O_offset[2])/FULL_SCALE);
			else
				sprintf((char*)displayBuff," %2.3fV", (REF_VOLT*O_offset[2])/FULL_SCALE);
			sLCD_putString_TFT(15 + 10*8 + 8*8,     24*4 + (6 - row)*F_HIGHTH,(uint8_t*)displayBuff,FONT);		
		}
		else if(row == 3)
		{
			if(O_offset[3] >= 0.0f)
				sprintf((char*)displayBuff,"  %2.3fV", (REF_VOLT*O_offset[3])/FULL_SCALE);
			else
				sprintf((char*)displayBuff," %2.3fV", (REF_VOLT*O_offset[3])/FULL_SCALE);
			sLCD_putString_TFT(15 + 10*8 + 8*8,     24*4 + (6 - row)*F_HIGHTH,(uint8_t*)displayBuff,FONT);		
		}
		else if(row == 4)
		{
			if(O_offset[1] >= 0.0f)
				sprintf((char*)displayBuff,"  %2.3fV", (REF_VOLT*O_offset[1])/FULL_SCALE);
			else
				sprintf((char*)displayBuff," %2.3fV", (REF_VOLT*O_offset[1])/FULL_SCALE);
			sLCD_putString_TFT(15 + 10*8 + 8*8,     24*4 + (6 - row)*F_HIGHTH,(uint8_t*)displayBuff,FONT);		
		}
		else if(row == 5)
		{
			if(O_offset[0] >= 0.0f)
				sprintf((char*)displayBuff,"  %2.3fV", (REF_VOLT*O_offset[0])/FULL_SCALE);
			else
				sprintf((char*)displayBuff," %2.3fV", (REF_VOLT*O_offset[0])/FULL_SCALE);
			sLCD_putString_TFT(15 + 10*8 + 8*8,     24*4 + (6 - row)*F_HIGHTH,(uint8_t*)displayBuff,FONT);		
		}	
  }
}

#define REF_VOLT30  30.0f
void Display905Voltage(uint16_t row, uint16_t DAC905_out)
{
	sprintf((char*)displayBuff," %2.2fV ", (REF_VOLT30*DAC905_out)/FULL_SCALE);
	sLCD_putString_TFT(15 + 10*8 + 8*8,     24*4 + (6 - row)*F_HIGHTH,(uint8_t*)displayBuff,FONT);
}

void DisplayOffsetVoltage(uint16_t row, int16_t offsetV)
{
	if(offsetV >= 0.0f)
		sprintf((char*)displayBuff,"  %2.3fV", (REF_VOLT30*offsetV)/FULL_SCALE);
	else
		sprintf((char*)displayBuff," %2.3fV", (REF_VOLT30*offsetV)/FULL_SCALE);
	sLCD_putString_TFT(15 + 10*8 + 8*8,     24*4 + (6 - row)*F_HIGHTH,(uint8_t*)displayBuff,FONT);
}

void DisplayPowermW(float powerValuemW)                                                       // Added by Jason Chen, 2015.09.19
{
	sprintf((char*)displayBuff,"Power:%4.2fmW", powerValuemW);
	//sprintf((char*)displayBuff,"%4.2f  ", powerValuemW);  
	sLCD_putString_TFT(5,     104+2*24-SHIFT_DOWN,(uint8_t*)"              ",Font16x24);	      // Added by jason Chen, 2015.04.02
	sLCD_putString_TFT(5,     104+2*24-SHIFT_DOWN,(uint8_t*)displayBuff,Font16x24);
}

void DisplayOneNumber(uint32_t oneNumber)                                                       // Added by Jason Chen, 2016.08.05
{
	sprintf((char*)displayBuff,"Number:%d", oneNumber);
//sprintf((char*)displayBuff,"%4.2f  ", powerValuemW);  
	sLCD_putString_TFT(5,     104+2*24-SHIFT_DOWN,(uint8_t*)"              ",Font16x24);	      // Added by jason Chen, 2015.04.02
	sLCD_putString_TFT(5,     104+2*24-SHIFT_DOWN,(uint8_t*)displayBuff,Font16x24);
}

void DisplayNoPowermW(void)
{	 
	sLCD_putString_TFT(5,     104+2*24-SHIFT_DOWN,(uint8_t*)"              ",Font16x24);	      // Added by jason Chen, 2015.04.02 
}

void Display_TensPower_old(uint16_t powerValue)
{
	float percentage = powerValue;
	uint16_t perValue;
	
	if(powerValue > 0)
	{
		percentage = (percentage*100.0f)/4095.0f;
		perValue = (uint16_t)percentage ;
		percentage = percentage - (float)perValue;
		if(percentage > 0.5f)
			perValue +=1;
		if(perValue >= 100)
			sprintf((char*)displayBuff,"Tens: %d%%   ",perValue);
		else if((perValue >= 10)&&(perValue < 100))
			sprintf((char*)displayBuff,"Tens:  %d%%   ",perValue);
		else
			sprintf((char*)displayBuff,"Tens:   %d%%   ",perValue);
		sLCD_putString_TFT(5,     104+2*24-SHIFT_DOWN,(uint8_t*)displayBuff,Font16x24);
  }
}

void Display_TensPower(uint16_t powerValue)
{
	float percentage = powerValue;
	uint16_t perValue;
	if(powerValue > 0)	
	{
			percentage = (percentage*100.0f)/4095.0f;
			perValue = (uint16_t)percentage ;
			percentage = percentage - (float)perValue;
			if(percentage > 0.5f)
				perValue +=1;
			if(perValue >= 100)
				sprintf((char*)displayBuff,"Tens :%d%%   ",perValue);
			else if((perValue >= 10)&&(perValue < 100))
				sprintf((char*)displayBuff,"Tens : %d%%   ",perValue);
			else
				sprintf((char*)displayBuff,"Tens :  %d%%   ",perValue);	
			sLCD_putString_TFT(5,     104+24-SHIFT_DOWN,(uint8_t*)displayBuff,Font16x24);
	}	
	else
		sLCD_putString_TFT(5,104+24-SHIFT_DOWN,(uint8_t*)"              ",Font16x24);	
}

void Display_PowerByte(uint8_t powerValue)
{
	uint16_t perValue = powerValue;
	if(perValue > 100)
		perValue = 100;	
	sprintf((char*)displayBuff,"Power:%d%%", perValue);
	sLCD_putString_TFT(5,     92+2*24-SHIFT_DOWN,(uint8_t*)displayBuff,Font16x24);
}

void Display_TensPowerByte(uint8_t powerValue)
{
	uint16_t perValue = powerValue;
	if(perValue > 100)
		perValue = 100;	
	sprintf((char*)displayBuff,"Tens: %d%%", perValue);
	sLCD_putString_TFT(5,     92+2*24-SHIFT_DOWN,(uint8_t*)displayBuff,Font16x24);
}

void Display_BT_Bar(uint16_t x, uint16_t y, uint16_t bar_width)
{
	uint8_t i;//,j;
	uint8_t len = 0;
	for(i = 0;i < bar_width;i++)
	{
	  len = i/2;
	  drawLine(x+2*i,   y, x+2*i,   y+len);//y+11);
		drawLine(x+2*i+1, y, x+2*i+1, y+len);//y+11);
	}
}

void Display_ProgressBar(uint16_t x, uint16_t y, uint16_t bar_width)
{
	uint8_t i;//,j;
	uint16_t fcolor,bcolor;
	//uint8_t len = 0;
	for(i = 0;i < bar_width;i++)
	{
		drawLine(x+2*i, y, x+2*i, y+30);
		drawLine(x+2*i+1,y, x+2*i+1, y+30);
		sprintf((char*)displayBuff,"%d%%", i+1);
		fcolor = getColor();
		bcolor = getBackColor();
		
		setColor(VGA_LIME);
		if((x+2*i) >= 90)
		{			
			setBackColor(fcolor);
			//if((x+2*i) >= 90+40)
				print_ttf((char*)displayBuff, 90, y+6, 0);	
			//else
			//	print_ttf((char*)displayBuff, 90-40, y+6, 0);	
		}
		else
		{
			print_ttf((char*)displayBuff, 90, y+6, 0);	
		}
		
		setColor(fcolor);
		setBackColor(bcolor);		
		
#if 0
    fcolor = getColor();
		_transparent = 1;
    if (!_transparent)
    {
        fontHeight = getFontHeight();
        setColor(getBackColor());
        fillRect(x, y, x + fontChar.xDelta+1, y + fontHeight);
        setColor(fcolor);
    }
#endif
    //setColor(fcolor);
		
	 Delay(120);
	}
}


extern char displayBuff_ttf[20];
void Display_BT_RSSI(int16_t rssi)
{
	int32_t rssi_16;
	uint16_t barWidth;
#if 0
	if(rssi <= -30000)
		sLCD_putString_TFT(BT_LOCATION,300, (uint8_t*)"BT     ",Font12x12);
	else if(rssi > -30000 && rssi <= -10000)
		sLCD_putString_TFT(BT_LOCATION,300, (uint8_t*)"BT|    ",Font12x12);
	else if(rssi > -10000 && rssi <= 0)
		sLCD_putString_TFT(BT_LOCATION,300, (uint8_t*)"BT||   ",Font12x12);	
	else if(rssi > 0 && rssi <= 10000)
		sLCD_putString_TFT(BT_LOCATION,300, (uint8_t*)"BT|||  ",Font12x12);		
	else if(rssi > 10000 && rssi <= 20000)
		sLCD_putString_TFT(BT_LOCATION,300, (uint8_t*)"BT|||| ",Font12x12);			
	else //if(rssi > 60 && rssi <= 80)
		sLCD_putString_TFT(BT_LOCATION,300, (uint8_t*)"BT|||||",Font12x12);				
#else
	rssi_16  = (int32_t)rssi;
	rssi_16 += (uint32_t)32768;
	barWidth   = rssi_16/2184;
	sLCD_putString_TFT(BT_LOCATION+24,308, (uint8_t*)"     ",Font12x12);
  sLCD_putString_TFT(BT_LOCATION+24,305, (uint8_t*)"     ",Font12x12);
	
	if(barWidth > 30)
		barWidth = 30;
	Display_BT_Bar(BT_LOCATION+24, 305, barWidth);
	
	//sprintf(displayBuff_ttf,"%d   ", barWidth);
	//sLCD_putString_TFT(BT_LOCATION+24,280,(uint8_t*)displayBuff_ttf,Font12x12);
#endif	
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
