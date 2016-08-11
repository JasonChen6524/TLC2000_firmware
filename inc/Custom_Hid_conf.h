#ifndef __CUSTOM_HID_CONF__
#define __CUSTOM_HID_CONF_


#define PACKET_MAX_65
#define MAX_TRANSFER_LEN      64//2
extern uint8_t Receive_Buffer[MAX_TRANSFER_LEN];

#ifdef PACKET_MAX_65
	#define REPORT_ID_5    2
	#define REPORT_ID_6    2
	#define REPORT_ID_7    2
#else
	#define REPORT_ID_5    5
	#define REPORT_ID_6    6
	#define REPORT_ID_7    7	
#endif

extern __IO uint8_t PrevXferComplete;
extern uint8_t recvCharFlag;

#endif
