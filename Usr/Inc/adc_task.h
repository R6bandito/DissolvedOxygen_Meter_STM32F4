#ifndef __ADC_TASK_H__
#define __ADC_TASK_H__

#include "FreeRTOS.h"
#include "task.h"
#include "adc_dma.h"


/* *************** Defins *************** */
  #define FILTER_N                    (16)                // 平均滑动滤波缓冲区大小.

  /* 以下是NTC热敏电阻 B值法 计算公式参数. */
  #define NTC_R_PULL                  (10000.0f)          // NTC上拉电阻(10k).
  #define NTC_R0                      (10000.0f)          // 热敏电阻25度时的阻值.
  #define NTC_B                       (3435.0f)           // B值.
  #define NTC_T0_K                    (273.15f + 25.0f)   // 25℃的开尔文温度.
/* *************** Defins *************** */


/* **************************************** */
void cTask_ADC( void *parameter );
void cTask_Update( void *parameter );

/* 零点校准. 用于串口处理任务中调用. */
void calib_zero( void );

/* 空气校准. 用于串口处理任务中调用. */
void calib_air( void );

/* 这两个API作为对外接口. 向外部输送经过滤波的最新ADC采集值. */
uint16_t get_ADC_O2( void );
uint16_t get_ADC_Temp( void );
/* **************************************** */

#endif /* __ADC_TASK_H__ */

