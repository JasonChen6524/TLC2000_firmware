/**
  ******************************************************************************
  * @file    stm32_it.c
  * @author  Jason Chen
  * @date    21-January-2013
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include "stm32_it.h"
#include "usb_istr.h"
#include "usb_lib.h"
#include "usb_pwr.h"
#include "hw_config.h"
#include "Potentiometer.h"
#include "ADS8319_ADC.h"
#include "MiscTools.h"
#include "Buzzer.h"
#include "RTC.h"

#include "custom_hid_conf.h"                    //It's me
#include "Switch2.h"                            // 2014.10.20
#include "tBT.h"

#include "FPGA_RAM.h"                           // 2014.12.19
#include "CPU_ADC.h"

#include "BatSensor.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

extern uint8_t TxBuffer[]; 
extern uint8_t RxBuffer[]; 
extern __IO uint8_t TxCounter;
extern __IO uint8_t RxCounter; 
extern uint8_t NbrOfDataToTransfer;
extern uint8_t NbrOfDataToRead;
extern UartParams uartComm;


extern __IO uint8_t Send_Buffer[MAX_TRANSFER_LEN];//2];
extern __IO uint8_t PrevXferComplete;
extern uint32_t ADC_ConvertedValueX;
extern uint32_t ADC_ConvertedValueX_1;
extern __IO uint32_t TimingDelay;
extern __IO uint32_t TimingDelay1;
extern __IO uint32_t TimingDending;
extern __IO uint8_t sendFlag;
extern __IO uint32_t TimeDisplay;
extern __IO uint32_t TemperatureDispay;
extern __IO uint32_t ADC_Dispay;
extern __IO uint32_t usb_out_flag;

//extern uint8_t buttonReqFlag;

extern uint16_t DAC7718_SendHalfWord(uint16_t HalfWord);
extern int16_t RSSI;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M Processor Exceptions Handlers                         */
/******************************************************************************/

/*******************************************************************************
* Function Name  : NMI_Handler
* Description    : This function handles NMI exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NMI_Handler(void)
{
}

/*******************************************************************************
* Function Name  : HardFault_Handler
* Description    : This function handles Hard Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : MemManage_Handler
* Description    : This function handles Memory Manage exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : BusFault_Handler
* Description    : This function handles Bus Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : UsageFault_Handler
* Description    : This function handles Usage Fault exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/*******************************************************************************
* Function Name  : SVC_Handler
* Description    : This function handles SVCall exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SVC_Handler(void)
{
}

/*******************************************************************************
* Function Name  : DebugMon_Handler
* Description    : This function handles Debug Monitor exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DebugMon_Handler(void)
{
}

/*******************************************************************************
* Function Name  : PendSV_Handler
* Description    : This function handles PendSVC exception.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PendSV_Handler(void)
{
}

/*******************************************************************************
* Function Name  : SysTick_Handler
* Description    : This function handles SysTick Handler.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SysTick_Handler(void)
{
    TimingDelay--;
    TimingDelay1++;   //--	
	  TimingDending++;
	
#if USE_TIM8_PWM
	     ;
#else
	  if(TurnOnBuzzer)
			BuzzerToggle();				
#endif		
	
	  SleepTimer_TickCount++;
		SWITCH2_Timer++;
		
		ADC_Dispay++;
		
		if(OneMs_count > 0)
			OneMs_count--;			
}

/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/

/**
  * @brief  This function handles RTC global interrupt request.
  * @param  None
  * @retval None
  */
