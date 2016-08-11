/**
  ******************************************************************************
  * @file    TLC2000_spi_LCD.c
  * @author  Jason Chen
  * @version V1.0
  * @date    16-May-2014
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
#include "TLC2000_SPI_LCD.h"
#include "fnt08x08.h"


/**
  * @brief  DeInitializes the peripherals used by the SPI FLASH driver.
  * @param  None
  * @retval None
  */
void sLCD_SPI_DeInit(void)
{
  sLCD_LowLevel_DeInit();
}

/**
  * @brief  Initializes the peripherals used by the SPI FLASH driver.
  * @param  None
  * @retval None
  */
void sLCD_SPI_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;

  sLCD_LowLevel_Init();
    
  /*!< Deselect the FLASH: Chip Select high */
  sLCD_CS_HIGH();

  /*!< SPI configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;//SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;

  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;//SPI_BaudRatePrescaler_4;  --> 72MHz/8 = 9MHz

  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(sLCD_SPI, &SPI_InitStructure);

  /*!< Enable the sFLASH_SPI  */
  SPI_Cmd(sLCD_SPI, ENABLE);
	
	sLCD_EN_VP_HIGH();
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
/*uint8_t*/void sLCD_SendByte(uint8_t byte)
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
uint16_t sLCD_SendHalfWord(uint16_t HalfWord)
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
void sLCD_Write_Command(uint8_t command)
{
    sLCD_CS_LOW();          // chip selected
	  sLCD_DC_LOW();          // command mode
	  
	  sLCD_SendByte(command); // set up data on bus
	
	  sLCD_CS_HIGH();         // deselect chip
}

/*************************************************/
void sLCD_write_data(uint8_t data)
{
   sLCD_DC_HIGH();          // SET_CD;       // data mode
   sLCD_CS_LOW();           // CLR_CS;       // chip selected

   sLCD_SendByte(data);     // SPI_WriteByte(data);	// set up data on bus

   sLCD_CS_HIGH();          // SET_CS;       // deselect chip
}
/*************************************************/

/*************************************************/
void sLCD_init(void)
{
	 sLCD_RST_LOW();              // CLR_RESET;
	 Delay(100);
	 sLCD_RST_HIGH();             // SET_RESET;
	 Delay(100);

	 sLCD_Write_Command(0xAE);    //  10101111:  set display off
   sLCD_Write_Command(0xA4);    //  10100100:  set nomal display(POR)
 //sLCD_Write_Command(0xAF);    //  display on
#if 1
	 sLCD_Write_Command(0x15);    //  00010101:  Set Column address
	 sLCD_Write_Command(0x00);    //  00000000:  A[6:0] sets the column start address from 0-95, RESET=00d.
	 sLCD_Write_Command(0x5F);    //  01011111:  B[6:0] sets the column end address from 0-95 RESET=95d.

	 sLCD_Write_Command(0x75);    //  10000001:  Set Row Address
	 sLCD_Write_Command(0x00);    //  00000000:  A[5:0] sets the row start address from 0-63, RESET=00d.
	 sLCD_Write_Command(0x3F);    //  00111111:  B[5:0] sets the row end address from 0-63, RESET=63d.

	 sLCD_Write_Command(0x87);    //  10000111:  Master current Control
	 sLCD_Write_Command(0x07);    //  00000111:  00000000 to 00001111 for attenuation factor (less is more)

	 sLCD_Write_Command(0x81);    //  10000001:  set contrast control for color A
	 sLCD_Write_Command(0xA0);    //  11111111:  contrast set(select from 1 to 256)

	 sLCD_Write_Command(0x82);    //  10000010:  set contrast control for color B
	 sLCD_Write_Command(0x60);    //  11111111:  contrast set(select from 1 to 256)

	 sLCD_Write_Command(0x83);    //  10000011:  set contrast control for color C
	 sLCD_Write_Command(0xB0);    //  11111111:  contrast set(select from 1 to 256)

	 sLCD_Write_Command(0xA0);    //  10100000:  set re-map & data format
	 sLCD_Write_Command(0x60);    //  01100000:  a[7,6]:01:65k color format(por)CCCCCBBBBBBAAAAA   00:256 color formatCCCBBBAA

	 sLCD_Write_Command(0xA1);	  //  10100001:  set display start line
	 sLCD_Write_Command(0x00);
	
	 sLCD_Write_Command(0xA2);    //  10100010:  set display offset
	 sLCD_Write_Command(0x00);
	
	 sLCD_Write_Command(0xA8);	  //  10101000:  set multiplex ratio
	 sLCD_Write_Command(0x3F);
	
	 sLCD_Write_Command(0xB0);    //  set power save
	 sLCD_Write_Command(0x00);    //  0x12:power saving mode
	
	 sLCD_Write_Command(0xB1);		//  phase 1 and period adjustment(discharge,charge)
	 sLCD_Write_Command(0x74);
	
	 sLCD_Write_Command(0xB3);		//  display clock divider/oscillator frequency
	 sLCD_Write_Command(0xD0);

	 sLCD_Write_Command(0xBB);		//  set vpa 
	 sLCD_Write_Command(0x1F);

	 sLCD_Write_Command(0xBC);		//  set vpb
	 sLCD_Write_Command(0x1F);

	 sLCD_Write_Command(0xBD);    //  set vpc
	 sLCD_Write_Command(0x1F);		
	
	 sLCD_Write_Command(0xBE);		//  set vcomh
	 sLCD_Write_Command(0x3F); 

	 sLCD_Write_Command(0xAD);    //  10101101:  set contrast control for color A
	 sLCD_Write_Command(0x8E);    //  11111111:  contrast set(select from 1 to 256)

   sLCD_Write_Command(0xAF);    //  display on
#endif	 
}

