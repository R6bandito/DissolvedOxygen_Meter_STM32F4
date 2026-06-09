#ifndef __ADC_TASK_H__
#define __ADC_TASK_H__

#include "FreeRTOS.h"
#include "task.h"
#include "adc_dma.h"


/* *************** Defins *************** */
  #define FILTER_N                    (16)      // 平均滑动滤波缓冲区大小.

/* *************** Defins *************** */


/* **************************************** */
void cTask_ADC( void *parameter );
void cTask_Update( void *parameter );
/* **************************************** */

#endif /* __ADC_TASK_H__ */

