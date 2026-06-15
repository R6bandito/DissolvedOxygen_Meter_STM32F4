#ifndef __SOFT_IIC_H__
#define __SOFT_IIC_H__


/* ═══════════════════════════════════════ */
              /* INCLUDE */
#include "stm32f4xx_hal.h"
/* ═══════════════════════════════════════ */


/* ═════════════════════════════════════════════════════════ */
                        /* DEFINES */
  /* 软件I2C引脚映射. */
  #define SOFT_I2C_SCL_PORT                 (GPIOE)
  #define SOFT_I2C_SCL_PIN                  (GPIO_PIN_5)
  #define SOFT_I2C_SDA_PORT                 (GPIOE)
  #define SOFT_I2C_SDA_PIN                  (GPIO_PIN_6)

  /* I2C时序速度 */
  #define I2C_DELAY_TIME                    (5)

  /* I2C超时. */
  #define I2C_TIMEOUT_US                    (5000)
/* ═════════════════════════════════════════════════════════ */                        


/* ═════════════════════════════════════════════════════════ */
                      /* API_LIST */
void Cus_SoftI2C_Init( void );
void Cus_I2C_SendByte( uint8_t txByte );    // 发送一个字节.
uint8_t Cus_I2C_ReadByte( uint8_t isAck );  // 读取一个字节.
uint8_t Cus_I2C_WriteRegister_8( uint8_t dev7BitAddr, uint8_t regAddr, const uint8_t *data, uint8_t len );   // 写寄存器(8位寄存器地址).
uint8_t Cus_I2C_WriteRegister_16( uint8_t dev7BitAddr, uint16_t regAddr, const uint8_t *data, uint8_t len ); // 写寄存器(重载版本. 16位寄存器地址).
uint8_t Cus_I2C_WriteAndRead( uint8_t dev7BitAddr, uint8_t regAddr, uint8_t *rcvBuffer, uint8_t len );       // 指定地址读(支持连续读取).
/* ═════════════════════════════════════════════════════════ */


#endif /* __SOFT_IIC_H__ */
