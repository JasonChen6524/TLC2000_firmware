#ifndef __TBT_H
#define __TBT_H

#include "stm3210e_TLC2000.h"


typedef unsigned char byte;
#define UART_COMM_BUFF_SIZE 150

typedef struct
{
	volatile byte TX[UART_COMM_BUFF_SIZE];
	volatile byte RX[UART_COMM_BUFF_SIZE];
	
	volatile byte TXp;
	volatile byte TXg;
	volatile byte RXp;
	volatile byte RXg;
	
	volatile byte TXf;
	volatile byte RXf;
	
	volatile byte TXl;
	volatile byte RXl;
} UartParams;


void TLC_UartInit(void);

void BT_Task(void);

typedef enum 
{
	UART_STATE_ALARM,
    UART_STATE_INIT,
    UART_STATE_RE_TX,
    UART_STATE_IDLE,
    UART_STATE_TX, 
    UART_STATE_RX,
    UART_STATE_RPT,
    UART_STATE_ON,
    UART_STATE_OFF
}UART_STATE;

#define UART_PROTO_CMD_MSK 0x80
#define UART_PROTO_LEN_MSK  0x7F

typedef enum 
{
	UART_CMD_NONE,
	UART_CMD_ON,
    UART_CMD_OFF,
    UART_CMD_ALARM, 
    UART_CMD_RPT 
    
}UART_CMD_;

#define UART_PROTO_PKT_HEAD_LEN 0x5
typedef struct 
{   
	byte magic[2]; // 
	int len;   // data len
	byte cmd;   // 
	byte data[20];    
} UART_PROTO_PKT; 

#define UART_MAX_PKT_LEN 14

typedef enum
{
	BLE_CMD_TYPE_NONE,
	BLE_CMD_TYPE_FAILED,
	BLE_CMD_TYPE_SET_TREATMENT ,
	BLE_CMD_TYPE_START_TREATMENT,
	BLE_CMD_TYPE_STOP_TREATMENT,
	BLE_CMD_TYPE_CHECK_TREATMENT,
	BLE_CMD_TYPE_SET_MEASUREMENT,
	BLE_CMD_TYPE_READ_MEASUREMENT,
	BLE_CMD_TYPE_SET_SYSTEM,
	BLE_CMD_TYPE_START_SELFTEST,
	 
		
} BLE_CMD_TYPE;

typedef enum
{
	BLE_CMD_STAT_PRE1,
	BLE_CMD_STAT_PRE2,
	BLE_CMD_STAT_CMD,
	BLE_CMD_STAT_SET_TREATMENT ,
	BLE_CMD_STAT_START_TREATMENT,
	BLE_CMD_STAT_STOP_TREATMENT,
	BLE_CMD_STAT_CHECK_TREATMENT,
	BLE_CMD_STAT_SET_MEASUREMENT,
	BLE_CMD_STAT_READ_MEASUREMENT,
	BLE_CMD_STAT_SET_SYSTEM,
	BLE_CMD_STAT_SELFTEST
	
	
} BLE_CMD_STAT;



typedef struct 
{   
	byte on; // 
	byte duration;
	byte power;   // 
	byte tense;
	
} BLE_CONFIG; 

typedef struct 
{
    short  cmd;
	short dec905[6] ; 
	short dec660[4] ;
	short fpga905 ;
	short fpga660;
	short fpga905Cadence;
	short fpga660Cadence;
	short duration;
	short tensDec;
    short tensPwm;
    float mW1;
    float mW2;
}TLC_TREATMENT_SETTING;

typedef struct 
{
    short cmd;
	short buzzer; 
	short color;
}TLC_SYS_SETTING;		

typedef struct  
{
    short cmd;
	short dec905;
	short fpga905;
	short fpga905Cadence;       
	short  which;
} TLC_MEASUREMENT_SETTING;


typedef struct 
{
	 int mode;
	 int battery;
	 int buzzer;
	 int wireless;
	 int color;
	 int bf0;
	 int bf1;
	 int bf2;
	 int bf3;
	 float mw0;
	 float mw1;
	 float mw2;
	 float mw3;
	 int   selfTest;
	 int   adc_ch_reading[5];
}TLC_REPORT;		
typedef enum
{
	TLC_SYSTEM_WHICH_ITEM_NONE,    
	TLC_SYSTEM_WHICH_ITEM_ALL,  
	TLC_SYSTEM_WHICH_ITEM_BUZZER,
	TLC_SYSTEM_WHICH_ITEM_COLOR	
}TLC_SYSTEM_WHICH_ITEM;

typedef struct 
{
	 short  which; 
	 short  buzzer;
	 short  color;
}TLC_SYSTEM_SETTING;			

//typedef enum   //remote side runing state  
//{
//    BLE_RPT_MODE_IDL,               // probe - command finished waiting for next 
//    BLE_RPT_MODE_MEASUREMENT_BUSY,  //probe doing measuring
//    BLE_RPT_MODE_MEASUREMENT_ARMED,  //probe measurement get armed
//    BLE_RPT_MODE_MEASUREMENT_ERR_1, // probe measure error 1
//    BLE_RPT_MODE_MEASUREMENT_ERR_2, // probe measure error 2
//    BLE_RPT_MODE_TREATMENT_BUSY,    // probe treatment in the middle of treatment laser is on
//    BLE_RPT_MODE_TREATMENT_ARMED,   // probe get armed and waiting for button to start laser is off
//    BLE_RPT_MODE_TREATMENT_ERR1,    // probe treatment error 1 
//    BLE_RPT_MODE_TREATMENT_ERR2,    // probe treatment error 2
//
//} PROBE_STATUS;	


//extern __IO uint8_t recvFlag;                // Added by Jason, 2014.11.11
//extern __IO BLE_CONFIG pcCmd;                // Added by Jason, 2014.11.11 
//extern __IO BLE_CONFIG oldPcCmd;             // Added by Jason, 2014.11.11
#endif


