/**
    ******************************************************************************
  * @file    FPGA_RAM.h
  * @author  Jason Chen
  * @version V1.0
  * @date    18-July-2014
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FPGA_RAM_H
#define __FPGA_RAM_H

/* Includes ------------------------------------------------------------------*/
//#include "stm3210e_TLC2000.h"
#include "stm32f10x.h"
//#include "stm32_eval_legacy.h"

enum
{
	PULSE_WIDTH_180 = 0x01 << 8,
	PULSE_WIDTH_185 = 0x02 << 8,
	PULSE_WIDTH_190 = 0x03 << 8,
	PULSE_WIDTH_195 = 0x04 << 8,
	PULSE_WIDTH_200 = 0x05 << 8,   
	PULSE_WIDTH_205 = 0x06 << 8,
	PULSE_WIDTH_210 = 0x07 << 8
};

enum                                     // Added by Jason Chen, 2015.09.17
{
	PULSE_WIDTH_20 = 0x01 << 8,
	PULSE_WIDTH_33 = 0x02 << 8,
	PULSE_WIDTH_46 = 0x03 << 8,
	PULSE_WIDTH_50 = 0x04 << 8,
	PULSE_WIDTH_72 = 0x05 << 8,   
	PULSE_WIDTH_85 = 0x06 << 8,
	PULSE_WIDTH_99 = 0x07 << 8
};

enum
{
	DAC0_905_FEEDBACK = 0x0001,
	DAC1_905_FEEDBACK = 0x0002,
	DAC2_905_FEEDBACK = 0x0004,
	DAC3_905_FEEDBACK = 0x0008,
	DAC4_905_FEEDBACK = 0x0010,
	DAC5_905_FEEDBACK = 0x0020
};

enum
{
	DAC0_660_FEEDBACK = 0x0001,
	DAC1_660_FEEDBACK = 0x0002,
	DAC2_660_FEEDBACK = 0x0004,
	DAC3_660_FEEDBACK = 0x0008
};

enum
{
	CADENCE_FREQ_905 = 0,
	CADENCE_FREQ_660 = 1
};

enum                                 // New Board, 2014.10.06
{
	FPGA_RUN_MODE  = 0,
	FPGA_TEST_MODE = !FPGA_RUN_MODE
};

enum                                 // New Board, 2014.10.06
{
	FPGA_DISABLE  = 0,
	FPGA_ENABLE   = !FPGA_DISABLE
};

