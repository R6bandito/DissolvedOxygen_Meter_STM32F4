#ifndef __CUS_UTILS_DELAY_H__
#define __CUS_UTILS_DELAY_H__


/* ************ INCLUDE ************* */
  #include "stm32f4xx.h"

/* ************ INCLUDE ************* */



/* ************ FEATURE ************* */
  #define CUS_DELAY_USE_DWT_TRACE    (1)        // 使用DWT内核定时器延时(高精度 推荐)
  #define CUS_DELAY_USE_SYSTICK      (0)        // 使用Systick定时器延时
  #define CUS_DELAY_USE_CMDCYCLE     (0)
/* ************ FEATURE ************* */


void Cus_delay_init( uint32_t SystemCoreClock_Hz );
void Cus_delay_us( uint32_t us );
void Cus_delay_ms( uint32_t ms );



#endif // __CUS_UTILS_DELAY_H__
