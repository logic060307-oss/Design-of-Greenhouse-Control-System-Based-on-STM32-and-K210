#include "Buzzer.h"

BUZZER_INFO buzzer_info = {0};

void Buzzer_Init(void)
{
    RCC_APB2PeriphClockCmd(BUZZER_CLK, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = BUZZER_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BUZZER_GPIO_PORT, &GPIO_InitStructure);
    
    Buzzer_Set(BUZZER_OFF); // 初始关闭（输出高电平）
}

void Buzzer_Set(_Bool status)
{
    // 低电平触发：ON 对应 Bit_RESET
    GPIO_WriteBit(BUZZER_GPIO_PORT, BUZZER_GPIO_PIN, status == BUZZER_ON ? Bit_RESET : Bit_SET);
    buzzer_info.Buzzer_Status = status; // 更新状态结构体
}