enum
{
  FREQ_100HZ = 1,
	FREQ_200HZ,
	FREQ_300HZ,
	FREQ_400HZ,
	FREQ_500HZ,
	FREQ_600HZ,
	FREQ_700HZ,
	FREQ_800HZ,
	FREQ_900HZ,
	FREQ_1000HZ,
  FREQ_1100HZ,
	FREQ_1200HZ,
	FREQ_1300HZ,
	FREQ_1400HZ,
	FREQ_1500HZ,
	FREQ_1600HZ,
	FREQ_1700HZ,
	FREQ_1800HZ,
	FREQ_1900HZ,
	FREQ_2000HZ,
	FREQ_2100HZ,
	FREQ_2200HZ,
	FREQ_2300HZ,
	FREQ_2400HZ,
	FREQ_2500HZ,
	FREQ_2600HZ,
	FREQ_2700HZ,
	FREQ_2800HZ,
	FREQ_2900HZ,
	FREQ_3000HZ,
	FREQ_3100HZ,
	FREQ_3200HZ,
	FREQ_3300HZ,
	FREQ_3400HZ,
	FREQ_3500HZ,
	FREQ_3600HZ,
	FREQ_3700HZ,
	FREQ_3800HZ,
	FREQ_3900HZ,
	FREQ_4000HZ,
	FREQ_4100HZ,
	FREQ_4200HZ,
	FREQ_4300HZ,
	FREQ_4400HZ,
	FREQ_4500HZ,
	FREQ_4600HZ,
	FREQ_4700HZ,
	FREQ_4800HZ,
	FREQ_4900HZ,	
	FREQ_5000HZ,
	FREQ_5100HZ,
	FREQ_5200HZ,
	FREQ_5300HZ,
	FREQ_5400HZ,
	FREQ_5500HZ,
	FREQ_5600HZ,
	FREQ_5700HZ,
	FREQ_5800HZ,
	FREQ_5900HZ,	
	FREQ_6000HZ,
	FREQ_6100HZ,
	FREQ_6200HZ,
	FREQ_6300HZ,
	FREQ_6400HZ,
	FREQ_6500HZ,
	FREQ_6600HZ,
	FREQ_6700HZ,
	FREQ_6800HZ,
	FREQ_6900HZ,	
	FREQ_7000HZ,
	FREQ_7100HZ,
	FREQ_7200HZ,
	FREQ_7300HZ,
	FREQ_7400HZ,
	FREQ_7500HZ,
	FREQ_7600HZ,
	FREQ_7700HZ,
	FREQ_7800HZ,
	FREQ_7900HZ,
	FREQ_8000HZ,
	FREQ_8100HZ,
	FREQ_8200HZ,
	FREQ_8300HZ,
	FREQ_8400HZ,
	FREQ_8500HZ,
	FREQ_8600HZ,
	FREQ_8700HZ,
	FREQ_8800HZ,
	FREQ_8900HZ,	
	FREQ_9000HZ,
	FREQ_9100HZ,
	FREQ_9200HZ,
	FREQ_9300HZ,
	FREQ_9400HZ,
	FREQ_9500HZ,
	FREQ_9600HZ,
	FREQ_9700HZ,
	FREQ_9800HZ,
	FREQ_9900HZ,	
	FREQ_10000HZ	
};

#if 0
enum
{
	FREQ_0_0_HZ = 0,                        // 00, not used
	FREQ_0_1_HZ,                            // 01
	FREQ_0_2_HZ,                            // 02
	FREQ_0_3_HZ,                            // 03
	FREQ_0_4_HZ,                            // 04
	FREQ_0_5_HZ,                            // 05
	FREQ_0_6_HZ,                            // 06
	FREQ_0_7_HZ,                            // 07
	FREQ_0_8_HZ,                            // 08
	FREQ_0_9_HZ,                            // 09
	FREQ_1_0_HZ,                            // 10
	FREQ_1_1_HZ,                            // 11, not used
	FREQ_1_2_HZ,                            // 12, not used
	FREQ_1_3_HZ,                            // 13, not used
	FREQ_1_4_HZ,                            // 14, not used
	FREQ_1_5_HZ,                            // 15
	FREQ_1_6_HZ,                            // 16, not used
	FREQ_1_7_HZ,                            // 17, not used
	FREQ_1_8_HZ,                            // 18, not used
	FREQ_1_9_HZ,                            // 19, not used
	FREQ_2_HZ,                              // 20
	FREQ_3_HZ,                              // 21, not used
	FREQ_4_HZ,                              // 22, not used
	FREQ_5_HZ,                              // 23 
	FREQ_6_HZ,                              // 24, not used
	FREQ_7_HZ,                              // 25, not used
	FREQ_8_HZ,                              // 26, not used
	FREQ_9_HZ,                              // 27, not used
	FREQ_10_HZ,                             // 28
	FREQ_20_HZ,	                            // 29
	FREQ_30_HZ,                             // 30, not used
	FREQ_40_HZ,                             // 31, not used
	FREQ_50_HZ,                             // 32
	FREQ_60_HZ,                             // 33, not used
	FREQ_70_HZ,                             // 34, not used
	FREQ_80_HZ,                             // 35, not used
	FREQ_90_HZ,                             // 36, not used
	FREQ_100_HZ,                            // 37
	FREQ_110_HZ,                            // 38, not used
	FREQ_120_HZ,                            // 39, not used
	FREQ_130_HZ,                            // 40, not used
	FREQ_140_HZ,                            // 41, not used
	FREQ_150_HZ,                            // 42, not used
	FREQ_160_HZ,                            // 43, not used
	FREQ_170_HZ,                            // 44, not used
	FREQ_180_HZ,                            // 45, not used
	FREQ_190_HZ,                            // 46, not used
	FREQ_200_HZ,                            // 47
	FREQ_300_HZ,                            // 48, not used
	FREQ_400_HZ,                            // 49
	FREQ_500_HZ,                            // 50, not used
	FREQ_600_HZ,                            // 51
	FREQ_700_HZ,                            // 52, not used
	FREQ_800_HZ,                            // 53
	FREQ_900_HZ,                            // 54, not used
	FREQ_1000_HZ                            // 55
};
#else
enum
{
	FREQ_0_1_HZ = 1,
	FREQ_0_2_HZ = 2,
	FREQ_0_3_HZ = 3,
	FREQ_0_4_HZ = 4,
	FREQ_0_5_HZ = 5,
	FREQ_0_6_HZ = 6,
	FREQ_0_7_HZ = 7,
	FREQ_0_8_HZ = 8,
	FREQ_0_9_HZ = 9,
	FREQ_1_0_HZ = 10,
	FREQ_1_5_HZ = 15,
	FREQ_2_HZ   = 20,
	FREQ_5_HZ   = 23, 
	FREQ_10_HZ  = 28,
	FREQ_20_HZ  = 29,
	FREQ_50_HZ  = 32,
	FREQ_100_HZ = 37,
	FREQ_200_HZ = 47,
	FREQ_400_HZ = 49,
	FREQ_600_HZ = 51,
	FREQ_800_HZ = 53,
	FREQ_1000_HZ= 55
};
#endif

