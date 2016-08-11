/**
    ******************************************************************************
  * @file    TLC2000_spi_LCD.c
  * @author  Jason Chen
  * @version V1.0
  * @date    16-May-2014
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TLC2000_SPI_H
#define __TLC2000_SPI_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm3210e_TLC2000.h"

/** @addtogroup Utilities
  * @{
  */
  
/** @addtogroup STM32_EVAL
  * @{
  */ 

/** @addtogroup STM3210E_EVAL
  * @{
  */
  
/** @addtogroup STM3210E_EVAL_SPI_FLASH
  * @{
  */  

/** @defgroup STM3210E_EVAL_SPI_FLASH_Exported_Types
  * @{
  */ 
/**
  * @}
  */
	 
#define sLCD_DUMMY_BYTE         0xA5
#define sLCD_CS_LOW()       GPIO_ResetBits(sLCD_SPI_CS_GPIO_PORT, sLCD_SPI_CS_PIN)
#define sLCD_CS_HIGH()      GPIO_SetBits(sLCD_SPI_CS_GPIO_PORT, sLCD_SPI_CS_PIN)   

#define sLCD_DC_LOW()       GPIO_ResetBits(sLCD_SPI_DC_GPIO_PORT, sLCD_SPI_DC_PIN)
#define sLCD_DC_HIGH()      GPIO_SetBits(sLCD_SPI_DC_GPIO_PORT, sLCD_SPI_DC_PIN)   

#define sLCD_EN_VP_LOW()    GPIO_ResetBits(sLCD_SPI_EN_VP_GPIO_PORT, sLCD_SPI_EN_VP_PIN)
#define sLCD_EN_VP_HIGH()   GPIO_SetBits(sLCD_SPI_EN_VP_GPIO_PORT, sLCD_SPI_EN_VP_PIN)   

#define sLCD_RST_LOW()      GPIO_ResetBits(sLCD_SPI_RST_GPIO_PORT, sLCD_SPI_RST_PIN)
#define sLCD_RST_HIGH()     GPIO_SetBits(sLCD_SPI_RST_GPIO_PORT, sLCD_SPI_RST_PIN)   

extern void Delay(__IO uint32_t nCount);
/**
  * @brief  High layer functions
  */
void sLCD_SPI_DeInit(void);
void sLCD_SPI_Init(void);
void sLCD_init(void);
void sLCD_Display_RGB(void);
void sLCD_background(uint8_t color);
void sLCD_putChar(uint8_t x, uint8_t y, uint8_t TestChar);
void sLCD_putString(uint8_t x, uint8_t y, uint8_t* Str);
void SetDisplayLocation(uint8_t x, uint8_t y);
void SetDisplayLocationX(uint8_t x);
void SetDisplayLocationY(uint8_t y);
void sLCD_pChar(uint8_t TestChar);
void SetVerticalScroll(uint8_t scr);
void ClearLastLine(uint8_t row);

/**
  * @brief  Low layer functions
  */
//uint8_t  sLCD_ReadByte(void);
/*uint8_t*/void  sLCD_SendByte(uint8_t byte);
uint16_t sLCD_SendHalfWord(uint16_t HalfWord);

#ifdef __cplusplus
}
#endif

#endif /* __STM3210E_EVAL_SPI_FLASH_H */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */ 

/**
  * @}
  */  

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
