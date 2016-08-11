/**
  ******************************************************************************
  * @file    Switch2.c
  * @author  Jason Chen
  * @version V1.0
  * @date    19-Oct-2014
  * @brief   
  **/ 

/* Includes ------------------------------------------------------------------*/
#include "TLC2000_SPI_LCD_TFT.h"
#include "Switch2.h"
#include "MiscTools.h"
#include "Buzzer.h"
#include "RTC.h"
#include "Hid_command_process.h"
#include "calibration.h"                
#include "tBT.h"

#include "CPU_ADC.h"

/**
  * @brief  Initializes switch Button
  * @param  None
  * @retval None
  */

__IO uint32_t SleepTimer_TickCount= 0;
__IO uint32_t SWITCH2_Timer       = 0;
     uint32_t SWITCH_HoldTimer    = 0;                           // Added by Jason Chen, 2016.08.05                      
__IO uint32_t selfTest_cnt        = 0;                           // Added by Jason Chen, 2015.10.07
__IO uint8_t  stopFlag            = 1;                           // Added by Jason Chen, 2015.10.07
__IO uint8_t  Pause_flag          = 0;                           // Added by Jason Chen, 2015.04.09
uint8_t       switch_stm_status   = SWITCH_INIT;
uint8_t       switch_stm_selfTest = SWITCH_INIT;                 // Added by Jason Chen, 2015.10.07

void Switch2_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
//EXTI_InitTypeDef EXTI_InitStructure;
//NVIC_InitTypeDef NVIC_InitStructure;
    
  /*!< sEE_I2C_SCL_GPIO_CLK and sEE_I2C_SDA_GPIO_CLK Periph clock enable */
	RCC_APB2PeriphClockCmd(SW_GPIO_CLK, ENABLE);                  // 2014.10.20
			
  GPIO_InitStructure.GPIO_Pin = SW_PIN;                         // 2014.10.20
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;         //GPIO_Mode_IPD;//GPIO_Mode_IN_FLOATING;         // 2014.10.20
  GPIO_Init(SW_GPIO_PORT, &GPIO_InitStructure);	                // 2014.10.20

#if 0                                                           // 2014.10.20
	GPIO_EXTILineConfig(SW_EXTI_PORT_SOURCE, SW_EXTI_PIN_SOURCE); // 2014.10.20

	/* Configure Button EXTI line */
	EXTI_InitStructure.EXTI_Line = SW_EXTI_LINE;                  // 2014.10.20
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;           // 2014.10.20
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;        //EXTI_Trigger_Falling;       // 2014.10.20
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;                     // 2014.10.20
	EXTI_Init(&EXTI_InitStructure);                               // 2014.10.20
				
	/* Clear the Tamper EXTI line pending bit */
	EXTI_ClearITPendingBit(SW_EXTI_LINE);		                      // 2014.10.20

	/* Enable and set Button EXTI Interrupt to the lowest priority */  
	NVIC_InitStructure.NVIC_IRQChannel = SW_EXTI_IRQn;            // 2014.10.20
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;  // 2014.10.20
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;         // 2014.10.20
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;               // 2014.10.20

	NVIC_Init(&NVIC_InitStructure);                               // 2014.10.20
#endif

}

uint32_t SW_Int_PBGetStateOn(void)
{
	if(GPIO_ReadInputDataBit(SW_GPIO_PORT, SW_PIN ))
		return 1;
	else
		return 0;
}

uint8_t GetButtonValue_click(void)
{
    if(SW_Int_PBGetStateOn())
    {
      Delay(45);
      if(SW_Int_PBGetStateOn())
      {
        while(SW_Int_PBGetStateOn())
				{
#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
          /* Reload IWDG counter */
          IWDG_ReloadCounter();  
#endif  					
				}
        return 1;
      }
    }
    return 0;
}

uint8_t ButtonHoldValue(void)
{	
	  //uint32_t cnt;
    if(SW_Int_PBGetStateOn())
    {
      Delay(45);
      if(SW_Int_PBGetStateOn())
      {
				Process_TimeSet(&SWITCH_HoldTimer, 5000);
				//cnt = SleepTimer_TickCount;
				//DisplayOneNumber(cnt);
        while(SW_Int_PBGetStateOn())
				{
#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
          /* Reload IWDG counter */
          IWDG_ReloadCounter();  
#endif  		          
					if(Process_TimeExpired(&SWITCH_HoldTimer))
					{
						while(SW_Int_PBGetStateOn())
						{
             #ifdef ENABLE_iWATCHDOG
               /* Reload IWDG counter */
               IWDG_ReloadCounter();  
             #endif  		          
						}
						return 5;
					}
					Delay(50);
					//DisplayOneNumber((SleepTimer_TickCount-cnt)/1000);
				}
        return 1;
      }
    }
    return 0;
}

