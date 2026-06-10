#ifndef __MAIN_H__
#define __MAIN_H__


#include "stm32f4xx_hal.h"
#include "create_task.h"
#include "debug_uart.h"
#include "FreeRTOS.h"
#include "task.h"



#ifndef __ATOMIC_MACROS_DEFINED__
#define __ATOMIC_MACROS_DEFINED__


#ifndef ATOMIC_SET_BIT
  #define ATOMIC_SET_BIT(REG, BIT)                           \
    do {                                                     \
      uint32_t val;                                          \
      do {                                                   \
        val = __LDREXW((__IO uint32_t*)&(REG));              \
        val |= (BIT);                                        \
      } while (__STREXW(val, (__IO uint32_t*)&(REG)));       \
    } while (0)
#endif

#ifndef ATOMIC_CLEAR_BIT
  #define ATOMIC_CLEAR_BIT(REG, BIT)                         \
    do {                                                     \
      uint32_t val;                                          \
      do {                                                   \
        val = __LDREXW((__IO uint32_t*)&(REG));              \
        val &= ~(BIT);                                       \
      } while (__STREXW(val, (__IO uint32_t*)&(REG)));       \
    } while (0)
#endif

#ifndef ATOMIC_MODIFY_REG
  #define ATOMIC_MODIFY_REG(REG, CLEARMSK, SETMASK)          \
    do {                                                     \
      uint32_t val;                                          \
      do {                                                   \
        val = __LDREXW((__IO uint32_t*)&(REG));              \
        val = (val & (~(CLEARMSK))) | (SETMASK);             \
      } while (__STREXW(val, (__IO uint32_t*)&(REG)));       \
    } while (0)
#endif

#endif // __ATOMIC_MACROS_DEFINED__



#endif 

