/**
  ******************************************************************************
  * @file    RTC.c
  * @author  Jason Chen
  * @version V1.0
  * @date    16-June-2014
  * @brief   
  */
	
/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "RTC.h"
#include "TLC2000_SPI_LCD_TFT.h"
#include "custom_hid_conf.h"

//#include "stm32_it.h"
//#include "usb_istr.h"
#include "usb_lib.h"
//#include "usb_pwr.h"
//#include "hw_config.h"

#include "Potentiometer.h"
#include "MiscTools.h"
#include "Buzzer.h"
#include "fonts.h"
#include "Hid_command_process.h"
#include "calibration.h"

#include "UTFT.h"
#include "UTFT_DLB.h"

#include "Switch2.h"         // Added by Jason Chen, 2015.04.09

extern uint8_t DejaVuSans18[];

/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
	

__IO int32_t new_treatment_time = 0;	
__IO int32_t treatment_time = 0;	
__IO int32_t treatment_time_interval = 0;	
__IO int32_t treatment_time_backup = 0;	
__IO uint32_t TimeDisplay = 0;
__IO uint8_t Send_Buffer[MAX_TRANSFER_LEN];
void RTC_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Configures the RTC.
  * @param  None
  * @retval None
  */
void RTC_Configuration(void)
{
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain */
  BKP_DeInit();

  /* Enable LSE */
  RCC_LSEConfig(RCC_LSE_ON);
  /* Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {
#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
          /* Reload IWDG counter */
    IWDG_ReloadCounter();  
#endif  		
		
	}

  /* Select LSE as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  /* Enable RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Enable the RTC Second */
  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Set RTC prescaler: set RTC period to 1sec */
  RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}

extern uint8_t recvCharFlag;
void RTC_Init(void)
{
	//uint32_t time_sec = 0;
 /* NVIC configuration */
  RTC_NVIC_Configuration();
	
  if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    /* Backup data register value is not correct or not yet programmed (when
       the first time the program is executed) */

    //printf("\r\n\n RTC not yet configured....");

    /* RTC Configuration */
    RTC_Configuration();

    //printf("\r\n RTC configured....");

    /* Adjust time by values entered by the user on the hyperterminal */
    //Time_Adjust();
		
    Send_Buffer[0] = REPORT_ID_5;
		Send_Buffer[1] = 0xE0;              // request setup time
    
		/* Write the descriptor through the endpoint */
    USB_SIL_Write(EP1_IN, (uint8_t*) Send_Buffer, MAX_TRANSFER_LEN);  
		SetEPTxValid(ENDP1);
    PrevXferComplete = 0;
					
		
    BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
  }
  else
  {
    /* Check if the Power On Reset flag is set */
    if (RCC_GetFlagStatus(RCC_FLAG_PORRST) != RESET)
    {
      //printf("\r\n\n Power On Reset occurred....");
    }
    /* Check if the Pin Reset flag is set */
    else if (RCC_GetFlagStatus(RCC_FLAG_PINRST) != RESET)
    {
      //printf("\r\n\n External Reset occurred....");
    }

    //printf("\r\n No need to configure RTC....");
    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();

    /* Enable the RTC Second */
    RTC_ITConfig(RTC_IT_SEC, ENABLE);
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
  }


  /* Clear reset flags */
  RCC_ClearFlag();

  /* Display time in infinite loop */
  //Time_Show();
	Time_Display(RTC_GetCounter());
}


void AdjustTime(uint8_t *dateTime)
{
	    /* Enable PWR and BKP clocks */
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

      /* Allow access to BKP Domain */
      PWR_BackupAccessCmd(ENABLE);
			
      /* Wait until last write operation on RTC registers has finished */
      RTC_WaitForLastTask();
	    
      /* Change the current time */
			//if(buttonReqFlag)
			//{
			//	buttonReqFlag = 0;				
			//	RTC_SetCounter(3600);//Receive_Buffer[2]*3600+Receive_Buffer[3]*60+Receive_Buffer[4]);
			//}
			//else
			{	
				RTC_SetCounter(dateTime[0]*3600+dateTime[1]*60+dateTime[2]);
			}
			
      /* Wait until last write operation on RTC registers has finished */
      RTC_WaitForLastTask();
			
     /* Disable PWR and BKP clocks */
     RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, DISABLE);

     /* Don't Allow access to BKP Domain */
     PWR_BackupAccessCmd(DISABLE);			
}
/**
  * @brief  Displays the current time.
  * @param  TimeVar: RTC counter value.
  * @retval None
  */
extern uint8_t displayBuff[];

