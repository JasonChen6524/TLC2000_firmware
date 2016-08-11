/**
    ******************************************************************************
  * @file    TLC2000_spi_LCD_TFT.c
  * @author  Jason Chen
  * @version V1.0
  * @date    18-Aug-2014
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TLC2000_SPI_H_TFT
#define __TLC2000_SPI_H_TFT

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm3210e_TLC2000.h"
#include "fonts.h"

enum
{
	COLOR_BLACK1       = 0x0000,
	COLOR_WHITE1       = 0xFFFF,
	COLOR_BLUE1        = 0x001F,
  COLOR_LIME1   		 = 0x07E0,
	COLOR_RED1         = 0xF800,
	COLOR_DEEPRED1     = 0xF000,
	COLOR_YELLOW1      = 0xFFE0,
	COLOR_LIGHTBLUE1   = 0x7FFF,
	COLOR_1PURPLE1     = 0xF81E,
	COLOR_DARKGREEN1   = 0x0560,	
	COLOR_MINE1        = 0xFBDF,//18,//((((255&248)|120>>5))<<8)|((120&28)<<3|255>>3),
  COLOR_GREEN1	     = 0x0400,
  COLOR_SILVER1      = 0xC618,
  COLOR_GRAY1        = 0x8410,
  COLOR_MAROON1      = 0x8000,
  COLOR_OLIVE1       = 0x8400,
  COLOR_AQUA1        = 0x07FF,
  COLOR_TEAL1        = 0x06F5, //0x0410,    Changed by Jason, 2015.06.04
  COLOR_NAVY1        = 0x0010,
  COLOR_FUCHSIA1     = 0xF81F,
  COLOR_2PURPLE1     = 0x8010,
	COLOR_THERALASE1   = 0x0473,
	COLOR_ORANGE1      = 0xFC60
};

enum
{
	COLOR_BLACK,
	COLOR_WHITE,
	COLOR_BLUE,
	COLOR_LIME,
	COLOR_RED,
	COLOR_DEEPRED,
	COLOR_YELLOW,
	COLOR_LIGHTBLUE,
	COLOR_PURPLE,
	COLOR_DARKGREEN,
	COLOR_MINE,
  COLOR_GREEN,
  COLOR_SILVER,
  COLOR_GRAY,
  COLOR_MAROON,
  COLOR_OLIVE,
  COLOR_AQUA,
  COLOR_TEAL,
  COLOR_NAVY,
  COLOR_FUCHSIA,
  COLOR_2PURPLE	,
	COLOR_THERALASE,
	COLOR_ORANGE
};

enum                               // Added by Jason Chen, 2015.04.09
{
	PROMPT_WAIT  = 0,
	PROMPT_START = 1,
	PROMPT_PAUSE = 2,
	PROMPT_CALI  = 3,	
	PROMPT_EMPTY = 4
};

#define BT_LOCATION  (12*13-1)
	 
#define sLCD_DUMMY_BYTE_TFT     0xA5
#define sLCD_CS_LOW_TFT()       GPIO_ResetBits(sLCD_SPI_CS_GPIO_PORT, sLCD_SPI_CS_PIN)
#define sLCD_CS_HIGH_TFT()      GPIO_SetBits(sLCD_SPI_CS_GPIO_PORT, sLCD_SPI_CS_PIN)   

#define sLCD_DC_LOW_TFT()       GPIO_ResetBits(sLCD_SPI_DC_GPIO_PORT, sLCD_SPI_DC_PIN)
#define sLCD_DC_HIGH_TFT()      GPIO_SetBits(sLCD_SPI_DC_GPIO_PORT, sLCD_SPI_DC_PIN)   

#define sLCD_EN_VP_LOW_TFT()    GPIO_ResetBits(sLCD_SPI_EN_VP_GPIO_PORT, sLCD_SPI_EN_VP_PIN)
#define sLCD_EN_VP_HIGH_TFT()   GPIO_SetBits(sLCD_SPI_EN_VP_GPIO_PORT, sLCD_SPI_EN_VP_PIN)   

#define sLCD_RST_LOW_TFT()      GPIO_ResetBits(sLCD_SPI_RST_GPIO_PORT, sLCD_SPI_RST_PIN)
#define sLCD_RST_HIGH_TFT()     GPIO_SetBits(sLCD_SPI_RST_GPIO_PORT, sLCD_SPI_RST_PIN)   

extern void Delay(__IO uint32_t nCount);
extern uint16_t background_color;
extern uint16_t foreground_color;
extern uint16_t probe_color;               // Added by Jason, 2015_0627
extern uint16_t ColorArray[23];
extern int16_t RSSI;

extern uint8_t armed_flag;
/**
  * @brief  High layer functions
  */
