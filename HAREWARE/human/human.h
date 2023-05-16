#ifndef __HUMAN_H_
#define __HUMAN_H_

#include "stm32f10x.h"

#define HUMAN_GPIO_GPIOX GPIOB
#define HUMAN_GPIO_Pinx GPIO_Pin_6


void human_gpio_init(void);
unsigned char get_human_detect(void);
#endif
