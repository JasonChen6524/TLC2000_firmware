/**
  ******************************************************************************
  * @file    AD5504_DAC.c
  * @author  Jason Chen
  * @version V1.0
  * @date    18-June-2014
  * @brief   
  *            
  */ 

/* Includes ------------------------------------------------------------------*/
#include "AD5504_DAC.h"
#include "hw_config.h"

#include "MiscTools.h"


/**
  * @brief  DeInitializes the peripherals used by the SPI driver.
  * @param  None
  * @retval None
  */
void AD5504_SPI_DeInit(void)
{
  //sLCD_LowLevel_DeInit();
}

void AD5504_SPI_GPIO_Init(void)
{
	s905_SPI_LowLevel_Init();
    
  /*!< Deselect the FLASH: Chip Select high */
  s905_DAC_CS0_HIGH();
	s905_DAC_CS1_HIGH();
	s660_DAC_CS_HIGH();
	
	S905_DAC_CLR();
	Delay(10);
	S905_DAC_ENABLE();
	S905_DAC_NORMAL();	 
	Delay(10);

	Power_V30_On();
}

/**
  * @brief  Initializes the peripherals used by the SPI FLASH driver.
  * @param  None
  * @retval None
  */
void S905_DAC_SPI_Init(uint16_t SPI_CPHA)
{
  SPI_InitTypeDef  SPI_InitStructure;

	SPI_Cmd(s905_DAC_SPI, DISABLE);
	
  /*!< SPI configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;//SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;

  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;//SPI_BaudRatePrescaler_4;  --> 72MHz/8 = 9MHz

  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(s905_DAC_SPI, &SPI_InitStructure);

  /*!< Enable the sFLASH_SPI  */
  SPI_Cmd(s905_DAC_SPI, ENABLE);
	
}


/**
  * @brief  Sends a byte through the SPI interface and return the byte received
  *         from the SPI bus.
  * @param  byte: byte to send.
  * @retval The value of the received byte.
  */
