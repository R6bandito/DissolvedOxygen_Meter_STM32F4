#ifndef __UART_CMD_TASK_H__
#define __UART_CMD_TASK_H__


#include "FreeRTOS.h"
#include "task.h"


/* ********************** Defins ********************** */
  #define CMD_BUFFER_SIZE                     (20)
  #define CMD_LIST_SIZE                       (5)

  #define READ_CMD_INDEX                      (0)     // 读命令列表索引.
  #define CALIB_ZERO_CMD_INDEX                (1)     // 零点校准命令列表索引.
  #define CALIB_AIR_CMD_INDEX                 (2)     // 空气校准命令列表索引.
  #define RESET_CALIB_CMD_INDEX               (3)     // 恢复默认校准参数命令列表索引.
  #define HELP_CMD_INDEX                      (4)     // 帮助命令列表索引.
/* ********************** Defins ********************** */


/* ******************************* */
void cTask_UartCmdTask( void *pramater );
/* ******************************* */


#endif /* __UART_CMD_TASK_H__ */
