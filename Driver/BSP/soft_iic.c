/* ═══════════════════════════════════════ */
              /* INCLUDE */
#include "soft_iic.h"
#include "Cus_Utils_delay.h"
/* ═══════════════════════════════════════ */


/* ═════════════════════════════════════════════════════════ */
                        /* DEFINES */
  #define SCL_LEVEL(x)       do {                                                                     \
                                  uint32_t pinMask = (x) ? SOFT_I2C_SCL_PIN : SOFT_I2C_SCL_PIN << 16;  \
                                  SOFT_I2C_SCL_PORT->BSRR = pinMask; }while(0)      

  #define SDA_LEVEL(x)       do {                                                                     \
                                  uint32_t pinMask = (x) ? SOFT_I2C_SDA_PIN : SOFT_I2C_SDA_PIN << 16;  \
                                  SOFT_I2C_SDA_PORT->BSRR = pinMask; }while(0)                            

  #define SDA_Read_Bit()    ((SOFT_I2C_SDA_PORT->IDR & SOFT_I2C_SDA_PIN) != 0)

  #define I2C_delay_us(x)    Cus_delay_us(x)
/* ═════════════════════════════════════════════════════════ */


/* ═══════════════════════════════════════ */
              /* Static_API */
static void i2c_start( void );          // 产生起始信号.         
static void i2c_restart( void );        // 产生重新起始信号(用于指定地址读).     
static void i2c_end( void );            // 产生结束信号.
static void i2c_ack( void );            // 产生ACK应答.
static void i2c_nack( void );           // 产生NACK应答.
static uint32_t i2c_wait_ack( void );   // 等待ACK应答.
/* ═══════════════════════════════════════ */


/* ═══════════════════════════════════════ */
              /* Public_API */
void Cus_SoftI2C_Init( void );
void Cus_I2C_SendByte( uint8_t txByte );    // 发送一个字节.
uint8_t Cus_I2C_ReadByte( uint8_t isAck );  // 读取一个字节.
uint8_t Cus_I2C_WriteRegister_8( uint8_t dev7BitAddr, uint8_t regAddr, const uint8_t *data, uint8_t len );   // 写寄存器(8位寄存器地址).
uint8_t Cus_I2C_WriteRegister_16( uint8_t dev7BitAddr, uint16_t regAddr, const uint8_t *data, uint8_t len ); // 写寄存器(重载版本. 16位寄存器地址).
uint8_t Cus_I2C_WriteAndRead( uint8_t dev7BitAddr, uint8_t regAddr, uint8_t *rcvBuffer, uint8_t len );       // 指定地址读(支持连续读取).
/* ═══════════════════════════════════════ */


/* ————————————————————————————— Init ————————————————————————————— */
void Cus_SoftI2C_Init( void )
{
  __HAL_RCC_GPIOE_CLK_ENABLE(); 

  GPIO_InitTypeDef I2C_gpioInitStrucutre;
  I2C_gpioInitStrucutre.Alternate     = 0;
  I2C_gpioInitStrucutre.Mode          = GPIO_MODE_OUTPUT_OD;    // 开漏模式.
  I2C_gpioInitStrucutre.Pin           = SOFT_I2C_SCL_PIN;
  I2C_gpioInitStrucutre.Pull          = GPIO_PULLUP;
  I2C_gpioInitStrucutre.Speed         = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(SOFT_I2C_SCL_PORT, &I2C_gpioInitStrucutre);

  I2C_gpioInitStrucutre.Pin           = SOFT_I2C_SDA_PIN;
  HAL_GPIO_Init(SOFT_I2C_SDA_PORT, &I2C_gpioInitStrucutre);

  /* 初始状态. 将SDA与SCL均拉到高. */
  SCL_LEVEL(1);
  SDA_LEVEL(1);
}


/* ————————————————————————————— static ————————————————————————————— */
static void i2c_start( void )
{
  /* 确保当前SCL SDA为高. */
  SCL_LEVEL(1);
  SDA_LEVEL(1);
  I2C_delay_us(I2C_DELAY_TIME);

  /* 起始信号: SCL高同时. SDA拉低. */
  SDA_LEVEL(0); 
  I2C_delay_us(I2C_DELAY_TIME);

  /* 主机主动拉低 SCL. 准备接下来传输. */
  SCL_LEVEL(0);
  I2C_delay_us(I2C_DELAY_TIME);
}


