#include "app_bsp.h"
#include "app_clock.h"


RTC_HandleTypeDef         RTCHandle;                   /*  Structure for RTC */

// Structure for change parameters
extern QUEUE_HandleTypeDef       Serial_Msg_2_Read;

SERIAL_MsgTypeDef         Serial_Msg;

/*  Variables for interruptions */
__IO ITStatus button            = RESET;
__IO ITStatus Alarm             = RESET;
__IO ITStatus let_up            = RESET;
__IO ITStatus Alarm_Active      = RESET;


/*  Variables for delay */
static uint32_t last_time        = 0u;
static uint32_t last_time_1      = 0u;
static uint32_t last_time_2      = 0u;
static uint32_t count_A          = 0u;
static uint32_t count_AOn        = 0u;

extern void initialise_monitor_handles( void );

void Clock_Init( void )
{
    initialise_monitor_handles();
    printf("\n");
    GPIO_C_Config(); 
    INT_Config();
    RTC_Init();
/*  Obtatin the first time the actual tick for show the value of clock */
    last_time   = HAL_GetTick();
    last_time_1 = HAL_GetTick();
    last_time_2 = HAL_GetTick();

}

void GPIO_C_Config( void )
{
    GPIO_InitTypeDef          GPIO_InitStruct;             /*  Structure for GPIO's */

    __HAL_RCC_GPIOC_CLK_ENABLE();                        /*  Enable clock for port C */
    GPIO_InitStruct.Pin = GPIO_PIN_13;                   /*  Configure pin 13 for the interruption */
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;  /*   Edge interrupt */
    GPIO_InitStruct.Pull = GPIO_NOPULL;                  /*  No Pull */
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;        /*  High speed */
    /*  Apply the configuration */
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

void INT_Config(void)
{
    HAL_NVIC_SetPriority(EXTI4_15_IRQn,3,0);
    HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
}

void RTC_Init(void)
{
    RTC_TimeTypeDef           RTC_TimeInit;                /*  Structure for init the time of RTC */
    RTC_DateTypeDef           RTC_DateInit;                /*  Structure for init the date of RTC */
    RTC_AlarmTypeDef          RTC_AlarmInit;               /*  Structure for init the alarm of RTC */

    RTCHandle.Instance            = RTC;
    RTCHandle.Init.HourFormat     = RTC_HOURFORMAT_24;
    RTCHandle.Init.AsynchPrediv   = 0x7F;    // 127
    RTCHandle.Init.SynchPrediv    = 0xFF;    // 255      (32.768)/((127+1)(255+1))=1Hz
    RTCHandle.Init.OutPut         = RTC_OUTPUT_DISABLE;
    RTCHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_LOW;
    RTCHandle.Init.OutPutType     = RTC_OUTPUT_TYPE_PUSHPULL;
    HAL_RTC_Init(&RTCHandle);

    /*  Initial timme for RTC like 00 hrs : 00 mins :00secs */
    RTC_TimeInit.Hours   = 0;
    RTC_TimeInit.Minutes = 0;
    RTC_TimeInit.Seconds = 0;
    HAL_RTC_SetTime(&RTCHandle,&RTC_TimeInit,RTC_FORMAT_BIN);

    /*  Inital date for RTC like  1/1/2000 */
    RTC_DateInit.Date    = 1;
    RTC_DateInit.Month   = 1;
    RTC_DateInit.Year    = 0;                 // 2000
    RTC_DateInit.WeekDay = RTC_WEEKDAY_SATURDAY;
    HAL_RTC_SetDate( &RTCHandle, &RTC_DateInit, RTC_FORMAT_BIN);

    /*  Enable alarm A */
    RTC_AlarmInit.AlarmTime.Hours    = 12;                      /*  Set the first alarm at 12 */
    RTC_AlarmInit.AlarmTime.Minutes  = 0;
    RTC_AlarmInit.AlarmMask          = RTC_ALARMMASK_SECONDS | RTC_ALARMMASK_DATEWEEKDAY;
    RTC_AlarmInit.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
    RTC_AlarmInit.Alarm              = RTC_ALARM_A;
    HAL_RTC_SetAlarm_IT(&RTCHandle,&RTC_AlarmInit, RTC_FORMAT_BIN);
}

void Clock_Task( void )
{
/*  Initial state of the machine */
static uint8_t Clock_State;                           
//  Data to modify the parameters of RTC
static RTC_TimeTypeDef           Get_Time;
static RTC_DateTypeDef           Get_Date;
static RTC_AlarmTypeDef          Get_Alarm;

    switch ( Clock_State )
    {
        case IDLE_CLK:
            if ( HIL_QUEUE_Read( &Serial_Msg_2_Read, &Serial_Msg ) == 1u )
            {
                Clock_State = SET_PARAM;
            }
            else if ( Alarm == SET ) 
            {
                Clock_State = ALARM_ON;
            }
            else if ( button == SET )
            {
                Clock_State = SHOW_ALARM;
            }
            else if ( ( HAL_GetTick() - last_time  ) >= 1000  )
            {
                last_time   = HAL_GetTick();
                Clock_State = HOUR_DATE;
            }
            else
            {

            }
            break;
        case HOUR_DATE: 
            HAL_RTC_GetTime( &RTCHandle, &Get_Time, RTC_FORMAT_BIN);             // First take the time
            HAL_RTC_GetDate( &RTCHandle, &Get_Date, RTC_FORMAT_BIN);             // After the Date
            Clock_State = PRINT_HD; 
            break;
        case SET_PARAM:
            Structure_Processign();
            Clock_State    = HOUR_DATE;
            break;
        case PRINT_HD:
            Disp_Date( &Get_Date);
            Disp_Hour( &Get_Time );
            Clock_State = IDLE_CLK;
            break;
        case SHOW_ALARM:
            if ( count_A < 1u ) 
            {
                Clock_State   = PRINT_ALARM;
            }
            else if ( ( HAL_GetTick() - last_time_1  ) >= 1000u )
            {
                last_time_1   = HAL_GetTick();
                Clock_State   = PRINT_ALARM;
            }
            else 
            {
                Clock_State = IDLE_CLK;
            }
            break;
        case PRINT_ALARM:
            HAL_RTC_GetAlarm( &RTCHandle, &Get_Alarm, RTC_ALARM_A, RTC_FORMAT_BIN );

            if ( Alarm_Active == SET )
            {
                Disp_Date( &Get_Date);
                Disp_Alarm( &Get_Alarm);
            }
            else 
            {
                printf(" NO ALARM CONFIG \n");
            }

            Clock_State = IDLE_CLK;
            break;
        case ALARM_ON:
            HAL_RTC_GetTime( &RTCHandle, &Get_Time, RTC_FORMAT_BIN);             // First take the time
            HAL_RTC_GetDate( &RTCHandle, &Get_Date, RTC_FORMAT_BIN);             // After the Date

            if ( count_AOn < 1u ) 
            {
                Clock_State = PRINT_WAKE;
                count_AOn++;
            }

            else if ( ( HAL_GetTick() - last_time_2  ) >= 1000u ) 
            {
                last_time_2   = HAL_GetTick();
                Clock_State = PRINT_WAKE;
            }
            
            else if ( let_up == SET )
            {
                Clock_State = DISABLE_ALARM;
            }
            
            else if ( Get_Time.Minutes != Get_Alarm.AlarmTime.Minutes )
            {
                Clock_State = DISABLE_ALARM;
            }

            else 
            {
                Clock_State = IDLE_CLK;
            }
            break;
        case PRINT_WAKE:
            Disp_Date( &Get_Date );
            Disp_Alarm_On( &Get_Time, count_AOn );

            Clock_State = IDLE_CLK;
            break;
        case DISABLE_ALARM:
            HAL_RTC_DeactivateAlarm ( &RTCHandle, RTC_ALARM_A ); 
            Alarm             = RESET;
            count_AOn         = 0;
            let_up            = RESET;
            Alarm_Active      = RESET;
            Clock_State       = IDLE_CLK;
            break;
        default:
            Clock_State = IDLE_CLK;
            break;
    }
}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_PIN)
{/* Disable the alarm  */ 
    
    button  = RESET;
    count_A = 0;
    if ( Alarm == SET )
    {
        let_up = SET;
    }
}

