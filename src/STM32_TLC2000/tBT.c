/**
  ******************************************************************************
  * @file    tUart.c
  * @author  Harry Yan
  * @version V1.0
  * @date    18-Aug-2014
  * @brief   
  *            
  */ 

/* Includes ------------------------------------------------------------------*/
#include "tBT.h"
#include "MiscTools.h"
#include "calibration.h"
#include "Hid_command_process.h"
#include "custom_hid_conf.h"
#include "RTC.h"
#include "CPU_ADC.h"
#include "DAC7718.h"
#include "FPGA_RAM.h"
#include "Potentiometer.h"
#include "MiscTools.h"
#include "Buzzer.h"
#include "ADS8319_ADC.h"
#include "Switch2.h"
#include "TLC2000_SPI_LCD_TFT.h"
#include "stdio.h"
#include "string.h"
#include "FPGA_RAM.h"

/* Private typedef -----------------------------------------------------------*/
//typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;



/* Private define ------------------------------------------------------------*/
#define TxBufferSize   0x20
#define RxBufferSize   0xff

/* Private macro -------------------------------------------------------------*/
#define countof(a)   (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
__IO TLC_MEASUREMENT_SETTING tlcMeasurementSetting;
__IO TLC_TREATMENT_SETTING tlcTreatmentSetting;
__IO TLC_REPORT tlcReport;
__IO TLC_SYSTEM_SETTING tlcSystem;
__IO BLE_CONFIG pcCmd; 
__IO BLE_CONFIG oldPcCmd;

USART_InitTypeDef USART_InitStructure;
uint8_t TxBuffer[TxBufferSize] ;//= "ATSPLE,2,0,3\r";//"ATSUART,7,0,0,1\r";
uint8_t RxBuffer[RxBufferSize];
uint8_t NbrOfDataToTransfer = 0;
uint8_t NbrOfDataToRead = RxBufferSize;

uint8_t TxCounter = 0;
uint8_t RxCounter = 0;

__IO UartParams uartComm;

__IO TestStatus TransferStatus = FAILED; 
__IO uint8_t recvFlag = 0; 

/* Private function prototypes -----------------------------------------------*/
static void Uart_RCC_Configuration(void);
static void Uart_GPIO_Configuration(void);
static void Uart_NVIC_Configuration(void);


void TLC_UartInit(void)
{
 /* System Clocks Configuration */
  Uart_RCC_Configuration();
  Uart_NVIC_Configuration();
  /* Configure the GPIO ports */
  Uart_GPIO_Configuration();
  
  
/* USART2 configuration ------------------------------------------------------*/
  /* USART2 configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control enabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl =  USART_HardwareFlowControl_RTS_CTS;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  USART_Init(USART2, &USART_InitStructure);
  /* Enable USARTy Receive and Transmit interrupts */
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
  //USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
  /* Enable the USART2 */
  
  USART_Cmd(USART2, ENABLE);

  uartComm.RXg = 0;
  uartComm.RXp = 0;
	
  uartComm.TXg = 0;
  uartComm.TXp = 0;
	
  uartComm.RXl = UART_COMM_BUFF_SIZE;
  uartComm.TXl = UART_COMM_BUFF_SIZE;

  uartComm.TXf =0;
  uartComm.RXf =0;
  #if 0
  /* Receive a string (Max RxBufferSize bytes) from the Hyperterminal ended by '\r' (Enter key) */
  do
  { 
    if((USART_GetFlagStatus(USART2, USART_FLAG_RXNE) != RESET)&&(RxCounter < RxBufferSize))
    {
       RxBuffer[RxCounter] = USART_ReceiveData(USART2);
       USART_SendData(USART2, RxBuffer[RxCounter++]);
    }   
 
  }while((RxBuffer[RxCounter - 1] != '\r')&&(RxCounter != RxBufferSize));
  #endif

 
}

int TLC_BT_Rx(void)
{
//     int cnt =0;
	 if(RxBuffer[0]== 'T')
	 	return 2;
     return 0;
}

