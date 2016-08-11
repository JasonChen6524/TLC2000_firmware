/**
  ******************************************************************************
  * @file    Hid_command_process.c
  * @author  Jason Chen
  * @version V1.0
  * @date    12-August-2014
  * @brief   
  *            
  */ 

/* Includes ------------------------------------------------------------------*/
#include <string.h>
//#include <stdio.h>
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_pwr.h"

#include "Hid_command_process.h"
#include "custom_hid_conf.h"
#include "RTC.h"
//#include "AD5504_DAC.h"
#include "DAC7718.h"
#include "FPGA_RAM.h"
#include "Potentiometer.h"
#include "MiscTools.h"
#include "ADS8319_ADC.h"
#include "calibration.h"
#include "TLC2000_SPI_LCD_TFT.h"
#include "tBT.h"
#include "Switch2.h"
#include "Buzzer.h"

#include "CPU_ADC.h"

extern __IO BLE_CONFIG pcCmd; 
extern uint8_t read_adc;
extern __IO uint8_t Send_Buffer[MAX_TRANSFER_LEN];
__IO uint8_t treatment_on_flag = 0;

__IO uint8_t treatment_off_flag = 0;
__IO uint32_t Treatment_Count = 0;           // Added by Jason Chen, 2015.04.09


__IO uint8_t FeedBack905Test_Flag = 0;
__IO uint8_t FeedBack905_Flag = 0;
__IO uint16_t AutoTestValue[2];
__IO uint16_t AutoTestTimeInterval =500;

uint16_t voltage_s = 0;                                                 // Added by Jason Chen, 2015.12.14

#define DAC_MAX_VALUE     4095

void PROBE_Idle(void);

void Calibration_setup(uint8_t ch_sel,uint16_t voltage)               // Added by Jason Chen, 2015.09.17
{
	//uint16_t maxPower;	 
	Power_V30_Off();
	
	DAC7718_OUT_VOLT(s905_DAC0,0);	Delay(10);
	DAC7718_OUT_VOLT(s905_DAC1,0);	Delay(10);
	DAC7718_OUT_VOLT(s905_DAC2,0);	Delay(10);
	DAC7718_OUT_VOLT(s905_DAC3,0);	Delay(10);
	DAC7718_OUT_VOLT(s905_DAC4,0);	Delay(10);
	
	Power_VIN660_Off();
	S660_DAC_OUT_VOLT(s660_DAC0,0);	Delay(10);
	S660_DAC_OUT_VOLT(s660_DAC1,0);	Delay(10);
	S660_DAC_OUT_VOLT(s660_DAC2,0);	Delay(10);
	S660_DAC_OUT_VOLT(s660_DAC3,0);	Delay(10);
	
	if     (ch_sel == 1) {Power_V30_On();DAC7718_OUT_VOLT(s905_DAC0,voltage);}//3052);
	else if(ch_sel == 2) {Power_V30_On();DAC7718_OUT_VOLT(s905_DAC1,voltage);}//3052);
	else if(ch_sel == 3) {Power_V30_On();DAC7718_OUT_VOLT(s905_DAC2,voltage);}//3052);
	else if(ch_sel == 4) {Power_V30_On();DAC7718_OUT_VOLT(s905_DAC3,voltage);}//3052);
	else if(ch_sel == 5) {Power_V30_On();DAC7718_OUT_VOLT(s905_DAC4,voltage);}//3052);
	else if(ch_sel == 6) {Power_VIN660_On();S660_DAC_OUT_VOLT(s660_DAC0,voltage);}//3276);
	else if(ch_sel == 7) {Power_VIN660_On();S660_DAC_OUT_VOLT(s660_DAC1,voltage);}//3276);
	else if(ch_sel == 8) {Power_VIN660_On();S660_DAC_OUT_VOLT(s660_DAC2,voltage);}//3276);
	else if(ch_sel == 9) {Power_VIN660_On();S660_DAC_OUT_VOLT(s660_DAC3,voltage);}//3276);
	else
	{
		Power_V30_Off();	
	  Power_VIN660_Off();              
		return;
	}
			
	Delay(10);
			
	FPGA_ENABLE();
	Delay(10);
  Reset_All_Registers_FPGA();                                             // Added by Jason Chen, 2015.02.23 according to request			
	Delay(10);
		
	Set_905_pulse(PULSE_WIDTH_200, FREQ_10000HZ);	Delay(10);      
	Set_660_pulse(PULSE_WIDTH_50,  FREQ_10000HZ); Delay(10);
		
  //Set_cadence_freq_duty(CADENCE_FREQ_905,(uint8_t)((userProfile.user_profile.p905CadenceDutyFreq >> 8)&0x00FF),(uint8_t)(userProfile.user_profile.p905CadenceDutyFreq&0x00FF));
	//Set_cadence_freq_duty(CADENCE_FREQ_660,(uint8_t)((userProfile.user_profile.p660CadenceDutyFreq >> 8)&0x00FF),(uint8_t)(userProfile.user_profile.p660CadenceDutyFreq&0x00FF));		 
	Display_LaserOn();			
}

#define DISPLAY_905
uint8_t SelfTest_adj(int16_t laser_sel, int16_t adj_value)                                 // Added by Jason Chen, 2016.08.10
{
	int32_t tempVolt;
	int32_t tempVolt1;
	int32_t tempVolt2;
	uint8_t retValue = 0;
	
	Power_VIN660_Off();
	S660_DAC_OUT_VOLT(s660_DAC0,0);	Delay(10);
	S660_DAC_OUT_VOLT(s660_DAC1,0);	Delay(10);
	S660_DAC_OUT_VOLT(s660_DAC2,0);	Delay(10);
	S660_DAC_OUT_VOLT(s660_DAC3,0);	Delay(10);
	
	//DAC7718_OUT_VOLT(s905_DAC0,0);	Delay(10);
	//DAC7718_OUT_VOLT(s905_DAC1,0);	Delay(10);
	//DAC7718_OUT_VOLT(s905_DAC2,0);	Delay(10);
	//DAC7718_OUT_VOLT(s905_DAC3,0);	Delay(10);
	//DAC7718_OUT_VOLT(s905_DAC4,0);	Delay(10);
	
	Power_V30_On();	
	if(laser_sel == 0)
	{		
		tempVolt   = (int32_t)2720;		
    tempVolt2  = (int32_t)probeOffset.probe_offset.p905DAC0VoltOffset;
		tempVolt2 += (int32_t)adj_value;
		
		tempVolt1 = tempVolt2;
		probeOffset.probe_offset.p905DAC0VoltOffset = (int16_t)tempVolt1;
		
		tempVolt += tempVolt2;
	  if(tempVolt < 0)
		{
		  tempVolt = 0;
			retValue = 1;
		}
	  if(tempVolt > DAC_MAX_VALUE)
		{
			tempVolt = DAC_MAX_VALUE;		
			retValue = 2;
		}
		//userProfile.user_profile.p905DAC0Voltage = (uint16_t)tempVolt;
		#ifdef DISPLAY_905
		Display905Voltage(laser_sel+1, (uint16_t)tempVolt);
		#else
		DisplayOffsetVoltage(laser_sel+1, (int16_t)tempVolt1);
		#endif
	  DAC7718_OUT_VOLT(s905_DAC0,(uint16_t)tempVolt); Delay(10);
	}
	else if(laser_sel == 1) 
	{
		tempVolt   = (int32_t)2720;		
    tempVolt2  = (int32_t)probeOffset.probe_offset.p905DAC1VoltOffset;
		tempVolt2 += (int32_t)adj_value;
		
		tempVolt1 = tempVolt2;
		probeOffset.probe_offset.p905DAC1VoltOffset = (int16_t)tempVolt1;
		
		tempVolt += tempVolt2;
	  if(tempVolt < 0)
		{
		  tempVolt = 0;
			retValue = 1;
		}
	  if(tempVolt > DAC_MAX_VALUE)
		{
			tempVolt = DAC_MAX_VALUE;		
			retValue = 2;
		}
		//userProfile.user_profile.p905DAC0Voltage = (uint16_t)tempVolt;
		#ifdef DISPLAY_905
		Display905Voltage(laser_sel+1, (uint16_t)tempVolt);
		#else
		DisplayOffsetVoltage(laser_sel+1, (int16_t)tempVolt1);
		#endif
	  DAC7718_OUT_VOLT(s905_DAC1,(uint16_t)tempVolt);		
	}
	else if(laser_sel == 2)
	{
		tempVolt   = (int32_t)2720;		
    tempVolt2  = (int32_t)probeOffset.probe_offset.p905DAC2VoltOffset;
		tempVolt2 += (int32_t)adj_value;
		
		tempVolt1 = tempVolt2;
		probeOffset.probe_offset.p905DAC2VoltOffset = (int16_t)tempVolt1;
		
		tempVolt += tempVolt2;
	  if(tempVolt < 0)
		{
		  tempVolt = 0;
			retValue = 1;
		}
	  if(tempVolt > DAC_MAX_VALUE)
		{
			tempVolt = DAC_MAX_VALUE;		
			retValue = 2;
		}
		//userProfile.user_profile.p905DAC0Voltage = (uint16_t)tempVolt;
		#ifdef DISPLAY_905
		Display905Voltage(laser_sel+1, (uint16_t)tempVolt);
		#else
		DisplayOffsetVoltage(laser_sel+1, (int16_t)tempVolt1);
		#endif
	  DAC7718_OUT_VOLT(s905_DAC2,(uint16_t)tempVolt);		
	}
	else if(laser_sel == 3) 
	{
		tempVolt   = (int32_t)2720;		
    tempVolt2  = (int32_t)probeOffset.probe_offset.p905DAC3VoltOffset;
		tempVolt2 += (int32_t)adj_value;
		
		tempVolt1 = tempVolt2;
		probeOffset.probe_offset.p905DAC3VoltOffset = (int16_t)tempVolt1;
		
		tempVolt += tempVolt2;
	  if(tempVolt < 0)
		{
		  tempVolt = 0;
			retValue = 1;
		}
	  if(tempVolt > DAC_MAX_VALUE)
		{
			tempVolt = DAC_MAX_VALUE;		
			retValue = 2;
		}
		//userProfile.user_profile.p905DAC0Voltage = (uint16_t)tempVolt;
		#ifdef DISPLAY_905
		Display905Voltage(laser_sel+1, (uint16_t)tempVolt);
		#else
		DisplayOffsetVoltage(laser_sel+1, (int16_t)tempVolt1);
		#endif
	  DAC7718_OUT_VOLT(s905_DAC3,(uint16_t)tempVolt);		
	}
	else if(laser_sel == 4) 
	{
		tempVolt   = (int32_t)2590;
    tempVolt2  = (int32_t)probeOffset.probe_offset.p905DAC4VoltOffset;
		tempVolt2 += (int32_t)adj_value;
		
		tempVolt1 = tempVolt2;
		probeOffset.probe_offset.p905DAC4VoltOffset = (int16_t)tempVolt1;
		
		tempVolt += tempVolt2;
	  if(tempVolt < 0)
		{
		  tempVolt = 0;
			retValue = 1;
		}
	  if(tempVolt > DAC_MAX_VALUE)
		{
			tempVolt = DAC_MAX_VALUE;		
			retValue = 2;
		}
		//userProfile.user_profile.p905DAC0Voltage = (uint16_t)tempVolt;
		#ifdef DISPLAY_905
		Display905Voltage(laser_sel+1, (uint16_t)tempVolt);
		#else
		DisplayOffsetVoltage(laser_sel+1, (int16_t)tempVolt1);
		#endif
	  DAC7718_OUT_VOLT(s905_DAC4,(uint16_t)tempVolt);		
	}		
	else
	{
		//Power_V30_Off();	
	  //Power_VIN660_Off();              
		return 0xFF;                    // Channnel selection error
	}
	return retValue;
			
}

