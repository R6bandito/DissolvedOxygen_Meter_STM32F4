/* ═══════════════════════════════════════ */
              /* INCLUDE */
#include "stm32f4xx_hal.h"
#include "create_task.h"
#include "adc_task.h"
#include "adc_dma.h"
#include "cmd_uart.h"
#include "uart_cmd_task.h"
#include "queue.h"
#include "do_data.h"
#include "key.h"
#include "key_task.h"
#include "lcd_task.h"
#include "task_guard.h"
#include "led.h"
#include "mb_task.h"
#include "mb.h"
#include "semphr.h"
/* ═══════════════════════════════════════ */


/* ═══════════════════════════════════════ */
            /* 全局变量(本文件) */
static TaskHandle_t ADC_taskHandle;
static TaskHandle_t Update_taskHandle;
static TaskHandle_t UartCmd_taskHandle;
static TaskHandle_t Key_taskHandle;
static TaskHandle_t Modbus_taskHandle;
static TaskHandle_t Lcd_taskHandle;
static TaskHandle_t Guard_taskHandle;

static IWDG_HandleTypeDef hiwdg;     // 看门狗.
/* ═══════════════════════════════════════ */


/* ═══════════════════════════════════════ */
              /* 全局变量 */
extern SemaphoreHandle_t xMutexUart2;
extern QueueHandle_t doDataQueue;
extern doData_t doData;
/* ═══════════════════════════════════════ */


/* ═══════════════════════════════════════ */
              /* Public_API */
void systemInit_Run( void );    // 系统初始化及其启动.(阻塞调用，不返回)

void IWDG_Refresh( void );    // 对外提供喂狗通道(通过extern引用).

/* 相关任务句柄外部获取通道. */
TaskHandle_t getADCTask_Handle( void );
TaskHandle_t getUpdateTask_Handle( void );
TaskHandle_t getUartCmdTask_Handle( void );
TaskHandle_t getKeyTask_Handle( void );
TaskHandle_t getModbusTask_Handle( void );
TaskHandle_t getLcdTask_Handle( void );
TaskHandle_t getGuardTask_Handle( void );
/* ═══════════════════════════════════════ */


/* ═══════════════════════════════════════ */
              /* Static_API */
static void createTask( void );   // 任务创建(内部方法).
/* ═══════════════════════════════════════ */


/* ————————————————————————————— SystemInit ————————————————————————————— */
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
    /* 指示灯初始化. 正常运行状态常亮. */
    Cus_Debug_LED_Init();
    runWrLEDSwitch(RUN_WR_LED_ON);

    /* ADC初始化. */
    Cus_ADC_Init();

    /* 提供一个默认校准参数用于计算. */
    calib_defaultInit();

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

  /* 初始化狗. */
  hiwdg.Instance = IWDG;
  hiwdg.Init.Prescaler = IWDG_PRESCALER_32;
  hiwdg.Init.Reload = 2000;       // 2秒超时.
  HAL_IWDG_Init(&hiwdg);

  /* 开启任务调度. */
  vTaskStartScheduler();
}


/* ————————————————————————————— IWDG_Feeder ————————————————————————————— */
void IWDG_Refresh( void )
{
  HAL_IWDG_Refresh(&hiwdg);
}


/* ————————————————————————————— TaskCreate ————————————————————————————— */
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

  BaseType_t pReturn_Modbus = xTaskCreate( cTask_Modbus, 
                                           "modbus", 
                                           CUS_MODBUS_TASK_DEEPTH, 
                                           NULL, 
                                           CUS_MODBUS_TASK_PRIO, 
                                           &Modbus_taskHandle );

  BaseType_t pReturn_Lcd = xTaskCreate( cTask_Lcd, 
                                        "lcd", 
                                        CUS_LCD_TASK_DEEPTH, 
                                        NULL, 
                                        CUS_LCD_TASK_PRIO, 
                                        &Lcd_taskHandle );

  BaseType_t pReturn_Guard = xTaskCreate( cTask_Guard, 
                                          "guard", 
                                          CUS_GUARD_TASK_DEEPTH, 
                                          NULL, 
                                          CUS_GUARD_TASK_PRIO, 
                                          &Guard_taskHandle );
  
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

  if ( pReturn_Modbus != pdPASS )
  {
    for( ; ; );
  }

  if ( pReturn_Lcd != pdPASS )
  {
    for( ; ; );
  }

  if ( pReturn_Guard != pdPASS )
  {
    for( ; ; );
  }
}


/* ————————————————————————————— getXXXX_TaskHandle ————————————————————————————— */
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


TaskHandle_t getModbusTask_Handle( void )
{
  return Modbus_taskHandle;
}


TaskHandle_t getLcdTask_Handle( void )
{
  return Lcd_taskHandle;
}


TaskHandle_t getGuardTask_Handle( void )
{
  return Guard_taskHandle;
}

