//单片机头文件
#include "stm32f10x.h"

//网络协议层
#include "onenet.h"

//网络设备
#include "esp8266.h"

//硬件驱动
#include "delay.h"
#include "UI.h"
//C库
#include <string.h>

#define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"mqtts.heclouds.com\",1883\r\n"

u8 System_Mode = 1;                 //开机进入自动模式
u8 temp,humi;                       //0-50℃ 20-90%
u16 Light_lux;                      //0-65535lx  
CCS811_Data_TypeDef ccs_data;       //400-8192ppm
u8 soil_humi;                       //0-100%

void Hardware_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断控制器分组设置
    
	Delay_Init();									//systick初始化
	
	Usart1_Init(115200);							//串口1，打印信息用
	Usart2_Init(115200);							//串口2，驱动ESP8266用
	
    Key_Init();                                     //按键初始化
    Timer_Init();                                   //定时器初始化
    //传感器初始化
	while(DHT11_Init())
	{
		UsartPrintf(USART_DEBUG, "DHT11 Error \r\n");
		DelayMs(1000);
	}
    BH1750_Init();
	CCS811_Init();
    Soil_Humidity_Init();
    LCD_Init();
    LCD_BLK_Set();
    //执行器初始化
	Led_Init();									    //LED初始化
    Buzzer_Init();                                  //Buzzer初始化
    Pump_Init();                                    //水泵初始化
    Fan_Init();                                     //风扇初始化
	UsartPrintf(USART_DEBUG, " Hardware init OK\r\n");
}

int main(void)
{
	//定义任务的时间戳变量
    uint32_t Task_Sensor_LastTime = 0;   // 传感器采集时间戳
    uint32_t Task_NetTx_LastTime = 0;    // 网络发送时间戳
    uint32_t Task_NetRx_LastTime = 0;    // 网络接收时间戳
    
	unsigned char *dataPtr = NULL;
	
	Hardware_Init();				//初始化外围硬件
	
	ESP8266_Init();					//初始化ESP8266
	UsartPrintf(USART_DEBUG, "Connect MQTTs Server...\r\n");
	while(ESP8266_SendCmd(ESP8266_ONENET_INFO, "CONNECT"))
		DelayXms(500);
    UsartPrintf(USART_DEBUG, "Connect MQTTs Server Success\r\n");
	
	while(OneNet_DevLink())			//接入OneNET
		DelayXms(500);
    
    OneNET_Subscribe();
    Show_UI();
    
	while(1)
	{
        //1. 动物入侵报警模块，处理 K210 数据
        Parse_K210_Data();
        //2. 传感器采集与智能控制 (1秒/次)
		if(ms_ticks - Task_Sensor_LastTime >= 1000)									//发送间隔1s
		{
            //串口打印
            UsartPrintf(USART_DEBUG, "temp %d ,humi %d Light_lux %d ,ccs_data %d soil_humi %d AnimalType %d LED:%d, Beep:%d, Fan:%d, Pump:%d\r\n",
            temp,humi,Light_lux,ccs_data.eCO2,soil_humi,AnimalType,led_info.Led_Status, buzzer_info.Buzzer_Status, fan_info.Fan_Status, pump_info.Pump_Status);
            // 刷新UI数字
            Read_Sensor_Data();             
            Auto_Control_Logic();           // 自动模式运算
			Task_Sensor_LastTime = ms_ticks;
		}
        //3. 用户交互
        UI_Key_Logic();
        // 4.1 数据上报 (1s一次)
        if(ms_ticks - Task_NetTx_LastTime >= 1000)
        {
            UsartPrintf(USART_DEBUG, "OneNet_SendData\r\n");
            OneNet_SendData();
			ESP8266_Clear();
            Task_NetTx_LastTime = ms_ticks;
        }
        // 4.2 数据下发 (10ms一次)
        if(ms_ticks - Task_NetRx_LastTime >= 10)
        {
            Task_NetRx_LastTime = ms_ticks;
            // 4.2 数据下发 (实时轮询)
            dataPtr = ESP8266_GetIPD(0);// 检查是否有平台数据包
            if(dataPtr != NULL)
                OneNet_RevPro(dataPtr);// 调用刚才修改的解析函数
        }
	}

}


