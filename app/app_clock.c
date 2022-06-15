#include "app_bsp.h"
#include "app_clock.h"
#include "lcd.h"
#include "temp.h"

/*  Pins for the LCD */
#define CS    GPIO_PIN_9
#define RST   GPIO_PIN_8
#define RS    GPIO_PIN_6

/* cppcheck-suppress misra-c2012-8.4; This Structure is used in app_ints file */
RTC_HandleTypeDef         RTCHandle;                   /*  Structure for RTC */

// Structure for change parameters
extern QUEUE_HandleTypeDef       Serial_Msg_2_Read;

static SERIAL_MsgTypeDef         Serial_Msg;

/*  Variables for interruptions */
__IO ITStatus button            = RESET;
__IO ITStatus Alarm             = RESET;
__IO ITStatus let_up            = RESET;
__IO ITStatus Alarm_Active      = RESET;
__IO ITStatus Alarm_Temp_Active = RESET;


/*  Structures for the LCD */
static SPI_HandleTypeDef            SpiHandle;
static LCD_HandleTypeDef            HLCD;
/*  Structures for the Sensor */
static I2C_HandleTypeDef            I2C_1;
static TEMP_HandleTypeDef           Sensor;

/*  Variables for delay */
static uint32_t last_time        = 0u;
static uint32_t last_time_1      = 0u;
static uint32_t last_time_2      = 0u;
static uint32_t count_A          = 0u;
static uint32_t count_AOn        = 0u;

/* cppcheck-suppress misra-c2012-8.4; this is a parameter tat is used in the main file */
uint16_t delay_heart_beat = 300u;

static uint8_t up                = 99u;
static uint8_t low               = 0u;

/*! \brief Clock_Init -> Init the clock process
 *
 *  Init and Configure all the resources that the clock process will use 
 *
 * \return None
 */
void Clock_Init( void )
{

    GPIO_C_Config(); 
    INT_Config();
    RTC_Init();
    SPI_Init();
    LCD_Init();
    I2C_Init();
    Sensor_Init();
/*  Obtatin the first time the actual tick for show the value of clock */
    last_time   = HAL_GetTick();
    last_time_1 = HAL_GetTick();
    last_time_2 = HAL_GetTick();

}

/*! \brief GPIO_C_Config -> Init the GPIOC
 *
 *  Configure and Init the GPIOC_PIN_13 as the button
 *
 * \return None
 */
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

/*! \brief INT_Config -> Enable the interruptions
 *
 *  This function enable the interruption for the EXTI4_15_IRQn
 *
 * \return None
 */
void INT_Config(void)
{
    HAL_NVIC_SetPriority(EXTI4_15_IRQn,3,0);
    HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
}

/*! \brief RTC_Init -> Init the RTC
 *
 *  Configure the RTC to use it
 *
 * \return None
 */
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

/*! \brief SPI_Init -> Init the SPI
 *
 *  This fuction set all the parameters of the SPI to use it
 *
 * \return None
 */
void SPI_Init( void )
{

    SpiHandle.Instance               = SPI1;
    SpiHandle.Init.Mode              = SPI_MODE_MASTER;
    SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
    SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
    SpiHandle.Init.CLKPhase          = SPI_PHASE_2EDGE;
    SpiHandle.Init.CLKPolarity       = SPI_POLARITY_HIGH;
    SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
    SpiHandle.Init.CRCPolynomial     = 7;
    SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
    SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
    SpiHandle.Init.NSS               = SPI_NSS_SOFT;
    SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;

    HAL_GPIO_WritePin( GPIOC, CS, GPIO_PIN_SET );
    HAL_SPI_Init( &SpiHandle );
    
}

/*! \brief  LCD_Init -> Init the LCD
 *
 *  This fuction set the ports and pin that is going to be used on the LCD
 *
 * \return None
 */
void LCD_Init( void )
{
    
    HLCD.SpiHandler = &SpiHandle;
    HLCD.RSTPort    = GPIOC;
    HLCD.RSTPin     = RST;
    HLCD.RSPort     = GPIOC;
    HLCD.RSPin      = RS;
    HLCD.CSPort     = GPIOC;
    HLCD.CSPin      = CS;

    MOD_LCD_Init( &HLCD );

}

/*! \brief I2C_Init -> Init the I2C
 *
 *  This fuction enable the all the parameter of I2C
 *
 * \return None
 */
