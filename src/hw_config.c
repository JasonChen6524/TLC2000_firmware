/**
  ******************************************************************************
  * @file    hw_config.c
  * @date    21-January-2013
  * @brief   Hardware Configuration & Setup
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_pwr.h"

#include "TLC2000_SPI_LCD_TFT.h"
#include "calibration.h"
#include "MiscTools.h"
#include "UTFT.h"
#include "tBT.h"
#include "RTC.h"
#include "Potentiometer.h"
#include "FPGA_RAM.h"
#include "DAC7718.h"
#include "CPU_ADC.h"
#include "BatSensor.h"
#include "Buzzer.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t TimingDending = 0;
__IO uint32_t LsiFreq       = 40000;

ErrorStatus HSEStartUpStatus;
//uint32_t ADC_ConvertedValueX = 0;
//uint32_t ADC_ConvertedValueX_1 = 0;
//__IO uint16_t  ADC1ConvertedValue = 0, ADC1ConvertedVoltage = 0, calibration_value = 0;

/* Extern variables ----------------------------------------------------------*/
extern uint8_t usbMode;
/* Private function prototypes -----------------------------------------------*/
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len);
/* Private functions ---------------------------------------------------------*/
extern void Screen_Display(void);

/*******************************************************************************
* Function Name  : Set_System
* Description    : Configures Main system clocks & power.
* Input          : None.
* Return         : None.
*******************************************************************************/
void Set_System(void)
{  
  /*!< At this stage the microcontroller clock setting is already configured, 
  this is done through SystemInit() function which is called from startup
  file (startup_stm32xxx.s) before to branch to application main.
  To reconfigure the default setting of SystemInit() function, refer to
  system_stm32xxx.c file
  */ 
   
  /* Enable USB_DISCONNECT GPIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_DISCONNECT, ENABLE);
	
  /* ADCCLK = PCLK2/8 */
  //RCC_ADCCLKConfig(RCC_PCLK2_Div8);    // don't need ADC
  
  /* Configure the used GPIOs*/
  GPIO_Configuration();

#ifdef EVAL_BOARD	
  /* Configure the KEY button in EXTI mode */
  STM_EVAL_PBInit(Button_KEY, Mode_EXTI);

  /* Configure the Tamper button in EXTI mode */
  STM_EVAL_PBInit(Button_TAMPER, Mode_EXTI);
#endif	

  /* Additional EXTI configuration (configure both edges) */
  EXTI_Configuration();
  
#ifdef EVAL_BOARD
  /* Configure the LEDs */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2);
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
	
  STM_EVAL_LEDInit(LED5);
  STM_EVAL_LEDInit(LED6);
	
  STM_EVAL_LEDInit(LED7);
  STM_EVAL_LEDInit(LED8);
  
  /* Configure the ADC*/
  ADC_Configuration();
#endif	
}

/*******************************************************************************
* Function Name  : Set_USBClock
* Description    : Configures USB Clock input (48MHz).
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Set_USBClock(void)
{
  /* Select USBCLK source */
  RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
  
  /* Enable the USB clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
}

/*******************************************************************************
* Function Name  : Enter_LowPowerMode.
* Description    : Power-off system clocks and power while entering suspend mode.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Enter_LowPowerMode(void)
{
  /* Set the device state to suspend */
  bDeviceState = SUSPENDED;
}

