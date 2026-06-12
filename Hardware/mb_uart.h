#ifndef __MB_UART_H__
#define __MB_UART_H__


#include "stm32f4xx_hal.h"


/* ************* Defines ************* */
  #define MB_UART_RX_TX_PORT        (GPIOC)
  #define MB_UART_RX_PIN            (GPIO_PIN_11)
  #define MB_UART_TX_PIN            (GPIO_PIN_10)
/* ************* Defines ************* */


/* ************************************ */
void Cus_Mb_UART_Init( void );
/* ************************************ */


#endif /* __MB_UART_H__ */

