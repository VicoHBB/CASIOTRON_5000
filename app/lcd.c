#include "app_bsp.h"
#include "lcd.h"
#include "temp.h"

/*! \brief MOD_LCD_Init -> Inits LCD
 *
 *  This function init all the configuration that is going to be used on the LCD
 *
 * \param *hlcd Is a pointer to the structure that contains the parameters of the LCD
 * \return none
 */
void MOD_LCD_Init( LCD_HandleTypeDef *hlcd )
{

    MOD_LCD_MspInit( hlcd );
    HAL_GPIO_WritePin( hlcd->CSPort, hlcd->CSPin, GPIO_PIN_SET );
    HAL_GPIO_WritePin( hlcd->RSTPort, hlcd->RSTPin, GPIO_PIN_RESET );
    HAL_Delay( 2 );
    HAL_GPIO_WritePin( hlcd->RSTPort, hlcd->RSTPin, GPIO_PIN_SET);
    HAL_Delay( 20 );
    MOD_LCD_Command( hlcd, WAKE_UP );                   /*  Send wake up command */
    HAL_Delay( 2 );
    MOD_LCD_Command( hlcd,  WAKE_UP );                  /*  Send wake up command */
    MOD_LCD_Command( hlcd,  WAKE_UP );                  /*  Send wake up command */
    MOD_LCD_Command( hlcd, FUNC_SET );                  /*  Send function set command */
    MOD_LCD_Command( hlcd,  PWR_CTRL );                 /*  Send power control command */
    MOD_LCD_Command( hlcd,  FWR_CTRL );                 /*  Send follower control command */
    HAL_Delay( 200 );
    MOD_LCD_Command( hlcd, 0x70 );                  /*  Send the value of the contrast */
    MOD_LCD_Command( hlcd, DISP_ON );                   /*  Display on */
    MOD_LCD_Command( hlcd, ENTRY_MODE );                /*  Entry mode */
    MOD_LCD_Command( hlcd, CLEAR );                     /*  Clear the screen */
    HAL_Delay( 1 );

}

/*! \brief MOD_LCD_MspInit -> Enable the Pin and Port
 *
 *  This function inits the resources necessaries ( GPIO and Pins ) for LCD
 *
 * \param *hlcd Is a pointer to the structure that contains all the parameters of the LCD
 * \return none
 */
__weak void MOD_LCD_MspInit( LCD_HandleTypeDef *hlcd)
{

    ( void )hlcd;

}

/*! \brief MOD_LCD_Command -> Send a command to the LCD
 *
 *  This function send a commands to control the LCD
 *
 * \param *hlcd Is a pointer to the structure that contains all the parameters of the LCD
 * \param cmd  Is the command that is going to be send
 * \return none
 */
void MOD_LCD_Command( LCD_HandleTypeDef *hlcd, uint8_t cmd )
{

    HAL_GPIO_WritePin( hlcd->CSPort, hlcd->CSPin, GPIO_PIN_RESET );
    HAL_GPIO_WritePin( hlcd->RSPort, hlcd->RSPin, GPIO_PIN_RESET );

    HAL_SPI_Transmit( hlcd->SpiHandler, &cmd, 1, 5000 );

    HAL_GPIO_WritePin( hlcd->CSPort, hlcd->CSPin, GPIO_PIN_SET );

    HAL_Delay( 1 );    /*  The biggest time of the commands is 1ms to involve all we wait this time */

}

/*! \brief MOD_LCD_Data -> Write a character to LCD
 *
 *  This fuction send a one character to the LCD and print it on the actual positio
 *
 * \param *hlcd Is a pointer to the structure that contains all the parameter of LCD
 * \param data Is the character that is going to be write 
 * \return None
 */