void Process_TimeSet(
    uint32_t *pTimer,           // Ptr to store expiration time
    uint32_t time               // 32767 counts maximum
    ) 
{
    uint32_t tmpTime;

    tmpTime = SleepTimer_TickCount;

    *pTimer = (uint32_t)(tmpTime + time);
    if( *pTimer == 0 )
        *pTimer = 1;
}

uint8_t Process_TimeExpired(     // Ret 1 if expired
    uint32_t *pTimer             // ptr to expiration time
    )                           
{
    uint32_t tmpTime;

    if( *pTimer == 0 )
        return 1;

    tmpTime = SleepTimer_TickCount;
    tmpTime = (uint32_t)(*pTimer - tmpTime);

    if (((uint8_t)(tmpTime>>8)&0x80) == 0)
        return 0;    // not expired

    *pTimer = 0;
    return 1;    // expired
}


void Switch_process(void)
{
	uint8_t sw_status;
	switch(switch_stm_status)
	{
		case SWITCH_INIT:
		  Switch2_Init();
		  switch_stm_status = SWITCH_WAIT_PRESS;
		  Display_Prompt(PROMPT_EMPTY);
		  break;
		case SWITCH_WAIT_PRESS:
		//if(GetButtonValue_click()&&armed_flag)		
		  sw_status = ButtonHoldValue();
		  if(sw_status == 1)
			{
				if(!armed_flag) break;
				if(treatment_on_flag) break;
#if 0				
				//LED_Toggle();	
//				button_press = 0;
				//Process_TimeSet(&SWITCH2_Timer, 2000);
				BuzzerOn();
				SWITCH2_Timer = 0;
				switch_stm_status = SWITCH_DELAY;		
                treatment_time = -1;		
				//BuzzerOff();
				Display_Prompt(PROMPT_WAIT);
#else
				BuzzerOn();
				SWITCH2_Timer = 0;
				switch_stm_status = SWITCH_START;		
			  treatment_time_backup = userProfile.user_profile.treatment_time;
				Delay(30);
				BuzzerOff();	
        Pause_flag = 0;
#endif				
			}
			else if(sw_status == 5)
			{							
				  PROBE_Mode(PROBE_MODE_SELTEST,0);                           // Added by Jason Chen, 2016.08.05				
				  Self_Cali_Clr();
					DisplaySelfTestTime(0);
				  
			}
			break;
		case SWITCH_DELAY:
			//if(Process_TimeExpired(&SWITCH2_Timer))
			if(SWITCH2_Timer > 2000)	
			{
				Display_Prompt(PROMPT_START);
				if(GetButtonValue_click()&&(SWITCH2_Timer < 10000))
				{
					BuzzerOn();
					switch_stm_status = SWITCH_START;
					treatment_time_backup = userProfile.user_profile.treatment_time;
					Delay(30);
					BuzzerOff();	
          Pause_flag = 0;
				}
				else
				{
					if(SWITCH2_Timer > 10000)
					{						
				    //switch_stm_status = SWITCH_WAIT_PRESS;//SWITCH_DELAY;
						switch_stm_status = SWITCH_INIT;
					}
				}
			}
			else
			{
				BuzzerOff();
			}
			break;			
		case SWITCH_START:	  
			if(armed_flag)//else
			{							
        Send_Buffer[1] = 0xFC;
				Usb_Send();				
			  treatment_time = treatment_time_backup;//userProfile.user_profile.treatment_time;
				switch_stm_status = SWITCH_S2DELAY;//SWITCH_RUNNING;
				if(userProfile.user_profile.mode == PATIENT_TREATMENT_MODE)
				  TreatmentOn();
				else
					MeasurementOn();
				SWITCH2_Timer = 0;
				
				new_treatment_time = 180;
				
				Display_Prompt(PROMPT_EMPTY);				
			}		
			//else
				
			break;
		case SWITCH_S2DELAY:
			if(SWITCH2_Timer > 500)
				switch_stm_status = SWITCH_RUNNING;
			break;
		case SWITCH_RUNNING:
        //if(userProfile.user_profile.mode == PATIENT_TREATMENT_MODE)	
				if(GetButtonValue_click())
				{
					BuzzerOn();
					if(userProfile.user_profile.mode == PATIENT_TREATMENT_MODE)
					{
					  Pause_flag = 1;
					  TreatmentPauseOff();
					  switch_stm_status = SWITCH_PAUSE;
					  Display_Pause();
						Display_square(180+3*12+6, 156-3*24+3-12, background_color,Font16x24);				
					}
					else if(userProfile.user_profile.mode == PATIENT_MEASUREMENT_MODE)
					{
				    switch_stm_status = SWITCH_STOP;
				    treatment_time = -1;										
					}
					BuzzerOff();
				}
				else
				{
					if(treatment_time == 0)
					{
						BuzzerOn();
						switch_stm_status = SWITCH_STOP;//SWITCH_WAIT_PRESS;
						TreatmentOff();
					}
				}
			break;
		case SWITCH_PAUSE:
			if(GetButtonValue_click())
			{
				BuzzerOn();
				//Pause_flag = 0;
				//ReTreatmentOn();
				SWITCH2_Timer = 0;
				//switch_stm_status = SWITCH_S2DELAY;
				switch_stm_status = SWITCH_WAIT_PRESS2;
				//BuzzerOff();
			}					
			break;
		case SWITCH_WAIT_PRESS2:
    //if(GetButtonValue_click())//&&armed_flag)
			{
#if 0				
				SWITCH2_Timer = 0;
				switch_stm_status = SWITCH_DELAY2;		
				BuzzerOn();
				Display_Prompt(PROMPT_WAIT);
#else
				BuzzerOn();
				switch_stm_status = SWITCH_START2;
				Delay(30);
				BuzzerOff();	
        Pause_flag = 0;				
#endif				
			}
			break;	
		case SWITCH_DELAY2:
			if(SWITCH2_Timer > 2000)	
			{
				Display_Prompt(PROMPT_START);
				if(GetButtonValue_click()&&(SWITCH2_Timer < 10000))
				{
					BuzzerOn();
					switch_stm_status = SWITCH_START2;
					Delay(30);
					BuzzerOff();	
          Pause_flag = 0;
				}
				else
				{
					if(SWITCH2_Timer > 10000)
					{						
				    //switch_stm_status = SWITCH_WAIT_PRESS;//SWITCH_DELAY;
						switch_stm_status = SWITCH_INIT;
					}
				}
			}
			else
				BuzzerOff();
			break;		
		case SWITCH_START2:	  
			{							
        Send_Buffer[1] = 0xFC;
				Usb_Send();				
				switch_stm_status = SWITCH_S2DELAY;//SWITCH_RUNNING;
				if(userProfile.user_profile.mode == PATIENT_TREATMENT_MODE)
				  ReTreatmentOn();
				else
					MeasurementOn();
				SWITCH2_Timer = 0;								
				Display_Prompt(PROMPT_EMPTY);				
			}						
			break;			
		case SWITCH_STOP:
			new_treatment_time = 0;		  
			TreatmentOff();
      Send_Buffer[1] = 0xFB;
      Usb_Send();	
		  BuzzerOff();
		  DisplayTreatmentTime(userProfile.user_profile.treatment_time);// 2014.10.28
		  Display_square(180+3*12+6, 156-3*24+3-12, background_color,Font16x24);
			switch_stm_status = SWITCH_WAIT_PRESS;
			break;
		case SWITCH_NOP:			
			break;		
    default:
			switch_stm_status = SWITCH_WAIT_PRESS;
      break;			
	}
	//DisplayTestOffset(6, switch_stm_status);
}

