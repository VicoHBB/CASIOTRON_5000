/******************************************************************************
* File:             queue.c
*
* Author:           Victor Hugo Becerril Bueno  
* Created:          03/13/22 
* Description:      Driver for the use of a queue of any type of data
*****************************************************************************/

#include "app_bsp.h"
#include "queue.h"
#include <stdint.h>
#include <string.h>


#define WRONG   0u
#define OK      1u

/*! \brief This function initializes all the parameters of the queue
 *
 *  Put the correct values of each parameter
 *
 * \param QUEUE_HandleTypeDef *hqueue 
 * \return void
 */
void HIL_QUEUE_Init( QUEUE_HandleTypeDef *hqueue )
{

    hqueue->Head  = 0u;
    hqueue->Tail  = 0u;
    hqueue->Empty = 1u;
    hqueue->Full  = 0u;

}

/*! \brief HIL_QUEUE_Write 
 *
 *  This fuction copies a parameter from a void pointer to the queue's Buffer
 *
 * \param *hqueue This is a pointer to the queue's structure
 * \param *data This is a pointer to the information thats is going to be stored
 * \return OK(1) if the information was saved correctly of WRONG(0) if not
 */
uint8_t HIL_QUEUE_Write( QUEUE_HandleTypeDef *hqueue, void *data )
{
    uint8_t status; 
 
    if ( hqueue->Full == 0u )
    {
       /* cppcheck-suppress misra-c2012-18.4; Pointer arithmetic is required for this procces */
       /* cppcheck-suppress misra-c2012-11.5; This cast ensures that the pointer moves 1 byte */
       ( void )memcpy( ( ( uint8_t* )hqueue->Buffer + ( hqueue->Head * hqueue->Size ) ) , data, hqueue->Size ); 
       hqueue->Head  = ( hqueue->Head + 1u ) % hqueue->Elements;
       hqueue->Empty = 0u;
       status        = OK;
       if ( hqueue->Head == hqueue->Tail ) 
       {
           hqueue->Full = 1u;
       }
    }
    else 
    {
        status       = WRONG;
    }
 
    return status;
}

/*! \brief HIL_QUEUE_Read
 *
 *  This function copies a value from queue's Buffer to a void pointer
 *
 * \param *hqueue This is a pointer to the structure of the queue
 * \param *data This is a void pointer where the parameter is going to be stored
 * \return OK(1) if the information was saved correctly of WRONG(0) if not
 */
uint8_t HIL_QUEUE_Read( QUEUE_HandleTypeDef *hqueue, void *data)
{
    uint8_t status;

    if ( hqueue->Empty == 0u )
    {
        /* cppcheck-suppress misra-c2012-18.4; Pointer arithmetic is required for this procces */
        /* cppcheck-suppress misra-c2012-11.5; This cast ensures that the pointer moves 1 byte */
        ( void )memcpy( data, ( ( uint8_t* )hqueue->Buffer + ( hqueue->Tail * hqueue->Size ) ), hqueue->Size );
        hqueue->Tail = ( hqueue->Tail + 1u ) % hqueue->Elements;
        hqueue->Full = 0u;
        status       = OK;
        if( hqueue->Head == hqueue->Tail )
        {
            hqueue->Empty = 1u;
        }
    }
    else 
    {
        status       = WRONG;
    }

    return status;
}

/*! \brief HIL_QUEUE_IsEmpty
 *
 *  Ask if the queue is empty
 *
 * \param *hqueue Is a pointer to the queue's structure
 * \return  1 if the queue is empty or 0 if not
 */
uint8_t HIL_QUEUE_IsEmpty( QUEUE_HandleTypeDef *hqueue)
{
    return hqueue->Empty;
}
