#ifndef __CCS811_H
#define __CCS811_H
//二氧化碳浓度1000ppm开启
#include "stm32f10x.h"

// CCS811 I2C 从机地址（7位地址 0x5A，写时左移1位）
#define CCS811_ADDR             0x5A

// WAKE 引脚定义（必须拉低才能通信）
#define CCS811_WAKE_GPIO        GPIOB
#define CCS811_WAKE_PIN         GPIO_Pin_5
#define CCS811_WAKE_CLK         RCC_APB2Periph_GPIOB

// CCS811 主要寄存器
#define CCS811_STATUS           0x00
#define CCS811_MEAS_MODE        0x01
#define CCS811_ALG_RESULT_DATA  0x02
#define CCS811_APP_START        0xF4
#define CCS811_HW_ID            0x20

// 返回值定义
#define CCS811_OK               0
#define CCS811_ERROR            1
#define CCS811_NO_NEW_DATA      2

// 数据结构体
typedef struct {
    uint16_t eCO2;   // 等效 CO2 浓度 (ppm)，正常范围 400~8192
    uint16_t TVOC;   // 总挥发性有机化合物 (ppb)，正常范围 0~1187
} CCS811_Data_TypeDef;

/**
 * @brief  初始化 CCS811（初始化 WAKE 引脚 + MyI2C）
 */
void CCS811_Init(void);

/**
 * @brief  拉低 WAKE 引脚（唤醒 CCS811）
 */
void CCS811_WakeUp(void);

/**
 * @brief  拉高 WAKE 引脚（让 CCS811 低功耗）
 */
void CCS811_Sleep(void);

/**
 * @brief  读取 CCS811 的 eCO2 和 TVOC 数据
 * @param  data 指向存放结果的结构体指针
 * @retval CCS811_OK            有新数据并读取成功
 *         CCS811_NO_NEW_DATA   暂无新数据
 *         CCS811_ERROR         通信失败
 */
uint8_t CCS811_Read_Data(CCS811_Data_TypeDef *data);

#endif
