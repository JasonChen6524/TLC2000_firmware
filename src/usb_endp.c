/**
  ******************************************************************************
  * @file    usb_endp.c
  * @author  Jason Chen
  * @date    21-January-2013
  * @brief   Endpoint routines
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/

#include "hw_config.h"
#include "usb_lib.h"
#include "usb_istr.h"


/* Private typedef -----------------------------------------------------------*/
#include "custom_hid_conf.h"

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t Receive_Buffer[MAX_TRANSFER_LEN];
extern __IO uint8_t PrevXferComplete;
extern __IO uint8_t sendFlag;
extern uint8_t recvChar;
extern uint8_t recvCharFlag;
extern uint8_t CMD_ID;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : EP1_OUT_Callback.
* Description    : EP1 OUT Callback Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
//extern void LED_Toggle(void);
#ifdef PACKET_MAX_65
  void EP2_OUT_Callback(void)
#else
  void EP1_OUT_Callback(void)
#endif	
{

  /* Read received data (2 bytes) */  
#ifdef PACKET_MAX_65
	USB_SIL_Read(EP2_OUT, Receive_Buffer);
#else	
  USB_SIL_Read(EP1_OUT, Receive_Buffer);
#endif
	
	
	CMD_ID = Receive_Buffer[1];
	
	//LED_Toggle();			
	
	
	#ifdef PACKET_MAX_65
	SetEPRxStatus(ENDP2, EP_RX_VALID);
	#else
  SetEPRxStatus(ENDP1, EP_RX_VALID);
	#endif
 
}

/*******************************************************************************
* Function Name  : EP1_IN_Callback.
* Description    : EP1 IN Callback Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_IN_Callback(void)
{
  PrevXferComplete = 1;
}
/*****END OF FILE****/

