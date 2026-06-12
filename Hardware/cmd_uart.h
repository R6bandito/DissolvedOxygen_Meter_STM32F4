#ifndef __CMD_UART_H__
#define __CMD_UART_H__


#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"


/* ********************************* */
  #define CMD_UART_TX_PORT          (GPIOA)
  #define CMD_UART_RX_PORT          (GPIOA)
  #define CMD_UART_TX_PIN           (GPIO_PIN_2)
  #define CMD_UART_RX_PIN           (GPIO_PIN_3)
/* ********************************* */

/* ********************************* */
/* 上位机通信串口初始化. */
void Cus_UART_Init( void );

/* 使能上位机通信串口传输. */
void Cus_UART_StartTransfer( void );

void UART2_Printf( const char *format, ... );
/* ********************************* */


#endif /* __CMD_UART_H__ */