/**
  * @brief This function inits GPIO_PIN_5 of port A as an output for heartbeat 
  * @param: void  
  * @retval void
  */
/*  cppcheck-suppress misra-c2012-8.4; This function is declared in a reserved file of HAL library */
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_PIN)
{/* Print the value of the alarm */
    (void)GPIO_PIN;
    button = SET;
    count_A++;
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    Alarm = SET;
    count_AOn++;
}

void Set_Date( RTC_DateTypeDef *RTC_Date, uint8_t day, uint8_t month, uint8_t year )
{
    
    RTC_Date->Date    = day;
    RTC_Date->Month   = month;
    RTC_Date->Year    = year;
    RTC_Date->WeekDay = WeekDay( day, month, ( 2000u + year ) );

    HAL_RTC_SetDate(&RTCHandle,RTC_Date,RTC_FORMAT_BIN);

}

void Set_Time(RTC_TimeTypeDef *RTC_Time, uint8_t hour, uint8_t min, uint8_t sec )
{
    RTC_Time->Hours   = hour;
    RTC_Time->Minutes = min;
    RTC_Time->Seconds = sec;
    HAL_RTC_SetTime(&RTCHandle,RTC_Time,RTC_FORMAT_BIN);
}

void Set_Alarm( RTC_AlarmTypeDef *RTC_Alarm, uint8_t hour, uint8_t min )
{
    RTC_Alarm->AlarmTime.Hours    = hour;
    RTC_Alarm->AlarmTime.Minutes  = min;
    RTC_Alarm->AlarmMask          = RTC_ALARMMASK_SECONDS | RTC_ALARMMASK_DATEWEEKDAY;
    RTC_Alarm->AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE;
    RTC_Alarm->Alarm              = RTC_ALARM_A;
    HAL_RTC_SetAlarm_IT(&RTCHandle,RTC_Alarm,RTC_FORMAT_BIN);
    Alarm_Active = SET;
}

