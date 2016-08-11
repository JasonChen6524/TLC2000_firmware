
#include <stdio.h>
#include <string.h>
#include "calibration.h"
#include "TLC2000_SPI_LCD_TFT.h"
#include "FPGA_RAM.h"
#include "Buzzer.h"                   // Added by Jason Chen, 2015.03.24
#include "CPU_ADC.h"                  // Added by Jason Chen, 2015.09.30

//typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

//#define FLASH_PAGE_SIZE           ((uint16_t)0x800)               // Commented by Jason Chen, 2015.09.13
//#define FLASH_STORE_BASE_ADDRESS  ((uint32_t)0x0807F800)

static uint32_t NbrOfPage = 0x00;
static uint32_t EraseCounter = 0x00, Address = 0x00;

static volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
static volatile TestStatus MemoryProgramStatus = PASSED;

__IO uint16_t probeMode = 0;
USER_PROFILE_T userProfile;
#if ENABLE_FLASH_PROFILE	
const USER_PROFILE_T userProfileFlash __attribute__((at(FLASH_STORE_BASE_ADDRESS)));// ={0,0,0,0};    
const PROBE_OFFSET_T probeOffsetFlash __attribute__((at(FLASH_OFFSET_BASE_ADDRESS)));// ={0,0,0,0};    
#endif

/*******************************************************************/               // Added by Jason Chen, 2015.09.13
PROBE_OFFSET_T probeOffset;
USB_FEEDBACK_OFFSET usbFeedbackOffset;                  // Added by Jason Chen, 2015.09.30

//const PROBE_OFFSET_T probeOffsetFlash __attribute__((at(FLASH_OFFSET_BASE_ADDRESS)));    
/*******************************************************************/

uint8_t myFlashWrite(uint8_t *addr, uint8_t *src, uint8_t len)
{
  uint8_t i;
  uint32_t Address_t;
  USER_PROFILE_T *sourc_data;
//MODE_SETTING_T *flash_data;  
  
  
  FLASH_UnlockBank1();

  /* Define the number of page to be erased */
  NbrOfPage = len / FLASH_PAGE_SIZE + 1;

  /* Clear All pending flags */
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	

  Address_t = (uint32_t)&(*addr);
  /* Erase the FLASH pages */
  for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
  {
    FLASHStatus = FLASH_ErasePage(Address_t + (FLASH_PAGE_SIZE * EraseCounter));
  }
  
  /* Program Flash Bank1 */
  Address = Address_t;
  
  sourc_data = (USER_PROFILE_T *)src;
  i = 0;
  while((Address < Address_t+len) && (FLASHStatus == FLASH_COMPLETE))
  {
    FLASHStatus = FLASH_ProgramWord(Address, sourc_data->flash_word[i]);
    i++;
    Address = Address + 4;
  }

  FLASH_LockBank1();
  
/* Check the correctness of written data */
  Address = Address_t;
  i = 0;
//flash_data = (MODE_SETTING_T *)Address;
  while((Address < Address_t+len) && (MemoryProgramStatus != FAILED))
  {
    if((*(__IO uint32_t*) Address) != sourc_data->flash_word[i])
//  if(flash_data->flash_word[i] != sourc_data->flash_word[i])
    {
      MemoryProgramStatus = FAILED;
    }
    i++;
    Address += 4;
  }
  return MemoryProgramStatus;
  
}
#define CALIBRATION_905 FREQ_10000HZ|PULSE_WIDTH_200
#define CALIBRATION_660 FREQ_10000HZ|PULSE_WIDTH_200

