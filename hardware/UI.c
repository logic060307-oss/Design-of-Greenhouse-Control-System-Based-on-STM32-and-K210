#include "stm32f10x.h"                  // Device header
#include "UI.h"


//定义UI内部使用的设备记录状态（1开0关）
u8 beep_state = 0;
u8 led_state  = 0;
u8 fan_state  = 0;
u8 pump_state = 0;

u8 AnimalType = 0;                          // 0:None, 1:dog, 2:duck, 3:cock
volatile u32 Animal_Last_Time = 0;           // 记录最后一次检测到动物的时间戳
void Show_UI(void)
{
    LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
    LCD_ShowChinese(16,2,"智能温室大棚",BLUE,WHITE,16,0);     //标题
    //参数标签
    LCD_ShowChinese(0,20,"温度",RED,WHITE,16,0); 
    LCD_ShowChinese(64,20,"湿度",GREEN,WHITE,16,0);
    LCD_ShowChinese(0,40,"土壤",YELLOW,WHITE,16,0);
    LCD_ShowChinese(0,60,"光照",BROWN,WHITE,16,0);
    LCD_ShowString(0,80,"CO2 ",YELLOW,WHITE,16,0);
    //执行器
    LCD_ShowChinese(0,120,"风扇",LIGHTBLUE,WHITE,16,0); 
    LCD_ShowChinese(64,120,"水泵",YELLOW,WHITE,16,0);
    LCD_ShowChinese(0,100,"蜂鸣",BLUE,WHITE,16,0); 
    LCD_ShowChinese(64,100,"灯光",GBLUE,WHITE,16,0);
    
    LCD_ShowChinese(0,140,"模式",BRED,WHITE,16,0);
    LCD_ShowChinese(40,140,"自动",RED,WHITE,16,0);
    LCD_ShowChinese_Inverse(80,140,"手动",BLUE,WHITE,16);
    
    //单位
    LCD_ShowChinese(50, 23, "℃", RED, WHITE, 12, 0);
    LCD_ShowString(115,16,"%",BRED,WHITE,16,0);
    LCD_ShowString(68,40,"%",BRED,WHITE,16,0);
    LCD_ShowString(85,60,"lx",RED,WHITE,16,0);
    LCD_ShowString(70, 80,"ppm", GREEN, WHITE,16,0);
}

//刷新传感器数值显示
void Read_Sensor_Data(void)
{
    //传感器数据采集
    DHT11_Read_Data(&temp,&humi);
    Light_lux = BH1750_Read_Light();
    soil_humi = Soil_Get_Humidity_Percent();
    if(CCS811_Read_Data(&ccs_data)==CCS811_OK)
    {
        if(ccs_data.eCO2 > 8192) {
        ccs_data.eCO2 = 0; // 或者保持上一次的值
        }
    }
    else{
        UsartPrintf(USART_DEBUG, "CCS811 Read Error!\r\n");
        ccs_data.eCO2 = 400;
    }
    LCD_ShowIntNum(32, 20, temp, 2, BLUE, WHITE, 16);  
    LCD_ShowIntNum(98, 20, humi, 2, BLUE, WHITE, 16); 
    LCD_ShowIntNum(36, 40, soil_humi, 2, LBBLUE, WHITE, 16);
    LCD_ShowIntNum(36, 60, Light_lux, 5, BLUE, WHITE, 16);
    LCD_ShowIntNum(30, 80, ccs_data.eCO2, 4, GBLUE, WHITE, 16);
}

