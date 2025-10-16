#include "stm32f10x.h" // Device header
#include "Delay.h"

static uint8_t gdistance = 0; // 0-254 cm (255会和校验位冲突，所以用254最大值)

uint16_t Cnt;
uint16_t OverCnt;
void Ultrasound_Init()
{

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // trig
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; // echo
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	TIM_InternalClockConfig(TIM4);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period = 60000 - 1; // ARR
	TIM_TimeBaseInitStructure.TIM_Prescaler = 72 - 1; // PSC
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStructure);
}
float Test_Distance()
{
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
	Delay_us(20);
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == RESET)
	{
	};
	TIM_Cmd(TIM4, ENABLE);
	while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == SET)
	{
	};
	TIM_Cmd(TIM4, DISABLE);
	Cnt = TIM_GetCounter(TIM4);
	float distance = (Cnt * 1.0 / 10 * 0.34) / 2;
	TIM4->CNT = 0;
	Delay_ms(100);
	return distance;
}

void Ultrasound_Update(void)
{
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
	Delay_us(20);
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);

	/* 等待高电平开始 */
	while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == RESET)
		;
	TIM_SetCounter(TIM4, 0);
	TIM_Cmd(TIM4, ENABLE);

	/* 等待高电平结束 */
	while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) == SET)
		;
	TIM_Cmd(TIM4, DISABLE);

	uint16_t cnt = TIM_GetCounter(TIM4);
	/* 原有测距代码 ... */
	float dist = cnt * 0.017f; // 厘米
	if (dist > 253)
		dist = 253;
	if (dist < 0)
		dist = 0;
	gdistance = (uint8_t)(dist + 0.5f); // 四舍五入
}

uint8_t Ultrasound_GetDistance(void)
{
	return gdistance;
}