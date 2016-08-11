/**
  ******************************************************************************
  * @file    CPU_ADC.c
  * @author  Jason Chen
  * @version V1.0
  * @date    27-Jan-2015
  * @brief
  ******************************************************************************
  **/ 

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "CPU_ADC.h"
#include "MiscTools.h"
#include "calibration.h"
#include "TLC2000_SPI_LCD_TFT.h"
#include "fonts.h"

#include "Hid_command_process.h"
#include "usb_lib.h"
#include "usb_pwr.h"

#include "Potentiometer.h"

#include "UTFT.h"
#include "UTFT_DLB.h"
#include "DAC7718.h"

// Declare which fonts we will be using
//extern uint8_t SevenSegNumFont[];
extern uint8_t DejaVuSans18[];

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/** @addtogroup ADC_ExtLinesTrigger
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define ADC1_DR_Address    ((uint32_t)0x4001244C)
#define ADC_REF_VOLT       3.0f                       // Added by Jason Chen, 2015.11.04

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint8_t ChannelSelect = 0;                       // Added by Jason Chen, 2015.03.04
__IO int16_t laser_sel     = 0;                       // Added by Jason Chen, 2015.10.07
__IO int16_t laserSelect   = 0;                       // Added by Jason Chen, 2015.12.14
__IO int16_t SelfTest_count= 0;                       // Added by Jason Chen, 2015.12.17
//__IO int16_t SelfTest_Flag_of_end = 0;                // Added by Jason Chen, 2015.12.17

__IO uint8_t Send_Buffer_test[64];
ADC_InitTypeDef   ADC_InitStructure;
DMA_InitTypeDef   DMA_InitStructure;
__IO uint16_t ADC_RegularConvertedValueTab[64];// ADC_InjectedConvertedValueTab[32];

uint16_t adc_ch_rd[8];
float measureVolt[8];
//float channelGain[4] = {0.25f,2.26f,9.1f,50.0f};
//const float channelGain[4] = {0.249f,25.5f,2.29f,9.1f};
const float channelGain[4] = {0.249f,25.5f,2.29f,9.1f};
//const float Ref_A2D_Value[10] = {1.0f,1.1f,1.2f,1.3f,1.4f,1.5f,1.6f,1.7f,1.8f,0.0f};    // Added by Jason Chen, 2015.10.07, TBD
//const float   Ref_A2D_Value[10] = {1.13f, 1.684f, 1.747f, 1.443f, 1.509f, 0,0,0,0};     // Added by Jason Chen, 2015.12.17 from Jack's test
const float   Ref_A2D_Value[10] = {0.133f, 0.582f, 0.264f, 0.488f, 2.390f, 0,0,0,0};       // Added by Jason Chen, 2016.08.07 from Jack's test
const float   Ref_A2D_Max[5]    = {0.165f, 0.643f, 0.315f, 0.533f, 2.700f};                // Added by Jason Chen, 2016.08.07 from Jack's test
const float   Ref_A2D_Min[5]    = {0.126f, 0.524f, 0.258f, 0.433f, 2.000f};                // Added by Jason Chen, 2016.08.07 from Jack's test

FEEDBACK_BUFFER_T  feedBack_profile;
SELF_TEST_RECORD_T selfTestBuf;                          // Added by Jason Chen, 2015.10.07
__IO int32_t OneMs_count = 0;

/* Private function prototypes -----------------------------------------------*/
void ADC_RCC_Configuration(void);
void ADC_GPIO_Configuration(void);
void ADC_NVIC_Configuration(void);
void ADC_EXTI_Configuration(void);

#if 0
  /* DMA1 channel1 configuration ----------------------------------------------*/
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADCConvertedValue;
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize = 1;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  
  /* Enable DMA1 channel1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);
  
  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channel14 configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 1, ADC_SampleTime_55Cycles5);

  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);

  /* Enable ADC1 reset calibration register */   
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));
     
  /* Start ADC1 Software Conversion */ 
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);

  while (1)
  {
  }	
	
/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void ADC_RCC_Configuration(void)
{
  /* ADCCLK = PCLK2/4 */
  RCC_ADCCLKConfig(RCC_PCLK2_Div4); 
  /* Enable peripheral clocks ------------------------------------------------*/
  /* Enable DMA1 clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  /* Enable ADC1 and GPIOC clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOC, ENABLE);
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Configure PC.04 (ADC Channel14) as analog input -------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}	

void ADC1_2_IRQHandler(void)
{
  /* Clear ADC1 JEOC pending interrupt bit */
  ADC_ClearITPendingBit(ADC1, ADC_IT_JEOC);
  
  /* Get injected channel11 and channel12 converted value */
  ADC_InjectedConvertedValueTab[Index++] = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
  ADC_InjectedConvertedValueTab[Index++] = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_2);
  
  if (Index == 32)
  {
    Index = 0;
  }
}
#endif
	
/* Private functions ---------------------------------------------------------*/

/**
  * @brief   Main program
  * @param  None
  * @retval None
  */
