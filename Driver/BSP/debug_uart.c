#include "debug_uart.h"

// ------------------------------------------------------------ //
UART_HandleTypeDef huart_debug;
// ------------------------------------------------------------ //




// ------------------------------------------------------------ //
void debug_uart_Init( void );
// ------------------------------------------------------------ //


#if 1
  #if (__ARMCC_VERSION >= 6010050)            /* 使用AC6编译器时 */
  __asm(".global __use_no_semihosting\n\t");  /* 声明不使用半主机模式 */
  __asm(".global __ARM_use_no_argv \n\t");    /* AC6下需要声明main函数为无参数格式，否则部分例程可能出现半主机模式 */

  #else
  /* 使用AC5编译器时, 要在这里定义__FILE 和 不使用半主机模式 */
  #pragma import(__use_no_semihosting)
#endif

//  struct __FILE
//  {
//      int handle;
         
//  };
//  FILE __stdout;

int _sys_open(const char *name, int openmode) { return -1; }
int _sys_close(int fd) { return 0; }
int _sys_write(int fd, const char *buf, int len) { return len; }  // 若不需要实际输出，直接返回len
int _sys_read(int fd, char *buf, int len) { return 0; }
int _sys_istty(int fd) { return 0; }
int _sys_seek(int fd, int pos) { return 0; }
int _sys_ensure(int fd) { return 0; }
int _sys_flen(int fd) { return 0; }
void _sys_exit(int x) { (void)x; }
void _ttywrch(int ch) {
  (void)ch;  // 空实现
}


 int fputc(int ch, FILE *f) {
     (void)f;
     if ( HAL_UART_Transmit(&huart_debug, (uint8_t*)&ch, 1, 500) != HAL_OK )
     {
       /* Error Handler. */
     }
     return ch;
   }
#endif

void debug_uart_Init( void )
{
  __HAL_RCC_USART1_CLK_ENABLE();

  huart_debug.Instance = USART1;
  huart_debug.Init.BaudRate = 115200;
  huart_debug.Init.Mode = UART_MODE_TX_RX;
  huart_debug.Init.Parity = UART_PARITY_NONE;
  huart_debug.Init.StopBits = UART_STOPBITS_1;
  huart_debug.Init.WordLength = UART_WORDLENGTH_8B;
  
  if ( HAL_UART_Init(&huart_debug) != HAL_OK )
  {
    for( ; ; );
  }

}


void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
  if ( huart->Instance == USART1 )
  {
    GPIO_InitTypeDef GPIO_InitStructure;
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Alternate = GPIO_AF7_USART1;
    GPIO_InitStructure.Pin = GPIO_PIN_9 | GPIO_PIN_10;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    
    HAL_GPIO_Init(GPIOA, &GPIO_InitStructure);
  }
}




