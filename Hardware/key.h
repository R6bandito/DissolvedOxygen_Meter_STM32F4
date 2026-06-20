#ifndef __KEY_H__
#define __KEY_H__


/* ═════════════════════════════════════════════════════════ */
                        /* DEFINES */
  /* 零点校准按钮按键引脚，端口映射. */
  #define CALIB_ZERO_KEY_PORT           (GPIOE)
  #define CALIB_ZERO_KEY_PIN            (GPIO_PIN_4)

  /* 空气校准按钮按键引脚，端口映射. */
  #define CALIB_AIR_KEY_PORT            (GPIOE)
  #define CALIB_AIR_KEY_PIN             (GPIO_PIN_3)

  /* 校准参数复原按键引脚，端口映射. */
  #define CALIB_RESET_KEY_PORT          (GPIOA)
  #define CALIB_RESET_KEY_PIN           (GPIO_PIN_0)

  /* 按钮长安识别时间间隔. */
  #define LONG_PRESS_IDENTIFY_MS        (500)  
/* ═════════════════════════════════════════════════════════ */


/* ═════════════════════════════════════════════════════════ */
                      /* API_LIST */
void Cus_Key_Init( void );
/* ═════════════════════════════════════════════════════════ */

#endif /* __KEY_H__ */

