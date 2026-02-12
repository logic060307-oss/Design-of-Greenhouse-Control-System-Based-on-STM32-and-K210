#include "Pump.h"

PUMP_INFO pump_info = {0};

void Pump_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(PUMP_CLK, ENABLE);
    
    GPIO_InitStructure.GPIO_Pin = PUMP_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(PUMP_GPIO_PROT, &GPIO_InitStructure);
    
    Pump_Set(PUMP_OFF);
}

void Pump_Set(_Bool status)
{
    // 高电平触发：ON 对应 Bit_SET
    GPIO_WriteBit(PUMP_GPIO_PROT, PUMP_GPIO_PIN, status == PUMP_ON ? Bit_SET : Bit_RESET);
    pump_info.Pump_Status = status;
}
