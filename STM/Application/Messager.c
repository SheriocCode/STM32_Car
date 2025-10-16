#include "stm32f10x.h"  // 标准库头文件
#include "Serial.h"     // HC340串口模块
#include "Ultrasound.h" // 超声波模块
#include "Motor.h"      // 电机模块

// 发送数据到PC端
void App_TxTimerCallback(void)
{
    int8_t left = Motor_GetLeftSpeed();
    int8_t right = Motor_GetRightSpeed();
    uint8_t dis = Ultrasound_GetDistance();

    Serial_TxPacket[0] = (uint8_t)left;
    Serial_TxPacket[1] = (uint8_t)right;
    Serial_TxPacket[2] = dis;
    Serial_TxPacket[3] = 0;
    Serial_SendPacket();
}