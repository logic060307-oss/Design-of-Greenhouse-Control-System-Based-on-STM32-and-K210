#include "fan.h"

FAN_INFO fan_info = {0};

void Fan_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(FAN_CLK, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = FAN_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; // 开漏输出，保留你原来的配置
    GPIO_Init(FAN_GPIO_PROT, &GPIO_InitStructure);
    
    Fan_Set(FAN_OFF);
}

void Fan_Set(_Bool status)
{
    // 低电平触发：ON 对应 Bit_RESET
    GPIO_WriteBit(FAN_GPIO_PROT, FAN_GPIO_PIN, status == FAN_ON ? Bit_RESET : Bit_SET);
    fan_info.Fan_Status = status;
}
