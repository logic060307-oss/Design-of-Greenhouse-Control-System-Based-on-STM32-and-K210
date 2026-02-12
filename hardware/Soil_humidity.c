#include "soil_humidity.h"

// 请根据你的传感器实际测试值修改下面两个宏定义！
// Dry_Value: 传感器完全干燥时（在空气中）的 ADC 值，通常接近 4095
// Wet_Value: 传感器完全浸入水中时的 ADC 值，通常在 800~2000 左右
#define SOIL_DRY_VALUE    3800    // 示例值：空气中 ≈3800（请你实际测量后修改）
#define SOIL_WET_VALUE    1200    // 示例值：水中 ≈1200（请你实际测量后修改）

void Soil_Humidity_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    
    // 1. 使能 GPIOA 和 ADC1 时钟
    RCC_APB2PeriphClockCmd(SOIL_ADC_GPIO_CLK | SOIL_ADC_CLK, ENABLE);
    
    // 2. 配置 PA1 为模拟输入模式（ADC 需要模拟模式）
    GPIO_InitStructure.GPIO_Pin = SOIL_ADC_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;     // 模拟输入
    GPIO_Init(SOIL_ADC_GPIO_PORT, &GPIO_InitStructure);
    
    // 3. ADC1 配置
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;              // 独立模式
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;                   // 单通道
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;             // 单次转换
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 软件触发
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;          // 右对齐
    ADC_InitStructure.ADC_NbrOfChannel = 1;                         // 1 个通道
    ADC_Init(SOIL_ADC, &ADC_InitStructure);
    
    // 4. 设置 ADC 时钟：PCLK2 8分频 → 9MHz（最大14MHz）
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);
    
    // 5. 使能 ADC
    ADC_Cmd(SOIL_ADC, ENABLE);
    
    // 6. ADC 校准（推荐）
    ADC_ResetCalibration(SOIL_ADC);
    while (ADC_GetResetCalibrationStatus(SOIL_ADC));
    ADC_StartCalibration(SOIL_ADC);
    while (ADC_GetCalibrationStatus(SOIL_ADC));
}

uint16_t Soil_Get_Adc_Value(void)
{
    // 配置通道1采样时间（建议55.5周期以上）
    ADC_RegularChannelConfig(SOIL_ADC, SOIL_ADC_CHANNEL, 1, ADC_SampleTime_55Cycles5);
    
    // 启动转换
    ADC_SoftwareStartConvCmd(SOIL_ADC, ENABLE);
    
    // 等待转换完成
    while (ADC_GetFlagStatus(SOIL_ADC, ADC_FLAG_EOC) == RESET);
    
    // 返回转换结果
    return ADC_GetConversionValue(SOIL_ADC);
}

uint8_t Soil_Get_Humidity_Percent(void)
{
    uint16_t adc_value;
    int32_t humidity;
    
    adc_value = Soil_Get_Adc_Value();
    
    // 防止超出范围
    if (adc_value >= SOIL_DRY_VALUE) {
        return 0;     // 完全干燥 → 湿度 0%
    }
    if (adc_value <= SOIL_WET_VALUE) {
        return 100;   // 完全湿润 → 湿度 100%
    }
    
    // 线性映射：湿度 = (干值 - 当前值) / (干值 - 湿值) * 100
    humidity = (int32_t)(SOIL_DRY_VALUE - adc_value) * 100 
               / (SOIL_DRY_VALUE - SOIL_WET_VALUE);
    
    // 限制在 0~100
    if (humidity < 0) humidity = 0;
    if (humidity > 100) humidity = 100;
    
    return (uint8_t)humidity;
}
