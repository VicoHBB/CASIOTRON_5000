#ifndef LCD_H_
#define LCD_H_

#include "app_bsp.h"

/*  COMMANDS */
#define WAKE_UP        0x30u
#define FUNC_SET       0x39u
#define PWR_CTRL       0x56u
#define FWR_CTRL       0x6Du
#define CONTRAST       0x70u
#define DISP_ON        0x0Cu
#define ENTRY_MODE     0x06u
#define CLEAR          0x01u

typedef struct
{
    SPI_HandleTypeDef       *SpiHandler;
    GPIO_TypeDef            *RSTPort;
    uint32_t                 RSTPin;
    GPIO_TypeDef            *RSPort;
    uint32_t                 RSPin;
    GPIO_TypeDef            *CSPort;
    uint32_t                 CSPin;
    
}LCD_HandleTypeDef;

void MOD_LCD_Init( LCD_HandleTypeDef *hlcd );                  /* Init the configuration of the LCD */
void MOD_LCD_MspInit( LCD_HandleTypeDef *hlcd );               /* Set the ports and pins that is goint to be used */
void MOD_LCD_Command( LCD_HandleTypeDef *hlcd, uint8_t cmd );  /* Send a command to the LCD */
void MOD_LCD_Data( LCD_HandleTypeDef *hlcd, uint8_t data );    /* Sed a character to print on the LCD */
void MOD_LCD_String( LCD_HandleTypeDef *hlcd, const char *str ); /* Send a string to print on the LCD */
void MOD_LCD_SetCursor( LCD_HandleTypeDef *hlcd, uint8_t row, uint8_t col );/* Specifies the cordenates on the LCD*/
void Delay_us( uint32_t counter );                             /* White us */



#endif
