#ifndef  TEMP_H_
#define  TEMP_H_

#include "app_bsp.h"

#define CONFIG_REG 0x01u
#define MSP_ADRS   ( 0x1Fu << 1u )
#define UPPER      0x02u
#define LOWER      0x03u
#define CRIT       0x04u

typedef struct
{

    I2C_HandleTypeDef        *I2CHandler;
    GPIO_TypeDef             *AlertPort;
    uint32_t                  AlertPin;

}TEMP_HandleTypeDef;

void MOD_TEMP_Init( TEMP_HandleTypeDef *htemp );
void MOD_TEMP_MspInit( TEMP_HandleTypeDef *htemp );
uint16_t MOD_TEMP_Read( TEMP_HandleTypeDef *htemp );
void MOD_TEMP_SetAlarm( TEMP_HandleTypeDef *htemp, uint8_t lower, uint8_t upper );
void MOD_TEMP_DisableAlarm( TEMP_HandleTypeDef *htemp );

#endif /*  ifndef TEMP_H_ */