void SelfTest_setup(int16_t laser_sel)                                 // Added by Jason Chen, 2015.10.07
{
	int32_t tempVolt;
	int32_t tempVolt2;
	Power_V30_On();
	
	DAC7718_OUT_VOLT(s905_DAC0,0);	Delay(10);
	DAC7718_OUT_VOLT(s905_DAC1,0);	Delay(10);
	DAC7718_OUT_VOLT(s905_DAC2,0);	Delay(10);
	DAC7718_OUT_VOLT(s905_DAC3,0);	Delay(10);
	DAC7718_OUT_VOLT(s905_DAC4,0);	Delay(10);
	
	Power_VIN660_Off();              
	S660_DAC_OUT_VOLT(s660_DAC0,0);	Delay(10);
	S660_DAC_OUT_VOLT(s660_DAC1,0);	Delay(10);
	S660_DAC_OUT_VOLT(s660_DAC2,0);	Delay(10);
	S660_DAC_OUT_VOLT(s660_DAC3,0);	Delay(10);
	
	if(laser_sel == 0)
	{
		tempVolt  = (int32_t)2720;
    tempVolt2 = (int32_t)probeOffset.probe_offset.p905DAC0VoltOffset;     // Changed from Jack, 2016.08.07
		tempVolt += tempVolt2;
	  if(tempVolt < 0)
		  tempVolt = 0;
	  if(tempVolt > DAC_MAX_VALUE)
			tempVolt = DAC_MAX_VALUE;		
	  DAC7718_OUT_VOLT(s905_DAC0,(uint16_t)tempVolt);
	}
	else if(laser_sel == 1) 
	{
		tempVolt  = (int32_t)2720;
    tempVolt2 = (int32_t)probeOffset.probe_offset.p905DAC1VoltOffset ;     // Changed from Jack, 2016.08.07
		tempVolt += tempVolt2;		
	  if(tempVolt < 0)
		  tempVolt = 0;
	  if(tempVolt > DAC_MAX_VALUE)
			tempVolt = DAC_MAX_VALUE;		
	  DAC7718_OUT_VOLT(s905_DAC1,(uint16_t)tempVolt);		
	}
	else if(laser_sel == 2)
	{
		tempVolt  = (int32_t)2720;
    tempVolt2 = (int32_t)probeOffset.probe_offset.p905DAC2VoltOffset ;     // Changed from Jack, 2016.08.07
		tempVolt += tempVolt2;		
	  if(tempVolt < 0)
		  tempVolt = 0;
	  if(tempVolt > DAC_MAX_VALUE)
			tempVolt = DAC_MAX_VALUE;		
	  DAC7718_OUT_VOLT(s905_DAC2,(uint16_t)tempVolt);		
	}
	else if(laser_sel == 3) 
	{
		tempVolt  = (int32_t)2720;
    tempVolt2 = (int32_t)probeOffset.probe_offset.p905DAC3VoltOffset ;     // Changed from Jack, 2016.08.07
		tempVolt += tempVolt2;		
	  if(tempVolt < 0)
		  tempVolt = 0;
	  if(tempVolt > DAC_MAX_VALUE)
			tempVolt = DAC_MAX_VALUE;		
	  DAC7718_OUT_VOLT(s905_DAC3,(uint16_t)tempVolt);		
	}
	else if(laser_sel == 4) 
	{
		tempVolt  = (int32_t)2590;
    tempVolt2 = (int32_t)probeOffset.probe_offset.p905DAC3VoltOffset ;     // Changed from Jack, 2016.08.07
		tempVolt += tempVolt2;		
	  if(tempVolt < 0)
		  tempVolt = 0;
	  if(tempVolt > DAC_MAX_VALUE)
			tempVolt = DAC_MAX_VALUE;		
	  DAC7718_OUT_VOLT(s905_DAC4,(uint16_t)tempVolt);		
	}		
	else
	{
		Power_V30_Off();	
	  Power_VIN660_Off();              
		return;
	}
			
	Delay(10);
			
	FPGA_ENABLE();
	Delay(10);
  Reset_All_Registers_FPGA();                                             // Added by Jason Chen, 2015.02.23 according to request			
	Delay(10);
		
	Set_905_pulse(PULSE_WIDTH_200, FREQ_10000HZ);	Delay(10);      
	Set_660_pulse(PULSE_WIDTH_50,  FREQ_10000HZ); Delay(10);
		
	//Set_905_Feedback_pulse(DAC4_905_FEEDBACK);
	//////////////////////////////////////////////////////////////////////  // Added by Jason Chen, 2015.12.17
	if(laser_sel == 0)      {Set_905_Feedback_pulse(DAC0_905_FEEDBACK);}
	else if(laser_sel == 1) {Set_905_Feedback_pulse(DAC1_905_FEEDBACK);}
	else if(laser_sel == 2) {Set_905_Feedback_pulse(DAC2_905_FEEDBACK);}
	else if(laser_sel == 3) {Set_905_Feedback_pulse(DAC3_905_FEEDBACK);}
	else if(laser_sel == 4) {Set_905_Feedback_pulse(DAC4_905_FEEDBACK);}
	else
	{
		Set_905_Feedback_pulse(0);
		Set_660_Feedback_pulse(0);
	}
  //////////////////////////////////////////////////////////////////////	
		
	//display   "Self-Test Mode"
}

void SelfTest_setup_Wayne(int16_t laser_sel)                                 // Added by Jason Chen, 2015.10.07
{
	int32_t tempVolt;
	Power_V30_On();
	
	DAC7718_OUT_VOLT(s905_DAC0,0);	Delay(10);
	DAC7718_OUT_VOLT(s905_DAC1,0);	Delay(10);
	DAC7718_OUT_VOLT(s905_DAC2,0);	Delay(10);
	DAC7718_OUT_VOLT(s905_DAC3,0);	Delay(10);
	DAC7718_OUT_VOLT(s905_DAC4,0);	Delay(10);
	
	Power_VIN660_On();              
	S660_DAC_OUT_VOLT(s660_DAC0,0);	Delay(10);
	S660_DAC_OUT_VOLT(s660_DAC1,0);	Delay(10);
	S660_DAC_OUT_VOLT(s660_DAC2,0);	Delay(10);
	S660_DAC_OUT_VOLT(s660_DAC3,0);	Delay(10);
	
	if(laser_sel == 0)
	{
	//tempVolt = 3052 +  probeOffset.probe_offset.p905DAC0VoltOffset;
		tempVolt = 2260 +  probeOffset.probe_offset.p905DAC0VoltOffset;     // Changed by Jason Chen, 2015.12.17
	  if(tempVolt < 0)
		  tempVolt = 0;
	  if(tempVolt > DAC_MAX_VALUE)
			tempVolt = DAC_MAX_VALUE;		
	  DAC7718_OUT_VOLT(s905_DAC0,(uint16_t)tempVolt);
	}
	else if(laser_sel == 1) 
	{
	//tempVolt = 3052 +  probeOffset.probe_offset.p905DAC1VoltOffset;
		tempVolt = 2260 +  probeOffset.probe_offset.p905DAC1VoltOffset;    // Changed by Jason Chen, 2015.12.17
	  if(tempVolt < 0)
		  tempVolt = 0;
	  if(tempVolt > DAC_MAX_VALUE)
			tempVolt = DAC_MAX_VALUE;		
	  DAC7718_OUT_VOLT(s905_DAC1,(uint16_t)tempVolt);		
	}
	else if(laser_sel == 2)
	{
	//tempVolt = 3052 +  probeOffset.probe_offset.p905DAC2VoltOffset;
		tempVolt = 2260 +  probeOffset.probe_offset.p905DAC2VoltOffset;    // Changed by Jason Chen, 2015.12.17
	  if(tempVolt < 0)
		  tempVolt = 0;
	  if(tempVolt > DAC_MAX_VALUE)
			tempVolt = DAC_MAX_VALUE;		
	  DAC7718_OUT_VOLT(s905_DAC2,(uint16_t)tempVolt);		
	}
	else if(laser_sel == 3) 
	{
	//tempVolt = 3052 +  probeOffset.probe_offset.p905DAC3VoltOffset;
		tempVolt = 2192 +  probeOffset.probe_offset.p905DAC3VoltOffset;    // Changed by Jason Chen, 2015.12.17
	  if(tempVolt < 0)
		  tempVolt = 0;
	  if(tempVolt > DAC_MAX_VALUE)
			tempVolt = DAC_MAX_VALUE;		
	  DAC7718_OUT_VOLT(s905_DAC3,(uint16_t)tempVolt);		
	}
	else if(laser_sel == 4) 
	{
	//tempVolt = 3052 +  probeOffset.probe_offset.p905DAC4VoltOffset;
		tempVolt = 2260 +  probeOffset.probe_offset.p905DAC4VoltOffset;    // Changed by Jason Chen, 2015.12.17
	  if(tempVolt < 0)
		  tempVolt = 0;
	  if(tempVolt > DAC_MAX_VALUE)
			tempVolt = DAC_MAX_VALUE;		
	  DAC7718_OUT_VOLT(s905_DAC4,(uint16_t)tempVolt);		
	}
	else if(laser_sel == 5) 
	{
	  tempVolt = 3276 +  probeOffset.probe_offset.p660DAC0VoltOffset;
	  if(tempVolt < 0)
		  tempVolt = 0;
	  if(tempVolt > DAC_MAX_VALUE)
		  tempVolt = DAC_MAX_VALUE;		
	  S660_DAC_OUT_VOLT(s660_DAC0,(uint16_t)tempVolt);				
	}
	else if(laser_sel == 6) 
	{
	  tempVolt = 3276 +  probeOffset.probe_offset.p660DAC1VoltOffset;
	  if(tempVolt < 0)
		  tempVolt = 0;
	  if(tempVolt > DAC_MAX_VALUE)
		  tempVolt = DAC_MAX_VALUE;		
	  S660_DAC_OUT_VOLT(s660_DAC1,(uint16_t)tempVolt);						
	}
	else if(laser_sel == 7) 
	{
	  tempVolt = 3276 +  probeOffset.probe_offset.p660DAC2VoltOffset;
	  if(tempVolt < 0)
		  tempVolt = 0;
	  if(tempVolt > DAC_MAX_VALUE)
		  tempVolt = DAC_MAX_VALUE;		
	  S660_DAC_OUT_VOLT(s660_DAC2,(uint16_t)tempVolt);						
	}
	else if(laser_sel == 8) 
	{
	  tempVolt = 3276 +  probeOffset.probe_offset.p660DAC3VoltOffset;
	  if(tempVolt < 0)
		  tempVolt = 0;
	  if(tempVolt > DAC_MAX_VALUE)
		  tempVolt = DAC_MAX_VALUE;		
	  S660_DAC_OUT_VOLT(s660_DAC3,(uint16_t)tempVolt);						
	}
	else
	{
		Power_V30_Off();	
	  Power_VIN660_Off();              
		return;
	}
			
	Delay(10);
			
	FPGA_ENABLE();
	Delay(10);
  Reset_All_Registers_FPGA();                                             // Added by Jason Chen, 2015.02.23 according to request			
	Delay(10);
		
	Set_905_pulse(PULSE_WIDTH_200, FREQ_10000HZ);	Delay(10);      
	Set_660_pulse(PULSE_WIDTH_50,  FREQ_10000HZ); Delay(10);
		
	//Set_905_Feedback_pulse(DAC4_905_FEEDBACK);
	//////////////////////////////////////////////////////////////////////  // Added by Jason Chen, 2015.12.17
	if(laser_sel == 0)      {Set_905_Feedback_pulse(DAC0_905_FEEDBACK);}
	else if(laser_sel == 1) {Set_905_Feedback_pulse(DAC1_905_FEEDBACK);}
	else if(laser_sel == 2) {Set_905_Feedback_pulse(DAC2_905_FEEDBACK);}
	else if(laser_sel == 3) {Set_905_Feedback_pulse(DAC3_905_FEEDBACK);}
	else if(laser_sel == 4) {Set_905_Feedback_pulse(DAC4_905_FEEDBACK);}
	else if(laser_sel == 5) {Set_660_Feedback_pulse(DAC0_660_FEEDBACK);}
	else if(laser_sel == 6) {Set_660_Feedback_pulse(DAC1_660_FEEDBACK);}
	else if(laser_sel == 7) {Set_660_Feedback_pulse(DAC2_660_FEEDBACK);}
	else if(laser_sel == 8) {Set_660_Feedback_pulse(DAC3_660_FEEDBACK);}
	else
	{
		Set_905_Feedback_pulse(0);
		Set_660_Feedback_pulse(0);
	}
  //////////////////////////////////////////////////////////////////////	
		

	//display   "Self-Test Mode"
}

