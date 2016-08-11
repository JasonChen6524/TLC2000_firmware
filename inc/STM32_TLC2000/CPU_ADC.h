/**
    ******************************************************************************
  * @file    CPU_ADC.h
  * @author  Jason Chen
  * @version V1.0
  * @date    27-JAN-2015
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CPU_ADC_H
#define __CPU_ADC_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/** @addtogroup define
  * @{
  */
	 
#define USE_DMA_MODE  1

/** @addtogroup STM32_EVAL
  * @{
  */ 

/** @addtogroup STM3210E_EVAL
  * @{
  */
  
/** @addtogroup STM3210E_EVAL_SPI_FLASH
  * @{
  */  

/** @defgroup STM3210E_EVAL_SPI_FLASH_Exported_Types
  * @{
  */ 
/**
  * @}
  */
	 
typedef struct
{
	uint8_t report_id;
	uint8_t report_cmd;
	uint8_t validF;
	uint8_t ChannelSelect;
  uint16_t adc_ch_reading[8];
	float adc_ch_reading_mw[4];
	//uint16_t offset[4];
	int16_t offset[4];                     // Changed by Jason Chen, 2015.09.30
	uint16_t DAC_output;
}FEEDBACK_BUFFER;

typedef union
{  
	uint8_t  feedBflash_byte[sizeof(FEEDBACK_BUFFER)];    
  FEEDBACK_BUFFER feed_back_buf;
} FEEDBACK_BUFFER_T;

typedef struct
{
	uint8_t report_id;
	uint8_t report_cmd;
	uint8_t laserSelect;
	uint8_t flag_of_end;                  //Changed by Jason Chen, 2016.08.07
	uint8_t SelfTest_count;               //Changed by Jason Chen, 2016.08.07		
	uint8_t validF[9];	
  uint16_t adc_ch_reading[9];
//float adc_offset_rate[9];
}SELF_TEST_RECORD;

typedef union
{  
	uint8_t  selfTest_byte[sizeof(SELF_TEST_RECORD)];    
  SELF_TEST_RECORD selfTest_buf;
} SELF_TEST_RECORD_T;


/**
  * @brief  Low layer functions
  */

extern FEEDBACK_BUFFER_T feedBack_profile;                     // Added by Jason Chen, 2015.09.30
extern SELF_TEST_RECORD_T selfTestBuf;                         // Added by Jason Chen, 2015.10.07

extern __IO uint32_t AMP_hold_flag;                            // Added by Jason Chen, 2014.12.19
extern uint16_t adc_rd;
extern __IO uint32_t adc_reading_interval;
extern __IO uint32_t led_on_count;
extern uint8_t displayBuff[];

extern __IO int32_t OneMs_count;

extern __IO uint32_t adc_period;

extern __IO uint8_t ChannelSelect;

extern __IO int16_t laser_sel;                               // Added by Jason Chen, 2015.10.07
extern __IO int16_t laserSelect;                             // Added by Jason Chen, 2015.12.14

void Cpu_ADC_Init(void);
void CPU_ADC_Value_Display(void);
void CPU_ADC_Value_SelfTest(void);                           // Added by Jason Chen, 2015.10.07
void SelfTest_Init(void);                                    // Added by Jason Chen, 2015.12.17

#ifdef __cplusplus
}
#endif

#endif /* __STM3210E_EVAL_SPI_FLASH_H */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */ 

/**
  * @}
  */  

