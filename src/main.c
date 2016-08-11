/**
  ******************************************************************************
  * @file    main.c
  * @date    21-January-2013
  * @brief   main file
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_pwr.h"

#include "custom_hid_conf.h"                    //It's me
#include "TLC2000_SPI_LCD_TFT.h"
//#include "AD5504_DAC.h"
#include "DAC7718.h"
#include "RTC.h"
#include "Potentiometer.h"
#include "FPGA_RAM.h"
//#include "ADS8319_ADC.h"
#include "CPU_ADC.h"
#include "MiscTools.h"
#include "Buzzer.h"
#include "calibration.h"

#include "Hid_command_process.h"
#include "Switch2.h"
#include "tBT.h"

#include "UTFT.h"

#include "BatSensor.h"                    // Added by Jason Chen, 2015.06.23

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint8_t PrevXferComplete = 1;
__IO uint32_t TimingDelay     = 0;
__IO uint32_t TimingDelay1    = 0;
uint8_t displayBuff[16]       ={0};
uint8_t CMD_ID                = 0;
uint8_t read_adc              = 0;
__IO uint32_t usb_out_flag    = 0;
uint8_t usbMode               = 0;
/* Extern variables ----------------------------------------------------------*/
extern __IO uint8_t Send_Buffer[MAX_TRANSFER_LEN];


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


void NVIC_Configuration(void)
{
  /* Set the Vector Table base location at 0x3000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x3000);
}


/*******************************************************************************
* Function Name  : main.
* Description    : main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int main(void)
{
/*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32xxx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32xxx.c file
     */  
	
//	uint8_t scr = 0;
//	int32_t adc_rd1 = 0;
	uint16_t Temperature = 0;
	uint16_t Temperature1 = 0;

#ifdef USE_DFU	
   NVIC_Configuration();
#endif		
	
  // Release PB4 into GPIO
	//GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST,ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable , ENABLE);

	system_Init();
	
  while (1)
  {		
		//if(VUSB_IN_PBGetState()&&(bDeviceState!=CONFIGURED)) NVIC_SystemReset();
#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
          /* Reload IWDG counter */
    IWDG_ReloadCounter();  
#endif
    
		//??????????????????ADC_Value_Display_normal();		
		if(userProfile.user_profile.mode == PROBE_MODE_SELTEST)              // Added by Jason Chen, 2015.10.07
		   CPU_ADC_Value_SelfTest();                                         // Added by Jason Chen, 2015.10.07 
    else                                                                 // Added by Jason Chen, 2015.10.07 		
		   CPU_ADC_Value_Display();
		if(TimingDelay1 > 150)//11000)
		{
			TimingDelay1 = 0;
			LED_Toggle();
		}

		BT_Task();
		if(userProfile.user_profile.mode == PROBE_MODE_SELTEST)               // Added by Jason Chen, 2015.10.07
			Switch_process_SelTest();	                                          // Added by Jason Chen, 2015.10.07
		else                                                                  // Added by Jason Chen, 2015.10.07   
			Switch_process();	
 		
		if(CMD_ID)
		{			
		  CMD_ID = 0;
			CommandProcess();
		}	

    Time_Show();
		BatVoltage_Display_normal();
		if(VUSB_IN_PBGetState())
		{
			if(!usbMode)
			{
			  if(bDeviceState!=CONFIGURED) NVIC_SystemReset();
		  }
		}

		// Temperature Sensor Process
		if(HighTemperatureAlarm)
		{
		  Temperature1 = Temperature_Display_normal();             // Display the temperature value on LCD as well
		  if(Temperature1>0)
			  Temperature = Temperature1;
	  }
		else
		{
			if(HighTemperatureClear)
			{
				HighTemperatureClear = 0;
				TemperatureDisplayClear();
			}
		}
		// Temperature Sensor Process End

#if 1
		if((read_adc)&&(usb_out_flag))
		{
			usb_out_flag = 0;			
			if ((PrevXferComplete) && (bDeviceState == CONFIGURED))
			{
				Send_Buffer[1] = (uint8_t)0xED; 
				Send_Buffer[2] = (uint8_t)(adc_rd >> 8); 
				Send_Buffer[3] = (uint8_t)adc_rd;        
			
				Send_Buffer[4] = (uint8_t)(Temperature >> 8); 
				Send_Buffer[5] = (uint8_t)Temperature;        				
				/* Write the descriptor through the endpoint */
				USB_SIL_Write(EP1_IN, (uint8_t*) Send_Buffer, MAX_TRANSFER_LEN);  
				SetEPTxValid(ENDP1);
				PrevXferComplete = 0; 
			}
	  }
#endif
  }
}

/*******************************************************************************
* Function Name  : Delay
* Description    : Inserts a delay time.
* Input          : nCount: specifies the delay time length.
* Output         : None
* Return         : None
*******************************************************************************/
#if 0
void Delay(__IO uint32_t nCount)
{
  TimingDelay = nCount;
	while(TimingDelay!=0);
}
#elif 1

__IO uint32_t temp = 10;
#define TIME_DELAY  4500
void Delay(__IO uint32_t nCount)
{
	temp = TIME_DELAY;
  while(nCount--)
	{
#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
      /* Reload IWDG counter */
      IWDG_ReloadCounter();  
#endif  					
		while(temp--)
		{

		}
		temp = TIME_DELAY;
	}
}
#else
void Delay(__IO uint32_t nCount)
{
  TimingDelay = nCount;
  for(; nCount!= 0;nCount--);
	for(;;)
	  if(TimingDelay == 0)
			 break;
}
#endif

#ifdef  USE_FULL_ASSERT
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while(1)
  {
  }
}
#endif

/*****END OF FILE****/
