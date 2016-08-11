/**
    ******************************************************************************
  * @file    Buzzer.h
  * @author  Jason Chen
  * @version V1.0
  * @date    24-Nov-2014
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BUZZER_H
#define __BUZZER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

#define USE_TIM8_PWM                  1

#define BUZZER_GPIO_PIN               GPIO_Pin_6
#define BUZZER_GPIO_PORT              GPIOC
#define BUZZER_GPIO_CLK               RCC_APB2Periph_GPIOC

#define VOLUME_VOLT_6V   (4096/30*6)

#define VOLUME_VOLT_5V   680
#define VOLUME_VOLT_4V   546
#define VOLUME_VOLT_3V   (4096/30*3)

#define VOLUME_VOLT_MAX VOLUME_VOLT_6V
#define VOLUME_VOLT_DEFAULT (VOLUME_VOLT_6V/2)

#define BUZZER_DUTY_CYCLE  50   //10
#define BUZZER_FREQENCY   4000   //1200

//#define NOP()                          __NOP();
extern uint8_t TurnOnBuzzer;

void BUZZER_Init(void);
void Buzzer_Volume(uint16_t volt_control);
void PWM_Frequency(uint16_t frequency,uint16_t dutyCycle);
void BuzzerOn(void);
void BuzzerOff(void);
void BuzzerToggle(void);

#endif
