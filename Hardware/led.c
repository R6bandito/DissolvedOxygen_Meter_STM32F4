#include "led.h"



/* **************************** */
void Cus_Debug_LED_Init( void );
void runWrLEDSwitch( uint8_t EN_DIS );
/* **************************** */


void Cus_Debug_LED_Init( void )
{
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStructure;

  GPIO_InitStructure.Mode       = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStructure.Pin        = RUN_WR_LED_PIN;
  GPIO_InitStructure.Pull       = GPIO_PULLUP;
  GPIO_InitStructure.Speed      = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStructure.Alternate  = 0;

  HAL_GPIO_Init(RUN_WR_LED_PORT, &GPIO_InitStructure);
}


void runWrLEDSwitch( uint8_t EN_DIS )
{
  HAL_GPIO_WritePin(RUN_WR_LED_PORT, RUN_WR_LED_PIN, (GPIO_PinState)EN_DIS);
}



