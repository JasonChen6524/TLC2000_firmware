/**
  ******************************************************************************
  * @file    platform_config.h
  * @author  Jason Chen
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Evaluation board specific configuration file.
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PLATFORM_CONFIG_H
#define __PLATFORM_CONFIG_H

/* Includes ------------------------------------------------------------------*/
 #include "stm32f10x.h"
 #include "stm3210e_TLC2000.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/*Unique Devices IDs register set*/

#define         ID1          (0x1FFFF7E8)
#define         ID2          (0x1FFFF7EC)
#define         ID3          (0x1FFFF7F0)


/* Define the STM32F10x hardware depending on the used evaluation board */

#define USB_DISCONNECT                      GPIOB  
#if 0
#define USB_DISCONNECT_PIN                  GPIO_Pin_14
#else
#define USB_DISCONNECT_PIN                  GPIO_Pin_1
#endif


//#define USB_DISCONNECT_PIN                  GPIO_Pin_1                           // TLC2000
#define RCC_APB2Periph_GPIO_DISCONNECT      RCC_APB2Periph_GPIOB
#define ADC1_DR_Address                     ((uint32_t)0x4001244C)

//#define RCC_APB2Periph_GPIO_IOAIN           RCC_APB2Periph_GPIOC
//#define GPIO_IOAIN                          GPIOC
//#define GPIO_IOAIN_PIN                      GPIO_Pin_4   /* PC.04 */
//#define ADC_AIN_CHANNEL                     ADC_Channel_14

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

#endif /* __PLATFORM_CONFIG_H */

/*****END OF FILE****/

