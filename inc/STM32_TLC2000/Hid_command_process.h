/**
    ******************************************************************************
  * @file    Hid_command_process.h
  * @author  Jason Chen
  * @version V1.0
  * @date    12-July-2014
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HID_COMMAND_PROCESS_H
#define __HID_COMMAND_PROCESS_H

/* Includes ------------------------------------------------------------------*/
//#include "stm3210e_TLC2000.h"
#include "stm32f10x.h"
//#include "stm32_eval_legacy.h"

// GUI to Controller Command
enum
{
	 CMD_SET_ENABLE_VIN660 = 0x58,          // 2014.10.17		
	 CMD_TIME_SETUP        = 0x59,
	 CMD_AUTO_TEST_START   = 0x5A,          // Auto Test Command, Added by Jason Chen, 2015.03.06
	 CMD_FEEDBACK_START    = 0x5B,          // CMD_FIRMWARE_DOWNLOAD = 0x5B,
   CMD_TIME_SYNC_ID      = 0x60,
	 CMD_905_DAC0_CONFIG   = 0x61,
	 CMD_905_DAC1_CONFIG   = 0x62,
	 CMD_905_DAC2_CONFIG   = 0x63,
	 CMD_905_DAC3_CONFIG   = 0x64,
	 CMD_905_DAC4_CONFIG   = 0x65,
	 CMD_905_DAC5_CONFIG   = 0x66,
	 CMD_TENS_DAC_CONFIG   = 0x67,
	 CMD_PROBE_ARM_CONGIF  = 0x68,
	 CMD_660_DAC0_CONFIG   = 0x69,
	 CMD_660_DAC1_CONFIG   = 0x6A,
	 CMD_660_DAC2_CONFIG   = 0x6B,
	 CMD_660_DAC3_CONFIG   = 0x6C,
	 CMD_READ_ADC_VALUE    = 0x6D,
	 CMD_SET_905_DUTY_FREQ = 0x6E,		
	 CMD_SET_660_DUTY_FREQ = 0x6F,
	 CMD_SET_CADENCE_DUTY_FREQ = 0x70,
	 CMD_PROBE_IDLE_CONGIF  = 0x71,
	 CMD_ENABLE_READ_TEMP      = 0x72,
	
	 CMD_SET_905FEEDBACK_PULSE = 0x73,
	 CMD_SET_660FEEDBACK_PULSE = 0x74,
	 
	 CMD_SET_ENABLE_FPGA       = 0x75,
	 CMD_SET_ENABLE_30V        = 0x76,
	 CMD_SET_ENABLE_INPUT      = 0x77,
	 CMD_LOAD_USERPROFILE      = 0x78,
	 CMD_START_TREAT           = 0x7A,
	 CMD_SET_TENS_FREQUENCE    = 0x57,     // 2014.10.21
	 CMD_RESET_FPGA            = 0x56,      // 2014.10.21
	 
	 CMD_SET_ADC_READING_INTERVAL = 0x55,      // 2014.11.12
	 CMD_WORKING_MODE          = 0x54,         // 2014.12.22
	 
	 CMD_SET_OFFSET            = 0x53,         // Added by Jason Chen, 2015.09.13
	 CMD_SET_CALIBRATION       = 0x52,         // Added by Jason Chen, 2015.09.13
	 
	 CMD_SET_FEEDBACK_OFFSET   = 0x51          // Added by Jason Chen, 2015.09.30
};

extern __IO uint8_t treatment_on_flag;
extern __IO uint8_t FeedBack905Test_Flag;
extern __IO uint8_t FeedBack905_Flag;
extern __IO uint16_t AutoTestValue[2];
extern __IO uint16_t AutoTestTimeInterval;

extern uint16_t voltage_s;                                                 // Added by Jason Chen, 2015.12.14

void Mode_GUI_Init(uint8_t mode);
void Mode_GUI_Fresh(uint8_t mode);
void Screen_Display(void);

void TreatmentOn(void);
void ReTreatmentOn(void);
void MeasurementOn(void);
void TreatmentOff(void);
void TreatmentPauseOff(void);
void CommandProcess(void);
void Usb_Send(void);

void PROBE_Armed(uint16_t voltage905,uint16_t voltage660, uint16_t voltageTens,uint16_t time);
void PROBE_Idle(void);
void PROBE_Mode(uint8_t mode, uint8_t store_flag);

void SelfTest_setup(int16_t laser_sel);       // Added by Jason Chen, 2015.10.07
void Active_SelfTest(void);                   // Added by Jason Chen, 2015.10.07
void DeActive_SelfTest(void);                 // Added by Jason Chen, 2015.10.07
void Self_Cali_End(void);                     // Added by Jason Chen, 2018.08.08
uint8_t SelfTest_adj(int16_t laser_sel, int16_t adj_value);

#endif
