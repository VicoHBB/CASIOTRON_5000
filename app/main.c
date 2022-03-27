#include "stm32g0xx.h"
#include <stdint.h>
#include <string.h>
#include "app_bsp.h"
#include "app_serial.h"
#include "app_clock.h"
#include "queue.h"

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/

int main( void )
{
    uint32_t ticks_serial = 0;
    uint32_t ticks_clock = 0;
    
    HAL_Init();
    Serial_Init();
    Clock_Init();

    ticks_serial = HAL_GetTick();
    ticks_clock = HAL_GetTick();

    while( 1 ) 
    {
        if ( ( HAL_GetTick() - ticks_clock ) >= 20u )
        {
            Clock_Task();
            ticks_clock = HAL_GetTick();
        }
        else if ( ( HAL_GetTick() - ticks_serial ) >= 10u )
        {
            Serial_Task();
            ticks_serial = HAL_GetTick();
        }
        else 
        {
         
        }
    }

    return 0u;
}
