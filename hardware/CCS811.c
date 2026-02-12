#include "ccs811.h"
#include "MyI2C.h"
#include "Delay.h"      
#include <stddef.h>         //包含NULL
static uint8_t CCS811_Write(uint8_t reg, uint8_t *data, uint8_t len);
static uint8_t CCS811_Read(uint8_t reg, uint8_t *data, uint8_t len);
static uint8_t CCS811_Check_DataReady(void);

void CCS811_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 1. 初始化 MyI2C（PB6 SCL, PB7 SDA）
    MyI2C_Init();
    
    // 2. 初始化 WAKE 引脚 PB5 为推挽输出，默认高电平（睡眠）
    RCC_APB2PeriphClockCmd(CCS811_WAKE_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = CCS811_WAKE_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(CCS811_WAKE_GPIO, &GPIO_InitStructure);
    
    CCS811_Sleep();                 // 默认睡眠
    
    DelayMs(50);                   // 上电稳定延时
    
    // 3. 唤醒并启动应用模式（必须）
    CCS811_WakeUp();
    DelayUs(100);                  // WAKE 低至少 50us
    
    CCS811_Write(CCS811_APP_START, NULL, 0);  // 启动 APP
    
    DelayMs(100);                  // 等待固件启动
    
    // 4. 设置测量模式：每秒测量一次（Mode 1）
    uint8_t mode = 0x10;            // bit[6:4] = 001 → 1秒一次
    CCS811_Write(CCS811_MEAS_MODE, &mode, 1);
    
    CCS811_Sleep();                 // 初始化完成，重新睡眠节省功耗
}

void CCS811_WakeUp(void)
{
    GPIO_ResetBits(CCS811_WAKE_GPIO, CCS811_WAKE_PIN);  // 拉低 WAKE
    DelayUs(100);                                      // 至少 50us，保险 100us
}

void CCS811_Sleep(void)
{
    GPIO_SetBits(CCS811_WAKE_GPIO, CCS811_WAKE_PIN);    // 拉高 WAKE
}

static uint8_t CCS811_Write(uint8_t reg, uint8_t *data, uint8_t len)
{
    MyI2C_Start();
    MyI2C_SendByte(CCS811_ADDR << 1 | 0);           // 写地址
    if (MyI2C_ReceiveAck()) return CCS811_ERROR;
    
    MyI2C_SendByte(reg);
    if (MyI2C_ReceiveAck()) { MyI2C_Stop(); return CCS811_ERROR; }
    
    for (uint8_t i = 0; i < len; i++)
    {
        MyI2C_SendByte(data[i]);
        if (MyI2C_ReceiveAck()) { MyI2C_Stop(); return CCS811_ERROR; }
    }
    
    MyI2C_Stop();
    return CCS811_OK;
}

static uint8_t CCS811_Read(uint8_t reg, uint8_t *data, uint8_t len)
{
    // 先写寄存器地址
    MyI2C_Start();
    MyI2C_SendByte(CCS811_ADDR << 1 | 0);
    if (MyI2C_ReceiveAck()) { MyI2C_Stop(); return CCS811_ERROR; }
    
    MyI2C_SendByte(reg);
    if (MyI2C_ReceiveAck()) { MyI2C_Stop(); return CCS811_ERROR; }
    
    // 重复启动，读数据
    MyI2C_Start();
    MyI2C_SendByte(CCS811_ADDR << 1 | 1);           // 读地址
    if (MyI2C_ReceiveAck()) { MyI2C_Stop(); return CCS811_ERROR; }
    
    for (uint8_t i = 0; i < len; i++)
    {
        data[i] = MyI2C_ReceiveByte();
        if (i == len - 1)
            MyI2C_SendAck(1);   // 最后一个字节发送 NACK
        else
            MyI2C_SendAck(0);   // ACK
    }
    
    MyI2C_Stop();
    return CCS811_OK;
}

static uint8_t CCS811_Check_DataReady(void)
{
    uint8_t status;
    if (CCS811_Read(CCS811_STATUS, &status, 1) != CCS811_OK)
        return 0;
    return (status & 0x08);  // bit3 = DATA_READY
}

uint8_t CCS811_Read_Data(CCS811_Data_TypeDef *data)
{
    uint8_t buf[8];
    
    CCS811_WakeUp();
    
    if (!CCS811_Check_DataReady())
    {
        CCS811_Sleep();
        return CCS811_NO_NEW_DATA;
    }
    
    if (CCS811_Read(CCS811_ALG_RESULT_DATA, buf, 8) != CCS811_OK)
    {
        CCS811_Sleep();
        return CCS811_ERROR;
    }
    
    CCS811_Sleep();
    
    // 高字节在前
    data->eCO2 = (uint16_t)buf[0] << 8 | buf[1];
    data->TVOC = (uint16_t)buf[2] << 8 | buf[3];
    
    return CCS811_OK;
}
