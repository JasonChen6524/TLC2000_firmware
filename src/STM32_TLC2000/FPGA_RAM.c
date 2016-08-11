/**
  ******************************************************************************
  * @file    FPGA_RAM.c
  * @author  Jason Chen
  * @version V1.0
  * @date    18-July-2014
  * @brief   
  *            
  */ 

/* Includes ------------------------------------------------------------------*/
#include "FPGA_RAM.h"
#include "hw_config.h"



/**
  * @brief  Initializes the FPGA_RAM
  * @param  None
  * @retval None
  */
void FPGA_RAM_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;	
	
  RCC_APB2PeriphClockCmd(FPGA_A0_GPIO_CLK|FPGA_A1_GPIO_CLK|FPGA_A2_GPIO_CLK|RCC_APB2Periph_AFIO/*|FPGA_A3_GPIO_CLK*/|
	                       FPGA_EN_GPIO_CLK|FPGA_RW_GPIO_CLK|FPGA_AD_RD_CLK_GPIO_CLK,ENABLE);	
  RCC_APB2PeriphClockCmd(FPGA_ST0_GPIO_CLK|FPGA_ST1_GPIO_CLK,ENABLE);	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	

	
  GPIO_InitStructure.GPIO_Pin   = FPGA_A0;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_Init(FPGA_A0_GPIO_PORT, &GPIO_InitStructure);	
  GPIO_InitStructure.GPIO_Pin   = FPGA_A1;
  GPIO_Init(FPGA_A1_GPIO_PORT, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin   = FPGA_A2;
  GPIO_Init(FPGA_A2_GPIO_PORT, &GPIO_InitStructure);
	
//GPIO_InitStructure.GPIO_Pin   = FPGA_A3;
//GPIO_Init(FPGA_A3_GPIO_PORT, &GPIO_InitStructure);
//GPIO_InitStructure.GPIO_Pin   = FPGA_A3;
//GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
//GPIO_Init(FPGA_A3_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = FPGA_EN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;			
  GPIO_Init(FPGA_EN_GPIO_PORT, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin   = FPGA_RW;
  GPIO_Init(FPGA_RW_GPIO_PORT, &GPIO_InitStructure);
		
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_Init(FPGA_D0_GPIO_PORT, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin   = FPGA_D1;
  GPIO_Init(FPGA_D1_GPIO_PORT, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin   = FPGA_D2;
  GPIO_Init(FPGA_D2_GPIO_PORT, &GPIO_InitStructure);	
  GPIO_InitStructure.GPIO_Pin   = FPGA_D3;
  GPIO_Init(FPGA_D3_GPIO_PORT, &GPIO_InitStructure);	
  GPIO_InitStructure.GPIO_Pin   = FPGA_D4;
  GPIO_Init(FPGA_D4_GPIO_PORT, &GPIO_InitStructure);	
  GPIO_InitStructure.GPIO_Pin   = FPGA_D5;
  GPIO_Init(FPGA_D5_GPIO_PORT, &GPIO_InitStructure);	
  GPIO_InitStructure.GPIO_Pin   = FPGA_D6;
  GPIO_Init(FPGA_D6_GPIO_PORT, &GPIO_InitStructure);	
  GPIO_InitStructure.GPIO_Pin   = FPGA_D7;
  GPIO_Init(FPGA_D7_GPIO_PORT, &GPIO_InitStructure);	
  GPIO_InitStructure.GPIO_Pin   = FPGA_D8;
  GPIO_Init(FPGA_D8_GPIO_PORT, &GPIO_InitStructure);	
  GPIO_InitStructure.GPIO_Pin   = FPGA_D9;
  GPIO_Init(FPGA_D9_GPIO_PORT, &GPIO_InitStructure);	
  GPIO_InitStructure.GPIO_Pin   = FPGA_D10;
  GPIO_Init(FPGA_D10_GPIO_PORT, &GPIO_InitStructure);	
  GPIO_InitStructure.GPIO_Pin   = FPGA_D11;
  GPIO_Init(FPGA_D11_GPIO_PORT, &GPIO_InitStructure);	
  GPIO_InitStructure.GPIO_Pin   = FPGA_D12;
  GPIO_Init(FPGA_D12_GPIO_PORT, &GPIO_InitStructure);	
  GPIO_InitStructure.GPIO_Pin   = FPGA_D13;
  GPIO_Init(FPGA_D13_GPIO_PORT, &GPIO_InitStructure);	
  GPIO_InitStructure.GPIO_Pin   = FPGA_D14;
  GPIO_Init(FPGA_D14_GPIO_PORT, &GPIO_InitStructure);	
  GPIO_InitStructure.GPIO_Pin   = FPGA_D15;
  GPIO_Init(FPGA_D15_GPIO_PORT, &GPIO_InitStructure);	
		
  //GPIO_InitStructure.GPIO_Pin   = FPGA_ST0 | FPGA_ST1;
  //GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
  //GPIO_Init(FPGA_ST0_GPIO_PORT, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin   = FPGA_ST0|FPGA_ST1|FPGA_ST2|FPGA_ST3_905|FPGA_ST4_660|FPGA_ST5_905|FPGA_ST6_660 ;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
  GPIO_Init(FPGA_ST0_GPIO_PORT, &GPIO_InitStructure);
	
	
	
  GPIO_InitStructure.GPIO_Pin   = FPGA_AD_RD_CLK;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
  GPIO_Init(FPGA_AD_RD_CLK_GPIO_PORT, &GPIO_InitStructure);	
	
	#if 1                            // ADC reading indicator, 2014.12.19
	/* Connect EXTI4 Line to PC.04 pin */
	GPIO_EXTILineConfig(FPGA_AD_RD_CLK_EXTI_PORT_SOURCE, FPGA_AD_RD_CLK_EXTI_PIN_SOURCE);

	/* Configure Button EXTI line */
	EXTI_InitStructure.EXTI_Line = FPGA_AD_RD_CLK_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;//EXTI_Trigger_Falling;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
				
	/* Clear the Tamper EXTI line pending bit */
	EXTI_ClearITPendingBit(FPGA_AD_RD_CLK_EXTI_LINE);		

	/* Enable and set Button EXTI Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = FPGA_AD_RD_CLK_EXTI_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure); 	
#endif

  FPGA_ENABLE();
	

	
}

uint32_t ST0_PBGetState(void)
{
  return GPIO_ReadInputDataBit(FPGA_ST0_GPIO_PORT, FPGA_ST0);
}

uint32_t ST1_PBGetState(void)                 // New Board, 2014.10.06
{
  return GPIO_ReadInputDataBit(FPGA_ST1_GPIO_PORT, FPGA_ST1);
}

uint32_t ST2_PBGetState(void)                 // R3 Board, 2014.12.05
{
  return GPIO_ReadInputDataBit(FPGA_ST2_GPIO_PORT, FPGA_ST2);
}

uint32_t ST3_905_PBGetState(void)                 // R3 Board, 2014.12.05
{
  return GPIO_ReadInputDataBit(FPGA_ST3_905_GPIO_PORT, FPGA_ST3_905);
}

uint32_t ST4_660_PBGetState(void)                 // R3 Board, 2014.12.05
{
  return GPIO_ReadInputDataBit(FPGA_ST4_660_GPIO_PORT, FPGA_ST4_660);
}

uint32_t ST5_905_PBGetState(void)                 // R3 Board, 2014.12.05
{
  return GPIO_ReadInputDataBit(FPGA_ST5_905_GPIO_PORT, FPGA_ST5_905);
}

uint32_t ST6_660_PBGetState(void)                 // R3 Board, 2014.12.05
{
  return GPIO_ReadInputDataBit(FPGA_ST6_660_GPIO_PORT, FPGA_ST6_660);
}


uint32_t AD_RD_CLK_PBGetState(void)                 // R3 Board, 2014.12.19
{
  return GPIO_ReadInputDataBit(FPGA_AD_RD_CLK_GPIO_PORT, FPGA_AD_RD_CLK);
}


//uint32_t A3_PBGetState(void)                 // New Board, 2014.10.21
//{
//  return GPIO_ReadInputDataBit(FPGA_A3_GPIO_PORT, FPGA_A3);
//}

void Reset_All_Registers_FPGA(void)           // New Board, 2014.10.06
{
	uint16_t FPGA_data = 0;
	
	GPIO_ResetBits(FPGA_RW_GPIO_PORT, FPGA_RW);
	
	 GPIO_ResetBits(FPGA_A2_GPIO_PORT, FPGA_A2);
	 GPIO_ResetBits(FPGA_A1_GPIO_PORT, FPGA_A1);
   GPIO_ResetBits(FPGA_A0_GPIO_PORT, FPGA_A0);
	
	GPIO_Write(GPIOD, FPGA_data);
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	GPIO_SetBits(FPGA_RW_GPIO_PORT, FPGA_RW);
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	GPIO_ResetBits(FPGA_RW_GPIO_PORT, FPGA_RW);
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
}

void Set_905_pulse(uint16_t pulse_width, uint16_t frequency)
{
	uint16_t FPGA_data = 0;
	
	GPIO_ResetBits(FPGA_RW_GPIO_PORT, FPGA_RW);
	
	 GPIO_ResetBits(FPGA_A2_GPIO_PORT, FPGA_A2);
	 GPIO_ResetBits(FPGA_A1_GPIO_PORT, FPGA_A1);
     GPIO_SetBits(FPGA_A0_GPIO_PORT, FPGA_A0);
	
	FPGA_data = pulse_width | frequency;
	
	GPIO_Write(GPIOD, FPGA_data);
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	
	GPIO_SetBits(FPGA_RW_GPIO_PORT, FPGA_RW);
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	GPIO_ResetBits(FPGA_RW_GPIO_PORT, FPGA_RW);
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
}

void Set_660_pulse(uint16_t pulse_width, uint16_t frequency)
{
	uint16_t FPGA_data = 0;
	
	GPIO_ResetBits(FPGA_RW_GPIO_PORT, FPGA_RW);
	
	 GPIO_ResetBits(FPGA_A2_GPIO_PORT, FPGA_A2);
	 GPIO_SetBits  (FPGA_A1_GPIO_PORT, FPGA_A1);
   GPIO_ResetBits(FPGA_A0_GPIO_PORT, FPGA_A0);
	
	FPGA_data = pulse_width | frequency;
	
	GPIO_Write(GPIOD, FPGA_data);
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	GPIO_SetBits(FPGA_RW_GPIO_PORT, FPGA_RW);
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	GPIO_ResetBits(FPGA_RW_GPIO_PORT, FPGA_RW);
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
}

void Set_905_Feedback_pulse(uint16_t DAC_sel)
{
	uint16_t FPGA_data = 0;
	
	GPIO_ResetBits(FPGA_RW_GPIO_PORT, FPGA_RW);
	
	 GPIO_ResetBits(FPGA_A2_GPIO_PORT, FPGA_A2);
	 GPIO_SetBits  (FPGA_A1_GPIO_PORT, FPGA_A1);
   GPIO_SetBits  (FPGA_A0_GPIO_PORT, FPGA_A0);
	
	FPGA_data = DAC_sel & 0x003F;
	
	GPIO_Write(GPIOD, FPGA_data);
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	GPIO_SetBits(FPGA_RW_GPIO_PORT, FPGA_RW);
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	GPIO_ResetBits(FPGA_RW_GPIO_PORT, FPGA_RW);
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
}

void Set_660_Feedback_pulse(uint16_t DAC_sel)
{
	uint16_t FPGA_data = 0;
	
	GPIO_ResetBits(FPGA_RW_GPIO_PORT, FPGA_RW);
	
	 GPIO_SetBits(FPGA_A2_GPIO_PORT, FPGA_A2);
	 GPIO_ResetBits  (FPGA_A1_GPIO_PORT, FPGA_A1);
   GPIO_ResetBits  (FPGA_A0_GPIO_PORT, FPGA_A0);
	
	FPGA_data = DAC_sel & 0x000F;
	
	GPIO_Write(GPIOD, FPGA_data);
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	GPIO_SetBits(FPGA_RW_GPIO_PORT, FPGA_RW);
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	GPIO_ResetBits(FPGA_RW_GPIO_PORT, FPGA_RW);
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
}


void Set_cadence_freq_duty(uint8_t select, uint16_t duty_cycle, uint16_t frequency)
{
	uint16_t FPGA_data = 0;
	
	GPIO_ResetBits(FPGA_RW_GPIO_PORT, FPGA_RW);
	
	 GPIO_SetBits(FPGA_A2_GPIO_PORT, FPGA_A2);
	 GPIO_ResetBits(FPGA_A1_GPIO_PORT, FPGA_A1);
   GPIO_SetBits(FPGA_A0_GPIO_PORT, FPGA_A0);
	
	FPGA_data = (duty_cycle<<8) | frequency;
	
	if(select == CADENCE_FREQ_905)
		FPGA_data &=0x7FFF;
	else
		FPGA_data |=0x8000;
	
	GPIO_Write(GPIOD, FPGA_data);
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	GPIO_SetBits(FPGA_RW_GPIO_PORT, FPGA_RW);
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	GPIO_ResetBits(FPGA_RW_GPIO_PORT, FPGA_RW);
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
}

void Set_TENS_Frequence(uint16_t frequency)        // 2014.10.21
{
	uint16_t FPGA_data = 0;
	
	GPIO_ResetBits(FPGA_RW_GPIO_PORT, FPGA_RW);
	
	 GPIO_SetBits(FPGA_A2_GPIO_PORT, FPGA_A2);
	 GPIO_SetBits(FPGA_A1_GPIO_PORT, FPGA_A1);
   GPIO_ResetBits(FPGA_A0_GPIO_PORT, FPGA_A0);
	
	FPGA_data = 0x0007 & frequency;
		
	GPIO_Write(GPIOD, FPGA_data);
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	GPIO_SetBits(FPGA_RW_GPIO_PORT, FPGA_RW);
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	GPIO_ResetBits(FPGA_RW_GPIO_PORT, FPGA_RW);
		__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
	__NOP();	__NOP();	__NOP();	__NOP();	__NOP();
}