void TreatmentOn(void)
{
	uint16_t maxPower;
	int32_t tempVolt;
	 
	probeMode =BLE_RPT_MODE_TREATMENT_BUSY;
  Power_V30_On();
	tempVolt = (int16_t)userProfile.user_profile.p905DAC0Voltage;
	tempVolt = tempVolt +  probeOffset.probe_offset.p905DAC0VoltOffset;
	if(tempVolt < 0)
		tempVolt = 0;
	if(tempVolt > DAC_MAX_VALUE)
		tempVolt = DAC_MAX_VALUE;		
	DAC7718_OUT_VOLT(s905_DAC0,(uint16_t)tempVolt);
	//DAC7718_OUT_VOLT(s905_DAC0,userProfile.user_profile.p905DAC0Voltage);	
	Delay(10);
	
	tempVolt = (int16_t)userProfile.user_profile.p905DAC1Voltage;
	tempVolt = tempVolt +  probeOffset.probe_offset.p905DAC1VoltOffset;
	if(tempVolt < 0)
		tempVolt = 0;
	if(tempVolt > DAC_MAX_VALUE)
		tempVolt = DAC_MAX_VALUE;		
	DAC7718_OUT_VOLT(s905_DAC1,(uint16_t)tempVolt);	
	//DAC7718_OUT_VOLT(s905_DAC1,userProfile.user_profile.p905DAC1Voltage);
	Delay(10);
	
  tempVolt = (int16_t)userProfile.user_profile.p905DAC2Voltage;
	tempVolt = tempVolt +  probeOffset.probe_offset.p905DAC2VoltOffset;
	if(tempVolt < 0)
		tempVolt = 0;
	if(tempVolt > DAC_MAX_VALUE)
		tempVolt = DAC_MAX_VALUE;		
	DAC7718_OUT_VOLT(s905_DAC2,(uint16_t)tempVolt);		
	//DAC7718_OUT_VOLT(s905_DAC2,userProfile.user_profile.p905DAC2Voltage);
	Delay(10);
	
	tempVolt = (int16_t)userProfile.user_profile.p905DAC3Voltage;
	tempVolt = tempVolt +  probeOffset.probe_offset.p905DAC3VoltOffset;
	if(tempVolt < 0)
		tempVolt = 0;
	if(tempVolt > DAC_MAX_VALUE)
		tempVolt = DAC_MAX_VALUE;		
	DAC7718_OUT_VOLT(s905_DAC3,(uint16_t)tempVolt);	
	//DAC7718_OUT_VOLT(s905_DAC3,userProfile.user_profile.p905DAC3Voltage);
	Delay(10);
	
	tempVolt = (int16_t)userProfile.user_profile.p905DAC4Voltage;
	tempVolt = tempVolt +  probeOffset.probe_offset.p905DAC4VoltOffset;
	if(tempVolt < 0)
		tempVolt = 0;
	if(tempVolt > DAC_MAX_VALUE)
		tempVolt = DAC_MAX_VALUE;		
	DAC7718_OUT_VOLT(s905_DAC4,(uint16_t)tempVolt);	
	//DAC7718_OUT_VOLT(s905_DAC4,userProfile.user_profile.p905DAC4Voltage);
	Delay(10);
	
	Power_VIN660_On();                                                     // 2014.10.28
	tempVolt = (int16_t)userProfile.user_profile.p660DAC0Voltage;
	tempVolt = tempVolt +  probeOffset.probe_offset.p660DAC0VoltOffset;
	if(tempVolt < 0)
		tempVolt = 0;
	if(tempVolt > DAC_MAX_VALUE)
		tempVolt = DAC_MAX_VALUE;		
	S660_DAC_OUT_VOLT(s660_DAC0,(uint16_t)tempVolt);		
	//S660_DAC_OUT_VOLT(s660_DAC0, userProfile.user_profile.p660DAC0Voltage);// 2014.10.28
	Delay(10);
	
	tempVolt = (int16_t)userProfile.user_profile.p660DAC1Voltage;
	tempVolt = tempVolt +  probeOffset.probe_offset.p660DAC1VoltOffset;
	if(tempVolt < 0)
		tempVolt = 0;
	if(tempVolt > DAC_MAX_VALUE)
		tempVolt = DAC_MAX_VALUE;		
	S660_DAC_OUT_VOLT(s660_DAC1,(uint16_t)tempVolt);			
	//S660_DAC_OUT_VOLT(s660_DAC1, userProfile.user_profile.p660DAC1Voltage);// 2014.10.28
	Delay(10);
	
	tempVolt = (int16_t)userProfile.user_profile.p660DAC2Voltage;
	tempVolt = tempVolt +  probeOffset.probe_offset.p660DAC2VoltOffset;
	if(tempVolt < 0)
		tempVolt = 0;
	if(tempVolt > DAC_MAX_VALUE)
		tempVolt = DAC_MAX_VALUE;		
	S660_DAC_OUT_VOLT(s660_DAC2,(uint16_t)tempVolt);		
	//S660_DAC_OUT_VOLT(s660_DAC2, userProfile.user_profile.p660DAC2Voltage);// 2014.10.28
	Delay(10);
	
	tempVolt = (int16_t)userProfile.user_profile.p660DAC3Voltage;
	tempVolt = tempVolt +  probeOffset.probe_offset.p660DAC3VoltOffset;
	if(tempVolt < 0)
		tempVolt = 0;
	if(tempVolt > DAC_MAX_VALUE)
		tempVolt = DAC_MAX_VALUE;		
	S660_DAC_OUT_VOLT(s660_DAC3,(uint16_t)tempVolt);		
	//S660_DAC_OUT_VOLT(s660_DAC3, userProfile.user_profile.p660DAC3Voltage);// 2014.10.28
	Delay(10);
	
	DAC7718_OUT_VOLT(TENS_DAC, userProfile.user_profile.pTensDACVoltage & 0x0FFF);
	
	FPGA_ENABLE();
	Delay(10);
    Reset_All_Registers_FPGA();                                             // Added by Jason Chen, 2015.02.23 according to request	
	
	//adc_reading_interval = 0;
	
	if(((userProfile.user_profile.p905DAC0Voltage > 0)||
		  (userProfile.user_profile.p905DAC1Voltage > 0)||
	    (userProfile.user_profile.p905DAC2Voltage > 0)||
	    (userProfile.user_profile.p905DAC3Voltage > 0)||
	    (userProfile.user_profile.p905DAC4Voltage > 0)||
      (userProfile.user_profile.p905DAC5Voltage > 0)||
      (userProfile.user_profile.p660DAC0Voltage > 0)||
		  (userProfile.user_profile.p660DAC1Voltage > 0)||
	    (userProfile.user_profile.p660DAC2Voltage > 0)||   
	    (userProfile.user_profile.p660DAC3Voltage > 0))&&
	    (userProfile.user_profile.treatment_time > 0))		
	//if(pcCmd.power)
	{
		Delay(10);
	//Set_905_pulse((userProfile.user_profile.p905DutyFreq << 8) & 0xFF00, userProfile.user_profile.p905DutyFreq & 0x00FF);	  // 2014.10.27, modified
		Set_905_pulse(userProfile.user_profile.p905DutyFreq & 0xFF00, userProfile.user_profile.p905DutyFreq & 0x00FF);	        // 2015.03.02
	  Delay(10);
	//Set_660_pulse((userProfile.user_profile.p660DutyFreq << 8) & 0xFF00, userProfile.user_profile.p660DutyFreq & 0x00FF);   // 2014.10.27, modified
		Set_660_pulse(userProfile.user_profile.p660DutyFreq & 0xFF00, userProfile.user_profile.p660DutyFreq & 0x00FF);          // 2015.03.02
		
		Set_cadence_freq_duty(CADENCE_FREQ_905,(uint8_t)((userProfile.user_profile.p905CadenceDutyFreq >> 8)&0x00FF),(uint8_t)(userProfile.user_profile.p905CadenceDutyFreq&0x00FF));
	  Set_cadence_freq_duty(CADENCE_FREQ_660,(uint8_t)((userProfile.user_profile.p660CadenceDutyFreq >> 8)&0x00FF),(uint8_t)(userProfile.user_profile.p660CadenceDutyFreq&0x00FF));	
	 
		Display_LaserOn();
		Display_Treat();
		
		
    //Display_TENsOff();		                                                       //Requred by Wayne, 2014.11.21
		//Display_TENsOn();		                                                         //Requred by Wayne, 2014.11.21  
		
		maxPower = userProfile.user_profile.p905DAC0Voltage;
		if(maxPower < userProfile.user_profile.p905DAC1Voltage)
			maxPower = userProfile.user_profile.p905DAC1Voltage;
		if(maxPower < userProfile.user_profile.p905DAC2Voltage)
			maxPower = userProfile.user_profile.p905DAC2Voltage;
		if(maxPower < userProfile.user_profile.p905DAC3Voltage)
			maxPower = userProfile.user_profile.p905DAC3Voltage;
		if(maxPower < userProfile.user_profile.p905DAC4Voltage)
			maxPower = userProfile.user_profile.p905DAC4Voltage;
		if(maxPower < userProfile.user_profile.p905DAC5Voltage)
			maxPower = userProfile.user_profile.p905DAC5Voltage;
			
		//Display_Power(maxPower & 0x0FFF);	
		DisplayPowermW(userProfile.user_profile.powerValue_mW);
	}
	//else                                                                           // commented by Wayne, 2014.11.21
  if(userProfile.user_profile.pTensDACVoltage  > 0)	
	//if(pcCmd.tense)                                                                  //Requred by Wayne, 2014.11.21		
	{
	  Delay(10);
    Set_TENS_Frequence(userProfile.user_profile.TensEnable);                       // 2014.10.27
		Display_TENsOn();
		//Display_LaserOff();                                                          //Requred by Wayne, 2014.11.21

		
		Display_TensPower(userProfile.user_profile.pTensDACVoltage&0xFFF);
	}
	
	Display_2s_count(++Treatment_Count);                                             // Added by Jason Chen, 2015.04.09
	Enable_temperature_reading(ENABLE_TEMP);                                         // Added by Jason Chen, 2015.04.09
	
	treatment_on_flag = 1;
}

void ReTreatmentOn(void)                             // Added by Jason Chen, 2015.04.09
{
	uint16_t maxPower;
	int32_t tempVolt;
	 
	probeMode =BLE_RPT_MODE_TREATMENT_BUSY;
  Power_V30_On();

	tempVolt = (int16_t)userProfile.user_profile.p905DAC0Voltage;
	tempVolt = tempVolt +  probeOffset.probe_offset.p905DAC0VoltOffset;
	if(tempVolt < 0)
		tempVolt = 0;
	if(tempVolt > DAC_MAX_VALUE)
		tempVolt = DAC_MAX_VALUE;		
	DAC7718_OUT_VOLT(s905_DAC0,(uint16_t)tempVolt);
	//DAC7718_OUT_VOLT(s905_DAC0,userProfile.user_profile.p905DAC0Voltage);	
	Delay(10);
	
	tempVolt = (int16_t)userProfile.user_profile.p905DAC1Voltage;
	tempVolt = tempVolt +  probeOffset.probe_offset.p905DAC1VoltOffset;
	if(tempVolt < 0)
		tempVolt = 0;
	if(tempVolt > DAC_MAX_VALUE)
		tempVolt = DAC_MAX_VALUE;		
	DAC7718_OUT_VOLT(s905_DAC1,(uint16_t)tempVolt);	
	//DAC7718_OUT_VOLT(s905_DAC1,userProfile.user_profile.p905DAC1Voltage);
	Delay(10);
	
  tempVolt = (int16_t)userProfile.user_profile.p905DAC2Voltage;
	tempVolt = tempVolt +  probeOffset.probe_offset.p905DAC2VoltOffset;
	if(tempVolt < 0)
		tempVolt = 0;
	if(tempVolt > DAC_MAX_VALUE)
		tempVolt = DAC_MAX_VALUE;		
	DAC7718_OUT_VOLT(s905_DAC2,(uint16_t)tempVolt);		
	//DAC7718_OUT_VOLT(s905_DAC2,userProfile.user_profile.p905DAC2Voltage);
	Delay(10);
	
	tempVolt = (int16_t)userProfile.user_profile.p905DAC3Voltage;
	tempVolt = tempVolt +  probeOffset.probe_offset.p905DAC3VoltOffset;
	if(tempVolt < 0)
		tempVolt = 0;
	if(tempVolt > DAC_MAX_VALUE)
		tempVolt = DAC_MAX_VALUE;		
	DAC7718_OUT_VOLT(s905_DAC3,(uint16_t)tempVolt);	
	//DAC7718_OUT_VOLT(s905_DAC3,userProfile.user_profile.p905DAC3Voltage);
	Delay(10);
	
	tempVolt = (int16_t)userProfile.user_profile.p905DAC4Voltage;
	tempVolt = tempVolt +  probeOffset.probe_offset.p905DAC4VoltOffset;
	if(tempVolt < 0)
		tempVolt = 0;
	if(tempVolt > DAC_MAX_VALUE)
		tempVolt = DAC_MAX_VALUE;		
	DAC7718_OUT_VOLT(s905_DAC4,(uint16_t)tempVolt);	
	//DAC7718_OUT_VOLT(s905_DAC4,userProfile.user_profile.p905DAC4Voltage);
	Delay(10);
	
	Power_VIN660_On();                                                     // 2014.10.28
	tempVolt = (int16_t)userProfile.user_profile.p660DAC0Voltage;
	tempVolt = tempVolt +  probeOffset.probe_offset.p660DAC0VoltOffset;
	if(tempVolt < 0)
		tempVolt = 0;
	if(tempVolt > DAC_MAX_VALUE)
		tempVolt = DAC_MAX_VALUE;		
	S660_DAC_OUT_VOLT(s660_DAC0,(uint16_t)tempVolt);		
	//S660_DAC_OUT_VOLT(s660_DAC0, userProfile.user_profile.p660DAC0Voltage);// 2014.10.28
	Delay(10);
	
	tempVolt = (int16_t)userProfile.user_profile.p660DAC1Voltage;
	tempVolt = tempVolt +  probeOffset.probe_offset.p660DAC1VoltOffset;
	if(tempVolt < 0)
		tempVolt = 0;
	if(tempVolt > DAC_MAX_VALUE)
		tempVolt = DAC_MAX_VALUE;		
	S660_DAC_OUT_VOLT(s660_DAC1,(uint16_t)tempVolt);			
	//S660_DAC_OUT_VOLT(s660_DAC1, userProfile.user_profile.p660DAC1Voltage);// 2014.10.28
	Delay(10);
	
	tempVolt = (int16_t)userProfile.user_profile.p660DAC2Voltage;
	tempVolt = tempVolt +  probeOffset.probe_offset.p660DAC2VoltOffset;
	if(tempVolt < 0)
		tempVolt = 0;
	if(tempVolt > DAC_MAX_VALUE)
		tempVolt = DAC_MAX_VALUE;		
	S660_DAC_OUT_VOLT(s660_DAC2,(uint16_t)tempVolt);		
	//S660_DAC_OUT_VOLT(s660_DAC2, userProfile.user_profile.p660DAC2Voltage);// 2014.10.28
	Delay(10);
	
	tempVolt = (int16_t)userProfile.user_profile.p660DAC3Voltage;
	tempVolt = tempVolt +  probeOffset.probe_offset.p660DAC3VoltOffset;
	if(tempVolt < 0)
		tempVolt = 0;
	if(tempVolt > DAC_MAX_VALUE)
		tempVolt = DAC_MAX_VALUE;		
	S660_DAC_OUT_VOLT(s660_DAC3,(uint16_t)tempVolt);		
	//S660_DAC_OUT_VOLT(s660_DAC3, userProfile.user_profile.p660DAC3Voltage);// 2014.10.28
	Delay(10);
	
	DAC7718_OUT_VOLT(TENS_DAC, userProfile.user_profile.pTensDACVoltage & 0x0FFF);
	
	FPGA_ENABLE();
	Delay(10);
  Reset_All_Registers_FPGA();                                             // Added by Jason Chen, 2015.02.23 according to request	
		
	if(((userProfile.user_profile.p905DAC0Voltage > 0)||
		  (userProfile.user_profile.p905DAC1Voltage > 0)||
	    (userProfile.user_profile.p905DAC2Voltage > 0)||
	    (userProfile.user_profile.p905DAC3Voltage > 0)||
	    (userProfile.user_profile.p905DAC4Voltage > 0)||
	    (userProfile.user_profile.p905DAC5Voltage > 0)||
      (userProfile.user_profile.p660DAC0Voltage > 0)||
		  (userProfile.user_profile.p660DAC1Voltage > 0)||
	    (userProfile.user_profile.p660DAC2Voltage > 0)||
	    (userProfile.user_profile.p660DAC3Voltage > 0))&&	
	    (userProfile.user_profile.treatment_time > 0))		
	{
		Delay(10);
		Set_905_pulse(userProfile.user_profile.p905DutyFreq & 0xFF00, userProfile.user_profile.p905DutyFreq & 0x00FF);	        // 2015.03.02
	  Delay(10);
		Set_660_pulse(userProfile.user_profile.p660DutyFreq & 0xFF00, userProfile.user_profile.p660DutyFreq & 0x00FF);          // 2015.03.02
		
		Set_cadence_freq_duty(CADENCE_FREQ_905,(uint8_t)((userProfile.user_profile.p905CadenceDutyFreq >> 8)&0x00FF),(uint8_t)(userProfile.user_profile.p905CadenceDutyFreq&0x00FF));
	  Set_cadence_freq_duty(CADENCE_FREQ_660,(uint8_t)((userProfile.user_profile.p660CadenceDutyFreq >> 8)&0x00FF),(uint8_t)(userProfile.user_profile.p660CadenceDutyFreq&0x00FF));	
	  
		Display_LaserOn();
		Display_Treat();
		
		maxPower = userProfile.user_profile.p905DAC0Voltage;
		if(maxPower < userProfile.user_profile.p905DAC1Voltage)
			maxPower = userProfile.user_profile.p905DAC1Voltage;
		if(maxPower < userProfile.user_profile.p905DAC2Voltage)
			maxPower = userProfile.user_profile.p905DAC2Voltage;
		if(maxPower < userProfile.user_profile.p905DAC3Voltage)
			maxPower = userProfile.user_profile.p905DAC3Voltage;
		if(maxPower < userProfile.user_profile.p905DAC4Voltage)
			maxPower = userProfile.user_profile.p905DAC4Voltage;
		if(maxPower < userProfile.user_profile.p905DAC5Voltage)
			maxPower = userProfile.user_profile.p905DAC5Voltage;
			
		DisplayPowermW(userProfile.user_profile.powerValue_mW);
	}
  if(userProfile.user_profile.pTensDACVoltage  > 0)	
	{
	  Delay(10);
    Set_TENS_Frequence(userProfile.user_profile.TensEnable);                       // 2014.10.27
		Display_TENsOn();		
		Display_TensPower(userProfile.user_profile.pTensDACVoltage&0xFFF);
	}
	
	treatment_on_flag = 1;
}