/*************************************************/
void sLCD_Display_Home(void)
{
	sLCD_Write_Command(0x15);    //  00010101:  Set Column address
	sLCD_Write_Command(0x00);    //  00000000:  A[6:0] sets the column start address from 0-95, RESET=00d.
	sLCD_Write_Command(0x5F);    //  01011111:  B[6:0] sets the column end address from 0-95 RESET=95d.

	sLCD_Write_Command(0x75);    //  10000001:  Set Row Address
	sLCD_Write_Command(0x00);    //  00000000:  A[5:0] sets the row start address from 0-63, RESET=00d.
	sLCD_Write_Command(0x3F);    //  00111111:  B[5:0] sets the row end address from 0-63, RESET=63d.
}

/*************************************************/
void sLCD_Display_RGB(void)
{
	int16_t i,j;
	
	sLCD_Display_Home();

// white, red, green, blue screen
	for(i=0;i<64;i++)
	{
		for(j=0;j<96;j++)
		{
			if(j>=0 && j<24)
			{
				sLCD_write_data(0xff);
				sLCD_write_data(0xff);		//white display
			}
			if(j>23 && j<48)
			{
				sLCD_write_data(0x00);
				sLCD_write_data(0x1f);		//blue display
			}
			if(j>47 && j<72)
			{
				sLCD_write_data(0x07);		//green display
				sLCD_write_data(0xe0);
			}
			if(j>71 && j<96)
			{
				sLCD_write_data(0xf8);		//red display
				sLCD_write_data(0x00);
			}
		}
	}
}

/*************************************************/
void sLCD_background(uint8_t color)
{
	uint8_t i,j;

	sLCD_Write_Command(0x15);    //  00010101:  Set Column address
	sLCD_Write_Command(0x00);    //  00000000:  A[6:0] sets the column start address from 0-95, RESET=00d.
	sLCD_Write_Command(0x5F);    //  01011111:  B[6:0] sets the column end address from 0-95 RESET=95d.

	sLCD_Write_Command(0x75);    //  10000001:  Set Row Address
	sLCD_Write_Command(0x00);    //  00000000:  A[5:0] sets the row start address from 0-63, RESET=00d.
	sLCD_Write_Command(0x3F);    //  00111111:  B[5:0] sets the row end address from 0-63, RESET=63d.

	for(i=0;i<64;i++)
	{
		for(j=0;j<=96;j++)
		{
		  if(color==0)
			{
				sLCD_write_data(0xff);
				sLCD_write_data(0xff);		//white display
			}
		  if(color==1)
			{
				sLCD_write_data(0x00);
				sLCD_write_data(0x1f);		//blue display
			}
		  if(color==2)
			{
				sLCD_write_data(0x07);		//green display
				sLCD_write_data(0xe0);
			}
		  if(color==3)
			{
				sLCD_write_data(0xf8);		//red display
				sLCD_write_data(0x00);
			}		
		  if(color==4)
			{
				sLCD_write_data(0x00);		//black display
				sLCD_write_data(0x00);
			}
			if(color == 5)
			{
				//sLCD_Display_RGB();
			}
		}       
	}
}