void Cpu_ADC_Init(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */     
       
  /* System clocks configuration ---------------------------------------------*/
  ADC_RCC_Configuration();   //72MHz/4 = 18MHz

  /* NVIC configuration ------------------------------------------------------*/
  ADC_NVIC_Configuration();

  /* GPIO configuration ------------------------------------------------------*/
  ADC_GPIO_Configuration();

  /* EXTI configuration ------------------------------------------------------*/
  //ADC_EXTI_Configuration();

  /* DMA1 channel1 configuration ----------------------------------------------*/
#if USE_DMA_MODE
  DMA_DeInit(DMA1_Channel1);
  DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_Address;
  DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)ADC_RegularConvertedValueTab;
  DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize         = 64;
  DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
  DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
  DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;
  DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  
	DMA_ITConfig(DMA1_Channel1,DMA_IT_TC, ENABLE);
	
  /* Enable DMA1 channel1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);

  
  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode               = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode       = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;//ADC_ExternalTrigConv_Ext_IT11_TIM8_TRGO;
  ADC_InitStructure.ADC_DataAlign          = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel       = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channels configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 1, ADC_SampleTime_28Cycles5);

  /* Regular discontinuous mode channel number configuration */
  ADC_DiscModeChannelCountConfig(ADC1, 1);
  /* Enable regular discontinuous mode */
  ADC_DiscModeCmd(ADC1, ENABLE);

  /* Enable JEOC interrupt */
  //ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);

  /* Enable ADC1 DMA */
  ADC_DMACmd(ADC1, ENABLE);
#else
  /* ADC1 configuration ------------------------------------------------------*/
  ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channel13 configuration */ 
  ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 1, ADC_SampleTime_28Cycles5);

  /* Regular discontinuous mode channel number configuration */
  ADC_DiscModeChannelCountConfig(ADC1, 1);
  /* Enable regular discontinuous mode */
  ADC_DiscModeCmd(ADC1, ENABLE);	

    /* Enable EOC interrupt */
  ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
	
#endif		
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);

  /* Enable ADC1 reset calibration register */   
  ADC_ResetCalibration(ADC1);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(ADC1));

  /* Start ADC1 calibration */
  ADC_StartCalibration(ADC1);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(ADC1));
}

