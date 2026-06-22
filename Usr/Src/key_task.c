/* ═══════════════════════════════════════ */
              /* INCLUDE */
#include "key_task.h"
#include "lcd_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "adc_task.h"
#include "cmd_uart.h"
#include "stm32f4xx_hal.h"
/* ═══════════════════════════════════════ */


/* ═══════════════════════════════════════ */
              /* 全局变量 */
/* 按键事件队列.  (0=短按KEY1 1=短按KEY2 2=短按KEY3 3=长按KEY1 4=长按KEY2 5=长按KEY3) */
QueueHandle_t g_keyEventQueue;              
extern QueueHandle_t g_menuEventQueue;    // 菜单事件队列.
extern uint8_t is_Menu;                   // 标志位. 判断当前是否处于菜单.
/* ═══════════════════════════════════════ */


/* ═══════════════════════════════════════ */
              /* Public_API */
void cTask_Key( void *parameter );
/* ═══════════════════════════════════════ */



/* ————————————————————————————— Task ————————————————————————————— */
void cTask_Key( void *parameter )
{
  uint8_t keyEvent;   // 按键事件.

  while(1)
  {
    /* 永久阻塞直到获取到事件. */
    xQueueReceive(g_keyEventQueue, &keyEvent, portMAX_DELAY);

    switch (keyEvent)
    {
      case KEY1_SHORT_PRESS:
      {
        if ( is_Menu )
        {
          /* 当前处于菜单状态. 短按响应prev. */
          menuEvent_t Event = MENU_PREV;
          xQueueSend(g_menuEventQueue, &Event, portMAX_DELAY);
        }
        else 
        {
          /* 零点校准. */
          calib_zero();
        }

        break;
      }

      case KEY2_SHORT_PRESS:
      {
        /* 空气校准. */
        if ( is_Menu )
        {
          /* 当前处于菜单状态. 短按响应Next. */
          menuEvent_t Event = MENU_NEXT;
          xQueueSend(g_menuEventQueue, &Event, portMAX_DELAY);
        }
        else 
        {
          /* 普通模式. 空气校准. */
          calib_air();
        }
        
        break;
      }

      case KEY3_SHORT_PRESS:
      {
        if ( is_Menu )
        {
          /* 菜单模式下作为确认按键转发. */
          menuEvent_t Event = MENU_ENTER;
          xQueueSend(g_menuEventQueue, &Event, portMAX_DELAY);
        }
        else 
        {
          /* 重置位于BKP SRAM中的魔数. */
          CALIB_STORE_ADDR_BASE = 0;
  
          /* 重置校准参数. */
          calib_defaultInit();
        }

        break;
      }

      case KEY1_LONG_PRESS:
      {
        menuEvent_t Event = MENU_MAIN;
        xQueueSend(g_menuEventQueue, &Event, portMAX_DELAY);
        break;
      }

      case KEY2_LONG_PRESS:
      {
 
        break;
      }

      case KEY3_LONG_PRESS:
      {
        if ( is_Menu )
        {
          menuEvent_t Event = MENU_DIGTAL_SWITCH;
          xQueueSend(g_menuEventQueue, &Event, portMAX_DELAY);
        }

        break;
      }

      default:  break;
    }
  }
}




