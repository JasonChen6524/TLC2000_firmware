/***************************************************************************
 *
 *
 ***************************************************************************/

#ifndef _FIRMWAREUPDATE_H_
#define _FIRMWAREUPDATE_H_


#include "stm32f10x.h"

#define FIRMWAREUPDATE_SIGNATURE        0x4646
#define BIT(x) ( 1<<(x) )


typedef struct
{
  uint16_t block_len;
	uint8_t  blockFlag;
	uint8_t* blockBuf;
  uint32_t block_base_address;
}CONTROL_BLOCK;



#define CONTROL_BLOCK_BASE_ADDRESS   ((uint32_t)0x08032000)
#define FIRMWARE_STORE_BASE_ADDRESS  ((uint32_t)0x08032800)

void Firmware_init(void);
uint8_t myFlashWrite_1(uint8_t *addr, uint8_t *src, uint16_t len);
uint8_t FirmwareReceive(uint8_t *recvBuf);
#endif