uint8_t S905_DAC_SendByte(uint8_t byte)
{
  /*!< Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(s905_DAC_SPI, SPI_I2S_FLAG_TXE) == RESET);

  /*!< Send byte through the SPI2 peripheral */
  SPI_I2S_SendData(s905_DAC_SPI, byte);

  /*!< Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(s905_DAC_SPI, SPI_I2S_FLAG_RXNE) == RESET);

  /*!< Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(s905_DAC_SPI);
}

/**
  * @brief  Sends a Half Word through the SPI interface and return the Half Word
  *         received from the SPI bus.
  * @param  HalfWord: Half Word to send.
  * @retval The value of the received Half Word.
  */
uint16_t S905_DAC_SendHalfWord(uint16_t HalfWord)
{
  /*!< Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(s905_DAC_SPI, SPI_I2S_FLAG_TXE) == RESET);

  /*!< Send Half Word through the sFLASH peripheral */
  SPI_I2S_SendData(s905_DAC_SPI, HalfWord);

  /*!< Wait to receive a Half Word */
  while (SPI_I2S_GetFlagStatus(s905_DAC_SPI, SPI_I2S_FLAG_RXNE) == RESET);

  /*!< Return the Half Word read from the SPI bus */
  return SPI_I2S_ReceiveData(s905_DAC_SPI);
}

/**
  * @brief  Reads a byte from the SPI Flash.
  * @note   This function must be used only if the Start_Read_Sequence function
  *         has been previously called.
  * @param  None
  * @retval Byte Read from the SPI Flash.
  */

uint16_t s905_DAC_ReadHalfWord(uint16_t reg)
{
	uint16_t rdValue;
	uint16_t regValue = 0x8000 | (reg << 12);
	
	s905_DAC_CS0_LOW();
  rdValue =  S905_DAC_SendHalfWord(regValue);
	s905_DAC_CS0_HIGH();
	return rdValue;
}

/*************************************************/
void S905_DAC_write_control0(uint16_t data)
{
	uint16_t ctrl_reg = 0x007F & data;
  s905_DAC_CS0_LOW();
  S905_DAC_SendHalfWord(CTL_REG_WRITE|ctrl_reg);
  //s905_DAC_CS0_HIGH();
	//MyDelay(10);
	//s905_DAC_CS0_LOW();
	S905_DAC_SendHalfWord(s905_DUMMY_WORD_WR);
  s905_DAC_CS0_HIGH();
}
/*************************************************/
void S905_DAC_write_control1(uint16_t data)
{
	uint16_t ctrl_reg = 0x007F & data;
  s905_DAC_CS1_LOW();
  S905_DAC_SendHalfWord(CTL_REG_WRITE|ctrl_reg);
  //s905_DAC_CS1_HIGH();
	//MyDelay(10);
	//s905_DAC_CS1_LOW();
	S905_DAC_SendHalfWord(s905_DUMMY_WORD_WR);
  s905_DAC_CS1_HIGH();
}
/*************************************************/
void S905_DAC_CLR(void)
{
  s905_DAC_CLR_LOW();
}

void S905_DAC_ENABLE(void)
{
  s905_DAC_CLR_HIGH();
}

void S905_DAC_NORMAL(void)
{
	S905_DAC_SPI_Init(s905_SPI);
	
	s905_DAC_CLR_HIGH();
	S905_DAC_write_control0(DAC_A_OUT|DAC_B_OUT|DAC_C_OUT|DAC_D_OUT);
	//S905_DAC_write_control0(DAC_B_OUT);
	//S905_DAC_write_control0(DAC_C_OUT);
	//S905_DAC_write_control0(DAC_D_OUT);	
	S905_DAC_write_control1(DAC_A_OUT|DAC_B_OUT|DAC_C_OUT);
	//S905_DAC_write_control1(DAC_B_OUT);	
	//S905_DAC_write_control1(DAC_C_OUT);	
}

void S905_DAC_OUT_VOLT(/*S905_DAC_TypeDef*/uint16_t sel, uint16_t data)
{
	uint16_t data_reg = 0x0FFF & data;
	uint16_t dac_sel = sel+1;
	//if(dac_sel > 6)
  //  return;
	S905_DAC_SPI_Init(s905_SPI);
	if(dac_sel <= 4)
	{
		dac_sel = (dac_sel << 12) & 0x7000;
		s905_DAC_CS0_LOW();		
		S905_DAC_SendHalfWord(dac_sel|data_reg);		
		s905_DAC_CS0_HIGH();
	}
	else
	{
		dac_sel = dac_sel-4;
		dac_sel = dac_sel << 12;
		s905_DAC_CS1_LOW();
		S905_DAC_SendHalfWord(dac_sel|data_reg);
		s905_DAC_CS1_HIGH();
	}
}

void ALL_S905_DAC_OUT_VOLT(uint16_t data)
{

	uint16_t data_reg = 0x0FFF & data;
	uint16_t dac_sel;
	
	S905_DAC_SPI_Init(s905_SPI);
	dac_sel = 0x5000;
  s905_DAC_CS0_LOW();		
	S905_DAC_SendHalfWord(dac_sel|data_reg);		
	s905_DAC_CS0_HIGH();
	
	s905_DAC_CS1_LOW();		
	S905_DAC_SendHalfWord(dac_sel|data_reg);		
	s905_DAC_CS1_HIGH();
}

void S660_DAC_OUT_VOLT(S660_DAC_TypeDef sel, uint16_t data)
{
	uint16_t data_reg = 0x0FFF & data;
	uint16_t dac_sel = sel;
	
	S905_DAC_SPI_Init(s660_SPI);
	if(dac_sel > 3)
    return;
	dac_sel = dac_sel << 14;
	dac_sel = dac_sel | 0x1000;    // Write and update
  s660_DAC_CS_LOW();		
	S905_DAC_SendHalfWord(dac_sel|data_reg);		
	s660_DAC_CS_HIGH();	
}
