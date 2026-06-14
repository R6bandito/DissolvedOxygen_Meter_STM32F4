#ifndef __UART_CMD_TASK_H__
#define __UART_CMD_TASK_H__



/* ═════════════════════════════════════════════════════════ */
                        /* DEFINES */
  /* 命令接收缓冲区大小及命令列表大小. */
  #define CMD_BUFFER_SIZE                     (20)
  #define CMD_LIST_SIZE                       (5)

  /* 命令列表中各命令索引. */
  #define READ_CMD_INDEX                      (0)     // 读命令列表索引.
  #define CALIB_ZERO_CMD_INDEX                (1)     // 零点校准命令列表索引.
  #define CALIB_AIR_CMD_INDEX                 (2)     // 空气校准命令列表索引.
  #define RESET_CALIB_CMD_INDEX               (3)     // 恢复默认校准参数命令列表索引.
  #define HELP_CMD_INDEX                      (4)     // 帮助命令列表索引.
/* ═════════════════════════════════════════════════════════ */


/* ═════════════════════════════════════════════════════════ */
                      /* API_LIST */
void cTask_UartCmdTask( void *pramater );
/* ═════════════════════════════════════════════════════════ */


#endif /* __UART_CMD_TASK_H__ */
