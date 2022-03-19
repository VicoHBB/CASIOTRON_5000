#include "stm32g0xx.h"
#include <stdint.h>
#include <string.h>
#include "app_bsp.h"
#include "queue.h"
#include "stm32g0xx_hal_uart.h"

/*  States */
#define IDLE         0u
#define READ_MSG     1u
#define CHECK_MSG    2u
#define SET_MSG      3u
#define CLEAN        4u

/*  Commands */
#define AT           10u
#define TIME         11u
#define DATE         12u
#define ALARM        13u 

/*  Flags */
#define BUFFER_OK    99u
#define BUFFER_WRONG 100u

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/
typedef struct  {

    uint8_t command;
    uint8_t param_1;
    uint8_t param_2;
    uint8_t param_3;

} CommandTypeDef;

/*unsigned int    */
/*short*/
/*long */
/*char     */
/*float  */
/*double*/
/*  Almacenar los datos en una cola de espera generica y revisar la tarea del serial cada 10ms */
/*  Presentacion de punteros con ejemplos visuales porrque a los punteros se les da un tipo
 *  Casting a un puntero y su efecto */


void Uart_Init( void );
void Serial_Task( void );
uint8_t CharToInt(char *hex);
uint8_t Read_Buffer( uint8_t *Buffer, CommandTypeDef *Cmd );
uint8_t Procces_Buffer( CommandTypeDef *Cmd );
void Clear_Command( CommandTypeDef *Cmd );
void UART_QUEUE_Init( void );

UART_HandleTypeDef               Uart_Handle;                 /*  Structure for the UART configuration */
CommandTypeDef                   Command_Rx;
CommandTypeDef                   Command_Verify;
/*  Parameters of the queue */
QUEUE_HandleTypeDef     UART_Queue;
uint8_t UART_QUEUE_Buffer[120] = { 0 };  // 1/115200*10 = 86us each 10ms 116.27 

/*  Parameters of the UART */
uint8_t UART_RxBuffer[ 50 ] = { 0 };
uint8_t RxByte              = 0;
/*__IO ITStatus Msg_Rx        = RESET;                    [>  Flag to indicate if Tx is free <]*/


int main( void )
{
    uint32_t ticks_serial = 0;
    
    HAL_Init();
    Uart_Init();
    UART_QUEUE_Init();

    ticks_serial = HAL_GetTick();

    while( 1 ) 
    {
        if ( ( HAL_GetTick() - ticks_serial ) >= 10u )
        {
            Serial_Task();
            ticks_serial = HAL_GetTick();
        }
    }

    return 0u;
}

void Uart_Init( void )
{
    /*  Declare the options to configure the UART2 modules, 9600 baudrate, 8 bits, 1 stop bit, no parity and no flow control */
    Uart_Handle.Instance          = USART2;
    Uart_Handle.Init.BaudRate     = 115200;
    Uart_Handle.Init.WordLength   = UART_WORDLENGTH_8B;
    Uart_Handle.Init.StopBits     = UART_STOPBITS_1;
    Uart_Handle.Init.Parity       = UART_PARITY_NONE;
    Uart_Handle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    Uart_Handle.Init.Mode         = USART_MODE_TX_RX;
    Uart_Handle.Init.OverSampling = UART_OVERSAMPLING_16;
    /*  Initialize uart2 with the above parameter */
    HAL_UART_Init( &Uart_Handle );
    /*  Prepare the reception to rev only one bit */
    HAL_UART_Receive_IT( &Uart_Handle, &RxByte, 1 );
    
}

void HAL_UART_RxCpltCallback( UART_HandleTypeDef *huart ) /*  Callback function that backs up the characters arrived and informs when */
{  
    HIL_QUEUE_Write( &UART_Queue, &RxByte);
    HAL_UART_Receive_IT( &Uart_Handle, &RxByte, 1 );
}

