#ifndef __KEY_H
#define __KEY_H

#define KEY_COUNT				4

#define KEY_1					0
#define KEY_2					1
#define KEY_3					2
#define KEY_4					3

#define KEY_HOLD				0x01                // 用法：检测按键是否被按住// 按键被按住时持续执行
#define KEY_DOWN				0x02                // 用法：检测按键刚刚被按下// 只在按键按下的瞬间触发一次
#define KEY_UP					0x04                // 只在按键从按下到释放的瞬间触发一次
#define KEY_SINGLE				0x08                // 快速按下并释放，且没有双击
#define KEY_DOUBLE				0x10                // 在短时间内快速按下两次
#define KEY_LONG				0x20                // 按住按键达到长按时间阈值
#define KEY_REPEAT				0x40                // 长按后每隔 KEY_TIME_REPEAT (100ms) 触发一次


void Key_Init(void);
uint8_t Key_Check(uint8_t n, uint8_t Flag);
void Key_Tick(void);

#endif