void Structure_Processign( void )
{
    //  Data to modify the parameters of RTC
    static RTC_TimeTypeDef           set_Time;
    static RTC_DateTypeDef           set_Date;
    static RTC_AlarmTypeDef          set_Alarm;

    switch (Serial_Msg.msg)
    {
        case TIME:                                              // AT+TIME
            
            Set_Time(&set_Time, Serial_Msg.param_1 , Serial_Msg.param_2, Serial_Msg.param_3);
            
            break;
        case DATE:                                              // AT+DATE
            
            Set_Date(&set_Date, Serial_Msg.param_1 , Serial_Msg.param_2, Serial_Msg.param_3);
            
            break;
        case ALARM:                                              // AT+ALARM
            
            Set_Alarm(&set_Alarm, Serial_Msg.param_1 , Serial_Msg.param_2);
            
            break;
        default:
            break;
    }
}

/**
  * @brief This function inits GPIO_PIN_5 of port A as an output for heartbeat 
  * @param: void  
  * @retval void
  */
void Disp_Date( RTC_DateTypeDef *Date )
{
    printf("%d / %d / %d -", Date->Date, Date->Month, Date->Year );
    
    /*  Print the day */
    switch ( Date->WeekDay) 
    {
        case 1:
            printf( "Mon \n" );
            break;
        case 2:
            printf( "Tue \n" );
            break;
        case 3:
            printf( "Wed \n" );
            break;
        case 4:
            printf( "Thu \n" );
            break;
        case 5:
            printf( "Fri \n" );
            break;
        case 6:
            printf( "Sat \n" );
            break;
        case 7:
            printf( "Sun \n" );
            break;
        default:
            break;
    }
}

void Disp_Hour( RTC_TimeTypeDef *Time )
{
    printf("%d : %d : %d \n", Time->Hours, Time->Minutes, Time->Seconds );
}

void Disp_Alarm( RTC_AlarmTypeDef *Alarm)
{
    printf("%d : %d \n", Alarm->AlarmTime.Hours, Alarm->AlarmTime.Minutes );
}

void Disp_Alarm_On( RTC_TimeTypeDef *Time,  uint8_t counter )
{
    printf("WAKE UP!!!!\n");
}

uint16_t WeekDay( uint8_t day ,uint8_t month, uint16_t year)
{

    uint16_t month_aux = ( uint16_t )month;
    uint8_t regular[] = { 0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5 };
    uint8_t leap[]    = { 0, 3, 4, 0, 2, 5, 0, 3, 6, 1, 4, 6 };
    uint16_t aux_1; 
    uint16_t aux_2;
    uint16_t aux_3; 
    uint16_t aux_4; 
    uint16_t aux_5;
    uint16_t result;

    /* Check the year first */
    if ( ( ( year % 4u ) == 0u ) && !( ( year % 100u ) == 0u ) )
    {
        month_aux = leap[ month_aux - 1u ];
    } 
    else if ( ( year % 400u ) == 0u )
    {
        month_aux = leap[ month_aux - 1u ];
    }
    else
    {
        month_aux = regular[ month_aux - 1u ]; 
    }
    /* calculate the day of the weak by parts  */
    aux_1 = ( year - 1u ) % 7u;
    aux_2 = ( year - 1u ) / 4u;
    aux_3 = ( 3u * ( ( ( year - 1u ) / 100u ) + 1u )) / 4u;
    aux_4 = ( aux_2 - aux_3 ) % 7u;
    aux_5 = ( uint16_t )day % 7u;
    result = ( aux_1 + aux_4 + month_aux + aux_5 ) % 7u;
    
    return result;

}


