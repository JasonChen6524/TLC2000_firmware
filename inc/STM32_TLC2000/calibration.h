/***************************************************************************
 *
 *            Copyright (c) 2012 by Artafelx INC.
 *
 * This software is copyrighted by and is the sole property of
 * Artaflex INC.  All rights, title, ownership, or other interests
 * in the software remain the property of Artaflex INC.  This
 * software may only be used in accordance with the corresponding
 * license agreement.  Any unauthorized use, duplication, transmission,
 * distribution, or disclosure of this software is expressly forbidden.
 *
 * This Copyright notice may not be removed or modified without prior
 * written consent of Artaflex INC.
 *
 * Artaflex INC reserves the right to modify this software without notice.
 *
 * Artaflex INC.
 * 96 Steelcase Road West
 * Markham, ON L3R 3J9
 * Canada
 *
 * Tel:   (905) 470-0109
 * http:  www.artaflex.com
 * email: Jason_Chen@artaflex.com
 *
 ***************************************************************************/

#ifndef _CALIBRATION_H_
#define _CALIBRATION_H_


#include "stm32f10x.h"

#define MODE_SIGNATURE        0x4545
#define BIT(x) ( 1<<(x) )

#define FLASH_PAGE_SIZE           ((uint16_t)0x800)            // Added by Jason Chen, 2015.09.13
//#define LARGE_LOGO_BASE_ADDRESS   ((uint32_t)0x8052800)      // Added by Jason Chen, 2015.09.13
#define LARGE_LOGO_BASE_ADDRESS   ((uint32_t)(0x8052800-0x800))// Added by Jason Chen, 2015.10.19

typedef struct
{
	uint16_t signature;
  uint16_t p905DAC0Voltage;    // indicate all 905 same if D15 == 1
  uint16_t p905DAC1Voltage;
	uint16_t p905DAC2Voltage;
	uint16_t p905DAC3Voltage;
	uint16_t p905DAC4Voltage;
	uint16_t p905DAC5Voltage;
	
  uint16_t p660DAC0Voltage;    // indicate all 660 same if D15 == 1
  uint16_t p660DAC1Voltage;
	uint16_t p660DAC2Voltage;
	uint16_t p660DAC3Voltage;
	
	uint16_t p905DutyFreq;
	uint16_t p660DutyFreq;
	
	uint16_t p905CadenceDutyFreq;
	uint16_t p660CadenceDutyFreq;
		
	uint16_t buzzerVol; // voltage for the buzzer 12 bits
	//uint8_t  PotPD2;
	//uint8_t  PotPD3;
	uint16_t probeTemperature;                                   // Added by Jason Chen, 2015.04.09
	
	uint8_t  p905Feedback_pulse;
	uint8_t  p660Feedback_pulse;
	
	int32_t  treatment_time;
	uint16_t pTensDACVoltage;
	
	uint16_t backColor;
	uint16_t foreColor;
	uint8_t  BootFlag;
	uint8_t  RESV0;
	
	uint16_t TensEnable;	
	uint8_t  PotAMP2;
	uint8_t  mode;//sensitivity;	
	
	//uint16_t adc_reading;
	//uint16_t userID;
	float powerValue_mW;
}USER_PROFILE;


typedef union
{  
  USER_PROFILE user_profile;
  uint32_t flash_word[sizeof(USER_PROFILE)/4];    
	uint8_t  flash_byte[sizeof(USER_PROFILE)];    
} USER_PROFILE_T;

enum
{
	PATIENT_TREATMENT_MODE   = 0,
	PATIENT_MEASUREMENT_MODE = 1,
	DOCTOR_CALIBRATION       = 2,
	FIRMWARE_UPDATE          = 3,
	PROBE_MODE_IDEL          = 4,
	PROBE_MODE_SELTEST       = 5                                     // Added by Jason Chen, 2015.10.07
};