void MeasurementOn(void)                                                   // 2015.03.02
{
	int32_t tempVolt;
	
  probeMode =BLE_RPT_MODE_MEASUREMENT_BUSY;
  Power_V30_On();
	
	tempVolt = (int16_t)userProfile.user_profile.p905DAC0Voltage;
	tempVolt = tempVolt +  probeOffset.probe_offset.p905DAC0VoltOffset;
	if(tempVolt < 0)
		tempVolt = 0;
	if(tempVolt > DAC_MAX_VALUE)
		tempVolt = DAC_MAX_VALUE;		
	DAC7718_OUT_VOLT(s905_DAC0,(uint16_t)tempVolt);
	//DAC7718_OUT_VOLT(s905_DAC0,userProfile.user_profile.p905DAC0Voltage);	
	Delay(10);
	
	tempVolt = (int16_t)userProfile.user_profile.p905DAC1Voltage;
	tempVolt = tempVolt +  probeOffset.probe_offset.p905DAC1VoltOffset;
	if(tempVolt < 0)
		tempVolt = 0;
	if(tempVolt > DAC_MAX_VALUE)
		tempVolt = DAC_MAX_VALUE;		
	DAC7718_OUT_VOLT(s905_DAC1,(uint16_t)tempVolt);	
	//DAC7718_OUT_VOLT(s905_DAC1,userProfile.user_profile.p905DAC1Voltage);
	Delay(10);
	
  tempVolt = (int16_t)userProfile.user_profile.p905DAC2Voltage;
	tempVolt = tempVolt +  probeOffset.probe_offset.p905DAC2VoltOffset;
	if(tempVolt < 0)
		tempVolt = 0;
	if(tempVolt > DAC_MAX_VALUE)
		tempVolt = DAC_MAX_VALUE;		
	DAC7718_OUT_VOLT(s905_DAC2,(uint16_t)tempVolt);		
	//DAC7718_OUT_VOLT(s905_DAC2,userProfile.user_profile.p905DAC2Voltage);
	Delay(10);
	
	tempVolt = (int16_t)userProfile.user_profile.p905DAC3Voltage;
	tempVolt = tempVolt +  probeOffset.probe_offset.p905DAC3VoltOffset;
	if(tempVolt < 0)
		tempVolt = 0;
	if(tempVolt > DAC_MAX_VALUE)
		tempVolt = DAC_MAX_VALUE;		
	DAC7718_OUT_VOLT(s905_DAC3,(uint16_t)tempVolt);	
	//DAC7718_OUT_VOLT(s905_DAC3,userProfile.user_profile.p905DAC3Voltage);
	Delay(10);
	
	tempVolt = (int16_t)userProfile.user_profile.p905DAC4Voltage;
	tempVolt = tempVolt +  probeOffset.probe_offset.p905DAC4VoltOffset;
	if(tempVolt < 0)
		tempVolt = 0;
	if(tempVolt > DAC_MAX_VALUE)
		tempVolt = DAC_MAX_VALUE;		
	DAC7718_OUT_VOLT(s905_DAC4,(uint16_t)tempVolt);	
	//DAC7718_OUT_VOLT(s905_DAC4,userProfile.user_profile.p905DAC4Voltage);
	Delay(10);	

	Power_VIN660_Off();  
	S660_DAC_OUT_VOLT(s660_DAC0, 0); 
	Delay(10);
	S660_DAC_OUT_VOLT(s660_DAC1, 0);
	Delay(10);
	S660_DAC_OUT_VOLT(s660_DAC2, 0);
	Delay(10);
	S660_DAC_OUT_VOLT(s660_DAC3, 0);
	Delay(10);
	
	DAC7718_OUT_VOLT(TENS_DAC, 0);
	
	FPGA_ENABLE();
	Delay(10);
    Reset_All_Registers_FPGA();                                             // Added by Jason Chen, 2015.02.23 according to request	

	adc_reading_interval = 1;
	
	if((userProfile.user_profile.p905DAC0Voltage > 0)||
		 (userProfile.user_profile.p905DAC1Voltage > 0)||
	   (userProfile.user_profile.p905DAC2Voltage > 0)||
	   (userProfile.user_profile.p905DAC3Voltage > 0)||
	   (userProfile.user_profile.p905DAC4Voltage > 0)||
	   (userProfile.user_profile.p905DAC5Voltage > 0))	
	//if(pcCmd.power)
	{
		Delay(10);
	  // Set_905_pulse((userProfile.user_profile.p905DutyFreq << 8) & 0xFF00, userProfile.user_profile.p905DutyFreq & 0x00FF);	  // 2014.10.27, modified
		Set_905_pulse(userProfile.user_profile.p905DutyFreq & 0xFF00, userProfile.user_profile.p905DutyFreq & 0x00FF);	            // 2015.03.02, modified
	  // Delay(10);
	  // Set_660_pulse((userProfile.user_profile.p660DutyFreq << 8) & 0xFF00, userProfile.user_profile.p660DutyFreq & 0x00FF);   // 2014.10.27, modified
	  Display_LaserOn();
		//Display_Active();
		Display_Measure();
		
 // Display_Power(userProfile.user_profile.p905DAC0Voltage&0xFFF);		      		  	
	}
	
	//if(userProfile.user_profile.p905DAC0Voltage > 0)
	//  Set_905_Feedback_pulse(DAC0_905_FEEDBACK);
	//if(userProfile.user_profile.p905DAC1Voltage > 0)
	//  Set_905_Feedback_pulse(DAC1_905_FEEDBACK);
	//if(userProfile.user_profile.p905DAC2Voltage > 0)
	//  Set_905_Feedback_pulse(DAC2_905_FEEDBACK);
	//if(userProfile.user_profile.p905DAC3Voltage > 0)
	//  Set_905_Feedback_pulse(DAC3_905_FEEDBACK);
	//if(userProfile.user_profile.p905DAC4Voltage > 0)
	  Set_905_Feedback_pulse(DAC4_905_FEEDBACK);
	//if(userProfile.user_profile.p905DAC5Voltage > 0)
	//  Set_905_Feedback_pulse(DAC5_905_FEEDBACK);
	
	treatment_on_flag = 1;
	
	ChannelSelect = 0;	                                   // Added by Jason Chen, 2015.04.02
	FeedBack905_Flag  = 0;                                 // Added by Jason Chen, 2015.04.02
  OneMs_count =  1000;					                         // Added by Jason Chen, 2015.04.02
	Enable_Input(CHANNEL0, CH_ENABLE);                     // Added by Jason Chen, 2015.04.02
	Enable_Input(CHANNEL1, CH_DISABLE);		                 // Added by Jason Chen, 2015.04.02
	Enable_Input(CHANNEL2, CH_DISABLE);                    // Added by Jason Chen, 2015.04.02
	Enable_Input(CHANNEL3, CH_DISABLE);		                 // Added by Jason Chen, 2015.04.02	
	
	AMP_hold_flag = 0;				                             // Added by Jason Chen, 2015.04.02
}

void TreatmentOff(void)
{    
	Power_V30_Off();
	Power_VIN660_Off();
	Reset_All_Registers_FPGA();
	Delay(10);
	FPGA_DISABLE();
	//ALL_DAC7718_OUT_VOLT(0);	
	DAC7718_OUT_VOLT(s905_DAC0,0);
	Delay(10);
	DAC7718_OUT_VOLT(s905_DAC1,0);
	Delay(10);
	DAC7718_OUT_VOLT(s905_DAC2,0);
	Delay(10);
	DAC7718_OUT_VOLT(s905_DAC3,0);
	Delay(10);
	DAC7718_OUT_VOLT(s905_DAC4,0);
	Delay(10);
	DAC7718_OUT_VOLT(s905_DAC5,0);
	Delay(10);	
	DAC7718_OUT_VOLT(TENS_DAC,0);		
	Delay(10);
	S660_DAC_OUT_VOLT(s660_DAC0, 0);//userProfile.user_profile.p660DAC0Voltage);// 2014.10.28
	Delay(10);
	S660_DAC_OUT_VOLT(s660_DAC1, 0);//userProfile.user_profile.p660DAC1Voltage);// 2014.10.28
	Delay(10);
	S660_DAC_OUT_VOLT(s660_DAC2, 0);//userProfile.user_profile.p660DAC2Voltage);// 2014.10.28
	Delay(10);
	S660_DAC_OUT_VOLT(s660_DAC3, 0);//userProfile.user_profile.p660DAC3Voltage);// 2014.10.28
	Delay(10);	
	Display_LaserOff();
	
    if(userProfile.user_profile.mode == PATIENT_TREATMENT_MODE)
	{
	  Display_TENsOff();
		Display_Armed();
		probeMode =BLE_RPT_MODE_TREATMENT_ARMED;
	}
	else
	{
		//Display_Halted();
		//Set_905_Feedback_pulse(0);
		adc_reading_interval = 0;
		ChannelSelect = 0;
		probeMode =BLE_RPT_MODE_IDL;
		PROBE_Idle();
	}
	
	treatment_on_flag = 0;
	Enable_temperature_reading(DISABLE_TEMP);                                         // Added by Jason Chen, 2015.04.09
	
}

