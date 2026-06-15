#include "Cus_Utils_delay.h"


/* ******************************************************* */
void Cus_delay_init( uint32_t SystemCoreClock_Hz );
void Cus_delay_us( uint32_t us );
void Cus_delay_ms( uint32_t ms );
/* ******************************************************* */


#if (CUS_DELAY_USE_DWT_TRACE)
  static uint32_t Cycle_us;

  void Cus_delay_init( uint32_t SystemCoreClock_Hz )
  {
    /* 跟踪系统使能 */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    /* 使能CYCCNT计数器 */
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    /* 复位CYCCNT. */
    DWT->CYCCNT = 0;

    /* 1us 所需计数时钟周期数 */
    Cycle_us = (SystemCoreClock_Hz / 1000000);
  }

  void Cus_delay_us( uint32_t us )
  {
    uint32_t tick_Start = DWT->CYCCNT;
    uint32_t tick_Delay = Cycle_us * us;
    uint32_t tick_End = tick_Delay + tick_Start;

    if ( tick_End > tick_Start )
    {
      /* 未溢出情况. */
      while( DWT->CYCCNT < tick_End );
    }
    else 
    {
      /* 数值溢出情况. */
      while( DWT->CYCCNT >= tick_Start );

      /* 绕回后保证继续延时直到End. */
      while( DWT->CYCCNT < tick_End );
    }
  }

  void Cus_delay_ms( uint32_t ms )
  {
    do 
    {
      Cus_delay_us(1000);
    } while(--ms);
  }
#endif /* CUS_DELAY_USE_DWT_TRACE */


#if (CUS_DELAY_USE_SYSTICK)
  static uint32_t Cycle_us;

  void Cus_delay_init( uint32_t SystemCoreClock_Hz )
  {
    /* 检查Systick是否已启动. 未启动则配置Systick */
    if ( !(SysTick->CTRL & SysTick_CTRL_ENABLE_Msk) )  
    {
      /* 产生 1ms 定时. */
      uint32_t loadVal = (SystemCoreClock_Hz / 1000) - 1;
      if ( loadVal > SysTick_LOAD_RELOAD_Msk )  
      {
        /* 静默返回.  */
        Cycle_us = 0;
        return;
      }

      SysTick->LOAD = loadVal;
      SysTick->VAL = 0;
      NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
      SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk | 
                        SysTick_CTRL_CLKSOURCE_Msk | 
                        SysTick_CTRL_TICKINT_Msk);
    } 

    /* 检查当前CLOCKSOURCE模式(不同CLOCKSOURCE工作频率不同. 计数频率也不同) */
    if ( SysTick->CTRL & SysTick_CTRL_CLKSOURCE_Msk )
    {
      /* CLK = AHB */
      Cycle_us = SystemCoreClock_Hz / 1000000;
    }
    else 
    {
      /* CLK = (1/8)*AHB */
      Cycle_us = (SystemCoreClock_Hz / 8) / 1000000;
    }
  }

  void Cus_delay_us( uint32_t us )
  {
    if ( Cycle_us == 0 )    return;

    uint32_t tick_delay = Cycle_us * us;
    uint32_t tick_start = SysTick->VAL;
    uint32_t elapsed;

    do 
    {
      elapsed = (tick_start - SysTick->VAL) & SysTick_LOAD_RELOAD_Msk;
    } while( elapsed < tick_delay );
  }

  void Cus_delay_ms( uint32_t ms )
  {
    do
    {
      Cus_delay_us(1000);
    } while(--ms);
  }
#endif /* CUS_DELAY_USE_SYSTICK */