void MySystemInit (void)
{
	__IO uint32_t StartUpCounter = 0, HSEStatus = 0;
	
  /* Reset the RCC clock configuration to the default reset state(for debug purpose) */
  /* Set HSION bit */
  RCC->CR |= (uint32_t)0x00000001;

  /* Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits */

  RCC->CFGR &= (uint32_t)0xF8FF0000;
  
  /* Reset HSEON, CSSON and PLLON bits */
  RCC->CR &= (uint32_t)0xFEF6FFFF;

  /* Reset HSEBYP bit */
  RCC->CR &= (uint32_t)0xFFFBFFFF;

  /* Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits */
  RCC->CFGR &= (uint32_t)0xFF80FFFF;

  /* Disable all interrupts and clear pending bits  */
  RCC->CIR = 0x009F0000;
    
  /* Configure the System clock frequency, HCLK, PCLK2 and PCLK1 prescalers */
  /* Configure the Flash Latency cycles and enable prefetch buffer */
  
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/    
  /* Enable HSE */    
  RCC->CR |= ((uint32_t)RCC_CR_HSEON);
 
  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CR & RCC_CR_HSERDY;
    StartUpCounter++;  
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));		

  if ((RCC->CR & RCC_CR_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }  

  if (HSEStatus == (uint32_t)0x01)
  {
    /* Enable Prefetch Buffer */
    FLASH->ACR |= FLASH_ACR_PRFTBE;

    /* Flash 2 wait state */
    FLASH->ACR &= (uint32_t)((uint32_t)~FLASH_ACR_LATENCY);
    FLASH->ACR |= (uint32_t)FLASH_ACR_LATENCY_2;    

 
    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;    //72MHz
      
    /* PCLK2 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;   //72MHz
    
    /* PCLK1 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;   //36MHz

    /*  PLL configuration: PLLCLK = HSE * 9 = 72 MHz */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE |
                                        RCC_CFGR_PLLMULL));
    RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMULL9);

    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
    }
    
    /* Select PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
    {
    }
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock 
         configuration. User can add here some code to deal with this error */
  }

  //SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH. */
	//NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x3000);
}
/*******************************************************************************
* Function Name  : Leave_LowPowerMode.
* Description    : Restores system clocks and power while exiting suspend mode.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Leave_LowPowerMode(void)
{
  DEVICE_INFO *pInfo = &Device_Info;
  
  /* Set the device state to the correct state */
  if (pInfo->Current_Configuration != 0)
  {
    /* Device configured */
    bDeviceState = CONFIGURED;
  }
  else 
  {
    bDeviceState = ATTACHED;
  }
  /*Enable SystemCoreClock*/
  MySystemInit();
}

/*******************************************************************************
* Function Name  : USB_Interrupts_Config.
* Description    : Configures the USB interrupts.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void USB_Interrupts_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure; 
	
  
  /* 2 bit for pre-emption priority, 2 bits for subpriority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  
  

  /* Enable the USB interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;//1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the USB Wake-up interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = USBWakeUp_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
}

void Custom_Interrupts_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure; 
    
  /* Enable the EXTI9_5 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the EXTI15_10 Interrupt */
 // NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
 // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
 // NVIC_Init(&NVIC_InitStructure);
  
  /* Enable the DMA1 Channel1 Interrupt */
  //NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
  //NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
  //NVIC_Init(&NVIC_InitStructure);
  
}
/*******************************************************************************
* Function Name  : USB_Cable_Config.
* Description    : Software Connection/Disconnection of USB Cable.
* Input          : NewState: new state.
* Output         : None.
* Return         : None
*******************************************************************************/
void USB_Cable_Config (FunctionalState NewState)
{ 
 /* USE_STM3210B_EVAL or USE_STM3210E_EVAL */
  if (NewState != DISABLE)
  {
    GPIO_ResetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
  }
  else
  {
    GPIO_SetBits(USB_DISCONNECT, USB_DISCONNECT_PIN);
  }
}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_DISCONNECT /*| 
                         RCC_APB2Periph_GPIO_IOAIN*/ , ENABLE);  
  
  /* USB_DISCONNECT used as USB pull-up */
  GPIO_InitStructure.GPIO_Pin = USB_DISCONNECT_PIN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(USB_DISCONNECT, &GPIO_InitStructure);
	  
  /* Configure Potentiometer IO as analog input */
  //GPIO_InitStructure.GPIO_Pin = GPIO_IOAIN_PIN;
  //GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  //GPIO_Init(GPIO_IOAIN, &GPIO_InitStructure);	
}

/*******************************************************************************
* Function Name : EXTI_Configuration.
* Description   : Configure the EXTI lines for Key and Tamper push buttons.
* Input         : None.
* Output        : None.
* Return value  : The direction value.
*******************************************************************************/
void EXTI_Configuration(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;
  
#ifdef EVAL_BOARD	
  /* Configure Key EXTI line to generate an interrupt on rising & falling edges */  
  EXTI_InitStructure.EXTI_Line = KEY_BUTTON_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* Clear the Key EXTI line pending bit */
  EXTI_ClearITPendingBit(KEY_BUTTON_EXTI_LINE);
  
  /* Configure Tamper EXTI Line to generate an interrupt rising & falling edges */  
  EXTI_InitStructure.EXTI_Line = TAMPER_BUTTON_EXTI_LINE;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* Clear the Tamper EXTI line pending bit */
  EXTI_ClearITPendingBit(TAMPER_BUTTON_EXTI_LINE);
#endif

  
  /* Configure the EXTI line 18 connected internally to the USB IP */
  EXTI_ClearITPendingBit(EXTI_Line18);
  EXTI_InitStructure.EXTI_Line = EXTI_Line18; 
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
}

