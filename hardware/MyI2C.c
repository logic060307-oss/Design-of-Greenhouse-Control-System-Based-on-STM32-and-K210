#include "stm32f10x.h"                  // Device header
#include "MyI2C.h"
#include "Delay.h"

void MyI2C_W_SCL(uint8_t BitValue)
{
	GPIO_WriteBit(SCL_PORT, SCL_PIN, (BitAction)BitValue);
	DelayUs(10);
}

void MyI2C_W_SDA(uint8_t BitValue)
{
	GPIO_WriteBit(SDA_PORT, SDA_PIN, (BitAction)BitValue);
	DelayUs(10);
}

uint8_t MyI2C_R_SDA(void)
{
	uint8_t BitValue;
	BitValue = GPIO_ReadInputDataBit(SDA_PORT, SDA_PIN);
	DelayUs(10);
	return BitValue;
}
 
void MyI2C_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Pin = SCL_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SCL_PORT, &GPIO_InitStructure);
    //配置SDA初始状态为开漏输出
    GPIO_InitStructure.GPIO_Pin = SDA_PIN;
    GPIO_Init(SDA_PORT, &GPIO_InitStructure);
    
    GPIO_SetBits(SCL_PORT, SCL_PIN);
    GPIO_SetBits(SDA_PORT, SDA_PIN);
}

/**
  * 函数名：MyI2C_SDA_Mode_Set
  * 功能：设置SDA引脚模式
  * 参数：Mode - 0:输入模式, 1:输出模式
  * 返回值：无
  */
void MyI2C_SDA_Mode_Set(uint8_t Mode)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    if(Mode == 0)
    {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    }
    else
    {
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    }
    
    GPIO_InitStructure.GPIO_Pin = SDA_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SDA_PORT,&GPIO_InitStructure);
}

void MyI2C_Start(void)
{
    MyI2C_SDA_Mode_Set(1);  // SDA设置为输出模式
	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(0);
}

void MyI2C_Stop(void)
{
    MyI2C_SDA_Mode_Set(1);  // SDA设置为输出模式
	MyI2C_W_SDA(0);
	MyI2C_W_SCL(1);
	MyI2C_W_SDA(1);
    MyI2C_SDA_Mode_Set(0);  // 停止后SDA设置为输入模式（释放总线）
}

void MyI2C_SendByte(uint8_t Byte)
{
	uint8_t i;
    MyI2C_SDA_Mode_Set(1);  // SDA设置为输出模式
	for (i = 0; i < 8; i ++)
	{
		MyI2C_W_SDA(!!(Byte & (0x80 >> i)));
		MyI2C_W_SCL(1);
		MyI2C_W_SCL(0);
	}
}

uint8_t MyI2C_ReceiveByte(void)
{
	uint8_t i, Byte = 0x00;
    MyI2C_SDA_Mode_Set(0);  // SDA设置为输入模式
	MyI2C_W_SDA(1);
	for (i = 0; i < 8; i ++)
	{
		MyI2C_W_SCL(1);
		if (MyI2C_R_SDA()){Byte |= (0x80 >> i);}
		MyI2C_W_SCL(0);
	}
	return Byte;
}

void MyI2C_SendAck(uint8_t AckBit)
{
    MyI2C_SDA_Mode_Set(1);  // SDA设置为输出模式
	MyI2C_W_SDA(AckBit);
	MyI2C_W_SCL(1);
	MyI2C_W_SCL(0);
}

uint8_t MyI2C_ReceiveAck(void)
{
	uint8_t AckBit;
    MyI2C_SDA_Mode_Set(0);  // SDA设置为输入模式
	MyI2C_W_SDA(1);
	MyI2C_W_SCL(1); 
	AckBit = MyI2C_R_SDA();
	MyI2C_W_SCL(0);
	return AckBit;
}