void userProfileInit(void) 
{
	uint8_t storeFlag = 0;
   //(void)memcpy((void*)&userProfile,(void*)&userProfileFlash, sizeof(USER_PROFILE_T));
	 (void)memcpy((void*)&userProfile,(void*)FLASH_STORE_BASE_ADDRESS, sizeof(USER_PROFILE_T));	
   if(userProfile.user_profile.signature != MODE_SIGNATURE)
   {
		  userProfile.user_profile.signature   = MODE_SIGNATURE;
      userProfile.user_profile.p905DAC0Voltage = 0;//3000;//4095;    // indicate all 905 same if D15 == 1
      userProfile.user_profile.p905DAC1Voltage = 0;
	    userProfile.user_profile.p905DAC2Voltage = 0;
	    userProfile.user_profile.p905DAC3Voltage = 0;
	    userProfile.user_profile.p905DAC4Voltage = 0;//2800;   // Changed by Jason Chen, 2015.03.24
	    userProfile.user_profile.p905DAC5Voltage = 0;
	
      userProfile.user_profile.p660DAC0Voltage = 0;           // Changed by Jason Chen, 2015.03.24
      userProfile.user_profile.p660DAC1Voltage = 0;// 4095;   // Changed by Jason Chen, 2015.03.24
	    userProfile.user_profile.p660DAC2Voltage = 0;// 4095;   // Changed by Jason Chen, 2015.03.24
	    userProfile.user_profile.p660DAC3Voltage = 0;// 4095;   // Changed by Jason Chen, 2015.03.24
	
	    userProfile.user_profile.p905DutyFreq = CALIBRATION_905;
	    userProfile.user_profile.p660DutyFreq = CALIBRATION_905;
		 
		  userProfile.user_profile.TensEnable = TENS_FREQ_15HZ ;     // 2014.10.21,Reset Tens
	
	    userProfile.user_profile.p905CadenceDutyFreq = 0x0000;
		  userProfile.user_profile.p660CadenceDutyFreq = 0x0000;
		
	    //userProfile.user_profile.PotPD0 = 1;    //--->gain = 1
		 // userProfile.user_profile.PotPD1 = 0x0A; //--->gain = 10
      userProfile.user_profile.buzzerVol = VOLUME_VOLT_DEFAULT;       // Added by Jason Chen, 2015.03.24
	    //userProfile.user_profile.PotPD2 = 4;    //--->gain = 4
	    //userProfile.user_profile.PotPD3 = 0x11; //--->gain = 16
			userProfile.user_profile.probeTemperature = 0;
			userProfile.user_profile.PotAMP2= 0;    //--->gain = 1
		
	    userProfile.user_profile.p905Feedback_pulse = 0x00;
	    userProfile.user_profile.p660Feedback_pulse = 0x00;
	
	    userProfile.user_profile.pTensDACVoltage = 0;//4095;//1024 * 3;//0x0000;
	    userProfile.user_profile.mode = PATIENT_TREATMENT_MODE;
			//userProfile.user_profile.mode = PATIENT_MEASUREMENT_MODE;

			userProfile.user_profile.backColor = COLOR_BLACK;
			userProfile.user_profile.foreColor = COLOR_BLUE;
			
		  if(userProfile.user_profile.mode == PATIENT_MEASUREMENT_MODE)
				 userProfile.user_profile.treatment_time = 30;
			else
				 userProfile.user_profile.treatment_time = 60;			
			 //userProfile.user_profile.treatment_time = 0;//30;//34608;//*60 + 30;
			
			userProfile.user_profile.BootFlag = 0;
			
#if ENABLE_FLASH_PROFILE	
     if(myFlashWrite((uint8_t*)&userProfileFlash,(uint8_t*)&userProfile, sizeof(USER_PROFILE_T)) != PASSED)    // Changed by Jason Chen, 2015.03.24
#else			 
	   if(myFlashWrite((uint8_t*)FLASH_STORE_BASE_ADDRESS,(uint8_t*)&userProfile, sizeof(USER_PROFILE_T)) != PASSED)			 
#endif		 
     {
     }
   }
	 
	 // Verify fore color, background color and mode value, it will setup as default value if error, 2015.05.04
	 //if(userProfile.user_profile.backColor == userProfile.user_profile.foreColor)
	 if(userProfile.user_profile.foreColor == COLOR_SILVER)      // Added by Jason Chen, 2015_0627
	 {
		  userProfile.user_profile.backColor = COLOR_BLACK;
	    userProfile.user_profile.foreColor = COLOR_BLUE;//SILVER // Changed by Jason Chen, 2015_0627  
		  storeFlag = 1;
	 }
	 
	 if((userProfile.user_profile.mode != PATIENT_TREATMENT_MODE) && 
		  (userProfile.user_profile.mode != PATIENT_MEASUREMENT_MODE) && 
	    (userProfile.user_profile.mode != PROBE_MODE_SELTEST))                // Added by Jason Chen, 2015.10.07
	 {
		 userProfile.user_profile.mode = PATIENT_TREATMENT_MODE;
		 userProfile.user_profile.treatment_time = 60;
		 storeFlag = 1;
	 }
	 
	 if(storeFlag)
	 {
#if ENABLE_FLASH_PROFILE	
     if(myFlashWrite((uint8_t*)&userProfileFlash,(uint8_t*)&userProfile, sizeof(USER_PROFILE_T)) != PASSED)    // Changed by Jason Chen, 2015.03.24
#else			 
	   if(myFlashWrite((uint8_t*)FLASH_STORE_BASE_ADDRESS,(uint8_t*)&userProfile, sizeof(USER_PROFILE_T)) != PASSED)			 
#endif		 
     {
     }		 
	 }
	 // Verify fore color, background color and mode value, it will setup as default value if error, 2015.05.04
	 
}