static u8 flag = 5;      // 光标位置
static u8 last_flag = 0; // 用于刷新光标
//界面按键交互逻辑
void UI_Key_Logic(void)
{
    // KEY1: 上一项/左移
    if(Key_Check(KEY_1, KEY_SINGLE))
    {
        flag--;
        if(flag == 0) flag = 6;
    }
    // KEY2: 下一项/右移
    if(Key_Check(KEY_2, KEY_SINGLE))
    {
        flag++;
        if(flag > 6)  flag = 1;
    }
    // 自动模式限制：光标强制停在模式选择区 (5-6)
    if(System_Mode == 1 && flag < 5)
    {
        flag = 5;
    }
    
    if(flag != last_flag)
    {
        // 恢复上次选中项颜色
        switch(last_flag) {
            case 1: LCD_ShowChinese(0, 100, "蜂鸣", BLUE, WHITE, 16, 0); break;
            case 2: LCD_ShowChinese(64, 100, "灯光", GBLUE, WHITE, 16, 0); break;
            case 3: LCD_ShowChinese(0, 120, "风扇", LIGHTBLUE, WHITE, 16, 0); break;
            case 4: LCD_ShowChinese(64, 120, "水泵", YELLOW, WHITE, 16, 0); break;
            case 5: LCD_ShowChinese(40, 140, "自动", RED, WHITE, 16, 0); break;
            case 6: LCD_ShowChinese(80, 140, "手动", BLUE, WHITE, 16, 0); break;
        }
        // 高亮当前选中项
        switch(flag) {
            case 1: LCD_ShowChinese_Inverse(0, 100, "蜂鸣", BLUE, WHITE, 16); break;
            case 2: LCD_ShowChinese_Inverse(64, 100, "灯光", GBLUE, WHITE, 16); break;
            case 3: LCD_ShowChinese_Inverse(0, 120, "风扇", LIGHTBLUE, WHITE, 16); break;
            case 4: LCD_ShowChinese_Inverse(64, 120, "水泵", YELLOW, WHITE, 16); break;
            case 5: LCD_ShowChinese_Inverse(40, 140, "自动", RED, WHITE, 16); break;
            case 6: LCD_ShowChinese_Inverse(80, 140, "手动", BLUE, WHITE, 16); break;
        }
        last_flag = flag;
    }
    
    if(Key_Check(KEY_3, KEY_SINGLE)) // 确认键
    {
        if(flag == 5 || flag == 6) {   // 模式切换
            System_Mode = (flag == 5) ? 1 : 0;
            if(System_Mode == 0) {     // 切手动时全关设备，防止误动作
                Buzzer_Set(BUZZER_OFF); Led_Set(LED_OFF); Fan_Set(FAN_OFF); Pump_Set(PUMP_OFF);
                beep_state = 0; led_state = 0; fan_state = 0; pump_state = 0;
                UI_Sync_Status(); 
            }
        } 
        else if(System_Mode == 0) {    // 手动模式下控制设备
            switch(flag) {
                case 1: beep_state = !beep_state; Buzzer_Set(beep_state); break;
                case 2: led_state  = !led_state;  Led_Set(led_state);   break;
                case 3: fan_state  = !fan_state;  Fan_Set(fan_state);   break;
                case 4: pump_state = !pump_state; Pump_Set(pump_state); break;
            }
            UI_Sync_Status(); 
        }
    }
}

// 3. 智能控制逻辑 (智能控制模块))
void Auto_Control_Logic(void)
{
    if(System_Mode == 1)            // 自动模式
    {
        // 风扇：温度>=28或CO2>2000开启，温度<26且CO2<=1800关闭
        if(temp >= 28 || ccs_data.eCO2 > 2000) fan_state = 1;
        else if(temp < 26 && ccs_data.eCO2 <= 1800) fan_state = 0;
        // 灯光：光照<800开启，>1500关闭
        if(Light_lux < 800) led_state = 1;
        else if(Light_lux > 1500) led_state = 0;
        // 水泵：土壤湿度<35%开启，>60%关闭
        if(soil_humi < 35) pump_state = 1;
        else if(soil_humi > 60) pump_state = 0;
        //蜂鸣器：检测到（鸡、鸭、狗）开启
        beep_state = (AnimalType > 0);
        // 执行硬件操作并同步UI
        Fan_Set(fan_state); Led_Set(led_state); Pump_Set(pump_state);Buzzer_Set(beep_state);
        UI_Sync_Status();
    }
}


// 供 OneNet 接收函数调用;强制刷新UI上的开关状态文字 (开/关)
void UI_Sync_Status(void)
{
    LCD_ShowChinese(40, 100, beep_state ? "开" : "关", BLUE, WHITE, 16, 0);
    LCD_ShowChinese(106, 100, led_state ? "开" : "关", BLUE, WHITE, 16, 0);
    LCD_ShowChinese(40, 120, fan_state ? "开" : "关", BLUE, WHITE, 16, 0);
    LCD_ShowChinese(106, 120, pump_state ? "开" : "关", BLUE, WHITE, 16, 0);
}

// 解析 K210 数据的函数
void Parse_K210_Data(void)
{
    if(USART_RX_STA & 0x8000)               //串口接收完成
    {
        if(strstr((char*)USART_RX_BUF, "dog"))       AnimalType = 1;
        else if(strstr((char*)USART_RX_BUF, "duck")) AnimalType = 2;
        else if(strstr((char*)USART_RX_BUF, "cock")) AnimalType = 3;
        
        Animal_Last_Time = ms_ticks; // 更新时间戳
        USART_RX_STA = 0;            //清除标志位
        memset(USART_RX_BUF, 0, USART_REC_LEN);// 清空缓存
    }
    //超时检测（每次调用都执行，不依赖是否有新数据）
    if(AnimalType != 0 && (ms_ticks - Animal_Last_Time > 2000)) 
    {
        AnimalType = 0;                      // 超过2秒没检测到，清除动物类型
    }
}

