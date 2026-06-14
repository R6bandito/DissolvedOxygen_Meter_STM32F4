#ifndef __ADC_DMA_H__
#define __ADC_DMA_H__


/* ═══════════════════════════════════════ */
              /* INCLUDE */
#include "stm32f4xx_hal.h"
/* ═══════════════════════════════════════ */


/* ═════════════════════════════════════════════════════════ */
                        /* DEFINES */
  /* 溶解氧信号采样通道端口，引脚映射. */
  #define CUS_ADC_O2_IN_PORT                   (GPIOA)             
  #define CUS_ADC_O2_IN_PIN                    (GPIO_PIN_1)        

  /* 温度信号采样通道端口，引脚映射. */
  #define CUS_ADC_TEMP_IN_PORT                 (GPIOA)             
  #define CUS_ADC_TEMP_IN_PIN                  (GPIO_PIN_4)        

  /* 信号在缓冲区中索引编号. */
  #define CUS_ADC_O2_INDEX                     (0)  
  #define CUS_ADC_TEMP_INDEX                   (1)
/* ═════════════════════════════════════════════════════════ */



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
