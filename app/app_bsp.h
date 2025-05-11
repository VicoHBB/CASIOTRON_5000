#ifndef BSP_H_
#define BSP_H_

#include "stm32g0xx.h"
#include <stdint.h>
#include <string.h>
#include "queue.h"
#include "lcd.h"
#include "temp.h"

/*  Parameters of the structure */
#define NONE      0u
#define TIME      1u
#define DATE      2u
#define ALARM     3u
#define HEARTBEAT 4u
#define TEMP      5u


typedef struct
{

    uint8_t    msg;      // Type of message
    uint16_t   param_1;   // Hour or day
    uint16_t   param_2;    // Minutes or month
    uint16_t   param_3;   // Seconds or year

}SERIAL_MsgTypeDef;

#endif

