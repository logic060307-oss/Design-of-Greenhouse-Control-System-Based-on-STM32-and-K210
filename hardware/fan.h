#ifndef __FAN_H
#define __FAN_H
#include "stm32f10x.h"

typedef struct {
    _Bool Fan_Status;
} FAN_INFO;

#define FAN_ON    1
#define FAN_OFF   0

// 硬件引脚保持不变
#define FAN_CLK          RCC_APB2Periph_GPIOB
#define FAN_GPIO_PIN     GPIO_Pin_13
#define FAN_GPIO_PROT    GPIOB

extern FAN_INFO fan_info;

void Fan_Init(void);
void Fan_Set(_Bool status);

#endif
