#ifndef __OLED_H_
#define __OLED_H_

#include "stm32f10x.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "delay.h"
#include "timer.h"
#include "key.h"

#define OLED_ADDRESS 0x78
#define SLAVE_ADDRESS OLED_ADDRESS

#define OLED_MAX_WIDTH 128
#define OLED_MAX_HIGH 8

#define IS_TEMPERATURE 0
#define IS_HUMIDITY 1
#define SHOW_TEMPERATURE IS_TEMPERATURE
#define SHOW_HUMIDITY IS_HUMIDITY

#define NO_CHARGE 0
#define IS_CHARGE 1
#define NO_EXIST 0
#define IS_EXIST 1

#define DIRECTION_NONE 0x00
#define DIRECTION_UP 0x01
#define DIRECTION_DOWN 0x80
#define DIRECTION_LEFT 0x10
#define DIRECTION_RIGHT 0x08

#define NOT_IMMEDIATELY 0
#define IS_IMMEDIATELY 1

/* The coordinate about setting temprature and humidity. */
#define SETTED_T_A_H_X 48
#define SETTED_T_A_H_Y 3

/* The coordinate about detected temprature and humidity. */
#define SDETECTED_T_A_H_X 48
#define SDETECTED_T_A_H_Y 6


/* The coordinate about connect iocn. */
#define CONNECT_ICON_X 0
#define CONNECT_ICON_Y 0

/* The coordinate about human iocn. */
#define HUMAN_ICON_X 16
#define HUMAN_ICON_Y 0

/* The coordinate about charge iocn. */
#define CHARGE_ICON_X 104
#define CHARGE_ICON_Y 0

/* The coordinate about battery iocn. */
#define BETTERY_ICON_X 112
#define BETTERY_ICON_Y 0

typedef struct {
    // 0xxx xxx0 
    // bit0 is change flag   0->no change
    // bit7 is connect_state 0->no connect
    unsigned char connect_state;
    unsigned char human_state;
    unsigned char charge_state;
    unsigned char battery_state;
} state_panel_t;

typedef struct {
    float set_data;
    char set_data_string[6];
    unsigned char set_data_string_len;
} data_form_t; 

typedef struct {
    unsigned char set_data;
    char set_data_string[6];
    unsigned char set_data_string_len;
} setting_data_form_t; 

void oled_data_array_translate(void);

// oledd basic function
void oled_init(void);
unsigned char get_oled_state(void);
void oled_turn_off(void);
void oled_turn_on(void);
void oled_show_array_data(void);
void oled_write_cmd(unsigned char oled_cmd);
void oled_write_data(unsigned char oled_data);
void oled_show_char(unsigned char x, unsigned char y, unsigned char chr);
void oled_show_string(unsigned char x, unsigned char y, unsigned char *str);

// stat panel api
void set_device_connect_state(unsigned char connect_state);
unsigned char get_device_connect_state(void);
void set_device_human_state(unsigned char human_state);
unsigned char get_device_human_state(void);
void set_device_charge_state(unsigned char charge_state);
unsigned char get_device_charge_state(void);
void set_device_battery_state(unsigned char battery_state);
unsigned char get_device_battery_state(void);

// home menu api
void set_T_or_H_set_data(float set_data, unsigned char is_T_or_H);
float get_T_or_H_set_data(unsigned char is_T_or_H);
void set_T_or_H_detect_data(float detect_data, unsigned char is_T_or_H);
float get_T_or_H_detect_data(unsigned char is_T_or_H);

// setting menu api
void set_device_sleep_time(unsigned char time);
unsigned char get_device_sleep_time(void);
void set_device_setting_frequency(unsigned char frequency);
unsigned char get_device_setting_frequency(void);
void set_device_setting_brightness(unsigned char brightness);
unsigned char get_device_setting_brightness(void);

void fill_picture(unsigned char fill_data);
void oled_clear(void);
void oled_show_temperature_humidity(unsigned char x, unsigned char y, char *number_string, unsigned char is_T_or_H);
void oled_show_set_T_or_H(unsigned char is_T_or_H);
void oled_show_battery_state(void);
void oled_show_connect_state(void);
void oled_show_T_or_H_title(unsigned char is_T_or_H);
void oled_refresh_T_or_H(unsigned char is_T_or_H);
int oled_change_T_or_H_tip(int number, unsigned char is_T_or_H, int direction);
void oled_show_state_panel(unsigned char immediately);


void oled_show_home_page(unsigned char is_T_or_H, unsigned char is_charge, unsigned char immediately);

void oled_animation(unsigned char start_x, unsigned char start_y, unsigned char width, unsigned char high, unsigned char direction, unsigned char speed);
void oled_normal_move(unsigned char direction);

void oled_show_first_menu_item(unsigned char n);
void oled_show_first_menu_icon(unsigned char n);
void oled_show_first_menu_icon_move(unsigned char n, unsigned char direction);
void oled_first_menu_select(unsigned char n, unsigned char immediately);

void oled_show_setting_items(int selected_item, unsigned char immediately);
void oled_show_setting_item_detial(unsigned char item, unsigned char is_change, unsigned char immediately);

#endif
