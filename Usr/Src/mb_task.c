#include "mb_task.h"
#include "mb.h"


/* ******************************* */
void cTask_Modbus( void *parameter );
/* ******************************* */


void cTask_Modbus( void *parameter )
{
  /* 该任务用于推进 Modbus 状态机. */
  /* Modbus 协议栈初始化. */
  eMBInit(MB_RTU, CUS_MODBUS_SLAVE_ADDR, 1, 115200, MB_PAR_NONE, 1);
  eMBEnable();

  while(1)
  {
    eMBPoll();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}


