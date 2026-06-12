#include "mb_timer.h"


/* *********************** */
TIM_HandleTypeDef htim_mb;
/* *********************** */


/* ******************************* */
void Cus_Mb_Timer_Init( void );
/* ******************************* */


void Cus_Mb_Timer_Init( void )
{
  /* 注意: 该定时器参数是基于 168Mhz 主频的基础上进行配置. */
  __HAL_RCC_TIM7_CLK_ENABLE();

  uint32_t apb1_c = HAL_RCC_GetPCLK1Freq();

  htim_mb.Instance                          = TIM7;
  htim_mb.Init.AutoReloadPreload            = TIM_AUTORELOAD_PRELOAD_DISABLE;
  htim_mb.Init.ClockDivision                = TIM_CLOCKDIVISION_DIV1;
  htim_mb.Init.CounterMode                  = TIM_COUNTERMODE_UP;
  htim_mb.Init.Period                       = 1800 - 1;         // 1800us超时.(T3.5)
  htim_mb.Init.Prescaler                    = (apb1_c / 1000000) - 1;      // 1us.
  htim_mb.Init.RepetitionCounter            = 0;

  HAL_TIM_Base_Init(&htim_mb);

  HAL_NVIC_EnableIRQ(TIM7_IRQn);
  HAL_NVIC_SetPriority(TIM7_IRQn, 5, 0);

  __HAL_TIM_CLEAR_FLAG(&htim_mb, TIM_IT_UPDATE);
  __HAL_TIM_ENABLE_IT(&htim_mb, TIM_IT_UPDATE);
}

