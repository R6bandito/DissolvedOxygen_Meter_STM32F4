#include "uart_cmd_task.h"
#include "cmd_uart.h"
#include "adc_task.h"
#include "queue.h"
#include "do_data.h"
#include <string.h>


/* ************************************ */
uint8_t cmd_buf[CMD_BUFFER_SIZE];                // 串口命令缓冲区.

const char *cmd_list[CMD_LIST_SIZE] = 
{
  "READ",                                        // 读命令.
  "CALIB_ZERO",                                  // 零点校准.
  "CALIB_AIR",                                   // 空气校准.
  "RESET_CALIB",                                 // 恢复默认校准参数.
  "HELP"                                         // 帮助(命令帮助页).
};


extern QueueHandle_t doDataQueue;

/* Declare */
void cTask_UartCmdTask( void *pramater );
/* ************************************ */


void cTask_UartCmdTask( void *pramater )
{
  static doData_t rcv_data = { 0 };

  while(1)
  {
    /* 等待信号量. */
    ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1500));

    /* 为防止命令响应期间重复命令干扰. 关闭CMD_Uart中断. */
    HAL_NVIC_DisableIRQ(USART2_IRQn);

    /* 遍历命令列表查命令. */
    if ( strcmp((const char *)cmd_buf, cmd_list[READ_CMD_INDEX]) == 0 )
    {
      /* 数据读取命令. */
      BaseType_t pReturn = xQueuePeek(doDataQueue, &rcv_data, pdMS_TO_TICKS(200));
      if ( pReturn != pdPASS )
      {
        /* 读取错误. 输出信息. 跳过该次命令. */
        UART2_Printf("READ Err. Plz try again later.\n");
        memset(cmd_buf, 0, sizeof(cmd_buf));
        goto REOPEN;
      }

      UART2_Printf("DO=%.2f mg/L, T=%.1f C\r\n", rcv_data.dissolved_oxygen, rcv_data.temperature);
      memset(cmd_buf, 0, sizeof(cmd_buf));
      goto REOPEN;
    }
    else if ( strcmp((const char *)cmd_buf, cmd_list[CALIB_ZERO_CMD_INDEX]) == 0 )
    {
      /* 零点校准. */
      UART2_Printf("ZERO_CALIB Will Start in 3 Sec...\n");

      vTaskDelay(pdMS_TO_TICKS(3000));

      calib_zero();

      UART2_Printf("CALIB Sample OK!\n");

      memset(cmd_buf, 0, sizeof(cmd_buf));    goto REOPEN;
    }
    else if ( strcmp((const char *)cmd_buf, cmd_list[CALIB_AIR_CMD_INDEX]) == 0 )
    {
      /* 空气校准. */
      UART2_Printf("AIR_CALIB Will Start in 3 Sec...\n");

      vTaskDelay(pdMS_TO_TICKS(3000));

      calib_air();

      UART2_Printf("AIR_CALIB Sample OK!\n");

      memset(cmd_buf, 0, sizeof(cmd_buf));    goto REOPEN;
    }
    else if ( strcmp((const char *)cmd_buf, cmd_list[RESET_CALIB_CMD_INDEX]) == 0 )
    {
      /* 恢复默认校准参数. */
      calib_defaultInit();

      /* 删除位于BKP SRAM中的保存校准参数(预留) */

      UART2_Printf("CALIB Params has been Reset!\n");

      memset(cmd_buf, 0, sizeof(cmd_buf));    goto REOPEN;
    }
    else if ( strcmp((const char *)cmd_buf, cmd_list[HELP_CMD_INDEX]) == 0 )
    {
      /* 帮助. */
      UART2_Printf("READ         - read dissolved oxygen and temperature\r\n");
      UART2_Printf("CALIB_ZERO   - zero calibration (0 mg/L)\r\n");
      UART2_Printf("CALIB_AIR    - air calibration (saturated DO at current temp)\r\n");
      UART2_Printf("RESET_CALIB  - restore default calibration parameters\r\n");
      UART2_Printf("HELP         - show this help\r\n");

      memset(cmd_buf, 0, sizeof(cmd_buf));
    }

REOPEN:    
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  }
}

