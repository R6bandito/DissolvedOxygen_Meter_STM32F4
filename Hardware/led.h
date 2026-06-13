#ifndef __LED_H__
#define __LED_H__


#include "stm32f4xx_hal.h"


/* **************** Defines **************** */
  /* 运行与告警指示灯. */
  #define RUN_WR_LED_PORT           (GPIOA)
  #define RUN_WR_LED_PIN            (GPIO_PIN_7)

  #define RUN_WR_LED_ON             (0)
  #define RUN_WR_LED_OFF            (1)
/* **************** Defines **************** */


/* ************************************ */
void Cus_Debug_LED_Init( void );        // 指示灯初始化.
void runWrLEDSwitch( uint8_t EN_DIS );  // 运行警告指示灯开关.
/* ************************************ */


#endif /* __LED_H__ */

