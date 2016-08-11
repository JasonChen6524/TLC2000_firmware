/**
  ******************************************************************************
  * @file    usb_type.h
  * @author  Jason Chen
  * @date    28-August-2012
  * @brief   Type definitions used by the USB Library
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_TYPE_H
#define __USB_TYPE_H

/* Includes ------------------------------------------------------------------*/
#include "usb_conf.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#ifndef NULL
#define NULL ((void *)0)
#endif

typedef enum
{
  FALSE = 0, TRUE  = !FALSE
}
bool;

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* External variables --------------------------------------------------------*/

#endif /* __USB_TYPE_H */

/*****END OF FILE****/
