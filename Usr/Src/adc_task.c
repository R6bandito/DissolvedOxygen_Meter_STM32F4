#include "adc_task.h"


/* ***************************************** */
typedef struct 
{
  const char *identify;           
  uint8_t head;                         // 环形缓冲区头.(写)
  uint8_t tail;                         // 环形缓冲区尾.(读)
  uint16_t cur_val;                     // 当前最新采样值.
  uint32_t sum;                         // 缓冲区数值总和.
  uint16_t filterBuffer[FILTER_N];      // 滤波采样缓冲区.

} adc_Filter_t;

static adc_Filter_t adc_FilterInstance_O2;          // O2滤波结构体.
static adc_Filter_t adc_FilterInstance_Temp;        // Temp滤波结构体.
/* ***************************************** */

/* **************************************** */
static void update_filter( adc_Filter_t *filter, uint16_t new_val );          // 平均滑动滤波更新.
static uint16_t get_current_filter_average( adc_Filter_t *filter );           // 获取当前最新的滤波数据.
/* **************************************** */

/* **************************************** */
void cTask_ADC( void *parameter );

/* 该任务使用平均滑动滤波定期更新采样数据. */
void cTask_Update( void *parameter );
/* **************************************** */


static void update_filter( adc_Filter_t *filter, uint16_t new_val )
{
  if ( !filter )    return;

  if ( (filter->head + 1) % FILTER_N != filter->tail )
  {
    /* 缓冲区未满. 此时只存入数据并且计算总和. 不减去最旧值. */
    filter->cur_val = new_val;

    /* 将新数据写入缓冲区. */
    filter->filterBuffer[filter->head] = filter->cur_val;

    /* 更新head索引. */
    filter->head = (filter->head + 1) % FILTER_N;

    /* 计算总和. */
    filter->sum += new_val;

    return;
  }

  /* 缓冲区已满. 新总和 = 旧总和 - 最旧值 + 新值. */
  filter->cur_val = new_val;

  /* 新总和. */
  filter->sum -= filter->filterBuffer[filter->tail];
  filter->sum += new_val;

  /* 更新tail索引(移除最旧数据). */
  filter->tail = (filter->tail + 1) % FILTER_N;

  /* 向缓冲区写入最新数据. */
  filter->filterBuffer[filter->head] = filter->cur_val;

  /* 更新head索引. */
  filter->head = (filter->head + 1) % FILTER_N;

  /* 下一次进入时 (filter->head + 1) % FILTER_N != filter->tail 依然为false. 在接下来的运行中再不进入. */
}


static uint16_t get_current_filter_average( adc_Filter_t *filter )
{
  if ( !filter )    return;

  /* 当前缓冲区有效数据个数. (环形缓冲区中 head 与 tail 之间的距离) */
  uint8_t N = (filter->head - filter->tail + FILTER_N) % FILTER_N;
  if ( N == 0 )   return;

  return (uint16_t)(filter->sum / N);
}


void cTask_ADC( void *parameter )
{

  while(1)
  {

  }
}


void cTask_Update( void *parameter )
{
  adc_FilterInstance_O2.identify = "O2_Strucut";
  adc_FilterInstance_Temp.identify = "Temp_Struct";

  /* 记录唤醒时间戳. */
  TickType_t xLastWakeUpTime = xTaskGetTickCount();

  while (1)
  {
    /* 获取生值(ADC采样裸值) */
    uint16_t O2_Raw = Cus_getLatestO2();
    uint16_t Temp_Raw = Cus_getLatestTemp();

    /* 滑动平均滤波. */
    update_filter(&adc_FilterInstance_O2, O2_Raw);
    update_filter(&adc_FilterInstance_Temp, Temp_Raw);

    /* 严格150ms转换一次. */
    vTaskDelayUntil(&xLastWakeUpTime, pdMS_TO_TICKS(150));
  }
}