void CPU_ADC_Value_Display(void)
{

  float temp_f;//,temp_f1;
  uint16_t m905Setup = 0;	
	//uint16_t foreground_color_bk;
	int i;
	uint32_t adc_rddd;
	
	if(adc_reading_interval)
	{						
		if(AMP_hold_flag)
		{	
			led_on_count++;
			if(led_on_count > 15)
			{
				led_on_count = 0;
				LED_Toggle();
			}

#if USE_DMA_MODE
			adc_rddd = 0;
			for(i = 0;i<64;i++)
				adc_rddd += ADC_RegularConvertedValueTab[i];
			adc_rd = adc_rddd/64;
									
      if(FeedBack905_Flag)
			{
			   adc_ch_rd[FeedBack905Test_Flag] = adc_rd;        // Added by Jason Chen, 2015.03.01
				 feedBack_profile.feed_back_buf.adc_ch_reading[FeedBack905Test_Flag] = adc_rd;
			}
			else
			{
				 adc_ch_rd[ChannelSelect] = adc_rd;        // Added by Jason Chen, 2015.03.04						
				 feedBack_profile.feed_back_buf.adc_ch_reading[ChannelSelect] = adc_rd;
			}
#else
			adc_rd = ADC_GetConversionValue(ADC1);
#endif			
			//userProfile.user_profile.adc_reading = adc_rd;     //Changed by Jason Chen, 2014.12.19

      //foreground_color_bk = foreground_color;
      //setFont(DejaVuSans18);		
			//setColor(COLOR_RED1);
			temp_f = adc_ch_rd[0];
			temp_f = feedBack_profile.feed_back_buf.adc_ch_reading[0];
			temp_f = (temp_f*ADC_REF_VOLT/4095.0f);		
      measureVolt[0] = temp_f;
			//sprintf((char*)displayBuff,"%4.2f  ", temp_f);            // Changed by Jason Chen, 2015.03.04
			//print_ttf((char*)displayBuff, 20+3*16, 55, 0);		
			
			//setColor(COLOR_YELLOW1);
			temp_f = adc_ch_rd[1];
			temp_f = feedBack_profile.feed_back_buf.adc_ch_reading[1];
			temp_f = (temp_f*ADC_REF_VOLT/4095.0f);			
			measureVolt[1] = temp_f;
			//sprintf((char*)displayBuff,"%4.2f  ", temp_f);            // Changed by Jason Chen, 2015.03.04
			//print_ttf((char*)displayBuff, 130+3*16-5, 55, 0);					
			
			//setColor(COLOR_BLUE1);
			temp_f = adc_ch_rd[2];
			temp_f = feedBack_profile.feed_back_buf.adc_ch_reading[2];
			temp_f = (temp_f*ADC_REF_VOLT/4095.0f);			
			measureVolt[2] = temp_f;
			//sprintf((char*)displayBuff,"%4.2f", temp_f);              // Changed by Jason Chen, 2015.03.04
			//print_ttf((char*)displayBuff, 20+3*16, 30, 0);					
			
			//setColor(COLOR_GREEN1);
			temp_f = adc_ch_rd[3];
			temp_f = feedBack_profile.feed_back_buf.adc_ch_reading[3];
			temp_f = (temp_f*ADC_REF_VOLT/4095.0f);			
			measureVolt[3] = temp_f;
			//sprintf((char*)displayBuff,"%4.2f", temp_f);              // Changed by Jason Chen, 2015.03.04
			//print_ttf((char*)displayBuff, 130+3*16-5, 30, 0);	

      //setColor(foreground_color_bk);
			
      if(FeedBack905_Flag)
			{
				if ((PrevXferComplete) && (bDeviceState == CONFIGURED))
			  {
					m905Setup = userProfile.user_profile.p905DAC4Voltage;
					Send_Buffer_test[0] = REPORT_ID_5; 
				  Send_Buffer_test[1] = (uint8_t)0xED; 
				  Send_Buffer_test[2] = (uint8_t)(adc_rd >> 8); 
				  Send_Buffer_test[3] = (uint8_t)adc_rd;        
			
				  Send_Buffer_test[4] = (uint8_t)(m905Setup >> 8); 
				  Send_Buffer_test[5] = (uint8_t)(m905Setup >> 0);   

				  Send_Buffer_test[6] = (uint8_t)(OneMs_count >> 8); 
				  Send_Buffer_test[7] = (uint8_t)(OneMs_count >> 0);   

					
				  /* Write the descriptor through the endpoint */
					Send_Buffer_test[8] = FeedBack905Test_Flag;
					Send_Buffer_test[9] = FeedBack905_Flag;
					
				  USB_SIL_Write(EP1_IN, (uint8_t*) Send_Buffer_test, MAX_TRANSFER_LEN);  
				  SetEPTxValid(ENDP1);
				  PrevXferComplete = 0; 
			  }
			}
			else
			{
			    feedBack_profile.feed_back_buf.adc_ch_reading_mw[0] = ((measureVolt[0]*1.5f)/channelGain[0])/0.55;
					feedBack_profile.feed_back_buf.adc_ch_reading_mw[1] = ((measureVolt[1]*1.5f)/channelGain[1])/0.55;
					feedBack_profile.feed_back_buf.adc_ch_reading_mw[2] = ((measureVolt[2]*1.5f)/channelGain[2])/0.55;
					feedBack_profile.feed_back_buf.adc_ch_reading_mw[3] = ((measureVolt[3]*1.5f)/channelGain[3])/0.55;
				
					//feedBack_profile.feed_back_buf.DAC_output = userProfile.user_profile.p905DAC4Voltage;
				  feedBack_profile.feed_back_buf.DAC_output = voltage_s;                                                     // Added by Jason Chen, 2015.12.14
				  feedBack_profile.feed_back_buf.validF  = ((measureVolt[0] >= 0.1f)&&(measureVolt[0] < 3.0f))? 1:0;
				  feedBack_profile.feed_back_buf.validF &= ((measureVolt[0] >= 0.1f)&&(measureVolt[0] < 3.0f))? 1:0;
				  feedBack_profile.feed_back_buf.validF &= ((measureVolt[0] >= 0.1f)&&(measureVolt[0] < 3.0f))? 1:0;
				  feedBack_profile.feed_back_buf.validF &= ((measureVolt[0] >= 0.1f)&&(measureVolt[0] < 3.0f))? 1:0;
				
				if ((PrevXferComplete) && (bDeviceState == CONFIGURED))
			    {
					feedBack_profile.feed_back_buf.report_id = REPORT_ID_5;
					feedBack_profile.feed_back_buf.report_cmd = 0xDD;
					feedBack_profile.feed_back_buf.ChannelSelect = ChannelSelect;
					
					
					//feedBack_profile.feed_back_buf.offset[0] = measureVolt[0];
					//feedBack_profile.feed_back_buf.offset[1] = measureVolt[1];
					//feedBack_profile.feed_back_buf.offset[2] = measureVolt[2];
					//feedBack_profile.feed_back_buf.offset[3] = measureVolt[3];
					
					
				  //USB_SIL_Write(EP1_IN, (uint8_t*) Send_Buffer_test, MAX_TRANSFER_LEN);  
					USB_SIL_Write(EP1_IN, (uint8_t*) feedBack_profile.feedBflash_byte, MAX_TRANSFER_LEN);  
				  SetEPTxValid(ENDP1);
				  PrevXferComplete = 0; 
			  }				
			}
			
			//ChannelSelect++;                       //removed by Jason Chen, 2015.04.02
			//if(ChannelSelect > 3)
			//if(ChannelSelect > 7)
			//	 ChannelSelect = 0;
			
			if((!FeedBack905_Flag)&&(OneMs_count == 0))
			{
#if 0				                                   //removed by Jason Chen, 2015.04.02
			   if((ChannelSelect == 0)||((ChannelSelect > 0) && (ChannelSelect <= 3)))
				 {					 
					 DAC7718_OUT_VOLT(s905_DAC4,userProfile.user_profile.p905DAC4Voltage);
				 }
				 else if((ChannelSelect >= 4) && (ChannelSelect <= 7))
				 {
					 DAC7718_OUT_VOLT(s905_DAC4,0);     // Testing, setup
				 }
				 				
			   if((ChannelSelect&0x03) == 0x00) 
				 {
					 Enable_Input(CHANNEL0, CH_ENABLE);
				   Enable_Input(CHANNEL1, CH_DISABLE);		
				   Enable_Input(CHANNEL2, CH_DISABLE);
		       Enable_Input(CHANNEL3, CH_DISABLE);								 
				 }
				 else if((ChannelSelect&0x03) == 0x01)
				 {
					 Enable_Input(CHANNEL0, CH_DISABLE);
				   Enable_Input(CHANNEL1, CH_ENABLE);		
				   Enable_Input(CHANNEL2, CH_DISABLE);
		       Enable_Input(CHANNEL3, CH_DISABLE);								 					 
				 }
				 else if((ChannelSelect&0x03) == 0x02)
				 {
					 Enable_Input(CHANNEL0, CH_DISABLE);		
				   Enable_Input(CHANNEL1, CH_DISABLE);		
				   Enable_Input(CHANNEL2, CH_ENABLE);
		       Enable_Input(CHANNEL3, CH_DISABLE);								 					 
				 }
				 else if((ChannelSelect&0x03) == 0x03)
				 {
					 Enable_Input(CHANNEL0, CH_DISABLE);		
				   Enable_Input(CHANNEL1, CH_DISABLE);		
				   Enable_Input(CHANNEL2, CH_DISABLE);
		       Enable_Input(CHANNEL3, CH_ENABLE);
				 }			
#else                                           //Addeded by Jason Chen, 2015.04.02
				ChannelSelect++;
				if(ChannelSelect > 7)                     
				ChannelSelect = 0;                      
				if((ChannelSelect&0x03) == 0x00) 
				{
					Enable_Input(CHANNEL0, CH_ENABLE);
					Enable_Input(CHANNEL1, CH_DISABLE);		
					Enable_Input(CHANNEL2, CH_DISABLE);
					Enable_Input(CHANNEL3, CH_DISABLE);								 
				}
				else if((ChannelSelect&0x03) == 0x01)
				{
					Enable_Input(CHANNEL0, CH_DISABLE);
					Enable_Input(CHANNEL1, CH_ENABLE);		
					Enable_Input(CHANNEL2, CH_DISABLE);
					Enable_Input(CHANNEL3, CH_DISABLE);								 					 
				}
				else if((ChannelSelect&0x03) == 0x02)
				{
					Enable_Input(CHANNEL0, CH_DISABLE);		
					Enable_Input(CHANNEL1, CH_DISABLE);		
					Enable_Input(CHANNEL2, CH_ENABLE);
					Enable_Input(CHANNEL3, CH_DISABLE);								 					 
				}
				else if((ChannelSelect&0x03) == 0x03)
				{
					Enable_Input(CHANNEL0, CH_DISABLE);		
					Enable_Input(CHANNEL1, CH_DISABLE);		
					Enable_Input(CHANNEL2, CH_DISABLE);
					Enable_Input(CHANNEL3, CH_ENABLE);
				}
				 
			  if((ChannelSelect == 0)||((ChannelSelect > 0) && (ChannelSelect <= 3)))
				{
						if(laserSelect == 0)      {DAC7718_OUT_VOLT(s905_DAC0,userProfile.user_profile.p905DAC0Voltage);}                  // Added by Jason Chen, 2015.12.,14
						else if(laserSelect == 1) {DAC7718_OUT_VOLT(s905_DAC1,userProfile.user_profile.p905DAC1Voltage);}
						else if(laserSelect == 2) {DAC7718_OUT_VOLT(s905_DAC2,userProfile.user_profile.p905DAC2Voltage);}
						else if(laserSelect == 3) {DAC7718_OUT_VOLT(s905_DAC3,userProfile.user_profile.p905DAC3Voltage);}
						else if(laserSelect == 4) {DAC7718_OUT_VOLT(s905_DAC4,userProfile.user_profile.p905DAC4Voltage);}
						else if(laserSelect == 5) {S660_DAC_OUT_VOLT(s660_DAC0,userProfile.user_profile.p660DAC0Voltage);}
						else if(laserSelect == 6) {S660_DAC_OUT_VOLT(s660_DAC1,userProfile.user_profile.p660DAC1Voltage);}
						else if(laserSelect == 7) {S660_DAC_OUT_VOLT(s660_DAC2,userProfile.user_profile.p660DAC2Voltage);}
						else if(laserSelect == 8) {S660_DAC_OUT_VOLT(s660_DAC3,userProfile.user_profile.p660DAC3Voltage);}
					
					  //DAC7718_OUT_VOLT(s905_DAC4,userProfile.user_profile.p905DAC4Voltage);
				}
				else if((FeedBack905Test_Flag >= 4) && (FeedBack905Test_Flag <= 7))
				{
						if(laserSelect == 0)      {DAC7718_OUT_VOLT(s905_DAC0,0);}                                                         // Added by Jason Chen, 2015.12.,14
						else if(laserSelect == 1) {DAC7718_OUT_VOLT(s905_DAC1,0);}
						else if(laserSelect == 2) {DAC7718_OUT_VOLT(s905_DAC2,0);}
						else if(laserSelect == 3) {DAC7718_OUT_VOLT(s905_DAC3,0);}
						else if(laserSelect == 4) {DAC7718_OUT_VOLT(s905_DAC4,0);}
						else if(laserSelect == 5) {S660_DAC_OUT_VOLT(s660_DAC0,0);}
						else if(laserSelect == 6) {S660_DAC_OUT_VOLT(s660_DAC1,0);}
						else if(laserSelect == 7) {S660_DAC_OUT_VOLT(s660_DAC2,0);}
						else if(laserSelect == 8) {S660_DAC_OUT_VOLT(s660_DAC3,0);}					
					
					 //DAC7718_OUT_VOLT(s905_DAC4,0);
				}				
				//OneMs_count =  100;				  // Test one time each 100ms	
        OneMs_count = AutoTestTimeInterval;				                             // Added by Jason Chen, 2015.09.29
				if(OneMs_count < 100)                                                  // Added by Jason Chen, 2015.09.29
					OneMs_count = 100;                                                   // Added by Jason Chen, 2015.09.29
#endif				 
			}
			
			if((OneMs_count == 0 )&&(FeedBack905_Flag))
			{		
				FeedBack905Test_Flag++;
				if(FeedBack905Test_Flag > 7)                     // Added by Jason Chen, 2015.03.01
				FeedBack905Test_Flag = 0;                      // Added by Jason Chen, 2015.03.01						
				if((FeedBack905Test_Flag&0x03) == 0x00) 
				{
					Enable_Input(CHANNEL0, CH_ENABLE);
					Enable_Input(CHANNEL1, CH_DISABLE);		
					Enable_Input(CHANNEL2, CH_DISABLE);
					Enable_Input(CHANNEL3, CH_DISABLE);								 
				}
				else if((FeedBack905Test_Flag&0x03) == 0x01)
				{
					Enable_Input(CHANNEL0, CH_DISABLE);
					Enable_Input(CHANNEL1, CH_ENABLE);		
					Enable_Input(CHANNEL2, CH_DISABLE);
					Enable_Input(CHANNEL3, CH_DISABLE);								 					 
				}
				else if((FeedBack905Test_Flag&0x03) == 0x02)
				{
					Enable_Input(CHANNEL0, CH_DISABLE);		
					Enable_Input(CHANNEL1, CH_DISABLE);		
					Enable_Input(CHANNEL2, CH_ENABLE);
					Enable_Input(CHANNEL3, CH_DISABLE);								 					 
				}
				else if((FeedBack905Test_Flag&0x03) == 0x03)
				{
					Enable_Input(CHANNEL0, CH_DISABLE);		
					Enable_Input(CHANNEL1, CH_DISABLE);		
					Enable_Input(CHANNEL2, CH_DISABLE);
					Enable_Input(CHANNEL3, CH_ENABLE);
				}
				 
		  //if(((FeedBack905Test_Flag >= 0) && (FeedBack905Test_Flag <= 3)))
			  if((FeedBack905Test_Flag == 0)||((FeedBack905Test_Flag > 0) && (FeedBack905Test_Flag <= 3)))
				{
					userProfile.user_profile.p905DAC4Voltage = AutoTestValue[0];
				}
				else if((FeedBack905Test_Flag >= 4) && (FeedBack905Test_Flag <= 7))
				{
					userProfile.user_profile.p905DAC4Voltage = AutoTestValue[1];
				}
				DAC7718_OUT_VOLT(s905_DAC4,userProfile.user_profile.p905DAC4Voltage);
				OneMs_count =  AutoTestTimeInterval;
				
			  //FeedBack905Test_Flag = 0;
        //Enable_Input(CHANNEL0, CH_ENABLE);
				//Enable_Input(CHANNEL1, CH_DISABLE);		
				//Enable_Input(CHANNEL2, CH_DISABLE);
		    //Enable_Input(CHANNEL3, CH_DISABLE);								 
			}			
			
      AMP_hold_flag = 0;			
		}
  }
}

