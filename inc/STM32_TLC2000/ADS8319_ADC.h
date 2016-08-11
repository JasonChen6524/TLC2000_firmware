/**
    ******************************************************************************
  * @file    Potentiometer.h
  * @author  Jason Chen
  * @version V1.0
  * @date    18-July-2014
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ADS8319_ADC_H
#define __ADS8319_ADC_H

/* Includes ------------------------------------------------------------------*/
//#include "stm3210e_TLC2000.h"
#include "stm32f10x.h"
//#include "stm32_eval_legacy.h"

//enum
//{
//
//};

enum 
{  
  CONV_MODE_GPIO = 0,
  CONV_MODE_EXTI = 1
};

extern uint16_t ADC_Value;
extern __IO uint32_t conert_end_flag;

#define SEN_ADC_CS                    GPIO_Pin_10
#define SEN_ADC_CS_GPIO_PORT          GPIOC
#define SEN_ADC_CS_GPIO_CLK           RCC_APB2Periph_GPIOC
#define CONVST                        GPIO_Pin_11
#define CONVST_GPIO_PORT              GPIOC
#define CONVST_GPIO_CLK               RCC_APB2Periph_GPIOC

#define CONV_END_PIN                  GPIO_Pin_12
#define CONV_END_GPIO_PORT            GPIOB
#define CONV_END_GPIO_CLK             RCC_APB2Periph_GPIOB
#define CONV_END_EXTI_LINE            EXTI_Line12
#define CONV_END_EXTI_PORT_SOURCE     GPIO_PortSourceGPIOB
#define CONV_END_EXTI_PIN_SOURCE      GPIO_PinSource12
#define CONV_END_EXTI_IRQn            EXTI15_10_IRQn 

#define ADC_SPI_SCK_PIN               GPIO_Pin_13                 /* PB.13 */
#define ADC_SPI_SCK_GPIO_PORT         GPIOB                       /* GPIOB */
#define ADC_SPI_SCK_GPIO_CLK          RCC_APB2Periph_GPIOB

#define SEN_DAC_CS_LOW()              GPIO_ResetBits(SEN_ADC_CS_GPIO_PORT, SEN_ADC_CS)
#define SEN_DAC_CS_HIGH()             GPIO_SetBits(SEN_ADC_CS_GPIO_PORT, SEN_ADC_CS)

#define CONVST_LOW()                  GPIO_ResetBits(CONVST_GPIO_PORT, CONVST)
#define CONVST_HIGH()                 GPIO_SetBits(CONVST_GPIO_PORT, CONVST)

#define ADC_SPI_SCK_LOW()             GPIO_ResetBits(ADC_SPI_SCK_GPIO_PORT, ADC_SPI_SCK_PIN)
#define ADC_SPI_SCK_HIGH()            GPIO_SetBits(ADC_SPI_SCK_GPIO_PORT, ADC_SPI_SCK_PIN)



//#define NOP()                          __NOP();
extern uint16_t adc_rd;
extern __IO uint32_t adc_reading_interval;
extern __IO uint32_t AMP_hold_flag;                            // Added by Jason Chen, 2014.12.19

void ADS8319_Init(void);
void Start_ADC_Convert(void);
void Stop_ADC_Convert(void);
uint32_t ADC_END_PBGetState(void);
//uint32_t ADC_END_GPIO_PBGetState(void);
void ADC_END_GPIO_INTERRUPT_Config(uint8_t conv_mode);
int32_t ADC_ReadValue(void);
void ADC_Value_Display_normal(void);
void ADC_Value_Display_one_time(void);

#endif
