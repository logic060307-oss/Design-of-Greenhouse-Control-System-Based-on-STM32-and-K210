#ifndef __BH1750_H
#define __BH1750_H
//光照强度低于800lx开启LED
#include "stm32f10x.h"

/* ================= BH1750 I2C 地址 =================
 * ADDR 接地或悬空：0x23
 * ADDR 接高电平：0x5C
 * 默认使用 ADDR = 0
 */
#define BH1750_ADDR             0x23

/* ================= BH1750 指令 ================= */
#define BH1750_POWER_DOWN       0x00    // 断电模式
#define BH1750_POWER_ON         0x01    // 上电
#define BH1750_RESET            0x07    // 数据寄存器复位（需先上电）

/* 连续测量模式（推荐） */
#define BH1750_CONT_H_RES       0x10    // 连续高分辨率模式（1 lx，典型 120ms）
#define BH1750_CONT_H_RES2      0x11    // 连续高分辨率模式2（0.5 lx）
#define BH1750_CONT_L_RES       0x13    // 连续低分辨率模式（4 lx，16ms）

/* ================= 返回值定义 ================= */
#define BH1750_OK               0
#define BH1750_ERROR            1

/* ================= 接口函数 ================= */

/**
 * @brief  初始化 BH1750
 * @note   上电后切换到【连续高分辨率模式】
 *         第一次测量需要等待约 120~150 ms
 *         初始化完成后，芯片会自动持续测量
 */
void BH1750_Init(void);

/**
 * @brief  读取当前光照强度
 * @note   连续模式下直接读取，无需启动测量、无需延时
 * @retval 光照强度值（单位：lx），失败返回 0.0
 */
float BH1750_Read_Light(void);

#endif /* __BH1750_H */