void sLCD_SPI_DeInit_TFT(void);
void sLCD_SPI_Init_TFT(void);
void sLCD_init_TFT(void);
void sLCD_background_TFT(uint16_t back_color,uint16_t fore_color);
void sLCD_putChar_TFT(uint16_t x, uint16_t y, uint8_t TestChar, sFONT font);
void sLCD_Display_Home_TFT(void);

void sLCD_putString_TFT(uint16_t x, uint16_t y, uint8_t * Str, sFONT font);
void sLCD_putString_TFT_D(uint16_t x, uint16_t y, uint8_t * Str, sFONT font, uint32_t delay);
void display_off(void);
void display_on(void);

void sLCD_Display_RGB_TFT(void);
void Display_BT_RSSI(int16_t rssi);// 2014.10.17
void Display_BT_Bar(uint16_t x, uint16_t y, uint16_t bar_width);

void Display_Armed(void);
void Display_Idle(void);
void Display_SlefTestMode(void);                       // Added by Jason Chen, 2015.10.07

void Display_Treat(void);
void Display_Active(void);
void Display_Measure(void);
void Display_Halted(void);

void Display_LaserOn(void);
void Display_LaserOff(void);
void Display_Power(uint16_t powerValue);
void DisplayPowermW(float powerValuemW);
void DisplayNoPowermW(void);

void DisplayTestOffset(uint16_t row, int16_t value);                                   //2015.09.19

void Display_TensPower(uint16_t powerValue);
void Display_TENsOn(void);
void Display_TENsOff(void);

void Display_Prompt(uint8_t prompt);      // aded by Jason Chen, 2015.04.09
void Display_2s_count(uint32_t count);    // Added by Jason Chen, 2015.04.09
void Display_Pause(void);                  //Added by Jason Chen, 2015.04.09

void Display_PowerByte(uint8_t powerValue);
void Display_TensPowerByte(uint8_t powerValue);

void Display_workingMode(uint8_t mode);
void Display_square(uint16_t x, uint16_t y, uint16_t fore_color, sFONT font);

void sLCD_putChar_TFT_ext(uint16_t x, uint16_t y, uint8_t TestChar, sFONT_ext font);
void sLCD_putString_TFT_ext(uint16_t x, uint16_t y, uint8_t * Str, sFONT_ext font);

void sLCD_Write_Command_TFT(uint8_t command);
void sLCD_Write_Data_TFT(uint8_t data);

void Display_ProgressBar(uint16_t x, uint16_t y, uint16_t bar_width);

void Display_SlefCaliMode(void);                       // Added by Jason Chen, 2016.08.05
void DisplayOneNumber(uint32_t oneNumber);             // Added by Jason Chen, 2016.08.05

/**
  * @brief  Low layer functions
  */
//uint8_t  sLCD_ReadByte(void);
/*uint8_t*/void  sLCD_SendByte_TFT(uint8_t byte);
uint16_t sLCD_SendHalfWord_TFT(uint16_t HalfWord);
void DisplaySelfCali(uint16_t row, float value, uint8_t flag);
void Self_Cali_Clr(void);
void Display905Voltage(uint16_t row, uint16_t DAC905_out);
void DisplayOffsetVoltage(uint16_t row, int16_t offsetV);

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
