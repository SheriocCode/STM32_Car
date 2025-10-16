#include "stm32f10x.h" // Device header
#include "Delay.h"
#include "Motor.h"
#include "Car.h"
#include "Serial_HC05.h" // 蓝牙串口
#include "Serial.h"      // HC340串口模块
#include "Servo.h"       // 舵机控制
#include "Ultrasound.h"  // 超声波模块
#include "Infrared.h"    // 循迹模块

/* application */
#include "TrackLine.h" // 循迹模块
#include "Messager.h"  // 消息处理模块

#include "Timer.h"

void Serial_SendMotorSpeed(void);

uint16_t Data1;
int main(void)
{
    Car_Init();
    Serial_HC05_Init(); // 蓝牙串口初始化
    Serial_Init();      // HC340串口初始化
    Servo_Init();       // 舵机初始化
    Ultrasound_Init();  // 超声波模块初始化
    IR_Init();          // 循迹模块初始化

    TIM3_Init(); // 配置定时器3
    TIM3_SetCallback(App_TxTimerCallback);

    Delay_ms(1000);
    while (1)
    {
        // 超声波测距
        Ultrasound_Update();
        // 循迹模块
        TrackLine_Update();

        Delay_ms(1000); // 每100ms发送一次，避免太频繁
    }
}

// 蓝牙串口中断函数
void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
        Data1 = USART_ReceiveData(USART1);
        if (Data1 == 0x30)
            Car_Stop();
        if (Data1 == 0x31)
            Go_Ahead();
        if (Data1 == 0x32)
            Go_Back();
        if (Data1 == 0x33)
            Turn_Left();
        if (Data1 == 0x34)
            Turn_Right();
        if (Data1 == 0x35)
            Self_Left();
        if (Data1 == 0x36)
            Self_Right();
        if (Data1 == 0x37)
            Servo_SetAngle(0);
        if (Data1 == 0x38)
            Servo_SetAngle(90);
        if (Data1 == 0x39)
            Servo_SetAngle(180);
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}
