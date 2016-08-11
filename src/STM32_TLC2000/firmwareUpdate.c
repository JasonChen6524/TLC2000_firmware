
#include <stdio.h>
#include <string.h>
#include "firmwareUpdate.h"
#include "calibration.h"
//#include "TLC2000_SPI_LCD_TFT.h"

#define FLASH_PAGE_SIZE           ((uint16_t)0x800)
#define FLASH_BLOCK_SIZE          ((uint16_t)(FLASH_PAGE_SIZE*3))

static uint32_t NbrOfPage = 0x00;
static uint32_t EraseCounter = 0x00, Address = 0x00;

static volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
static volatile TestStatus MemoryProgramStatus = PASSED;

static CONTROL_BLOCK firmwareControlBLK[64];
static int block_count =0;;
static uint16_t block_len;	
static uint32_t base_addr_last = 0;
static uint32_t base_addr_curr = 0;
//static uint8_t blockBuf[32];
static uint8_t blockBuffer[FLASH_BLOCK_SIZE];


uint8_t myFlashWrite_1(uint8_t *addr, uint8_t *src, uint16_t len)
{
  uint8_t i;
  uint32_t Address_t;
  USER_PROFILE_T *sourc_data;
//MODE_SETTING_T *flash_data;  
  
  
  FLASH_UnlockBank1();

  /* Define the number of page to be erased */
  NbrOfPage = len / FLASH_PAGE_SIZE + 1;

  /* Clear All pending flags */
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	

  Address_t = (uint32_t)&(*addr);
  /* Erase the FLASH pages */
  for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
  {
    FLASHStatus = FLASH_ErasePage(Address_t + (FLASH_PAGE_SIZE * EraseCounter));
  }
  
  /* Program Flash Bank1 */
  Address = Address_t;
  
  sourc_data = (USER_PROFILE_T *)src;
  i = 0;
  while((Address < Address_t+len) && (FLASHStatus == FLASH_COMPLETE))
  {
    FLASHStatus = FLASH_ProgramWord(Address, sourc_data->flash_word[i]);
    i++;
    Address = Address + 4;
  }

  FLASH_LockBank1();
  
/* Check the correctness of written data */
  Address = Address_t;
  i = 0;
//flash_data = (MODE_SETTING_T *)Address;
  while((Address < Address_t+len) && (MemoryProgramStatus != FAILED))
  {
    if((*(__IO uint32_t*) Address) != sourc_data->flash_word[i])
//  if(flash_data->flash_word[i] != sourc_data->flash_word[i])
    {
      MemoryProgramStatus = FAILED;
    }
    i++;
    Address += 4;
  }
  return MemoryProgramStatus;
  
}

void Firmware_init(void)
{
	int i;
	for(i = 0; i<64 ; i++)
	{
		firmwareControlBLK[i].block_len = 0;
		firmwareControlBLK[i].blockBuf = &blockBuffer[0];
		firmwareControlBLK[i].blockFlag = 0;
	}
	block_count = -1;
}

/*
typedef struct
{
  uint16_t block_len;
	uint8_t  blockFlag;
	uint8_t* blockBuf;
  uint32_t block_base_address;
}CONTROL_BLOCK;
*/
uint8_t FirmwareReceive(uint8_t *recvBuf)
{
	uint8_t retValue = 0;

	block_len = recvBuf[0];
	base_addr_curr = (recvBuf[1] << 24)|(recvBuf[2] << 16)|(recvBuf[3] << 8)|(recvBuf[4] << 0);
	if(base_addr_last == 0)
	{
		block_count++;
    firmwareControlBLK[block_count].block_len = block_len;
		firmwareControlBLK[block_count].block_base_address = base_addr_curr;
		(void)memcpy((void*)&firmwareControlBLK[block_count].blockBuf[0],(void*)&recvBuf[5], block_len);	
		base_addr_last = base_addr_curr;
	}
	else if(base_addr_curr == base_addr_last + block_len)
	{
		(void)memcpy((void*)&firmwareControlBLK[block_count].blockBuf[firmwareControlBLK[block_count].block_len],(void*)&recvBuf[5], block_len);	
		firmwareControlBLK[block_count].block_len += block_len;
		if(firmwareControlBLK[block_count].block_len == 2048*3)
		{		
			firmwareControlBLK[block_count].blockFlag = 1;
			base_addr_last = 0;    // indicate one store block ends
			if(block_count == 0)
			{
			   if(myFlashWrite_1((uint8_t*)(FLASH_STORE_BASE_ADDRESS),
				                   (uint8_t*)firmwareControlBLK[block_count].blockBuf, 2048*3) != PASSED)
					 retValue = 1;
         else
				   retValue = 0;
			}
			else 
			{
			   if(myFlashWrite_1((uint8_t*)(FLASH_STORE_BASE_ADDRESS+firmwareControlBLK[block_count-1].block_len),
				                   (uint8_t*)firmwareControlBLK[block_count].blockBuf, 2048*3) != PASSED)
					 retValue = 1;
         else
				   retValue = 0;					 
			}
	  }
		else
		{
			base_addr_last = base_addr_curr;
			return 1;     // one store block (2048*3 not end)
		}
	}
	else if(base_addr_curr != base_addr_last + block_len)
	{
			if(block_count == 0)
			{
			   if(myFlashWrite_1((uint8_t*)(FLASH_STORE_BASE_ADDRESS),
				                   (uint8_t*)firmwareControlBLK[block_count].blockBuf, firmwareControlBLK[block_count].block_len) != PASSED)
					 retValue = 1;
         else
				   retValue = 0;
			}
			else 
			{
			   if(myFlashWrite_1((uint8_t*)(FLASH_STORE_BASE_ADDRESS+firmwareControlBLK[block_count-1].block_len),
				                   (uint8_t*)firmwareControlBLK[block_count].blockBuf, firmwareControlBLK[block_count].block_len) != PASSED)
					 retValue = 1;
         else
				   retValue = 0;
			}		
			
			block_count = 0;
			firmwareControlBLK[block_count].block_len = block_len;
			firmwareControlBLK[block_count].block_base_address = base_addr_curr;
			(void)memcpy((void*)&firmwareControlBLK[block_count].blockBuf[0],(void*)&recvBuf[5], block_len);	
			base_addr_last = base_addr_curr;
	}
	
	return retValue;
}

