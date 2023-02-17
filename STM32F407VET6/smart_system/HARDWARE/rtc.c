#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "rtc.h"

extern U8 alarm_set_event;
extern char alarm_data[20];
static volatile U32  wake_up_event;  
static volatile U32  alarm_event;

void Rtc_Init(void)
{
    NVIC_InitTypeDef  NVIC_InitStructure;
    EXTI_InitTypeDef  EXTI_InitStructure;

    RTC_TimeTypeDef  RTC_TimeStructure;
    RTC_InitTypeDef  RTC_InitStructure;
    RTC_DateTypeDef  RTC_DateStructure;

    uint32_t uwAsynchPrediv = 0;
    uint32_t uwSynchPrediv = 0;

    /* Enable the PWR clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

    /* Allow access to RTC */
    PWR_BackupAccessCmd(ENABLE);
    /* Enable the LSE OSC */
    RCC_LSEConfig(RCC_LSE_ON);          
    /* Wait till LSE is ready */  
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
    {
    }
    /* Select the RTC Clock Source */
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); 

    /* Enable the RTC Clock */
    RCC_RTCCLKCmd(ENABLE);   
    /* Wait for RTC APB registers synchronisation */
    RTC_WaitForSynchro();

    uwAsynchPrediv = 0x7F;
    uwSynchPrediv = 0xFF;
    /* Configure the RTC data register and RTC prescaler */
    RTC_InitStructure.RTC_AsynchPrediv = uwAsynchPrediv;
    RTC_InitStructure.RTC_SynchPrediv = uwSynchPrediv;
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
    RTC_Init(&RTC_InitStructure);

    if (RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x32F2)
    {
        /* Set the date: Monday October 17th 2022 */
        RTC_DateStructure.RTC_Year = 0x22;
        RTC_DateStructure.RTC_Month = RTC_Month_October ;
        RTC_DateStructure.RTC_Date = 0x17;
        RTC_DateStructure.RTC_WeekDay = RTC_Weekday_Monday;
        RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);

        /* Set the time to 11h 55mn 00s PM */
        RTC_TimeStructure.RTC_H12     = RTC_H12_PM;   
        RTC_TimeStructure.RTC_Hours   = 0x11;
        RTC_TimeStructure.RTC_Minutes = 0x55;
        RTC_TimeStructure.RTC_Seconds = 0x00; 
        RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);   
    }


    /* Indicator for the RTC configuration */
    RTC_WriteBackupRegister(RTC_BKP_DR0, 0x32F2);

    //Close wakeup function
	RTC_WakeUpCmd(DISABLE);
	
    //Choose the configed clock to wakeup function
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
	
    //Set the wakeup count value to automatically reload,the write value is 0 by default
	RTC_SetWakeUpCounter(1-1);
	
	//Clear RTC wakeup interrupt flag
	RTC_ClearITPendingBit(RTC_IT_WUT);
	
	//enable RTC wakeup interrupt
	RTC_ITConfig(RTC_IT_WUT, ENABLE);

	//enable wakeup function
	RTC_WakeUpCmd(ENABLE);

	/* Configure EXTI Line22 */
	EXTI_InitStructure.EXTI_Line = EXTI_Line22;			
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;			
	EXTI_Init(&EXTI_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;		
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02    ;		
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	NVIC_Init(&NVIC_InitStructure);


} 

void Rtc_Alarm_Init(void)
{
    NVIC_InitTypeDef  NVIC_InitStructure;
    EXTI_InitTypeDef  EXTI_InitStructure;

    RTC_AlarmTypeDef RTC_AlarmStructure;
    /* Enable RTC Alarm A Interrupt */
    RTC_ITConfig(RTC_IT_ALRA, ENABLE);

    RTC_ClearFlag(RTC_FLAG_ALRAF);  
    /* RTC Alarm A Interrupt Configuration */
    /* EXTI configuration *******************************************************/
    EXTI_ClearITPendingBit(EXTI_Line17);
    EXTI_InitStructure.EXTI_Line = EXTI_Line17;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable the RTC Alarm Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure); 
    /* Set the alarm 11h:55min:30s */
    RTC_AlarmStructure.RTC_AlarmTime.RTC_H12     = RTC_H12_PM;
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   = 0x11;
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = 0x55;
    RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = 0x10;
    //RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x18;
    RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
    RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;

    /* Disnable the alarm */
    RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
    /* Configure the RTC Alarm A register */
    RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);
    /* Enable the alarm */
    RTC_AlarmCmd(RTC_Alarm_A, ENABLE);


}