void I2C_Init( void )
{

    I2C_1.Instance              = I2C1;
    I2C_1.Init.Timing           = 0x20303E5D;
    I2C_1.Init.OwnAddress1      = 0x00;
    I2C_1.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
    I2C_1.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
    I2C_1.Init.OwnAddress2      = 0;
    I2C_1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    I2C_1.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
    I2C_1.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;
   
    HAL_I2C_Init( &I2C_1 );
    
}

/*! \brief Sensor_Init -> Init the Sensor
 *
 *  This funcion enable the sensor with no alarms 
 *
 * \return None
 */
void Sensor_Init( void )
{

    Sensor.I2CHandler          = &I2C_1;
    Sensor.AlertPort           = GPIOC;
    Sensor.AlertPin            = GPIO_PIN_5;
    
    MOD_TEMP_Init( &Sensor );

}

/*! \brief Clock_Task -> Control of the clock
 *
 *  This fuction contains the state machine which controls the clock process
 *
 * \return None
 */
void Clock_Task( void )
{
/*  Initial state of the machine */
static uint8_t Clock_State;                           
//  Data to modify the parameters of RTC
static RTC_TimeTypeDef           Get_Time;
static RTC_DateTypeDef           Get_Date;
static RTC_AlarmTypeDef          Get_Alarm;
/*  Data to modify the temperature */
static uint16_t temperature;

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
            else if ( ( HAL_GPIO_ReadPin( Sensor.AlertPort, Sensor.AlertPin ) ) == 0 )
            {
                count_A++;
                count_AOn++;
                Clock_State = ALARM_ON;
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
            temperature = MOD_TEMP_Read( &Sensor );
            Clock_State = PRINT_HD; 
            break;
        case SET_PARAM:
            Structure_Processign();
            Clock_State    = HOUR_DATE;
            break;
        case PRINT_HD:
            MOD_LCD_Command( &HLCD, CLEAR );
            Disp_Date( &Get_Date);
            Disp_Hour_Temp( &Get_Time, temperature );
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
            MOD_LCD_Command( &HLCD, CLEAR );
            if ( ( Alarm_Active == SET ) || ( Alarm_Temp_Active == SET ) )
            {
                Disp_Date( &Get_Date);
                Disp_Alarm( &Get_Alarm);
            }
            else 
            {
                Disp_Date( &Get_Date);
                MOD_LCD_SetCursor( &HLCD, 1, 0);
                MOD_LCD_String( &HLCD, "NO ALARM CONFIG\0");
            }
            Clock_State = IDLE_CLK;
            break;
        case ALARM_ON:
            HAL_RTC_GetTime( &RTCHandle, &Get_Time, RTC_FORMAT_BIN);             // First take the time
            HAL_RTC_GetDate( &RTCHandle, &Get_Date, RTC_FORMAT_BIN);             // After the Date
            temperature = MOD_TEMP_Read( &Sensor );
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

            else if ( ( Alarm_Temp_Active == SET ) && ( HAL_GPIO_ReadPin( Sensor.AlertPort, Sensor.AlertPin ) == 1u ) )
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
            Alarm_Temp_Active = RESET;
            low               = 0;
            up                = 99;
            Clock_State       = IDLE_CLK;
            break;
        default:
            Clock_State = IDLE_CLK;
            break;
    }
}
/*! \brief HAL_GPIO_EXTI_Rising_Callback -> Callback of the interrupt
 *
 *  This function is the callback of the interruption of EXTI rising edge
 *
 * \param GPIO_PIN  Is the pin that activate the interruption
 * \return None
 */
/*  cppcheck-suppress misra-c2012-8.4; This function is declared in a reserved file of HAL library */
void HAL_GPIO_EXTI_Rising_Callback( uint16_t GPIO_PIN )
{ 
    ( void )GPIO_PIN;
    /* Disable the alarm  */
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
void HAL_GPIO_EXTI_Falling_Callback( uint16_t GPIO_PIN )
{/* Print the value of the alarm */
    ( void )GPIO_PIN;
    button = SET;
    count_A++;
}

/*  cppcheck-suppress misra-c2012-8.4; This function is declared in a reserved file of HAL library */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    ( void )hrtc;
    Alarm = SET;
    count_AOn++;
}

