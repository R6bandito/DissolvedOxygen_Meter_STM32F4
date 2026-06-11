#ifndef __KEY_H__
#define __KEY_H__


#include "stm32f4xx_hal.h"


/* *************** Defiens *************** */
  /* 零点校准按钮按键引脚，端口映射. */
  #define CALIB_ZERO_KEY_PORT           (GPIOE)
  #define CALIB_ZERO_KEY_PIN            (GPIO_PIN_4)

  /* 空气校准按钮按键引脚，端口映射. */
  #define CALIB_AIR_KEY_PORT            (GPIOE)
  #define CALIB_AIR_KEY_PIN             (GPIO_PIN_3)
/* *************** Defiens *************** */


/* ************************************** */
void Cus_Key_Init( void );
/* ************************************** */

#endif /* __KEY_H__ */