void DisplayTreatmentTime(int32_t treatmentTime)
{
	  if(userProfile.user_profile.mode == PATIENT_TREATMENT_MODE)              // Added by Jason Chen, 2016.02.03
		{	
	
    uint32_t THH = 0, TMM = 0, TSS = 0;
	  uint16_t tempColor;

    THH = treatmentTime / 3600;
    TMM = (treatmentTime % 3600) / 60;
    TSS = (treatmentTime % 3600) % 60;
    sprintf((char*)displayBuff,"%0.1d:%0.2d:%0.2d", THH, TMM, TSS);
		
		tempColor = foreground_color;
		//if(treatmentTime==0)
		//	foreground_color = 0xFFFF;	  
		//else
			foreground_color = COLOR_TEAL1;//COLOR_RED1;
		
		//sLCD_putString_TFT(10,92+3*24,displayBuff,Font16x24);//displayBuff);
		sLCD_putString_TFT_ext(10,110+3*24,displayBuff, SevenSegNumFont);
		foreground_color = tempColor;		
		}
    else
			sLCD_putString_TFT_ext(10,110+3*24,(uint8_t*)"0:00:00", SevenSegNumFont);
	
}

void DisplaySelfTestTime(int32_t treatmentTime)
{
	  //if(userProfile.user_profile.mode == PATIENT_TREATMENT_MODE)              // Added by Jason Chen, 2016.02.03
		{	
	
    uint32_t THH = 0, TMM = 0, TSS = 0;
	  uint16_t tempColor;

    THH = treatmentTime / 3600;
    TMM = (treatmentTime % 3600) / 60;
    TSS = (treatmentTime % 3600) % 60;
    sprintf((char*)displayBuff,"%0.1d:%0.2d:%0.2d", THH, TMM, TSS);
		
		tempColor = foreground_color;
		//if(treatmentTime==0)
		//	foreground_color = 0xFFFF;	  
		//else
			foreground_color = COLOR_TEAL1;//COLOR_RED1;
		
		//sLCD_putString_TFT(10,92+3*24,displayBuff,Font16x24);//displayBuff);
		sLCD_putString_TFT_ext(10,110+3*24,displayBuff, SevenSegNumFont);
		foreground_color = tempColor;		
		}
    //else
		//	sLCD_putString_TFT_ext(10,110+3*24,(uint8_t*)"0:00:00", SevenSegNumFont);
	
}

