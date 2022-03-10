#include "stm32g0xx.h"
#include <stdint.h>
#include "app_bsp.h"

void HAL_MspInit( void )
{
    
}

void HAL_UART_MspInit( UART_HandleTypeDef *huart)
{

    ( void )huart;
    GPIO_InitTypeDef GPIO_InitStruct;              /*  Declare a structure for GPIOs */

    __HAL_RCC_GPIOA_CLK_ENABLE();                  /*  Enable the clock of port A */
    __HAL_RCC_USART2_CLK_ENABLE();                 /*  Enable the click of the USART2 module */

    /*  Configure pin 2(tx) and pin 3(rx) in alternate mode for USART2 */
    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART2;

    /* Apply this configuration to the port A */
    HAL_GPIO_Init( GPIOA, &GPIO_InitStruct );

    /*  Activate the interrupt of USART2 with priority 1 */
    HAL_NVIC_SetPriority( USART2_LPUART2_IRQn, 1, 0 );
    HAL_NVIC_EnableIRQ( USART2_LPUART2_IRQn );

}
