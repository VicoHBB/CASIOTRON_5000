#include "stm32g0b1xx.h"
#include "stm32g0xx.h"
#include <stdint.h>
#include "app_bsp.h"
#include "stm32g0xx_hal_tim.h"

extern UART_HandleTypeDef Uart_Handle;
extern RTC_HandleTypeDef  RTCHandle;
extern TIM_HandleTypeDef  Timer;

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/
/* cppcheck-suppress misra-c2012-8.4; This fuction is declared in a reserved file of HAL library */
void NMI_Handler( void )
{

}

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/
/* cppcheck-suppress misra-c2012-8.4; This fuction is declared in a reserved file of HAL library */
void HardFault_Handler( void )
{
    assert_param( 0u );
}

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/
/* cppcheck-suppress misra-c2012-8.4; This fuction is declared in a reserved file of HAL library */
void SVC_Handler( void )
{

}

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/
/* cppcheck-suppress misra-c2012-8.4; This fuction is declared in a reserved file of HAL library */
void PendSV_Handler( void )
{

}

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/
/* cppcheck-suppress misra-c2012-8.4; This fuction is declared in a reserved file of HAL library */
void SysTick_Handler( void )
{
    HAL_IncTick( );
}

/* cppcheck-suppress misra-c2012-8.4; This fuction is declared in a reserved file of HAL library */
void USART2_LPUART2_IRQHandler( void )         /*  Interruption vector of USART2 */
{ /* Function that performs the interrupt operations of the uart drive  */
    HAL_UART_IRQHandler( &Uart_Handle );
}

/* cppcheck-suppress misra-c2012-8.4; This fuction is declared in a reserved file of HAL library */
void EXTI4_15_IRQHandler( void )
{
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_13 );
}

/* cppcheck-suppress misra-c2012-8.4; This fuction is declared in a reserved file of HAL library */
void RTC_TAMP_IRQHandler ( void )
{
    HAL_RTC_AlarmIRQHandler( &RTCHandle );        
}

/* cppcheck-suppress misra-c2012-8.4; This fuction is declared in a reserved file of HAL library */
void TIM6_DAC_LPTIM1_IRQHandler( void )
{
    HAL_TIM_IRQHandler( &Timer );
}
