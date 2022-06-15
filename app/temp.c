#include "app_bsp.h"
#include "stm32g0xx_hal_i2c.h"
#include "temp.h"

static uint8_t data_write[ 3 ];

/*! \brief MOD_TEMP_Init Initialize the sensor
 *
 *  This function init all the parameters necesaries for the sensor
 *
 * \param *htemp Is a pointer to the structure that contains all the params of the sensor
 * \return None
 */
void MOD_TEMP_Init( TEMP_HandleTypeDef *htemp )
{

    MOD_TEMP_MspInit( htemp );

    /* Send the initial configuration for the sensor */
    data_write[ 0 ] = CONFIG_REG;     /* Configuration Register */
    data_write[ 1 ] = 0x00;
    data_write[ 2 ] = 0x00;

    HAL_I2C_Master_Transmit( htemp->I2CHandler, MSP_ADRS, data_write, 3, 1000);

}

/*! \brief MOD_TEMP_MspInit Init and configure the ports
 *
 *  This fuction enable and configure the ports that is going to be use by the sensor
 *
 * \param *htemp  Is a pointer to the structure that contains all the parameters of the sensor
 * \return None
 */
__weak void MOD_TEMP_MspInit( TEMP_HandleTypeDef *htemp )
{

    ( void )htemp;

}

/*! \brief MOD_TEMP_Read, read the lectures of the sensor 
 *
 *  This functions send a message to the sensor to know the temperature at this momento
 *
 * \param *htemp Is a pointer to the structure that contains all the parameters of the sensor
 * \return An uint16_t with the lecture of the temperature
 */
uint16_t MOD_TEMP_Read( TEMP_HandleTypeDef *htemp )
{

    uint8_t data_read[ 2 ];
    uint8_t temp;

    data_write[ 0 ] = 0x05; /* Command to  read */

    /* Send the command to the sensor and receive the information */
    HAL_I2C_Master_Transmit( htemp->I2CHandler, MSP_ADRS, data_write, 1, 1000 );
    HAL_I2C_Master_Receive( htemp->I2CHandler, MSP_ADRS, data_read, 2, 1000 );

    /* Convert the lecture */
    if ( ( data_read[ 0 ] & 0x10u ) == 0x10u )         /*  Ta < 0 */
    {
        data_read[ 0 ] = data_read[ 0 ] & 0x0Fu;       /*  Clear the sign */
        temp           = 256u - ( ( data_read[ 0 ] * 16u ) + ( data_read[ 1 ] / 16u ) );
    }
    else 
    {
        temp = ( ( data_read[ 0 ] * 16u ) + ( data_read[ 1 ] / 16u ) );
    }

    return temp;

}

/*! \brief MOD_temp_SetAlarm -> Set the alarm
 *
 *  This fuction adjust the bandwith of the alarm of the sensor
 *
 * \param *htemp This is a pointer to the structure that contains all the parameters of the structure
 * \param lower This is the lower alert limit;
 * \param upper This id the upper alert limit;
 * \return None
 */
void MOD_TEMP_SetAlarm( TEMP_HandleTypeDef *htemp, uint8_t lower, uint8_t upper )
{
    /* Send the lower limit 0x02 */
    data_write[ 0 ] = LOWER;                         
    data_write[ 1 ] = ( ( lower & 0x00FFu ) >> 4u );
    data_write[ 2 ] = ( ( lower & 0x00FFu ) << 4u );

    HAL_I2C_Master_Transmit( htemp->I2CHandler, MSP_ADRS, data_write, 3, 1000 );
    /* Send the upper limit 0x03 */
    data_write[ 0 ] = UPPER;                         
    data_write[ 1 ] = ( ( upper & 0x00FFu ) >> 4u );
    data_write[ 2 ] = ( ( upper & 0x00FFu ) << 4u );

    HAL_I2C_Master_Transmit( htemp->I2CHandler, MSP_ADRS, data_write, 3, 1000 );

    /* Send the upper limit 0x03 */
    data_write[ 0 ] = CRIT;                         
    data_write[ 1 ] = ( ( ( upper + 5u ) & 0x00FFu ) >> 4u );
    data_write[ 2 ] = ( ( ( upper + 5u ) & 0x00FFu ) << 4u );

    HAL_I2C_Master_Transmit( htemp->I2CHandler, MSP_ADRS, data_write, 3, 1000 );
    /* Config the alarm */
    data_write[ 0 ] = CONFIG_REG;                         
    data_write[ 1 ] = 0x00;       // up
    data_write[ 2 ] = 0x08;       // low

    HAL_I2C_Master_Transmit( htemp->I2CHandler, MSP_ADRS, data_write, 3, 1000 );
    
}

/*! \brief MOD_TEMP_DisableAlarm -> Disable the alarm
 *
 *  This fuction disable the alarm at the moment calls it
 *
 * \param *htemp  This is a pointer with all the parameters of the structure
 * \return None
 */
void MOD_TEMP_DisableAlarm( TEMP_HandleTypeDef *htemp )
{
    /* Config the alarm */
    data_write[ 0 ] = CONFIG_REG;                         
    data_write[ 1 ] = 0x00; 
    data_write[ 2 ] = 0x00;

    HAL_I2C_Master_Transmit( htemp->I2CHandler, MSP_ADRS, data_write, 3, 1000 );

}







