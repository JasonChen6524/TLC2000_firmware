/**
    ******************************************************************************
  * @file    Switch2.h
  * @author  Jason Chen
  * @version V1.0
  * @date    19-Oct-2014
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SWITCH2_H
#define __SWITCH2_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

enum
{
	SWITCH_INIT = 0,
	SWITCH_WAIT_PRESS,
	SWITCH_WAIT_PRESS2,
	SWITCH_DELAY,
	SWITCH_DELAY2,
	SWITCH_START,
	SWITCH_START2,
	SWITCH_RUNNING,
	SWITCH_S2DELAY,
	SWITCH_PAUSE,
	SWITCH_STOP,
	SWITCH_NOP
};

#if 0
#define SW_PIN                           GPIO_Pin_2                 /* PE2 */  // 2014.10.20
#define SW_GPIO_PORT                     GPIOE
#define SW_GPIO_CLK                      RCC_APB2Periph_GPIOE
#define SW_EXTI_LINE                     EXTI_Line2
#define SW_EXTI_PORT_SOURCE              GPIO_PortSourceGPIOE
#define SW_EXTI_PIN_SOURCE               GPIO_PinSource2
#define SW_EXTI_IRQn                     EXTI2_IRQn
#else
#define SW_PIN                           GPIO_Pin_2                 /* PE2 */  // 2014.10.20
#define SW_GPIO_PORT                     GPIOB                      // GPIOE
#define SW_GPIO_CLK                      RCC_APB2Periph_GPIOB       //RCC_APB2Periph_GPIOE
#define SW_EXTI_LINE                     EXTI_Line2
#define SW_EXTI_PORT_SOURCE              GPIO_PortSourceGPIOB       //GPIO_PortSourceGPIOE
#define SW_EXTI_PIN_SOURCE               GPIO_PinSource2
#define SW_EXTI_IRQn                     EXTI2_IRQn
#endif

//#define NOP()                          __NOP();

extern __IO uint32_t SleepTimer_TickCount;
//extern __IO uint8_t  button_press;
extern uint8_t switch_stm_status;
extern uint8_t switch_stm_selfTest;              // Added by Jason Chen, 2015.10.07
extern __IO uint32_t SWITCH2_Timer;
extern __IO uint32_t selfTest_cnt;               // Added by Jason Chen, 2015.10.07
extern __IO uint8_t  stopFlag;                   // Added by Jason Chen, 2015.10.07
extern __IO uint8_t Pause_flag;                  // Added by Jason Chen, 2015.04.09

void Switch2_Init(void);
uint32_t SW_Int_PBGetStateOn(void);         // 2014.10.20
uint8_t GetButtonValue_click(void);
void Switch_process(void);
void Switch_process_SelTest(void);          // Added by Jason Chen, 2015.10.07
void Process_TimeSet(uint32_t *pTimer,uint32_t time);
uint8_t Process_TimeExpired(uint32_t *pTimer);
#endif
