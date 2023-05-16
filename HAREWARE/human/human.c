#include "human.h"

unsigned char human_init_flag = 0;


/**
  * @brief  Initialize human gpio.
  * @retval None
  */
void human_gpio_init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = HUMAN_GPIO_Pinx;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(HUMAN_GPIO_GPIOX, &GPIO_InitStruct);
	
    human_init_flag = 1;

    return;
}

/**
  * @brief  Get the human detect device state.
  * @retval human detect state
  */
unsigned char get_human_detect(void) {
    if(!human_init_flag) return 0;
    return GPIO_ReadInputDataBit(HUMAN_GPIO_GPIOX, HUMAN_GPIO_Pinx);
}