void MOD_LCD_Data( LCD_HandleTypeDef *hlcd, uint8_t data )
{

    HAL_GPIO_WritePin( hlcd->CSPort, hlcd->CSPin, GPIO_PIN_RESET );
    HAL_GPIO_WritePin( hlcd->RSPort, hlcd->RSPin, GPIO_PIN_SET );

    HAL_SPI_Transmit( hlcd->SpiHandler, &data, 1, 5000 );

    HAL_GPIO_WritePin( hlcd->CSPort, hlcd->CSPin, GPIO_PIN_SET );
    /*HAL_Delay( 30 );*/
    Delay_us( 30 );

}

/*! \brief MOD_LCD_String -> Write the string on the LCD
 *
 *  This function send a complete string and write on the LCD starting on the actual positio 
 *
 * \param *hlcd Is a pointer to the structure that contains all the configuration of the LCD
 * \param *str  Is a pointer to the first position of the string that is going to be send
 * \return None
 */
void MOD_LCD_String( LCD_HandleTypeDef *hlcd, const char *str )
{

    uint8_t size        = strlen( str );
    const char *str_aux = str;

    HAL_GPIO_WritePin( hlcd->CSPort, hlcd->CSPin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin( hlcd->RSPort, hlcd->RSPin, GPIO_PIN_SET );

    for ( uint8_t i = 0; i <= size; i++)
    {

        /* cppcheck-suppress misra-c2012-11.8; This parameter is a copy of the principal variable so this cannot be modified */
        HAL_SPI_Transmit( hlcd->SpiHandler, ( uint8_t* )str_aux, 1, 5000 );
        str_aux++;
        /*HAL_Delay( 1 );*/
        Delay_us( 30 );

    } /* for ( uint8_t i = 0; i <= size; i++ ) */

    HAL_GPIO_WritePin( hlcd->CSPort, hlcd->CSPin, GPIO_PIN_SET);

}

/*! \brief MOD_LCD_SetCursor -> Go to the position on the LCD
 *
 *  This fuction put the cursor of the LCD, indicates the row and colunm
 *
 * \param *hlcd Is a pointer to the structure that contains all the configuration of the LCD
 * \param row Is the row of the LCD
 * \param col Is the colunm of the LCD
 * \return None
 */
void MOD_LCD_SetCursor( LCD_HandleTypeDef *hlcd, uint8_t row, uint8_t col )
{
    
    uint8_t address = 0u;

    if ( row == 0u ) 
    {
        
        address = col | 0b10000000u ;

        HAL_GPIO_WritePin( hlcd->CSPort, hlcd->CSPin, GPIO_PIN_RESET );
        HAL_GPIO_WritePin( hlcd->RSPort, hlcd->RSPin, GPIO_PIN_RESET );

        HAL_SPI_Transmit( hlcd->SpiHandler, &address, 1, 5000 );

        HAL_GPIO_WritePin( hlcd->CSPort, hlcd->CSPin, GPIO_PIN_SET );
        /*HAL_Delay( 1 );*/
        Delay_us( 32 );

    } /*  End if  */
    else if ( row == 1u ) 
    {

        address = ( col + 64u ) | 0b10000000u;

        HAL_GPIO_WritePin( hlcd->CSPort, hlcd->CSPin, GPIO_PIN_RESET );
        HAL_GPIO_WritePin( hlcd->RSPort, hlcd->RSPin, GPIO_PIN_RESET );

        HAL_SPI_Transmit( hlcd->SpiHandler, &address, 1, 5000 );

        HAL_GPIO_WritePin( hlcd->CSPort, hlcd->CSPin, GPIO_PIN_SET );
        /*HAL_Delay( 1 );*/
        Delay_us( 30 );
        
    } /*  End else if */
    else 
    {
    } /*  End else */

}

/*! \brief Delay_us -> Wait approx 1us
 *
 *  Is a simple loop that waits approximate 1 micro second
 *
 * \param counter Is the us that is going to be wait
 * \return None
 */
void Delay_us( uint32_t counter )
{
    
    uint32_t ticks = 0u;

    while ( ticks < ( counter + 1u ) ) 
    {

        ticks++;

    } /*  End of the while  */

}

