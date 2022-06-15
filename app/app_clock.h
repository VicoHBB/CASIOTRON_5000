#ifndef APP_CLOCK_H_
#define APP_CLOCK_H_

#include "app_bsp.h"
#include <stdint.h>

/*  states of clock task */
#define IDLE_CLK      10u
#define HOUR_DATE     11u
#define SHOW_ALARM    12u
#define ALARM_ON      13u
#define SET_PARAM     14u
#define PRINT_HD      15u
#define PRINT_ALARM   16u
#define PRINT_WAKE    17u
#define DISABLE_ALARM 18u

void Clock_Init( void );    /*  Init all the configs of the clock */
void GPIO_C_Config( void ); /*  Init the GPIO of the board */
void INT_Config( void );    /*  Init the interruptions */
void RTC_Init( void );      /*  Init the RTC */
void SPI_Init( void );      /*  Init the SPI */
void LCD_Init( void );      /*  Init the LCD */
void I2C_Init( void );           /*  Init the I2C */
void Sensor_Init( void );   /*  Init temperature sensor */
void Clock_Task( void );    /*  Do the procces of the clock */
void Set_Date( RTC_DateTypeDef *RTC_Date, uint8_t day, uint8_t month, uint8_t year ); /*  Set the date of RTC */ 
void Set_Time( RTC_TimeTypeDef *RTC_Time, uint8_t hour, uint8_t min, uint8_t sec );   /*  Set the time of RTC */
void Set_Alarm( RTC_AlarmTypeDef *RTC_Alarm, uint8_t hour, uint8_t min );             /*  Set the alarm of RTC */
void Structure_Processign( void );                         /*  Proces all the strucure */
void Disp_Date( RTC_DateTypeDef *Date );                   /*  Show the date in LCD */
void Disp_Hour_Temp( RTC_TimeTypeDef *Time, uint8_t temperature );                     /*  Show The day in LCD */
void Disp_Alarm( RTC_AlarmTypeDef *Alarm );                /*  Print the params of the alarm */
void Disp_Alarm_On( RTC_TimeTypeDef *Time, uint8_t counter ); /*  Print the alarm on state */
uint16_t WeekDay( uint8_t day, uint8_t month, uint16_t year );              /*  Calculate the day of the week */

#endif
