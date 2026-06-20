/* ═══════════════════════════════════════ */
              /* INCLUDE */
#include "lcd_task.h"
#include "Cus_ST7789.h"
#include "do_data.h"
#include "adc_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdio.h>
/* ═══════════════════════════════════════ */


/* ═══════════════════════════════════════ */
            /* 全局变量(本文件) */
static doData_t rcvData; 
static tftDevice_HandleTypeDef st7789;
/* ═══════════════════════════════════════ */


/* ═══════════════════════════════════════ */
              /* 全局变量 */
extern QueueHandle_t doDataQueue;
extern volatile TickType_t g_modbus_last_rx_tick;   // 最后一次通信tick.
extern volatile uint32_t g_modbus_rx_count;         // Modbus通信接收计数器.
extern volatile uint32_t g_modbus_tx_count;         // Modbus通信发送计数器.

QueueHandle_t g_menuEventQueue;                     // 菜单操作事件队列.
uint8_t is_Menu = 0;                                // 标志位. 判断当前是否处于菜单.
/* ═══════════════════════════════════════ */


/* ═══════════════════════════════════════ */
              /* Public_API */
void cTask_Lcd( void *parameter );
extern void IWDG_Refresh(void);
/* ═══════════════════════════════════════ */


/* ═══════════════════════════════════════ */
              /* Static_API */
static void Lcd_DisplayStaticElement( void );         // 显示静态元素.
static void Lcd_DisplayUpdate( doData_t ucData );     // 动态元素刷新显示.
static void Lcd_DisplayStatus( void );                // 状态显示.
static void Lcd_DisplaySystime( void );               // 系统运行时长显示.

static void Lcd_DisplayMenu( uint8_t uxIndex );                  // 绘制主菜单.
/* ═══════════════════════════════════════ */


/* ————————————————————————————— Static Element ————————————————————————————— */
static void Lcd_DisplayStaticElement( void )
{
  /* 显示大标题. */
  st7789.lcd_drawString(&st7789, 80, 10, "DO METER", CUS_FONT_SIZE_24, COLOR_TITLE, COLOR_BG);

  /* 显示实时参数标题. */
  st7789.lcd_drawHLine(&st7789, 46, 14, 210, CUS_LINE_NORMAL, COLOR_SEP);
  st7789.lcd_drawString(&st7789, 35, 50, "DO:", CUS_FONT_SIZE_12, COLOR_LABEL, COLOR_BG);
  st7789.lcd_drawString(&st7789, 20, 75, "Temp:", CUS_FONT_SIZE_12, COLOR_LABEL, COLOR_BG);
  st7789.lcd_drawHLine(&st7789, 95, 14, 210, CUS_LINE_NORMAL, COLOR_SEP);

  /* 校准参数显示区. */
  st7789.lcd_drawRect(&st7789, 60, 105, 180, 200, CUS_LINE_NORMAL, COLOR_SEP);
  st7789.lcd_drawString(&st7789, 65, 110, "CALIB_PARAM", CUS_FONT_SIZE_12, COLOR_LABEL, COLOR_BG);
  st7789.lcd_drawString(&st7789, 75, 125, "ZeroADC:", CUS_FONT_SIZE_12, COLOR_LABEL, COLOR_BG);
  st7789.lcd_drawString(&st7789, 75, 145, "AirADC:", CUS_FONT_SIZE_12, COLOR_LABEL, COLOR_BG);
  st7789.lcd_drawString(&st7789, 75, 165, "AirSAT:", CUS_FONT_SIZE_12, COLOR_LABEL, COLOR_BG);
  st7789.lcd_drawString(&st7789, 75, 185, "AirTemp:", CUS_FONT_SIZE_12, COLOR_LABEL, COLOR_BG);

  /* 系统状态. */
  st7789.lcd_drawString(&st7789, 20, 210, "SYS_STAUTE", CUS_FONT_SIZE_16, COLOR_LABEL, COLOR_BG);
}


static void Lcd_DisplayMenu( uint8_t uxIndex )
{
  /* 显示菜单项目. */
  uint16_t item_y[5] = { 70, 108, 146, 184, 222 };
  const char *labels[] = { "Zero ADC", "Air ADC", "Air SAT", "Air Temp", "RETURN" };

  /* ── 标题栏 ── */
  st7789.lcd_drawString(&st7789, 55, 30, "CALIB SETUP", CUS_FONT_SIZE_24, COLOR_TITLE, COLOR_BG);
  st7789.lcd_drawHLine(&st7789, 55, 10, 230, CUS_LINE_NORMAL, COLOR_SEP);

  for ( int i = 0; i < 5; i++ )
  {
    uint16_t y = item_y[i];
    uint8_t select = (i == uxIndex);
    uint16_t dot_color;

    if ( select )
    {
      /* 选中项. 绿灯指示. */
      dot_color = COLOR_RUN;
    }
    else 
    {
      /* 未选中项. 背景色. */
      dot_color = COLOR_BG;
    }

    /* 图标 */
    st7789.lcd_drawFillCircle(&st7789, 68, y + 12, 5, dot_color);

    /* 标签 */
    st7789.lcd_drawString(&st7789, 87, y + 5, labels[i], CUS_FONT_SIZE_12, COLOR_LABEL, COLOR_BG);
  }

}


