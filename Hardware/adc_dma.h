#ifndef __ADC_DMA_H__
#define __ADC_DMA_H__


/* ═══════════════════════════════════════ */
              /* INCLUDE */
#include "stm32f4xx_hal.h"
/* ═══════════════════════════════════════ */


/* ═════════════════════════════════════════════════════════ */
                        /* DEFINES */
  /* ADC宏开关. */
  #define USE_ADC_12BIT                        (1)    // 使用MCU内部12位ADC+DMA.
  #define USE_ADC_EXT_16BIT                    (0)    // 使用外部16位ADC转换(I2C通信).
    #if (USE_ADC_EXT_16BIT)
      #define   ADS1115_7BIT_ADDR              (0x48)   // ADDR接GND. 7位地址: 1001000.
      /* Ps: O2测量使用 AIN0. Temp测量使用 AIN1. */ 
    #endif 
                  
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

#if (USE_ADC_12BIT) && (USE_ADC_EXT_16BIT)
  #error "You cant enable USE_ADC_12BIT & USE_ADC_EXT_16BIT at the same time."
#endif 

/* *********************************************** */
#if (USE_ADC_12BIT)
  /* 初始化ADC. */
  void Cus_ADC_Init( void );        

  /* 初始化DMA. */
  void Cus_DMA_Init( void );

  /* 开始采样. */
  void Cus_ADC_SampleStart( void );
#endif 
/* 获取最新的O2. */
uint16_t Cus_getLatestO2( void );

/* 获取最新的Temp. */
uint16_t Cus_getLatestTemp( void );
/* *********************************************** */

#endif /* __ADC_DMA_H__ */
