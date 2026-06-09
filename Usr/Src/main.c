#include "main.h"

/* *************************************** */
TIM_HandleTypeDef htim6;
void SystemClock_Config( void );
HAL_StatusTypeDef HAL_InitTick( uint32_t TickPriority );
/* *************************************** */

void task1( void *param );

int main( void )
{
	HAL_Init();

	SystemClock_Config();

	debug_uart_Init();

	printf("======================== System Start ========================\n");

	while(1)
	{

	}
}

void task1( void *param )
{

}



HAL_StatusTypeDef HAL_InitTick( uint32_t TickPriority )
{
	/* 重定义 HAL_InitTick ，进行时基分离. Systick交由FreeRTOS配置. */
	/* 初始化基本定时器 TIM6 作为HAL库的tick时基基准. */
	__HAL_RCC_TIM6_CLK_ENABLE();
	
	htim6.Instance = TIM6;
	htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	htim6.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim6.Init.CounterMode = TIM_COUNTERMODE_UP;

	uint32_t pclk1_freq = HAL_RCC_GetPCLK1Freq();
	uint32_t presc = (2 * pclk1_freq / 1000000 == 0) ? (2 * pclk1_freq / 100000) : (2 * pclk1_freq / 1000000);

	// Period = (1MHz * 0.001s) - 1 = 1000 - 1 = 999. (0.001s = 1ms)
	htim6.Init.Period = 1000 - 1;
	htim6.Init.Prescaler = presc - 1;
	htim6.Init.RepetitionCounter = 0;

	if ( HAL_TIM_Base_Init(&htim6) != HAL_OK )
	{
		for( ; ; );
	}

	HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
	HAL_NVIC_SetPriority(TIM6_DAC_IRQn, TickPriority, 0);

	HAL_TIM_Base_Start_IT(&htim6);
	return HAL_OK;
}



void SystemClock_Config( void )
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;                     
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;                
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;        // PLL时钟源 = HSE
    RCC_OscInitStruct.PLL.PLLM = 8;                             // 分频因子: 8MHz / 8 = 1MHz
    RCC_OscInitStruct.PLL.PLLN = 336;                           // 倍频因子: 1MHz * 336 = 336MHz
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;                 // 分频输出: 336MHz / 2 = 168MHz
    RCC_OscInitStruct.PLL.PLLQ = 7;                             
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
			for( ; ; );
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;   // SYSCLK = PLL输出 168MHz
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;          // HCLK = SYSCLK/1 = 168MHz
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;           // APB1 = HCLK/4 = 42MHz
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;           // APB2 = HCLK/2 = 84MHz
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
    {
			for( ; ; );
    }
}






