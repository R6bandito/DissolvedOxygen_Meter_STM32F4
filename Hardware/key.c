/* ═══════════════════════════════════════ */
              /* INCLUDE */
#include "key.h"
#include "stm32f4xx_hal.h"
/* ═══════════════════════════════════════ */


/* ═══════════════════════════════════════ */
              /* Public_API */
void Cus_Key_Init( void );
/* ═══════════════════════════════════════ */


/* ————————————————————————————— KeyInit ————————————————————————————— */
void Cus_Key_Init( void )
{
  __HAL_RCC_GPIOE_CLK_ENABLE();

  GPIO_InitTypeDef InitStructure;

  /* 零点校准按钮引脚配置. */
  InitStructure.Alternate       = 0;
  InitStructure.Mode            = GPIO_MODE_IT_FALLING;
  InitStructure.Pin             = CALIB_ZERO_KEY_PIN;
  InitStructure.Pull            = GPIO_PULLUP;
  InitStructure.Speed           = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(CALIB_ZERO_KEY_PORT, &InitStructure);

  /* 空气校准按钮引脚配置. */
  InitStructure.Pin = CALIB_AIR_KEY_PIN;
  HAL_GPIO_Init(CALIB_AIR_KEY_PORT, &InitStructure);

  /* 按键外部中断配置. */
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);
  HAL_NVIC_SetPriority(EXTI4_IRQn, 6, 0);

  HAL_NVIC_EnableIRQ(EXTI3_IRQn);
  HAL_NVIC_SetPriority(EXTI3_IRQn, 6, 0);
}


