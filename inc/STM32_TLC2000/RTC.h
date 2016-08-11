#ifndef __RTC_H
#define __RTC_H

#include "stm3210e_TLC2000.h"

extern __IO uint32_t TimeDisplay;
extern __IO int32_t treatment_time;
extern __IO int32_t treatment_time_backup;
extern __IO int32_t new_treatment_time;

extern __IO uint8_t Send_Buffer[];// 2014.10.20

void RTC_NVIC_Configuration(void);
void RTC_Configuration(void);
void RTC_Init(void);
void AdjustTime(uint8_t *dateTime);
void DisplayTreatmentTime(int32_t treatmentTime);
void Time_Show(void);
void Time_Display(uint32_t TimeVar);

void DisplaySelfTestTime(int32_t treatmentTime);

#endif
