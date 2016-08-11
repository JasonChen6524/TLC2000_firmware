/**
  ******************************************************************************
  * @file    Potentiometer.c
  * @author  Jason Chen
  * @version V1.0
  * @date    18-July-2014
  * @brief   
  *            
  */ 

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "TLC2000_SPI_LCD_TFT.h"
#include "Potentiometer.h"
#include "MiscTools.h"
#include "fonts.h"

#include "calibration.h"                                // Added by Jason Chen, 2015.04.09
#include "BatSensor.h"                                  // Added by Jason Chen, 2015.07.03

/* I2C STOP mask */
#define CR1_STOP_Set            ((uint16_t)0x0200)
#define CR1_STOP_Reset          ((uint16_t)0xFDFF)

/* I2C ACK mask */
#define CR1_ACK_Set             ((uint16_t)0x0400)
#define CR1_ACK_Reset           ((uint16_t)0xFBFF)

/* I2C POS mask */
#define CR1_POS_Set             ((uint16_t)0x0800)
#define CR1_POS_Reset           ((uint16_t)0xF7FF)

__IO int32_t  POT_Timeout = POT_LONG_TIMEOUT; 
extern uint8_t displayBuff[];

/**
  * @brief  Initializes Potentiometer
  * @param  None
  * @retval None
  */

__IO uint32_t TemperatureDispay = 0;
uint8_t HighTemperatureAlarm = 0;
uint8_t HighTemperatureClear = 0;                             // Added by Jason Chen, 2015.07.04

#if TEMPERATURE_DISPLAY_TEST
uint16_t HighTemperatureCount = 0;                            // Added by Jason Chen, 2015.07.04
#endif

void Pot_LowLevel_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
//  EXTI_InitTypeDef EXTI_InitStructure;
//  NVIC_InitTypeDef NVIC_InitStructure;
    
  /*!< sEE_I2C_SCL_GPIO_CLK and sEE_I2C_SDA_GPIO_CLK Periph clock enable */
  RCC_APB2PeriphClockCmd(POT_I2C_SCL_GPIO_CLK | POT_I2C_SDA_GPIO_CLK | INT_TEMP_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);	
	RCC_APB2PeriphClockCmd(INPUT_CH0_GPIO_CLK | INPUT_CH1_GPIO_CLK | INPUT_CH2_GPIO_CLK | INPUT_CH3_GPIO_CLK, ENABLE);
	
  /*!< sEE_I2C Periph clock enable */
  RCC_APB1PeriphClockCmd(POT_I2C_CLK, ENABLE);
    
  /*!< GPIO configuration */  
  /*!< Configure sEE_I2C pins: SCL, SDA */
  GPIO_InitStructure.GPIO_Pin = POT_I2C_SCL_PIN | POT_I2C_SDA_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(POT_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);

  /*!< Configure sEE_I2C pins: SDA */
//  GPIO_InitStructure.GPIO_Pin = POT_I2C_SDA_PIN;
  //GPIO_Init(POT_I2C_SDA_GPIO_PORT, &GPIO_InitStructure); 

  GPIO_InitStructure.GPIO_Pin = INPUT_CH0|INPUT_CH1|INPUT_CH2|INPUT_CH3;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(INPUT_CH0_GPIO_PORT, &GPIO_InitStructure);

  //GPIO_InitStructure.GPIO_Pin = INPUT_CH1;
  //GPIO_Init(INPUT_CH1_GPIO_PORT, &GPIO_InitStructure);
  //GPIO_InitStructure.GPIO_Pin = INPUT_CH2;
  //GPIO_Init(INPUT_CH2_GPIO_PORT, &GPIO_InitStructure);
  //GPIO_InitStructure.GPIO_Pin = INPUT_CH3;
  //GPIO_Init(INPUT_CH3_GPIO_PORT, &GPIO_InitStructure);
	
  GPIO_InitStructure.GPIO_Pin = INT_TEMP_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(INT_TEMP_GPIO_PORT, &GPIO_InitStructure);	
	
