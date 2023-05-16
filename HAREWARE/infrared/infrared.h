#ifndef _INFRARED_H_
#define _INFRARED_H_

#include "stm32f10x.h"
#include "delay.h"
#include "led.h"
#include "oled.h"
#include "human.h"
#include "timer.h"

#define DISABLE_SEND 0
#define ABLE_SEND 1

#define INFRARED_SEND_MAX_SECONE 120
#define INFRARED_SEND_MIN_SECONE 20

#define DEVICE_ADDRESS 0xa3

#define INFRARED_RCC_GPIOX RCC_APB2Periph_GPIOA
#define INFRARED_GPIOX GPIOA
#define INFRARED_GPIO_Pin_X GPIO_Pin_0

void infrared_init(void);
void infrared_output_high(void);
void infrared_output_low(void);

void set_infrared_send_flag(uint8_t flag);
uint8_t get_infrared_send_flag(void);

void infrared_set_send_frequence(uint16_t new_frequence);
uint16_t infrared_get_send_frequece(void);
unsigned char infrared_get_device_address(void);

void infrared_send_data(void);
#endif
