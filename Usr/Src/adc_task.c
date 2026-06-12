#include "adc_task.h"
#include "do_data.h"
#include "queue.h"
#include <math.h>


/* ************************************ */
/* 饱和溶氧含量温度对照表. (0 - 40°) */
static const float do_sat_table[41] = 
{
  14.60, 14.22, 13.86, 13.52, 13.19,  // 0-4℃
  12.77, 12.46, 12.17, 11.89, 11.62,  // 5-9℃
  11.29, 11.02, 10.76, 10.52, 10.29,  // 10-14℃
  10.08,  9.86,  9.66,  9.46,  9.27,  // 15-19℃
   9.08,  8.90,  8.72,  8.55,  8.38,  // 20-24℃
   8.24,  8.09,  7.95,  7.81,  7.67,  // 25-29℃
   7.56,  7.44,  7.33,  7.22,  7.12,  // 30-34℃
   6.99,  6.88,  6.77,  6.66,  6.50   // 35-39℃, 40℃
};
/* ************************************ */


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

typedef struct 
{
  uint16_t zero_adc;                                // 零点校准ADC值.
  uint16_t air_adc;                                 // 空气校准ADC值.
  float air_sat;                                    // 空气校准时的溶解氧值.
  float air_temp_c;                                 // 空气校准时的温度.    

} calibParams_t;
calibParams_t adc_calibParams;                      // 全局校准参数.

/* 数据队列相关. */
QueueHandle_t doDataQueue;
doData_t doData;

/* Modbus 保持寄存器数组. */
extern uint16_t REG_HOLD_BUF[6];
/* ***************************************** */

/* **************************************** */
static void update_filter( adc_Filter_t *filter, uint16_t new_val );          // 平均滑动滤波更新.
static uint16_t get_current_filter_average( adc_Filter_t *filter );           // 获取当前最新的滤波数据.
static float ADC_temp_convert2C( uint16_t adc_val );                          // 将ADC模拟量转换为温度(摄氏度).
/* **************************************** */

/* **************************************** */
void cTask_ADC( void *parameter );

/* 该任务使用平均滑动滤波定期更新采样数据. */
void cTask_Update( void *parameter );


void calib_sync( void );            
void calib_defaultInit( void );
void calib_zero( void );
void calib_air( void );
uint16_t get_ADC_O2( void );
uint16_t get_ADC_Temp( void );
void get_CalibParam( uint16_t *o_ZeroAdc, uint16_t *o_AirAdc, float *o_AirSat, float *o_AirTemp );
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
  if ( !filter )    return 0;

  /* 当前缓冲区有效数据个数. (环形缓冲区中 head 与 tail 之间的距离) */
  taskENTER_CRITICAL();
  uint8_t N = (filter->head - filter->tail + FILTER_N) % FILTER_N;
  uint32_t now_sum = filter->sum;
  taskEXIT_CRITICAL();

  if ( N == 0 )   return 0;

  return (uint16_t)(now_sum / N);
}


static float ADC_temp_convert2C( uint16_t adc_val )
{
  /* 采用B值法将ADC采集的NTC热敏电阻模块输出模拟量转换为摄氏度. */
  float volt = adc_val * 3.3f / 4095.0f;

  /* 限幅. */
  if ( volt < 0.001f )   volt = 0.001f;
  if ( volt > 3.299f )   volt = 3.299f;

  float r_ntc = NTC_R_PULL * volt / (3.3f - volt);
  float invert_temp_k = 1.0f / NTC_T0_K + (1.0f / NTC_B) * logf(r_ntc / NTC_R0);
  float temp_k = 1.0f / invert_temp_k;    // 取倒数.

  return (temp_k - 273.15);     // 算出来的温度是开尔文. 需要转化为摄氏度.
}


uint16_t get_ADC_O2( void )
{
  return get_current_filter_average(&adc_FilterInstance_O2);
}


uint16_t get_ADC_Temp( void )
{
  return get_current_filter_average(&adc_FilterInstance_Temp);
}


void calib_zero( void )
{
  /* 将零点命令时的滤波过后的ADC值存入校准参数结构体. */
  adc_calibParams.zero_adc = get_ADC_O2();
  REG_HOLD_BUF[2] = adc_calibParams.zero_adc;
}


