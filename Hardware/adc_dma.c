#include "adc_dma.h"


/* ****************** Global & Static ***************** */
static ADC_HandleTypeDef hadc1;
static DMA_HandleTypeDef hdma1;

static uint16_t recvBuf[2];
/* ****************** Global & Static ***************** */

/* ****************** Decalre ***************** */
void Cus_ADC_Init( void );
void Cus_ADC_SampleStart( void );
void Cus_DMA_Init( void );

static void HX_ADC_Init( void );
/* ****************** Decalre ***************** */


static void HX_ADC_Init( void )
{
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitTypeDef ADC_GPIOInitstructure;

  ADC_GPIOInitstructure.Mode = GPIO_MODE_ANALOG;    // 模拟量输入.
  ADC_GPIOInitstructure.Pin = CUS_ADC_O2_IN_PIN;
  ADC_GPIOInitstructure.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(CUS_ADC_O2_IN_PORT, &ADC_GPIOInitstructure);

  ADC_GPIOInitstructure.Pin = CUS_ADC_TEMP_IN_PIN;
  HAL_GPIO_Init(CUS_ADC_TEMP_IN_PORT, &ADC_GPIOInitstructure);
}


void Cus_ADC_Init( void )
{
  /* ADC采样引脚初始化. */
  HX_ADC_Init();

  /* 开ADC1时钟. */
  __HAL_RCC_ADC1_CLK_ENABLE();

  hadc1.Instance                      = ADC1;
  hadc1.Init.ClockPrescaler           = ADC_CLOCKPRESCALER_PCLK_DIV4;         // PCLK2 4分频. 21Mhz.
  hadc1.Init.ContinuousConvMode       = ENABLE;                               // 连线转换.
  hadc1.Init.DataAlign                = ADC_DATAALIGN_RIGHT;                  // 向右对齐.
  hadc1.Init.DiscontinuousConvMode    = DISABLE;                              // 不使用间断模式.(一次性转换完两个通道)
  hadc1.Init.DMAContinuousRequests    = ENABLE;                               // DMA连续请求.
  hadc1.Init.EOCSelection             = ADC_EOC_SEQ_CONV;                     // 所有通道采集完置标志.
  hadc1.Init.ExternalTrigConv         = ADC_SOFTWARE_START;                   // 软件触发.(触发一次连续运行)
  hadc1.Init.ExternalTrigConvEdge     = ADC_EXTERNALTRIG_EDGE_NONE;
  hadc1.Init.NbrOfConversion          = 2;                                    // 两个通道(溶解氧信号量采集通道，温度信号采集通道).
  hadc1.Init.NbrOfDiscConversion      = 0;
  hadc1.Init.Resolution               = ADC_RESOLUTION_12B;                   // ADC 12位分辨率.
  hadc1.Init.ScanConvMode             = ENABLE;                               // 多通道扫描.

  if ( HAL_ADC_Init(&hadc1) != HAL_OK )
  {
    for( ; ; );
  }

  /* 采样通道参数配置. */
  ADC_ChannelConfTypeDef adc_channel1_O2;
  adc_channel1_O2.Channel             = ADC_CHANNEL_1;                         // ADC1_IN1.
  adc_channel1_O2.Rank                = 1;                                     
  adc_channel1_O2.SamplingTime        = ADC_SAMPLETIME_480CYCLES;
  HAL_ADC_ConfigChannel(&hadc1, &adc_channel1_O2);

  ADC_ChannelConfTypeDef adc_channel2_temp;
  adc_channel2_temp.Channel           = ADC_CHANNEL_4;                          // ADC1_IN4.
  adc_channel2_temp.Rank              = 2;
  adc_channel2_temp.SamplingTime      = ADC_SAMPLETIME_480CYCLES;
  HAL_ADC_ConfigChannel(&hadc1, &adc_channel2_temp);

  /* 将ADC连接到DMA. */
  __HAL_LINKDMA(&hadc1, DMA_Handle, hdma1);
}


void Cus_DMA_Init( void )
{
  /* 开DMA时钟. */
  __HAL_RCC_DMA2_CLK_ENABLE();

  hdma1.Instance                      = DMA2_Stream4;
  hdma1.Init.Channel                  = DMA_CHANNEL_0;                  // DMA2_Stream4_Channel0 = ADC1.
  hdma1.Init.Direction                = DMA_PERIPH_TO_MEMORY;           // 外设 到 RAM.
  hdma1.Init.FIFOMode                 = DISABLE;
  hdma1.Init.FIFOThreshold            = DMA_FIFO_THRESHOLD_HALFFULL;
  hdma1.Init.MemBurst                 = DMA_MBURST_SINGLE;
  hdma1.Init.MemDataAlignment         = DMA_MDATAALIGN_HALFWORD;        // RAM半字对齐(16位).
  hdma1.Init.MemInc                   = DMA_MINC_ENABLE;                // 数据缓冲区地址递增.
  hdma1.Init.Mode                     = DMA_CIRCULAR;                   // 循环模式.
  hdma1.Init.PeriphBurst              = DMA_PBURST_SINGLE;
  hdma1.Init.PeriphDataAlignment      = DMA_PDATAALIGN_HALFWORD;        // 外设半字对齐(16位).
  hdma1.Init.PeriphInc                = DMA_PINC_DISABLE;               // 外设地址不递增.
  hdma1.Init.Priority                 = DMA_PRIORITY_MEDIUM;            // 中优先级.

  HAL_DMA_Init(&hdma1);
}


void Cus_ADC_SampleStart( void )
{
  /* 启动ADC-DMA传输. */
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)recvBuf, 2);
}


uint16_t Cus_getLatestO2( void )
{
  return recvBuf[CUS_ADC_O2_INDEX];
}


uint16_t Cus_getLatestTemp( void )
{
  return recvBuf[CUS_ADC_TEMP_INDEX];
}
