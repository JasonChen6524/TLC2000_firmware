/**
    ******************************************************************************
  * @file    BatSensor.h
  * @author  Jason Chen
  * @version V1.0
  * @date    04-Dec-2014
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BAT_SENSOR_H
#define __BAT_SENSOR_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

enum
{
  BAT_STATUS_REG                = 0x00, // A Status R See Table 2
  BAT_CONTROL_REG               = 0x01, // B Control R/W 3Ch
  BAT_ACCUMULATED_CHARGE_MSB    = 0x02, // C Accumulated Charge MSB R/W 7Fh
  BAT_ACCUMULATED_CHARGE_LSB    = 0x03, // D Accumulated Charge LSB R/W FFh
  BAT_CHARGE_THRESHOLD_HIGH_MSB = 0x04, // E Charge Threshold High MSB R/W FFh
  BAT_CHARGE_THRESHOLD_HIGH_LSB = 0x05, // F Charge Threshold High LSB R/W FFh
  BAT_CHARGE_THRESHOLD_LOW_MSB  = 0x06, // G Charge Threshold Low MSB R/W 00h
  BAT_CHARGE_THRESHOLD_LOW_LSB  = 0x07, // H Charge Threshold Low LSB R/W 00h
  BAT_VOLTAGE_MSB               = 0x08, // I Voltage MSB R 00h
  BAT_VOLTAGE_LSB               = 0x09, // J Voltage LSB R 00h
  BAT_VOLTAGE_THRESHOLD_HIGH_MSB= 0x0A, // K Voltage Threshold High MSB R/W FFh
  BAT_VOLTAGE_THRESHOLD_HIGH_LSB= 0x0B, // L Voltage Threshold High LSB R/W FFh
  BAT_VOLTAGE_THRESHOLD_LOW_MSB = 0x0C, // M Voltage Threshold Low MSB R/W 00h
  BAT_VOLTAGE_THRESHOLD_LOW_LSB = 0x0D, // N Voltage Threshold Low LSB R/W 00h
  BAT_CURRENT_MSB               = 0x0E, // O Current MSB R 00h
  BAT_CURRENT_LSB               = 0x0F, // P Current LSB R 00h
  BAT_CURRENT_ThRESHOLD_HIGH_MSB= 0x10, // Q Current Threshold High MSB R/W FFh
  BAT_CURRENT_ThRESHOLD_HIGH_LSB= 0x11, // R Current Threshold High LSB R/W FFh
  BAT_CURRENT_ThRESHOLD_LOW_MSB = 0x12, // S Current Threshold Low MSB R/W 00h
  BAT_CURRENT_ThRESHOLD_LOW_LSB = 0x13, // T Current Threshold Low LSB R/W 00h
  BAT_TEMPERATURE_MSB           = 0x14, // U Temperature MSB R 00h
  BAT_TEMPERATURE_LSB           = 0x15, // V Temperature LSB R 00h
  BAT_TEMPERATURE_THRESHOLD_HIGH= 0x16, // W Temperature Threshold High R/W FFh
  BAT_TEMPERATURE_THRESHOLD_LOW = 0x17  // X Temperature Threshold Low R/W 00h
};


enum
{
	CONTROL_SHUTDOWN                  = 0x01,
	
	CONTROL_ALCC_DISABLE              = (0x00 << 1),
	CONTROL_ALCC_CHARGE_COMPLETE_MODE = (0x01 << 1),
	CONTROL_ALCC_ALERT_MODE           = (0x02 << 1),
	CONTROL_ALCC_MASK                 = (0x03 << 1),
	
	CONTROL_PRESCALER_0001            = (0x00 << 3),
	CONTROL_PRESCALER_0004            = (0x01 << 3),
	CONTROL_PRESCALER_0016            = (0x02 << 3),
	CONTROL_PRESCALER_0064            = (0x03 << 3),
	CONTROL_PRESCALER_0256            = (0x04 << 3),
	CONTROL_PRESCALER_1024            = (0x05 << 3),
	CONTROL_PRESCALER_4096            = (0x06 << 3),
	CONTROL_PRESCALER_MASK            = (0x07 << 3),
	
	CONTROL_ADC_SLEEP_MODE            = (0x00 << 6),
	CONTROL_ADC_MANUAL_MODE           = (0x01 << 6),
	CONTROL_ADC_SCAN_MODE             = (0x02 << 6),
	CONTROL_ADC_AUTO_MODE             = (0x03 << 6),
	CONTROL_ADC_MODE_MASK             = (0x03 << 6)
	
};

#define BAT_I2C                     I2C1
#define BAT_I2C_CLK                 RCC_APB1Periph_I2C1
#define BAT_I2C_SCL_PIN             GPIO_Pin_6                  /* PB.6 */
#define BAT_I2C_SCL_GPIO_PORT       GPIOB                       /* GPIOB */
#define BAT_I2C_SCL_GPIO_CLK        RCC_APB2Periph_GPIOB
#define BAT_I2C_SDA_PIN             GPIO_Pin_7                  /* PB.7 */
#define BAT_I2C_SDA_GPIO_PORT       GPIOB                       /* GPIOB */
#define BAT_I2C_SDA_GPIO_CLK        RCC_APB2Periph_GPIOB

#define I2C1_SPEED                  400000
#define I2C_SLAVE_ADDRESS7_BAT      0xC8
#define ARA_RESPONSE_AADR           0x18
#define FULLY_CHARGED_VOLTAGE       16.686f
#define LOW_BATTERY_VOLTAGE         12.0f

#define COMMON_RATIO                ((FULLY_CHARGED_VOLTAGE-LOW_BATTERY_VOLTAGE)/0.95f)

#define BAT_FLAG_TIMEOUT            ((int32_t)0x1000)
#define BAT_LONG_TIMEOUT            ((int32_t)(10 * BAT_FLAG_TIMEOUT))    


typedef struct
{
	uint8_t lowBatteryAlarm;
	uint8_t HighTempAlarm;
	uint8_t resev0;
	uint8_t resev1;
	float gas_gauge_value;
	float probeTempValue;
}FEEDBACK_BATTERY;

typedef union
{  
	uint8_t  feedBflash_byte[sizeof(FEEDBACK_BATTERY)];    
  FEEDBACK_BATTERY bat_feedback;
} FEEDBACK_BATTERY_T;


extern uint8_t EnableReadBattery;
extern __IO uint32_t BatteryDispay;
extern uint8_t displayBuff[];
extern FEEDBACK_BATTERY_T battery_profile;

void Bat_Init(void);
uint8_t VerifyBatSensor(void);
uint8_t Bat_RegisterWrite(/*uint8_t i2c_addr, */uint8_t reg_addr, uint8_t value);
uint8_t BatSensorSetup(uint8_t sel, uint16_t value);
uint16_t ReadBatVoltage(void);
uint16_t ReadBatCurrent(void);
uint16_t ReadBatTemperature(void);
uint8_t GetBatStatus(void);
void BatVoltage_Display(int voltage);
uint16_t BatVoltage_Display_normal(void);
#endif