/*! \brief Set_Date -> Set the date on RTC
 *
 *  This function set the date on the RTC
 *
 * \param *RTC_Date Is a pointer which contains all the parameter of date of RTC
 * \param day Is the day to set
 * \param month Is the month to set 
 * \param year Is the year to set ( Only the last two digits ) 
 * \return None
 */
void Set_Date( RTC_DateTypeDef *RTC_Date, uint8_t day, uint8_t month, uint8_t year )
{
    
    RTC_Date->Date    = day;
    RTC_Date->Month   = month;
    RTC_Date->Year    = year;
    RTC_Date->WeekDay = WeekDay( day, month, ( 2000u + year ) );

    HAL_RTC_SetDate(&RTCHandle,RTC_Date,RTC_FORMAT_BIN);

}

/*! \brief Set_Time -> Set the time of RTC
 *
 *  Configure the time in the RTC 
 *
 * \param *RTC_Time Is a pointer to the Structure that contains the information of RTC
 * \param hour Is the hour to set in RTC
 * \param min Is the minutes to set in RTC
 * \param sec Is the seconds to set in RTC
 * \return None
 */
void Set_Time(RTC_TimeTypeDef *RTC_Time, uint8_t hour, uint8_t min, uint8_t sec )
{

    RTC_Time->Hours   = hour;
    RTC_Time->Minutes = min;
    RTC_Time->Seconds = sec;
    HAL_RTC_SetTime(&RTCHandle,RTC_Time,RTC_FORMAT_BIN);

}

/*! \brief Set_Alarm -> This fuction set the alarm of the RTC
 *
 *  This fuction set the alarm of RTC only in Hour and minutes
 *
 * \param *RTC_AlarmTypeDef This a pointer to the Structure which contains the parameters of Alarm in the RTC
 * \param hour This the hour to set the alarm
 * \param min This is the minutes to set on the alarm
 * \return None
 */
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

/*! \brief Structure_Processign -> Read the meessage
 *
 *  This fuction read the MSG that arrives on Serial_Msg_2_Read and set all the configuration and parameters
 *
 * \return None
 */
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

        case HEARTBEAT:

            delay_heart_beat = Serial_Msg.param_1;
            
            break;

        case TEMP:

            MOD_TEMP_SetAlarm( &Sensor, Serial_Msg.param_1, Serial_Msg.param_2 );

            low = ( uint8_t )Serial_Msg.param_1;
            up  = ( uint8_t )Serial_Msg.param_2;
            Alarm_Temp_Active = SET;

            break;

        default:
            break;
    }
}

/*! \brief Disp_Date -> Show the date in LCD
 *
 *  This fuction substract all parameters of the Structure date and print it on LCD
 *
 * \param *Date  Is a pointer to the Structure that contains the date on the RTC
 * \return None
 */
