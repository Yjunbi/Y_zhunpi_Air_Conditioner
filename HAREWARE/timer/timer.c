#include "timer.h"

/**
  * @brief  Initialize timer_2.
  * @retval None
  */
void timer2_init(void) {
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStr;
	NVIC_InitTypeDef NVIC_Initstr;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseInitStr.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStr.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStr.TIM_Period = 4999; //2hz  72000000/（4999+1）（7199+1）  
	TIM_TimeBaseInitStr.TIM_Prescaler = 7199;
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStr);
	
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE );
	
	NVIC_Initstr.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_Initstr.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Initstr.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Initstr.NVIC_IRQChannelSubPriority= 0;
	
	NVIC_Init(&NVIC_Initstr);
	TIM_Cmd(TIM2, ENABLE);
}

/**
  * @brief  Turn off timer_2.
  * @retval None
  */
void timer2_turn_on(void) {
    TIM_Cmd(TIM2, ENABLE);//开启定时器	
    return;
}

/**
  * @brief  Turn on timer_2.
  * @retval None
  */
void timer2_turn_off(void) {
    TIM_Cmd(TIM2, DISABLE);//开启定时器	
    return;
}