void SetDisplayLocation(uint8_t x, uint8_t y)
{
   sLCD_Write_Command(0x15);    //  00010101:  Set Column address
   sLCD_Write_Command(x);    //  00000000:  A[6:0] sets the column start address from 0-95, RESET=00d.
   sLCD_Write_Command(0x5F);    //  01011111:  B[6:0] sets the column end address from 0-95 RESET=95d.

   sLCD_Write_Command(0x75);    //  10000001:  Set Row Address
   sLCD_Write_Command(y);    //  00000000:  A[5:0] sets the row start address from 0-63, RESET=00d.
	sLCD_Write_Command(0x3F);    //  00111111:  B[5:0] sets the row end address from 0-63, RESET=63d.			
}

void SetDisplayLocationX(uint8_t x)
{
   sLCD_Write_Command(0x15);    //  00010101:  Set Column address
   sLCD_Write_Command(x);    //  00000000:  A[6:0] sets the column start address from 0-95, RESET=00d.
   sLCD_Write_Command(0x5F);    //  01011111:  B[6:0] sets the column end address from 0-95 RESET=95d.
}

void SetDisplayLocationY(uint8_t y)
{
   sLCD_Write_Command(0x75);    //  10000001:  Set Row Address
   sLCD_Write_Command(y);    //  00000000:  A[5:0] sets the row start address from 0-63, RESET=00d.
   sLCD_Write_Command(0x3F);    //  00111111:  B[5:0] sets the row end address from 0-63, RESET=63d.			
}


void SetVerticalScroll(uint8_t scr)
{
   sLCD_Write_Command(0xA2);  
   sLCD_Write_Command(scr);    
}

void ClearLastLine(uint8_t row)
{
	uint8_t i, j;
	
  sLCD_Write_Command(0x15);    //  00010101:  Set Column address
  sLCD_Write_Command(0);    //  00000000:  A[6:0] sets the column start address from 0-95, RESET=00d.
  sLCD_Write_Command(0x5F);    //  01011111:  B[6:0] sets the column end address from 0-95 RESET=95d.

  sLCD_Write_Command(0x75);    //  10000001:  Set Row Address
  sLCD_Write_Command(row);    //  00000000:  A[5:0] sets the row start address from 0-63, RESET=00d.
	sLCD_Write_Command(0x3F);    //  00111111:  B[5:0] sets the row end address from 0-63, RESET=63d.				
	for(i=row;i<row+8;i++)
	{
		for(j=0;j<=96;j++)
		{
  		sLCD_write_data(0x00);
			sLCD_write_data(0x00);		//white display
		}
	}
}

uint8_t *pStr;// = TestStr;

void sLCD_putChar(uint8_t x, uint8_t y, uint8_t TestChar)
{
	uint8_t i,j,k;
	uint8_t tempChar;
		for(i=0;i<8;i++)
		{			
      sLCD_Write_Command(0x15);    //  00010101:  Set Column address
	    sLCD_Write_Command(x);    //  00000000:  A[6:0] sets the column start address from 0-95, RESET=00d.
	    sLCD_Write_Command(0x5F);    //  01011111:  B[6:0] sets the column end address from 0-95 RESET=95d.

	    sLCD_Write_Command(0x75);    //  10000001:  Set Row Address
	    sLCD_Write_Command(y + i);    //  00000000:  A[5:0] sets the row start address from 0-63, RESET=00d.
			sLCD_Write_Command(0x3F);    //  00111111:  B[5:0] sets the row end address from 0-63, RESET=63d.			
			k = 0x01;
			for(j=0;j<8;j++)
			{
				tempChar = Font_char[TestChar-0x20][j];				
				//tempChar = Font_char['J'-0x20][j];				
				if(tempChar & (k<<i))
				{
				  sLCD_write_data(0x07);		//green display
				  sLCD_write_data(0xe0);
			  }
				else
				{
				  sLCD_write_data(0x00);		//green display
				  sLCD_write_data(0x00);					
				}				
			}
		}	
}