void Switch_process_SelTest(void)
{
	static int8_t redFlip;
	switch(switch_stm_selfTest)
	{
		case SWITCH_INIT:
		  Switch2_Init();
		  switch_stm_selfTest = SWITCH_WAIT_PRESS;
		  //Display_Prompt(PROMPT_EMPTY);
		  Display_Prompt(PROMPT_CALI);
		  break;
		case SWITCH_WAIT_PRESS:
		  if(GetButtonValue_click())//&&armed_flag)
			{
				if(treatment_on_flag) break;
				BuzzerOn();
				SWITCH2_Timer = 0;
				switch_stm_selfTest = SWITCH_DELAY;		
        treatment_time = -1;		
				Display_Prompt(PROMPT_WAIT);				
			}
			break;
		case SWITCH_DELAY:
			if(SWITCH2_Timer > 2000)	
			{
				Display_Prompt(PROMPT_START);
				if(GetButtonValue_click()&&(SWITCH2_Timer < 10000))
				{
					BuzzerOn();
					switch_stm_selfTest = SWITCH_START;
					//treatment_time_backup = userProfile.user_profile.treatment_time;
					Delay(30);
					BuzzerOff();	
					//Self_Cali_Clr();
					//DisplaySelfTestTime(0);
				}
				else
				{
					if(SWITCH2_Timer > 10000)
					{						
						switch_stm_selfTest = SWITCH_INIT;
					}
				}
			}
			else
			{
				BuzzerOff();
			}
			break;			
		case SWITCH_START:	  
			//if(armed_flag)//else
			{							
        //Send_Buffer[1] = 0xFC;
				//Usb_Send();				
				switch_stm_selfTest = SWITCH_S2DELAY;//SWITCH_RUNNING;
				Active_SelfTest();
				Display_square(222, 157, COLOR_RED1,Font16x24);
				SWITCH2_Timer = 0;
        selfTest_cnt = 0;
				stopFlag = 0;
				Display_Prompt(PROMPT_EMPTY);				
			}						
			break;
		case SWITCH_S2DELAY:			
			if(SWITCH2_Timer > 500)
			{
				switch_stm_selfTest = SWITCH_RUNNING;
				SWITCH2_Timer = 0;
				//Display_SlefTestMode();				
			}
			break;
		case SWITCH_RUNNING:
				if(GetButtonValue_click())
				{
					BuzzerOn();
				  DeActive_SelfTest();
				  switch_stm_selfTest = SWITCH_PAUSE;
				  Display_Prompt(PROMPT_PAUSE);
					BuzzerOff();
          Display_square(222, 157, background_color,Font16x24);					
					stopFlag = 1;
				}
				else
				{
					if(SWITCH2_Timer > 500)
					{
						SWITCH2_Timer = 0;
						if(redFlip) Display_square(222, 157 - 24*(laser_sel), COLOR_RED1,Font16x24);
						else        Display_square(222, 157 - 24*(laser_sel), background_color,Font16x24);
						redFlip = !redFlip;						
						DisplaySelfTestTime(selfTest_cnt);
					}					
				}
			break;
		case SWITCH_PAUSE:
			if(GetButtonValue_click())
			{
				BuzzerOn();
				SWITCH2_Timer = 0;
				switch_stm_selfTest = SWITCH_WAIT_PRESS2;
			}					
			break;
		case SWITCH_WAIT_PRESS2:
			SWITCH2_Timer = 0;
			switch_stm_selfTest = SWITCH_DELAY2;		
			BuzzerOn();
			Display_Prompt(PROMPT_WAIT);
			break;	
		case SWITCH_DELAY2:
			if(SWITCH2_Timer > 2000)	
			{
				Display_Prompt(PROMPT_START);
				if(GetButtonValue_click()&&(SWITCH2_Timer < 10000))
				{
					BuzzerOn();
					switch_stm_selfTest = SWITCH_START2;
					Delay(30);
					BuzzerOff();	
				}
				else
				{
					if(SWITCH2_Timer > 10000)
					{						
						switch_stm_selfTest = SWITCH_INIT;
					}
				}
			}
			else
				BuzzerOff();
			break;		
		case SWITCH_START2:	  
			{							
        //Send_Buffer[1] = 0xFC;
				//Usb_Send();				
				switch_stm_selfTest = SWITCH_S2DELAY;//SWITCH_RUNNING;
        Active_SelfTest();
				Display_square(222, 157-24*(laser_sel), COLOR_RED1,Font16x24);
				SWITCH2_Timer = 0;								
				stopFlag = 0;
				Display_Prompt(PROMPT_EMPTY);								
			}						
			break;			
		case SWITCH_STOP:
			DeActive_SelfTest();
      //Send_Buffer[1] = 0xFB;
      //Usb_Send();								
		  BuzzerOff();
		  Display_square(222,157-24*(laser_sel), background_color,Font16x24);
			switch_stm_selfTest = SWITCH_WAIT_PRESS;
		  Display_Prompt(PROMPT_CALI);
			break;
		case SWITCH_NOP:			
			break;		
    default:
			switch_stm_selfTest = SWITCH_WAIT_PRESS;
      break;			
	}
}


