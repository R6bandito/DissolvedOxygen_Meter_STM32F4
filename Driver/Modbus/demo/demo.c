#include "mb.h"
#include "mbport.h"
#include "FreeRTOS.h"
#include "task.h"

extern void calib_sync( void );

// 十路保持寄存器
#define REG_HOLD_SIZE   6
uint16_t REG_HOLD_BUF[REG_HOLD_SIZE];


/// CMD6、3、16命令处理回调函数
eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    USHORT usRegIndex = usAddress - 1;

    // 非法检测
    if((usRegIndex + usNRegs) > REG_HOLD_SIZE)
    {
        return MB_ENOREG;
    }

    // 写寄存器
    if(eMode == MB_REG_WRITE)
    {
        taskENTER_CRITICAL();
        while( usNRegs > 0 )
        {
            REG_HOLD_BUF[usRegIndex] = (pucRegBuffer[0] << 8) | pucRegBuffer[1];
            pucRegBuffer += 2;
            usRegIndex++;
            usNRegs--;
        }
        taskEXIT_CRITICAL();

        /* 写入后参数同步. */
        calib_sync();
    }

    // 读寄存器
    else
    {
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ = ( unsigned char )( REG_HOLD_BUF[usRegIndex] >> 8 );
            *pucRegBuffer++ = ( unsigned char )( REG_HOLD_BUF[usRegIndex] & 0xFF );
            usRegIndex++;
            usNRegs--;
        }
    }

    return MB_ENOERR;
}


// 线圈回调（功能码 0x01, 0x05, 0x0F）
eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
    (void)pucRegBuffer;
    (void)usAddress;
    (void)usNCoils;
    (void)eMode;
    // 未实现，返回寄存器不存在
    return MB_ENOREG;
}

// 离散输入回调（功能码 0x02）
eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    (void)pucRegBuffer;
    (void)usAddress;
    (void)usNDiscrete;
    return MB_ENOREG;
}



// 输入寄存器回调（功能码 0x04）
eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    (void)pucRegBuffer;
    (void)usAddress;
    (void)usNRegs;
    return MB_ENOREG;
}