int TLC_BT_Tx(void)
{
	while(NbrOfDataToTransfer--)
	 {
	   USART_SendData(USART2, TxBuffer[TxCounter++]);
	   while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET); 		 
	 }
	TxCounter = 0;
	NbrOfDataToTransfer = TxBufferSize;
	 
	 return 0;

 #if 0
  int count;
  uint8_t c;
  
  for(count = 0; *s != 0; count++)
  {
	  c = *s++;
	  USART_SendData(USART2, c);
  }
  return(count);
 #endif
}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
static void Uart_RCC_Configuration(void)
{    
  /* Enable GPIOx and AFIO clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

  /* Enable USART2 clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
}

/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
static void Uart_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

#if defined(USE_STM3210B_EVAL) || defined(USE_STM32100B_EVAL)
  /* Enable the USART2 Pins Software Remapping */
  //GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
#endif

  /* Configure USART2 RTS and USART2 Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2| GPIO_Pin_1;// GPIO_RTSPin | GPIO_TxPin;GPIO_Pin_1 | 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Configure USART2 CTS and USART2 Rx as input floating */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3| GPIO_Pin_0;//GPIO_CTSPin | GPIO_RxPin;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}
static void Uart_NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure the NVIC Preemption Priority Bits */  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  
  
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}

byte uartCommGetc(void)
{
	volatile byte index;
	volatile byte c;
	if(uartComm.RXg == uartComm.RXp)
  {
		    uartComm.RXf = 0;								// rx flag is only an indication
        return(0xff);
  }
	
	c = uartComm.RX[uartComm.RXg];
	index = uartComm.RXg;
	if(++index == uartComm.RXl)
		uartComm.RXg = 0;
	else
		uartComm.RXg = index;
	return(c);	
}

int uartCommGetb(byte *s, int len)
{
	volatile byte index;
	int count;

	for(count = 0; count < len; count++)
	{
		if(uartComm.RXg ==uartComm.RXp)
		{
			uartComm.RXf = 0;								// rx flag is only an indication
			return(count);								    // disable()/enable() for exact
		}
		
		*s++ = uartComm.RX[uartComm.RXg] & 0xff;
		index = uartComm.RXg;
		if(++index == uartComm.RXl)
			uartComm.RXg = 0;
		else
			uartComm.RXg = index;
	}
	return(count);
}

byte uartCommPutc( byte c)
{
   volatile byte  index;
   index  = uartComm.TXp;
   if(++index == uartComm.TXl)
   	 index = 0;
   while( index == uartComm.TXg) 
   	{
   	  __ASM volatile ("nop");
	    __ASM volatile ("nop");
	    __ASM volatile ("nop");

   	}
   	  
   uartComm.TX[uartComm.TXp] = c;
   uartComm.TXp = index;

   if(uartComm.TXf  == 0)
   {
		USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
		uartComm.TXf =1;
    USART_ITConfig(USART2, USART_IT_TXE, ENABLE); 
   }
   return 0;
}

int uartCommPutb( byte *s, int len)
{
	int count;
	
	for(count = 0; count < len; count++)
	{
		if(uartCommPutc( *s++))
			break;
	}
	return(count);
}
int uartCommPuts(char *s)
{
	int count;
	byte c;
	
	for(count = 0; *s != 0; count++)
	{
		c = *s++;
		if(uartCommPutc(c))
		  break;
	}
	return(count);
}


#define STUPID_PKT_LEN  31
byte stupidPkt[STUPID_PKT_LEN];
byte stupidRx[STUPID_PKT_LEN];

