#include "create_task.h"
#include "adc_task.h"


/* *************************************** */
static TaskHandle_t ADC_taskHandle;
static TaskHandle_t Update_taskHandle;
/* *************************************** */

/* *************************************** */
void systemInit_Run( void );
static void createADCTask( void );
/* *************************************** */


static void createADCTask( void )
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
  if ( pReturn_ADC != pdPASS )
  {
    for( ; ; );
  }

  if ( pReturn_update != pdPASS )
  {
    for( ; ; );
  }
}


void systemInit_Run( void )
{

  {
    /* ADC初始化. */
    Cus_ADC_Init();

    /* DMA初始化. */
    Cus_DMA_Init();

    /* 开始采样. */
    Cus_ADC_SampleStart();
  }

  /* 创建ADC处理任务. */
  createADCTask();

  /* 开启任务调度. */
  vTaskStartScheduler();
}