/*******************************************************************************
* Function Name  : Get_SerialNum.
* Description    : Create the serial number string descriptor.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Get_SerialNum(void)
{
  uint32_t Device_Serial0, Device_Serial1, Device_Serial2;
  
  Device_Serial0 = *(uint32_t*)ID1;
  Device_Serial1 = *(uint32_t*)ID2;
  Device_Serial2 = *(uint32_t*)ID3;
  
  Device_Serial0 += Device_Serial2;
  
  if (Device_Serial0 != 0)
  {
    IntToUnicode (Device_Serial0, &CustomHID_StringSerial[2] , 8);
    IntToUnicode (Device_Serial1, &CustomHID_StringSerial[18], 4);
  }
}

/*******************************************************************************
* Function Name  : HexToChar.
* Description    : Convert Hex 32Bits value into char.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len)
{
  uint8_t idx = 0;
  
  for( idx = 0 ; idx < len ; idx ++)
  {
    if( ((value >> 28)) < 0xA )
    {
      pbuf[ 2* idx] = (value >> 28) + '0';
    }
    else
    {
      pbuf[2* idx] = (value >> 28) + 'A' - 10; 
    }
    
    value = value << 4;
    
    pbuf[ 2* idx + 1] = 0;
  }
}

__IO uint32_t tempp = 10;
#define TIME_DELAY  4500
void MyDelay(__IO uint32_t nCount)
{
	tempp = TIME_DELAY;
  while(nCount--)
	{
		while(tempp--)
		{

		}
		tempp = TIME_DELAY;
	}
}

void system_Init(void)
{
	MyPowerOff();

	SysTick_Config(SystemCoreClock/1000);      /* Generate interrupt each 1 ms  */

  Set_System();	
	
  MyDelay(250);
//USB_Cable_Config (ENABLE);
//USB_Interrupts_Config();     // Moved into USB_Init()
	
	if(VUSB_IN_PBGetState())
	{
		usbMode = 1;
		Set_USBClock();
		USB_Init();
		
		TimingDending = 0;
		for(;;)
		{
			 if(bDeviceState ==  CONFIGURED)
				 break;
			 else
			 {
				 if(TimingDending > 5000)
				 {				 
					 PowerOff();
					 RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, DISABLE);
					 break;
				 }
				 
			 }		 
		}
  }
	else
		usbMode = 0;

#ifdef ENABLE_iWATCHDOG                          // Changed from "#if ..." to "#ifdef ..." by Jason Chen, 2015.04.30
  /* IWDG timeout equal to 250 ms (the timeout may varies due to LSI frequency
     dispersion) */
  /* Enable write access to IWDG_PR and IWDG_RLR registers */
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

  /* IWDG counter clock: LSI/32 */
  IWDG_SetPrescaler(IWDG_Prescaler_32);   // 40000/32 = 1250 Hz

  /* Set counter reload value to obtain 250ms IWDG TimeOut.
     Counter Reload Value = 250ms/IWDG counter clock period
                          = 250ms / (LSI/32)
                          = 0.25s / (LsiFreq/32)
                          = LsiFreq/(32 * 4)
                          = LsiFreq/128
   */
  IWDG_SetReload(LsiFreq/128); // 40000*128/125081 = 0.025s = 250 ms

  /* Reload IWDG counter */
  IWDG_ReloadCounter();

  /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  IWDG_Enable();  
#endif

	Custom_Interrupts_Config();	
	Delay(100);
	sLCD_SPI_Init_TFT();
	Delay(100);
	
	//sLCD_init_TFT();
	InitLCD();
	
	Delay(50);
	
	LED_and_IO_Init();
	
	TLC_UartInit();	
	LED_On();
	
	DrawTheralaseLogo();
    Delay(500);
	//#define BT_LOCATION  (12*13-1)
	//Display_BT_Bar(BT_LOCATION+24, 300, 65535/2000);
	//Display_BT_Bar(239-32, 276, 65535/2000);
	//Display_BT_Bar(BT_LOCATION+24, 300, 4);
	//Display_BT_Bar(BT_LOCATION+24, 300, 3);
	
#if 0		
	sLCD_background_TFT(COLOR_BLACK1,COLOR_WHITE1);                // Set as white
	Delay(10);
	sLCD_background_TFT(COLOR_BLUE1, COLOR_BLACK1);                // Set as Blue
	Delay(10);
	sLCD_background_TFT(COLOR_DARKGREEN1, COLOR_BLACK1);           // Set as Green
	Delay(10);
	sLCD_background_TFT(COLOR_RED1, COLOR_DARKGREEN1);             // Set as Red
	Delay(10);
	sLCD_background_TFT(COLOR_DEEPRED1, COLOR_DARKGREEN1);
	Delay(10);

  background_color = 0xFFEA-0x10;
	foreground_color = 0x0010+0x10;
  while(background_color)
	{
		sLCD_background_TFT(background_color, foreground_color);  
		sLCD_putString_TFT(0,272, (uint8_t*)"BackGD ForegGD",Font16x24);		
		sprintf((char*)displayBuff,"%4x", background_color);
		sLCD_putString_TFT(0,248, displayBuff,Font16x24);		
		sprintf((char*)displayBuff,"%4x", foreground_color);
		sLCD_putString_TFT(120,248, displayBuff,Font16x24);		
		background_color -=0x40;
		foreground_color +=0x40;
		Delay(10);
	}
