/**
  ******************************************************************************
  * @file    ADS8319_ADC.c
  * @author  Jason Chen
  * @version V1.0
  * @date    18-July-2014
  * @brief   
  *            
  */ 

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "TLC2000_SPI_LCD_TFT.h"
#include "ADS8319_ADC.h"
//#include "AD5504_DAC.h"
#include "DAC7718.h"
#include "MiscTools.h"
#include "fonts.h"
#include "calibration.h"                                              // Added by Jason Chen, 2014.12.19


uint16_t ADC_Value = 0;
__IO uint32_t conert_end_flag = 0;
__IO uint32_t ADC_Dispay = 0;
__IO uint32_t AMP_hold_flag = 0;                            // Added by Jason Chen, 2014.12.19
__IO uint32_t adc_reading_interval = 0;//1000;
extern uint8_t displayBuff[];
uint16_t adc_rd = 0;

/**
  * @brief  Initializes ADC
  * @param  None
  * @retval None
  */
	
	
void ADC_END_GPIO_INTERRUPT_Config(uint8_t conv_mode)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the BUTTON Clock */
    RCC_APB2PeriphClockCmd(CONV_END_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);

	  RCC_APB2PeriphClockCmd(SEN_ADC_CS_GPIO_CLK | CONVST_GPIO_CLK, ENABLE);
	
    GPIO_InitStructure.GPIO_Pin = SEN_ADC_CS;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(SEN_ADC_CS_GPIO_PORT, &GPIO_InitStructure);
	
    GPIO_InitStructure.GPIO_Pin = CONVST;
    GPIO_Init(CONVST_GPIO_PORT, &GPIO_InitStructure);
		
    /* Configure Button pin as input floating */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = CONV_END_PIN;
    GPIO_Init(CONV_END_GPIO_PORT, &GPIO_InitStructure);

	  if(conv_mode == CONV_MODE_EXTI)
		{
				/* Connect Button EXTI Line to Button GPIO Pin */
				GPIO_EXTILineConfig(CONV_END_EXTI_PORT_SOURCE, CONV_END_EXTI_PIN_SOURCE);

				/* Configure Button EXTI line */
				EXTI_InitStructure.EXTI_Line = CONV_END_EXTI_LINE;
				EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
				EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
				EXTI_InitStructure.EXTI_LineCmd = ENABLE;
				EXTI_Init(&EXTI_InitStructure);
				
				/* Clear the Tamper EXTI line pending bit */
				EXTI_ClearITPendingBit(CONV_END_EXTI_LINE);		

				/* Enable and set Button EXTI Interrupt to the lowest priority */
				NVIC_InitStructure.NVIC_IRQChannel = CONV_END_EXTI_IRQn;
				NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
				NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
				NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

				NVIC_Init(&NVIC_InitStructure); 
		}
}

uint32_t ADC_END_PBGetState(void)
{
  return GPIO_ReadInputDataBit(CONV_END_GPIO_PORT, CONV_END_PIN);
}

//uint32_t ADC_END_GPIO_PBGetState(void)
//{
 // return GPIO_ReadInputDataBit(SEN_ADC_CS_GPIO_PORT, SEN_ADC_CS);
//}

#define ENABLE_ADC     1
void ADS8319_Init(void)
{
	#if ENABLE_ADC
  ADC_END_GPIO_INTERRUPT_Config(CONV_MODE_GPIO);
	CONVST_LOW();                                   // Stop ADC
	SEN_DAC_CS_HIGH();
  #endif	
}

void Start_ADC_Convert(void)
{
	 CONVST_HIGH();      // Start ADC convert
}

void Stop_ADC_Convert(void)
{
	 CONVST_LOW();      // Stop ADC convert
}

int32_t ADC_ReadValue(void)
{
	#if ENABLE_ADC
	GPIO_InitTypeDef GPIO_InitStructure;
	uint16_t ADC_Value_temp;
	uint16_t count = 15;
	
	SPI_Cmd(s905_DAC_SPI, DISABLE);
	
	GPIO_InitStructure.GPIO_Pin = ADC_SPI_SCK_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//GPIO_Mode_AF_PP;
  GPIO_Init(s905_DAC_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);
  ADC_SPI_SCK_HIGH();
	
	CONVST_HIGH();      // Start ADC convert
	__NOP();__NOP();
	
	SEN_DAC_CS_LOW();
	while(ADC_END_PBGetState())
	{
		if((count--) == 0) 
		{
	     SEN_DAC_CS_HIGH();
	    __NOP();
	    CONVST_LOW();          
	   __NOP();
			
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;                   // 2014.12.17, bug fixed 
      GPIO_Init(s905_DAC_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);       // 2014.12.17, bug fixed
			DAC7718_SPI_Init(SPI_DataSize_16b, SPI_CPOL_Low, s660_SPI);       // 2014.12.17, bug fixed
			return -1;
		}
#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
          /* Reload IWDG counter */
    IWDG_ReloadCounter();                      // 2014.12.03
#endif  
	}
	ADC_SPI_SCK_LOW();
	__NOP();
	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(s905_DAC_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);
	
  //DAC7718_SPI_Init(s660_SPI);
	DAC7718_SPI_Init(SPI_DataSize_16b, SPI_CPOL_Low, s660_SPI);
	ADC_Value_temp = DAC7718_SendHalfWord(ADC_DUMMY_WORD_RD);
	SEN_DAC_CS_HIGH();
	CONVST_LOW();           //Stop ADC
	__NOP();	
	return ADC_Value_temp;
	#else
	 return -1;
	#endif
}

__IO uint32_t led_on_count =0;
__IO uint32_t adc_period =0;
void ADC_Value_Display_normal(void)
{
#if ENABLE_ADC
  float temp_f;

  //if(ADC_Dispay > adc_reading_interval)
	
	if(adc_reading_interval)	          //Changed by Jason Chen, 2014.12.19
	{		
		//ADC_Dispay = 0;
		if(AMP_hold_flag)
		{	
			led_on_count++;
			if(led_on_count > 7)
			{
				led_on_count = 0;
				LED_Toggle();
			}
			
      AMP_hold_flag = 0;		
#if 1		
			
			adc_rd  = ADC_ReadValue();			
			
			//adc_rd += ADC_ReadValue();
			//adc_rd += ADC_ReadValue();
			//adc_rd += ADC_ReadValue();
			//adc_rd += ADC_ReadValue();
			//adc_rd = adc_rd/5;
			//userProfile.user_profile.adc_reading = adc_rd;     //Changed by Jason Chen, 2014.12.19
		
			temp_f = adc_rd;
			temp_f = (temp_f*3.3f/65535.0f);
			sprintf((char*)displayBuff,"A %4.2f    ", temp_f);
			sLCD_putString_TFT(10,25,displayBuff,Font16x24);//displayBuff);
#else
			sprintf((char*)displayBuff,"A %d    ", led_on_count);
			sLCD_putString_TFT(10,25,displayBuff,Font16x24);//displayBuff);
#endif			
		}
  }
#endif
}

void ADC_Value_Display_one_time(void)
{
    float temp_f;

		//if(AMP_hold_flag)
		{		
      //AMP_hold_flag = 0;			
			adc_rd = 65535/2;//ADC_ReadValue();
			//userProfile.user_profile.adc_reading = adc_rd;     //Changed by Jason Chen, 2014.12.19
		
			temp_f = adc_rd;
			temp_f = (temp_f*3.3f/65535.0f);
			sprintf((char*)displayBuff,"A %4.2f", temp_f);
			sLCD_putString_TFT(10,25,displayBuff,Font16x24);//displayBuff);
		}
}