int16_t L_offset[5];
int16_t O_offset[4];
void probeOffsetInit(void)                                         // Added by Jason Chen, 2015.09.13
{
 	 (void)memcpy((void*)&probeOffset,(void*)FLASH_OFFSET_BASE_ADDRESS, sizeof(PROBE_OFFSET_T));	
   if(probeOffset.probe_offset.signature != MODE_SIGNATURE)		 
   {
		  probeOffset.probe_offset.signature   = MODE_SIGNATURE;
      probeOffset.probe_offset.p905DAC0VoltOffset = 0;
      probeOffset.probe_offset.p905DAC1VoltOffset = 0;
	    probeOffset.probe_offset.p905DAC2VoltOffset = 0;
	    probeOffset.probe_offset.p905DAC3VoltOffset = 0;
	    probeOffset.probe_offset.p905DAC4VoltOffset = 0;
	    probeOffset.probe_offset.p905DAC5VoltOffset = 0;
	
      probeOffset.probe_offset.p660DAC0VoltOffset = 0;
      probeOffset.probe_offset.p660DAC1VoltOffset = 0;
	    probeOffset.probe_offset.p660DAC2VoltOffset = 0;
	    probeOffset.probe_offset.p660DAC3VoltOffset = 0;
		 
		  probeOffset.probe_offset.feedbackCh0Offset = 0;                  // Added by Jason Chen, 2015.09.30
		  probeOffset.probe_offset.feedbackCh1Offset = 0;                  // Added by Jason Chen, 2015.09.30  
		  probeOffset.probe_offset.feedbackCh2Offset = 0;                  // Added by Jason Chen, 2015.09.30 
		  probeOffset.probe_offset.feedbackCh3Offset = 0;                  // Added by Jason Chen, 2015.09.30
		 
		  feedBack_profile.feed_back_buf.offset[0] = 0;                    // Added by Jason Chen, 2015.09.30
		  feedBack_profile.feed_back_buf.offset[1] = 0;                    // Added by Jason Chen, 2015.09.30
		  feedBack_profile.feed_back_buf.offset[2] = 0;                    // Added by Jason Chen, 2015.09.30
			feedBack_profile.feed_back_buf.offset[3] = 0;                    // Added by Jason Chen, 2015.09.30								
			
#if 0//ENABLE_FLASH_PROFILE	
     if(myFlashWrite((uint8_t*)&probeOffsetFlash,        (uint8_t*)&probeOffset, sizeof(PROBE_OFFSET_T)) != PASSED)
#else			 
     if(myFlashWrite((uint8_t*)FLASH_OFFSET_BASE_ADDRESS,(uint8_t*)&probeOffset, sizeof(PROBE_OFFSET_T)) != PASSED)		 
#endif		 
     {
     }
   }
	 else
	 {		 
	    L_offset[0] = probeOffset.probe_offset.p905DAC0VoltOffset;
	    L_offset[1] = probeOffset.probe_offset.p905DAC1VoltOffset;
	    L_offset[2] = probeOffset.probe_offset.p905DAC2VoltOffset;
	    L_offset[3] = probeOffset.probe_offset.p905DAC3VoltOffset;
	    L_offset[4] = probeOffset.probe_offset.p905DAC4VoltOffset;
		 
	    O_offset[0] = probeOffset.probe_offset.feedbackCh0Offset;
	    O_offset[1] = probeOffset.probe_offset.feedbackCh1Offset;
	    O_offset[2] = probeOffset.probe_offset.feedbackCh2Offset;
	    O_offset[3] = probeOffset.probe_offset.feedbackCh3Offset;
		 
		 #if 1
		  feedBack_profile.feed_back_buf.offset[0] = probeOffset.probe_offset.feedbackCh0Offset;    // Added by Jason Chen, 2015.09.30
		  feedBack_profile.feed_back_buf.offset[1] = probeOffset.probe_offset.feedbackCh1Offset;    // Added by Jason Chen, 2015.09.30
		  feedBack_profile.feed_back_buf.offset[2] = probeOffset.probe_offset.feedbackCh2Offset;    // Added by Jason Chen, 2015.09.30
			feedBack_profile.feed_back_buf.offset[3] = probeOffset.probe_offset.feedbackCh3Offset;    // Added by Jason Chen, 2015.09.30		 
		 #else		 
      probeOffset.probe_offset.p905DAC0VoltOffset = 0;                 // Added by Jason Chen, 2016.08.09
      probeOffset.probe_offset.p905DAC1VoltOffset = 0;                 // Added by Jason Chen, 2016.08.09
	    probeOffset.probe_offset.p905DAC2VoltOffset = 0;                 // Added by Jason Chen, 2016.08.09
	    probeOffset.probe_offset.p905DAC3VoltOffset = 0;                 // Added by Jason Chen, 2016.08.09
	    probeOffset.probe_offset.p905DAC4VoltOffset = 0;                 // Added by Jason Chen, 2016.08.09
	    probeOffset.probe_offset.p905DAC5VoltOffset = 0;                 // Added by Jason Chen, 2016.08.09
	      
      probeOffset.probe_offset.p660DAC0VoltOffset = 0;                 // Added by Jason Chen, 2016.08.09
      probeOffset.probe_offset.p660DAC1VoltOffset = 0;                 // Added by Jason Chen, 2016.08.09
	    probeOffset.probe_offset.p660DAC2VoltOffset = 0;                 // Added by Jason Chen, 2016.08.09
	    probeOffset.probe_offset.p660DAC3VoltOffset = 0;                 // Added by Jason Chen, 2016.08.09
		 
		  probeOffset.probe_offset.feedbackCh0Offset = 0;                  // Added by Jason Chen, 2016.08.09
		  probeOffset.probe_offset.feedbackCh1Offset = 0;                  // Added by Jason Chen, 2016.08.09
		  probeOffset.probe_offset.feedbackCh2Offset = 0;                  // Added by Jason Chen, 2016.08.09
		  probeOffset.probe_offset.feedbackCh3Offset = 0;                  // Added by Jason Chen, 2016.08.09
		 
		  feedBack_profile.feed_back_buf.offset[0] = 0;                    // Added by Jason Chen, 2016.08.09
		  feedBack_profile.feed_back_buf.offset[1] = 0;                    // Added by Jason Chen, 2016.08.09
		  feedBack_profile.feed_back_buf.offset[2] = 0;                    // Added by Jason Chen, 2016.08.09
			feedBack_profile.feed_back_buf.offset[3] = 0;                    // Added by Jason Chen, 2016.08.09
		 #endif
	 }
	 
	 feedBack_profile.feed_back_buf.validF = 0;                                                   // Added by Jason Chen, 2015.10.07
}