#endif	
	userProfileInit();		
	probeOffsetInit();                                         // Added by Jason Chen, 2015.09.13	
	
	//Delay(7000);//13000);	
	//Delay(6500);	
	Delay(500);
//sLCD_background_TFT(ColorArray[userProfile.user_profile.backColor], ColorArray[userProfile.user_profile.foreColor]);
	sLCD_background_TFT(ColorArray[COLOR_BLACK], ColorArray[COLOR_SILVER]);     // Added by Jason Chen, 2015_0627
	probe_color = userProfile.user_profile.foreColor;                           // Added by Jason Chen for probe, 2016_0627
	//Delay(50);	
	//Display_Idle();
	//sLCD_putString_TFT(5,92+2*24,(uint8_t*)"Treatment",Font16x24);
	//sLCD_putString_TFT(5,92+24,(uint8_t*)"Time Left:",Font16x24);
	
	treatment_time = -1;//userProfile.user_profile.treatment_time;
	treatment_time_backup = userProfile.user_profile.treatment_time;
	//DisplayTreatmentTime(userProfile.user_profile.treatment_time);
	
	//if(userProfile.user_profile.treatment_time > 255)  
	//	pcCmd.duration = 255;                                      // Added by Jason, 2014.11.11
	//else
	//	pcCmd.duration = userProfile.user_profile.treatment_time;  // Added by Jason, 2014.11.11
	
	//pcCmd.power = oldPcCmd.power = (uint8_t)((userProfile.user_profile.p905DAC0Voltage * 100)/4095);  // Added by Jason, 2014.11.11
	//pcCmd.tense = oldPcCmd.tense = (uint8_t)((userProfile.user_profile.pTensDACVoltage * 100)/4095);  // Added by Jason, 2014.11.11
	
	//Screen_Treatment();	
	RTC_Init();
	Delay(50);
	
	LED_Off();
	
	Pot_Init();
	//Pot_SetU18(REGA, userProfile.user_profile.PotU18A);
	//Pot_SetU18(REGB, userProfile.user_profile.PotU18B);
	//Pot_SetU14(REGA,  userProfile.user_profile.PotU14A);
	//Pot_SetU14(REGB,  userProfile.user_profile.PotU14B);
	
	TempSensorTCN75Init();

    FPGA_RAM_Init();
	Delay(10);	
	Set_905_pulse(userProfile.user_profile.p905DutyFreq & 0xFF00, userProfile.user_profile.p905DutyFreq & 0x00FF);
	Set_660_pulse(userProfile.user_profile.p660DutyFreq & 0xFF00, userProfile.user_profile.p660DutyFreq & 0x00FF);
	
	//Set_TENS_Frequence(userProfile.user_profile.TensEnable);        // 2014.10.21
	
	Set_905_Feedback_pulse(0);//userProfile.user_profile.p905Feedback_pulse);
	Set_660_Feedback_pulse(0);//userProfile.user_profile.p660Feedback_pulse);

  DAC7718_SPI_GPIO_Init();
  if(userProfile.user_profile.p905DAC0Voltage & 0x8000)
		 ;//ALL_DAC7718_OUT_VOLT(userProfile.user_profile.p905DAC0Voltage & 0x0FFF);
	else
	{
		 DAC7718_OUT_VOLT(s905_DAC0, userProfile.user_profile.p905DAC0Voltage & 0x0FFF);
		 DAC7718_OUT_VOLT(s905_DAC1, userProfile.user_profile.p905DAC1Voltage & 0x0FFF);
		 DAC7718_OUT_VOLT(s905_DAC2, userProfile.user_profile.p905DAC2Voltage & 0x0FFF);
		 DAC7718_OUT_VOLT(s905_DAC3, userProfile.user_profile.p905DAC3Voltage & 0x0FFF);
		 DAC7718_OUT_VOLT(s905_DAC4, userProfile.user_profile.p905DAC4Voltage & 0x0FFF);
		 DAC7718_OUT_VOLT(s905_DAC5, userProfile.user_profile.p905DAC5Voltage & 0x0FFF);
	}
	
	Power_V30_Off();	                                                                    // Added b y Jason Chen, 2016.03.18
	// 2014.11.17
	Set_cadence_freq_duty(CADENCE_FREQ_905,(uint8_t)((userProfile.user_profile.p905CadenceDutyFreq >> 8)&0x00FF),(uint8_t)(userProfile.user_profile.p905CadenceDutyFreq&0x00FF));
	Set_cadence_freq_duty(CADENCE_FREQ_660,(uint8_t)((userProfile.user_profile.p660CadenceDutyFreq >> 8)&0x00FF),(uint8_t)(userProfile.user_profile.p660CadenceDutyFreq&0x00FF));	
		
	if(userProfile.user_profile.TensEnable)
	{
		DAC7718_OUT_VOLT(TENS_DAC, userProfile.user_profile.pTensDACVoltage & 0x0FFF);
		//Set_TENS_Frequence(userProfile.user_profile.TensEnable);       // 2014.10.27
	}
	else
	{
		DAC7718_OUT_VOLT(TENS_DAC, 0);
	}
 
	//????????????ADS8319_Init();
	Cpu_ADC_Init();
		
	Delay(10);
