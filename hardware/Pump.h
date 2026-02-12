#ifndef __PUMP_H
#define __PUMP_H
#include "stm32f10x.h"

typedef struct {
    _Bool Pump_Status;
} PUMP_INFO;

#define PUMP_ON    1
#define PUMP_OFF   0

// 硬件引脚保持不变
#define PUMP_CLK         RCC_APB2Periph_GPIOB
#define PUMP_GPIO_PIN    GPIO_Pin_14
#define PUMP_GPIO_PROT   GPIOB

extern PUMP_INFO pump_info;

void Pump_Init(void);
void Pump_Set(_Bool status);

#endif