#if ENALE_HI_TEMP
/* Connect EXTI5 Line to PB.05 pin */
	GPIO_EXTILineConfig(INT_TEMP_EXTI_PORT_SOURCE, INT_TEMP_EXTI_PIN_SOURCE);

	/* Configure Button EXTI line */
	EXTI_InitStructure.EXTI_Line    = INT_TEMP_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;//EXTI_Trigger_Falling;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
				
	/* Clear the Tamper EXTI line pending bit */
	EXTI_ClearITPendingBit(INT_TEMP_EXTI_LINE);		

	/* Enable and set Button EXTI Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel                   = INT_TEMP_EXTI_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;

	NVIC_Init(&NVIC_InitStructure); 
#endif
	
  //GPIO_InitStructure.GPIO_Pin = SW_PIN;                         // 2014.10.20
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;         // 2014.10.20
  //GPIO_Init(SW_GPIO_PORT, &GPIO_InitStructure);	                // 2014.10.20
}

void Enable_Input(uint8_t channel, uint8_t enable_ch)
{
	  if(channel == 0)
		{
			if(enable_ch != 0) GPIO_ResetBits(INPUT_CH0_GPIO_PORT, INPUT_CH0);
			else               GPIO_SetBits(INPUT_CH0_GPIO_PORT, INPUT_CH0);
		}
		else if(channel == 1)
		{
			if(enable_ch != 0) GPIO_ResetBits(INPUT_CH1_GPIO_PORT, INPUT_CH1);
			else               GPIO_SetBits(INPUT_CH1_GPIO_PORT, INPUT_CH1);			
		}
		else if(channel == 2)
		{
		  if(enable_ch != 0) GPIO_ResetBits(INPUT_CH2_GPIO_PORT, INPUT_CH2);
			else               GPIO_SetBits(INPUT_CH2_GPIO_PORT, INPUT_CH2);
		}
		else if(channel == 3)
		{
			if(enable_ch != 0) GPIO_ResetBits(INPUT_CH3_GPIO_PORT, INPUT_CH3);
			else               GPIO_SetBits(INPUT_CH3_GPIO_PORT, INPUT_CH3);
		}	
}

void Enable_All_Input(void)
{
	GPIO_ResetBits(INPUT_CH0_GPIO_PORT, INPUT_CH0);
	GPIO_ResetBits(INPUT_CH0_GPIO_PORT, INPUT_CH1);
	GPIO_ResetBits(INPUT_CH0_GPIO_PORT, INPUT_CH2);
	GPIO_ResetBits(INPUT_CH0_GPIO_PORT, INPUT_CH3);	
}

void Disable_All_Input(void)
{
	GPIO_SetBits(INPUT_CH0_GPIO_PORT, INPUT_CH0);
	GPIO_SetBits(INPUT_CH0_GPIO_PORT, INPUT_CH1);
	GPIO_SetBits(INPUT_CH0_GPIO_PORT, INPUT_CH2);
	GPIO_SetBits(INPUT_CH3_GPIO_PORT, INPUT_CH3);
}

void Pot_Init(void)
{
  I2C_InitTypeDef  I2C_InitStructure;
  
	//I2C_DeInit(POT_I2C);
		
  Pot_LowLevel_Init();
  
  /*!< I2C configuration */
  /* POT_I2C configuration */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = I2C_SLAVE_ADDRESS7_PD0;//U18;//I2C_address;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;
  
  /* sEE_I2C Peripheral Enable */
  I2C_Cmd(POT_I2C, ENABLE);
  /* Apply sEE_I2C configuration after enabling it */
  I2C_Init(POT_I2C, &I2C_InitStructure);
	
	Disable_All_Input();
	
	//Pot_SetPDx(I2C_SLAVE_ADDRESS7_PD0, CH_RDAC2|MIDSCALE_RST, 0x80);
	//Pot_SetPDx(I2C_SLAVE_ADDRESS7_PD1, CH_RDAC1|MIDSCALE_RST, 0x80);
	//Pot_SetPDx(I2C_SLAVE_ADDRESS7_PD2, CH_RDAC1|MIDSCALE_RST, 0x80);
	//Pot_SetPDx(I2C_SLAVE_ADDRESS7_PD3, CH_RDAC1|MIDSCALE_RST, 0x80);
	//Pot_SetPDx(I2C_SLAVE_ADDRESS7_PD0, CH_RDAC1|MIDSCALE_RST, 0x80);
}

