#include "stm32f10x.h" // Device header
#include "PWM_Motor.h"

/* 静态变量保存 Set 值 */
static int8_t gLeftSpeed = 0;
static int8_t gRightSpeed = 0;

void Motor_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	PWM_Init();
}

// 设置左侧轮速度
void Motor_SetLeftSpeed(int8_t Speed)
{
	gLeftSpeed = Speed; // 缓存
	if (Speed > 0)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_4);
		GPIO_ResetBits(GPIOA, GPIO_Pin_5);
		PWM_SetCompare3(Speed);
	}
	else if (Speed == 0)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_4);
		GPIO_SetBits(GPIOA, GPIO_Pin_5);
		PWM_SetCompare3(Speed);
	}
	else
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_4);
		GPIO_SetBits(GPIOA, GPIO_Pin_5);
		PWM_SetCompare3(-Speed);
	}
}

// 设置右侧轮速度
void Motor_SetRightSpeed(int8_t Speed)
{
	gRightSpeed = Speed; // 缓存
	if (Speed > 0)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_6);
		GPIO_ResetBits(GPIOA, GPIO_Pin_7);
		PWM_SetCompare3(Speed);
	}
	else if (Speed == 0)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_6);
		GPIO_SetBits(GPIOA, GPIO_Pin_7);
		PWM_SetCompare3(Speed);
	}
	else
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_6);
		GPIO_SetBits(GPIOA, GPIO_Pin_7);
		PWM_SetCompare3(-Speed);
	}
}

// 获取左侧轮速度
int8_t Motor_GetLeftSpeed(void) { return gLeftSpeed; }

// 获取右侧轮速度
int8_t Motor_GetRightSpeed(void) { return gRightSpeed; }
