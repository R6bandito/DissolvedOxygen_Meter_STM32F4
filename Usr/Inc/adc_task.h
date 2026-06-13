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

  /* 以下是全局校准参数的默认值. 系统初始化时就选用该值进行计算. */
  /* 该值基于默认线性关系的浓度值给出. */
  #define CALIB_DEFAULT_ZERO_ADC      (0)
  #define CALIB_DEFAULT_AIR_ADC       (1860)               // 9.08mg/L 对应的 ADC值.
  #define CALIB_DEFAULT_AIR_SAT       (9.08)               // 20度对应饱和溶解氧值.
  #define CALIB_DEFAULT_AIR_TEMP      (20)                 // 假设校准温度 20℃.

  /* 以下是BKPSRAM的内存地址映射. 不要修改! */
  #define CALIB_STORE_MAGIC_ZERO      (0x5A5A0000UL)
  #define CALIB_STORE_MAGIC_AIR       (0x00005A5AUL)
  #define CALIB_STORE_ADDR_BASE       *((volatile uint32_t *)0x40024000UL)
  #define CALIB_STORE_ZERO_ADC        *((volatile uint32_t *)0x40024004UL)   
  #define CALIB_STORE_AIR_ADC         *((volatile uint32_t *)0x40024008UL)   
  #define CALIB_STORE_AIR_SAT         *((volatile uint32_t *)0x4002400CUL)
  #define CALIB_STORE_AIR_TEMP        *((volatile uint32_t *)0x40024010UL)
/* *************** Defins *************** */


/* **************************************** */
void cTask_ADC( void *parameter );
void cTask_Update( void *parameter );

/* 零点校准. 用于串口处理任务中调用. */
void calib_zero( void );

/* 空气校准. 用于串口处理任务中调用. */
void calib_air( void );

/* 提供一个默认校准参数. */
void calib_defaultInit( void );

/* 该方法为同步函数供Modbus HoldingCB调用. 用于同步写入的校准参数. */
void calib_sync( void );

/* 这两个API作为对外接口. 向外部输送经过滤波的最新ADC采集值. */
uint16_t get_ADC_O2( void );
uint16_t get_ADC_Temp( void );

/* 该接口用于向外界提供校准参数读数. */
void get_CalibParam( uint16_t *o_ZeroAdc, uint16_t *o_AirAdc, float *o_AirSat, float *o_AirTemp );
/* **************************************** */

#endif /* __ADC_TASK_H__ */

