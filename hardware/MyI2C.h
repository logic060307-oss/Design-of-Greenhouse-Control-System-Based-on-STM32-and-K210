#ifndef __MYI2C_H
#define __MYI2C_H
#include "stm32f10x.h"                  // Device header

//只需要修改这里
#define SCL_PORT GPIOB
#define SCL_PIN  GPIO_Pin_6

#define SDA_PORT GPIOB
#define SDA_PIN  GPIO_Pin_7

void MyI2C_Init(void);
void MyI2C_Start(void);
void MyI2C_Stop(void);
void MyI2C_SendByte(uint8_t Byte);
uint8_t MyI2C_ReceiveByte(void);
void MyI2C_SendAck(uint8_t AckBit);
uint8_t MyI2C_ReceiveAck(void);
void MyI2C_SDA_Mode_Set(uint8_t Mode);
/*
发送数据时：SDA设为输出模式

接收数据时：SDA设为输入模式

这样可以正确读取从设备的数据
*/
#endif
