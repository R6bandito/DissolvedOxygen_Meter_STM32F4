#include "uart_cmd_task.h"
#include "cmd_uart.h"
#include "adc_task.h"
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
extern UART_HandleTypeDef huart2; 

/* Declare */
void cTask_UartCmdTask( void *pramater );
/* ************************************ */


void cTask_UartCmdTask( void *pramater )
{

  while(1)
  {
    /* 等待信号量. */
    ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(1500));

    /* 遍历命令列表查命令. */
    if ( strcmp((const char *)cmd_buf, cmd_list[READ_CMD_INDEX]) == 0 )
    {
      /* 数据读取命令. */
      /* 从数据队列中取出一帧数据报文进行显示. 待实现. */

    }
    else if ( strcmp((const char *)cmd_buf, cmd_list[CALIB_ZERO_CMD_INDEX]) == 0 )
    {
      /* 零点校准. */
      UART2_Printf("ZERO_CALIB Will Start in 3 Sec...\n");

      vTaskDelay(pdMS_TO_TICKS(3000));

      calib_zero();

      UART2_Printf("CALIB Sample OK!\n");
    }
    else if ( strcmp((const char *)cmd_buf, cmd_list[CALIB_AIR_CMD_INDEX]) == 0 )
    {
      /* 空气校准. */
      UART2_Printf("AIR_CALIB Will Start in 3 Sec...\n");

      vTaskDelay(pdMS_TO_TICKS(3000));

      calib_air();

      UART2_Printf("AIR_CALIB Sample OK!\n");
    }
    else if ( strcmp((const char *)cmd_buf, cmd_list[RESET_CALIB_CMD_INDEX]) == 0 )
    {
      /* 恢复默认校准参数. */
    }
    else if ( strcmp((const char *)cmd_buf, cmd_list[HELP_CMD_INDEX]) == 0 )
    {
      /* 帮助. */
    }
  }
}

