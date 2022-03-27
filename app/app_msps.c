#include "stm32g0xx.h"
#include <stdint.h>
#include "app_bsp.h"

void HAL_MspInit( void )
{

    RCC_OscInitTypeDef   Osc_Init;
    RCC_ClkInitTypeDef   CLK_Init; 

    Osc_Init.OscillatorType      =  RCC_OSCILLATORTYPE_NONE;
    Osc_Init.PLL.PLLState        =  RCC_PLL_ON;
    Osc_Init.PLL.PLLSource       =  RCC_PLLSOURCE_HSI;
    Osc_Init.PLL.PLLM            =  RCC_PLLM_DIV1;
    Osc_Init.PLL.PLLN            =  6;
    Osc_Init.PLL.PLLR            =  RCC_PLLR_DIV2;
    Osc_Init.PLL.PLLP            =  RCC_PLLP_DIV2;
    Osc_Init.PLL.PLLQ            =  RCC_PLLQ_DIV2;

    HAL_RCC_OscConfig ( &Osc_Init);

    CLK_Init.ClockType           = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1; 
    CLK_Init.SYSCLKSource        = RCC_SYSCLKSOURCE_PLLCLK;
    CLK_Init.AHBCLKDivider       = RCC_SYSCLK_DIV1;
    CLK_Init.APB1CLKDivider      = RCC_HCLK_DIV1;

    HAL_RCC_ClockConfig ( &CLK_Init, FLASH_LATENCY_1);
    
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

void HAL_RTC_MspInit(RTC_HandleTypeDef *RtcHandle)
{

    RCC_OscInitTypeDef        RCC_OscInitStruct;
    RCC_PeriphCLKInitTypeDef  RCC_RTCPeriphClkInitStruct; 
    /* Enable cloock LSE */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.LSEState       = RCC_LSE_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_NONE;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);
   
    /*  Select LSE as clk source for RTC  */
    RCC_RTCPeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    RCC_RTCPeriphClkInitStruct.RTCClockSelection    = RCC_RTCCLKSOURCE_LSE;
    HAL_RCCEx_PeriphCLKConfig (&RCC_RTCPeriphClkInitStruct);

    __HAL_RCC_RTC_ENABLE();        /*  Enable clock of RTC */
    __HAL_RCC_RTCAPB_CLK_ENABLE();
    /*  Activate the interruptions */
    HAL_NVIC_SetPriority(RTC_TAMP_IRQn,2,0);
    HAL_NVIC_EnableIRQ(RTC_TAMP_IRQn);

}
