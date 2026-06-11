#include "key_task.h"
#include "queue.h"
#include "adc_task.h"
#include "cmd_uart.h"


/* *********************************** */
volatile uint8_t g_zeroKey_Event;           // 零点校准按键事件到达.
volatile uint8_t g_airKey_Event;            // 空气校准按键事件到达.
/* *********************************** */


void cTask_Key( void *parameter )
{
  static TickType_t last_press_tick = 0;

  while(1)
  {
    /* 永久阻塞直到获取信号. */
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    /* 获取信号量后. 消抖. */
    TickType_t now = xTaskGetTickCount();
    if ( now - last_press_tick > pdMS_TO_TICKS(100) )
    {
      last_press_tick = now;

      if ( g_zeroKey_Event )
      {
        /* 零点校准. */
        UART2_Printf("ZERO_CALIB Trigger.\n");

        calib_zero();

        UART2_Printf("CALIB Sample OK!\n");

        /* 清除事件. */
        g_zeroKey_Event = 0;
      }
      
      if ( g_airKey_Event )
      {
        /* 空气校准. */
        UART2_Printf("AIR_CALIB Trigger.");

        calib_air();

        UART2_Printf("AIR_CALIB Sample OK!\n");

        g_airKey_Event = 0;
      }
    }
  }
}




