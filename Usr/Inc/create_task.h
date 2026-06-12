#ifndef __C_TASK_H__
#define __C_TASK_H__


#include "FreeRTOS.h"
#include "task.h"


/* ********************* Defines ********************* */
  #define CUS_ADC_TASK_DEEPTH                   (128)
  #define CUS_ADC_TASK_PRIO                     (5)

  #define CUS_UPDATE_TASK_DEEPTH                (128)
  #define CUS_UPDATE_TASK_PRIO                  (6)

  #define CUS_UART_CMD_TASK_DEEPTH              (128)
  #define CUS_UART_CMD_TASK_PRIO                (7)

  #define CUS_KEY_TASK_DEEPTH                   (128)
  #define CUS_KEY_TASK_PRIO                     (7)

  #define CUS_MODBUS_TASK_DEEPTH                (512)
  #define CUS_MODBUS_TASK_PRIO                  (8)

  #define CUS_LCD_TASK_DEEPTH                   (256)
  #define CUS_LCD_TASK_PRIO                     (4)
/* ********************* Defines ********************* */


/* ********************************************* */
void systemInit_Run( void );

/* 句柄获取方法. */
TaskHandle_t getADCTask_Handle( void );
TaskHandle_t getUpdateTask_Handle( void );
TaskHandle_t getUartCmdTask_Handle( void );
TaskHandle_t getKeyTask_Handle( void );
TaskHandle_t getLcdTask_Handle( void );
/* ********************************************* */

#endif /* __C_TASK_H__ */