static void i2c_restart( void )
{
  /* 先拉高SDA. */
  SDA_LEVEL(1);
  I2C_delay_us(I2C_DELAY_TIME);

  /* 再拉高SCL.  */
  SCL_LEVEL(1);
  I2C_delay_us(I2C_DELAY_TIME);

  /* SCL高时拉低SDA. 产生起始信号. */
  SDA_LEVEL(0);
  I2C_delay_us(I2C_DELAY_TIME);

  /* 拉低SCL. 开始后继通信. */
  SCL_LEVEL(0);
  I2C_delay_us(I2C_DELAY_TIME);
}


static void i2c_end( void )
{
  /* 确保SCL SDA为低. */
  SCL_LEVEL(0);
  SDA_LEVEL(0);
  I2C_delay_us(I2C_DELAY_TIME);

  /* 先拉高SCL. */
  SCL_LEVEL(1);
  I2C_delay_us(I2C_DELAY_TIME);

  /* 再拉高SDA. 产生停止条件. */
  SDA_LEVEL(1);
  I2C_delay_us(I2C_DELAY_TIME);
}


static void i2c_ack( void )
{
  /* 确保SCL SDA为低. */
  SCL_LEVEL(0);
  SDA_LEVEL(0);
  I2C_delay_us(I2C_DELAY_TIME);

  /* SCL拉高. 供从机采样SDA. */
  SCL_LEVEL(1);
  I2C_delay_us(I2C_DELAY_TIME);

  SCL_LEVEL(0);
  I2C_delay_us(I2C_DELAY_TIME);
}


static void i2c_nack( void )
{
  /* 拉高SDA. 回复NACK. */
  SCL_LEVEL(0);
  SDA_LEVEL(1);
  I2C_delay_us(I2C_DELAY_TIME);

  /* 拉高SCL. 供从机采样. */
  SCL_LEVEL(1);
  I2C_delay_us(I2C_DELAY_TIME);

  /* 第九个时钟周期结束. */
  SCL_LEVEL(0);
  I2C_delay_us(I2C_DELAY_TIME);
}


static uint32_t i2c_wait_ack( void )
{
  uint32_t timeout = I2C_TIMEOUT_US;

  /* 确保SCL低. */
  SCL_LEVEL(0);
  I2C_delay_us(I2C_DELAY_TIME);

  /* 释放SDA总线.供从机操作. */
  SDA_LEVEL(1);
  I2C_delay_us(I2C_DELAY_TIME);

  /* 拉高SCL. 从机在此段高电平期间拉低SDA. */
  SCL_LEVEL(1);
  I2C_delay_us(I2C_DELAY_TIME);

  /* 读取SDA总线状况. */
  while( SDA_Read_Bit() != 0 )
  {
    if ( --timeout == 0 )
    {
      /* 超时. 结束通信.*/
      i2c_end();
      return 1;
    }

    I2C_delay_us(1);
  }

  /* 收到ACK. 复原状态. */
  SCL_LEVEL(0);
  I2C_delay_us(I2C_DELAY_TIME);

  return 0;
}


/* ————————————————————————————— public ————————————————————————————— */
void Cus_I2C_SendByte( uint8_t txByte )
{
  /* 拉低SCL. 准备开始通讯. */
  SCL_LEVEL(0);
  I2C_delay_us(I2C_DELAY_TIME);

  for( uint8_t index = 0; index < 8; index++ )
  {
    uint8_t bit = ((txByte >> 7) - index) & 0x01;

    if ( bit )
    {
      /* 输出高电平. */
      SDA_LEVEL(1);
    }
    else 
    {
      /* 低电平. */
      SDA_LEVEL(0);
    }
    I2C_delay_us(I2C_DELAY_TIME);

    /* 拉高SCL供从机采样. */
    SCL_LEVEL(1);
    I2C_delay_us(I2C_DELAY_TIME);

    /* 开始下次通信. */
    SCL_LEVEL(0);
    I2C_delay_us(I2C_DELAY_TIME);
  }
}


