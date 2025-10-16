#include "stm32f10x.h" // Device header
#include "Motor.h"
#include "Delay.h"

void Car_Init(void)
{
	Motor_Init(); // 初始化电机驱动模块
}

void Go_Ahead(void)
{
	Motor_SetLeftSpeed(70);
	Motor_SetRightSpeed(70);
}

void Go_Back(void)
{
	Motor_SetLeftSpeed(-70);
	Motor_SetRightSpeed(-70);
}

void Turn_Left(void) // 原地左拐：右轮前转，左轮不动
{
	Motor_SetLeftSpeed(0);
	Motor_SetRightSpeed(70);
}

void Turn_Right(void) // 原地右拐：左轮前转，右轮不动
{
	Motor_SetRightSpeed(0);
	Motor_SetLeftSpeed(70);
}

void Self_Left(void) // 自旋逆时针：左轮后、右轮前
{
	Motor_SetLeftSpeed(-70);
	Motor_SetRightSpeed(70);
}

void Self_Right(void) // 自旋顺时针：左轮前、右轮后
{
	Motor_SetRightSpeed(-70);
	Motor_SetLeftSpeed(70);
}

void Car_Stop(void)
{
	Motor_SetLeftSpeed(0);
	Motor_SetRightSpeed(0);
}