void TreatmentPauseOff(void)
{

    
	Power_V30_Off();
	Power_VIN660_Off();
	Reset_All_Registers_FPGA();
	Delay(10);
	FPGA_DISABLE();
	//ALL_DAC7718_OUT_VOLT(0);	
	DAC7718_OUT_VOLT(s905_DAC0,0);
	Delay(10);
	DAC7718_OUT_VOLT(s905_DAC1,0);
	Delay(10);
	DAC7718_OUT_VOLT(s905_DAC2,0);
	Delay(10);
	DAC7718_OUT_VOLT(s905_DAC3,0);
	Delay(10);
	DAC7718_OUT_VOLT(s905_DAC4,0);
	Delay(10);
	DAC7718_OUT_VOLT(s905_DAC5,0);
	Delay(10);	
	DAC7718_OUT_VOLT(TENS_DAC,0);		
	Delay(10);
	S660_DAC_OUT_VOLT(s660_DAC0, 0);//userProfile.user_profile.p660DAC0Voltage);// 2014.10.28
	Delay(10);
	S660_DAC_OUT_VOLT(s660_DAC1, 0);//userProfile.user_profile.p660DAC1Voltage);// 2014.10.28
	Delay(10);
	S660_DAC_OUT_VOLT(s660_DAC2, 0);//userProfile.user_profile.p660DAC2Voltage);// 2014.10.28
	Delay(10);
	S660_DAC_OUT_VOLT(s660_DAC3, 0);//userProfile.user_profile.p660DAC3Voltage);// 2014.10.28
	Delay(10);	
	Display_LaserOff();
	
    if(userProfile.user_profile.mode == PATIENT_TREATMENT_MODE)
	{
	  Display_TENsOff();
	//Display_Armed();
		probeMode =BLE_RPT_MODE_TREATMENT_ARMED;
	}
	else
	{
		//Display_Halted();
		//Set_905_Feedback_pulse(0);
		adc_reading_interval = 0;
		ChannelSelect = 0;
		probeMode =BLE_RPT_MODE_IDL;
		PROBE_Idle();
	}
	
	treatment_on_flag = 0;
	Enable_temperature_reading(DISABLE_TEMP);                                         // Added by Jason Chen, 2015.04.09
	
}

void PROBE_Armed(uint16_t voltage905,uint16_t voltage660, uint16_t voltageTens,uint16_t time)
{
	if(userProfile.user_profile.mode == PATIENT_TREATMENT_MODE)               // Added by Jason Chen, 2015.03.26			
	{
			userProfile.user_profile.p905DAC0Voltage = voltage905;
			userProfile.user_profile.p905DAC1Voltage = voltage905;
			userProfile.user_profile.p905DAC2Voltage = voltage905;
			userProfile.user_profile.p905DAC3Voltage = voltage905;
			userProfile.user_profile.p905DAC4Voltage = voltage905;
			userProfile.user_profile.p905DAC5Voltage = voltage905;  	  
       
		  userProfile.user_profile.p660DAC0Voltage = voltage660;
      userProfile.user_profile.p660DAC1Voltage = voltage660;
      userProfile.user_profile.p660DAC2Voltage = voltage660;
	    userProfile.user_profile.p660DAC3Voltage = voltage660;		

      userProfile.user_profile.pTensDACVoltage = voltageTens;		
		
		  probeMode =BLE_RPT_MODE_TREATMENT_ARMED;		
	}
	else if(userProfile.user_profile.mode == PATIENT_MEASUREMENT_MODE)               // Added by Jason Chen, 2015.03.26			
	{
			userProfile.user_profile.p905DAC0Voltage = 0;
			userProfile.user_profile.p905DAC1Voltage = 0;
			userProfile.user_profile.p905DAC2Voltage = 0;
			userProfile.user_profile.p905DAC3Voltage = 0;
			userProfile.user_profile.p905DAC4Voltage = voltage905;
			userProfile.user_profile.p905DAC5Voltage = 0;

      userProfile.user_profile.p660DAC0Voltage = 0;
      userProfile.user_profile.p660DAC1Voltage = 0;
      userProfile.user_profile.p660DAC2Voltage = 0;
	    userProfile.user_profile.p660DAC3Voltage = 0;	
		
		  userProfile.user_profile.pTensDACVoltage = 0;
		  probeMode =BLE_RPT_MODE_MEASUREMENT_ARMED;    
	}	
	userProfile.user_profile.treatment_time = time;
  //Mode_GUI_Init(userProfile.user_profile.mode);	
	
  switch_stm_status = SWITCH_WAIT_PRESS;                                           // Added by Jason Chen, 2016.05.18
  switch_stm_selfTest = SWITCH_WAIT_PRESS;                                         // Added by Jason Chen, 2016.05.18
	
	Mode_GUI_Fresh(userProfile.user_profile.mode);
}

void PROBE_Idle(void)                                                             // Changed by Jason Chen, 2015.03.26
{

		Display_square(180+3*12+6, 156-3*24+3-12, background_color,Font16x24);
		if(treatment_time > 0 )
		{
		  treatment_time_backup = treatment_time;
		  treatment_time = 1;
		}
		userProfile.user_profile.p905DAC0Voltage = 0;
		userProfile.user_profile.p905DAC1Voltage = 0;
		userProfile.user_profile.p905DAC2Voltage = 0;
		userProfile.user_profile.p905DAC3Voltage = 0;
		userProfile.user_profile.p905DAC4Voltage = 0;
		userProfile.user_profile.p905DAC5Voltage = 0;
	
    userProfile.user_profile.p660DAC0Voltage = 0;
    userProfile.user_profile.p660DAC1Voltage = 0;
    userProfile.user_profile.p660DAC2Voltage = 0;
	  userProfile.user_profile.p660DAC3Voltage = 0;
		
	  userProfile.user_profile.pTensDACVoltage = 0;
	  userProfile.user_profile.treatment_time  = 0;
	
		Power_V30_Off();
		Power_VIN660_Off();
		Reset_All_Registers_FPGA();
		Delay(10);
		FPGA_DISABLE();
		//ALL_DAC7718_OUT_VOLT(0);	
		DAC7718_OUT_VOLT(s905_DAC0,0);
		Delay(10);
		DAC7718_OUT_VOLT(s905_DAC1,0);
		Delay(10);
		DAC7718_OUT_VOLT(s905_DAC2,0);
		Delay(10);
		DAC7718_OUT_VOLT(s905_DAC3,0);
		Delay(10);
		DAC7718_OUT_VOLT(s905_DAC4,0);
		Delay(10);
		DAC7718_OUT_VOLT(s905_DAC5,0);
		Delay(10);	
		DAC7718_OUT_VOLT(TENS_DAC,0);		
		Delay(10);
		S660_DAC_OUT_VOLT(s660_DAC0, 0);
		Delay(10);
		S660_DAC_OUT_VOLT(s660_DAC1, 0);
		Delay(10);
		S660_DAC_OUT_VOLT(s660_DAC2, 0);
		Delay(10);
		S660_DAC_OUT_VOLT(s660_DAC3, 0);
		Delay(10);	
		Display_LaserOff();

		Set_905_Feedback_pulse(0);
	  adc_reading_interval = 0;
		ChannelSelect = 0;		
    //Display_Idle();
		//treatment_on_flag = 0;		                              // Removed by Jason Chen, 2015.10.07
		
	  //Mode_GUI_Init(userProfile.user_profile.mode);			
 		probeMode =BLE_RPT_MODE_IDL;
	    userProfile.user_profile.mode =PROBE_MODE_IDEL;
		Mode_GUI_Fresh(userProfile.user_profile.mode);
		Treatment_Count = 0;
		Display_2s_count(0);
		if(((switch_stm_status == SWITCH_RUNNING)||(switch_stm_status == SWITCH_PAUSE)))//&&(treatment_on_flag))    // Changed by Jason Chen, 2016.05.16//Added by Jason Chen, 2015.10.07  
		   switch_stm_status = SWITCH_WAIT_PRESS;//SWITCH_STOP;                                                     // Changed by Jason Chen, 2016.05.16//Added by Jason Chen, 2015.04.09
		
		if((switch_stm_selfTest == SWITCH_RUNNING)||(switch_stm_status == SWITCH_PAUSE))                            // Changed by Jason Chen, 2016.05.16//Added by Jason Chen, 2015.10.07  
		   switch_stm_selfTest = SWITCH_WAIT_PRESS;//SWITCH_STOP;                                                   // Changed by Jason Chen, 2016.05.16//Added by Jason Chen, 2015.10.07
}

void Self_Cali_End(void)                                                             // Added by Jason Chen, 2016.08.08
{

		Display_square(180+3*12+6, 156-3*24+3-12, background_color,Font16x24);
		//if(treatment_time > 0 )
		//{
		//  treatment_time_backup = treatment_time;
		//  treatment_time = 1;
		//}
		userProfile.user_profile.p905DAC0Voltage = 0;
		userProfile.user_profile.p905DAC1Voltage = 0;
		userProfile.user_profile.p905DAC2Voltage = 0;
		userProfile.user_profile.p905DAC3Voltage = 0;
		userProfile.user_profile.p905DAC4Voltage = 0;
		userProfile.user_profile.p905DAC5Voltage = 0;
	
    userProfile.user_profile.p660DAC0Voltage = 0;
    userProfile.user_profile.p660DAC1Voltage = 0;
    userProfile.user_profile.p660DAC2Voltage = 0;
	  userProfile.user_profile.p660DAC3Voltage = 0;
		
	  userProfile.user_profile.pTensDACVoltage = 0;
	  userProfile.user_profile.treatment_time  = 0;
	
		Power_V30_Off();
		Power_VIN660_Off();
		Reset_All_Registers_FPGA();
		Delay(10);
		FPGA_DISABLE();
		//ALL_DAC7718_OUT_VOLT(0);	
		DAC7718_OUT_VOLT(s905_DAC0,0);
		Delay(10);
		DAC7718_OUT_VOLT(s905_DAC1,0);
		Delay(10);
		DAC7718_OUT_VOLT(s905_DAC2,0);
		Delay(10);
		DAC7718_OUT_VOLT(s905_DAC3,0);
		Delay(10);
		DAC7718_OUT_VOLT(s905_DAC4,0);
		Delay(10);
		DAC7718_OUT_VOLT(s905_DAC5,0);
		Delay(10);	
		DAC7718_OUT_VOLT(TENS_DAC,0);		
		Delay(10);
		S660_DAC_OUT_VOLT(s660_DAC0, 0);
		Delay(10);
		S660_DAC_OUT_VOLT(s660_DAC1, 0);
		Delay(10);
		S660_DAC_OUT_VOLT(s660_DAC2, 0);
		Delay(10);
		S660_DAC_OUT_VOLT(s660_DAC3, 0);
		Delay(10);	
  //Display_LaserOff();

		Set_905_Feedback_pulse(0);
	  adc_reading_interval = 0;
		ChannelSelect = 0;		
		
		Enable_Input(CHANNEL0, CH_ENABLE);
		Enable_Input(CHANNEL1, CH_DISABLE);		
		Enable_Input(CHANNEL2, CH_DISABLE);
		Enable_Input(CHANNEL3, CH_DISABLE);								 
		
 		//probeMode =BLE_RPT_MODE_IDL;
    //userProfile.user_profile.mode =PROBE_MODE_IDEL;
  //Mode_GUI_Fresh(userProfile.user_profile.mode);
		Treatment_Count = 0;
		//Display_2s_count(0);
		//if(((switch_stm_status == SWITCH_RUNNING)||(switch_stm_status == SWITCH_PAUSE)))//&&(treatment_on_flag))    // Changed by Jason Chen, 2016.05.16//Added by Jason Chen, 2015.10.07  
		//   switch_stm_status = SWITCH_WAIT_PRESS;//SWITCH_STOP;                                                     // Changed by Jason Chen, 2016.05.16//Added by Jason Chen, 2015.04.09
		
		//if((switch_stm_selfTest == SWITCH_RUNNING)||(switch_stm_status == SWITCH_PAUSE))                            // Changed by Jason Chen, 2016.05.16//Added by Jason Chen, 2015.10.07  
		switch_stm_selfTest = SWITCH_STOP;//SWITCH_WAIT_PRESS;                                                 // Changed by Jason Chen, 2016.05.16//Added by Jason Chen, 2015.10.07
}

