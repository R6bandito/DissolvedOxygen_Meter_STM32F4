#include "task_guard.h"
#include "create_task.h"
#include "cmd_uart.h"
#include "led.h"


/* ********************************* */
static taskInfo_t taskInfoInstance[CTASK_TOTAL];

void cTask_Guard( void *parameter );
/* ********************************* */


/* 护卫任务. 间隔 3s 对所有任务进行一次巡检. */
void cTask_Guard( void *parameter )
{
  /* 将所有任务进行注册. */
  taskInfoInstance[0] = (taskInfo_t){ .Handle = getADCTask_Handle(),      .stackSize = CUS_ADC_TASK_DEEPTH };
  taskInfoInstance[1] = (taskInfo_t){ .Handle = getUpdateTask_Handle(),   .stackSize = CUS_UPDATE_TASK_DEEPTH };
  taskInfoInstance[2] = (taskInfo_t){ .Handle = getUartCmdTask_Handle(),  .stackSize = CUS_UART_CMD_TASK_DEEPTH };
  taskInfoInstance[3] = (taskInfo_t){ .Handle = getKeyTask_Handle(),      .stackSize = CUS_KEY_TASK_DEEPTH };
  taskInfoInstance[4] = (taskInfo_t){ .Handle = getLcdTask_Handle(),      .stackSize = CUS_LCD_TASK_DEEPTH };
  taskInfoInstance[5] = (taskInfo_t){ .Handle = getModbusTask_Handle(),   .stackSize = CUS_MODBUS_TASK_DEEPTH };
  taskInfoInstance[6] = (taskInfo_t){ .Handle = getGuardTask_Handle(),    .stackSize = CUS_GUARD_TASK_DEEPTH };

  static uint8_t ledPin = 0;
  TickType_t xLastWakeUpTime = xTaskGetTickCount();

  while(1)
  {
    for( uint8_t index = 0; index < CTASK_TOTAL; index++ )
    {
      /* 遍历每一个任务. 依次获取信息. */
      if ( taskInfoInstance[index].Handle )
      {
        taskInfoInstance[index].taskName = pcTaskGetName(taskInfoInstance[index].Handle);
        taskInfoInstance[index].priority = uxTaskPriorityGet(taskInfoInstance[index].Handle);
        taskInfoInstance[index].status = eTaskGetState(taskInfoInstance[index].Handle);
        taskInfoInstance[index].minimumStackSize = uxTaskGetStackHighWaterMark(taskInfoInstance[index].Handle);
      }
    }

    for( uint8_t index = 0; index < CTASK_TOTAL; index++ )
    {
      if ( taskInfoInstance[index].Handle && taskInfoInstance[index].stackSize > 0 )
      {
        /* 计算剩余栈空间百分比. */
        uint32_t remainPercent = (taskInfoInstance[index].minimumStackSize * 100) / taskInfoInstance[index].stackSize;
        if ( remainPercent < 10 )
        {
          /* 剩余栈空间小于10%. 翻转指示灯告警. */
          ledPin ^= RUN_WR_LED_ON;
          runWrLEDSwitch(ledPin);

          /* 通过命令串口回送警告信息. */
          UART2_Printf("[GUARD] LOW STACK: %s %u/%u words remaining\n", taskInfoInstance[index].taskName, taskInfoInstance[index].minimumStackSize, taskInfoInstance[index].stackSize);
          
        }
      }
    }

    vTaskDelayUntil(&xLastWakeUpTime, pdMS_TO_TICKS(3000));
  }
}



/* 任务栈空间溢出检测钩子. */
void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
  /* 关闭所有中断，防止后续打印被打断 */
  taskDISABLE_INTERRUPTS();

  static volatile const char *fault_task = NULL;
  fault_task = pcTaskName;

  while(1) { __NOP(); }
}