void RTC_IRQHandler(void)
{
  if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
  {
    /* Clear the RTC Second interrupt */
    RTC_ClearITPendingBit(RTC_IT_SEC);

    /* Toggle LED1 */
    //LED_Toggle();

    /* Enable time update */
    TimeDisplay = 1;
		TemperatureDispay = 1;
		BatteryDispay++;                    // Added by Jason Chen, 2016.06.24
		
		if(!stopFlag)
		   selfTest_cnt++;                     // Added by Jason Chen, 2015.10.07
		
		//ADC_Dispay = 1;
		
		usb_out_flag = 1;

		if(new_treatment_time >=0)
			new_treatment_time--;
		
		if(!Pause_flag)
		{
		  if(treatment_time>=0)
			  treatment_time--;
	  }
		
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
		
		RSSI = rand();
#if TEMPERATURE_DISPLAY_TEST                                // just for testing, Added by Jason Chen, 2015.07.04
		HighTemperatureCount++;
		if(HighTemperatureCount==10)
		{
			 BuzzerOn();
			 HighTemperatureAlarm = 1;
			 battery_profile.bat_feedback.HighTempAlarm = 1;
		}
		else if(HighTemperatureCount==20)
		{
			 HighTemperatureCount = 0;
			 BuzzerOff();
			 HighTemperatureAlarm = 0;                      
			 battery_profile.bat_feedback.HighTempAlarm = 0;
			 HighTemperatureClear = 1;                      
		}
#endif		
  }
}

/******************************************************************************/
/*            STM32 Peripherals Interrupt Handlers                        */
/******************************************************************************/

/*******************************************************************************
* Function Name  : USB_IRQHandler
* Description    : This function handles USB Low Priority interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_LP_CAN1_RX0_IRQHandler(void)
{
  USB_Istr();
}

/*******************************************************************************
* Function Name  : EXTI4_IRQHandler
* Description    : This function handles External lines interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//__IO uint32_t sec_count = 0;
void EXTI4_IRQHandler(void)                                       // Added by Jason, 2014.12.19
{	
#ifndef BOARD_R4	                                                // Added by Jason Chen, 2015.06.016	
  if(EXTI_GetITStatus(FPGA_AD_RD_CLK_EXTI_LINE) != RESET)
  {  
    if(AD_RD_CLK_PBGetState())
		{
#if 0			
       AMP_hold_flag = 1;
#else
       /* Start ADC1 Software Conversion */ 
			 if(!AMP_hold_flag)
			 {
				 ADC_SoftwareStartConvCmd(ADC1, ENABLE);				 
			 }				 
#endif			 
		}
		else
		{

		}
    /* Clear the EXTI line  pending bit */
    EXTI_ClearITPendingBit(FPGA_AD_RD_CLK_EXTI_LINE);
  }
#endif	
}

void DMA1_Channel1_IRQHandler(void)
{
  /* Test on DMA1 Channel6 Transfer Complete interrupt */
  if(DMA_GetITStatus(DMA1_IT_TC1))
  {
    /* Get Current Data Counter value after complete transfer */
    //CurrDataCounterEnd = DMA_GetCurrDataCounter(DMA1_Channel6);
    /* Clear DMA1 Channel6 Half Transfer, Transfer Complete and Global interrupt pending bits */
		AMP_hold_flag = 1;
		//LED_Toggle();
    DMA_ClearITPendingBit(DMA1_IT_GL1);
  }
}

void ADC1_2_IRQHandler(void)
{
  /* Clear ADC1 JEOC pending interrupt bit */
	
  if(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)!=RESET)        //End of conversion flaguint8_t ADC_FLAG)
		AMP_hold_flag = 1;
  ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);//ADC_IT_JEOC);
  
  /* Get injected channel11 and channel12 converted value */
  //ADC_InjectedConvertedValueTab[Index++] = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_1);
  //ADC_InjectedConvertedValueTab[Index++] = ADC_GetInjectedConversionValue(ADC1, ADC_InjectedChannel_2);
  
  //if (Index == 32)
  //{
  //  Index = 0;
  //}
}

/*******************************************************************************
* Function Name  : EXTI_IRQHandler
* Description    : This function handles External lines interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI9_5_IRQHandler(void)                 // Temperature Alarm
{
  if(EXTI_GetITStatus(INT_TEMP_EXTI_LINE) != RESET)
  {  
#if ENALE_HI_TEMP
    if (Temp_Int_PBGetState() == Bit_RESET)
		{
			 BuzzerOn();
			 HighTemperatureAlarm = 1;                                    // Added by Jason Chen, 2015.07.03
			 battery_profile.bat_feedback.HighTempAlarm = 1;              // Added by Jason Chen, 2015.07.03
		}
		else
		{
			 BuzzerOff();
			 HighTemperatureAlarm = 0;                                    // Added by Jason Chen, 2015.07.03
			 battery_profile.bat_feedback.HighTempAlarm = 0;              // Added by Jason Chen, 2015.07.03
			 HighTemperatureClear = 1;                                    // Added by Jason Chen, 2015.07.04
		}
#endif
    /* Clear the EXTI line  pending bit */
    EXTI_ClearITPendingBit(INT_TEMP_EXTI_LINE);
  }
}