#if 0
uint8_t Pot_SetPDx(uint8_t i2c_addr, uint8_t mode, uint8_t gain)
{
   
  /* Enable the I2C peripheral */
  I2C_GenerateSTART(POT_I2C, ENABLE);
  
  /* Test on SB Flag */
  POT_Timeout = POT_FLAG_TIMEOUT;
//while(!I2C_CheckEvent(POT_I2C, I2C_EVENT_MASTER_MODE_SELECT)); 	
  while (!I2C_CheckEvent(POT_I2C, I2C_EVENT_MASTER_MODE_SELECT)) 
  {
    if((POT_Timeout--) == 0) return 1;
		//LED_On();
  }
  
  /* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(POT_I2C, i2c_addr, I2C_Direction_Transmitter);
  
  /* Test on ADDR Flag */
  POT_Timeout = POT_FLAG_TIMEOUT;
  //while (!I2C_CheckEvent(POT_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  while (!I2C_CheckEvent(POT_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((POT_Timeout--) == 0) return 1;
		//LED_Off();
  }
  
  /* Transmit the first address for r/w operations */
  I2C_SendData(POT_I2C, mode);
  while (!I2C_CheckEvent(POT_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((POT_Timeout--) == 0) return 1;
		//LED_On();
  }
	
  /* Transmit the first address for r/w operations */
  I2C_SendData(POT_I2C, gain);
  while (!I2C_CheckEvent(POT_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((POT_Timeout--) == 0) return 1;
  }	
	  
  /* Send STOP Condition */
  I2C_GenerateSTOP(POT_I2C, ENABLE);
  
  return 0;
}

uint8_t Pot_SetAMP2(uint8_t gain)
{
   
  //Pot_SetPDx(I2C_SLAVE_ADDRESS7_PD3, gain, CH_RDAC2);
	Pot_SetPDx(I2C_SLAVE_ADDRESS7_PD0, CH_RDAC1, gain);
  
  return 0;
}
#endif

uint8_t TempSensorConfig(uint8_t sel, uint8_t value)
{
  /* Enable the I2C peripheral */
  I2C_GenerateSTART(POT_I2C, ENABLE);
  
  /* Test on SB Flag */
  POT_Timeout = POT_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(POT_I2C, I2C_EVENT_MASTER_MODE_SELECT)) 
  {
    if((POT_Timeout--) == 0) return 1;
  }
  
  /* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(POT_I2C, I2C_SLAVE_ADDRESS7_TEMPSENSOR, I2C_Direction_Transmitter);
  
  /* Test on ADDR Flag */
  POT_Timeout = POT_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(POT_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((POT_Timeout--) == 0) return 1;
  }
  
  /* Transmit the first address for r/w operations */
  I2C_SendData(POT_I2C, sel);
	POT_Timeout = POT_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(POT_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((POT_Timeout--) == 0) return 1;
  }
	
  /* Transmit the first address for r/w operations */
  I2C_SendData(POT_I2C, value);
	POT_Timeout = POT_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(POT_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((POT_Timeout--) == 0) return 1;
  }	
	  
  /* Send STOP Condition */
  I2C_GenerateSTOP(POT_I2C, ENABLE);
  
  return 0;	
}

