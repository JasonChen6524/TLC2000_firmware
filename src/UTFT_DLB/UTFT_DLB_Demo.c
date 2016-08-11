//***************************************************************
// UTFT_DLB Demo
//***************************************************************

#include <stdio.h>
#include "UTFT.h"
#include "UTFT_DLB.h"
#include "TLC2000_SPI_LCD_TFT.h"
#include "calibration.h"
#include "RTC.h"
#include "ADS8319_ADC.h"

#include "CPU_ADC.h"

// Declare which fonts we will be using
extern uint8_t DejaVuSans18[];


char displayBuff_ttf[20];
const char FirmwareVersion[] = {"V 1.9.35B"};           // Changed by Jason from "1.9.13  to 1.9.14, 2015.06.02// Added by Jason Chen, 2015.05.04
                                                        // Changed by Jason from "1.9.15T to 1.9.16T, 2015.07.03
                                                        // Changed by Jason from "1.9.16T to 1.9.17T, 2015.07.10
                                                        // Changed by Jason from "1.9.17T to 1.9.18T, 2015.07.2
                                                        // Changed by Jason from "1.9.17T to 1.9.28T, 2016.02.03
                                                        // Changed by Jason from "1.9.28T to 1.9.29T, 2016.02.18   
                                                        // Changed by Jason from "1.9.28T to 1.9.29T, 2016.03.18
                                                        // Changed by Jason from "1.9.30T to 1.9.31T, 2016.05.12
                                                        // Changed by Jason from "1.9.31T to 1.9.32T, 2016.05.16
                                                        // Changed by Jason from "1.9.32T to 1.9.33T, 2016.05.18
                                                        // Changed by Jason from "1.9.33T to 1.9.34R, 2016.07.28
                                                        // Changed by Jason from "1.9.34R to 1.9.35B, 2016.08.03
//*************************************************************************************************
// Setup
//*************************************************************************************************

void setup()
{
  //randomSeed(analogRead(0));

  //myGLCD.InitLCD(LANDSCAPE);
	 InitLCD();
}

//******************************************************************************************************************
// Main loop
//******************************************************************************************************************
void Mode_GUI_Init(uint8_t mode)
{
	  //uint16_t maxPower=0;
	  //uint16_t maxPower660=0;
	  Delay(100);
      clrScr();		

		DrawSmallLogoFlip(0,235);
		sLCD_putString_TFT(BT_LOCATION,305, (uint8_t*)"BT     ",Font12x12);
	//sLCD_putString_TFT(10,282,(uint8_t*)"V1.9.1",Font16x24);
	//print_ttf("V 1.9.12", 10, 282, 0);	
	  print_ttf((char*)&FirmwareVersion[0], 10, 278/*282*/, 0);
	#if 0
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
		
		maxPower660 = userProfile.user_profile.p660DAC0Voltage;
		if(maxPower660 < userProfile.user_profile.p660DAC1Voltage)
			maxPower660 = userProfile.user_profile.p660DAC1Voltage;
		if(maxPower660 < userProfile.user_profile.p660DAC2Voltage)
			maxPower660 = userProfile.user_profile.p660DAC2Voltage;
		if(maxPower660 < userProfile.user_profile.p660DAC3Voltage)
			maxPower660 = userProfile.user_profile.p660DAC3Voltage;		
	#endif		
		//Display_Power(maxPower & 0x0FFF);	
		//DisplayPowermW(userProfile.user_profile.powerValue_mW);
	    DisplayPowermW(0);

		DisplayTreatmentTime(0);
		if(mode == PATIENT_TREATMENT_MODE)
		{										
			Display_TensPower(userProfile.user_profile.pTensDACVoltage  & 0x0FFF);
			//if(((maxPower > 0)||(maxPower660)||(userProfile.user_profile.pTensDACVoltage  > 0))&&
			 //   (userProfile.user_profile.treatment_time > 0))
			//	Display_Armed();
			//else
			Display_Idle();		    
			Display_LaserOff();
			Display_TENsOff();                   // Added by Jason Chen, 2014.11.11
			
			//print_ttf("-->Treatment",120,3,0);	
			//sLCD_putString_TFT_ext(120,3,(uint8_t*)"->Treatment", Arial_Normal16x16);
		
	  }
		else if(mode == PATIENT_MEASUREMENT_MODE)
		{
			//if((userProfile.user_profile.p905DAC4Voltage > 0)&&(userProfile.user_profile.treatment_time > 0))
			//	Display_Armed();
			//else
				Display_Idle();		    
			Display_LaserOff();

#if 0			
			sprintf((char*)displayBuff_ttf,"CH0:%4.2f  CH1:%4.2f", 0.0,0.0);            // Added by Jason Chen, 2015.03.04
      print_ttf((char*)displayBuff_ttf, 20, 55, 0);		
			
			sprintf((char*)displayBuff_ttf,"CH2:%4.2f  CH3:%4.2f", 0.0,0.0);            // Added by Jason Chen, 2015.03.04						
      print_ttf((char*)displayBuff_ttf, 20, 30, 0);					
#endif			
		}
		else
		{
				Display_Idle();		                                                        // Added by Jason Chen, 2015.05.04   
  			Display_LaserOff();                                                       // Added by Jason Chen, 2015.05.04    
		}
		
  //Display_workingMode(userProfile.user_profile.mode);
		Display_workingMode(mode);
		
    Display_BT_RSSI(50);	
    //Delay(2000);
	  setColorRGB(0, 0, 255);
    //drawRect(0, 0, 239, 319);
	  //setColor(ColorArray[userProfile.user_profile.foreColor]);
		setColor(ColorArray[COLOR_SILVER]);                             // Added by Jason Chen for color probe, 20160627
}