void Set_RtcDate(U32 Date)
{
	RTC_DateTypeDef  RTC_DateStructure;
    /* Set the date: Monday October 17th 2022 */
    RTC_DateStructure.RTC_Year = (U8)((Date & (DATI_MASK << 24))>>24);
    RTC_DateStructure.RTC_Month = (U8)((Date & (DATI_MASK << 16))>>16);
    RTC_DateStructure.RTC_Date = (U8)((Date & (DATI_MASK << 8))>>8);
    RTC_DateStructure.RTC_WeekDay = (U8)(Date & (DATI_MASK));
    RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);

}
void Set_RtcTime(U32 Time)
{

	RTC_TimeTypeDef  RTC_TimeStructure;
    /* Set the time to 11h 55mn 00s PM */  
 
    if((U8)((Time & (DATI_MASK << 24))>>24) < 12)
    {
        RTC_TimeStructure.RTC_H12 = RTC_H12_AM;
    }
    else
    {
        RTC_TimeStructure.RTC_H12 = RTC_H12_PM;
    }
    RTC_TimeStructure.RTC_Hours   = (U8)((Time & (DATI_MASK << 16))>>16);
    RTC_TimeStructure.RTC_Minutes = (U8)((Time & (DATI_MASK << 8))>>8);
    RTC_TimeStructure.RTC_Seconds = (U8)(Time & (DATI_MASK));  
    RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);  
}

void Show_Alarm(void)
{
    RTC_AlarmTypeDef RTC_AlarmStructure;;
    RTC_GetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);
    printf("%0.2x:%0.2x:%0.2x\n", RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours, RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes, RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds);
}

void Show_Date(void)
{
    RTC_DateTypeDef RTC_DateStructure;
    RTC_TimeTypeDef RTC_TimeStructure;
    RTC_GetTime(RTC_Format_BCD, &RTC_TimeStructure);
    RTC_GetDate(RTC_Format_BCD, &RTC_DateStructure);

    printf("20%02x.%02x.%02x week_%02x %02x:%02x:%02x\n",RTC_DateStructure.RTC_Year,RTC_DateStructure.RTC_Month,RTC_DateStructure.RTC_Date,RTC_DateStructure.RTC_WeekDay,\
    RTC_TimeStructure.RTC_Hours, RTC_TimeStructure.RTC_Minutes, RTC_TimeStructure.RTC_Seconds);
}


void Update_Time(void)
{
    U32  Time = 0;
    U32  Date = 0;
    U32  data = 0;
    char* ptr;

    if(1 == wake_up_event)
    {
        Show_Date();			
        wake_up_event = 0;
    }
    if(1 == alarm_event)
    {
        printf("ding ding ding\n");
        alarm_event = 0;
    }       
    else
    {

    }
    if(1 == alarm_set_event)
        {
			printf(&alarm_data[0]);
			printf("\n");
			if(strstr(&alarm_data[0], "TIME"))
            {
                strtok(&alarm_data[0],"-");

                ptr = strtok(NULL,"-"); //Hours
                data = atoi(ptr);
                data = (data/10)*16+(data%10);
                Time = Time|(data << 16);

                ptr = strtok(NULL,"-"); //Minutes
                data = atoi(ptr);
                data = (data/10)*16+(data%10);
                Time = Time|(data << 8);

                ptr = strtok(NULL,"-"); //Seconds
                data = atoi(ptr);
                data = (data/10)*16+(data%10);
                Time = Time|data;

                Set_RtcTime(Time);

            }
            if(strstr(&alarm_data[0], "DATE"))
            {
                strtok(&alarm_data[0],"-");

                ptr = strtok(NULL,"-"); //Year
                data = atoi(ptr);
                data = (data/10)*16+(data%10);
                Date = Date|(data << 24);

                ptr = strtok(NULL,"-"); //Month
                data = atoi(ptr);
                data = (data/10)*16+(data%10);
                Date = Date|(data << 16);

                ptr = strtok(NULL,"-"); //Date
                data = atoi(ptr);
                data = (data/10)*16+(data%10);
                Date = Date|(data << 8);

                ptr = strtok(NULL,"-"); //Weekday
                data = atoi(ptr);
                data = (data/10)*16+(data%10);

                Date = Date|data;
                Set_RtcDate(Date);
            }
			
            alarm_set_event = 0;
            memset(&alarm_data[0],0,20);
			Date = 0;
			Time = 0;
        }
}


void RTC_WKUP_IRQHandler(void)
{
    if(RTC_GetITStatus(RTC_IT_WUT)!= RESET)
    {
        wake_up_event = 1;
        RTC_ClearITPendingBit(RTC_IT_WUT);
        EXTI_ClearITPendingBit(EXTI_Line22);
    }
    else
    {
        
    }
}

void RTC_Alarm_IRQHandler(void)
{
    if(RTC_GetITStatus(RTC_IT_ALRA)!= RESET)
    {
        alarm_event = 1;
        RTC_ClearITPendingBit(RTC_IT_ALRA);
        EXTI_ClearITPendingBit(EXTI_Line17);
    }
    else
    {
        
    }
}