#include "stm32g0b1xx.h"
#include "stm32g0xx.h"
#include <stdint.h>
#include "app_bsp.h"

/*  Pins of SPI */
#define CLK    GPIO_PIN_3   /*  Pin_B3 */
#define MISO   GPIO_PIN_4   /*  Pin_B4 */
#define MOSI   GPIO_PIN_5   /*  Pin_B5 */

/*  Pins of LCD */
#define CS     GPIO_PIN_9   /*  Pin_B3 */
#define RST    GPIO_PIN_8   /*  Pin_B4 */
#define RS     GPIO_PIN_6   /*  Pin_B5 */

/* cppcheck-suppress misra-c2012-8.4; This fuction is declared in a reserved file of HAL library */
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

/* cppcheck-suppress misra-c2012-8.4; This fuction is declared in a reserved file of HAL library */
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

/* cppcheck-suppress misra-c2012-8.4; This fuction is declared in a reserved file of HAL library */
void HAL_RTC_MspInit( RTC_HandleTypeDef *RtcHandle )
{

    ( void )RtcHandle;

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

/* cppcheck-suppress misra-c2012-8.4; This fuction is declared in a reserved file of HAL library */
void HAL_SPI_MspInit( SPI_HandleTypeDef *hspi )
{

    ( void )hspi;
    GPIO_InitTypeDef         GPIO_InitStruct;

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_SPI1_CLK_ENABLE();

    GPIO_InitStruct.Pin       = CLK | MISO | MOSI;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF0_SPI1;

    HAL_GPIO_Init( GPIOB, &GPIO_InitStruct );

}

void MOD_LCD_MspInit( LCD_HandleTypeDef *hlcd )
{
    
    ( void )hlcd;

    GPIO_InitTypeDef          GPIO_InitStruct;

    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitStruct.Pin         = CS | RST | RS;
    GPIO_InitStruct.Mode        = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull        = GPIO_NOPULL;
    GPIO_InitStruct.Speed       = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init( GPIOC, &GPIO_InitStruct );

}

/* cppcheck-suppress misra-c2012-8.4; This fuction is declared in a reserved file of HAL library */
void HAL_TIM_Base_MspInit( TIM_HandleTypeDef *htim )
{
    
    ( void )htim;

    __HAL_RCC_TIM6_CLK_ENABLE();

    HAL_NVIC_SetPriority( TIM6_DAC_LPTIM1_IRQn, 0, 0 );
    HAL_NVIC_EnableIRQ( TIM6_DAC_LPTIM1_IRQn );

}

/* cppcheck-suppress misra-c2012-8.4; This fuction is declared in a reserved file of HAL library */
void HAL_I2C_MspInit( I2C_HandleTypeDef* hi2c )
{

    ( void )hi2c;
    GPIO_InitTypeDef           GPIO_InitStruct;
    RCC_PeriphCLKInitTypeDef   PeriphClkInit;

    /*  Enable both clocks */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_I2C1_CLK_ENABLE(); 

    /*  Configuration of the clock for the peripherial*/
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
    PeriphClkInit.I2c1ClockSelection    = RCC_I2C1CLKSOURCE_HSI;

    HAL_RCCEx_PeriphCLKConfig( &PeriphClkInit );

    /*  Configure the pins PB8(SCL) & PB9(SDA) */
    GPIO_InitStruct.Pin       = GPIO_PIN_8 | GPIO_PIN_9;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull      = GPIO_PULLUP; 
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH; 
    GPIO_InitStruct.Alternate = GPIO_AF6_I2C1;

    HAL_GPIO_Init( GPIOB, &GPIO_InitStruct );

}

void MOD_TEMP_MspInit( TEMP_HandleTypeDef *htemp )
{
    /*  Config the pin C5 for the alert */
    ( void )htemp;
    GPIO_InitTypeDef           Alert;

    __HAL_RCC_GPIOC_CLK_ENABLE();

    Alert.Pin       = GPIO_PIN_5;
    Alert.Mode      = GPIO_MODE_INPUT;
    Alert.Pull      = GPIO_PULLUP;
    Alert.Speed     = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init( GPIOC, &Alert );

}