void sLCD_pChar(uint8_t TestChar)
{
	uint8_t i,j,k;
	uint8_t tempChar;
	
	for(i=0;i<8;i++)
	{	
		k = 0x01;
		for(j=0;j<8;j++)
		{
			tempChar = Font_char[TestChar-0x20][j];				
			//tempChar = Font_char['J'-0x20][j];				
			if(tempChar & (k<<i))
			{
				sLCD_write_data(0x07);		//green display
				sLCD_write_data(0xe0);
			}
			else
			{
				sLCD_write_data(0x00);		//green display
				sLCD_write_data(0x00);					
			}				
		}
	}	
}

void sLCD_putString(uint8_t x, uint8_t y, uint8_t * Str)
{
//	uint8_t i,j;//,k;
	//uint8_t tempChar;//,count=0;
	pStr = Str;
#if 0
	sLCD_Write_Command(0x15);    //  00010101:  Set Column address
	sLCD_Write_Command(0x00);    //  00000000:  A[6:0] sets the column start address from 0-95, RESET=00d.
	sLCD_Write_Command(0x5F);    //  01011111:  B[6:0] sets the column end address from 0-95 RESET=95d.

	sLCD_Write_Command(0x75);    //  10000001:  Set Row Address
	sLCD_Write_Command(0x00);    //  00000000:  A[5:0] sets the row start address from 0-63, RESET=00d.
	sLCD_Write_Command(0x3F);    //  00111111:  B[5:0] sets the row end address from 0-63, RESET=63d.

	for(i=0;i<64;i++)
	{
			for(j=0;j<=96;j++)
			{
				sLCD_write_data(0xff);
				sLCD_write_data(0xff);		//white display
			}
	}
	Delay(500);
#endif
  //sLCD_Write_Command(0x15);    //  00010101:  Set Column address
  //sLCD_Write_Command(x);    //  00000000:  A[6:0] sets the column start address from 0-95, RESET=00d.
  //sLCD_Write_Command(0x5F);    //  01011111:  B[6:0] sets the column end address from 0-95 RESET=95d.

	//sLCD_Write_Command(0x75);    //  10000001:  Set Row Address
	//sLCD_Write_Command(y);    //  00000000:  A[5:0] sets the row start address from 0-63, RESET=00d.
	//sLCD_Write_Command(0x3F);    //  00111111:  B[5:0] sets the row end address from 0-63, RESET=63d.				
	while(*pStr)
	{
#if 0		
		for(i=0;i<8;i++)
		{			
      sLCD_Write_Command(0x15);    //  00010101:  Set Column address
	    sLCD_Write_Command(x);    //  00000000:  A[6:0] sets the column start address from 0-95, RESET=00d.
	    sLCD_Write_Command(0x5F);    //  01011111:  B[6:0] sets the column end address from 0-95 RESET=95d.

	    sLCD_Write_Command(0x75);    //  10000001:  Set Row Address
	    sLCD_Write_Command(y + i);    //  00000000:  A[5:0] sets the row start address from 0-63, RESET=00d.
			sLCD_Write_Command(0x3F);    //  00111111:  B[5:0] sets the row end address from 0-63, RESET=63d.			
			k = 0x01;
			for(j=0;j<8;j++)
			{
				tempChar = Font_char[(*pStr)-0x20][j];				
				//tempChar = Font_char['J'-0x20][j];				
				if(tempChar & (k<<i))
				{
				  sLCD_write_data(0xf8);		//green display
				  sLCD_write_data(0x00);
			  }
				else
				{
				  sLCD_write_data(0xff);		//green display
				  sLCD_write_data(0xff);					
				}				
			}
		}
#else
		sLCD_putChar(x, y, *pStr);
#endif		
    //count +=8;
		x +=8;
		pStr++;
		//sLCD_Write_Command(0x15);    //  00010101:  Set Column address
    //sLCD_Write_Command(x + count);    //  00000000:  A[6:0] sets the column start address from 0-95, RESET=00d.
	  //sLCD_Write_Command(0x5F);    //  01011111:  B[6:0] sets the column end address from 0-95 RESET=95d.

	  //sLCD_Write_Command(0x75);    //  10000001:  Set Row Address
	  //sLCD_Write_Command(y);    //  00000000:  A[5:0] sets the row start address from 0-63, RESET=00d.
	  //sLCD_Write_Command(0x3F);    //  00111111:  B[5:0] sets the row end address from 0-63, RESET=63d.					
	}
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
