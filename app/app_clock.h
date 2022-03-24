#ifndef APP_CLOCK_H_
#define APP_CLOCK_H_

#include "app_bsp.h"
#include "stm32g0xx_hal_rtc.h"
#include <stdint.h>

void Clock_Init( void );  /*  Init all the configs of the clock */
void INT_Config( void ); /*  Init the interruptions */
void RTC_Init( void );   /*  Init the RTC */
void Clock_Task( void );  /*  Do the procces of the clock */
uint8_t Set_Date( RTC_DateTypeDef *RTC_date, uint8_t day, uint8_t month, uint8_t year );    /*  Set the date of RTC */ 
uint8_t Set_Time( RTC_TimeTypeDef *RTC_Time, uint8_t hour, uint8_t min, uint8_t sec );     /*  Set the time of RTC */
uint8_t Set_Alarm( RTC_AlarmTypeDef *RTC_Alarm, uint8_t hour, uint8_t min );               /*  Set the alarm of RTC */







#endif
