#ifndef __TASK_GUARD_H__
#define __TASK_GUARD_H__


#include "FreeRTOS.h"
#include "task.h"


/* 任务信息数据结构. */
typedef struct 
{
  TaskHandle_t Handle;            // 任务句柄.
  const char *taskName;           // 任务名称.
  UBaseType_t minimumStackSize;   // 历史栈最高水位.
  UBaseType_t stackSize;         // 栈大小.
  UBaseType_t priority;           // 优先级.
  eTaskState status;              // 任务状态. 

} taskInfo_t;


/* **************** Defines **************** */
  /* 总任务数. */
  #define CTASK_TOTAL                 (7)   
/* **************** Defines **************** */


/* *********************************** */
void cTask_Guard( void *parameter );
/* *********************************** */

#endif /* __TASK_GUARD_H__ */

