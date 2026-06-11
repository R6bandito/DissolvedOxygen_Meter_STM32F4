#include "cmd_uart.h"
#include "semphr.h"
#include <stdarg.h>
#include <stdio.h>

/* **************************** */
uint8_t uart_rcvByte = 0;
UART_HandleTypeDef huart2;
SemaphoreHandle_t xMutexUart2;
/* **************************** */


/* **************************** */
void Cus_UART_Init( void );
void Cus_UART_StartTransfer( void );
void UART2_Printf( const char *format, ... );
/* **************************** */


void Cus_UART_Init( void )
{
  /* 开时钟. */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_USART2_CLK_ENABLE();

  /* GPIO配置. */
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.Alternate        = GPIO_AF7_USART2;
  GPIO_InitStructure.Mode             = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pin              = CMD_UART_TX_PIN;
  GPIO_InitStructure.Pull             = GPIO_PULLUP;
  GPIO_InitStructure.Speed            = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(CMD_UART_TX_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.Alternate        = GPIO_AF7_USART2;
  GPIO_InitStructure.Mode             = GPIO_MODE_AF_PP;
  GPIO_InitStructure.Pin              = CMD_UART_RX_PIN;
  HAL_GPIO_Init(CMD_UART_RX_PORT, &GPIO_InitStructure);

  /* UART 配置. */
  huart2.Instance                     = USART2;
  huart2.Init.BaudRate                = 115200;
  huart2.Init.HwFlowCtl               = UART_HWCONTROL_NONE;          // 无流控.
  huart2.Init.Mode                    = UART_MODE_TX_RX;              // 全双工模式.
  huart2.Init.StopBits                = UART_STOPBITS_1;              // 1位停止位.
  huart2.Init.WordLength              = UART_WORDLENGTH_8B;           // 8位数据.
  huart2.Init.Parity                  = UART_PARITY_NONE;             // 无奇偶校验.

  HAL_UART_Init(&huart2);

  /* 中断优先级设置. */
  HAL_NVIC_EnableIRQ(USART2_IRQn);
  HAL_NVIC_SetPriority(USART2_IRQn, 8, 0);
}


void Cus_UART_StartTransfer( void )
{
  HAL_UART_Receive_IT(&huart2, &uart_rcvByte, 1);                       // 利用RXNE 逐字节中断接收.
}


void UART2_Printf( const char *format, ... )
{
  /* 获取互斥量. */
  if ( xSemaphoreTake(xMutexUart2, pdMS_TO_TICKS(1000)) == pdTRUE )
  {
    char buffer[64];
    va_list args;
    va_start(args, format);
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
  
    if ( len > 0 )
    {
      HAL_UART_Transmit(&huart2, (const uint8_t *)buffer, len, 20);
    }

    xSemaphoreGive(xMutexUart2);
  }
}




