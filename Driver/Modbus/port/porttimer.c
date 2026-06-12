/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "mb_timer.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvTIMERExpiredISR( void );

extern TIM_HandleTypeDef htim_mb;
/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
    Cus_Mb_Timer_Init();
    return TRUE;
}


inline void
vMBPortTimersEnable(  )
{
    WRITE_REG(htim_mb.Instance->CNT, 0);
    SET_BIT(htim_mb.Instance->CR1, TIM_CR1_CEN);
}

inline void
vMBPortTimersDisable(  )
{
    CLEAR_BIT(htim_mb.Instance->CR1, TIM_CR1_CEN);
}

/* Create an ISR which is called whenever the timer has expired. This function
 * must then call pxMBPortCBTimerExpired( ) to notify the protocol stack that
 * the timer has expired.
 */
static void prvvTIMERExpiredISR( void )
{
    ( void )pxMBPortCBTimerExpired(  );
}


void TIM7_IRQHandler( void )
{
    if ( __HAL_TIM_GET_FLAG(&htim_mb, TIM_FLAG_UPDATE) )
    {
        __HAL_TIM_CLEAR_FLAG(&htim_mb, TIM_FLAG_UPDATE);
        prvvTIMERExpiredISR();
    }
}

