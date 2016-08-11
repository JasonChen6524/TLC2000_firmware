/**
  ******************************************************************************
  * @file    usb_int.h
  * @author  Jason Chen
  * @date    28-August-2012
  * @brief   Endpoint CTR (Low and High) interrupt's service routines prototypes
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_INT_H
#define __USB_INT_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void CTR_LP(void);
void CTR_HP(void);

/* External variables --------------------------------------------------------*/

#endif /* __USB_INT_H */

/*****END OF FILE****/
