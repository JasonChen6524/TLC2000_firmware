/**
  ******************************************************************************
  * @file    Potentiometer.c
  * @author  Jason Chen
  * @version V1.0
  * @date    04-Dec-2014
  * @brief   
  *            
  */ 

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "TLC2000_SPI_LCD_TFT.h"
#include "BatSensor.h"
//#include "Potentiometer.h"
#include "MiscTools.h"
#include "fonts.h"
#include "Switch2.h"

#include "UTFT.h"     
#include "UTFT_DLB.h"

/* I2C STOP mask */
#define CR1_STOP_Set            ((uint16_t)0x0200)
#define CR1_STOP_Reset          ((uint16_t)0xFDFF)

/* I2C ACK mask */
#define CR1_ACK_Set             ((uint16_t)0x0400)
#define CR1_ACK_Reset           ((uint16_t)0xFBFF)

/* I2C POS mask */
#define CR1_POS_Set             ((uint16_t)0x0800)
#define CR1_POS_Reset           ((uint16_t)0xF7FF)

extern uint8_t arial12[];
extern uint8_t arial14[];
extern uint8_t arial15[];
extern uint8_t arial16[];
extern uint8_t DejaVuSans18[];

__IO int32_t  Bat_Timeout = BAT_LONG_TIMEOUT; 
//FEEDBACK_BUFFER_T feedBack_profile;

/**
  * @brief  Initializes Potentiometer
  * @param  None
  * @retval None
  */

__IO uint32_t BatteryDispay = 31;                                // Changed from 0 to 31 to make the imediate update in Initialization, 2016.02.18
uint8_t EnableReadBattery = 0;
FEEDBACK_BATTERY_T battery_profile;

void Bat_LowLevel_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
    
  /*!< sEE_I2C_SCL_GPIO_CLK and sEE_I2C_SDA_GPIO_CLK Periph clock enable */
  RCC_APB2PeriphClockCmd(BAT_I2C_SCL_GPIO_CLK | BAT_I2C_SDA_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);	
	
  /*!< sEE_I2C1 Periph clock enable */
  RCC_APB1PeriphClockCmd(BAT_I2C_CLK, ENABLE);
    
  /*!< GPIO configuration */  
  /*!< Configure sEE_I2C pins: SCL, SDA */
  GPIO_InitStructure.GPIO_Pin = BAT_I2C_SCL_PIN | BAT_I2C_SDA_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(BAT_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

}

void Bat_Init(void)
{

	 I2C_InitTypeDef  I2C_InitStructure;
  
	I2C_DeInit(BAT_I2C);
		
  Bat_LowLevel_Init();
  
  /*!< I2C configuration */
  /* POT_I2C configuration */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = I2C_SLAVE_ADDRESS7_BAT;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = I2C1_SPEED;
  
  /* sEE_I2C Peripheral Enable */
  I2C_Cmd(BAT_I2C, ENABLE);
  /* Apply sEE_I2C configuration after enabling it */
  I2C_Init(BAT_I2C, &I2C_InitStructure);
}

