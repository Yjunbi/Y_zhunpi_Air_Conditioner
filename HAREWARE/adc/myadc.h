#ifndef _MYADC_H_
#define _MYADC_H_

#include "stm32f10x.h"
#include "delay.h"

#define MYADC_GPIOX GPIOA
#define MYADC_GPIO_PinX GPIO_Pin_1

void myadc_init(void);
uint16_t myadc_get_value(void);
uint16_t myadc_get_average(uint8_t times);

#endif