uint8_t Cus_I2C_ReadByte( uint8_t isAck )
{
  uint8_t recvByte = 0;

  /* 拉低SCL. 准备开始通信. */
  SCL_LEVEL(0);
  I2C_delay_us(I2C_DELAY_TIME);

  for( uint8_t index = 0; index < 8; index++ )
  {
    /* 先移位. */
    recvByte <<= 1;

    /* SCL高采样. */
    SCL_LEVEL(1);
    recvByte |= SDA_Read_Bit();
    I2C_delay_us(I2C_DELAY_TIME);

    /* SCL拉低. 准备下一轮接收. */
    SCL_LEVEL(0);
    I2C_delay_us(I2C_DELAY_TIME);
  }

  /* 第九个周期. ACK周期. */
  if ( isAck )   
    i2c_ack();
  else
    i2c_nack(); 

  return recvByte;
}


uint8_t Cus_I2C_WriteRegister_8( uint8_t dev7BitAddr, uint8_t regAddr, const uint8_t *data, uint8_t len )
{
  /* 7位从机地址(写) */
  uint8_t slave_addr = (dev7BitAddr << 1) & ~0x01;

  /* 产生起始信号. 开始一次通讯. */
  i2c_start();

  /* 先送从机地址. */
  Cus_I2C_SendByte(slave_addr);
  if ( i2c_wait_ack() )   goto ERROR;

  /* 送操作寄存器地址. */
  Cus_I2C_SendByte(regAddr);
  if ( i2c_wait_ack() )   goto ERROR;

  for( uint8_t index = 0; index < len; index++ )
  {
    /* 写数据. */
    Cus_I2C_SendByte(data[index]);
    if ( i2c_wait_ack() )   goto ERROR;
  }

  /* 通信结束. */
  i2c_end();
  return 0;

ERROR:
  i2c_end();
  return 1;
}


uint8_t Cus_I2C_WriteRegister_16( uint8_t dev7BitAddr, uint16_t regAddr, const uint8_t *data, uint8_t len )
{
  /* 7位从机地址(写) */
  uint8_t slave_addr = (dev7BitAddr << 1) & ~0x01;

  /* 产生起始信号. 开始一次通讯. */
  i2c_start();

  /* 先送从机地址. */
  Cus_I2C_SendByte(slave_addr);
  if ( i2c_wait_ack() )   goto ERROR;

  /* 送操作寄存器地址.(先送高字节) */
  Cus_I2C_SendByte((regAddr >> 8));
  if ( i2c_wait_ack() )   goto ERROR;

  Cus_I2C_SendByte((regAddr & 0xFF));
  if ( i2c_wait_ack() )   goto ERROR;

  for( uint8_t index = 0; index < len; index++ )
  {
    /* 写数据. */
    Cus_I2C_SendByte(data[index]);
    if ( i2c_wait_ack() )   goto ERROR;
  }

  /* 通信结束. */
  i2c_end();
  return 0;

ERROR:
  i2c_end();
  return 1;
}


uint8_t Cus_I2C_WriteAndRead( uint8_t dev7BitAddr, uint8_t regAddr, uint8_t *rcvBuffer, uint8_t len )
{
  if ( !rcvBuffer || len == 0 )   return 1;

  /* 7位从机地址(写/读) */
  uint8_t slave_addr_w = (dev7BitAddr << 1) & ~0x01;
  uint8_t slave_addr_r = (dev7BitAddr << 1) | 0x01;

  /* 产生起始信号. 开始一次通讯. */
  i2c_start();

  /* 先送从机地址. */
  Cus_I2C_SendByte(slave_addr_w);
  if ( i2c_wait_ack() )   goto ERROR;

  /* 写入要读取位置. 将指针移到该处. */
  Cus_I2C_SendByte(regAddr);
  if ( i2c_wait_ack() )   goto ERROR;

  /* 重新发送起始信号. */
  i2c_restart();
  
  /* 送从机地址. */
  Cus_I2C_SendByte(slave_addr_r);
  if ( i2c_wait_ack() )   goto ERROR;

  /* 开始连续接收. */
  for( uint8_t index = 0; index < len; index++ )
  {
    /* 最后一字节发送NACK. */
    rcvBuffer[index] = Cus_I2C_ReadByte( index < (len - 1) ? 1 : 0 );
  }

  i2c_end();
  return 0;

ERROR:
  i2c_end();
  return 1;
}