void Serial_Task( void )
{
    /*  States of the machine */
    static uint8_t SERIAL_STATE = IDLE;
    /*  Flags to chech */
    uint8_t syntax;
    uint8_t validity;
    /*  Messages */
    const uint8_t Ok_Msg[ 11 ]      = "-OK...\r";
    const uint8_t Error_Msg_1[ 16 ] = "-ERROR syntax \r";
    const uint8_t Error_Msg_2[ 16 ] = "-ERROR values \r";
    /*  Index for the buffer of the UART */
    uint8_t Idx                     = 0;
    uint8_t Byte_Recived;
    

    switch ( SERIAL_STATE ) 
    { 

        case IDLE:

            while ( HIL_QUEUE_IsEmpty( &UART_Queue ) == 0u )
            {
                HAL_NVIC_DisableIRQ( USART2_LPUART2_IRQn );        /*  Disable the interruption */
                HIL_QUEUE_Read( &UART_Queue, &Byte_Recived ); 
                HAL_NVIC_EnableIRQ( USART2_LPUART2_IRQn );         /*  Enable the interruption again */

                if ( Byte_Recived == (uint8_t)'\r' ) 
                {
                    Idx = 0;
                    SERIAL_STATE = READ_MSG;
                    break;
                }
                else 
                {
                    UART_RxBuffer[ Idx ] = Byte_Recived;
                    Idx++;
                }
            }

            break;

        case READ_MSG:

            syntax = Read_Buffer( &UART_RxBuffer[0], &Command_Rx );

            if ( syntax == BUFFER_OK )
            {
                SERIAL_STATE = CHECK_MSG;
            }
            else 
            {
                HAL_UART_Transmit( &Uart_Handle, ( uint8_t* )Error_Msg_1, sizeof( Error_Msg_1 ), 5000 );
                SERIAL_STATE = CLEAN;
            }

            break;

        case CHECK_MSG:

            validity = Procces_Buffer( &Command_Rx );

            if ( validity == BUFFER_OK )
            {
                SERIAL_STATE = SET_MSG;
            }
            else 
            {
                HAL_UART_Transmit( &Uart_Handle, ( uint8_t* )Error_Msg_2, sizeof( Error_Msg_2 ), 5000 );
                SERIAL_STATE = CLEAN;
            }

            break;

        case SET_MSG:

            HAL_UART_Transmit( &Uart_Handle, ( uint8_t* )Ok_Msg, sizeof( Ok_Msg ), 5000 );
            Command_Verify = Command_Rx;
            SERIAL_STATE = CLEAN;

            break;

        case CLEAN:

            Clear_Command( &Command_Rx );
            memset( &UART_RxBuffer[0], 0, 50);
            SERIAL_STATE = IDLE;

            break;

        default:

            SERIAL_STATE = IDLE;

            break;

    }

}

uint8_t CharToInt(char *hex) {

    char *hex_aux    = hex;
    uint8_t val      = 0;
    uint8_t i        = 0;
    uint8_t index[2] = { 10, 1 };
    
    while ( *hex_aux != '\0' ) 
    {
        val += ( ( uint8_t )*hex_aux - ( uint8_t )'0')*(index[i] ); 
        hex_aux++;
        i++;
    }

    return val;

}
uint8_t Read_Buffer( uint8_t *Buffer, CommandTypeDef *Cmd )
{

    uint8_t status;             /*  Status of the buffer */
    /*  Varibales to save data */
    char *at;
    char *param;
    char *value;
    /*  Read the information of the buffer */
    at = strtok( ( char * )Buffer, "+" );
    /*  Check the first part of the command */
    if ( strcmp( at, "AT" ) == 0 ) 
    {
        /*  Read the second information of the buffer */
        param = strtok( NULL, "=" );
        /*  Check and classify the next part of the command */
        if ( strcmp( param, "TIME" ) == 0 )
        {
            Cmd->command = TIME;                   /*  Assign the command */
            /*  Store the params */
            value        = strtok( NULL, "," );
            Cmd->param_1 = CharToInt( value );
            value        = strtok( NULL, "," );
            Cmd->param_2 = CharToInt( value );
            value        = strtok( NULL, "\r" );
            Cmd->param_3 = CharToInt( value );

            /*  Set the flag of OK */
            status = BUFFER_OK;
        }
        else if ( strcmp( param, "DATE" ) == 0 )
        {
            Cmd->command = DATE;                   /*  Assign the command */
            /*  Store the params */
            value        = strtok( NULL, "," );
            Cmd->param_1 = CharToInt( value );
            value        = strtok( NULL, "," );
            Cmd->param_2 = CharToInt( value );
            value        = strtok( NULL, "\r" );
            value++;
            value++;
            Cmd->param_3 = CharToInt( value );

            /*  Set the flag of OK */
            status = BUFFER_OK;
        }
        else if ( strcmp( param, "ALARM" ) == 0 )
        {
            Cmd->command = ALARM;                   /*  Assign the command */
            /*  Store the params */
            value        = strtok( NULL, "," );
            Cmd->param_1 = CharToInt( value );
            value        = strtok( NULL, "\r" );
            Cmd->param_2 = CharToInt( value );

            /*  Set the flag of OK */
            status = BUFFER_OK;
        }
        else 
        {
            status = BUFFER_WRONG;
        }
        
    }
    else 
    {
        status = BUFFER_WRONG;
    }

    return status;

}