enum
{
	TENS_FREQ_00HZ = 0x00,
	TENS_FREQ_05HZ = 0x01,
	TENS_FREQ_10HZ = 0x02,
	TENS_FREQ_15HZ = 0x03,
	TENS_FREQ_20HZ = 0x04,
	TENS_FREQ_25HZ = 0x05,
	TENS_FREQ_30HZ = 0x06
};


#define FPGA_A0                        GPIO_Pin_0
#define FPGA_A0_GPIO_PORT              GPIOC
#define FPGA_A0_GPIO_CLK               RCC_APB2Periph_GPIOC
#define FPGA_A1                        GPIO_Pin_1
#define FPGA_A1_GPIO_PORT              GPIOC
#define FPGA_A1_GPIO_CLK               RCC_APB2Periph_GPIOC
#define FPGA_A2                        GPIO_Pin_2
#define FPGA_A2_GPIO_PORT              GPIOC
#define FPGA_A2_GPIO_CLK               RCC_APB2Periph_GPIOC
//#define FPGA_A3                        GPIO_Pin_3
//#define FPGA_A3_GPIO_PORT              GPIOC
//#define FPGA_A3_GPIO_CLK               RCC_APB2Periph_GPIOC

#ifdef BOARD_R4                                                // Added by Jason Chen, 2016.06.16
  #define FPGA_EN                        GPIO_Pin_8
  #define FPGA_EN_GPIO_PORT              GPIOB
  #define FPGA_EN_GPIO_CLK               RCC_APB2Periph_GPIOB
#else
  #define FPGA_EN                        GPIO_Pin_5
  #define FPGA_EN_GPIO_PORT              GPIOC
  #define FPGA_EN_GPIO_CLK               RCC_APB2Periph_GPIOC
#endif

#define FPGA_RW                        GPIO_Pin_12
#define FPGA_RW_GPIO_PORT              GPIOC
#define FPGA_RW_GPIO_CLK               RCC_APB2Periph_GPIOC

#define FPGA_ST0                       GPIO_Pin_0
#define FPGA_ST0_GPIO_PORT             GPIOE
#define FPGA_ST0_GPIO_CLK              RCC_APB2Periph_GPIOE

