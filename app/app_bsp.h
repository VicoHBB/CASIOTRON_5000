#ifndef _BSP_H_
#define _BSP_H_

#include "stm32g0xx.h"
#include <stdint.h>
#include <string.h>
#include "queue.h"

/*  Parameters of the structure */
#define NONE  0 
#define TIME  1
#define DATE  2
#define ALARM 3


typedef struct
{  
    uint8_t    msg;      // Type of message
    uint16_t   param_1;   // Hour or day
    uint16_t   param_2;    // Minutes or month
    uint16_t   param_3;   // Seconds or year
}SERIAL_MsgTypeDef;



#endif

