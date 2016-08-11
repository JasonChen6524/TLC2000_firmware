/**
    ******************************************************************************
  * @file    MiscTools.h
  * @author  Jason Chen
  * @version V1.0
  * @date    18-July-2014
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MISCTOOLS_H
#define __MISTOOLS_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

//enum
//{
//
//};

#define LED_GPIO_PIN                  GPIO_Pin_8
#define LED_GPIO_PORT                 GPIOA
#define LED_GPIO_CLK                  RCC_APB2Periph_GPIOA

#define VLP_CTR_PIN                    GPIO_Pin_8                  /* PE.08 */
#define VLP_CTR_PORT                   GPIOE
#define VLP_CTR_GPIO_CLK               RCC_APB2Periph_GPIOE

#define VCC_CTR_PIN                    GPIO_Pin_9                  /* PE.09 */
#define VCC_CTR_PORT                   GPIOE
#define VCC_CTR_GPIO_CLK               RCC_APB2Periph_GPIOE

#define VUSB_IN_D_PIN                  GPIO_Pin_0                  /* PB.00 */
#define VUSB_IN_D_PORT                 GPIOB
#define VUSB_IN_D_GPIO_CLK             RCC_APB2Periph_GPIOB

#define VUSB_IN_D_PIN                  GPIO_Pin_0                  /* PB.00 */
#define VUSB_IN_D_PORT                 GPIOB
#define VUSB_IN_D_GPIO_CLK             RCC_APB2Periph_GPIOB

//#define NOP()                          __NOP();

void LED_and_IO_Init(void);
void LED_On(void);
void LED_Off(void);
void LED_Toggle(void);
void Power_V30_On(void);
void Power_V30_Off(void);
void Power_VIN660_On(void);                // 2014.10.17
void Power_VIN660_Off(void);                 // 2014.10.17
uint32_t VUSB_IN_PBGetState(void);

#endif
