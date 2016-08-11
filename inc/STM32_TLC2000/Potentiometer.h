/**
    ******************************************************************************
  * @file    Potentiometer.h
  * @author  Jason Chen
  * @version V1.0
  * @date    18-July-2014
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __POTENTIOMETER_H
#define __POTENTIOMETER_H

/* Includes ------------------------------------------------------------------*/
//#include "stm3210e_TLC2000.h"
#include "stm32f10x.h"
//#include "stm32_eval_legacy.h"

enum
{
	CHANNEL0  = 0,
	CHANNEL1  = 1,
	CHANNEL2  = 2,
	CHANNEL3  = 3
};

enum
{
	CH_DISABLE  = 0,
	CH_ENABLE   = !CH_DISABLE
};

enum
{
	REGA  = 0x11,
	REGB  = 0x12,
	REGAB = 0x13,
};

enum
{
	CONTROL_NOTHING   = 0x00,
	WRITE_RDAC        = 0x10,
	WRITE_SHIFT       = 0x20,
	READ_CONTENT      = 0x30,
	RDAC_TO_EEPROM    = 0x70,
	SOFTWARE_RESET    = 0xB0,
	SOFTWARE_SHUTDOWN = 0xC0
};

enum
{
	MIDSCALE_RST    = 0x40,
	SOFT_SHUTDOWN   = 0x20
};//mode

enum
{
	CH_RDAC1    = 0x00,
	CH_RDAC2    = 0x80
	//ALL_RDAC    = 0x80
};

enum
{
	CH_MEM_RDAC1    = 0x00,
	CH_MEM_RDAC2    = 0x02
};

enum      //TCN75
{
	TEMP_SENSOR_TEMP    = 0x00,
	TEMP_SENSOR_CONFIG  = 0x01,
	TEMP_SENSOR_THYST   = 0x02,
	TEMP_SENSOR_TSET    = 0x03
};

enum                                         // Added Jason Chen, 2015.04.09
{
	DISABLE_TEMP = 0,
	ENABLE_TEMP  = !DISABLE_TEMP
};

enum
{
	FAULT_QUEUE_1   = 0x00,
	FAULT_QUEUE_2   = 0x08,
	FAULT_QUEUE_4   = 0x10,
	FAULT_QUEUE_6   = 0x18,
	TEMP_SHUTDOWN   = 0x01,
  TEMP_INT_MODE   = 0x02,
	TEMP_ACTIVE_HIGH= 0x04,
	TEMP_ACTIVE_LOW = 0x00
};

#define INPUT_CH0                        GPIO_Pin_10
#define INPUT_CH0_GPIO_PORT              GPIOE
#define INPUT_CH0_GPIO_CLK               RCC_APB2Periph_GPIOE
#define INPUT_CH1                        GPIO_Pin_11
#define INPUT_CH1_GPIO_PORT              GPIOE
#define INPUT_CH1_GPIO_CLK               RCC_APB2Periph_GPIOE
#define INPUT_CH2                        GPIO_Pin_12
#define INPUT_CH2_GPIO_PORT              GPIOE
#define INPUT_CH2_GPIO_CLK               RCC_APB2Periph_GPIOE
#define INPUT_CH3                        GPIO_Pin_13
#define INPUT_CH3_GPIO_PORT              GPIOE
#define INPUT_CH3_GPIO_CLK               RCC_APB2Periph_GPIOE

#define INT_TEMP_PIN                     GPIO_Pin_5                 /* PB.5 */
#define INT_TEMP_GPIO_PORT               GPIOB                       
#define INT_TEMP_GPIO_CLK                RCC_APB2Periph_GPIOB
#define INT_TEMP_EXTI_LINE               EXTI_Line5
#define INT_TEMP_EXTI_PORT_SOURCE        GPIO_PortSourceGPIOB
#define INT_TEMP_EXTI_PIN_SOURCE         GPIO_PinSource5
#define INT_TEMP_EXTI_IRQn               EXTI9_5_IRQn 

