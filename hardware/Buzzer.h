#ifndef __BUZZER_H
#define __BUZZER_H
#include "stm32f10x.h"

// 结构体风格对齐 LED
typedef struct {
    _Bool Buzzer_Status;
} BUZZER_INFO;

#define BUZZER_ON    1
#define BUZZER_OFF   0

// 硬件引脚保持不变
#define BUZZER_CLK          RCC_APB2Periph_GPIOB
#define BUZZER_GPIO_PIN     GPIO_Pin_12
#define BUZZER_GPIO_PORT    GPIOB

extern BUZZER_INFO buzzer_info;

void Buzzer_Init(void);
void Buzzer_Set(_Bool status); // 统一接口

#endif
