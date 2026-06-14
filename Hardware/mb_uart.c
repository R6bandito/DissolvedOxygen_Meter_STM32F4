/* ═══════════════════════════════════════ */
              /* INCLUDE */
#include "mb_uart.h"
#include "stm32f4xx_hal.h"
/* ═══════════════════════════════════════ */


/* ═══════════════════════════════════════ */
              /* 全局变量 */
UART_HandleTypeDef huart_mb;
/* ═══════════════════════════════════════ */


/* ═══════════════════════════════════════ */
              /* Public_API */
void HX_MB_GPIO_Init( void );
/* ═══════════════════════════════════════ */


/* ————————————————————————————— Modbus GPIO Init ————————————————————————————— */
void HX_MB_GPIO_Init( void )
{
  __HAL_RCC_GPIOC_CLK_ENABLE();

  GPIO_InitTypeDef InitStrucutre;

  /* Modbus通信串口初始化. */
  InitStrucutre.Alternate               = GPIO_AF7_USART3;
  InitStrucutre.Mode                    = GPIO_MODE_AF_PP;
  InitStrucutre.Pin                     = MB_UART_RX_PIN | MB_UART_TX_PIN;
  InitStrucutre.Pull                    = GPIO_PULLUP;
  InitStrucutre.Speed                   = GPIO_SPEED_FREQ_HIGH;

  HAL_GPIO_Init(MB_UART_RX_TX_PORT, &InitStrucutre);
}


/* ————————————————————————————— Modbus UART Init ————————————————————————————— */
void Cus_Mb_UART_Init( void )
{
  /* 初始化引脚. */
  HX_MB_GPIO_Init();

  __HAL_RCC_USART3_CLK_ENABLE();

  /* 串口参数. */
  huart_mb.Instance                 = USART3;
  huart_mb.Init.BaudRate            = 115200;
  huart_mb.Init.HwFlowCtl           = UART_HWCONTROL_NONE;
  huart_mb.Init.Mode                = UART_MODE_TX_RX;
  huart_mb.Init.Parity              = UART_PARITY_NONE;
  huart_mb.Init.StopBits            = UART_STOPBITS_1;
  huart_mb.Init.WordLength          = UART_WORDLENGTH_8B;

  HAL_UART_Init(&huart_mb);

  /* 使能NVIC中断线. */
  HAL_NVIC_EnableIRQ(USART3_IRQn);
  HAL_NVIC_SetPriority(USART3_IRQn, 6, 0);
}
