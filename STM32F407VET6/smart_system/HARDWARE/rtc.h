#ifndef RTC_H
#define RTC_H

#include "type.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_rtc.h"
#include "stm32f4xx_pwr.h"
#include "stm32f4xx_exti.h"
#include "misc.h"

#define DATI_MASK 0x000000FF


void Rtc_Init(void);
void Show_Date(void);
void Rtc_Alarm_Init(void);
void Show_Alarm(void);
void Set_RtcDate(U32 Date);
void Set_RtcTime(U32 Time);
void Update_Time(void);

#endif
