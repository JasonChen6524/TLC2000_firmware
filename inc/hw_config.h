/**
  ******************************************************************************
  * @file    hw_config.h
  * @author  Jason Chen
  * @date    21-January-2013
  * @brief   Hardware Configuration & Setup
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

/* Includes ------------------------------------------------------------------*/
#include "platform_config.h"
#include "usb_type.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Set_System(void);
void Set_USBClock(void);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void USB_Interrupts_Config(void);
void Custom_Interrupts_Config(void);
void USB_Cable_Config (FunctionalState NewState);
void GPIO_Configuration(void);
void EXTI_Configuration(void);
//void ADC_Configuration(void);
//void ADC30x_Configuration(void);
void Get_SerialNum(void);
//void TimingDelay_Decrement(void);
void Delay(__IO uint32_t nCount);
void MyDelay(__IO uint32_t nCount);
void system_Init(void);

#endif  /*__HW_CONFIG_H*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