uint8_t Procces_Buffer( CommandTypeDef *Cmd )
{

    uint8_t status;

    switch ( Cmd->command )
    {
        case TIME:
            if ( ( Cmd->param_1 <= 24u ) && ( Cmd->param_2 <= 59u ) && ( Cmd->param_3 <= 59u ) )
            {
                status = BUFFER_OK;
            }
            else 
            {
                status = BUFFER_WRONG;
            }
            break;
        case DATE:
            if ( Cmd->param_2 <= 12u )
            {
                switch ( Cmd->param_2 )
                {
                    case 1:
                    case 3:
                    case 5:
                    case 7:
                    case 8:
                    case 10:
                    case 12:               /*  Months with 31 days */
                        if ( Cmd->param_1 <= 31u )
                        {
                            status = BUFFER_OK;
                        }
                        else 
                        {
                            status = BUFFER_WRONG;
                        }
                        break;
                    case 4:
                    case 6:
                    case 9:
                    case 11:               /*  Months with 30 days */
                        if ( Cmd->param_1 <= 30u )
                        {
                            status = BUFFER_OK;
                        }
                        else 
                        {
                            status = BUFFER_WRONG;
                        }
                        break;
                    case 2:                /*  February depends of the year */
                        if ( ( Cmd->param_3 % 4 ) == 0u )
                        {
                            if ( Cmd->param_1 <= 29u  )
                            {
                                status = BUFFER_OK;
                            }
                            else 
                            {
                                status = BUFFER_WRONG;
                            }
                        }
                        else 
                        {
                            if ( Cmd->param_1 <= 28u  )
                            {
                                status = BUFFER_OK;
                            }
                            else 
                            {
                                status = BUFFER_WRONG;
                            }
                        }
                        break;
                    default:
                        status = BUFFER_WRONG;
                        break;
                }
            }
            else 
            {
                status = BUFFER_WRONG;
            }
            break;
        case ALARM:
            if ( ( Cmd->param_1 <= 24u ) && ( Cmd->param_2 <= 59u ) )
            {
                status = BUFFER_OK;
            }
            else 
            {
                status = BUFFER_WRONG;
            }
            break;
        default:
            status = BUFFER_WRONG;
            break;
    }

    return  status;

}


void Clear_Command( CommandTypeDef *Cmd )
{

    Cmd->command = 0;
    Cmd->param_1 = 0;
    Cmd->param_2 = 0;
    Cmd->param_3 = 0;
    
}

void UART_QUEUE_Init( void )
{

   UART_Queue.Buffer   = ( void* )UART_QUEUE_Buffer;
   UART_Queue.Elements = 120;
   UART_Queue.Size     = sizeof( uint8_t );

   HIL_QUEUE_Init( &UART_Queue );

}