#define FPGA_ST1                       GPIO_Pin_1
#define FPGA_ST1_GPIO_PORT             GPIOE
#define FPGA_ST1_GPIO_CLK              RCC_APB2Periph_GPIOE

/////////////////////////////////////////////////////////////////////
// definitions for R3 controller, Added by Jason Chen, 2014.12.05
#define FPGA_ST2                       GPIO_Pin_2
#define FPGA_ST2_GPIO_PORT             GPIOE
#define FPGA_ST2_GPIO_CLK              RCC_APB2Periph_GPIOE

#define FPGA_ST3_905                   GPIO_Pin_3
#define FPGA_ST3_905_GPIO_PORT         GPIOE
#define FPGA_ST3_905_GPIO_CLK          RCC_APB2Periph_GPIOE

#define FPGA_ST4_660                   GPIO_Pin_4
#define FPGA_ST4_660_GPIO_PORT         GPIOE
#define FPGA_ST4_660_GPIO_CLK          RCC_APB2Periph_GPIOE

#define FPGA_ST5_905                   GPIO_Pin_5
#define FPGA_ST5_905_GPIO_PORT         GPIOE
#define FPGA_ST5_905_GPIO_CLK          RCC_APB2Periph_GPIOE

#define FPGA_ST6_660                   GPIO_Pin_6
#define FPGA_ST6_660_GPIO_PORT         GPIOE
#define FPGA_ST6_660_GPIO_CLK          RCC_APB2Periph_GPIOE
//////////////////////////////////////////////////////////////////////

#ifdef BOARD_R4
  #define FPGA_AD_RD_CLK                   GPIO_Pin_11
  #define FPGA_AD_RD_CLK_GPIO_PORT         GPIOC
  #define FPGA_AD_RD_CLK_GPIO_CLK          RCC_APB2Periph_GPIOC
  #define FPGA_AD_RD_CLK_EXTI_LINE         EXTI_Line11
  #define FPGA_AD_RD_CLK_EXTI_PORT_SOURCE  GPIO_PortSourceGPIOC
  #define FPGA_AD_RD_CLK_EXTI_PIN_SOURCE   GPIO_PinSource11
  #define FPGA_AD_RD_CLK_EXTI_IRQn         EXTI15_10_IRQn
#else
  #define FPGA_AD_RD_CLK                   GPIO_Pin_4
  #define FPGA_AD_RD_CLK_GPIO_PORT         GPIOC
  #define FPGA_AD_RD_CLK_GPIO_CLK          RCC_APB2Periph_GPIOC
  #define FPGA_AD_RD_CLK_EXTI_LINE         EXTI_Line4
  #define FPGA_AD_RD_CLK_EXTI_PORT_SOURCE  GPIO_PortSourceGPIOC
  #define FPGA_AD_RD_CLK_EXTI_PIN_SOURCE   GPIO_PinSource4
  #define FPGA_AD_RD_CLK_EXTI_IRQn         EXTI4_IRQn 
#endif