#define REF_VOLT       3.0f
#define FULL_SCALE     4095.0f
void Self_Cali_Clr(void)
{
	  Delay(100);
    clrScr();		

		DrawSmallLogoFlip(0,235);
		sLCD_putString_TFT(BT_LOCATION,305, (uint8_t*)"BT     ",Font12x12);
	  print_ttf((char*)&FirmwareVersion[0], 10, 278/*282*/, 0);

		
   //Display_workingMode(mode);
	 //Display_Prompt(PROMPT_CALI);
	#if 0
	  DisplaySelfCali(1, (10.0f*REF_VOLT*L_offset[0])/FULL_SCALE);
	  DisplaySelfCali(2, (10.0f*REF_VOLT*L_offset[1])/FULL_SCALE);	
	  DisplaySelfCali(3, (10.0f*REF_VOLT*L_offset[2])/FULL_SCALE);	
	  DisplaySelfCali(4, (10.0f*REF_VOLT*L_offset[3])/FULL_SCALE);	
	  DisplaySelfCali(5, (10.0f*REF_VOLT*L_offset[4])/FULL_SCALE);	
	#elif 1
	  DisplaySelfCali(1, (REF_VOLT*O_offset[3])/FULL_SCALE,1);	
	  DisplaySelfCali(2, (REF_VOLT*O_offset[2])/FULL_SCALE,1);	
	  DisplaySelfCali(3, (REF_VOLT*O_offset[3])/FULL_SCALE,1);	
	  DisplaySelfCali(4, (REF_VOLT*O_offset[1])/FULL_SCALE,1);	
	  DisplaySelfCali(5, (REF_VOLT*O_offset[0])/FULL_SCALE,1);	
  #endif	
    Display_BT_RSSI(50);	
	  setColorRGB(0, 0, 255);
    //drawRect(0, 0, 239, 319);
		setColor(ColorArray[COLOR_SILVER]);                             // Added by Jason Chen for color probe, 20160627
}