void calib_air( void )
{
  /* 获取滤波ADC采样值 并且转换为摄氏度. */
  uint16_t temp_adc = get_ADC_Temp();
  float temp = ADC_temp_convert2C(temp_adc);

  /* 查表获取当前温度所对应的饱和溶解氧含量. */
  int8_t index = (int8_t)(temp + 0.5f);   
  if ( index < 0 )  index = 0;
  if ( index > 40 ) index = 40;
  float sat_do = do_sat_table[index];

  /* 获取当前空气校准中的 O2 ADC采样值. */
  uint16_t air_adc = get_ADC_O2();

  /* 将数据更新到全局结构体中. */
  taskENTER_CRITICAL();
  adc_calibParams.air_adc = air_adc;
  adc_calibParams.air_sat = sat_do;
  adc_calibParams.air_temp_c = temp;

  REG_HOLD_BUF[3] = adc_calibParams.air_adc;
  REG_HOLD_BUF[4] = adc_calibParams.air_sat * 100;
  REG_HOLD_BUF[5] = adc_calibParams.air_temp_c * 10;
  taskEXIT_CRITICAL();
}


void calib_defaultInit( void )
{
  taskENTER_CRITICAL();
  adc_calibParams.air_adc = CALIB_DEFAULT_AIR_ADC;
  adc_calibParams.air_sat = CALIB_DEFAULT_AIR_SAT;
  adc_calibParams.air_temp_c = CALIB_DEFAULT_AIR_TEMP;
  adc_calibParams.zero_adc = CALIB_DEFAULT_ZERO_ADC;

  REG_HOLD_BUF[2] = CALIB_DEFAULT_ZERO_ADC;
  REG_HOLD_BUF[3] = CALIB_DEFAULT_AIR_ADC;
  REG_HOLD_BUF[4] = CALIB_DEFAULT_AIR_SAT * 100;
  REG_HOLD_BUF[5] = CALIB_DEFAULT_AIR_TEMP * 10;
  taskEXIT_CRITICAL();
}


void calib_sync( void )
{
  taskENTER_CRITICAL();
  adc_calibParams.zero_adc = REG_HOLD_BUF[2];
  adc_calibParams.air_adc = REG_HOLD_BUF[3];
  adc_calibParams.air_sat = REG_HOLD_BUF[4] / 100.0f;
  adc_calibParams.air_temp_c = REG_HOLD_BUF[5] / 10.0f;

  taskEXIT_CRITICAL();
}


void get_CalibParam( uint16_t *o_ZeroAdc, uint16_t *o_AirAdc, float *o_AirSat, float *o_AirTemp )
{
  taskENTER_CRITICAL();
  if ( o_ZeroAdc )    *o_ZeroAdc = adc_calibParams.zero_adc;
  if ( o_AirAdc )     *o_AirAdc = adc_calibParams.air_adc;
  if ( o_AirSat )     *o_AirSat = adc_calibParams.air_sat;
  if ( o_AirTemp )    *o_AirTemp = adc_calibParams.air_temp_c;
  taskEXIT_CRITICAL();
}


void cTask_ADC( void *parameter )
{
  
  while(1)
  {
    /* 获取当前O2滤波后ADC采样值 与 当前温度值. */
    uint16_t adc_o2 = get_ADC_O2();
    float temp = ADC_temp_convert2C(get_ADC_Temp());

    /* 查表获得当前温度下的饱和溶解氧值. */
    int8_t index = (int8_t)(temp + 0.5f);
    if ( index < 0 )    index = 0;
    if ( index > 40 )   index = 40;
    float current_sat_do = do_sat_table[index];

    uint16_t zero, air_adc;
    float air_sat;

    taskENTER_CRITICAL();
    zero = adc_calibParams.zero_adc;
    air_adc = adc_calibParams.air_adc;
    air_sat = adc_calibParams.air_sat;
    taskEXIT_CRITICAL();

    if ( air_adc <= zero ) air_adc = zero + 1;
    if ( air_sat <= 0.0f ) air_sat = 9.08f; 

    /* 计算未经温度补偿的测量浓度. */
    float do_means = (adc_o2 - zero) * air_sat / (air_adc - zero);

    /* 计算温度补偿后的最终浓度. */
    float do_final = do_means * current_sat_do / air_sat;

    if ( do_final < 0 )   do_final = 0;

    /* 转换后的新数据覆写入队列. */
    doData.dissolved_oxygen = do_final;
    doData.temperature = temp;
    xQueueOverwrite(doDataQueue, &doData);

    /* 写入Modbus保持寄存器. */
    taskENTER_CRITICAL();
    REG_HOLD_BUF[0] = doData.dissolved_oxygen * 100;      // Modbus寄存器只接收整数.此处扩大100倍处理.
    REG_HOLD_BUF[1] = doData.temperature * 10;            // 温度扩大10倍处理.
    taskEXIT_CRITICAL();

    /* 每500ms周期更新一次. */
    vTaskDelay(pdMS_TO_TICKS(500));
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


