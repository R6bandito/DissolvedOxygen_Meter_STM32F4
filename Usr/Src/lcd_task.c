#include "lcd_task.h"
#include "Cus_ST7789.h"
#include "do_data.h"
#include "adc_task.h"
#include "queue.h"
#include <stdio.h>


/* ***************************** */
static tftDevice_HandleTypeDef st7789;
static doData_t rcvData; 

extern QueueHandle_t doDataQueue;
extern volatile TickType_t g_modbus_last_rx_tick;  // 最后一次通信tick.
extern volatile uint32_t g_modbus_rx_count;        // Modbus通信接收计数器.
extern volatile uint32_t g_modbus_tx_count;         // Modbus通信发送计数器.

void cTask_Lcd( void *parameter );

static void Lcd_DisplayStaticElement( void );         // 显示静态元素.
static void Lcd_DisplayUpdate( doData_t ucData );     // 动态元素刷新显示.
static void Lcd_DisplayStatus( void );                // 状态显示.
static void Lcd_DisplaySystime( void );               // 系统运行时长显示.
/* ***************************** */


static void Lcd_DisplayStaticElement( void )
{
  /* 显示大标题. */
  st7789.lcd_drawString(&st7789, 80, 10, "DO METER", CUS_FONT_SIZE_24, COLOR_TITLE, COLOR_BG);

  /* 显示实时参数标题. */
  st7789.lcd_drawHLine(&st7789, 46, 14, 210, CUS_LINE_NORMAL, CUS_COLOR_WHITE);
  st7789.lcd_drawString(&st7789, 35, 50, "DO:", CUS_FONT_SIZE_12, COLOR_LABEL, COLOR_BG);
  st7789.lcd_drawString(&st7789, 20, 75, "Temp:", CUS_FONT_SIZE_12, COLOR_LABEL, COLOR_BG);
  st7789.lcd_drawHLine(&st7789, 95, 14, 210, CUS_LINE_NORMAL, CUS_COLOR_WHITE);

  /* 校准参数显示区. */
  st7789.lcd_drawRect(&st7789, 60, 105, 180, 200, CUS_LINE_NORMAL, CUS_COLOR_WHITE);
  st7789.lcd_drawString(&st7789, 65, 110, "CALIB_PARAM", CUS_FONT_SIZE_12, COLOR_LABEL, COLOR_BG);
  st7789.lcd_drawString(&st7789, 75, 125, "ZeroADC:", CUS_FONT_SIZE_12, COLOR_LABEL, COLOR_BG);
  st7789.lcd_drawString(&st7789, 75, 145, "AirADC:", CUS_FONT_SIZE_12, COLOR_LABEL, COLOR_BG);
  st7789.lcd_drawString(&st7789, 75, 165, "AirSAT:", CUS_FONT_SIZE_12, COLOR_LABEL, COLOR_BG);
  st7789.lcd_drawString(&st7789, 75, 185, "AirTemp:", CUS_FONT_SIZE_12, COLOR_LABEL, COLOR_BG);

  /* 系统状态. */
  st7789.lcd_drawString(&st7789, 20, 210, "SYS_STAUTE", CUS_FONT_SIZE_16, COLOR_LABEL, COLOR_BG);
}


static void Lcd_DisplayUpdate( doData_t ucData )
{
  char buffer[32];
  uint16_t air_adc, zero_adc;
  float air_sat, air_temp;

  sprintf(buffer, "%.2f mg/L", ucData.dissolved_oxygen);
  st7789.lcd_drawString(&st7789, 55, 48, buffer, CUS_FONT_SIZE_16, COLOR_VALUE, COLOR_BG);

  sprintf(buffer, "%.1f C", ucData.temperature);
  st7789.lcd_drawString(&st7789, 55, 78, buffer, CUS_FONT_SIZE_16, COLOR_VALUE, COLOR_BG);

  get_CalibParam(&zero_adc, &air_adc, &air_sat, &air_temp);
  sprintf(buffer, "%u", zero_adc);
  st7789.lcd_drawString(&st7789, 125, 125, buffer, CUS_FONT_SIZE_12, COLOR_VALUE, COLOR_BG);
  
  sprintf(buffer, "%u", air_adc);
  st7789.lcd_drawString(&st7789, 120, 145, buffer, CUS_FONT_SIZE_12, COLOR_VALUE, COLOR_BG);

  sprintf(buffer, "%.2f mg/L", air_sat);
  st7789.lcd_drawString(&st7789, 120, 165, buffer, CUS_FONT_SIZE_12, COLOR_VALUE, COLOR_BG);

  sprintf(buffer, "%.1f C", air_temp);
  st7789.lcd_drawString(&st7789, 125, 185, buffer, CUS_FONT_SIZE_12, COLOR_VALUE, COLOR_BG);
}


static void Lcd_DisplayStatus( void )
{
  TickType_t xLastWakeTime = xTaskGetTickCount();

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

  vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(500));
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


void cTask_Lcd( void *parameter )
{
  Cus_ST7789_InitHandle(&st7789);

  st7789.displayInit(&st7789, ST7789_ROTATION_0);

  Lcd_DisplayStaticElement();

  while(1)
  {
    /* 从队列获取实时信息并显示. */
    xQueuePeek(doDataQueue, &rcvData, pdMS_TO_TICKS(10));
    Lcd_DisplayUpdate(rcvData);
    Lcd_DisplayStatus();
    Lcd_DisplaySystime();
  }
}





