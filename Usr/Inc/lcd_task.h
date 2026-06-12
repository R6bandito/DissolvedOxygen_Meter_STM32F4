#ifndef __LCD_TASK_H__
#define __LCD_TASK_H__


#include "FreeRTOS.h"
#include "task.h"

/* **************** Defiens **************** */
  #define COLOR_BG       0x0000  // 深黑
  #define COLOR_TITLE    0xFFFF
  #define COLOR_LABEL    0xBDF7
  #define COLOR_VALUE    0x07FF
  #define COLOR_UNIT     0xAD55
  #define COLOR_SEP      0x3186
  #define COLOR_RUN      0x07E0
  #define COLOR_MODBUS   0xFFE0
  #define COLOR_STOP     0xF800
/* **************** Defiens **************** */

/* *************************** */
void cTask_Lcd( void *parameter );
/* *************************** */

#endif /* CUS_LINE_THICK */
