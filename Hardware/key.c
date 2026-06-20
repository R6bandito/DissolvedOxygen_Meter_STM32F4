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
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitTypeDef InitStructure;

  /* 零点校准按钮引脚配置. */
  InitStructure.Alternate       = 0;
  InitStructure.Mode            = GPIO_MODE_IT_RISING_FALLING;      // 双边沿触发外部中断.
  InitStructure.Pin             = CALIB_ZERO_KEY_PIN;
  InitStructure.Pull            = GPIO_PULLUP;
  InitStructure.Speed           = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(CALIB_ZERO_KEY_PORT, &InitStructure);

  /* 空气校准按钮引脚配置. */
  InitStructure.Pin             = CALIB_AIR_KEY_PIN;
  HAL_GPIO_Init(CALIB_AIR_KEY_PORT, &InitStructure);

  /* 校准参数复位引脚配置. */
  InitStructure.Pin             = CALIB_RESET_KEY_PIN;
  InitStructure.Pull            = GPIO_PULLDOWN;        // 由于引脚外部接线方式，此处为下拉到地.
  HAL_GPIO_Init(CALIB_RESET_KEY_PORT, &InitStructure);

  /* 由于接线方式. 初始推挽输出高. */
  HAL_GPIO_WritePin(CALIB_ZERO_KEY_PORT, CALIB_ZERO_KEY_PIN, GPIO_PIN_SET);
  HAL_GPIO_WritePin(CALIB_AIR_KEY_PORT, CALIB_AIR_KEY_PIN, GPIO_PIN_SET);

  /* 初始输出低. */
  HAL_GPIO_WritePin(CALIB_RESET_KEY_PORT, CALIB_RESET_KEY_PIN, GPIO_PIN_RESET);
  
  /* 按键外部中断配置. */
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);
  HAL_NVIC_SetPriority(EXTI4_IRQn, 6, 0);

  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
  HAL_NVIC_SetPriority(EXTI0_IRQn, 6, 0);

  HAL_NVIC_EnableIRQ(EXTI3_IRQn);
  HAL_NVIC_SetPriority(EXTI3_IRQn, 6, 0);
}


