#ifndef __MB_TASK_H__
#define __MB_TASK_H__


#include "FreeRTOS.h"
#include "task.h"


/* ******************************* */
#define CUS_MODBUS_SLAVE_ADDR                 (1)             // 作为从机的地址.

void cTask_Modbus( void *parameter );
/* ******************************* */

#endif /* __MB_TASK_H__ */

