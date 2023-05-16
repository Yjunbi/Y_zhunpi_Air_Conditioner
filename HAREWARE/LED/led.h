#ifndef __LED_H_
#define __LED_H_

#include "stm32f10x.h"

#define LED_GPIO GPIO_Pin_13

void led_init(void);
uint8_t led_state(void);
void led_turn_on(void);
void led_turn_off(void);

#endif
