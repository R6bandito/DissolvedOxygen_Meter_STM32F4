#ifndef __LED_H__
#define __LED_H__


/* ═══════════════════════════════════════ */
              /* INCLUDE */
#include "stm32f4xx_hal.h"
/* ═══════════════════════════════════════ */


/* ═══════════════════════════════════════ */
              /* DEFINES */
  /* 运行与告警指示灯. 引脚及端口映射. */
  #define RUN_WR_LED_PORT           (GPIOA)
  #define RUN_WR_LED_PIN            (GPIO_PIN_7)

  /* 输出电平宏(根据LED连接方式进行修改). */
  #define RUN_WR_LED_ON             (0)
  #define RUN_WR_LED_OFF            (1)
/* ═══════════════════════════════════════ */


/* ═════════════════════════════════════════════════════════ */
                      /* API_LIST */
void Cus_Debug_LED_Init( void );        // 指示灯初始化.
void runWrLEDSwitch( uint8_t EN_DIS );  // 运行警告指示灯开关.
/* ═════════════════════════════════════════════════════════ */


#endif /* __LED_H__ */

