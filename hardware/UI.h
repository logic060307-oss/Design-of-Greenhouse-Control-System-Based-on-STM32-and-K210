#ifndef __UI_H
#define __UI_H

#include "stdio.h"
#include "dht11.h"
#include "BH1750.h"
#include "MyI2C.h"
#include "soil_humidity.h"
#include "CCS811.h"
#include "lcd.h"
#include "string.h"
#include "led.h"
#include "Pump.h"
#include "fan.h"
#include "Buzzer.h"
#include "key.h"
#include "Timer.h"
#include "MyI2C.h"
#include "Key.h"
#include "usart.h"

//模式控制变量
extern u8 System_Mode;                      // 0:手动, 1:自动
//温度变量
extern u8 temp,humi;
extern u16 Light_lux;
extern CCS811_Data_TypeDef ccs_data;
extern u8 soil_humi;

extern volatile u32 Animal_Last_Time; // 记录最后一次检测到动物的时间戳 (定义在 main.c)
extern volatile u32 ms_ticks;
extern u8 AnimalType;
// --- 设备状态共享变量 (供 UI 和 OneNet 同步使用) ---
extern u8 beep_state;
extern u8 led_state;
extern u8 fan_state;
extern u8 pump_state;

//函数接口
void Show_UI(void);                         // UI展示
void UI_Key_Logic(void);                    // 按键处理逻辑
void Read_Sensor_Data(void);                // 刷新传感器数值
void Auto_Control_Logic(void);              // 智能控制逻辑
void UI_Sync_Status(void);                  // 强制刷新UI上的开关状态显示
void Parse_K210_Data(void);                 // K210数据读取

#endif

