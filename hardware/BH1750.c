#include "bh1750.h"
#include "MyI2C.h"
#include "Delay.h"      

static uint8_t BH1750_Write_Byte(uint8_t cmd);
static uint8_t BH1750_Read_TwoBytes(uint16_t *data);
/*
新增：初始化时把芯片切到连续模式
第一次转换需要 150 ms，后面随时可读（不用再每次读取的时候都等待180ms）
读取函数里只做 I2C 取数，零延时
*/  
void BH1750_Init(void)
{
    MyI2C_Init();           // 初始化你已有的软件 I2C (PB6 SCL, PB7 SDA)
    
    DelayMs(10);
    
    // 上电
    BH1750_Write_Byte(BH1750_POWER_ON);
    DelayMs(10);
    
    // 连续高分辨率模式（1lx 分辨率）
    BH1750_Write_Byte(BH1750_CONT_H_RES);
    DelayMs(150);
}

static uint8_t BH1750_Write_Byte(uint8_t cmd)
{
    MyI2C_Start();
    MyI2C_SendByte(BH1750_ADDR << 1 | 0);   // 写地址
    if (MyI2C_ReceiveAck()) {
        MyI2C_Stop();
        return BH1750_ERROR;
    }
    
    MyI2C_SendByte(cmd);
    if (MyI2C_ReceiveAck()) {
        MyI2C_Stop();
        return BH1750_ERROR;
    }
    
    MyI2C_Stop();
    return BH1750_OK;
}

static uint8_t BH1750_Read_TwoBytes(uint16_t *data)
{
    uint8_t high, low;
    
    MyI2C_Start();
    MyI2C_SendByte(BH1750_ADDR << 1 | 1);    // 读地址
    if (MyI2C_ReceiveAck()) {
        MyI2C_Stop();
        return BH1750_ERROR;
    }
    
    high = MyI2C_ReceiveByte();
    MyI2C_SendAck(0);                       // ACK
    
    low = MyI2C_ReceiveByte();
    MyI2C_SendAck(1);                       // NACK（最后一个字节）
    
    MyI2C_Stop();
    
    *data = (uint16_t)(high << 8) | low;
    return BH1750_OK;
}

float BH1750_Read_Light(void)
{
    uint16_t raw_data = 0;
    float lux = 0.0f;
    
    if (BH1750_Read_TwoBytes(&raw_data) == BH1750_OK)
    {
        // BH1750 原始数据转换为 lx（高分辨率模式下除以 1.2）
        lux = (float)raw_data / 1.2f;
    }
    else
    {
        lux = 0.0f;  // 读取失败返回 0
    }
    
    return lux;
}