uint8_t TempSensorSetup(uint8_t sel, uint16_t value)
{
	uint16_t wordValue = value<<7;
	uint8_t ByteValue = (uint8_t)((wordValue >> 8)&0xFF);
  /* Enable the I2C peripheral */
  I2C_GenerateSTART(POT_I2C, ENABLE);
  
  /* Test on SB Flag */
  POT_Timeout = POT_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(POT_I2C, I2C_EVENT_MASTER_MODE_SELECT)) 
  {
    if((POT_Timeout--) == 0) return 1;
  }
  
  /* Transmit the slave address and enable writing operation */
  I2C_Send7bitAddress(POT_I2C, I2C_SLAVE_ADDRESS7_TEMPSENSOR, I2C_Direction_Transmitter);
  
  /* Test on ADDR Flag */
  POT_Timeout = POT_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(POT_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((POT_Timeout--) == 0) return 1;
  }
  
  /* Transmit the first address for r/w operations */
  I2C_SendData(POT_I2C, sel);
  while (!I2C_CheckEvent(POT_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((POT_Timeout--) == 0) return 1;
  }
	
  /* Transmit the first address for r/w operations */
  I2C_SendData(POT_I2C, ByteValue);
	POT_Timeout = POT_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(POT_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((POT_Timeout--) == 0) return 1;
  }	
	  
  /* Transmit the first address for r/w operations */
  ByteValue = (uint8_t) (value&0xFF);
  I2C_SendData(POT_I2C, ByteValue);
	POT_Timeout = POT_FLAG_TIMEOUT;
  while (!I2C_CheckEvent(POT_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((POT_Timeout--) == 0) return 1;
  }	
	
  /* Send STOP Condition */
  I2C_GenerateSTOP(POT_I2C, ENABLE);
  
  return 0;	
}

void TempSensorTCN75Init(void)
{
	TempSensorConfig(TEMP_SENSOR_CONFIG, FAULT_QUEUE_4|/*TEMP_INT_MODE|*/TEMP_ACTIVE_LOW);
	//TempSensorSetup(TEMP_SENSOR_TSET, SENSORTEMP_40_DEGRESS);
	TempSensorSetup(TEMP_SENSOR_TSET, SENSORTEMP_45_DEGRESS);                 // Added by Jason Chen, 2015.07.03
	//TempSensorSetup(TEMP_SENSOR_THYST, SENSORTEMP_35_DEGRESS);
	TempSensorSetup(TEMP_SENSOR_THYST, SENSORTEMP_40_DEGRESS);                // Added by Jason Chen, 2015.07.03
	
	EXTI_ClearITPendingBit(INT_TEMP_EXTI_LINE);
}

uint32_t Temp_Int_PBGetState(void)
{
  return GPIO_ReadInputDataBit(INT_TEMP_GPIO_PORT, INT_TEMP_PIN );
}


uint16_t ReadTemperature(uint8_t reg_sel)
{
  uint8_t       byte0 = 0;
  uint8_t       byte1 = 0;
  __IO uint32_t temp = 0;
    
  
  // /* While the bus is busy * /
	POT_Timeout = POT_FLAG_TIMEOUT;
  while(I2C_GetFlagStatus(POT_I2C, I2C_FLAG_BUSY))
  {
    if((POT_Timeout--) == 0) return 1;
  }	
  // * Send START condition * /
  I2C_GenerateSTART(POT_I2C, ENABLE);

  // / * Test on EV5 and clear it * /
	POT_Timeout = POT_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(POT_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((POT_Timeout--) == 0) return 1;
  }	

  // / * Send EEPROM address for write  * /
  I2C_Send7bitAddress(POT_I2C, I2C_SLAVE_ADDRESS7_TEMPSENSOR, I2C_Direction_Transmitter);

  // / * Test on EV6 and clear it * /
	POT_Timeout = POT_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(POT_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((POT_Timeout--) == 0) return 1;
  }		

  // / * Send the EEPROM's internal address to read from: Only one byte address  * /
  I2C_SendData(POT_I2C, reg_sel);     

  /// * Test on EV8 and clear it * /
	POT_Timeout = POT_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(POT_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
  {
    if((POT_Timeout--) == 0) return 1;
  }		
    
  /// * Send STRAT condition a second time * /  
  I2C_GenerateSTART(POT_I2C, ENABLE);

  /// * Test on EV5 and clear it * /
	POT_Timeout = POT_FLAG_TIMEOUT;
  while(!I2C_CheckEvent(POT_I2C, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((POT_Timeout--) == 0) return 0;
  }		
  
    // * Send EEPROM address for read * /
  I2C_Send7bitAddress(POT_I2C, I2C_SLAVE_ADDRESS7_TEMPSENSOR, I2C_Direction_Receiver);  
                  
  /* Set POS bit */
  POT_I2C->CR1 |= CR1_POS_Set;
  /* Wait until ADDR is set: EV6 */
	POT_Timeout = POT_FLAG_TIMEOUT;
  while ((POT_I2C->SR1&0x0002) != 0x0002)
  {
    if((POT_Timeout--) == 0) return 0;
  }		
  /* EV6_1: The acknowledge disable should be done just after EV6,
  that is after ADDR is cleared, so disable all active IRQs around ADDR clearing and 
  ACK clearing */
  __disable_irq();
  /* Clear ADDR by reading SR2 register  */
  temp = POT_I2C->SR2;
  /* Clear ACK */
  POT_I2C->CR1 &= CR1_ACK_Reset;
  /*Re-enable IRQs */
  __enable_irq();
  /* Wait until BTF is set */
	POT_Timeout = POT_FLAG_TIMEOUT;
  while ((POT_I2C->SR1 & 0x00004) != 0x000004)
  {
    if((POT_Timeout--) == 0) return 0;
  }		
  /* Disable IRQs around STOP programming and data reading */
  __disable_irq();
  /* Program the STOP */
  I2C_GenerateSTOP(POT_I2C, ENABLE);
    /* Read first data */
  byte1 = POT_I2C->DR;
  /* Re-enable IRQs */
  __enable_irq();
  /**/
  /* Read second data */
  byte0 = POT_I2C->DR;
  /* Clear POS bit */
  POT_I2C->CR1  &= CR1_POS_Reset;
 
  /* Make sure that the STOP bit is cleared by Hardware before CR1 write access */
	POT_Timeout = POT_FLAG_TIMEOUT;
  while ((POT_I2C->CR1&0x200) == 0x200)
  {
    if((POT_Timeout--) == 0) return 0;
  }		

  // * Enable Acknowledgement to be ready for another reception * /
  I2C_AcknowledgeConfig(POT_I2C, ENABLE);
  
  return (uint16_t)((byte1 << 8)| byte0);
}

void Temperature_Display(int temp_i)
{
  //int temp;
  float temp_f;
	
	//temp = (int)(ReadTemperature(TEMP_SENSOR_TEMP);
	
	temp_f = temp_i;
	temp_f = (temp_f/128.0f) * 0.5f;
  sprintf((char*)displayBuff,"T%5.1fC", temp_f);
	sLCD_putString_TFT(0,46,displayBuff,Font16x24);//displayBuff);
}

void Enable_temperature_reading(uint8_t enableReading)           // Added by Jason Chen, 2015.04.09
{
	//HighTemperatureAlarm = enableReading;                        // Removed by Jason Chen, 2015.07.20
}

void DisplayHiTemp(void) 
{
   sLCD_putString_TFT(5,80-24-12,(uint8_t*)"  Hi Temp ",Font16x24);
}

uint16_t Temperature_Display_normal(void)
{
  static int temp=0;
  float temp_f;
	uint16_t tempProbeColor;                          // Added by Jason Chen, 2016_07.03
	
  if(TemperatureDispay)
	{		
		TemperatureDispay = 0;
#if TEMPERATURE_DISPLAY_TEST		
		// Added by Jason Chen, 2015.07.04 for testing
	  tempProbeColor = foreground_color;                               
	  foreground_color = COLOR_RED1;
  #if 0		
    temp++;                                         		           
    sprintf((char*)displayBuff,"%d   ", temp);
    sLCD_putString_TFT_ext(150,3,displayBuff, hallfetica_normal16x16);
  #else
		DisplayHiTemp();
	#endif
	  foreground_color = tempProbeColor;                               		
#else                                                              
	  tempProbeColor = foreground_color;                               
	  foreground_color = COLOR_RED1;   
		temp = (int)ReadTemperature(TEMP_SENSOR_TEMP);		             // Added by Jason Chen, 2015.04.09		
		if(userProfile.user_profile.probeTemperature < temp)           // Added by Jason Chen, 2015.04.09
			userProfile.user_profile.probeTemperature = temp;			       // Added by Jason Chen, 2015.04.09

	  temp_f = temp;
	  temp_f = (temp_f/128.0f) * 0.5f;
		battery_profile.bat_feedback.probeTempValue = temp_f;          // Added by Jason Chen, 2015.07.03
	#if 0		      			                                                      
    sprintf((char*)displayBuff,"%2.1fC ", temp_f);                 // Added by Jason Chen, 2015.07.03
    sLCD_putString_TFT_ext(150,3,displayBuff, hallfetica_normal16x16);
	#else	
		DisplayHiTemp();
	#endif
	  foreground_color = tempProbeColor;                              
#endif		
  }
	return temp;
}	

extern __IO uint8_t treatment_on_flag ;
void TemperatureDisplayClear(void)                                 // Added by Jason Chen, 2015.07.03 for display testing
{
	#if 0
     sLCD_putString_TFT_ext(150,3,(uint8_t*)"     ", hallfetica_normal16x16);
	#else
	 //sLCD_putString_TFT(5,80-24-SHIFT_DOWN,(uint8_t*)"  Hi Temp ",Font16x24);
	 //sLCD_putString_TFT(5,80-24-12,(uint8_t*)"          ",Font16x24);
	   if(treatment_on_flag)                                         // Added by Jason Chen, 2015.07.22
				 Display_TENsOn();
		 else
			   Display_TENsOff();
	#endif
}