int uartConstructPkt( byte *s, int len)
{
	int count;
	static const byte hex[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	for(count = 0; count < len*2; )
	{
		stupidPkt[count+1] =  hex[*s >> 4];
		count++;
		stupidPkt[count+1] =   hex[*s++ & 0xf];	
		count++;
	}
	
	return(count);
}

byte uartChar1Hex( byte b)
{
	  byte temp = b;
    if(b<='9'&& b>='0')
		  temp = b-'0';
	  else if(b<='F' && b>= 'A')
		  temp = b-'A'+10;
    else 
			temp = 0xff;
	  return(temp);
}

static int bleChar2Hex(byte* b)
{
    byte temp[2];
	  temp[0] = uartChar1Hex( b[0]);
    if(temp[0]== 0xff)
		   return -1;
	  temp[1] = uartChar1Hex( b[1]);
	  if(temp[1]== 0xff)
		   return -1;
		
	  return((int)temp[1]| (int)temp[0]<<4);		
}




static  BLE_CMD_STAT uartStat =BLE_CMD_STAT_PRE1;

BLE_CMD_TYPE getCmd(byte *s )
{     
	 static int i,j;
	 BLE_CMD_TYPE cmd;
// byte len;                                             //2016.07.12
// byte ret = 99;                                        //2016.07.12
	 
	 int iRead;
	 int result =0;
	 //byte index;
// static  byte first = 0;                               //2016.07.12
	 byte c;
	 static byte mByte[2];
	 static char tBuf[32];
	 
   c = uartCommGetc();
	 cmd = BLE_CMD_TYPE_NONE;
	 if(c==0xff)
		 return BLE_CMD_TYPE_NONE;
	 
	 switch(uartStat)
   {
      case BLE_CMD_STAT_PRE1:
		  	if(c =='T')
			  {
				  uartStat =BLE_CMD_STAT_PRE2;
			  }
	      break;
      case BLE_CMD_STAT_PRE2:
	      if(c =='L')
		   	{
				  uartStat =BLE_CMD_STAT_CMD;
		   	}					
			  else 
				  uartStat =BLE_CMD_STAT_PRE1;
	      break;
 	
      case BLE_CMD_STAT_CMD:
        i = 0;
			  j = 0;
			  if(c =='T') //set measurement
		   	{
	        	uartStat = BLE_CMD_STAT_SET_MEASUREMENT;
		   	}
			  else if(c =='G') //get measurement
		   	{
		   	    cmd = BLE_CMD_TYPE_READ_MEASUREMENT;
			      uartStat =BLE_CMD_STAT_PRE1;
		   	}				
			  else if(c =='S')//set treatment
		   	{		   	
	        	uartStat = BLE_CMD_STAT_SET_TREATMENT;
		   	}
			  else if(c =='A')//start treatment
		   	{				
		    	  cmd = BLE_CMD_TYPE_START_TREATMENT;		    
	        	uartStat = BLE_CMD_STAT_PRE1;
		   	}
			  else if(c =='P') //stop treatment
		   	{
		   	    cmd = BLE_CMD_TYPE_STOP_TREATMENT;
			      uartStat =BLE_CMD_STAT_PRE1;
		   	}
			  else if(c =='C')//check treatment
		   	{  
			    cmd = BLE_CMD_TYPE_CHECK_TREATMENT;
				  uartStat =BLE_CMD_STAT_PRE1;				 
		   	}
			  else if(c =='E') //set system
		   	{		   	    
			    uartStat =BLE_CMD_STAT_SET_SYSTEM;				 
		   	}
			  else if(c=='D')
			  {
			    uartStat =BLE_CMD_STAT_SELFTEST;				 
		   	}
			  else if(c==0xff)
					;
			  else 
				  uartStat =BLE_CMD_STAT_PRE1;
	      break; //end of cmd

	    case BLE_CMD_STAT_SET_TREATMENT:
        if(i<sizeof(TLC_TREATMENT_SETTING)*2)
		    {        
			   	  mByte[i%2] = c;
					  if(i%2 == 1)
					  {
						  iRead = bleChar2Hex(mByte);
					    if(iRead !=-1)		
		       		       tBuf[j++] =(byte)iRead;
					    else
					    {					  	
					      result =1;	
						    i =  sizeof(TLC_TREATMENT_SETTING)*2;
					    }
				    }
					  i++;
		     }
		     else
		     {
		       if(result ==1)
		       {
					    cmd = BLE_CMD_TYPE_FAILED;
		       }
			     else
			   	 {
				     memcpy( (void *)(&tlcTreatmentSetting),(void *) tBuf, sizeof(TLC_TREATMENT_SETTING));
		    	   cmd = BLE_CMD_TYPE_SET_TREATMENT;
			   	 }
				   uartStat =BLE_CMD_STAT_PRE1;
		   	 }			   
	       break;
	    case BLE_CMD_STAT_SET_SYSTEM:
		    if(i<sizeof(TLC_SYSTEM_SETTING)*2)
		    {        
			    mByte[i%2] = c;
					if(i%2 == 1)
					{
						iRead = bleChar2Hex(mByte);
					  if(iRead !=-1)		
		       	   tBuf[j++] =(byte)iRead;
					  else
					  {					  	
					      result =1;	
						  i =  sizeof(TLC_SYSTEM_SETTING)*2;
					  }
				  }
					i++;
		     }
		     else
		     {
		       if(result ==1)
		       {
					   cmd = BLE_CMD_TYPE_FAILED;
		       }
			     else
			   	 {
				     memcpy( (void *)(&tlcSystem),(void *) tBuf, sizeof(TLC_SYSTEM_SETTING));
		    	   cmd = BLE_CMD_TYPE_SET_SYSTEM;
			   	 }
				   uartStat =BLE_CMD_STAT_PRE1;
		   	 }		
	       break;
	    case BLE_CMD_STAT_START_TREATMENT:
	       break;
	    case BLE_CMD_STAT_STOP_TREATMENT:
	       break;
	    case BLE_CMD_STAT_CHECK_TREATMENT:
	       break;
	    case BLE_CMD_STAT_SET_MEASUREMENT:
		     if(i<sizeof(TLC_MEASUREMENT_SETTING)*2)
		     {        		   			 
			     mByte[i%2] = c;
					 if(i%2 == 1)
					 {
						 iRead = bleChar2Hex(mByte);
					   if(iRead !=-1)		
		       		       tBuf[j++] =(byte)iRead;
					   else
					   {					  	
					      result =1;	
						    i =  sizeof(TLC_MEASUREMENT_SETTING)*2;
					   }
				   }
					 i++;
		     }
		     else
		     {
		       if(result ==1)
		       {
					   cmd = BLE_CMD_TYPE_FAILED;
		       }
			     else
			   	 {
				      memcpy( (void *)(&tlcMeasurementSetting),(void *) tBuf, sizeof(TLC_MEASUREMENT_SETTING));
		    	    cmd = BLE_CMD_TYPE_SET_MEASUREMENT;
			   	 }
				   uartStat =BLE_CMD_STAT_PRE1;
		   	 }			   
	       break;
	    case BLE_CMD_STAT_READ_MEASUREMENT:
	       break;
	    case BLE_CMD_STAT_SELFTEST:		
		     cmd = BLE_CMD_TYPE_START_SELFTEST;
		     uartStat =BLE_CMD_STAT_PRE1;	   
	       break;	
     }
	
	   //if(cmd> BLE_CMD_TYPE_FAILED)
	   //	tlcCmd.valid = 1;
     //tlcCmd.cmd = cmd;
	   return cmd;
}

int uartCommSendPkt(void)
{
	int count;
	for(count = 0; count < STUPID_PKT_LEN; count++)
	{
		if(uartCommPutc( stupidPkt[count]))
			break;
	}
	return(count);
}

int uartTlcFillReport(void)
{       
#if 0
         tlcReport.mode = BLE_RPT_MODE_MEASUREMENT_READY;
		 tlcReport.bf0 = 0x1122;
		 tlcReport.mw0 = 4.1;
#else
		// (void)tlcReadReport();
#endif
    return 0;                                        //2016.07.12
}
//volume will be 0-10  in scale 
int tlcSetBuzzer(short volume)
{
	   
   userProfile.user_profile.buzzerVol=(uint16_t)volume *VOLUME_VOLT_5V/10 ;
	 return 0;                                       //2016.07.12
}


int tlcSetColor(int colorNo)
{
	 //if(colorNo == userProfile.user_profile.foreColor)                       // Added by Jason Chen, 2015.04.30
	// userProfile.user_profile.foreColor = COLOR_SILVER;                    // Added by Jason Chen, 2015.04.30
	// else                                                                    // Added by Jason Chen, 2015.04.30 
	   userProfile.user_profile.foreColor =(uint16_t)colorNo;                // Added by Jason Chen, 2015.04.30 
#if ENABLE_FLASH_PROFILE	
   if(myFlashWrite((uint8_t*)&userProfileFlash,(uint8_t*)&userProfile, sizeof(USER_PROFILE_T)) != PASSED)    // Changed by Jason Chen, 2015.03.24
#else		 
   if(myFlashWrite((uint8_t*)FLASH_STORE_BASE_ADDRESS,(uint8_t*)&userProfile, sizeof(USER_PROFILE_T)) != PASSED)				
#endif		 
	 {				
	 }	
 //sLCD_background_TFT(ColorArray[userProfile.user_profile.backColor], ColorArray[userProfile.user_profile.foreColor]);
	 sLCD_background_TFT(ColorArray[COLOR_BLACK], ColorArray[COLOR_SILVER]);   // Added by Jason Chen for color probe, 20160627
	 probe_color = userProfile.user_profile.foreColor;                         // Added by Jason Chen for probe, 2016_0627
	 return 0;                                               //2016.07.12
}

int tlcSetMode(int mode)
{
	
	
	userProfile.user_profile.mode = mode;
		
	if(userProfile.user_profile.mode == PATIENT_MEASUREMENT_MODE)
		 userProfile.user_profile.treatment_time = 5;

    //if(myFlashWrite((uint8_t*)&userProfileFlash,(uint8_t*)&userProfile, sizeof(USER_PROFILE_T)) != PASSED)    // Changed by Jason Chen, 2015.03.24
	//if(myFlashWrite((uint8_t*)FLASH_STORE_BASE_ADDRESS,(uint8_t*)&userProfile, sizeof(USER_PROFILE_T)) != PASSED)				
	//{				
	//}				
				
	//Mode_GUI_Init(userProfile.user_profile.mode);
	Mode_GUI_Fresh(userProfile.user_profile.mode);
	
  switch_stm_status = SWITCH_WAIT_PRESS;                                           // Added by Jason Chen, 2016.05.18
	treatment_on_flag = 0;                                                           // Added by Jason Chen, 2016.07.22
	
	
	//Screen_Display();
	//MeasurementOn();	
	
	return 0;                                      //2016.07.12
}

extern FEEDBACK_BUFFER_T feedBack_profile;
uint32_t hasMeasurementSet =0;
uint32_t hasSelftestSet =0;

#define SELF_TEST_MAX_CHANNEL_NUMBER 5
int tlcReadReport(void)
{
	int i=0;
	
	tlcReport.mode =  probeMode+1;
	if(feedBack_profile.feed_back_buf.validF)
	{
			i = 0;
			tlcReport.bf0 = feedBack_profile.feed_back_buf.adc_ch_reading[0];
		    tlcReport.bf1 = feedBack_profile.feed_back_buf.adc_ch_reading[2];
		    tlcReport.bf2 = feedBack_profile.feed_back_buf.adc_ch_reading[3];
		    tlcReport.bf3 = feedBack_profile.feed_back_buf.adc_ch_reading[1];
			i = 0;
			tlcReport.mw0 =feedBack_profile.feed_back_buf.adc_ch_reading_mw[0];
			tlcReport.mw1 =feedBack_profile.feed_back_buf.adc_ch_reading_mw[2];
			tlcReport.mw2 =feedBack_profile.feed_back_buf.adc_ch_reading_mw[3];
			tlcReport.mw3 =feedBack_profile.feed_back_buf.adc_ch_reading_mw[1];
	
	}
	else
	{
		i = 0;
		tlcReport.bf0 = 0;
		tlcReport.bf1 = 0;
		tlcReport.bf2 = 0;
		tlcReport.bf3 = 0;
		i = 0;
		tlcReport.mw0 = 0;
		tlcReport.mw1 = 0;
		tlcReport.mw2 = 0;
		tlcReport.mw3 = 0;
       

	}
   
	tlcReport.battery = 50;
	tlcReport.buzzer = 10 * userProfile.user_profile.buzzerVol/VOLUME_VOLT_MAX ;

	tlcReport.wireless = 50;
	tlcReport.color = userProfile.user_profile.foreColor;

	tlcReport.selfTest = 0;
	#if 1
	//for (i =0; i<SELF_TEST_MAX_CHANNEL_NUMBER;i++)
	//	selfTestBuf.selfTest_buf.validF[i] =0;
	#endif
	for(i =0;i <SELF_TEST_MAX_CHANNEL_NUMBER;i++)
	{ 
	   
       if(selfTestBuf.selfTest_buf.validF[i]!=0)
	   	  break;
	    
	}
	tlcReport.selfTest = ( i>=SELF_TEST_MAX_CHANNEL_NUMBER ) ? 1 :0 ;   
	for(i =0; i< SELF_TEST_MAX_CHANNEL_NUMBER; i++)	
	    tlcReport.adc_ch_reading[i] = selfTestBuf.selfTest_buf.adc_ch_reading[i];
	
	if(!hasMeasurementSet  && !hasSelftestSet)
		tlcReport.mode = BLE_RPT_MODE_MEASUREMENT_ERR_1+1;

	
	return 0;                                             //2016.07.12
}

int  tlcConfigMeasurement(void)
{
   PROBE_Idle();                                                              // Added by Jason Chen, 2016.07.22
 //only set laser 905 no. 4 the resest set to zero
   userProfile.user_profile.p905DAC4Voltage = tlcMeasurementSetting.dec905;
	 userProfile.user_profile.p905DAC0Voltage = 0;
	 userProfile.user_profile.p905DAC1Voltage = 0;
	 userProfile.user_profile.p905DAC2Voltage = 0;
	 userProfile.user_profile.p905DAC3Voltage = 0;
	 userProfile.user_profile.p905DAC5Voltage = 0;

	 userProfile.user_profile.p905DutyFreq= tlcMeasurementSetting.fpga905;
	 userProfile.user_profile.p905CadenceDutyFreq= tlcMeasurementSetting.fpga905Cadence;

	 
	 tlcSetMode(PATIENT_MEASUREMENT_MODE);
	 hasMeasurementSet = 1;
	 return 0;                                         //2016.07.12
}

int tlcStopTreatment(void)
{
	 
	
	PROBE_Idle();
	//treatment_time = 0;
	
	return 0;                                    //2016.07.12
}


int tlcStartTreatment(void)
{
	 
	Display_Armed();  
			  
	//treatment_time = treatment_time_backup;//userProfile.user_profile.treatment_time;
	
	switch_stm_status = SWITCH_RUNNING;
	 
	//TreatmentOn();
	 
	switch_stm_status = SWITCH_STOP;    
    return 0;
}
#define TLC_905_D
int tlcConfigTreatment(void)
{
    PROBE_Idle();                                                              // Added by Jason Chen, 2016.07.22
	  userProfile.user_profile.p905DAC0Voltage = tlcTreatmentSetting.dec905[0];
	  userProfile.user_profile.p905DAC1Voltage = tlcTreatmentSetting.dec905[1];
	  userProfile.user_profile.p905DAC2Voltage = tlcTreatmentSetting.dec905[2];
	  userProfile.user_profile.p905DAC3Voltage = tlcTreatmentSetting.dec905[3];
	  userProfile.user_profile.p905DAC4Voltage = tlcTreatmentSetting.dec905[4];
	  userProfile.user_profile.p905DAC5Voltage = tlcTreatmentSetting.dec905[5];
	  

	  userProfile.user_profile.p660DAC0Voltage = tlcTreatmentSetting.dec660[0];
	  userProfile.user_profile.p660DAC1Voltage = tlcTreatmentSetting.dec660[1];
	  userProfile.user_profile.p660DAC2Voltage = tlcTreatmentSetting.dec660[2];
	  userProfile.user_profile.p660DAC3Voltage = tlcTreatmentSetting.dec660[3];

	  userProfile.user_profile.p905DutyFreq= tlcTreatmentSetting.fpga905;
	  userProfile.user_profile.p905CadenceDutyFreq= tlcTreatmentSetting.fpga905Cadence;

	  userProfile.user_profile.p660DutyFreq= tlcTreatmentSetting.fpga660;
	  userProfile.user_profile.p660CadenceDutyFreq= tlcTreatmentSetting.fpga660Cadence;

	  userProfile.user_profile.treatment_time = tlcTreatmentSetting.duration;
    userProfile.user_profile.powerValue_mW = tlcTreatmentSetting.mW1;
			
		userProfile.user_profile.probeTemperature = 0;                                         // Aded by Jason Chen, 2015.04.09
	  //userProfile.user_profile.tensDec = tlcTreatMentSetting.tensDec
	  //userProfile.user_profile.tensPwm = tlcTreatMentSetting.tensPwm
	  //userProfile.user_profile.mW1 = tlcTreatMentSetting.mW1

	  //Display_Armed();  
			  
	  //treatment_time = treatment_time_backup;//userProfile.user_profile.treatment_time;
	
	  //switch_stm_status = SWITCH_RUNNING;
	 
	  //TreatmentOn();
	 
	  //switch_stm_status = SWITCH_STOP; 
	   tlcSetMode(PATIENT_TREATMENT_MODE);
	  return 0;
			 	
}

byte *ptr;
/*__IO*/ uint32_t buzzerTimer = 0;                              //2016.07.12
__IO uint32_t BleTimer = 0;


UART_PROTO_PKT uartCommPkt;

// 0 --0 
// 100 4095    4095*/100 *
void BT_Task(void)
{
	  static int buzzerMode =0;
//  static int bleTimerStartFlag =0;                         //2016.07.12
	  __IO uint16_t voltage;
//  float voltage1;                                          //2016.07.12
	  byte outStr[100];
//  uint16_t timeSeconds = 0;                                //2016.07.12
	  volatile BLE_CMD_TYPE cmd =   BLE_CMD_TYPE_NONE;
//	  uint16_t start = 0;                                    //2016.07.12
//	  int i;                                                 //2016.07.12
      cmd = getCmd(stupidRx );
	  if (cmd != BLE_CMD_TYPE_NONE)
	  {
		 buzzerMode = 1;
//	 bleTimerStartFlag = 0;                                  //2016.07.12
	  }
#ifdef ENABLE_BLE_RESET
	  else if(!bleTimerStartFlag)
	  {
			
			Process_TimeSet(&BleTimer, 1000*60*2);

	  }
	  else
	  {
		    if(Process_TimeExpired(&BleTimer))
	        {
	        	uartCommPuts("ATRST\r\n\r\n");
	        }

	  }
#endif
    if(buzzerMode == 1)
    {
	  		Process_TimeSet(&buzzerTimer, 300);
			  BuzzerOn();
			  buzzerMode++;
    }
	  else if(buzzerMode ==2) 
	  {
	  		
	      if(Process_TimeExpired(&buzzerTimer))
	      {
		  		BuzzerOff();
			  	buzzerMode++;
	      }
	  }
	  switch(cmd)
		{
		
			case BLE_CMD_TYPE_SET_MEASUREMENT:
			   uartCommPuts("TLSM\r\n\r\n");
			   tlcConfigMeasurement();
				 
			break;
			case BLE_CMD_TYPE_READ_MEASUREMENT:
				   tlcReadReport();				  
				   uartConstructPkt( (byte*)(&tlcReport), sizeof(tlcReport));					  
				   sprintf((char*)outStr,  "TLRM %X %X %X %X %X %X %X %X %X %f %f %f %f %X %X %X %X %X %X \r\n\r\n",
						              tlcReport.mode, 
					                tlcReport.battery,
					                tlcReport.buzzer,
					 	              tlcReport.wireless,
					                tlcReport.color,
					                tlcReport.bf0, tlcReport.bf1, tlcReport.bf2, tlcReport.bf3,
					 	              tlcReport.mw0,tlcReport.mw1,tlcReport.mw2,tlcReport.mw3,
					                tlcReport.selfTest, 
					                tlcReport.adc_ch_reading[0], tlcReport.adc_ch_reading[1], tlcReport.adc_ch_reading[2],tlcReport.adc_ch_reading[3],tlcReport.adc_ch_reading[4]
					        );
				   uartCommPuts((char*)outStr);          //2016.07.12				  
			     break;
			case BLE_CMD_TYPE_SET_TREATMENT:
				   uartCommPuts("TLST\r\n\r\n");
				   (void)tlcConfigTreatment();
				 
                	
			     
			break;
			case  BLE_CMD_TYPE_START_TREATMENT:
				 
			    uartCommPuts("TLSTA\r\n\r\n");
				(void)tlcStartTreatment();
			     
			break;
			case  BLE_CMD_TYPE_STOP_TREATMENT:
				  uartCommPuts("TLSTO\r\n\r\n");
				  (void)tlcStopTreatment();
			    
			break;
			case  BLE_CMD_TYPE_CHECK_TREATMENT:
				 uartCommPuts("TLCT\r\n\r\n");
		    break;
			case BLE_CMD_TYPE_SET_SYSTEM:
				uartCommPuts("TLSS\r\n\r\n");
				if(tlcSystem.buzzer <= 10)
				{ 
					userProfile.user_profile.buzzerVol = tlcSystem.buzzer*VOLUME_VOLT_MAX/10 ;
					Buzzer_Volume(userProfile.user_profile.buzzerVol);
				}
				if(tlcSystem.color <= COLOR_ORANGE)
				{                                                                            // Added by Jason Chen, 2015.04.30
				   //if(tlcSystem.color == userProfile.user_profile.foreColor)               // Added by Jason Chen, 2015.04.30
		       //  userProfile.user_profile.foreColor = COLOR_SILVER;                    // Added by Jason Chen, 2015.04.30
	         //else                                                                    // Added by Jason Chen, 2015.04.30 
		         userProfile.user_profile.foreColor = tlcSystem.color;                  
				   #if ENABLE_FLASH_PROFILE	
              if(myFlashWrite((uint8_t*)&userProfileFlash,(uint8_t*)&userProfile, sizeof(USER_PROFILE_T)) != PASSED)    // Changed by Jason Chen, 2015.03.24
           #else			 
	            if(myFlashWrite((uint8_t*)FLASH_STORE_BASE_ADDRESS,(uint8_t*)&userProfile, sizeof(USER_PROFILE_T)) != PASSED)			 
           #endif	
              {
              }
				}                                                                          // Added by Jason Chen, 2015.04.30
				#if 0
				if(tlcSystem.which == TLC_SYSTEM_WHICH_ITEM_ALL)
				{
					


				}
				else if (tlcSystem.which == TLC_SYSTEM_WHICH_ITEM_COLOR)
				{

				}
				else if (tlcSystem.which == TLC_SYSTEM_WHICH_ITEM_BUZZER)
				{

				}
				
				#endif
				 //uartCommPuts("TLSS\r\n\r\n");
			   break;
			case BLE_CMD_TYPE_FAILED:
				 uartCommPuts("TLSERR\r\n\r\n");
			   break;

			case BLE_CMD_TYPE_START_SELFTEST:
                  
				 tlcSetMode(PROBE_MODE_SELTEST);
				 probeMode = BLE_RPT_MODE_SELFTEST_BUSY;
				 hasSelftestSet =1;
				 uartCommPuts("TLDSW\r\n\r\n");
			break;

			//case BLE_CMD_YTPE_GET_SELFTEST:
			//	 if(selfTestBuf.selfTest_buf.validF[8] ==0x1)
			//	 	{
			//		 uartCommPuts("TLDSW\r\n\r\n");//passed
			//		 break;

			//	 	}
			//	 for(i =0;i <9;i++)
			//	 {
            //       selfTestBuf.selfTest_buf.validF[i]!=0;
			//	   break;
			//	 }
			//	 if (i >9)  uartCommPuts("TLDSP\r\n\r\n");//passed
			//	 else       uartCommPuts("TLDSF\r\n\r\n"); //failed
				 
			//break;
			case BLE_CMD_TYPE_NONE:
			default:
				//let update the wireless signal
				
				break;
			
			
		}
	 

}



#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif
