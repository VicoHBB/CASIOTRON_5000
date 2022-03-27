#include "stm32g0xx.h"
#include <stdint.h>
#include "app_bsp.h"

extern UART_HandleTypeDef Uart_Handle;
extern RTC_HandleTypeDef  RTCHandle;

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/
void NMI_Handler( void )
{

}

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/
void HardFault_Handler( void )
{
    assert_param( 0u );
}

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/
void SVC_Handler( void )
{

}

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/
void PendSV_Handler( void )
{

}

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/
void SysTick_Handler( void )
{
    HAL_IncTick( );
}

void USART2_LPUART2_IRQHandler( void )         /*  Interruption vector of USART2 */
{ /* Function that performs the interrupt operations of the uart drive  */
    HAL_UART_IRQHandler( &Uart_Handle );
}

void EXTI4_15_IRQHandler( void )
{
    HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_13 );
}

void RTC_TAMP_IRQHandler ( void )
{
    HAL_RTC_AlarmIRQHandler( &RTCHandle );        
}