#define FPGA_D0                        GPIO_Pin_0
#define FPGA_D0_GPIO_PORT              GPIOD
#define FPGA_D0_GPIO_CLK               RCC_APB2Periph_GPIOD
#define FPGA_D1                        GPIO_Pin_1
#define FPGA_D1_GPIO_PORT              GPIOD
#define FPGA_D1_GPIO_CLK               RCC_APB2Periph_GPIOD
#define FPGA_D2                        GPIO_Pin_2
#define FPGA_D2_GPIO_PORT              GPIOD
#define FPGA_D2_GPIO_CLK               RCC_APB2Periph_GPIOD
#define FPGA_D3                        GPIO_Pin_3
#define FPGA_D3_GPIO_PORT              GPIOD
#define FPGA_D3_GPIO_CLK               RCC_APB2Periph_GPIOD
#define FPGA_D4                        GPIO_Pin_4
#define FPGA_D4_GPIO_PORT              GPIOD
#define FPGA_D4_GPIO_CLK               RCC_APB2Periph_GPIOD
#define FPGA_D5                        GPIO_Pin_5
#define FPGA_D5_GPIO_PORT              GPIOD
#define FPGA_D5_GPIO_CLK               RCC_APB2Periph_GPIOD
#define FPGA_D6                        GPIO_Pin_6
#define FPGA_D6_GPIO_PORT              GPIOD
#define FPGA_D6_GPIO_CLK               RCC_APB2Periph_GPIOD
#define FPGA_D7                        GPIO_Pin_7
#define FPGA_D7_GPIO_PORT              GPIOD
#define FPGA_D7_GPIO_CLK               RCC_APB2Periph_GPIOD
#define FPGA_D8                        GPIO_Pin_8
#define FPGA_D8_GPIO_PORT              GPIOD
#define FPGA_D8_GPIO_CLK               RCC_APB2Periph_GPIOD
#define FPGA_D9                        GPIO_Pin_9
#define FPGA_D9_GPIO_PORT              GPIOD
#define FPGA_D9_GPIO_CLK               RCC_APB2Periph_GPIOD
#define FPGA_D10                       GPIO_Pin_10
#define FPGA_D10_GPIO_PORT             GPIOD
#define FPGA_D10_GPIO_CLK              RCC_APB2Periph_GPIOD
#define FPGA_D11                       GPIO_Pin_11
#define FPGA_D11_GPIO_PORT             GPIOD
#define FPGA_D11_GPIO_CLK              RCC_APB2Periph_GPIOD
#define FPGA_D12                       GPIO_Pin_12
#define FPGA_D12_GPIO_PORT             GPIOD
#define FPGA_D12_GPIO_CLK              RCC_APB2Periph_GPIOD
#define FPGA_D13                       GPIO_Pin_13
#define FPGA_D13_GPIO_PORT             GPIOD
#define FPGA_D13_GPIO_CLK              RCC_APB2Periph_GPIOD
#define FPGA_D14                       GPIO_Pin_14
#define FPGA_D14_GPIO_PORT             GPIOD
#define FPGA_D14_GPIO_CLK              RCC_APB2Periph_GPIOD
#define FPGA_D15                       GPIO_Pin_15
#define FPGA_D15_GPIO_PORT             GPIOD
#define FPGA_D15_GPIO_CLK              RCC_APB2Periph_GPIOD

#define FPGA_ENABLE()                  GPIO_SetBits(FPGA_EN_GPIO_PORT, FPGA_EN)
#define FPGA_DISABLE()                 GPIO_ResetBits(FPGA_EN_GPIO_PORT, FPGA_EN)

#define NOP()                          __NOP();

void FPGA_RAM_Init(void);
uint32_t ST0_PBGetState(void);
uint32_t ST1_PBGetState(void);

uint32_t ST2_PBGetState(void);                     // R3 Board, 2014.12.05
uint32_t ST3_905_PBGetState(void);                 // R3 Board, 2014.12.05
uint32_t ST4_660_PBGetState(void);                 // R3 Board, 2014.12.05
uint32_t ST5_905_PBGetState(void);                 // R3 Board, 2014.12.05
uint32_t ST6_660_PBGetState(void);                 // R3 Board, 2014.12.05

uint32_t AD_RD_CLK_PBGetState(void);                 // R3 Board, 2014.12.19

//uint32_t A3_PBGetState(void);                 // New Board, 2014.10.21
void Reset_All_Registers_FPGA(void);           // New Board, 2014.10.06
void Set_905_pulse(uint16_t pulse_width, uint16_t frequency);
void Set_660_pulse(uint16_t pulse_width, uint16_t frequency);
void Set_905_Feedback_pulse(uint16_t DAC_sel);
void Set_660_Feedback_pulse(uint16_t DAC_sel);
void Set_cadence_freq_duty(uint8_t select, uint16_t duty_cycle, uint16_t frequency);

void Set_TENS_Frequence(uint16_t frequency);        // 2014.10.21

#endif
