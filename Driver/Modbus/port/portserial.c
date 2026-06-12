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

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "stm32f4xx_hal.h"
#include "mb_uart.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR( void );
static void prvvUARTRxISR( void );

extern UART_HandleTypeDef huart_mb;
/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    if ( xRxEnable )
    {
        SET_BIT(huart_mb.Instance->CR1, USART_CR1_RXNEIE);
    }
    else 
    {
        CLEAR_BIT(huart_mb.Instance->CR1, USART_CR1_RXNEIE);
    }

    if ( xTxEnable )
    {
        SET_BIT(huart_mb.Instance->CR1, USART_CR1_TXEIE);
    }
    else 
    {
        CLEAR_BIT(huart_mb.Instance->CR1, USART_CR1_TXEIE);
    }
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity, UCHAR ucStopBits )
{
    Cus_Mb_UART_Init();
    return TRUE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    huart_mb.Instance->DR = ucByte;
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    *pucByte = (char)(huart_mb.Instance->DR & 0xFF);
    return TRUE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
static void prvvUARTTxReadyISR( void )
{
    pxMBFrameCBTransmitterEmpty(  );
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void prvvUARTRxISR( void )
{
    pxMBFrameCBByteReceived(  );
}


void USART3_IRQHandler( void )
{
    if ( __HAL_UART_GET_FLAG(&huart_mb, UART_FLAG_RXNE) )
    {
        __HAL_UART_CLEAR_FLAG(&huart_mb, UART_FLAG_RXNE);
        prvvUARTRxISR();
    }

    if ( __HAL_UART_GET_FLAG(&huart_mb, UART_FLAG_TXE) )
    {
        __HAL_UART_CLEAR_FLAG(&huart_mb, UART_FLAG_TXE);
        prvvUARTTxReadyISR();
    }
}