uint8_t Bat_RegisterWrite(/*uint8_t i2c_addr,*/ uint8_t reg_addr, uint8_t value)
{
   
  /* Enable the I2C peripheral */
  I2C_GenerateSTART(BAT_I2C, ENABLE);
  
  /* Test on SB Flag */
  Bat_Timeout = BAT_FLAG_TIMEOUT;
 //while(!I2C_CheckEvent(POT_I2C, I2C_EVENT_MASTER_MODE_SELECT)); 	
  while (!I2C_CheckEvent(BAT_I2C, I2C_EVENT_MASTER_MODE_SELECT)) 
  {
    if((Bat_Timeout--) == 0) return 1;
		//LED_On();
  }
  
  /* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(BAT_I2C, I2C_SLAVE_ADDRESS7_BAT, I2C_Direction_Transmitter);
  
  /* Test on ADDR Flag */
  Bat_Timeout = BAT_FLAG_TIMEOUT;
  //while (!I2C_CheckEvent(POT_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  while (!I2C_CheckEvent(BAT_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((Bat_Timeout--) == 0) return 1;
		//LED_Off();
  }
  
  /* Transmit the first address for r/w operations */
  I2C_SendData(BAT_I2C, reg_addr);
  while (!I2C_CheckEvent(BAT_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((Bat_Timeout--) == 0) return 1;
		//LED_On();
  }
	
  /* Transmit the first address for r/w operations */
  I2C_SendData(BAT_I2C, value);
  while (!I2C_CheckEvent(BAT_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((Bat_Timeout--) == 0) return 1;
  }	
	  
  /* Send STOP Condition */
  I2C_GenerateSTOP(BAT_I2C, ENABLE);
  
  return 0;	
}

static uint8_t ReadByte(uint8_t reg_addr)
{
  uint8_t       byte1 = 0;
  __IO uint32_t temp = 0;
    
  // /* While the bus is busy * /
	Bat_Timeout = BAT_FLAG_TIMEOUT;
  while(I2C_GetFlagStatus(BAT_I2C, I2C_FLAG_BUSY))
  {
    if((Bat_Timeout--) == 0) return 1;
  }	
  // * Send START condition * /
  I2C_GenerateSTART(BAT_I2C, ENABLE);

  // / * Test on EV5 and clear it * /
	Bat_Timeout = BAT_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(BAT_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((Bat_Timeout--) == 0) return 1;
  }	

  // / * Send EEPROM address for write  * /
  I2C_Send7bitAddress(BAT_I2C, I2C_SLAVE_ADDRESS7_BAT, I2C_Direction_Transmitter);

  // / * Test on EV6 and clear it * /
	Bat_Timeout = BAT_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(BAT_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((Bat_Timeout--) == 0) return 1;
  }		

  // / * Send the EEPROM's internal address to read from: Only one byte address  * /
  I2C_SendData(BAT_I2C, reg_addr);     

  /// * Test on EV8 and clear it * /
	Bat_Timeout = BAT_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(BAT_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((Bat_Timeout--) == 0) return 1;
  }		
    
  /// * Send STRAT condition a second time * /  
  I2C_GenerateSTART(BAT_I2C, ENABLE);

  /// * Test on EV5 and clear it * /
	Bat_Timeout = BAT_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(BAT_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((Bat_Timeout--) == 0) return 0;
  }		
  
    // * Send EEPROM address for read * /
  I2C_Send7bitAddress(BAT_I2C, I2C_SLAVE_ADDRESS7_BAT, I2C_Direction_Receiver);  
 	
  /* Wait until ADDR is set */
  Bat_Timeout = BAT_FLAG_TIMEOUT;
  while ((BAT_I2C->SR1&0x0002) != 0x0002)
  {
    if((Bat_Timeout--) == 0) return 0;
  }	
  /* Clear ACK bit */
  BAT_I2C->CR1 &= CR1_ACK_Reset;
  /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
  software sequence must complete before the current byte end of transfer */
  __disable_irq();
  /* Clear ADDR flag */
  temp = BAT_I2C->SR2;
  /* Program the STOP */
  I2C_GenerateSTOP(BAT_I2C, ENABLE);
  /* Re-enable IRQs */
  __enable_irq();
    /* Wait until a data is received in DR register (RXNE = 1) EV7 */
	Bat_Timeout = BAT_FLAG_TIMEOUT;
  while ((BAT_I2C->SR1 & 0x00040) != 0x000040)
  {
    if((Bat_Timeout--) == 0) return 0;
  }		
  /* Read the data */
  byte1 = BAT_I2C->DR;
  
  return byte1;
}


uint8_t BatSensorSetup(uint8_t sel, uint16_t value)
{
	uint16_t wordValue = value;
	uint8_t  ByteValue = (uint8_t)((wordValue >> 8)&0xFF);
  /* Enable the I2C peripheral */
  I2C_GenerateSTART(BAT_I2C, ENABLE);
  
  /* Test on SB Flag */
  Bat_Timeout = BAT_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(BAT_I2C, I2C_EVENT_MASTER_MODE_SELECT)) 
  {
    if((Bat_Timeout--) == 0) return 1;
  }
  
  /* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(BAT_I2C, I2C_SLAVE_ADDRESS7_BAT, I2C_Direction_Transmitter);
  
  /* Test on ADDR Flag */
  Bat_Timeout = BAT_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(BAT_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((Bat_Timeout--) == 0) return 1;
  }
  
  /* Transmit the first address for r/w operations */
  I2C_SendData(BAT_I2C, sel);
  while (!I2C_CheckEvent(BAT_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((Bat_Timeout--) == 0) return 1;
  }
	
  /* Transmit the first address for r/w operations */
  I2C_SendData(BAT_I2C, ByteValue);
	Bat_Timeout = BAT_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(BAT_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((Bat_Timeout--) == 0) return 1;
  }	
	  
  /* Transmit the first address for r/w operations */
  ByteValue = (uint8_t) (value&0xFF);
  I2C_SendData(BAT_I2C, ByteValue);
	Bat_Timeout = BAT_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(BAT_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((Bat_Timeout--) == 0) return 1;
  }	
	
  /* Send STOP Condition */
  I2C_GenerateSTOP(BAT_I2C, ENABLE);
  
  return 0;	
}



static uint16_t ReadWord(uint8_t reg_addr)
{
  uint8_t       byte0 = 0;
  uint8_t       byte1 = 0;
  __IO uint32_t temp = 0;
    
  
  // /* While the bus is busy * /
	Bat_Timeout = BAT_FLAG_TIMEOUT;
  while(I2C_GetFlagStatus(BAT_I2C, I2C_FLAG_BUSY))
  {
    if((Bat_Timeout--) == 0) return 1;
  }	
  // * Send START condition * /
  I2C_GenerateSTART(BAT_I2C, ENABLE);

  // / * Test on EV5 and clear it * /
	Bat_Timeout = BAT_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(BAT_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((Bat_Timeout--) == 0) return 1;
  }	

  // / * Send EEPROM address for write  * /
  I2C_Send7bitAddress(BAT_I2C, I2C_SLAVE_ADDRESS7_BAT, I2C_Direction_Transmitter);

  // / * Test on EV6 and clear it * /
	Bat_Timeout = BAT_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(BAT_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((Bat_Timeout--) == 0) return 1;
  }		

  // / * Send the EEPROM's internal address to read from: Only one byte address  * /
  I2C_SendData(BAT_I2C, reg_addr);     

  /// * Test on EV8 and clear it * /
	Bat_Timeout = BAT_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(BAT_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((Bat_Timeout--) == 0) return 1;
  }		
    
  /// * Send STRAT condition a second time * /  
  I2C_GenerateSTART(BAT_I2C, ENABLE);

  /// * Test on EV5 and clear it * /
	Bat_Timeout = BAT_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(BAT_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((Bat_Timeout--) == 0) return 0;
  }		
  
    // * Send EEPROM address for read * /
  I2C_Send7bitAddress(BAT_I2C, I2C_SLAVE_ADDRESS7_BAT, I2C_Direction_Receiver);  
                  
  /* Set POS bit */
  BAT_I2C->CR1 |= CR1_POS_Set;
  /* Wait until ADDR is set: EV6 */
	Bat_Timeout = BAT_FLAG_TIMEOUT;
  while ((BAT_I2C->SR1&0x0002) != 0x0002)
  {
    if((Bat_Timeout--) == 0) return 0;
  }		
  /* EV6_1: The acknowledge disable should be done just after EV6,
  that is after ADDR is cleared, so disable all active IRQs around ADDR clearing and 
  ACK clearing */
  __disable_irq();
  /* Clear ADDR by reading SR2 register  */
  temp = BAT_I2C->SR2;
  /* Clear ACK */
  BAT_I2C->CR1 &= CR1_ACK_Reset;
  /*Re-enable IRQs */
  __enable_irq();
  /* Wait until BTF is set */
	Bat_Timeout = BAT_FLAG_TIMEOUT;
  while ((BAT_I2C->SR1 & 0x00004) != 0x000004)
  {
    if((Bat_Timeout--) == 0) return 0;
  }		
  /* Disable IRQs around STOP programming and data reading */
  __disable_irq();
  /* Program the STOP */
  I2C_GenerateSTOP(BAT_I2C, ENABLE);
    /* Read first data */
  byte1 = BAT_I2C->DR;
  /* Re-enable IRQs */
  __enable_irq();
  /**/
  /* Read second data */
  byte0 = BAT_I2C->DR;
  /* Clear POS bit */
  BAT_I2C->CR1  &= CR1_POS_Reset;
 
  /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
	Bat_Timeout = BAT_FLAG_TIMEOUT;
  while ((BAT_I2C->CR1&0x200) == 0x200)
  {
    if((Bat_Timeout--) == 0) return 0;
  }		

  // * Enable Acknowledgement to be ready for another reception * /
  I2C_AcknowledgeConfig(BAT_I2C, ENABLE);
  
  return (uint16_t)((byte1 << 8)| byte0);
}

uint16_t ReadBatVoltage(void)
{
	return ReadWord(BAT_VOLTAGE_MSB);
}

uint16_t ReadBatCurrent(void)
{
	return ReadWord(BAT_CURRENT_MSB);
}

uint16_t ReadBatTemperature(void)
{
	return ReadWord(BAT_TEMPERATURE_MSB);
}

uint8_t GetBatStatus(void)
{
	return ReadByte(BAT_STATUS_REG);
}

uint8_t ARA_process(void)
{
  uint8_t       byte1 = 0;
  __IO uint32_t temp = 0;
    
  // /* While the bus is busy * /
	Bat_Timeout = BAT_FLAG_TIMEOUT;
  while(I2C_GetFlagStatus(BAT_I2C, I2C_FLAG_BUSY))
  {
    if((Bat_Timeout--) == 0) return 0;
  }	
  // * Send START condition * /
  I2C_GenerateSTART(BAT_I2C, ENABLE);

  // / * Test on EV5 and clear it * /
	Bat_Timeout = BAT_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(BAT_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((Bat_Timeout--) == 0) return 0;
  }	

  // / * Send EEPROM address for write  * /
  I2C_Send7bitAddress(BAT_I2C, ARA_RESPONSE_AADR, I2C_Direction_Receiver);

  /* Wait until ADDR is set */
  Bat_Timeout = BAT_FLAG_TIMEOUT;
  while ((BAT_I2C->SR1&0x0002) != 0x0002)
  {
    if((Bat_Timeout--) == 0) return 0;
  }			

  /* Clear ACK bit */
  BAT_I2C->CR1 &= CR1_ACK_Reset;
  /* Disable all active IRQs around ADDR clearing and STOP programming because the EV6_3
  software sequence must complete before the current byte end of transfer */
  __disable_irq();
  /* Clear ADDR flag */
  temp = BAT_I2C->SR2;
  /* Program the STOP */
  I2C_GenerateSTOP(BAT_I2C, ENABLE);
  /* Re-enable IRQs */
  __enable_irq();
    /* Wait until a data is received in DR register (RXNE = 1) EV7 */
	Bat_Timeout = BAT_FLAG_TIMEOUT;
  while ((BAT_I2C->SR1 & 0x00040) != 0x000040)
  {
    if((Bat_Timeout--) == 0) return 0;
  }		
  /* Read the data */
  byte1 = BAT_I2C->DR;
  
  return byte1;
}

uint8_t VerifyBatSensor(void)
{
	uint8_t retValue;
		
	Bat_RegisterWrite(BAT_CONTROL_REG, 0x3C);
	retValue = ReadByte(BAT_CONTROL_REG);	
	setFont(arial15);
	if(retValue == 0x3C)
	{
		 retValue  = ARA_process();      // Clear ALCC pin to high
		
		 //print_ttf((char*)displayBuff, 10, 305, 0);
		
		 sprintf((char*)displayBuff,"Bat");
		 print_ttf((char*)displayBuff,10,305,0);
	   //sprintf((char*)displayBuff,"50%%");
		 //print_ttf((char*)displayBuff,38,305,0);
		 EnableReadBattery = 1;
				 
	   //BatSensorSetup(BAT_VOLTAGE_THRESHOLD_HIGH_MSB, 0xB63C);    // High Threshold Voltage = 16.8V
		 //Bat_RegisterWrite(BAT_VOLTAGE_THRESHOLD_HIGH_MSB, 0xB6);   // 9B1E = 14.3
		 //Bat_RegisterWrite(BAT_VOLTAGE_THRESHOLD_HIGH_LSB, 0x3C);
		 BatSensorSetup(BAT_VOLTAGE_THRESHOLD_HIGH_MSB, 0xBDD3);      // High Threshold Voltage = 17.5V
		 BatSensorSetup(BAT_CURRENT_ThRESHOLD_HIGH_MSB, 0xF553);      // High Threshold Current = 2.2A
	 //BatSensorSetup(BAT_TEMPERATURE_THRESHOLD_HIGH, 0xB635);      // High Threshold Temperature = 90C
		 BatSensorSetup(BAT_TEMPERATURE_THRESHOLD_HIGH, 0xAC2B);      // High Threshold Temperature = 70C
		
		 GetBatStatus();                                              // Clear all status flags
		
		 Bat_RegisterWrite(BAT_CONTROL_REG, 0xBC);                    // Enable ADC measurement		
	}
  else
  {
		 //sprintf((char*)displayBuff,"NoBat");	   
		 //print_ttf((char*)displayBuff,10,305,0);
		 EnableReadBattery = 0;
  }		
	return retValue;
}

void BatVoltage_Display(int voltage)
{
  //int temp;
  float temp_f;
	
	//temp = (int)(ReadTemperature(TEMP_SENSOR_TEMP);
	
	temp_f = voltage;
	temp_f = (voltage/128.0f) * 0.5f;
  sprintf((char*)displayBuff,"T%5.1fC", temp_f);
	sLCD_putString_TFT(0,46,displayBuff,Font16x24);//displayBuff);
}

extern __IO uint8_t treatment_on_flag;                           // Added by Jason Chen, 2015.07.20
uint16_t BatVoltage_Display_normal(void)
{
  int temp=0;
  float temp_f,temp_fc;
//	uint8_t status;
	uint16_t tempColor;
	uint16_t current;
	
  if((BatteryDispay>30)&&(EnableReadBattery))                        // Sample rate Changed from 10 to 30, 2016.02.18
	{				
		BatteryDispay = 0;
		//if(!treatment_on_flag)                                         // Added by Jason Chen, 2015.07.20
		{
			/*status = */GetBatStatus();                                          // Clear all status flags		
	#if 0				
			if(status & 0x02)
			{
				 Bat_RegisterWrite(BAT_CONTROL_REG, 0x3A);
				 ReadByte(BAT_CONTROL_REG);	
				 ARA_process();                                                 // Clear ALCC pin to high				 
				 Bat_RegisterWrite(BAT_VOLTAGE_THRESHOLD_HIGH_MSB, 0x9B);       // 9B1E = 14.3
				 Bat_RegisterWrite(BAT_VOLTAGE_THRESHOLD_HIGH_LSB, 0x1E);
				 Bat_RegisterWrite(BAT_CONTROL_REG, 0xBC);                      // Enable ADC measurement
				 GetBatStatus(); 
				 Delay(500);
			}
	#endif		
			
			temp = (int)ReadBatVoltage();		
			if(temp == 1)
			{
				 Bat_Init();
				 Delay(5);
				 VerifyBatSensor();
				 return 1;
			}
			temp_f = temp;
			temp_f = (temp_f*23.6f/65535.0f);
			battery_profile.bat_feedback.gas_gauge_value = temp_f;
	#if 1		
			setFont(arial16);		
			temp_f = ((temp_f-LOW_BATTERY_VOLTAGE)*100.0f)/COMMON_RATIO + 5.0f;		
			tempColor = foreground_color;
			//if(battery_profile.bat_feedback.gas_gauge_value <= 12.0f)
			if(temp_f <= 10.0f)
			{
				//foreground_color = COLOR_RED1;
				setColor(COLOR_RED1);
				battery_profile.bat_feedback.lowBatteryAlarm = 1;			
				if(temp_f < 0.0f)
					temp_f = 0.0f;
			}
			else
			{
				if(temp_f < 33)
					setColor(COLOR_YELLOW1);
				else
					setColor(COLOR_LIME1);
				battery_profile.bat_feedback.lowBatteryAlarm = 0;
				if(temp_f > 100.0f)
					temp_f = 100.0f;
			}
			
			current = ReadBatCurrent();
			temp_fc = (60.0f/25.0f)*(current-32767.0f)/32767.0f;
						
			//sprintf((char*)displayBuff,"%2.1f%%%2.3f", temp_f,battery_profile.bat_feedback.gas_gauge_value);
			if((battery_profile.bat_feedback.gas_gauge_value > 16.770f)&&(temp_fc < 0.00010f))
			{
				//print_ttf((char*)"         ",   38, 305, 0);
				//print_ttf((char*)"No Bat      ",10, 305, 0);
				  print_ttf((char*)"                 ",10, 305, 0);
			}
			else
			{
					sprintf((char*)displayBuff,"%2.0f%%  ", temp_f);
					print_ttf((char*)displayBuff, 38, 305, 0);
			}
			
			setColor(tempColor);
			
	  //sprintf((char*)displayBuff,"%2.3f %1.5f ", battery_profile.bat_feedback.gas_gauge_value,temp_fc);
		//sLCD_putString_TFT_ext(3/*150-30*/,3+16,displayBuff, hallfetica_normal16x16);
	#else
			sprintf((char*)displayBuff,"%2.1f %d", temp_f,status);
			sLCD_putString_TFT_ext(20,305,displayBuff, hallfetica_normal16x16);
			setFont(DejaVuSans18);
	#endif				
			//sLCD_putString_TFT(8*16,25/*53*/,displayBuff,Font16x24);
		}	  
  }
	return temp;
}	
