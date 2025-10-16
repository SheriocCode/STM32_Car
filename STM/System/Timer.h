#ifndef __TIMER_H
#define __TIMER_H

void TIM3_Init(void);
void TIM3_SetCallback(void (*cb)(void));
#endif
