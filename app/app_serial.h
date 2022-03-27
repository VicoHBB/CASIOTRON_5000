#ifndef APP_SERIAL_H_
#define APP_SERIAL_H_ 

#include "app_bsp.h"

/*  States of UART */
#define IDLE         20u
#define READ_MSG     21u
#define CHECK_MSG    22u
#define SET_MSG      23u
#define CLEAN        24u

/*  Flags */
#define BUFFER_OK    99u
#define BUFFER_WRONG 100u


void Serial_Init( void );
void Uart_Init( void );
void Serial_Task( void );
uint8_t CharToInt( char *hex );
uint8_t Read_Buffer( uint8_t *Buffer, SERIAL_MsgTypeDef *Msg );
uint8_t Procces_Buffer( SERIAL_MsgTypeDef *Cmd );
void Clear_Command( SERIAL_MsgTypeDef *Cmd );
void UART_QUEUE_Init( void );
void Msg_Queue_Init( void );


#endif