#define SENSORTEMP_45_DEGRESS            90 // 45 degree            // Added by Jason Chen, 2015.07.03
#define SENSORTEMP_40_DEGRESS            80 // 40 degree
#define SENSORTEMP_35_DEGRESS            70 // 35 degree
#define SENSORTEMP_31_DEGRESS            62 // 31
#define SENSORTEMP_29_DEGRESS            58 // 29
#define SENSORTEMP_28_5_DEGRESS          57 // 28.5
#define SENSORTEMP_4_DEGRESS             8  // 4
#define SENSORTEMP_1_DEGRESS             2  // 1

//#define SW_PIN                           GPIO_Pin_2                 /* PE2 */  // 2014.10.20
//#define SW_GPIO_PORT                     GPIOE
//#define SW_GPIO_CLK                      RCC_APB2Periph_GPIOE
//#define SW_EXTI_LINE                     EXTI_Line2
//#define SW_EXTI_PORT_SOURCE              GPIO_PortSourceGPIOE
//#define SW_EXTI_PIN_SOURCE               GPIO_PinSource2
//#define SW_EXTI_IRQn                     EXTI2_IRQn


#define POT_I2C                          I2C2
#define POT_I2C_CLK                      RCC_APB1Periph_I2C2
#define POT_I2C_SCL_PIN                  GPIO_Pin_10                 /* PB.10 */
#define POT_I2C_SCL_GPIO_PORT            GPIOB                       /* GPIOB */
#define POT_I2C_SCL_GPIO_CLK             RCC_APB2Periph_GPIOB
#define POT_I2C_SDA_PIN                  GPIO_Pin_11                  /* PB.11 */
#define POT_I2C_SDA_GPIO_PORT            GPIOB                       /* GPIOB */
#define POT_I2C_SDA_GPIO_CLK             RCC_APB2Periph_GPIOB

//#define I2C_DAC_GPIO       GPIOB
//#define I2C_DAC_GPIO_CLK   RCC_APB2Periph_GPIOB


#define I2C_SPEED               400000
#define I2C_SLAVE_ADDRESS7_U18  0x50
#define I2C_SLAVE_ADDRESS7_U14  0x52

#define I2C_SLAVE_ADDRESS7_PD0  0x5E
#define I2C_SLAVE_ADDRESS7_PD1  0x5A//0x58
#define I2C_SLAVE_ADDRESS7_PD2  0x5C//0x46
#define I2C_SLAVE_ADDRESS7_PD3  0x58//0x40

#define I2C_SLAVE_ADDRESS7_TEMPSENSOR 0x9E//0x9A//0x9E

#define POT_FLAG_TIMEOUT         ((int32_t)0x1000)
#define POT_LONG_TIMEOUT         ((int32_t)(10 * POT_FLAG_TIMEOUT))    

//#define NOP()                          __NOP();

extern uint8_t HighTemperatureAlarm;
extern uint8_t HighTemperatureClear;                   // Added by Jason Chen, 2015.07.04
extern uint16_t HighTemperatureCount;                  // Added by Jason Chen, 2015.07.04
#define ENALE_HI_TEMP                        0         // Added by Jason Chen, 2015.07.06
#define TEMPERATURE_DISPLAY_TEST             0

void Enable_Input(uint8_t channel, uint8_t enable_ch);
void Enable_All_Input(void);
void Disable_All_Input(void);

void Pot_Init(void);

//uint8_t Pot_SetPDx(uint8_t i2c_addr, uint8_t mode, uint8_t gain);// 2014.10.15
//uint8_t Pot_SetAMP2(uint8_t gain);                               // 2014.10.15

uint8_t TempSensorConfig(uint8_t sel, uint8_t value);
uint8_t TempSensorSet(uint8_t sel, uint16_t value);
void TempSensorTCN75Init(void);
uint16_t ReadTemperature(uint8_t reg_sel);
void Temperature_Display(int temp_i);
void Enable_temperature_reading(uint8_t enableReading);           // Added by Jason Chen, 2015.04.09
uint16_t Temperature_Display_normal(void);
void TemperatureDisplayClear(void);                               // Added by Jason Chen, 2015.07.04
uint32_t Temp_Int_PBGetState(void);
#endif