void Mode_GUI_Fresh(uint8_t mode)
{
	  static uint16_t maxPower=0;
	  uint16_t maxPower660=0;
	  Delay(100);

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
			
		maxPower660 = userProfile.user_profile.p660DAC0Voltage;
		if(maxPower660 < userProfile.user_profile.p660DAC1Voltage)
			maxPower660 = userProfile.user_profile.p660DAC1Voltage;
		if(maxPower660 < userProfile.user_profile.p660DAC2Voltage)
			maxPower660 = userProfile.user_profile.p660DAC2Voltage;
		if(maxPower660 < userProfile.user_profile.p660DAC3Voltage)
			maxPower660 = userProfile.user_profile.p660DAC3Voltage;		
		
		//Display_Power(maxPower & 0x0FFF);	
		
		DisplayTreatmentTime(userProfile.user_profile.treatment_time);	  
		if(mode == PATIENT_TREATMENT_MODE)
		{		
		    DisplayPowermW(userProfile.user_profile.powerValue_mW);
			if(((maxPower > 0)||(maxPower660)||(userProfile.user_profile.pTensDACVoltage  > 0))&&
			    (userProfile.user_profile.treatment_time > 0))
				Display_Armed();
			else
				Display_Idle();		    
	    }
		else if(mode == PATIENT_MEASUREMENT_MODE)
		{		
		    DisplayNoPowermW();
            if((userProfile.user_profile.p905DAC4Voltage > 0)&&(userProfile.user_profile.treatment_time > 0))
				Display_Armed();
			else
				Display_Idle();		    
		}
		else if(mode == PROBE_MODE_SELTEST)             // Added by Jason Chen, 2015.10.07
		{
			DisplayTreatmentTime(0);
			Display_SlefTestMode();                       // Added by Jason Chen, 2015.10.07
		}
		else// if(mode == PROBE_MODE_IDEL)              // Modified by Jason Chen, 2015.05.04
		{	
			DisplayPowermW(0);
			Display_Idle();	

		}
		Display_LaserOff();
		Display_TENsOff();                   // Added by Jason Chen, 2014.11.11						
		Display_TensPower(userProfile.user_profile.pTensDACVoltage  & 0x0FFF);		
		Display_workingMode(mode);		
	  setColorRGB(0, 0, 255);
    //drawRect(0, 0, 239, 319);
	  //setColor(ColorArray[userProfile.user_profile.foreColor]);
		setColor(ColorArray[COLOR_SILVER]);                             // Added by Jason Chen for color probe, 20160627
}

void Screen_Display(void)
{
  //while (1)
  {
    //Delay(1500);		
    setColor(foreground_color);//VGA_WHITE);
    setBackColor(background_color);
    clrScr();				
		DrawSmallLogoFlip(0,235);                   // 2014.12.09
		Delay(1000-75);                             // 2014.12.09
    //sLCD_putString_TFT_D(3, 206, (uint8_t*)"Loading Version 1.8", Font16x24, 75);
		//sLCD_putString_TFT_D(3, 206, (uint8_t*)"Loading Version 1.8", Font12x12, 75);
		
    setFont(DejaVuSans18);		
		//print_ttf("Loading Version 1.9.12", 3, 206, 0);		
		print_ttf("Loading Version ", 3, 206, 0);		
		print_ttf((char*)&FirmwareVersion[2], 165, 206, 0);
		
		Delay(1000-75);
		//sLCD_putString_TFT_D(3+16*7-4, 206, (uint8_t*)"........", Font16x24, 1000);	
		
    //Display_ProgressBar(5, 160, 100);           // 2014.12.09
		
		//Delay(200);
	  //sLCD_putString_TFT_D(5,230-60, (uint8_t*)"User Profile",Font16x24,75);
		//Delay(3000);
	  //sLCD_putString_TFT_D(5,194-60, (uint8_t*)"Load Finished!",Font16x24,75);
		//sLCD_putString_TFT_D(5,134, (uint8_t*)"Load Finished!",Font16x24,75);		
		print_ttf("Load Finished!", 5, 120, 0);		
		//Delay(1000);
	  //sLCD_putString_TFT_D(5,158-60,(uint8_t*)"TheraLase V1.8",Font16x24,75);
		
    Delay(2500);
    clrScr();		

		//DrawSmallLogoFlip(0,235);
		//sLCD_putString_TFT(BT_LOCATION,300, (uint8_t*)"BT     ",Font12x12);
		//sLCD_putString_TFT(10,282,(uint8_t*)"V1.9DMA",Font16x24);

    Mode_GUI_Init(userProfile.user_profile.mode);

    //Display_BT_RSSI(50);	
    //Delay(2000);
	  //setColorRGB(0, 0, 255);
    //drawRect(0, 0, 239, 319);
	  //setColor(ColorArray[userProfile.user_profile.foreColor]);			
  }
}