void Disp_Date( RTC_DateTypeDef *Date )
{

    /*  Split the values in units and teens */

    uint8_t date_tens  = Date->Date / 10u;
    uint8_t date_units = Date->Date - ( date_tens * 10u );
    uint8_t year_tens  = Date->Year / 10u;
    uint8_t year_units = Date->Year - ( year_tens * 10u );

    /*  Set the cursor */
    MOD_LCD_SetCursor( &HLCD, 0, 1);

    switch ( Date->Month ) 
    {
        case 1:
            MOD_LCD_String( &HLCD, "JAN,\0");
            break;
        case 2:
            MOD_LCD_String( &HLCD, "FEB,\0");
            break;
        case 3:
            MOD_LCD_String( &HLCD, "MAR,\0");
            break;
        case 4:
            MOD_LCD_String( &HLCD, "APR,\0");
            break;
        case 5:
            MOD_LCD_String( &HLCD, "MAY,\0");
            break;
        case 6:
            MOD_LCD_String( &HLCD, "JUN,\0");
            break;
        case 7:
            MOD_LCD_String( &HLCD, "JUL,\0");
            break;
        case 8:
            MOD_LCD_String( &HLCD, "AUG,\0");
            break;
        case 9:
            MOD_LCD_String( &HLCD, "SEP,\0");
            break;
        case 10:
            MOD_LCD_String( &HLCD, "OCT,\0");
            break;
        case 11:
            MOD_LCD_String( &HLCD, "NOV,\0");
            break;
        case 12:
            MOD_LCD_String( &HLCD, "DEC,\0");
            break;
            
        default:
            break;
            
    }/*  End of the while  */

    /*  Move the cursor -1 position */
    MOD_LCD_SetCursor( &HLCD, 0, 5 );
    /*  Print the date */
    MOD_LCD_Data( &HLCD, ( date_tens + '0' ) );
    MOD_LCD_Data( &HLCD, ( date_units + '0' ) );

    /*  Move the cursor -1 position */
    MOD_LCD_SetCursor( &HLCD, 0, 8 );
    /*  Print year */
    MOD_LCD_String( &HLCD, "20\n");

    /*  Move 2 positions */
    MOD_LCD_SetCursor( &HLCD, 0, 10 );
    /*  Print the year */
    MOD_LCD_Data( &HLCD, ( year_tens + '0' ) );
    MOD_LCD_Data( &HLCD, ( year_units + '0' ) );

    /* Move the cursor 1 position */
    MOD_LCD_SetCursor( &HLCD, 0, 13 );
    
    /*  Print the day */
    switch ( Date->WeekDay) 
    {
        case 1:
            MOD_LCD_String( &HLCD, "Lu\n");
            break;
        case 2:
            MOD_LCD_String( &HLCD, "Ma\n");
            break;
        case 3:
            MOD_LCD_String( &HLCD, "Mi\n");
            break;
        case 4:
            MOD_LCD_String( &HLCD, "Ju\n");
            break;
        case 5:
            MOD_LCD_String( &HLCD, "Vi\n");
            break;
        case 6:
            MOD_LCD_String( &HLCD, "Sa\n");
            break;
        case 7:
            MOD_LCD_String( &HLCD, "Do\n");
            break;
        default:
            break;
    }
}

/*! \brief Disp_Hour_Temp -> Print hour nad temperature on the LCD
 *
 *  This funcion substract the information of the time of RTC
 *  and the information of the sensor and print it on the LCD
 *
 * \param *Time Is a pointer to contains the information of the hour in the RTC
 * \param temp, Is the value recieved from the sensor 
 * \return None
 */
void Disp_Hour_Temp( RTC_TimeTypeDef *Time, uint8_t temp )
{

    /*  Split the units of each parameter */
    uint8_t hour_tens  = Time->Hours / 10u;
    uint8_t hour_units = Time->Hours - ( hour_tens * 10u );
    uint8_t min_tens   = Time->Minutes / 10u;
    uint8_t min_units  = Time->Minutes - ( min_tens * 10u );
    uint8_t sec_tens   = Time->Seconds / 10u; 
    uint8_t sec_units  = Time->Seconds - ( sec_tens * 10u );
    /*  Split the units of the temperature */
    uint8_t temp_tens  = temp / 10u;
    uint8_t temp_units = temp - ( temp_tens * 10u );
    /*  Set the cursor */
    MOD_LCD_SetCursor( &HLCD, 1, 1);
    /*  Print the hour */
    MOD_LCD_Data( &HLCD, ( hour_tens + '0') );
    MOD_LCD_Data( &HLCD, ( hour_units + '0') );
    MOD_LCD_Data( &HLCD, ':');
    MOD_LCD_Data( &HLCD, ( min_tens + '0') );
    MOD_LCD_Data( &HLCD, ( min_units + '0') );
    MOD_LCD_Data( &HLCD, ':');
    MOD_LCD_Data( &HLCD, ( sec_tens + '0') );
    MOD_LCD_Data( &HLCD, ( sec_units + '0') );
    /*  Leave an space and print temperature */
    MOD_LCD_Data( &HLCD, ' ');
    MOD_LCD_Data( &HLCD, ( temp_tens + '0' ) );
    MOD_LCD_Data( &HLCD, ( temp_units + '0' ) );
    MOD_LCD_Data( &HLCD, ( 'C' ) );

    if ( ( Alarm_Active == SET  ) || ( Alarm_Temp_Active == SET ) )
    {
        MOD_LCD_String( &HLCD, " AT\n");
    }

}