void SelfTest_Init(void)                                  // Added by Jason Chen, 2015.12.17
{
	uint8_t i;
	for(i = 0; i<9; i++)
	{
		selfTestBuf.selfTest_buf.validF[i] = 0;
		selfTestBuf.selfTest_buf.adc_ch_reading[i] = 0;
	}
	selfTestBuf.selfTest_buf.flag_of_end = 0;
	
	ChannelSelect = 0;
  laser_sel     = 0;      
  SelfTest_count= 0;
//	SelfTest_Flag_of_end = 0;
}

void CPU_ADC_Value_SelfTest(void)
{

  float temp_f;//,temp_f1;
//uint16_t m905Setup = 0;	
//uint16_t foreground_color_bk;
	int i;
	uint32_t adc_rddd;
	uint8_t testResult;
	
	if(adc_reading_interval)
	{						
		if(AMP_hold_flag)
		{	
			led_on_count++;
			if(led_on_count > 15)
			{
				led_on_count = 0;
				LED_Toggle();
			}

			adc_rddd = 0;
			for(i = 0;i<64;i++)
				adc_rddd += ADC_RegularConvertedValueTab[i];
			adc_rd = adc_rddd/64;
									
  		adc_ch_rd[ChannelSelect] = adc_rd;                                                 // Added by Jason Chen, 2015.03.04						
			selfTestBuf.selfTest_buf.adc_ch_reading[laser_sel] = adc_rd;

			temp_f  = adc_ch_rd[ChannelSelect];
			temp_f += feedBack_profile.feed_back_buf.offset[ChannelSelect];
			if(temp_f > 4095)
				temp_f = 4095;
			temp_f  = (temp_f*ADC_REF_VOLT/4095.0f);	
      
#if 0 
			DisplaySelfCali(laser_sel+1, temp_f, 1);					
			temp_f  = temp_f - Ref_A2D_Value[laser_sel];
			temp_f = (temp_f/Ref_A2D_Value[laser_sel])*100;
			if(temp_f < 0.0f) temp_f = -temp_f;

      testResult = ((temp_f > 20.0f))? 1:0;
		//selfTestBuf.selfTest_buf.validF[laser_sel] += testResult;                             // Removed by Jason Chen, 2015.12.21
		//selfTestBuf.selfTest_buf.adc_offset_rate[laser_sel] = temp_f;
			selfTestBuf.selfTest_buf.adc_ch_reading[laser_sel] = adc_ch_rd[ChannelSelect];
									
			selfTestBuf.selfTest_buf.laserSelect    = laser_sel;//ChannelSelect;
  		if((SelfTest_count >= 9)&&(laser_sel == 4))
				selfTestBuf.selfTest_buf.flag_of_end  = 1;
			selfTestBuf.selfTest_buf.SelfTest_count = SelfTest_count;				
#else
			if(temp_f < Ref_A2D_Min[laser_sel])
			{
				SelfTest_adj(laser_sel, 20);
			}
			else if(temp_f > Ref_A2D_Max[laser_sel])
			{
				SelfTest_adj(laser_sel, -20);
			}
			else
			{
			}
			DisplaySelfCali(laser_sel+1, temp_f,0);
#endif			
			if(OneMs_count == 0)
			{	
        if(testResult) selfTestBuf.selfTest_buf.validF[laser_sel] += 1;                    // Added by Jason Chen, 2015.12.21
				
				if ((PrevXferComplete) && (bDeviceState == CONFIGURED))
				{
					selfTestBuf.selfTest_buf.report_id = REPORT_ID_5;
					selfTestBuf.selfTest_buf.report_cmd     = 0xDF;                         // Changed by Jason Chen from 0xDD to 0xDF, 2015.10.07
//				selfTestBuf.selfTest_buf.laserSelect    = laser_sel;//ChannelSelect;
//				if((SelfTest_count >= 9)&&(laser_sel == 4))
//					  selfTestBuf.selfTest_buf.flag_of_end  = 1;
//				selfTestBuf.selfTest_buf.SelfTest_count = SelfTest_count;
					USB_SIL_Write(EP1_IN, (uint8_t*) selfTestBuf.selfTest_byte, MAX_TRANSFER_LEN);//sizeof(SELF_TEST_RECORD_T));
					SetEPTxValid(ENDP1);
					PrevXferComplete = 0; 
				}
							
        // Changed by Jason Chen, just test 905 laser, 2015.12.17
				laser_sel++;                           // 5 905 lasers selection
			  if(laser_sel > 4)
				{
					laser_sel = 0;					
					SelfTest_count++;										
          if(SelfTest_count >= 20)
					{
					  Self_Cali_End();
						//PROBE_Idle();
						//SelfTest_count = 0;
						//SelfTest_Flag_of_end = 1;
						if(selfTestBuf.selfTest_buf.validF[0] <= 4)
							selfTestBuf.selfTest_buf.validF[0] = 0;
						else
							selfTestBuf.selfTest_buf.validF[0] = 1;
						
						if(selfTestBuf.selfTest_buf.validF[1] <= 4)
							selfTestBuf.selfTest_buf.validF[1] = 0;
						else
							selfTestBuf.selfTest_buf.validF[1] = 1;
						
						if(selfTestBuf.selfTest_buf.validF[2] <= 4)
							selfTestBuf.selfTest_buf.validF[1] = 0;
						else
							selfTestBuf.selfTest_buf.validF[2] = 1;

						if(selfTestBuf.selfTest_buf.validF[3] <= 4)
							selfTestBuf.selfTest_buf.validF[3] = 0;
						else
							selfTestBuf.selfTest_buf.validF[3] = 1;

						if(selfTestBuf.selfTest_buf.validF[4] <= 4)
							selfTestBuf.selfTest_buf.validF[4] = 0;
						else
							selfTestBuf.selfTest_buf.validF[4] = 1;							
						return;
					}
				}
				
				if(laser_sel == 0)					ChannelSelect = 3;
				else if(laser_sel == 1)			ChannelSelect = 2;
				else if(laser_sel == 2)			ChannelSelect = 3;
				else if(laser_sel == 3)			ChannelSelect = 1;
				else /*laser_sel = 4*/  		ChannelSelect = 0;			

				//if(laser_sel == 250)                               // Added Jason Chen, 2015.12.17
				//	DeActive_SelfTest();				
				if((ChannelSelect&0x03) == 0x00) 
				{
					Enable_Input(CHANNEL0, CH_ENABLE);
					Enable_Input(CHANNEL1, CH_DISABLE);		
					Enable_Input(CHANNEL2, CH_DISABLE);
					Enable_Input(CHANNEL3, CH_DISABLE);								 
				}
				else if((ChannelSelect&0x03) == 0x01)
				{
					Enable_Input(CHANNEL0, CH_DISABLE);
					Enable_Input(CHANNEL1, CH_ENABLE);		
					Enable_Input(CHANNEL2, CH_DISABLE);
					Enable_Input(CHANNEL3, CH_DISABLE);								 					 
				}
				else if((ChannelSelect&0x03) == 0x02)
				{
					Enable_Input(CHANNEL0, CH_DISABLE);		
					Enable_Input(CHANNEL1, CH_DISABLE);		
					Enable_Input(CHANNEL2, CH_ENABLE);
					Enable_Input(CHANNEL3, CH_DISABLE);								 					 
				}
				else if((ChannelSelect&0x03) == 0x03)
				{
					Enable_Input(CHANNEL0, CH_DISABLE);		
					Enable_Input(CHANNEL1, CH_DISABLE);		
					Enable_Input(CHANNEL2, CH_DISABLE);
					Enable_Input(CHANNEL3, CH_ENABLE);
				}
				 
				SelfTest_setup(laser_sel);
				
				OneMs_count = 1000;	 
		  }
      AMP_hold_flag = 0;			
		}
  }
}

