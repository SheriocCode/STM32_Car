#include "stm32f10x.h" // Device header

#define IR_R1 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)
#define IR_R2 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_12)
#define IR_L2 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11)
#define IR_L1 GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1)

uint8_t IR_Scan(void)
{
	uint8_t ir = (IR_L1 << 3) | (IR_L2 << 2) | (IR_R2 << 1) | IR_R1;
	return ir;
}

void IR_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef gpio;
	// gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Mode = GPIO_Mode_IPU;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;

	/* 最右  -> PA3  右二 -> PA12 左二 -> PA11 */
	gpio.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_12 | GPIO_Pin_11;
	GPIO_Init(GPIOA, &gpio);

	/* 最左  -> PB1 */
	gpio.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOB, &gpio);
}
