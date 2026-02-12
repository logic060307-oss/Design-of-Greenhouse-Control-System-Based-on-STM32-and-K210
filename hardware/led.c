#include "stm32f10x.h"
#include "led.h"


LED_INFO led_info = {0};


/*
************************************************************
*	函数名称：	Led_Init
*
*	函数功能：	LED初始化
*
*	入口参数：	无
*
*	返回参数：	无
*
*	说明：		
************************************************************
*/
void Led_Init(void)
{

	GPIO_InitTypeDef gpio_initstruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//打开GPIOB的时钟
	
	gpio_initstruct.GPIO_Mode = GPIO_Mode_Out_PP;				//设置为输出
	gpio_initstruct.GPIO_Pin = GPIO_Pin_1;						//将初始化的Pin脚
	gpio_initstruct.GPIO_Speed = GPIO_Speed_50MHz;				//可承载的最大频率
	
	GPIO_Init(GPIOB, &gpio_initstruct);							//初始化GPIO
	
	Led_Set(LED_OFF);											//初始化完成后，关闭蜂鸣器

}

/*
************************************************************
*	函数名称：	Led_Set
*
*	函数功能：	LED控制
*
*	入口参数：	status：开关蜂鸣器
*
*	返回参数：	无
*
*	说明：		开-LED_ON		关-LED_OFF
************************************************************
*/
void Led_Set(_Bool status)
{
	
	GPIO_WriteBit(GPIOB, GPIO_Pin_1, status == LED_ON ? Bit_SET : Bit_RESET);		//如果status等于LED_ON，则返回Bit_SET，否则返回Bit_RESET
	
	led_info.Led_Status = status;

}