void CPU_ADC_Value_SelfTest_Wayne(void)
{

  float temp_f;//,temp_f1;
//uint16_t m905Setup = 0;	
//uint16_t foreground_color_bk;
	int i;
	uint32_t adc_rddd;
	uint8_t testResult;
	
	if(adc_reading_interval)
	{						
		if(AMP_hold_flag)
		{	
			led_on_count++;
			if(led_on_count > 15)
			{
				led_on_count = 0;
				LED_Toggle();
			}

#if USE_DMA_MODE
			adc_rddd = 0;
			for(i = 0;i<64;i++)
				adc_rddd += ADC_RegularConvertedValueTab[i];
			adc_rd = adc_rddd/64;
									
  		adc_ch_rd[ChannelSelect] = adc_rd;        // Added by Jason Chen, 2015.03.04						
			selfTestBuf.selfTest_buf.adc_ch_reading[laser_sel] = adc_rd;
#else
			adc_rd = ADC_GetConversionValue(ADC1);
#endif			

			temp_f = adc_ch_rd[ChannelSelect];
			//temp_f = feedBack_profile.feed_back_buf.adc_ch_reading[0];
			temp_f += feedBack_profile.feed_back_buf.offset[ChannelSelect];
			temp_f = (temp_f*ADC_REF_VOLT/4095.0f);								     
			temp_f = temp_f - Ref_A2D_Value[laser_sel];
			temp_f = (temp_f/Ref_A2D_Value[laser_sel])*100;
			if(temp_f < 0.0f) temp_f = -temp_f;
							
      testResult = ((temp_f > 20.0f))? 1:0;
		//selfTestBuf.selfTest_buf.validF[laser_sel] += testResult;                             // Removed by Jason Chen, 2015.12.21
		//selfTestBuf.selfTest_buf.adc_offset_rate[laser_sel] = temp_f;
			selfTestBuf.selfTest_buf.adc_ch_reading[laser_sel] = adc_ch_rd[ChannelSelect];
									
			selfTestBuf.selfTest_buf.laserSelect    = laser_sel;//ChannelSelect;
  		if((SelfTest_count >= 9)&&(laser_sel == 4))
		  selfTestBuf.selfTest_buf.flag_of_end  = 1;
			selfTestBuf.selfTest_buf.SelfTest_count = SelfTest_count;				
				
			if(OneMs_count == 0)
			{	
        if(testResult) selfTestBuf.selfTest_buf.validF[laser_sel] += 1;                    // Added by Jason Chen, 2015.12.21
				
				if ((PrevXferComplete) && (bDeviceState == CONFIGURED))
				{
					selfTestBuf.selfTest_buf.report_id = REPORT_ID_5;
					selfTestBuf.selfTest_buf.report_cmd     = 0xDF;                         // Changed by Jason Chen from 0xDD to 0xDF, 2015.10.07
//				selfTestBuf.selfTest_buf.laserSelect    = laser_sel;//ChannelSelect;
//				if((SelfTest_count >= 9)&&(laser_sel == 4))
//					  selfTestBuf.selfTest_buf.flag_of_end  = 1;
//				selfTestBuf.selfTest_buf.SelfTest_count = SelfTest_count;
					USB_SIL_Write(EP1_IN, (uint8_t*) selfTestBuf.selfTest_byte, MAX_TRANSFER_LEN);//sizeof(SELF_TEST_RECORD_T));
					SetEPTxValid(ENDP1);
					PrevXferComplete = 0; 
				}
							
#if 0	
        laser_sel++;                           // 9 lasers selection				
			  if(laser_sel > 8)                                   // Removed by Jason Chen, 2015.12.17
					laser_sel = 0;
				if((laser_sel >= 0)&&(laser_sel < 2))
					ChannelSelect = 0;
				else if((laser_sel >= 2)&&(laser_sel < 4))
					ChannelSelect = 1;
				else if((laser_sel >= 3)&&(laser_sel < 6))
					ChannelSelect = 2;
				else if((laser_sel >= 4)&&(laser_sel < 8))
					ChannelSelect = 3;
				else
					ChannelSelect = 1;				
#else                                          // Changed by Jason Chen, just test 905 laser, 2015.12.17
				laser_sel++;                           // 5 905 lasers selection
			  if(laser_sel > 4)
				{
					laser_sel = 0;					
					SelfTest_count++;										
          if(SelfTest_count >= 10)
					{
					  PROBE_Idle();												
						//SelfTest_count = 0;
						//SelfTest_Flag_of_end = 1;
						if(selfTestBuf.selfTest_buf.validF[0] <= 4)
							selfTestBuf.selfTest_buf.validF[0] = 0;
						else
							selfTestBuf.selfTest_buf.validF[0] = 1;
						
						if(selfTestBuf.selfTest_buf.validF[1] <= 4)
							selfTestBuf.selfTest_buf.validF[1] = 0;
						else
							selfTestBuf.selfTest_buf.validF[1] = 1;
						
						if(selfTestBuf.selfTest_buf.validF[2] <= 4)
							selfTestBuf.selfTest_buf.validF[1] = 0;
						else
							selfTestBuf.selfTest_buf.validF[2] = 1;

						if(selfTestBuf.selfTest_buf.validF[3] <= 4)
							selfTestBuf.selfTest_buf.validF[3] = 0;
						else
							selfTestBuf.selfTest_buf.validF[3] = 1;

						if(selfTestBuf.selfTest_buf.validF[4] <= 4)
							selfTestBuf.selfTest_buf.validF[4] = 0;
						else
							selfTestBuf.selfTest_buf.validF[4] = 1;							
						return;
					}
				}
				
				if(laser_sel == 0)					ChannelSelect = 3;
				else if(laser_sel == 1)			ChannelSelect = 2;
				else if(laser_sel == 2)			ChannelSelect = 3;
				else if(laser_sel == 3)			ChannelSelect = 1;
				else /*laser_sel = 4*/  		ChannelSelect = 0;			

				//if(laser_sel == 250)                               // Added Jason Chen, 2015.12.17
				//	DeActive_SelfTest();				
#endif				
				if((ChannelSelect&0x03) == 0x00) 
				{
					Enable_Input(CHANNEL0, CH_ENABLE);
					Enable_Input(CHANNEL1, CH_DISABLE);		
					Enable_Input(CHANNEL2, CH_DISABLE);
					Enable_Input(CHANNEL3, CH_DISABLE);								 
				}
				else if((ChannelSelect&0x03) == 0x01)
				{
					Enable_Input(CHANNEL0, CH_DISABLE);
					Enable_Input(CHANNEL1, CH_ENABLE);		
					Enable_Input(CHANNEL2, CH_DISABLE);
					Enable_Input(CHANNEL3, CH_DISABLE);								 					 
				}
				else if((ChannelSelect&0x03) == 0x02)
				{
					Enable_Input(CHANNEL0, CH_DISABLE);		
					Enable_Input(CHANNEL1, CH_DISABLE);		
					Enable_Input(CHANNEL2, CH_ENABLE);
					Enable_Input(CHANNEL3, CH_DISABLE);								 					 
				}
				else if((ChannelSelect&0x03) == 0x03)
				{
					Enable_Input(CHANNEL0, CH_DISABLE);		
					Enable_Input(CHANNEL1, CH_DISABLE);		
					Enable_Input(CHANNEL2, CH_DISABLE);
					Enable_Input(CHANNEL3, CH_ENABLE);
				}
				 
				SelfTest_setup(laser_sel);
				
				OneMs_count = 1000;	 
		  }
      AMP_hold_flag = 0;			
		}
  }
}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void ADC_RCC_Configuration_backup(void)
{
  /* ADCCLK = PCLK2/4 */
  RCC_ADCCLKConfig(RCC_PCLK2_Div4); 
  /* Enable peripheral clocks ------------------------------------------------*/
  /* Enable DMA1 clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  /* Enable GPIOs and ADC1 clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | 
                         RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO |
                         RCC_APB2Periph_ADC1, ENABLE);
}

void ADC_RCC_Configuration(void)
{
  /* ADCCLK = PCLK2/4 */
  RCC_ADCCLKConfig(RCC_PCLK2_Div4); //  72MHz/4 = 18MHz

  /* Enable peripheral clocks ------------------------------------------------*/
#if USE_DMA_MODE
  /* Enable DMA1 clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
#endif	

  /* Enable GPIOs and ADC1 clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO | RCC_APB2Periph_ADC1, ENABLE);
}
/**
  * @brief  Configures the different EXTI lines.
  * @param  None
  * @retval None
  */
void ADC_EXTI_Configuration(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource11);

  /* EXTI line11 configuration -----------------------------------------------*/  
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_Line = EXTI_Line11;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Select the EXTI Line15 the GPIO pin source */
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource15);
  /* EXTI line15 configuration -----------------------------------------------*/  
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_Line = EXTI_Line15;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void ADC_GPIO_Configuration_backup(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Configure PC.01, PC.02 and PC.04 (ADC Channel11, Channel12 and Channel14)
   as analog input -----------------------------------------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Configure PA.04 (ADC Channel4) as analog input --------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure EXTI line11 ---------------------------------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;

  GPIO_Init(GPIOE, &GPIO_InitStructure);

  /* Configure EXTI line15 ---------------------------------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
}

void ADC_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Configure PC.03 (ADC Channel13)
   as analog input -----------------------------------------------------------*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/**
  * @brief  Configures NVIC and Vector Table base location.
  * @param  None
  * @retval None
  */
void ADC_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
	
#if USE_DMA_MODE	
	  /* Enable DMA1 channel1 IRQ Channel */
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#else	
    /* Configure and enable ADC interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif	
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

