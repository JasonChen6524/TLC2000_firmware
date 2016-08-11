/**
  ******************************************************************************
  * @file    MiscTools.c
  * @author  Jason Chen
  * @version V1.0
  * @date    18-July-2014
  * @brief   
  *            
  */ 

/* Includes ------------------------------------------------------------------*/
#include "Buzzer.h"
#include "DAC7718.h"
#include "calibration.h"

uint8_t TurnOnBuzzer = 0;

TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
uint16_t TimerPeriod = 0;
uint16_t Channel1Pulse = 0;




void BUZZER_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
#if USE_TIM8_PWM
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8|RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = BUZZER_GPIO_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(BUZZER_GPIO_PORT, &GPIO_InitStructure);		
#else
	RCC_APB2PeriphClockCmd(BUZZER_GPIO_CLK, ENABLE);
		
  GPIO_InitStructure.GPIO_Pin = BUZZER_GPIO_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	
  GPIO_Init(BUZZER_GPIO_PORT, &GPIO_InitStructure);	
#endif	
	
	PWM_Frequency(BUZZER_FREQENCY,BUZZER_DUTY_CYCLE);
	Buzzer_Volume(0);//VOLUME_VOLT_4V);
	BuzzerOff();
	
}

void Buzzer_Volume(uint16_t volt_control)
{
    if(volt_control > VOLUME_VOLT_MAX)
		volt_control = VOLUME_VOLT_MAX;
	DAC7718_OUT_VOLT(BUZZER_VOLUME, volt_control);
}

void PWM_Frequency(uint16_t frequency,uint16_t dutyCycle)
{
#if USE_TIM8_PWM	
	  /* Compute the value to be set in ARR regiter to generate signal frequency at 17.57 Khz */
  //TimerPeriod = (SystemCoreClock / 17570 ) - 1;
	if(frequency < 1099)
		frequency = 1099;
	if(dutyCycle > 100)
		dutyCycle = 100;
	TimerPeriod = (SystemCoreClock / frequency ) - 1;
  /* Compute CCR3 value to generate a duty cycle at 25%  for channel 3 and 3N */
  //Channel1Pulse = (uint16_t) (((uint32_t) 25 * (TimerPeriod - 1)) / 100);
	Channel1Pulse = (uint16_t) (((uint32_t) dutyCycle * (TimerPeriod - 1)) / 100);
	
/* Time Base configuration */
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

  TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

  /* Channel 1 Configuration in PWM mode */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
//TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
  TIM_OCInitStructure.TIM_Pulse = Channel1Pulse;
  TIM_OCInitStructure.TIM_OCPolarity =   TIM_OCPolarity_Low;
//TIM_OCInitStructure.TIM_OCNPolarity =  TIM_OCNPolarity_High;
  TIM_OCInitStructure.TIM_OCIdleState =  TIM_OCIdleState_Reset;//TIM_OCIdleState_Set;
//TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

  TIM_OC1Init(TIM8, &TIM_OCInitStructure);
	
  /* TIM8 counter enable */
  //TIM_Cmd(TIM8, ENABLE);

  /* TIM8 Main Output Enable */
  //TIM_CtrlPWMOutputs(TIM8, ENABLE);
#endif	
}

void BuzzerOn(void)
{
	//BUZZER_GPIO_PORT->BSRR = BUZZER_GPIO_PIN;     
#if USE_TIM8_PWM
  
  //Buzzer_Volume(VOLUME_VOLT_4V);
  Buzzer_Volume(userProfile.user_profile.buzzerVol);
  TIM_Cmd(TIM8, ENABLE);
  TIM_CtrlPWMOutputs(TIM8, ENABLE);
	TurnOnBuzzer = 0;
#else	
	TurnOnBuzzer = 1;
#endif	
}

void BuzzerOff(void)
{
#if USE_TIM8_PWM
	Buzzer_Volume(0);
	TIM_CtrlPWMOutputs(TIM8, DISABLE);
  TIM_Cmd(TIM8, DISABLE);
  TurnOnBuzzer = 1;  
#else		
	TurnOnBuzzer = 0;
	BUZZER_GPIO_PORT->BRR = BUZZER_GPIO_PIN;    
#endif	
}

void BuzzerToggle(void)
{
	BUZZER_GPIO_PORT->ODR ^= BUZZER_GPIO_PIN;
}