/* ————————————————————————————— Dynamic Element ————————————————————————————— */
static void Lcd_DisplayUpdate( doData_t ucData )
{
  char buffer[32];
  uint16_t air_adc, zero_adc;
  float air_sat, air_temp;

  sprintf(buffer, "%5.2f mg/L", ucData.dissolved_oxygen);
  st7789.lcd_drawString(&st7789, 55, 48, buffer, CUS_FONT_SIZE_16, COLOR_VALUE, COLOR_BG);

  sprintf(buffer, "%5.1f C", ucData.temperature);
  st7789.lcd_drawString(&st7789, 55, 78, buffer, CUS_FONT_SIZE_16, COLOR_VALUE, COLOR_BG);

  get_CalibParam(&zero_adc, &air_adc, &air_sat, &air_temp);
  sprintf(buffer, "%-5u", zero_adc);
  st7789.lcd_drawString(&st7789, 125, 125, buffer, CUS_FONT_SIZE_12, COLOR_SEC_VALUE, COLOR_BG);
  
  sprintf(buffer, "%-5u", air_adc);
  st7789.lcd_drawString(&st7789, 120, 145, buffer, CUS_FONT_SIZE_12, COLOR_SEC_VALUE, COLOR_BG);

  sprintf(buffer, "%4.2f mg/L", air_sat);
  st7789.lcd_drawString(&st7789, 120, 165, buffer, CUS_FONT_SIZE_12, COLOR_SEC_VALUE, COLOR_BG);

  sprintf(buffer, "%4.1f C", air_temp);
  st7789.lcd_drawString(&st7789, 125, 185, buffer, CUS_FONT_SIZE_12, COLOR_SEC_VALUE, COLOR_BG);
}


static void Lcd_DisplayStatus( void )
{
  /* 系统运行状态. */
  static uint8_t run_phase = 0;
  uint16_t current_color = (run_phase) ? COLOR_RUN : COLOR_BG;
  st7789.lcd_drawFillCircle(&st7789, 26, 244, 3, current_color);
  st7789.lcd_drawString(&st7789, 40, 235, "RUNNING", CUS_FONT_SIZE_16, COLOR_LABEL, COLOR_BG);
  run_phase ^= 1;

  /* Modbus 通信状态. */
  int8_t is_mb_OK = ((xTaskGetTickCount() - g_modbus_last_rx_tick) < pdMS_TO_TICKS(5000));
  uint16_t mb_statue_color = (is_mb_OK) ? COLOR_RUN : COLOR_STOP;
  st7789.lcd_drawFillCircle(&st7789, 120, 244, 3, mb_statue_color);

  const char *mb_label = (is_mb_OK) ? "MODBUS-OK" : "MODBUS---";
  uint16_t label_color = is_mb_OK ? COLOR_LABEL : 0xFFE0;  
  st7789.lcd_drawString(&st7789, 130, 235, mb_label, CUS_FONT_SIZE_16, label_color, COLOR_BG);
}


static void Lcd_DisplaySystime( void )
{
  TickType_t uptime_ticks = xTaskGetTickCount();
  uint32_t  uptime_sec    = uptime_ticks / configTICK_RATE_HZ;
  uint32_t  days  = uptime_sec / 86400;
  uint32_t  hours = (uptime_sec % 86400) / 3600;
  uint32_t  mins  = (uptime_sec % 3600)   / 60;
  uint32_t  secs  = uptime_sec % 60;

  char buf[32];
  sprintf(buf, "RunTME:%ud %02u:%02u:%02u", days, hours, mins, secs);
  st7789.lcd_drawString(&st7789, 30, 295, buf, CUS_FONT_SIZE_16, COLOR_LABEL, COLOR_BG);
}


/* ————————————————————————————— Task ————————————————————————————— */
void cTask_Lcd( void *parameter )
{
  Cus_ST7789_InitHandle(&st7789);

  st7789.displayInit(&st7789, ST7789_ROTATION_0);

  Lcd_DisplayStaticElement();

  uint8_t menuEvent = 0xFF;   // 无效事件.

  /* 当前所指向的菜单栏目.(默认指向第一栏目 ZeroADC) */
  static uint8_t currentIndex = 0;

  while(1)
  {
    if ( !is_Menu )
    {
      /* 从队列获取实时信息并显示. 上电默认处于该显示方式 */
      xQueuePeek(doDataQueue, &rcvData, pdMS_TO_TICKS(10));
      Lcd_DisplayUpdate(rcvData);
      Lcd_DisplayStatus();
      Lcd_DisplaySystime();
    }

    /* 检查操作队列是否有菜单操作. 进入菜单后以80ms快速进行刷新. */
    xQueueReceive(g_menuEventQueue, &menuEvent, (is_Menu) ? pdMS_TO_TICKS(100) : pdMS_TO_TICKS(500));

    /* 有菜单操作. 立即响应. */
    switch (menuEvent)
    {
      case MENU_MAIN:
      {
        /* 显示主菜单. */
        if ( !is_Menu )
        {
          /* 先整体清一次屏. 以后的菜单变化只清局部不清大屏. */
          st7789.setWindow(&st7789, 0, 0, LCD_SCREEN_WIDTH - 1, LCD_SCREEN_HEIGHT - 1);
          st7789.lcd_fill(&st7789, COLOR_BG);
        }
        is_Menu = 1;
        Lcd_DisplayMenu(currentIndex);

        break;
      }

      case MENU_NEXT:
      {
        currentIndex++;     // 菜单指向下一栏目.
        if ( currentIndex >= MENU_ITEM_NUM )
        {
          /* 回转指向第一栏目. */
          currentIndex = 0;
        }
        menuEvent = MENU_MAIN;

        break;
      }

      case MENU_ENTER:
      {

        break;
      }
    
      default:    break;
    }

    /* 喂狗. */
    IWDG_Refresh();
  }
}





