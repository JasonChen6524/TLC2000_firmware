/**
    ******************************************************************************
  * @file    DAC7818S.h
  * @author  Jason Chen
  * @version V1.0
  * @date    06-OCT-2014
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DAC7718_H
#define __DAC7718_H

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
	 
enum 
{
	DAC7718_WRITE = 0x000,
	DAC7718_READ  = 0x800
};

enum
{
	CONFIGURATION_REGISTER    = 0x000000,
	MONITOR_REGISTER          = 0x010000,
  GPIO_REGISTER             = 0x020000,
  OFFSETA_DAC_DATA_REGISTER	= 0x030000,
	OFFSETB_DAC_DATA_REGISTER	= 0x040000,
	SPI_MODE_REGISTER         = 0x060000,
	BROADCAST_REGISTER        = 0x070000,
	DAC_0_DATA_REGISTER       = 0x080000,
	DAC_1_DATA_REGISTER       = 0x090000,
	DAC_2_DATA_REGISTER       = 0x0A0000,
	DAC_3_DATA_REGISTER       = 0x0B0000,
	DAC_4_DATA_REGISTER       = 0x0C0000,
	DAC_5_DATA_REGISTER       = 0x0D0000,
	DAC_6_DATA_REGISTER       = 0x0E0000,
	DAC_7_DATA_REGISTER       = 0x0F0000,
	ZERO_0_REGISTER           = 0x100000,
	GAIN_0_REGISTER           = 0x110000,
	ZERO_1_REGISTER           = 0x120000,
	GAIN_1_REGISTER           = 0x130000,
	ZERO_2_REGISTER           = 0x140000,
	GAIN_2_REGISTER           = 0x150000,
	ZERO_3_REGISTER           = 0x160000,
	GAIN_3_REGISTER           = 0x170000,
	ZERO_4_REGISTER           = 0x180000,
	GAIN_4_REGISTER           = 0x190000,
	ZERO_5_REGISTER           = 0x1A0000,
	GAIN_5_REGISTER           = 0x1B0000,
	ZERO_6_REGISTER           = 0x1C0000,
	GAIN_6_REGISTER           = 0x1D0000,
	ZERO_7_REGISTER           = 0x1E0000,
	GAIN_7_REGISTER           = 0x1F0000
};

/*typedef*/ enum 
{
	s905_ALL_DACs = BROADCAST_REGISTER,
	s905_DAC0     = DAC_0_DATA_REGISTER,
	s905_DAC1     = DAC_1_DATA_REGISTER,
	s905_DAC2     = DAC_2_DATA_REGISTER,
	s905_DAC3     = DAC_3_DATA_REGISTER,
	s905_DAC4     = DAC_4_DATA_REGISTER,
	s905_DAC5     = DAC_5_DATA_REGISTER,
	TENS_DAC      = DAC_6_DATA_REGISTER,
	BUZZER_VOLUME = DAC_7_DATA_REGISTER
} ;//S905_DAC_TypeDef;

enum
{
	DAC_DATA_REG   = 0x8000,
	SOFTWARE_RST   = 0x2000,
	POWER_DOWN_A   = 0x1000,
	POWER_DOWN_B   = 0x0800,
	SYS_CAL_ENABLE = 0x0400,  //SCE
	GAIN_A_4       = 0x0100,  // 0-->6, 1-->4
	GAIN_B_4       = 0x0080,
	DISABLE_SDO    = 0x0040,
	NOP            = 0x0020,
	WAKE_UP        = 0x0010
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
//#define s905_DAC_CS1_LOW()  GPIO_ResetBits(s905_DAC_CS1_PORT, s905_DAC_CS1_PIN)
//#define s905_DAC_CS1_HIGH() GPIO_SetBits(s905_DAC_CS1_PORT, s905_DAC_CS1_PIN) 
#define s905_DAC_RST_LOW()  GPIO_ResetBits(s905_DAC_RST_PORT, s905_DAC_RST_PIN)
#define s905_DAC_RST_HIGH() GPIO_SetBits(s905_DAC_RST_PORT, s905_DAC_RST_PIN) 

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
void DAC7718_SPI_GPIO_Init(void);
//void DAC7718_SPI_Init(uint16_t SPI_mode);
void DAC7718_SPI_Init(uint16_t SPI_DATASIZE, uint16_t SPI_CPOL, uint16_t SPI_CPHA);
void DAC7718_CLR(void);
void DAC7718_ENABLE(void);
uint16_t DAC7718_SendHalfWord(uint16_t HalfWord);
void DAC7718_NORMAL(void);
//void DAC7718_write_control0(uint16_t data);
//void DAC7718_write_control1(uint16_t data);
//void S905_DAC_OUT_VOLT(S905_DAC_TypeDef sel, uint16_t data);
void DAC7718_OUT_VOLT(/*S905_DAC_TypeDef*/uint32_t sel, uint32_t data);
void ALL_DAC7718_OUT_VOLT(uint32_t data);

void S660_DAC_OUT_VOLT(S660_DAC_TypeDef sel, uint16_t data);
void DAC7718_ZERO_SETUP(uint32_t sel, uint32_t data);
void DAC7718_GAIN_SETUP(uint32_t sel, uint32_t data);
uint32_t DAC7718_Read_RegisterVaule(uint32_t registerAddr);
	



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