void Disp_Alarm( RTC_AlarmTypeDef *Alarm)
{

    uint8_t hour_tens  = Alarm->AlarmTime.Hours / 10u;
    uint8_t hour_units = Alarm->AlarmTime.Hours - ( hour_tens * 10u );
    uint8_t min_tens   = Alarm->AlarmTime.Minutes / 10u;
    uint8_t min_units  = Alarm->AlarmTime.Minutes - ( min_tens * 10u );

    uint8_t low_tens   = low / 10u;
    uint8_t low_units  = low - ( low_tens * 10u );
    uint8_t up_tens   = up / 10u;
    uint8_t up_units  = up - ( up_tens * 10u );

    /*  Set the cursor */
    MOD_LCD_SetCursor( &HLCD, 1, 0);
    MOD_LCD_String( &HLCD, "TA\n");
    /*  Set the cursor */
    MOD_LCD_SetCursor( &HLCD, 1, 3);
    /*  Print the values of the alarm */
    MOD_LCD_Data( &HLCD, ( hour_tens + '0') );
    MOD_LCD_Data( &HLCD, ( hour_units + '0') );
    MOD_LCD_Data( &HLCD, ':');
    MOD_LCD_Data( &HLCD, ( min_tens + '0') );
    MOD_LCD_Data( &HLCD, ( min_units + '0') );

    MOD_LCD_Data( &HLCD, ' ');
    MOD_LCD_Data( &HLCD, ( low_tens + '0' ) );
    MOD_LCD_Data( &HLCD, ( low_units + '0' ) );
    MOD_LCD_Data( &HLCD, '-');
    MOD_LCD_Data( &HLCD, ( up_tens + '0' ) );
    MOD_LCD_Data( &HLCD, ( up_units + '0' ) );
    MOD_LCD_Data( &HLCD, 'C');

}

/*! \brief Disp_Alarm_On -> Disp the alarm 
 *
 *  Display alarm status on LCD
 *
 * \param *Time Is a pointer to the Structure that contains the hour on the LCD
 * \param counter Is a parameter make blink the LCD
 * \return None
 */
void Disp_Alarm_On( RTC_TimeTypeDef *Time,  uint8_t counter )
{
    /*  Split the units of the parameters */
    uint8_t hour_tens  = Time->Hours / 10;
    uint8_t hour_units = Time->Hours - ( hour_tens * 10u );
    uint8_t min_tens   = Time->Minutes / 10;
    uint8_t min_units  = Time->Minutes - ( min_tens * 10u );
   
    /*  Move the cursor */
    MOD_LCD_SetCursor( &HLCD, 1, 0 ); 
    
    if ( ( counter % 2u ) == 0u ) 
    {

        MOD_LCD_String( &HLCD, "***" );
        MOD_LCD_SetCursor( &HLCD, 1, 3 );
        MOD_LCD_Data( &HLCD, ( hour_tens + '0' ) );
        MOD_LCD_Data( &HLCD, ( hour_units + '0' ) );
        MOD_LCD_Data( &HLCD, (':' ) );
        MOD_LCD_Data( &HLCD, ( min_tens + '0' ) );
        MOD_LCD_Data( &HLCD, ( min_units + '0' ) );
        MOD_LCD_Data( &HLCD,  ' ' );
        MOD_LCD_Data( &HLCD,  ' ' );
        MOD_LCD_Data( &HLCD,  ' ' );
        MOD_LCD_Data( &HLCD,  ' ' );
        MOD_LCD_String( &HLCD, "***" );

    }
    else 
    {

        MOD_LCD_String( &HLCD, "   " );
        MOD_LCD_SetCursor( &HLCD, 1, 3 );
        MOD_LCD_Data( &HLCD, ( hour_tens + '0' ) );
        MOD_LCD_Data( &HLCD, ( hour_units + '0' ) );
        MOD_LCD_Data( &HLCD, (':' ) );
        MOD_LCD_Data( &HLCD, ( min_tens + '0' ) );
        MOD_LCD_Data( &HLCD, ( min_units + '0' ) );
        MOD_LCD_Data( &HLCD,  ' ' );
        MOD_LCD_Data( &HLCD,  ' ' );
        MOD_LCD_Data( &HLCD,  ' ' );
        MOD_LCD_Data( &HLCD,  ' ' );
        MOD_LCD_String( &HLCD, "   " );

    }

}

/*! \brief WeekDay -> Give the day of the week
 *
 *  This fuction give the day of the week based on the day, month and year
 *
 * \param day The day of the month
 * \param month The month of the year
 * \param year The year ( does not matter if it is a leap year )
 * \return An uint16_t from 1(Mon) to 7(Sun) that means the day of the week 
 */
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

