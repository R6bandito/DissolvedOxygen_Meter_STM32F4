/**
  ******************************************************************************
  * @file    Templates/Src/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2017 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
#include "uart_cmd_task.h"
#include "key.h"
#include "key_task.h"
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"


extern TIM_HandleTypeDef htim6;
extern UART_HandleTypeDef huart_cmd; 
extern uint8_t uart_rcvByte;                              // 串口接收字节.
extern uint8_t cmd_buf[CMD_BUFFER_SIZE];                  // 串口命令缓冲区.
extern QueueHandle_t g_keyEventQueue;                     // 按键事件队列.

extern TaskHandle_t getUartCmdTask_Handle( void );
extern void Cus_UART_StartTransfer( void );

static uint8_t cmd_temp_buf[CMD_BUFFER_SIZE];             // 命令暂存缓冲区.
static TickType_t last_rx_tick;                             
/* ------------------------------------------------------------------ */

void TIM6_DAC_IRQHandler( void )
{
  HAL_TIM_IRQHandler(&htim6);
}

void HAL_TIM_PeriodElapsedCallback( TIM_HandleTypeDef *htim )
{
  if ( htim->Instance == TIM6 )
  {
    HAL_IncTick();
  }
}


void EXTI0_IRQHandler( void )
{
  static TickType_t exti0_LastWakeUpTick = 0;
  static uint8_t guard = 0;               // 确保LastWakeUp在一次按下与释放周期（两次中断进入）中，只被更新第一次.
  static uint8_t pressPinLevel_1 = 1;     // 第一次采样电平记录. (该按键默认复位为高电平)
  static uint8_t pressPinLevel_2 = 1;     // 第二次采样电平记录.
  static TickType_t lastValidTick = 0;    // 该Tick用于滤除抖动.

  /* KEY3. */
  if ( __HAL_GPIO_EXTI_GET_IT(CALIB_RESET_KEY_PIN) != RESET )
  {
    /* 先清中断标志. */
    __HAL_GPIO_EXTI_CLEAR_IT(CALIB_RESET_KEY_PIN);

    /* 非阻塞滤抖动. */
    TickType_t currentTick = xTaskGetTickCountFromISR();
    if ( currentTick - lastValidTick < pdMS_TO_TICKS(30) )
    {
      return;
    }
    lastValidTick = currentTick;

    if ( !guard )
    {
      /* 记录时间戳. */
      exti0_LastWakeUpTick = xTaskGetTickCountFromISR();
      guard = 1;

      /* 采集当前电平状态. */
      pressPinLevel_1 = (uint8_t)HAL_GPIO_ReadPin(CALIB_RESET_KEY_PORT, CALIB_RESET_KEY_PIN);
    }
    else 
    {
      /* 第二次采样电平. */
      pressPinLevel_2 = (uint8_t)HAL_GPIO_ReadPin(CALIB_RESET_KEY_PORT, CALIB_RESET_KEY_PIN);

      TickType_t during = xTaskGetTickCountFromISR() - exti0_LastWakeUpTick ;

      if ( (during < pdMS_TO_TICKS(LONG_PRESS_IDENTIFY_MS)) && (pressPinLevel_1 != pressPinLevel_2) )
      {
        /* 短按. */
        /* 通知按键短按事件到达. */
        uint8_t eve = KEY3_SHORT_PRESS;
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(g_keyEventQueue, &eve, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
      }
      else if ( during >= pdMS_TO_TICKS(LONG_PRESS_IDENTIFY_MS) )
      {
        /* 通知按键长按事件到达. */
        uint8_t eve = KEY3_LONG_PRESS;
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(g_keyEventQueue, &eve, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
      }

      /* 更新时间戳. 恢复状态. */
      exti0_LastWakeUpTick = xTaskGetTickCountFromISR();
      guard = 0;
      pressPinLevel_1 = 1;
      pressPinLevel_2 = 1;
    }
  }
}


void EXTI4_IRQHandler( void )
{
  static TickType_t exti4_LastWakeUpTick = 0;
  static uint8_t guard = 0;               // 确保LastWakeUp在一次按下与释放周期（两次中断进入）中，只被更新第一次.
  static uint8_t pressPinLevel_1 = 1;     // 第一次采样电平记录. (该按键默认复位为高电平)
  static uint8_t pressPinLevel_2 = 1;     // 第二次采样电平记录.
  static TickType_t lastValidTick = 0;    // 该Tick用于滤除抖动.

  /* 零点校准. */
  if ( __HAL_GPIO_EXTI_GET_IT(CALIB_ZERO_KEY_PIN) != RESET )
  {
    /* 先清中断标志. */
    __HAL_GPIO_EXTI_CLEAR_IT(CALIB_ZERO_KEY_PIN);

    /* 非阻塞滤抖动. */
    TickType_t currentTick = xTaskGetTickCountFromISR();
    if ( currentTick - lastValidTick < pdMS_TO_TICKS(30) )
    {
      return;
    }
    lastValidTick = currentTick;

    if ( !guard )
    {
      /* 记录时间戳. */
      exti4_LastWakeUpTick = xTaskGetTickCountFromISR();
      guard = 1;

      /* 采集当前电平状态. */
      pressPinLevel_1 = (uint8_t)HAL_GPIO_ReadPin(CALIB_ZERO_KEY_PORT, CALIB_ZERO_KEY_PIN);
    }
    else 
    {
      /* 第二次采样电平. */
      pressPinLevel_2 = (uint8_t)HAL_GPIO_ReadPin(CALIB_ZERO_KEY_PORT, CALIB_ZERO_KEY_PIN);

      TickType_t during = xTaskGetTickCountFromISR() - exti4_LastWakeUpTick ;

      if ( (during < pdMS_TO_TICKS(LONG_PRESS_IDENTIFY_MS)) && (pressPinLevel_1 != pressPinLevel_2) )
      {
        /* 短按. */
        /* 通知按键短按事件到达. */
        uint8_t eve = KEY1_SHORT_PRESS;
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(g_keyEventQueue, &eve, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
      }
      else if ( during >= pdMS_TO_TICKS(LONG_PRESS_IDENTIFY_MS) )
      {
        /* 通知按键长按事件到达. */
        uint8_t eve = KEY1_LONG_PRESS;
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(g_keyEventQueue, &eve, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
      }

      /* 更新时间戳. 恢复状态. */
      exti4_LastWakeUpTick = xTaskGetTickCountFromISR();
      guard = 0;
      pressPinLevel_1 = 1;
      pressPinLevel_2 = 1;
    }
  }
}


void EXTI3_IRQHandler( void )
{
  static TickType_t exti3_LastWakeUpTick = 0;
  static uint8_t guard = 0;               
  static uint8_t pressPinLevel_1 = 1;     
  static uint8_t pressPinLevel_2 = 1;     
  static TickType_t lastValidTick = 0;    

  /* KEY2. */
  if ( __HAL_GPIO_EXTI_GET_IT(CALIB_AIR_KEY_PIN) != RESET )
  {
    __HAL_GPIO_EXTI_CLEAR_IT(CALIB_AIR_KEY_PIN);

    /* 非阻塞滤抖动. */
    TickType_t currentTick = xTaskGetTickCountFromISR();
    if ( currentTick - lastValidTick < pdMS_TO_TICKS(30) )
    {
      return;
    }
    lastValidTick = currentTick;

    if ( !guard )
    {
      /* 记录时间戳. */
      exti3_LastWakeUpTick = xTaskGetTickCountFromISR();
      guard = 1;

      /* 采集当前电平状态. */
      pressPinLevel_1 = (uint8_t)HAL_GPIO_ReadPin(CALIB_AIR_KEY_PORT, CALIB_AIR_KEY_PIN);
    }
    else 
    {
      /* 第二次采样电平. */
      pressPinLevel_2 = (uint8_t)HAL_GPIO_ReadPin(CALIB_AIR_KEY_PORT, CALIB_AIR_KEY_PIN);

      TickType_t during = xTaskGetTickCountFromISR() - exti3_LastWakeUpTick ;

      if ( (during < pdMS_TO_TICKS(LONG_PRESS_IDENTIFY_MS)) && (pressPinLevel_1 != pressPinLevel_2) )
      {
        /* 短按. */
        /* 通知按键短按事件到达. */
        uint8_t eve = KEY2_SHORT_PRESS;
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(g_keyEventQueue, &eve, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
      }
      else if ( during >= pdMS_TO_TICKS(LONG_PRESS_IDENTIFY_MS) )
      {
        /* 通知按键长按事件到达. */
        uint8_t eve = KEY2_LONG_PRESS;
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(g_keyEventQueue,&eve, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
      }

      /* 更新时间戳. 恢复状态. */
      exti3_LastWakeUpTick = xTaskGetTickCountFromISR();
      guard = 0;
      pressPinLevel_1 = 1;
      pressPinLevel_2 = 1;
    }
  }
}


void USART2_IRQHandler( void )
{
  HAL_UART_IRQHandler(&huart_cmd);
}

void HAL_UART_RxCpltCallback( UART_HandleTypeDef *huart )
{
  if ( huart->Instance == USART2 )
  {
    static uint8_t recv_count = 0;

    /* 每个字节到来时都记录时间戳. */
    TickType_t now = xTaskGetTickCountFromISR();
    if ( recv_count > 0 && (now - last_rx_tick) > pdMS_TO_TICKS(100) )
    {
      /* 设计该超时机制主要为了解决第一次输入失败后，第二次输入无效问题. */
      recv_count = 0;
    }
    last_rx_tick = now;

    if ( recv_count > 0 && uart_rcvByte == '\n' && cmd_temp_buf[(recv_count - 1)] == '\r' )
    {
      if ( recv_count + 1 >= CMD_BUFFER_SIZE - 1 )  
      {
        /* 超限. */
        recv_count = 0;
        goto END;
      }

      /* 检测到结束标志 \r\n. */
      cmd_temp_buf[recv_count++] = uart_rcvByte;

      uint8_t len = recv_count - 2;

      /* 将暂存缓冲区数据拷贝到命令缓冲区供任务处理. */
      uint32_t basepri = taskENTER_CRITICAL_FROM_ISR();
      memcpy(cmd_buf, cmd_temp_buf, len);
      cmd_buf[len] = '\0';
      taskEXIT_CRITICAL_FROM_ISR(basepri);

      /* 从上位机收到一条命令. 通知串口命令处理任务. */
      BaseType_t xHigherPriorityTaskWoken = pdFALSE;
      vTaskNotifyGiveFromISR(getUartCmdTask_Handle(), &xHigherPriorityTaskWoken);
      portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

      recv_count = 0;
    }
    else 
    {
      /* 命令接收阶段. */
      cmd_temp_buf[recv_count++] = uart_rcvByte;

      if ( recv_count >= CMD_BUFFER_SIZE - 1 )
      {
        /* 命令超限. 覆盖原缓冲区. */
        recv_count = 0;
      }
    }

END:
    /* 继续接收. */
    Cus_UART_StartTransfer();
  }
}


/* ------------------------------------------------------------------ */
/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
// void SVC_Handler(void)
// {
// }

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
// void PendSV_Handler(void)
// {
// }

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
// void SysTick_Handler(void)
// {
//   HAL_IncTick();
// }

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/


/**
  * @}
  */ 

/**
  * @}
  */
