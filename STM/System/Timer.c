#include "stm32f10x.h" // Device header

void TIM3_Init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_TimeBaseInitTypeDef t;
    t.TIM_Prescaler = 7200 - 1; // 72 MHz / 720 = 100 kHz
    // t.TIM_Period = 5000 - 1;  // 每 0.5 s 触发一次中断

    t.TIM_Period = 10000 - 1;
    t.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM3, &t);

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    NVIC_EnableIRQ(TIM3_IRQn);

    TIM_Cmd(TIM3, ENABLE);
}

// /* 中断函数：每 10 ms 硬发一次速度 */
// void TIM3_IRQHandler(void)
// {
//     if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
//     {
//         TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

//         // 电机速度
//         int8_t left = Motor_GetLeftSpeed();
//         int8_t right = Motor_GetRightSpeed();

//         // 超声波数据
//         uint8_t dis = Ultrasound_GetDistance();

//         Serial_TxPacket[0] = (uint8_t)left;
//         Serial_TxPacket[1] = (uint8_t)right;
//         Serial_TxPacket[2] = (uint8_t)dis;
//         Serial_TxPacket[3] = 0;

//         Serial_SendPacket(); // 立即发出，与主循环无关
//     }
// }

static void (*Tim3Callback)(void) = 0;

void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
        if (Tim3Callback)
            Tim3Callback(); // 只跳出去
    }
}

void TIM3_SetCallback(void (*cb)(void))
{
    Tim3Callback = cb;
}