#include "create_task.h"
#include "adc_task.h"
#include "cmd_uart.h"
#include "uart_cmd_task.h"
#include "queue.h"
#include "do_data.h"
#include "key.h"
#include "key_task.h"
#include "semphr.h"


/* *************************************** */
static TaskHandle_t ADC_taskHandle;
static TaskHandle_t Update_taskHandle;
static TaskHandle_t UartCmd_taskHandle;
static TaskHandle_t Key_taskHandle;
/* *************************************** */

/* *************************************** */
void systemInit_Run( void );
static void createTask( void );

TaskHandle_t getADCTask_Handle( void );
TaskHandle_t getUpdateTask_Handle( void );
TaskHandle_t getUartCmdTask_Handle( void );
TaskHandle_t getKeyTask_Handle( void );

extern SemaphoreHandle_t xMutexUart2;
extern QueueHandle_t doDataQueue;
extern doData_t doData;
/* *************************************** */


static void createTask( void )
{
  BaseType_t pReturn_ADC =  xTaskCreate( cTask_ADC, 
                                         "adc", 
                                         CUS_ADC_TASK_DEEPTH, 
                                         NULL, 
                                         CUS_ADC_TASK_PRIO, 
                                         &ADC_taskHandle );

  BaseType_t pReturn_update = xTaskCreate( cTask_Update, 
                                           "update", 
                                           CUS_UPDATE_TASK_DEEPTH, 
                                           NULL, 
                                           CUS_UPDATE_TASK_PRIO, 
                                           &Update_taskHandle );

  BaseType_t pReturn_UartCmd = xTaskCreate( cTask_UartCmdTask, 
                                            "uartCmd", 
                                            CUS_UART_CMD_TASK_DEEPTH, 
                                            NULL, 
                                            CUS_UART_CMD_TASK_PRIO, 
                                            &UartCmd_taskHandle );

  BaseType_t pReturn_Key = xTaskCreate( cTask_Key, 
                                        "key", 
                                        CUS_KEY_TASK_DEEPTH, 
                                        NULL, 
                                        CUS_KEY_TASK_PRIO, 
                                        &Key_taskHandle );
  if ( pReturn_ADC != pdPASS )
  {
    for( ; ; );
  }

  if ( pReturn_update != pdPASS )
  {
    for( ; ; );
  }

  if ( pReturn_UartCmd != pdPASS )
  {
    for( ; ; );
  }

  if ( pReturn_Key != pdPASS )
  {
    for( ; ; );
  }
}


void systemInit_Run( void )
{
  /* 数据结构初始化. */
  doDataQueue = xQueueCreate(1, sizeof(doData_t));    // 覆写队列. 只保留最新值. 长度为1.
  if ( doDataQueue == NULL )
  {
    /* 错误处理. 预留. */
    for( ; ; );
  }

  xMutexUart2 = xSemaphoreCreateMutex();
  if ( xMutexUart2 == NULL )
  {
    for( ; ; );
  }

  {
    /* 提供一个默认校准参数用于计算. */
    calib_defaultInit();
  }

  {
    /* ADC初始化. */
    Cus_ADC_Init();

    /* DMA初始化. */
    Cus_DMA_Init();

    /* 上位机通信串口初始化. */
    Cus_UART_Init();

    /* 按键初始化. */
    Cus_Key_Init();

    /* 开始采样. */
    Cus_ADC_SampleStart();

    /* 串口中断接收开始. */
    Cus_UART_StartTransfer();
  }

  /* 创建任务. */
  createTask();

  /* 开启任务调度. */
  vTaskStartScheduler();
}


TaskHandle_t getADCTask_Handle( void )
{
  return ADC_taskHandle;
}


TaskHandle_t getUpdateTask_Handle( void )
{
  return Update_taskHandle;
}


TaskHandle_t getUartCmdTask_Handle( void )
{
  return UartCmd_taskHandle;
}


TaskHandle_t getKeyTask_Handle( void )
{
  return Key_taskHandle;
}