#define FLASH_STORE_BASE_ADDRESS  ((uint32_t)0x0807F800)
#define ENABLE_FLASH_PROFILE    0
/***************************************************/              // Added by Jason Chen, 2013.09.13
typedef struct
{
	uint16_t signature;
  int16_t p905DAC0VoltOffset;  
  int16_t p905DAC1VoltOffset;
	int16_t p905DAC2VoltOffset;
	int16_t p905DAC3VoltOffset;
	int16_t p905DAC4VoltOffset;
	int16_t p905DAC5VoltOffset;
	
  int16_t p660DAC0VoltOffset;
  int16_t p660DAC1VoltOffset;
	int16_t p660DAC2VoltOffset;
	int16_t p660DAC3VoltOffset;
	int16_t RESV0;
	
  int16_t feedbackCh0Offset;
  int16_t feedbackCh1Offset;
	int16_t feedbackCh2Offset;
	int16_t feedbackCh3Offset;	
}PROBE_OFFSET;


typedef union
{  
  PROBE_OFFSET probe_offset;
  uint32_t flash_word[sizeof(PROBE_OFFSET)/4];    
	uint8_t  flash_byte[sizeof(PROBE_OFFSET)];    
} PROBE_OFFSET_T;

typedef struct
{
	int16_t ch_sel;
  int16_t feedbackOffset[4];
	int16_t RESV0;	
}USB_FEEDBACK_OFFSET;

//#define FLASH_OFFSET_BASE_ADDRESS  ((uint32_t)(LARGE_LOGO_BASE_ADDRESS-FLASH_PAGE_SIZE))
#define FLASH_OFFSET_BASE_ADDRESS  ((uint32_t)(FLASH_STORE_BASE_ADDRESS-FLASH_PAGE_SIZE))
/*******************************************************/


//#define FLASH_STORE_BASE_ADDRESS  ((uint32_t)0x0807F800)
//#define ENABLE_FLASH_PROFILE    0

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

extern USER_PROFILE_T userProfile;
//extern const USER_PROFILE_T userProfileSFlash;     // last 1 page for mode parameters, 2048byte
#if 1//ENABLE_FLASH_PROFILE	
extern const USER_PROFILE_T userProfileFlash;// __attribute__((at(FLASH_STORE_BASE_ADDRESS)));    
#endif

/**************************************************/               // Added by Jason Chen, 2015.09.13
extern PROBE_OFFSET_T probeOffset;
extern USB_FEEDBACK_OFFSET usbFeedbackOffset;                  // Added by Jason Chen, 2015.09.30
//extern const PROBE_OFFSET_T probeOffsetFlash;// __attribute__((at(FLASH_OFFSET_BASE_ADDRESS)));    
/**************************************************/
typedef enum   //remote side runing state  
{
    BLE_RPT_MODE_IDL,               // probe - command finished waiting for next 
    BLE_RPT_MODE_MEASUREMENT_BUSY,  //probe doing measuring
    BLE_RPT_MODE_MEASUREMENT_ARMED,  //probe measurement get armed
    BLE_RPT_MODE_MEASUREMENT_ERR_1, // probe measure error 1
    BLE_RPT_MODE_MEASUREMENT_ERR_2, // probe measure error 2
    BLE_RPT_MODE_TREATMENT_BUSY,    // probe treatment in the middle of treatment laser is on
    BLE_RPT_MODE_TREATMENT_ARMED,   // probe get armed and waiting for button to start laser is off
    BLE_RPT_MODE_TREATMENT_ERR1,    // probe treatment error 1 
    BLE_RPT_MODE_TREATMENT_ERR2,    // probe treatment error 2
    BLE_RPT_MODE_SELFTEST_BUSY,  //probe doing selftest
    BLE_RPT_MODE_SELFTEST_ARMED,   
    BLE_RPT_MODE_SELFTEST_ERR_1,  
    BLE_RPT_MODE_SELFTEST_ERR_2  
} PROBE_STATUS;	

extern __IO uint16_t probeMode;
extern int16_t L_offset[5];
extern int16_t O_offset[4];

uint8_t myFlashWrite(uint8_t *addr, uint8_t *src, uint8_t len);
void userProfileInit(void);
void probeOffsetInit(void);                                         // Added by Jason Chen, 2015.09.13
#endif