void PROBE_Mode(uint8_t mode, uint8_t store_flag)
{
		Buzzer_Volume(0);
	  PROBE_Idle();                                      // Added by Jason Chen, 2015.04.09
		userProfile.user_profile.mode = mode;//Receive_Buffer[2]; // 2014.12.22
	
	  if(userProfile.user_profile.mode == PROBE_MODE_SELTEST)
		{
	   //Display_SlefTestMode();                           // Added by Jason Chen, 2015.10.07			 
		   Display_SlefCaliMode();
			 if(store_flag)
			 {
			   Self_Cali_Clr();
			   DisplaySelfTestTime(0);
			 }
		}
		
		treatment_on_flag = 0;                               // Added by Jason Chen, 2015.10.07

		//if(userProfile.user_profile.mode == PATIENT_MEASUREMENT_MODE)
		//	 userProfile.user_profile.treatment_time = 60;
		//else if(userProfile.user_profile.mode == PATIENT_MEASUREMENT_MODE)
		//	 userProfile.user_profile.treatment_time = 150;					 
		if(store_flag)
		{
			#if ENABLE_FLASH_PROFILE			
					if(myFlashWrite((uint8_t*)&userProfileFlash,(uint8_t*)&userProfile, sizeof(USER_PROFILE_T)) != PASSED)    // Changed by Jason Chen, 2015.03.24
			#else			
					if(myFlashWrite((uint8_t*)FLASH_STORE_BASE_ADDRESS,(uint8_t*)&userProfile, sizeof(USER_PROFILE_T)) != PASSED)				
			#endif		
					{				
					}		
	  }		
		//Mode_GUI_Init(userProfile.user_profile.mode);
}

void Active_SelfTest(void)                             // Added by Jason Chen, 2015.10.07
{
	  SelfTest_Init();                                   // Added by Jason Chen, 2015.12.17
	
		Reset_All_Registers_FPGA();                                             
		Delay(10);
		
		adc_reading_interval = 0;
		AMP_hold_flag = 1;				
		
		//switch_stm_selfTest = SWITCH_INIT;                // Added by Jason Chen, 2015.10.07
							
		FPGA_ENABLE();
		Delay(10);

	  laser_sel = 0;
	  ChannelSelect = 3;                                  // Added by Jason Chen, 2016.08.09
    SelfTest_setup(laser_sel);	
	  //Set_905_Feedback_pulse(DAC4_905_FEEDBACK);
//	Display_LaserOn();					
				
		OneMs_count = 1000;
		treatment_on_flag    = 1;					
		adc_reading_interval = 1;
		AMP_hold_flag = 0;	
}

void DeActive_SelfTest(void)                          // Added by Jason Chen, 2015.10.07
{
		FPGA_ENABLE();	
		Reset_All_Registers_FPGA();                                             // Added by Jason Chen, 2015.02.23 according to request	
		Delay(10);
		
		adc_reading_interval = 0;
		AMP_hold_flag = 1;				
	  Set_905_Feedback_pulse(0);								
		Delay(10);

	  laser_sel = 0;

		Power_V30_Off();
		Power_VIN660_Off();
		Delay(10);
		FPGA_DISABLE();
		DAC7718_OUT_VOLT(s905_DAC0,0);
		Delay(10);
		DAC7718_OUT_VOLT(s905_DAC1,0);
		Delay(10);
		DAC7718_OUT_VOLT(s905_DAC2,0);
		Delay(10);
		DAC7718_OUT_VOLT(s905_DAC3,0);
		Delay(10);
		DAC7718_OUT_VOLT(s905_DAC4,0);
		Delay(10);
		DAC7718_OUT_VOLT(s905_DAC5,0);
		Delay(10);	
		DAC7718_OUT_VOLT(TENS_DAC,0);		
		Delay(10);
		S660_DAC_OUT_VOLT(s660_DAC0, 0);//userProfile.user_profile.p660DAC0Voltage);// 2014.10.28
		Delay(10);
		S660_DAC_OUT_VOLT(s660_DAC1, 0);//userProfile.user_profile.p660DAC1Voltage);// 2014.10.28
		Delay(10);
		S660_DAC_OUT_VOLT(s660_DAC2, 0);//userProfile.user_profile.p660DAC2Voltage);// 2014.10.28
		Delay(10);
		S660_DAC_OUT_VOLT(s660_DAC3, 0);//userProfile.user_profile.p660DAC3Voltage);// 2014.10.28
		Delay(10);
	
    //SelfTest_setup(laser_sel);	
		//Display_LaserOff();					
				
		OneMs_count = 1000;
		treatment_on_flag = 0;					
}


