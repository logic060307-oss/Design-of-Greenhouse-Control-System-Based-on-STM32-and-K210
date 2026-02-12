#ifndef __SOIL_HUMIDITY_H
#define __SOIL_HUMIDITY_H
//土壤 ≥ 65 % 或连续运行 10 min 强制停
#include "stm32f10x.h"

// 土壤湿度传感器引脚定义：PA1 (ADC1 Channel 1)
#define SOIL_ADC_GPIO_PORT     GPIOA
#define SOIL_ADC_GPIO_PIN      GPIO_Pin_1
#define SOIL_ADC_GPIO_CLK      RCC_APB2Periph_GPIOA

#define SOIL_ADC               ADC1
#define SOIL_ADC_CHANNEL       ADC_Channel_1
#define SOIL_ADC_CLK           RCC_APB2Periph_ADC1

// 返回值定义
#define SOIL_SUCCESS           0
#define SOIL_ADC_ERROR         1

/**
 * @brief  初始化土壤湿度传感器相关的 ADC 和 GPIO (PA1)
 * @param  无
 * @retval 无
 */
void Soil_Humidity_Init(void);

/**
 * @brief  读取土壤湿度原始 ADC 值（12位，0~4095）
 * @param  无
 * @retval 返回 16 位 ADC 值（实际为 12 位右对齐）
 */
uint16_t Soil_Get_Adc_Value(void);

/**
 * @brief  获取土壤湿度百分比（0% ~ 100%）
 *         注意：需要根据你的传感器实际干湿值进行校准！
 *         默认：完全干燥（空气中）ADC ≈ 4095，浸入水中 ADC ≈ 1000（需自行测量调整）
 * @param  无
 * @retval 返回湿度百分比（0 ~ 100）
 */
uint8_t Soil_Get_Humidity_Percent(void);

#endif /* __SOIL_HUMIDITY_H */
