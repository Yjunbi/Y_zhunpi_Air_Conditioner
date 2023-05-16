#include "led.h"

/**
  * @brief  Initialize led gpio.
  * @retval None
  */
void led_init(void) {
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	GPIO_InitStruct.GPIO_Pin = LED_GPIO;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	GPIO_ResetBits(GPIOC, LED_GPIO);
    return;
}	

/**
  * @brief  Get led state.
  * @retval None
  */
uint8_t led_state(void) {
    return GPIO_ReadOutputDataBit(GPIOC, LED_GPIO);
}

/**
  * @brief  Turn on led.
  * @retval None
  */
void led_turn_on(void) {
    GPIO_ResetBits(GPIOC,LED_GPIO);
    return;
}

/**
  * @brief  Turn off led.
  * @retval None
  */
void led_turn_off(void) {
    GPIO_SetBits(GPIOC,LED_GPIO);
    return;
}
