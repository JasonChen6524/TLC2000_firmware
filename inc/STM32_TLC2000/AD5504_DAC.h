/**
    ******************************************************************************
  * @file    AD5504_DAC.h
  * @author  Jason Chen
  * @version V1.0
  * @date    18-June-2014
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AD5504_DAC_H
#define __AD5504_DAC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm3210e_TLC2000.h"

/** @addtogroup define
  * @{
  */
#define       CTL_REG_WRITE      0x7000
#define       DAC_A_OUT          0x0004
#define       DAC_B_OUT          0x0008
#define       DAC_C_OUT          0x0010
#define       DAC_D_OUT          0x0020	 
#define       DAC_ALL_3_STATE    0x0040	 
	 
#define       DAC_OUT_NO_UPDATE  0x0000
#define       DAC_OUT_UPDATE     0x1000
#define       DAC_OUT_ALL_UPDATE 0x2000
#define       DAC_OUT_POWER_DW   0x3000	 
	 
	 
/*typedef*/ enum 
{
	s905_DAC0 = 0,
	s905_DAC1 = 1,
	s905_DAC2 = 2,	 
	s905_DAC3 = 3,	 
	s905_DAC4 = 4,	 
	s905_DAC5 = 5,
	TENS_DAC  = 6
} ;//S905_DAC_TypeDef;

typedef enum 
{
	s660_DAC0 = 0,
	s660_DAC1 = 1,
	s660_DAC2 = 2,	 
	s660_DAC3 = 3
} S660_DAC_TypeDef;

enum 
{
	s905_SPI  = SPI_CPHA_1Edge,
	s660_SPI  = SPI_CPHA_2Edge
};
	 
  
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
	 
#define s905_DUMMY_WORD_WR     0x0000
#define s905_DUMMY_WORD_RD     0x85A5
#define ADC_DUMMY_WORD_RD      0x5555

#define s905_DAC_CLR_LOW()  GPIO_ResetBits(s905_DAC_CLR_PORT, s905_DAC_CLR_PIN)
#define s905_DAC_CLR_HIGH() GPIO_SetBits(s905_DAC_CLR_PORT, s905_DAC_CLR_PIN)

#define s905_DAC_CS0_LOW()  GPIO_ResetBits(s905_DAC_CS0_PORT, s905_DAC_CS0_PIN)
#define s905_DAC_CS0_HIGH() GPIO_SetBits(s905_DAC_CS0_PORT, s905_DAC_CS0_PIN)   
#define s905_DAC_CS1_LOW()  GPIO_ResetBits(s905_DAC_CS1_PORT, s905_DAC_CS1_PIN)
#define s905_DAC_CS1_HIGH() GPIO_SetBits(s905_DAC_CS1_PORT, s905_DAC_CS1_PIN) 

#define s660_DAC_CS_LOW()   GPIO_ResetBits(s660_DAC_CS_PORT, s660_DAC_CS_PIN)
#define s660_DAC_CS_HIGH()  GPIO_SetBits(s660_DAC_CS_PORT, s660_DAC_CS_PIN)   

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
	
	
//void S905_DAC_SPI_DeInit(void);
void AD5504_SPI_GPIO_Init(void);
void S905_DAC_SPI_Init(uint16_t SPI_mode);
void S905_DAC_CLR(void);
void S905_DAC_ENABLE(void);
uint16_t S905_DAC_SendHalfWord(uint16_t HalfWord);
void S905_DAC_NORMAL(void);
void S905_DAC_write_control0(uint16_t data);
void S905_DAC_write_control1(uint16_t data);
//void S905_DAC_OUT_VOLT(S905_DAC_TypeDef sel, uint16_t data);
void S905_DAC_OUT_VOLT(/*S905_DAC_TypeDef*/uint16_t sel, uint16_t data);
void ALL_S905_DAC_OUT_VOLT(uint16_t data);
void S660_DAC_OUT_VOLT(S660_DAC_TypeDef sel, uint16_t data);
uint16_t s905_DAC_ReadHalfWord(uint16_t reg);



/**
  * @brief  Low layer functions
  */

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
