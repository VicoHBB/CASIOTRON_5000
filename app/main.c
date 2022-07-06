#include "app_bsp.h"
#include "app_serial.h"
#include "app_clock.h"

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/

void Heart_Init( void );       /*  Config the pint that is going to be the heart beat */
void Heart_Beat( uint32_t ticks );       /*  Blink the Heart Beat */
void Dog_Init( void );         /*  Init the watchdog */
void Peth_The_Dog( void );     /*  Refresh the watchdog */
void Timer_Init( void );       
uint32_t Timer_GetTick( void );

extern uint16_t delay_heart_beat;

static WWDG_HandleTypeDef      WWDG_Init;

/* cppcheck-suppress misra-c2012-8.4; This fuction variable is required in app_ints */
TIM_HandleTypeDef      Timer;

static uint32_t TimerTicks = 0;

int main( void )
{
    /*  Variables for scheduler */
    uint32_t ticks_serial = 0u;
    uint32_t ticks_clock  = 0u;
    uint32_t ticks_heart  = 0u;
    uint32_t ticks_wd     = 0u;
    
    /*  Init the HAL */
    HAL_Init();
    /*  Init all the proccess */
    Serial_Init();
    Clock_Init();
    Heart_Init();
    Timer_Init();
    Dog_Init();

    /*  Take the first ticks of each proccess */
    ticks_serial = Timer_GetTick();
    ticks_clock  = Timer_GetTick();
    ticks_heart  = Timer_GetTick();

    while( 1 ) 
    {
        if ( ( Timer_GetTick() - ticks_clock ) >= 2u )
        {
            Clock_Task();
            ticks_clock = Timer_GetTick();
        }
        else if ( ( Timer_GetTick() - ticks_serial ) >= 1u )
        {
            Serial_Task();
            ticks_serial = Timer_GetTick();
        }
        else if ( ( Timer_GetTick() - ticks_heart ) >= 5u ) 
        {
            Heart_Beat( Timer_GetTick() );
            ticks_heart = Timer_GetTick();
        }
        else if ( ( Timer_GetTick() - ticks_wd ) >= 1u )
        {
            Peth_The_Dog();
            ticks_wd = Timer_GetTick();
        }
        else 
        {
            ticks_wd = Timer_GetTick();
        }
    }

    return 0u;
}

/*! \brief Heart_Init -> Config a pin as heartbeat 
 *
 *  Configure pin 5 of port A as an output 
 *
 * \return None
 */
void Heart_Init( void )
{

    GPIO_InitTypeDef          GPIOA_InitStruct;       /*  Structure for GPIOA */
    
    __HAL_RCC_GPIOA_CLK_ENABLE();                     /*  Enable the clock of port A */

    GPIOA_InitStruct.Pin    = GPIO_PIN_5;             /*  Configure pin 5 for the LED */
    GPIOA_InitStruct.Mode   = GPIO_MODE_OUTPUT_PP;    /*  Configure the pin as an output  */
    GPIOA_InitStruct.Pull   = GPIO_NOPULL;            /*  Configure as not pull */
    GPIOA_InitStruct.Speed  = GPIO_SPEED_FREQ_HIGH;   /*  Configure high speed */

    /*  Apply the configurations */
    HAL_GPIO_Init( GPIOA, &GPIOA_InitStruct );
    HAL_GPIO_WritePin( GPIOA, GPIO_PIN_5, GPIO_PIN_RESET );

}

/*! \brief Timer_Init
 *
 *  This fuction set all the parameter of the timer 
 *
 * \return None
 */
void Timer_Init( void )
{

    Timer.Instance         = TIM6;
    Timer.Init.CounterMode = TIM_COUNTERMODE_UP;
    Timer.Init.Prescaler   = 79;
    Timer.Init.Period      = 5999;

    HAL_TIM_Base_Init( &Timer );
    /*  Start the TIM6 with the interruption */
    HAL_TIM_Base_Start_IT( &Timer );

}

/*! \brief Dog_Init -> Config the whatchdog
 *
 *  This fuction init all the parameters to use the whatchdog
 *
 * \return None
 */
void Dog_Init( void )
{

    WWDG_Init.Instance          = WWDG;
    WWDG_Init.Init.Prescaler    = WWDG_PRESCALER_32;   // f = ( PCLK1 / ( 4096 * 2^Pres[2:0] ) ) PCLK1 out of APB
                                                       // f = 366.2109375Hz  t = 2.730666667ms 
    WWDG_Init.Init.Window       = 120;               // 64 Window  100 Delay 127; do the refresh   64| |90| Cannot do the refresh  |127  // calcularlo a 10 y 30 
    WWDG_Init.Init.Counter      = 127;                     // Counter of 127 bits
    WWDG_Init.Init.EWIMode      = WWDG_EWI_DISABLE;        // 27 to 100 its 74.79ms  32 to 100 its 87
    HAL_WWDG_Init ( &WWDG_Init );  

}

/*! \brief Heart_Beat -> This function make the heart beat 
 *
 *  This fuction toggle the state of the pin config as heart beat
 *
 * \param ticks Is the time elapsed untill the fuction is called 
 * \return None
 */
void Heart_Beat( uint32_t ticks )
{
    if ( ( Timer_GetTick() - ticks ) >= ( delay_heart_beat / 10u ) ) 
    {
        HAL_GPIO_TogglePin( GPIOA, GPIO_PIN_5 );
        ticks = Timer_GetTick();
    }
}

/*! \brief Peth_The_Dog -> Refresh the whatchdog
 *
 *  Refresh the watchdog timer
 *
 * \return None
 */
void Peth_The_Dog( void )
{
    HAL_WWDG_Refresh( &WWDG_Init );
}

/*! \brief Timer_GetTick
 *
 *  Detailed description of the function
 *
 * \return None
 */
uint32_t Timer_GetTick( void )
{
    return TimerTicks;
}

/*! \brief HAL_TIM_CALLBACK
 *
 *  Detailed description of the function
 * \param htim Is the Structure that contains the information of the proccess which produces the interruption
 * \return Return parameter description
 */
/*  cppcheck-suppress misra-c2012-8.4; This fuction is declared in a reserved file of HAL library */
void HAL_TIM_PeriodElapsedCallback( TIM_HandleTypeDef *htim )
{
    ( void )htim;
    TimerTicks++;
}
