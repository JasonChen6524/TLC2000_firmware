/**
  ******************************************************************************
  * @file    usb_conf.h
  * @author  Jason Chen
  * @date    21-January-2013
  * @brief   Custom HID demo configuration file
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_CONF_H
#define __USB_CONF_H

/* Includes ------------------------------------------------------------------*/
#include "custom_hid_conf.h"                    //It's me
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* External variables --------------------------------------------------------*/
/*-------------------------------------------------------------*/
/* EP_NUM */
/* defines how many endpoints are used by the device */
/*-------------------------------------------------------------*/
#ifdef PACKET_MAX_65
  #define EP_NUM     (3)
#else
  #define EP_NUM     (2)
#endif

/*-------------------------------------------------------------*/
/* --------------   Buffer Description Table  -----------------*/
/*-------------------------------------------------------------*/
/* buffer table base address */
/* buffer table base address */
#define BTABLE_ADDRESS      (0x00)

/* EP0  */
/* rx/tx buffer base address */
#define ENDP0_RXADDR        (0x18)
#define ENDP0_TXADDR        (0x58)

#ifdef PACKET_MAX_65
		/* EP1  */
		/* tx buffer base address */
		#define ENDP1_TXADDR        (0x100)

		/* EP2  */
		/* tx buffer base address */
		#define ENDP2_RXADDR        (0x150)
#else
		/* EP1  */
		/* tx buffer base address */
		#define ENDP1_TXADDR        (0x100)
		#define ENDP1_RXADDR        (0x150)//(0x104)
#endif

/*-------------------------------------------------------------*/
/* -------------------   ISTR events  -------------------------*/
/*-------------------------------------------------------------*/
/* IMR_MSK */
/* mask defining which events has to be handled */
/* by the device application software */

#define IMR_MSK (CNTR_CTRM  | CNTR_WKUPM | CNTR_SUSPM | CNTR_ERRM  | CNTR_SOFM \
                 | CNTR_ESOFM | CNTR_RESETM )

/* CTR service routines */
/* associated to defined endpoints */
/* #define  EP1_IN_Callback   NOP_Process */
#define  EP2_IN_Callback   NOP_Process
#define  EP3_IN_Callback   NOP_Process
#define  EP4_IN_Callback   NOP_Process
#define  EP5_IN_Callback   NOP_Process
#define  EP6_IN_Callback   NOP_Process
#define  EP7_IN_Callback   NOP_Process

#ifdef PACKET_MAX_65
    #define  EP1_OUT_Callback   NOP_Process
	  //#define  EP2_OUT_Callback   NOP_Process                      // Cutomized 
#else
   //#define  EP1_OUT_Callback   NOP_Process
	   #define  EP2_OUT_Callback   NOP_Process   
#endif

#define  EP3_OUT_Callback   NOP_Process
#define  EP4_OUT_Callback   NOP_Process
#define  EP5_OUT_Callback   NOP_Process
#define  EP6_OUT_Callback   NOP_Process
#define  EP7_OUT_Callback   NOP_Process

#endif /*__USB_CONF_H*/

/****END OF FILE****/