uint8_t flash_flag = 0;
void Time_Display(uint32_t TimeVar)
{  
	uint16_t tempProbeColor;                          // Added by Jason Chen, 2016_0627
	//uint16_t current_color;
	//uint16_t tempColor;
 
#if 0	
  uint32_t THH = 0, TMM = 0, TSS = 0;	
 
  /* Compute  hours */
  THH = TimeVar / 3600;
  /* Compute minutes */
  TMM = (TimeVar % 3600) / 60;
  /* Compute seconds */
  TSS = (TimeVar % 3600) % 60;
  
	THH = THH%24;
  sprintf((char*)displayBuff,"%0.2d:%0.2d:%0.2d", THH, TMM, TSS);
#else	
	sprintf((char*)displayBuff,"         ");
#endif	

  /* Reset RTC Counter when Time is 23:59:59 */
  if (RTC_GetCounter() == 0x0001517F)
  {
     RTC_SetCounter(0x0);
     /* Wait until last write operation on RTC registers has finished */
     RTC_WaitForLastTask();
  }
	
	//setColor(foreground_color);                                     // Added by Jason Chen, 2015.03.05
	tempProbeColor = background_color;                                // Added by Jason Chen, 2016_0627
	background_color = ColorArray[userProfile.user_profile.foreColor];// Added by Jason Chen, 2016_0627
  sLCD_putString_TFT_ext(10,3,displayBuff, hallfetica_normal16x16);//Arial_Normal16x16);//DotMatrix16x22);//Inola24x32);//SevenSegNumFont);	
	background_color = tempProbeColor;                                // Added by Jason Chen, 2016_0627
  //setFont(DejaVuSans18);		
  //print_ttf((char*)displayBuff, 10, 3, 0);			
		
	if((treatment_time>=0)&&(!Pause_flag))          // Modified by Jason Chen, 2015.04.09
	//if(treatment_time>=0)
	{
    DisplayTreatmentTime(treatment_time);
#if 0		
		DisplayTreatmentTime(new_treatment_time);
				
		treatment_time_interval++;		
		if(treatment_time_interval>0 && treatment_time_interval<=180)
		{
			 //if(!treatment_on_flag)
			 {	
				 Display_LaserOn();
         if(userProfile.user_profile.mode == PATIENT_TREATMENT_MODE)
				 {            
           Display_TENsOn();
			     TreatmentOn();
				 
           //BuzzerOn();
				   //Delay(30);
				   //BuzzerOff();
				}
				else
			  {
					MeasurementOn();
				}	
			 }
			 if(treatment_time_interval == 180)
				 new_treatment_time = 60;
		}
		else if(treatment_time_interval>180 && treatment_time_interval<=240)
		{
			 //if(treatment_on_flag)
			 {
			   Display_LaserOff();
				 TreatmentOff();
				 if(userProfile.user_profile.mode == PATIENT_TREATMENT_MODE)
				 {	 
           Display_TENsOff();				 
         //BuzzerOn();
				 //Delay(30);
				 //BuzzerOff();				 
				 }
			 }
			 if(treatment_time_interval == 240)
				 new_treatment_time = 180;			 
		}
		else
		{
			 treatment_time_interval = 0;
			 //if(!treatment_on_flag)
			 {
         Display_LaserOn();
         if(userProfile.user_profile.mode == PATIENT_TREATMENT_MODE)
				 {				 
           Display_TENsOn();
			     TreatmentOn();
				 
           //BuzzerOn();
				   //Delay(30);
				   //BuzzerOff();				 
				 }
				 else
			   {
					 MeasurementOn();
				 }					 
			 }
		}
#endif		
		//if(userProfile.user_profile.p905DAC0Voltage > 0)
	  if((userProfile.user_profile.p905DAC0Voltage > 0)||
		   (userProfile.user_profile.p905DAC1Voltage > 0)||
	     (userProfile.user_profile.p905DAC2Voltage > 0)||
	     (userProfile.user_profile.p905DAC3Voltage > 0)||
	     (userProfile.user_profile.p905DAC4Voltage > 0)||
	     (userProfile.user_profile.p905DAC5Voltage > 0))				
		{		
      if(userProfile.user_profile.mode == PATIENT_TREATMENT_MODE)              // Added by Jason Chen, 2016.02.03
			{
				if(flash_flag)
				{ 
					flash_flag = 0;
					Display_square(180+3*12+6, 156-3*24+3-12, background_color,Font16x24);
				}
				else
				{
					flash_flag = 1;
					Display_square(180+3*12+6, 156-3*24+3-12, COLOR_RED1,Font16x24);
				}
		  }
	  }
		else if(userProfile.user_profile.pTensDACVoltage  > 0)
		{
      if(userProfile.user_profile.mode == PATIENT_TREATMENT_MODE)              // Added by Jason Chen, 2016.02.03
			{
				if(flash_flag)
				{ 
					flash_flag = 0;
					Display_square(180+3*12+6, 156-3*24+3-12, background_color,Font16x24);
				}
				else
				{
					flash_flag = 1;
					Display_square(180+3*12+6, 156-3*24+3-12, COLOR_RED1,Font16x24);
				}	
		  }			
		}
		else
		{
				Display_square(180+3*12+6, 156-3*24+3-12, background_color,Font16x24);
		}
		
		if((treatment_time>0)&&(treatment_time<=10))
		{
			if(userProfile.user_profile.mode == PATIENT_TREATMENT_MODE)              // Added by Jason Chen, 2016.02.03
			{
#if USE_TIM8_PWM			
			if(TurnOnBuzzer)
			  BuzzerOn();                           // Added by Jason Chen, 2014.11.25
			else
			  BuzzerOff();			
#else
			if(TurnOnBuzzer)
				BuzzerOff();                           // Added by Jason Chen, 2014.11.25
			else
			  BuzzerOn();			
#endif		
		  }			
		}
		else 
		{
			BuzzerOff();                             // Added by Jason Chen, 2014.11.25
			if(treatment_time == 0)
			{
				treatment_time_backup = userProfile.user_profile.treatment_time;
				//Display_square(180, 156, background_color,Font16x24);
				//Display_square(180, 156-24, background_color,Font16x24);
				Display_square(180+3*12+6, 156-3*24+3-12, background_color,Font16x24);				
				LED_Off();
				//Send_Buffer[1] = 0xFB;
				//Usb_Send();
				//TreatmentOff();
			}
		}
	}
	
	Display_BT_RSSI(RSSI);    // 2014.10.17
}

/**
  * @brief  Shows the current time (HH:MM:SS) on the Hyperterminal.
  * @param  None
  * @retval None
  */   
void Time_Show(void)
{
  //printf("\n\r");

  /* Infinite loop */
  //while (1)
  {
    /* If 1s has been elapsed */
    if (TimeDisplay == 1)
    {
      /* Display current time */
      Time_Display(RTC_GetCounter());
      TimeDisplay = 0;
			
    }
  }
}










//============================================================================