/*******************************************************************************
* Function Name  : EXTI2_IRQHandler
* Description    : This function handles External lines interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI2_IRQHandler(void)
{
  if(EXTI_GetITStatus(SW_EXTI_LINE) != RESET)
  {  
    if (SW_Int_PBGetStateOn())// == Bit_RESET)
		{
       //button_press = 1;
		}
		else
		{

		}
    /* Clear the EXTI line  pending bit */
    EXTI_ClearITPendingBit(SW_EXTI_LINE);
  }
}

/*******************************************************************************
* Function Name  : EXTI15_10_IRQHandler
* Description    : This function handles External lines 15 to 10 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI15_10_IRQHandler(void)
{
	
#ifdef BOARD_R4	                                              // Added by Jason Chen, 2015.06.016
	if(EXTI_GetITStatus(FPGA_AD_RD_CLK_EXTI_LINE) != RESET)
	{
     if(AD_RD_CLK_PBGetState())
		 {
      #if 0			
       AMP_hold_flag = 1;
      #else
       /* Start ADC1 Software Conversion */ 
			 if(!AMP_hold_flag)
			 {
				 ADC_SoftwareStartConvCmd(ADC1, ENABLE);				 
			 }				 
      #endif			 
		 }
		 else{}
    /* Clear the EXTI line  pending bit */
    EXTI_ClearITPendingBit(FPGA_AD_RD_CLK_EXTI_LINE);
	}
#endif
	
  if(EXTI_GetITStatus(CONV_END_EXTI_LINE) != RESET)
  { 
#if 0    
    if(ADC_END_PBGetState() == Bit_RESET)
    {
	    SEN_DAC_CS_LOW();
	    while(ADC_END_PBGetState())
  
	    ADC_Value = DAC7718_SendHalfWord(0xA5A5);
	
	    SEN_DAC_CS_HIGH();

      conert_end_flag = 1;
      CONVST_LOW();                                  //Stop ADC			
    }
#endif		
    /* Clear the EXTI line 13 pending bit */		
    EXTI_ClearITPendingBit(TAMPER_BUTTON_EXTI_LINE);
  }
}


/*******************************************************************************
* Function Name  : USB_FS_WKUP_IRQHandler
* Description    : This function handles USB WakeUp interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBWakeUp_IRQHandler(void)
{
  EXTI_ClearITPendingBit(EXTI_Line18);
}

/******************************************************************************/
/*            STM32F10x Peripherals Interrupt Handlers                        */
/******************************************************************************/

/**
  * @brief  This function handles USARTy global interrupt request.
  * @param  None
  * @retval None
  */
extern __IO BLE_CONFIG pcCmd; 

//__IO byte uartStat =0;

void USART2_IRQHandler(void)
{

   __IO byte c;
   __IO byte index;
   __IO static  byte first = 0;
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {

    /* Read one byte from the receive data register */

    c = USART_ReceiveData(USART2);
	 
    index = uartComm.RXp;			
    if(++index == uartComm.RXl)
		   index = 0;
    if(index != uartComm.RXg)
    {
	    uartComm.RX[uartComm.RXp]= c;
      uartComm.RXp = index;
      uartComm.RXf = 1;
    }
  }
  
  if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
  {   
		index = uartComm.TXg;			
    if(++index == uartComm.TXl)
		index = 0;
    if(index != uartComm.TXp)
    {
	    c = uartComm.TX[uartComm.TXg];
      uartComm.TXg = index;
      USART_SendData(USART2, c);
    }
    else 
    {
      /* Disable the USART Transmit interrupt */
      USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
			uartComm.TXf  = 0;
    }    
	}
}


/******************************************************************************/
/*                 STM32 Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32xxx.s).                                            */
/******************************************************************************/

/*******************************************************************************
* Function Name  : PPP_IRQHandler
* Description    : This function handles PPP interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/*void PPP_IRQHandler(void)
{
}*/

/*****END OF FILE****/
