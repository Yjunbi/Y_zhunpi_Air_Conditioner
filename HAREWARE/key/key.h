#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"
#include "delay.h"
#include "led.h"
#include "oled.h"
#include "ath20_model.h"
#include "infrared.h"
#include "myinterrupt.h"
#include "myadc.h"
#include "human.h"

#define K1 GPIO_Pin_10
#define K2 GPIO_Pin_11
#define K3 GPIO_Pin_12
#define K4 GPIO_Pin_13 
#define K5 GPIO_Pin_14
#define K6 GPIO_Pin_15


#define SET_MAX_TEMPRATURE_VALUE 35
#define SET_MIN_TEMPRATURE_VALUE 16
#define SET_MAX_HUMIDITY_VALUE 99
#define SET_MIN_HUMIDITY_VALUE 9

#define FIRST_MENU_NUMBER 4
#define HOME_MENU_ITEM_NUMBER 2
#define SETTING_MENU_ITEM_NUMBER 3

typedef enum {
    HOME_MENU = 1,
    SETTING_MENU,
    SEND_MENU,
    POWER_MENU,
} oled_first_menu_tiem_t;

typedef struct menu_info_t{
    unsigned char menu_flag;
    float item_data;
    struct menu_info_t *next_menu;
    struct menu_info_t *sub_menu;
} menu_info_t;

extern menu_info_t start_menu_info;

void state_panel_info_change(void);
void first_menu_info_change_flag(void);

void key_gpio_init(void);

void first_menu_info_change_flag(void);

void key_first_menu_scan(void);

void key_home_menu_scan(void);
void key_home_menu_change_data(void);

void key_setting_menu_scan(void);
void key_setting_menu_hanle_data(void);
void key_test_function(void);
#endif

