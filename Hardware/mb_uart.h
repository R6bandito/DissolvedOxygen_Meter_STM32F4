#ifndef __MB_UART_H__
#define __MB_UART_H__


/* ═════════════════════════════════════════════════════════ */
                        /* DEFINES */
  /* Modbus 通信串口引脚及端口映射. */
  #define MB_UART_RX_TX_PORT        (GPIOC)
  #define MB_UART_RX_PIN            (GPIO_PIN_11)
  #define MB_UART_TX_PIN            (GPIO_PIN_10)
/* ═════════════════════════════════════════════════════════ */


/* ═════════════════════════════════════════════════════════ */
                      /* API_LIST */
void Cus_Mb_UART_Init( void );
/* ═════════════════════════════════════════════════════════ */


#endif /* __MB_UART_H__ */