void CommandProcess(void)
{
	uint16_t voltage,volt660,voltTens,t_time;
  uint8_t resp = 0x80;
	
	Send_Buffer[1] = Receive_Buffer[1] | resp;
	Send_Buffer[2] = Receive_Buffer[2];
	Send_Buffer[3] = Receive_Buffer[3];
	Send_Buffer[4] = Receive_Buffer[4];
	Send_Buffer[5] = Receive_Buffer[5];
	Send_Buffer[6] = Receive_Buffer[6];	
	Send_Buffer[7] = Receive_Buffer[7];
	Send_Buffer[8] = Receive_Buffer[8];
	Send_Buffer[9] = Receive_Buffer[9];
	Send_Buffer[10] = Receive_Buffer[10];
	Send_Buffer[11] = Receive_Buffer[11];	
	
	switch (Receive_Buffer[1])
  {
    case CMD_TIME_SYNC_ID:
	    AdjustTime(&Receive_Buffer[2]);
		  break;
    case CMD_TIME_SETUP:
	    AdjustTime(&Receive_Buffer[2]);
		  //userProfile.user_profile.userID = 0x55565758; // for testing
		  (void)memcpy((void*)&Send_Buffer[2],(void*)&userProfile/*Flash*/,sizeof(USER_PROFILE_T));		
		  break;		
		case CMD_905_DAC0_CONFIG:
			voltage = Receive_Buffer[2]*256 + Receive_Buffer[3];
		  if(voltage > 4095)
				voltage = 4095;			
		  userProfile.user_profile.p905DAC0Voltage = voltage;										
			//voltage = (voltage << 4) & 0xFFF0;
			DAC7718_OUT_VOLT(s905_DAC0,voltage);
			break;
		case CMD_905_DAC1_CONFIG:
			voltage = Receive_Buffer[2]*256 + Receive_Buffer[3];
		  if(voltage > 4095)
				voltage = 4095;			
		  userProfile.user_profile.p905DAC1Voltage = voltage;
			DAC7718_OUT_VOLT(s905_DAC1,voltage);
			break;
		case CMD_905_DAC2_CONFIG:
			voltage = Receive_Buffer[2]*256 + Receive_Buffer[3];
		  if(voltage > 4095)
				voltage = 4095;			
		  userProfile.user_profile.p905DAC2Voltage = voltage;
			DAC7718_OUT_VOLT(s905_DAC2,voltage);
			break;
		case CMD_905_DAC3_CONFIG:
			voltage = Receive_Buffer[2]*256 + Receive_Buffer[3];
		  if(voltage > 4095)
				voltage = 4095;			
		  userProfile.user_profile.p905DAC3Voltage = voltage;
			DAC7718_OUT_VOLT(s905_DAC3,voltage);
			break;
		case CMD_905_DAC4_CONFIG:
			voltage = Receive_Buffer[2]*256 + Receive_Buffer[3];
		  if(voltage > 4095)
				voltage = 4095;			
		  userProfile.user_profile.p905DAC4Voltage = voltage;
			DAC7718_OUT_VOLT(s905_DAC4,voltage);
			break;
		case CMD_905_DAC5_CONFIG:
			voltage = Receive_Buffer[2]*256 + Receive_Buffer[3];
		  if(voltage > 4095)
				voltage = 4095;			
		  userProfile.user_profile.p905DAC5Voltage = voltage;
			DAC7718_OUT_VOLT(s905_DAC5,voltage);
			break;
		case CMD_TENS_DAC_CONFIG:
			voltage = Receive_Buffer[2]*256 + Receive_Buffer[3];
		  userProfile.user_profile.pTensDACVoltage = voltage;										
		  if(voltage > 4095)
				voltage = 4095;
			DAC7718_OUT_VOLT(TENS_DAC,voltage);
			break;			
		case CMD_PROBE_ARM_CONGIF://?????????????????????????????  power
			//LED_Toggle();
			voltage = Receive_Buffer[2]*256 + Receive_Buffer[3];
		  if(voltage > 4095)
				voltage = 4095;		
			volt660 = Receive_Buffer[4]*256 + Receive_Buffer[5];
		  if(volt660 > 4095)
				volt660 = 4095;					
			voltTens = Receive_Buffer[6]*256 + Receive_Buffer[7];
		  if(voltTens > 4095)
				voltTens = 4095;								
			t_time = Receive_Buffer[8]*256 + Receive_Buffer[9];

			PROBE_Armed(voltage,volt660,voltTens,t_time);                                                     // Added by Jason Chen, 2015.03.26
			break;		
		case CMD_660_DAC0_CONFIG:
			voltage = Receive_Buffer[2]*256 + Receive_Buffer[3];
		  if(voltage > 4095)
				voltage = 4095;
      userProfile.user_profile.p660DAC0Voltage =  voltage;
			S660_DAC_OUT_VOLT(s660_DAC0,voltage);
			break;
		case CMD_660_DAC1_CONFIG:
			voltage = Receive_Buffer[2]*256 + Receive_Buffer[3];
		  if(voltage > 4095)
				voltage = 4095;
			userProfile.user_profile.p660DAC1Voltage =  voltage;
			S660_DAC_OUT_VOLT(s660_DAC1,voltage);
			break;
		case CMD_660_DAC2_CONFIG:
			voltage = Receive_Buffer[2]*256 + Receive_Buffer[3];
		  if(voltage > 4095)
				voltage = 4095;
			userProfile.user_profile.p660DAC2Voltage =  voltage;
			S660_DAC_OUT_VOLT(s660_DAC2,voltage);
			break;
		case CMD_660_DAC3_CONFIG:
			voltage = Receive_Buffer[2]*256 + Receive_Buffer[3];
		  if(voltage > 4095)
				voltage = 4095;
			userProfile.user_profile.p660DAC3Voltage =  voltage;
			S660_DAC_OUT_VOLT(s660_DAC3,voltage);
			break;			
		case CMD_READ_ADC_VALUE:
			read_adc = !read_adc;
      Send_Buffer[2] = (uint8_t)(adc_rd >> 8);      
			Send_Buffer[3] = (uint8_t)adc_rd;           
			break;
		case CMD_SET_ADC_READING_INTERVAL:                                      // 2014.11.12
			adc_reading_interval = Receive_Buffer[2]*256 + Receive_Buffer[3];		  // 2014.11.12
		  //if(adc_reading_interval < 10)                                         // 2014.11.12
			//	adc_reading_interval = 10;                                          // 2014.11.12
		  LED_Off();
			break;                                                                // 2014.11.12
		case CMD_SET_905_DUTY_FREQ:
			Set_905_pulse(Receive_Buffer[2] << 8, Receive_Buffer[3]);
			break;
		case CMD_SET_660_DUTY_FREQ:
			Set_660_pulse(Receive_Buffer[2] << 8, Receive_Buffer[3]);
			break;	
    case CMD_SET_CADENCE_DUTY_FREQ:
			Set_cadence_freq_duty(Receive_Buffer[2],Receive_Buffer[3],Receive_Buffer[4]);
			break;
		case CMD_SET_TENS_FREQUENCE:                    // 2014.10.21
			Set_TENS_Frequence(Receive_Buffer[2]);        // 2014.10.21
		  break;
		case CMD_RESET_FPGA:                            // 2014.10.21
			Reset_All_Registers_FPGA();                   // 2014.10.21
		  break;		
		case CMD_PROBE_IDLE_CONGIF:
	    //Pot_SetPDx(I2C_SLAVE_ADDRESS7_PD0, CH_RDAC2, Receive_Buffer[2]);       // 2014.10.23
	    //Pot_SetPDx(I2C_SLAVE_ADDRESS7_PD1, CH_RDAC1, Receive_Buffer[3]);       // 2014.10.23
	    //Pot_SetPDx(I2C_SLAVE_ADDRESS7_PD2, CH_RDAC1, Receive_Buffer[4]);       // 2014.10.23
	    //Pot_SetPDx(I2C_SLAVE_ADDRESS7_PD3, CH_RDAC1, Receive_Buffer[5]);       // 2014.10.23	
      //Pot_SetAMP2(Receive_Buffer[6]);		                                     // 2014.10.23		
		  PROBE_Idle();                                                            // Added by Jason Chen, 2015.03.26
			break;
		case CMD_ENABLE_READ_TEMP:
		  HighTemperatureAlarm = Receive_Buffer[2];
		  break;
		case CMD_SET_905FEEDBACK_PULSE:
		  Set_905_Feedback_pulse(Receive_Buffer[2] & 0x3F);
		  break;
		case CMD_SET_660FEEDBACK_PULSE:
		  Set_660_Feedback_pulse(Receive_Buffer[2] & 0x0F);
		  break;
		case CMD_SET_ENABLE_FPGA:
			 if(Receive_Buffer[2])
				 FPGA_ENABLE();
			 else
				 FPGA_DISABLE();			 
			 Send_Buffer[2];// = ST0_PBGetState();
			break;
	  case CMD_WORKING_MODE:
			//if(!treatment_on_flag)      // 
			{
				PROBE_Mode(Receive_Buffer[2],1);                       // Added by Jason Chen, 2015.03.26								
		  }						
			Send_Buffer[3] = userProfile.user_profile.mode;        // Added by Jason Chen, 2015.10.07			
			break;
		case CMD_AUTO_TEST_START:
		  if(userProfile.user_profile.mode != PATIENT_MEASUREMENT_MODE) break;
			if(Receive_Buffer[2])
			{
				if(treatment_on_flag) break;

				Reset_All_Registers_FPGA();                                             // Added by Jason Chen, 2015.02.23 according to request	
        Delay(10);
				
				adc_reading_interval = 0;
        AMP_hold_flag = 1;				
				
				switch_stm_status = SWITCH_RUNNING;
				
				AutoTestValue[0] = Receive_Buffer[3]*256 + Receive_Buffer[4];
				AutoTestValue[1] = Receive_Buffer[5]*256 + Receive_Buffer[6];
				AutoTestTimeInterval = Receive_Buffer[7]*256 + Receive_Buffer[8];
								
				Power_V30_On();
				DAC7718_OUT_VOLT(s905_DAC0,0);
				Delay(10);
				DAC7718_OUT_VOLT(s905_DAC1,0);
				Delay(10);
				DAC7718_OUT_VOLT(s905_DAC2,0);
				Delay(10);
				DAC7718_OUT_VOLT(s905_DAC3,0);
				Delay(10);
				DAC7718_OUT_VOLT(s905_DAC4,0);
				Delay(10);
				Power_VIN660_Off();  
				S660_DAC_OUT_VOLT(s660_DAC0, 0); 
				Delay(10);
				S660_DAC_OUT_VOLT(s660_DAC1, 0);
				Delay(10);
				S660_DAC_OUT_VOLT(s660_DAC2, 0);
				Delay(10);
				S660_DAC_OUT_VOLT(s660_DAC3, 0);
				Delay(10);
				DAC7718_OUT_VOLT(TENS_DAC, 0);
        Delay(10);
				FPGA_ENABLE();
				Delay(10);
				Reset_All_Registers_FPGA();                                             // Added by Jason Chen, 2015.02.23 according to request	
        Delay(10);
				adc_reading_interval = 1;

        userProfile.user_profile.p905DAC4Voltage = AutoTestValue[0];
				DAC7718_OUT_VOLT(s905_DAC4,userProfile.user_profile.p905DAC4Voltage);
				if((userProfile.user_profile.p905DAC0Voltage > 0)||
					 (userProfile.user_profile.p905DAC1Voltage > 0)||
					 (userProfile.user_profile.p905DAC2Voltage > 0)||
					 (userProfile.user_profile.p905DAC3Voltage > 0)||
					 (userProfile.user_profile.p905DAC4Voltage > 0)||
					 (userProfile.user_profile.p905DAC5Voltage > 0))	
				{
					Set_905_pulse(userProfile.user_profile.p905DutyFreq & 0xFF00, userProfile.user_profile.p905DutyFreq & 0x00FF);	            // 2015.03.02, modified
					// Delay(10);
					// Set_660_pulse((userProfile.user_profile.p660DutyFreq << 8) & 0xFF00, userProfile.user_profile.p660DutyFreq & 0x00FF);   // 2014.10.27, modified
					Display_LaserOn();					
				}

				//if(userProfile.user_profile.p905DAC0Voltage > 0)
				//	Set_905_Feedback_pulse(DAC0_905_FEEDBACK);
				//if(userProfile.user_profile.p905DAC1Voltage > 0)
			  //	Set_905_Feedback_pulse(DAC1_905_FEEDBACK);
				//if(userProfile.user_profile.p905DAC2Voltage > 0)
				//	Set_905_Feedback_pulse(DAC2_905_FEEDBACK);
				//if(userProfile.user_profile.p905DAC3Voltage > 0)
				//	Set_905_Feedback_pulse(DAC3_905_FEEDBACK);
				//if(userProfile.user_profile.p905DAC4Voltage > 0)
					Set_905_Feedback_pulse(DAC4_905_FEEDBACK);
				//if(userProfile.user_profile.p905DAC5Voltage > 0)
				//	Set_905_Feedback_pulse(DAC5_905_FEEDBACK);

				treatment_on_flag = 1;	
        OneMs_count =  AutoTestTimeInterval;
				FeedBack905_Flag = 1;
				
			  FeedBack905Test_Flag = 0;
        Enable_Input(CHANNEL0, CH_ENABLE);
				Enable_Input(CHANNEL1, CH_DISABLE);		
				Enable_Input(CHANNEL2, CH_DISABLE);
		    Enable_Input(CHANNEL3, CH_DISABLE);		
				
				FeedBack905Test_Flag = 0;
				adc_reading_interval = 1;
        AMP_hold_flag = 0;				
			}
			else
			{
				if(treatment_on_flag)
				{		
				  if(treatment_time > 0 )
				  {
				    treatment_time_backup = treatment_time;
				    treatment_time = 0;
					  //TreatmentOff();
				  }					
				}
				//if(!FeedBack905_Flag)
				adc_reading_interval = 0;
				Power_V30_Off();
				Power_VIN660_Off();
				Reset_All_Registers_FPGA();
				Delay(10);
				FPGA_DISABLE();
				DAC7718_OUT_VOLT(s905_DAC0,0);
				Delay(10);
				DAC7718_OUT_VOLT(s905_DAC1,0);
				Delay(10);
				DAC7718_OUT_VOLT(s905_DAC2,0);
				Delay(10);
				DAC7718_OUT_VOLT(s905_DAC3,0);
				Delay(10);
				DAC7718_OUT_VOLT(s905_DAC4,0);
				Delay(10);
				DAC7718_OUT_VOLT(s905_DAC5,0);
				Delay(10);	
				DAC7718_OUT_VOLT(TENS_DAC,0);		
				Delay(10);
				S660_DAC_OUT_VOLT(s660_DAC0, 0);//userProfile.user_profile.p660DAC0Voltage);// 2014.10.28
				Delay(10);
				S660_DAC_OUT_VOLT(s660_DAC1, 0);//userProfile.user_profile.p660DAC1Voltage);// 2014.10.28
				Delay(10);
				S660_DAC_OUT_VOLT(s660_DAC2, 0);//userProfile.user_profile.p660DAC2Voltage);// 2014.10.28
				Delay(10);
				S660_DAC_OUT_VOLT(s660_DAC3, 0);//userProfile.user_profile.p660DAC3Voltage);// 2014.10.28
				Delay(10);	
				Display_LaserOff();							
				treatment_on_flag = 0;	
        OneMs_count =  0;				
				FeedBack905_Flag = 0;
			}
			break;			
		case	CMD_FEEDBACK_START:                                                // Added Jason Chen, 2015.09.29
			if(Receive_Buffer[2])
			{
				if(treatment_on_flag) break;

				Reset_All_Registers_FPGA();                                             // Added by Jason Chen, 2015.02.23 according to request	
        Delay(10);
				
				adc_reading_interval = 0;
        AMP_hold_flag = 1;				
				
				switch_stm_status = SWITCH_RUNNING;
				
#if 1				                                                                    // Added by Jason Chen, 2015.12.14
				laserSelect = Receive_Buffer[5];
				if(laserSelect > 8)
					laserSelect = 0;
				voltage_s = Receive_Buffer[3]*256 + Receive_Buffer[4];	
				
	      Power_V30_Off();	
	      DAC7718_OUT_VOLT(s905_DAC0,0);	Delay(10);
	      DAC7718_OUT_VOLT(s905_DAC1,0);	Delay(10);
	      DAC7718_OUT_VOLT(s905_DAC2,0);	Delay(10);
	      DAC7718_OUT_VOLT(s905_DAC3,0);	Delay(10);
	      DAC7718_OUT_VOLT(s905_DAC4,0);	Delay(10);
	
	      Power_VIN660_Off();
	      S660_DAC_OUT_VOLT(s660_DAC0,0);	Delay(10);
	      S660_DAC_OUT_VOLT(s660_DAC1,0);	Delay(10);
	      S660_DAC_OUT_VOLT(s660_DAC2,0);	Delay(10);
	      S660_DAC_OUT_VOLT(s660_DAC3,0);	Delay(10);
				
				userProfile.user_profile.p905DAC0Voltage = 0;
				userProfile.user_profile.p905DAC1Voltage = 0;
				userProfile.user_profile.p905DAC2Voltage = 0;
				userProfile.user_profile.p905DAC3Voltage = 0;
				userProfile.user_profile.p905DAC4Voltage = 0;
				userProfile.user_profile.p660DAC0Voltage = 0;
				userProfile.user_profile.p660DAC1Voltage = 0;
				userProfile.user_profile.p660DAC2Voltage = 0;
				userProfile.user_profile.p660DAC3Voltage = 0;
	
				if(laserSelect == 0)      {Power_V30_On();userProfile.user_profile.p905DAC0Voltage = voltage_s;      DAC7718_OUT_VOLT(s905_DAC0,voltage_s);}
				else if(laserSelect == 1) {Power_V30_On();userProfile.user_profile.p905DAC1Voltage = voltage_s;      DAC7718_OUT_VOLT(s905_DAC1,voltage_s);}
				else if(laserSelect == 2) {Power_V30_On();userProfile.user_profile.p905DAC2Voltage = voltage_s;      DAC7718_OUT_VOLT(s905_DAC2,voltage_s);}
				else if(laserSelect == 3) {Power_V30_On();userProfile.user_profile.p905DAC3Voltage = voltage_s;      DAC7718_OUT_VOLT(s905_DAC3,voltage_s);}
				else if(laserSelect == 4) {Power_V30_On();userProfile.user_profile.p905DAC4Voltage = voltage_s;      DAC7718_OUT_VOLT(s905_DAC4,voltage_s);}
				else if(laserSelect == 5) {Power_VIN660_On();userProfile.user_profile.p660DAC0Voltage = voltage_s;  S660_DAC_OUT_VOLT(s660_DAC0,voltage_s);}
				else if(laserSelect == 6) {Power_VIN660_On();userProfile.user_profile.p660DAC1Voltage = voltage_s;  S660_DAC_OUT_VOLT(s660_DAC1,voltage_s);}
				else if(laserSelect == 7) {Power_VIN660_On();userProfile.user_profile.p660DAC2Voltage = voltage_s;  S660_DAC_OUT_VOLT(s660_DAC2,voltage_s);}
				else if(laserSelect == 8) {Power_VIN660_On();userProfile.user_profile.p660DAC3Voltage = voltage_s;  S660_DAC_OUT_VOLT(s660_DAC3,voltage_s);}
				else
				{
					Power_V30_Off();	
					Power_VIN660_Off();              
				}
				Delay(10);	
				FPGA_ENABLE();
				Delay(10);
				Reset_All_Registers_FPGA();                                             // Added by Jason Chen, 2015.02.23 according to request	
        Delay(10);
				adc_reading_interval = 1;
        
				if(laserSelect >= 0 && laserSelect <= 4)
					Set_905_pulse(userProfile.user_profile.p905DutyFreq & 0xFF00, userProfile.user_profile.p905DutyFreq & 0x00FF);
				else if(laserSelect >= 5 && laserSelect <= 8)
					Set_660_pulse(userProfile.user_profile.p660DutyFreq & 0xFF00, userProfile.user_profile.p660DutyFreq & 0x00FF);
				else
				{
					Set_905_pulse(0,0);
					Set_660_pulse(0,0);
				}
					
				if((userProfile.user_profile.p905DAC0Voltage > 0)||
					 (userProfile.user_profile.p905DAC1Voltage > 0)||
					 (userProfile.user_profile.p905DAC2Voltage > 0)||
					 (userProfile.user_profile.p905DAC3Voltage > 0)||
					 (userProfile.user_profile.p905DAC4Voltage > 0)||
					 (userProfile.user_profile.p905DAC5Voltage > 0)||
				   (userProfile.user_profile.p660DAC0Voltage > 0)||
				   (userProfile.user_profile.p660DAC1Voltage > 0)||
				   (userProfile.user_profile.p660DAC2Voltage > 0)||
				   (userProfile.user_profile.p660DAC3Voltage > 0))	
				{					
					Display_LaserOn();					
				}

				if(laserSelect == 0)      {Set_905_Feedback_pulse(DAC0_905_FEEDBACK);}
				else if(laserSelect == 1) {Set_905_Feedback_pulse(DAC1_905_FEEDBACK);}
				else if(laserSelect == 2) {Set_905_Feedback_pulse(DAC2_905_FEEDBACK);}
				else if(laserSelect == 3) {Set_905_Feedback_pulse(DAC3_905_FEEDBACK);}
				else if(laserSelect == 4) {Set_905_Feedback_pulse(DAC4_905_FEEDBACK);}
				else if(laserSelect == 5) {Set_660_Feedback_pulse(DAC0_660_FEEDBACK);}
				else if(laserSelect == 6) {Set_660_Feedback_pulse(DAC1_660_FEEDBACK);}
				else if(laserSelect == 7) {Set_660_Feedback_pulse(DAC2_660_FEEDBACK);}
				else if(laserSelect == 8) {Set_660_Feedback_pulse(DAC3_660_FEEDBACK);}
				else
				{
					Set_905_Feedback_pulse(0);
					Set_660_Feedback_pulse(0);
				}				
				Delay(10);				
#else				
				userProfile.user_profile.p905DAC4Voltage = Receive_Buffer[3]*256 + Receive_Buffer[4];		
				AutoTestTimeInterval = Receive_Buffer[7]*256 + Receive_Buffer[8];
								
				if(userProfile.user_profile.p905DAC4Voltage > 0)
					Power_V30_On();
				else
					Power_V30_Off();				
				
				DAC7718_OUT_VOLT(s905_DAC0,0);
				Delay(10);
				DAC7718_OUT_VOLT(s905_DAC1,0);
				Delay(10);
				DAC7718_OUT_VOLT(s905_DAC2,0);
				Delay(10);
				DAC7718_OUT_VOLT(s905_DAC3,0);
				Delay(10);
				DAC7718_OUT_VOLT(s905_DAC4,0);
				Delay(10);
				Power_VIN660_Off();  
				S660_DAC_OUT_VOLT(s660_DAC0, 0); 
				Delay(10);
				S660_DAC_OUT_VOLT(s660_DAC1, 0);
				Delay(10);
				S660_DAC_OUT_VOLT(s660_DAC2, 0);
				Delay(10);
				S660_DAC_OUT_VOLT(s660_DAC3, 0);
				Delay(10);
				DAC7718_OUT_VOLT(TENS_DAC, 0);
        Delay(10);
				FPGA_ENABLE();
				Delay(10);
				Reset_All_Registers_FPGA();                                             // Added by Jason Chen, 2015.02.23 according to request	
        Delay(10);
				adc_reading_interval = 1;
        
				DAC7718_OUT_VOLT(s905_DAC4,userProfile.user_profile.p905DAC4Voltage);
				Set_905_pulse(userProfile.user_profile.p905DutyFreq & 0xFF00, userProfile.user_profile.p905DutyFreq & 0x00FF);	            // 2015.03.02, modified
				if((userProfile.user_profile.p905DAC0Voltage > 0)||
					 (userProfile.user_profile.p905DAC1Voltage > 0)||
					 (userProfile.user_profile.p905DAC2Voltage > 0)||
					 (userProfile.user_profile.p905DAC3Voltage > 0)||
					 (userProfile.user_profile.p905DAC4Voltage > 0)||
					 (userProfile.user_profile.p905DAC5Voltage > 0))	
				{					
					Display_LaserOn();					
				}

			  Set_905_Feedback_pulse(DAC4_905_FEEDBACK);
#endif

        OneMs_count =  AutoTestTimeInterval;
				FeedBack905_Flag = 0;
				treatment_on_flag = 1;	
				
        Enable_Input(CHANNEL0, CH_ENABLE);
				Enable_Input(CHANNEL1, CH_DISABLE);		
				Enable_Input(CHANNEL2, CH_DISABLE);
		    Enable_Input(CHANNEL3, CH_DISABLE);		
				
				adc_reading_interval = 1;
        AMP_hold_flag = 0;				
			}
			else
			{
				adc_reading_interval = 0;
				Power_V30_Off();
				Power_VIN660_Off();
				Reset_All_Registers_FPGA();
				Delay(10);
				FPGA_DISABLE();
				DAC7718_OUT_VOLT(s905_DAC0,0);
				Delay(10);
				DAC7718_OUT_VOLT(s905_DAC1,0);
				Delay(10);
				DAC7718_OUT_VOLT(s905_DAC2,0);
				Delay(10);
				DAC7718_OUT_VOLT(s905_DAC3,0);
				Delay(10);
				DAC7718_OUT_VOLT(s905_DAC4,0);
				Delay(10);
				DAC7718_OUT_VOLT(s905_DAC5,0);
				Delay(10);	
				DAC7718_OUT_VOLT(TENS_DAC,0);		
				Delay(10);
				S660_DAC_OUT_VOLT(s660_DAC0, 0);//userProfile.user_profile.p660DAC0Voltage);// 2014.10.28
				Delay(10);
				S660_DAC_OUT_VOLT(s660_DAC1, 0);//userProfile.user_profile.p660DAC1Voltage);// 2014.10.28
				Delay(10);
				S660_DAC_OUT_VOLT(s660_DAC2, 0);//userProfile.user_profile.p660DAC2Voltage);// 2014.10.28
				Delay(10);
				S660_DAC_OUT_VOLT(s660_DAC3, 0);//userProfile.user_profile.p660DAC3Voltage);// 2014.10.28
				Delay(10);	
				Display_LaserOff();							
				treatment_on_flag = 0;	
        OneMs_count =  0;				
				//FeedBack905_Flag = 0;
			}		
			break;
	  case CMD_SET_ENABLE_30V:
			 //userProfile.user_profile.VIN_30V = Receive_Buffer[2]; // 2014.10.17
			 if(Receive_Buffer[2])
				 Power_V30_On();
			 else
				 Power_V30_Off();
			break;			 
	  case CMD_SET_ENABLE_VIN660:       // 2014.10.17
			 //userProfile.user_profile.VIN_660 = Receive_Buffer[2]; // 2014.10.17
			 if(Receive_Buffer[2])          // 2014.10.17
				 Power_VIN660_On();           // 2014.10.17
			 else                           // 2014.10.17  
				 Power_VIN660_Off();          // 2014.10.17 
			break;			 
		case CMD_SET_ENABLE_INPUT:
			   if(Receive_Buffer[2]&0x01)
					 Enable_Input(CHANNEL0, CH_ENABLE);
				 else
					 Enable_Input(CHANNEL0, CH_DISABLE);
			   if(Receive_Buffer[2]&0x02)
					 Enable_Input(CHANNEL1, CH_ENABLE);
				 else
					 Enable_Input(CHANNEL1, CH_DISABLE);
			   if(Receive_Buffer[2]&0x04)
					 Enable_Input(CHANNEL2, CH_ENABLE);
				 else
					 Enable_Input(CHANNEL2, CH_DISABLE);
			   if(Receive_Buffer[2]&0x08)
					 Enable_Input(CHANNEL3, CH_ENABLE);
				 else
					 Enable_Input(CHANNEL3, CH_DISABLE);				 
			break;
		case CMD_LOAD_USERPROFILE://???????????????????????????????????????????????????   time
		  
		  (void)memcpy((void*)&userProfile,(void*)&Receive_Buffer[2],sizeof(USER_PROFILE_T));
		
 			treatment_time_backup = userProfile.user_profile.treatment_time;
	    DisplayTreatmentTime(userProfile.user_profile.treatment_time);
		  //foreground_color = ColorArray[userProfile.user_profile.foreColor];       // Removed by Jason Chen, 2016_0627
		  background_color = ColorArray[userProfile.user_profile.backColor];
		
		  //userProfile.user_profile.foreColor = foreground_color;
		  //userProfile.user_profile.backColor = background_color;
			TreatmentOff();	
#if ENABLE_FLASH_PROFILE			
      if(myFlashWrite((uint8_t*)&userProfileFlash,(uint8_t*)&userProfile, sizeof(USER_PROFILE_T)) != PASSED)    // Changed by Jason Chen, 2015.03.24
#else				
		  if(myFlashWrite((uint8_t*)FLASH_STORE_BASE_ADDRESS,(uint8_t*)&userProfile, sizeof(USER_PROFILE_T)) != PASSED)				
#endif			
      {				
      }
		  NVIC_SystemReset();
			break;
			
    case CMD_SET_OFFSET:                                                   // Added by Jason Chen, 2015.09.13
		  //(void)memcpy((void*)&probeOffset,(void*)&Receive_Buffer[2],sizeof(PROBE_OFFSET_T));
		  (void)memcpy((void*)&probeOffset,(void*)&Receive_Buffer[2],sizeof(PROBE_OFFSET_T)-8);          // Don't overwrite feedback offset, 2015.09.30
#if 0//ENABLE_FLASH_PROFILE			
      if(myFlashWrite((uint8_t*)&probeOffsetFlash,        (uint8_t*)&probeOffset, sizeof(PROBE_OFFSET_T)) != PASSED) 
#else				
		  if(myFlashWrite((uint8_t*)FLASH_OFFSET_BASE_ADDRESS,(uint8_t*)&probeOffset, sizeof(PROBE_OFFSET_T)) != PASSED)				
#endif			
      {				
      }
		  NVIC_SystemReset();			
			break;	
    case CMD_SET_FEEDBACK_OFFSET:                                                                    // Added by Jason Chen, 2015.09.30
		  //(void)memcpy((void*)&probeOffset,(void*)&Receive_Buffer[2],sizeof(PROBE_OFFSET_T));
		  (void)memcpy((void*)&usbFeedbackOffset,(void*)&Receive_Buffer[2],sizeof(USB_FEEDBACK_OFFSET)); // Don't overwrite feedback offset, 2015.09.30
		  if(usbFeedbackOffset.ch_sel == 0)
			{
				probeOffset.probe_offset.feedbackCh0Offset = usbFeedbackOffset.feedbackOffset[0];
		    feedBack_profile.feed_back_buf.offset[0] = probeOffset.probe_offset.feedbackCh0Offset;       // Added by Jason Chen, 2015.09.30
			}
			else if(usbFeedbackOffset.ch_sel == 1)
			{
				probeOffset.probe_offset.feedbackCh1Offset = usbFeedbackOffset.feedbackOffset[1];
		    feedBack_profile.feed_back_buf.offset[1] = probeOffset.probe_offset.feedbackCh1Offset;       // Added by Jason Chen, 2015.09.30
			}
			else if(usbFeedbackOffset.ch_sel == 2)
			{
				probeOffset.probe_offset.feedbackCh2Offset = usbFeedbackOffset.feedbackOffset[2];
		    feedBack_profile.feed_back_buf.offset[2] = probeOffset.probe_offset.feedbackCh2Offset;       // Added by Jason Chen, 2015.09.30
			}
			else if(usbFeedbackOffset.ch_sel == 3)
			{
				probeOffset.probe_offset.feedbackCh3Offset = usbFeedbackOffset.feedbackOffset[3];
			  feedBack_profile.feed_back_buf.offset[3] = probeOffset.probe_offset.feedbackCh3Offset;       // Added by Jason Chen, 2015.09.30		 												
			}
			else
				break;
			
#if 0//ENABLE_FLASH_PROFILE			
      if(myFlashWrite((uint8_t*)&probeOffsetFlash,        (uint8_t*)&probeOffset, sizeof(PROBE_OFFSET_T)) != PASSED) 
#else				
		  if(myFlashWrite((uint8_t*)FLASH_OFFSET_BASE_ADDRESS,(uint8_t*)&probeOffset, sizeof(PROBE_OFFSET_T)) != PASSED)				
#endif			
      {				
      }
		  //NVIC_SystemReset();			                                                 // Removed by Jason Chen, 2015.10.21
			break;				
    case CMD_SET_CALIBRATION:                             // Added by Jason Chen, 2015.09.13
			voltage = Receive_Buffer[3]*256 + Receive_Buffer[4];
			Calibration_setup(Receive_Buffer[2],voltage);               // Added by Jason Chen, 2015.09.17
		  break;
    case CMD_START_TREAT:   //???????????????????????????   
			if(Receive_Buffer[2])
			{
				 if(armed_flag)
				 {
			     treatment_time = treatment_time_backup;//userProfile.user_profile.treatment_time;
				   switch_stm_status = SWITCH_RUNNING;
				
				   TreatmentOn();
				 }
			}
			else
			{
				if(treatment_time > 0 )
				{
				  treatment_time_backup = treatment_time;
				  treatment_time = 0;
					TreatmentOff();
				}
			}
			Send_Buffer[3] = (uint8_t)(treatment_time_backup >> 8);
			Send_Buffer[4] = (uint8_t)(treatment_time_backup >> 0);
			Send_Buffer[5] = (uint8_t)(treatment_time >> 8);
			Send_Buffer[6] = (uint8_t)(treatment_time >> 0);			
      break;			
		default:
		  //Set_660_Feedback_pulse(Receive_Buffer[2] & 0x0F);			
		  resp = 0xff;
		  Send_Buffer[1] = 0xFF;
			break;		
	}
	
	if(Receive_Buffer[1] == CMD_TIME_SETUP)
	{
		Send_Buffer[1] = 0xF8;
	}
	
  //if(Receive_Buffer[9] == 0)	
	  Usb_Send();
}

void Usb_Send(void)
{
	  if ((PrevXferComplete) && (bDeviceState == CONFIGURED))
    {
      Send_Buffer[0] = REPORT_ID_5; 
    
      /* Write the descriptor through the endpoint */
      USB_SIL_Write(EP1_IN, (uint8_t*) Send_Buffer, MAX_TRANSFER_LEN);  
			SetEPTxValid(ENDP1);
      PrevXferComplete = 0;  
    }
}
