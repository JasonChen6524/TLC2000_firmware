/**
  ******************************************************************************
  * @file    DAC7818S.c
  * @author  Jason Chen
  * @version V1.0
  * @date    06-OCT-2014
  * @brief   
  *            
  */ 

/* Includes ------------------------------------------------------------------*/
#include "DAC7718.h"
#include "hw_config.h"

#include "MiscTools.h"


/**
  * @brief  DeInitializes the peripherals used by the SPI driver.
  * @param  None
  * @retval None
  */
void DAC7718_SPI_DeInit(void)
{
  //sLCD_LowLevel_DeInit();
}

void DAC7718_SPI_GPIO_Init(void)
{
	s905_SPI_LowLevel_Init();
    
  /*!< Deselect the FLASH: Chip Select high */
  s905_DAC_CS0_HIGH();
	s905_DAC_RST_HIGH();
	s660_DAC_CS_HIGH();
	
	DAC7718_CLR();
	Delay(10);
	DAC7718_ENABLE();
	DAC7718_NORMAL();	 
	Delay(10);

	Power_V30_Off();                                    // Changed from ON to OFF, 2016.03.18
}

/**
  * @brief  Initializes the peripherals used by the SPI FLASH driver.
  * @param  None
  * @retval None
  */
void DAC7718_SPI_Init(uint16_t SPI_DATASIZE, uint16_t SPI_CPOL, uint16_t SPI_CPHA)
{
  SPI_InitTypeDef  SPI_InitStructure;

	SPI_Cmd(s905_DAC_SPI, DISABLE);
	
  /*!< SPI configuration */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DATASIZE;//SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL;
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
uint8_t DAC7718_SendByte(uint8_t byte)
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
uint16_t DAC7718_SendHalfWord(uint16_t HalfWord)
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
  * @brief  Sends 3 bytes through the SPI interface and return the byte received
  *         from the SPI bus.
  * @param  3byte: 3byte to send.
  * @retval The value of the received byte.
  */
uint32_t DAC7718_SendTripleByte(uint32_t TripByte)
{
	uint8_t sendByte = 0;
	uint32_t temp = TripByte;
	uint32_t ret_value = 0;
  /*!< Loop while DR register in not emplty */
  while (SPI_I2S_GetFlagStatus(s905_DAC_SPI, SPI_I2S_FLAG_TXE) == RESET);

	temp = (temp >> 16)&0x0000FF;
	sendByte = (uint8_t)temp;
  /*!< Send byte through the SPI2 peripheral */
  SPI_I2S_SendData(s905_DAC_SPI, sendByte);
  /*!< Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(s905_DAC_SPI, SPI_I2S_FLAG_RXNE) == RESET);
  /*!< Return the byte read from the SPI bus */
  ret_value = SPI_I2S_ReceiveData(s905_DAC_SPI) & 0x00FF;
	ret_value = (ret_value << 8)&0x00FF00;
	
	temp = TripByte;
	temp = (temp >> 8)&0x0000FF;
	sendByte = (uint8_t)temp;
  /*!< Send byte through the SPI2 peripheral */
  SPI_I2S_SendData(s905_DAC_SPI, sendByte);
  while (SPI_I2S_GetFlagStatus(s905_DAC_SPI, SPI_I2S_FLAG_RXNE) == RESET);
  /*!< Return the byte read from the SPI bus */
  ret_value |= (SPI_I2S_ReceiveData(s905_DAC_SPI) & 0x00FF);
	ret_value = (ret_value << 8)&0xFFFF00;	
	
	temp = TripByte;
	temp = (temp >> 0)&0x0000FF;
	sendByte = (uint8_t)temp;
  /*!< Send byte through the SPI2 peripheral */
  SPI_I2S_SendData(s905_DAC_SPI, sendByte);	
  /*!< Wait to receive a byte */
  while (SPI_I2S_GetFlagStatus(s905_DAC_SPI, SPI_I2S_FLAG_RXNE) == RESET);
  /*!< Return the byte read from the SPI bus */
  ret_value |= (SPI_I2S_ReceiveData(s905_DAC_SPI) & 0x00FF);	

  /*!< Return the byte read from the SPI bus */
  return ret_value;
}

uint32_t DAC7718_Read_RegisterVaule(uint32_t registerAddr)
{
   return DAC7718_SendTripleByte(DAC7718_READ|registerAddr);
}
/**
  * @brief  Reads a byte from the SPI Flash.
  * @note   This function must be used only if the Start_Read_Sequence function
  *         has been previously called.
  * @param  None
  * @retval Byte Read from the SPI Flash.
  */

uint16_t DAC7718_ReadHalfWord(uint16_t reg)
{
	uint16_t rdValue;
	uint16_t regValue = 0x8000 | (reg << 12);
	
	s905_DAC_CS0_LOW();
  rdValue =  DAC7718_SendHalfWord(regValue);
	s905_DAC_CS0_HIGH();
	return rdValue;
}


/*************************************************/
void DAC7718_CLR(void)
{
  s905_DAC_CLR_LOW();
}

void DAC7718_ENABLE(void)
{
  s905_DAC_CLR_HIGH();
}

void DAC7718_NORMAL(void)
{
	DAC7718_SPI_Init(SPI_DataSize_8b, SPI_CPOL_High, s905_SPI);
	
	s905_DAC_CLR_HIGH();
	//DAC7718_write_control0(DAC_A_OUT|DAC_B_OUT|DAC_C_OUT|DAC_D_OUT);
	//DAC7718_write_control1(DAC_A_OUT|DAC_B_OUT|DAC_C_OUT);
}

void DAC7718_OUT_VOLT(uint32_t sel, uint32_t data)
{
	uint16_t data_reg = (data << 4)&0x00FFF0;
	DAC7718_SPI_Init(SPI_DataSize_8b, SPI_CPOL_High, s905_SPI);

  s905_DAC_CS0_LOW();		
	DAC7718_SendTripleByte(sel|data_reg);		
	s905_DAC_CS0_HIGH();
}

void DAC7718_ZERO_SETUP(uint32_t sel, uint32_t data)
{
	uint16_t data_reg = (data << 4)&0x00FFF0;
	//DAC7718_SPI_Init(SPI_DataSize_16b, SPI_CPOL_Low, s905_SPI);
	DAC7718_SPI_Init(SPI_DataSize_8b, SPI_CPOL_High, s905_SPI);

  s905_DAC_CS0_LOW();		
	DAC7718_SendTripleByte(sel|data_reg);		
	s905_DAC_CS0_HIGH();
}

void DAC7718_GAIN_SETUP(uint32_t sel, uint32_t data)
{
	uint16_t data_reg = (data << 4)&0x00FFF0;
	//DAC7718_SPI_Init(SPI_DataSize_16b, SPI_CPOL_Low, s905_SPI);
	DAC7718_SPI_Init(SPI_DataSize_8b, SPI_CPOL_High, s905_SPI);

  s905_DAC_CS0_LOW();		
	DAC7718_SendTripleByte(sel|data_reg);		
	s905_DAC_CS0_HIGH();
}

void ALL_DAC7718_OUT_VOLT(uint32_t data)
{

	uint16_t data_reg = 0x00FFF0 & (data << 4);
	
	DAC7718_SPI_Init(SPI_DataSize_8b, SPI_CPOL_High, s905_SPI);
  s905_DAC_CS0_LOW();		
	DAC7718_SendTripleByte(BROADCAST_REGISTER|data_reg);		
	s905_DAC_CS0_HIGH();
}



void S660_DAC_OUT_VOLT(S660_DAC_TypeDef sel, uint16_t data)
{
	uint16_t data_reg = 0x0FFF & data;
	uint16_t dac_sel = sel;
	
	//DAC7718_SPI_Init(s660_SPI);
	DAC7718_SPI_Init(SPI_DataSize_16b, SPI_CPOL_Low, s660_SPI);
	if(dac_sel > 3)
    return;
	dac_sel = dac_sel << 14;
	dac_sel = dac_sel | 0x1000;    // Write and update
  s660_DAC_CS_LOW();		
	DAC7718_SendHalfWord(dac_sel|data_reg);		
	s660_DAC_CS_HIGH();	
}
