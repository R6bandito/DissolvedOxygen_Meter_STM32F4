#ifndef __ADC_DMA_H__
#define __ADC_DMA_H__


#include "stm32f4xx_hal.h"


/* ****************** Defines ****************** */
  #define CUS_ADC_O2_IN_PORT                   (GPIOA)             // 溶解氧信号采样通道 端口.
  #define CUS_ADC_O2_IN_PIN                    (GPIO_PIN_1)        // 溶解氧信号采样通道 引脚.

  #define CUS_ADC_TEMP_IN_PORT                 (GPIOA)             // NTC温度信号采样通道 端口.
  #define CUS_ADC_TEMP_IN_PIN                  (GPIO_PIN_4)        // NTC温度信号采样通道 引脚.

  #define CUS_ADC_O2_INDEX                     (0)  
  #define CUS_ADC_TEMP_INDEX                   (1)

  #define CUS_DMA2_STREAM4_PRIO                (8)
/* ****************** Defines ****************** */



/* *********************************************** */
/* 初始化ADC. */
void Cus_ADC_Init( void );        

/* 初始化DMA. */
void Cus_DMA_Init( void );

/* 开始采样. */
void Cus_ADC_SampleStart( void );

/* 获取最新的O2. */
uint16_t Cus_getLatestO2( void );

/* 获取最新的Temp. */
uint16_t Cus_getLatestTemp( void );
/* *********************************************** */

#endif /* __ADC_DMA_H__ */
