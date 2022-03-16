/******************************************************************************
* File:             queue.h
*
* Author:           Victor Hugo Becerril Bueno  
* Created:          03/13/22 
* Description:      TDriver for the use of a queue of any type of data
*****************************************************************************/

#ifndef QUEUE_H_
#define QUEUE_H_

#include "app_bsp.h"
#include <stdint.h>
#include <string.h>

typedef struct{

    void        *Buffer;
    uint32_t    Elements;
    uint8_t     Size;
    uint32_t    Head;
    uint32_t    Tail;
    uint8_t     Empty;
    uint8_t     Full;

}QUEUE_HandleTypeDef;


void HIL_QUEUE_Init( QUEUE_HandleTypeDef *hqueue );                 /* Initialize the queue */
uint8_t HIL_QUEUE_Write( QUEUE_HandleTypeDef *hqueue, void *data ); /* Write a element in the queue */
uint8_t HIL_QUEUE_Read( QUEUE_HandleTypeDef *hqueue, void *data );  /* Read a element in the queue */
uint8_t HIL_QUEUE_IsEmpty( QUEUE_HandleTypeDef *hqueue );           /* Ask for the content of the queue */

#endif