#if 0
    adc_rd1 = ADC_ReadValue();
    if(adc_rd1 >= 0)
	  if ((PrevXferComplete) && (bDeviceState == CONFIGURED))
    {
			adc_rd = (uint16_t)adc_rd1;
      Send_Buffer[0] = REPORT_ID_5; 
			Send_Buffer[1] = (uint8_t)0xED;                // request setup time			
      Send_Buffer[2] = (uint8_t)(adc_rd >> 8);       // request setup time
			Send_Buffer[3] = (uint8_t)adc_rd;              // request setup time
    
      /* Write the descriptor through the endpoint */
      USB_SIL_Write(EP1_IN, (uint8_t*) Send_Buffer, MAX_TRANSFER_LEN);  
			SetEPTxValid(ENDP1);
      PrevXferComplete = 0;  
    }
#endif   
	
  if(userProfile.user_profile.p660DAC0Voltage & 0x8000)
	{
		 //ALL_S660_DAC_OUT_VOLT(userProfile.user_profile.p905DAC0Voltage & 0x0FFF);
  }
	else
	{
		 S660_DAC_OUT_VOLT(s660_DAC0, userProfile.user_profile.p660DAC0Voltage & 0x0FFF);
		 S660_DAC_OUT_VOLT(s660_DAC1, userProfile.user_profile.p660DAC1Voltage & 0x0FFF);
		 S660_DAC_OUT_VOLT(s660_DAC2, userProfile.user_profile.p660DAC2Voltage & 0x0FFF);
		 S660_DAC_OUT_VOLT(s660_DAC3, userProfile.user_profile.p660DAC3Voltage & 0x0FFF);
	}
	Delay(10);
	//ADC_Value_Display_one_time();    // Added by Jason Chen, 2014.12.19
	
	Bat_Init();	
	
	BUZZER_Init();
  Screen_Display();	
	AMP_hold_flag = 0;
	
	VerifyBatSensor();
#if 0		
	DisplayTestOffset(8,probeOffset.probe_offset.p905DAC0VoltOffset);
	DisplayTestOffset(7,probeOffset.probe_offset.p905DAC1VoltOffset);
	DisplayTestOffset(6,probeOffset.probe_offset.p905DAC2VoltOffset);
	DisplayTestOffset(5,probeOffset.probe_offset.p905DAC3VoltOffset);
	
	#if 1
	DisplayTestOffset(4,probeOffset.probe_offset.p905DAC4VoltOffset);
	DisplayTestOffset(3,probeOffset.probe_offset.p660DAC0VoltOffset);
	DisplayTestOffset(2,probeOffset.probe_offset.p660DAC1VoltOffset);
	DisplayTestOffset(1,probeOffset.probe_offset.p660DAC2VoltOffset);
	DisplayTestOffset(0,probeOffset.probe_offset.p660DAC3VoltOffset);
	#else
	DisplayTestOffset(3,probeOffset.probe_offset.feedbackCh0Offset);
	DisplayTestOffset(2,probeOffset.probe_offset.feedbackCh1Offset);
	DisplayTestOffset(1,probeOffset.probe_offset.feedbackCh2Offset);
	DisplayTestOffset(0,probeOffset.probe_offset.feedbackCh3Offset);
	#endif
#elif 0
   DisplayTestOffset(6,sizeof(SELF_TEST_RECORD_T));
	 DisplayTestOffset(5,sizeof(FEEDBACK_BUFFER_T));
#endif	
	
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
