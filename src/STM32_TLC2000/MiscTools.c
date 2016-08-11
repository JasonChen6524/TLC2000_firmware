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
#include "MiscTools.h"

void LED_and_IO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(LED_GPIO_CLK /*| BUZZER_GPIO_CLK*/, ENABLE);
	RCC_APB2PeriphClockCmd(VLP_CTR_GPIO_CLK|VCC_CTR_GPIO_CLK, ENABLE);
	RCC_APB2PeriphClockCmd(VUSB_IN_D_GPIO_CLK, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = LED_GPIO_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = VLP_CTR_PIN|VCC_CTR_PIN;     // 2014.10.17
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	
  GPIO_Init(VLP_CTR_PORT, &GPIO_InitStructure);	
	
  GPIO_InitStructure.GPIO_Pin = VUSB_IN_D_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
  GPIO_Init(VUSB_IN_D_PORT, &GPIO_InitStructure);	
	
}


void Power_V30_On(void)
{
	VLP_CTR_PORT->BSRR = VLP_CTR_PIN;     
}

void Power_V30_Off(void)
{
	VLP_CTR_PORT->BRR = VLP_CTR_PIN;     
}

void Power_VIN660_On(void)                // 2014.10.17
{
	VCC_CTR_PORT->BSRR = VCC_CTR_PIN;     
}

void Power_VIN660_Off(void)                 // 2014.10.17
{
	VCC_CTR_PORT->BRR = VCC_CTR_PIN;     
}

void LED_On(void)
{
	LED_GPIO_PORT->BSRR = LED_GPIO_PIN;     
}

void LED_Off(void)
{
	LED_GPIO_PORT->BRR = LED_GPIO_PIN;    
}

void LED_Toggle(void)
{
	LED_GPIO_PORT->ODR ^= LED_GPIO_PIN;
}



uint32_t VUSB_IN_PBGetState(void)
{
  return GPIO_ReadInputDataBit(VUSB_IN_D_PORT, VUSB_IN_D_PIN);
}

