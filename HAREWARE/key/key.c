#include "key.h"

menu_info_t start_menu_info;

float set_max_min_T_or_H_value[2][2] = {{SET_MAX_TEMPRATURE_VALUE, SET_MIN_TEMPRATURE_VALUE}, {SET_MAX_HUMIDITY_VALUE, SET_MIN_HUMIDITY_VALUE}};

/**
  * @brief  Find the target menu.
  * @param  first: the first layer index number
  * @param  second: the second layer index number
  * @retval Return the target menu pointer.
  */
menu_info_t * find_menu(unsigned char first, unsigned char second) {
    unsigned char i, j;
    menu_info_t *temp;
    temp = &start_menu_info;
    
    if(!first) return temp;
    for(i=0; i<first; i++) {
        temp = temp->next_menu;
        if(temp == NULL) return NULL;
    }

    if(!second) return temp;
    for(j=0; j<second; j++) {
        if(!j)  temp = temp->sub_menu;
        else temp = temp->next_menu;
        if(temp == NULL) return NULL;
    }
    return temp;
}

/**
  * @brief  Create some menus which are in the same layer.
  * @param  n: how many menu need to be created
  * @retval Return the menu pointer that was created at first.
  */
menu_info_t * create_menu(unsigned char n) {
    menu_info_t *head_node, *temp_node0, *temp_node1;
    unsigned char i;
    if(!n) return NULL;
    temp_node0 = (menu_info_t *)malloc(sizeof(menu_info_t));
    temp_node0->menu_flag = 0x00;
    temp_node0->item_data = 0.0;
    temp_node0->next_menu = NULL;
    temp_node0->sub_menu = NULL;
    head_node = temp_node0;

    for(i=1; i<n; i++) {
        temp_node1 = (menu_info_t *)malloc(sizeof(menu_info_t));
        temp_node1->menu_flag = 0x00;
        temp_node1->item_data = 0.0;
        temp_node1->next_menu = NULL;
        temp_node1->sub_menu = NULL;
        temp_node0->next_menu = temp_node1;
        temp_node0 =  temp_node0->next_menu;
    }
    return head_node;
}

/**
  * @brief  Initialize the first menu info.
  * @retval None.
  */
void first_menu_info(void) {
    start_menu_info.menu_flag = 0X71;
    start_menu_info.next_menu = NULL;
    start_menu_info.next_menu = create_menu(FIRST_MENU_NUMBER);
    start_menu_info.sub_menu = NULL;
    return;
}

/**
  * @brief  Renew the first menu info change flag.
  * @retval None.
  */
void first_menu_info_change_flag(void) {
    menu_info_t *temp;
    temp = find_menu(0, 0);
    if(temp != NULL) {
        temp->menu_flag |= 0x80;
    }
}

/**
  * @brief  Renew the state panel info change flage.
  * @retval None.
  */
void state_panel_info_change(void) {
    uint16_t test;
    float voltage;
    test = myadc_get_value();
    voltage = (test * (3.3 / 4095) * 2 - 1.8) * 10 * 4.3;
    if(voltage >= 80) {
        set_device_battery_state(4);
    } else if(voltage >= 40) {
        set_device_battery_state(3);
    } else if(voltage >= 20) {
        set_device_battery_state(2);
    } else if(voltage >= 10) {
        set_device_battery_state(1);
    } else {
        set_device_battery_state(0); 
    }
    // led_state() ? led_turn_on() : led_turn_off();
    return;
}

/**
  * @brief  Renew the detect data info change flage.
  * @retval None.
  */
void renew_oled_detcet_data(void) {
    uint32_t CT_data[2];
    uint32_t c1, t1;
    float temp;
    led_state() ? led_turn_on() : led_turn_off();
    if((AHT20_Read_Status() & 0x18)!=0x18) {
        AHT20_Start_Init(); //重新初始化寄存器
        Delay_1ms(10);
    }
    AHT20_Read_CTdata_crc(CT_data);
    c1 = (int)(CT_data[0]*100*10/1024/1024.0);
    t1 = (int)(CT_data[1]*200*10/1024/1024.0-500);
    temp = t1/10.0;  if(temp > 1) set_T_or_H_detect_data(temp, IS_TEMPERATURE);
    temp = c1/10.0;  if(temp > 1) set_T_or_H_detect_data(temp, IS_HUMIDITY);
    return;
}

/**
  * @brief  Routine function run in the key sacn function.
  * @retval None.
  */
void _routine_function(void) {
    state_panel_info_change();
    renew_oled_detcet_data();
    infrared_send_data();
}

/**
  * @brief  Initialize the home menu info.
  * @retval None.
  */
void home_menu_info(void) {
    menu_info_t *temp, *temp1, *temp2;
    temp = find_menu(1, 0);
    temp->menu_flag = 0x01;
    temp->sub_menu = create_menu(HOME_MENU_ITEM_NUMBER);
    if(temp->sub_menu != NULL) {
        temp1 = find_menu(1, 1);
        temp1->menu_flag = 0x00;
        temp1->item_data = get_T_or_H_set_data(IS_TEMPERATURE);
        temp2 = find_menu(1, 2);
        temp2->menu_flag = 0x00;
        temp2->item_data = get_T_or_H_set_data(IS_HUMIDITY);
    }
    return;
}
 
/**
  * @brief  Initialize the setting menu info.
  * @retval None.
  */
void setting_menu_info(void) {
    menu_info_t *temp;
    temp = find_menu(SETTING_MENU, 0);
    temp->menu_flag = 0x01;
    temp->sub_menu = create_menu(SETTING_MENU_ITEM_NUMBER);
    if(temp->sub_menu != NULL) {
        temp = find_menu(SETTING_MENU, 1);
        temp->menu_flag = 0x00;
        temp->item_data = get_device_sleep_time() * 1.0;
        temp = find_menu(SETTING_MENU, 2);
        temp->menu_flag = 0x00;
        temp->item_data = get_device_setting_frequency() * 1.0;
        temp = find_menu(SETTING_MENU, 3);
        temp->menu_flag = 0x00;
        temp->item_data = get_device_setting_brightness() * 1.0;
    }
    return;
}

/**
  * @brief  Initialize key gpio.
  * @retval None
  */
void key_gpio_init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = K2 | K2 | K3 | K4 | K5 | K6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    first_menu_info();
    home_menu_info();
    setting_menu_info();
    // start_menu_info.menu_flag = 1; // why? The fine_menu function is wrong;
    return;
} 

/**
  * @brief  Get the key state.
  * @retval Retuen the key state.
  */
unsigned char key_read(uint16_t GPIO_Pin) {
    return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin);
}

/**
  * @brief  Scan the key state in the first menu.
  * @retval none.
  */
void key_first_menu_scan(void) {
    unsigned char temp;
    menu_info_t *temp1, *temp2;
    
    if(!get_oled_state()) {
        temp1 = find_menu(0, 0);
        if(temp1 != NULL) {
            temp1->menu_flag = 0x71;
            temp2 = find_menu(HOME_MENU, 0);    // get "HOME_MENU" menu
            if(temp2 != NULL) {
                temp2->menu_flag = 0x01;
                delay_ms(200);
                return;
            }
        }
    }
    
    temp1 = find_menu(0, 0);
    if(temp1 != NULL && temp1->menu_flag & 0x80) {
        temp = temp1->menu_flag & 0x0f;
        _routine_function();
        oled_first_menu_select(temp - 1, IS_IMMEDIATELY);
        temp1->menu_flag &= 0x7f;
    }

    if(!key_read(K1)) {
        delay_ms(10);
        if(!key_read(K1)) { 
            renew_sleep_time();
            temp1 = find_menu(0, 0);
            if(temp1 != NULL) {
                temp1->menu_flag |= 0x70;
                temp = (temp1->menu_flag & 0x0f);
                if(temp == HOME_MENU) {                 // handle "HOME_MENU" envent
                    temp2 = find_menu(HOME_MENU, 0);    // get "HOME_MENU" menu
                    if(temp2 != NULL) {
                        temp2->menu_flag = 0x01;
                        delay_ms(200);
                        return;
                    }
                } else if(temp == SETTING_MENU) {           // handle "SETTING_MENU" envent
                    temp2 = find_menu(SETTING_MENU, 0);     // get "SETTING_MENU" menu
                    if(temp2 != NULL) {
                        temp2->menu_flag = 0x01;
                        oled_show_setting_items(0, NOT_IMMEDIATELY);
                        oled_normal_move(DIRECTION_UP);
                        delay_ms(500);
                        while(temp1->menu_flag & 0x70) {
                            key_setting_menu_scan();
                        }
                        oled_first_menu_select(SETTING_MENU - 1, NOT_IMMEDIATELY);
                        oled_normal_move(DIRECTION_DOWN);
                    }
                } else if(temp == SEND_MENU) {
                    infrared_send_data();
                } else if(temp == POWER_MENU) {
                    get_oled_state() ? oled_turn_off() : oled_turn_on();
                }
            } else {
                // led_state() ? led_turn_on() : led_turn_off();
            }
        }
        // delay_ms(200);
        while(!key_read(K1)) {
            delay_ms(200);
        }
    }

    if(!key_read(K6)) {
        delay_ms(10);
        if(!key_read(K6)) { 
            renew_sleep_time();
            temp1 = find_menu(0, 0);
            if(temp1 != NULL) {
                temp1->menu_flag = 0x71;
                temp2 = find_menu(HOME_MENU, 0);    // get "HOME_MENU" menu
                if(temp2 != NULL) {
                    temp2->menu_flag = 0x01;
                    delay_ms(200);
                    return;
                }
            }
        }
    }

    if(!key_read(K3)) {
        delay_ms(10);
        if(!key_read(K3)) { 
            renew_sleep_time();
            temp1 = find_menu(0, 0);
            if(temp1 != NULL) {
                temp = (temp1->menu_flag & 0x0f);
                temp = temp > 1 ? temp - 1 : FIRST_MENU_NUMBER;
                oled_show_first_menu_icon_move(temp - 1, DIRECTION_RIGHT);
                oled_first_menu_select(temp - 1, IS_IMMEDIATELY);
                temp1->menu_flag &= 0x70; 
                temp1->menu_flag |= temp;
            } else {
                led_state() ? led_turn_on() : led_turn_off();
            }
        }
        // delay_ms(200);
        while(!key_read(K3)) {
            delay_ms(200);
        }
    }

    if(!key_read(K5)) {
        delay_ms(10);
        if(!key_read(K5)) { 
            renew_sleep_time();
            temp1 = find_menu(0, 0);
            if(temp1 != NULL) {
                temp = (temp1->menu_flag & 0x0f);
                temp = temp < FIRST_MENU_NUMBER ? temp + 1 : 1;
                oled_show_first_menu_icon_move(temp - 1, DIRECTION_LEFT);
                oled_first_menu_select(temp - 1, IS_IMMEDIATELY);
                temp1->menu_flag &= 0x70; 
                temp1->menu_flag |= temp;
            } else {
                led_state() ? led_turn_on() : led_turn_off();
            }
        }
        // delay_ms(200);
        while(!key_read(K5)) {
            delay_ms(200);
        }
    }
    return;
}

/**
  * @brief  Scan the key state in the home menu.
  * @retval none.
  */
void key_home_menu_scan(void) {
    unsigned char temp;
    menu_info_t *temp1, *temp2;

    temp1 = find_menu(0, 0);
    if(temp1 != NULL && temp1->menu_flag & 0x80) {
        _routine_function();
        temp1->menu_flag &= 0x7f;
        temp2 = find_menu(HOME_MENU, 0);
        if(temp2 != NULL) {
            temp = temp2->menu_flag & 0x0f;
            oled_show_home_page(temp - 1, IS_CHARGE, IS_IMMEDIATELY);
            temp2->menu_flag &= 0x7f;
        }
    }

    if(!get_oled_state() && get_human_detect()) {
        oled_turn_on();
        temp1 = find_menu(HOME_MENU, 0);
        if(temp1 != NULL) { 
            temp1->menu_flag = 0x01;
            oled_show_home_page(temp1->menu_flag - 1, IS_CHARGE, IS_IMMEDIATELY);
        }
    }

    if(!key_read(K6)) {
        delay_ms(10);
        if(!key_read(K6)) { 
            renew_sleep_time();
            if(!get_oled_state()) {
                oled_turn_on();
                temp1 = find_menu(HOME_MENU, 0);
                temp1->menu_flag = 0x01;
                if(temp1 != NULL) { 
                    oled_show_home_page(temp1->menu_flag - 1, IS_CHARGE, IS_IMMEDIATELY);
                }
                while(!key_read(K6)) {
                    delay_ms(200);
                }
                return;
            }
            temp1 = find_menu(0, 0);
            if(temp1 != NULL) {
                temp2 = find_menu(HOME_MENU, 0);
                if(temp2 != NULL) {
                    temp1->menu_flag = 0x81;
                    oled_first_menu_select(HOME_MENU - 1, NOT_IMMEDIATELY);
                    oled_normal_move(DIRECTION_DOWN);
                    delay_ms(200);
                    while((temp1->menu_flag & 0x7f) != 0x71) {
                        key_first_menu_scan();
                    }
                    temp2->menu_flag = 0x01;
                    oled_show_home_page(temp2->menu_flag - 1, IS_CHARGE, NOT_IMMEDIATELY);
                    oled_normal_move(DIRECTION_UP);
                    temp1->menu_flag = 0x71;
                }
            } else {
                led_state() ? led_turn_on() : led_turn_off();
            }
        }
        // delay_ms(200);
        while(!key_read(K6)) {
            delay_ms(200);
        }
    }

    if(!key_read(K1)) {
        delay_ms(10);
        if(!key_read(K1)) { 
            renew_sleep_time();
            if(!get_oled_state()) {
                oled_turn_on();
                temp1 = find_menu(HOME_MENU, 0);
                if(temp1 != NULL) { 
                    temp1->menu_flag = 0x01;
                    oled_show_home_page(temp1->menu_flag - 1, IS_CHARGE, IS_IMMEDIATELY);
                }
                while(!key_read(K1)) {
                    delay_ms(200);
                }
                return;
            }
            temp1 = find_menu(HOME_MENU, 0);
            if(temp1 != NULL) {
                temp2 = find_menu(HOME_MENU, temp1->menu_flag & 0x0f);
                if(temp2 != NULL) {
                    temp = (temp1->menu_flag & 0x0f) - 1;
                    temp2->menu_flag = 0xf0;
                    oled_show_home_page(temp, IS_CHARGE, NOT_IMMEDIATELY);
                    oled_change_T_or_H_tip(temp2->menu_flag & 0x0f, temp, 0);
                    delay_ms(500);
                    while(temp2->menu_flag & 0xf0) {
                        key_home_menu_change_data();
                    }
                    oled_show_home_page(temp, IS_CHARGE, IS_IMMEDIATELY);
                }
            } else {
                led_state() ? led_turn_on() : led_turn_off();
            }
        }
        // delay_ms(200);
        while(!key_read(K1)) {
            delay_ms(200);
        }
    }

    if(!key_read(K3)) {
        delay_ms(10);
        if(!key_read(K3)) { 
            renew_sleep_time();
            if(!get_oled_state()) {
                oled_turn_on();
                temp1 = find_menu(HOME_MENU, 0);
                if(temp1 != NULL) { 
                    temp1->menu_flag = 0x01;
                    oled_show_home_page(temp1->menu_flag - 1, IS_CHARGE, IS_IMMEDIATELY);
                }
                while(!key_read(K3)) {
                    delay_ms(200);
                }
                return;
            }
            temp1 = find_menu(HOME_MENU, 0);
            if(temp1 != NULL) {
                temp2 = find_menu(HOME_MENU, temp1->menu_flag & 0x0f);
                if(temp2 != NULL) {
                    if(!(temp2->menu_flag & 0xf0)) {
                        temp = (temp1->menu_flag & 0x0f) - 1;
                        temp = temp ? IS_TEMPERATURE : IS_HUMIDITY;
                        oled_show_home_page(temp, IS_CHARGE, NOT_IMMEDIATELY);
                        oled_normal_move(DIRECTION_LEFT);
                        temp1->menu_flag = temp + 1;
                    }
                }
            } else {
                led_state() ? led_turn_on() : led_turn_off();
            }        
        }
        // delay_ms(200);
        while(!key_read(K3)) {
            delay_ms(200);
        }
    }

    if(!key_read(K5)) {
        delay_ms(10);
        if(!key_read(K5)) { 
            renew_sleep_time();
            if(!get_oled_state()) {
                oled_turn_on();
                temp1 = find_menu(HOME_MENU, 0);
                if(temp1 != NULL) { 
                    temp1->menu_flag = 0x01;
                    oled_show_home_page(temp1->menu_flag - 1, IS_CHARGE, IS_IMMEDIATELY);
                }
                while(!key_read(K5)) {
                    delay_ms(200);
                }
                return;
            }
            temp1 = find_menu(HOME_MENU, 0);
            if(temp1 != NULL) {
                temp2 = find_menu(HOME_MENU, temp1->menu_flag & 0x0f);
                if(temp2 != NULL) {
                    if(!(temp2->menu_flag & 0xf0)) {
                        temp = (temp1->menu_flag & 0x0f) - 1;
                        temp = temp ? IS_TEMPERATURE : IS_HUMIDITY;
                        oled_show_home_page(temp, IS_CHARGE, NOT_IMMEDIATELY);
                        oled_normal_move(DIRECTION_RIGHT);
                        temp1->menu_flag = temp + 1;
                    }
                }
            } else {
                led_state() ? led_turn_on() : led_turn_off();
            }
        }
        // delay_ms(200);
        while(!key_read(K5)) {
            delay_ms(200);
        }
    }

    return;
}

/**
  * @brief  Scan the key state in the home change data menu.
  * @retval none.
  */
void key_home_menu_change_data(void) {
    float float_temp;
    float max_value, min_value;
    unsigned char target;
    unsigned char temp;
    menu_info_t *temp1, *temp2;

    if(!get_oled_state()) {
        temp1 = find_menu(HOME_MENU, 0);
        if(temp1 != NULL) {
            temp2 = find_menu(HOME_MENU, temp1->menu_flag & 0x0f);
            temp2->menu_flag = 0x00;
            return;
        }
    }

    temp1 = find_menu(0, 0);
    if(temp1 != NULL && temp1->menu_flag & 0x80) {
        _routine_function();
        temp1->menu_flag &= 0x7f;
        temp1 = find_menu(HOME_MENU, 0);
        if(temp1 != NULL) {
            temp2 = find_menu(HOME_MENU, temp1->menu_flag & 0x0f);
            if(temp2 != NULL) {
                if(temp2->menu_flag & 0xf0) {
                    target = (temp1->menu_flag & 0x0f) - 1;
                    temp = temp2->menu_flag & 0x0f;
                    oled_show_home_page(target, IS_CHARGE, NOT_IMMEDIATELY);
                    oled_change_T_or_H_tip(temp, target, 0);
                }
            }
        }
    }

    

    if(!key_read(K6)) {
        delay_ms(10);
        if(!key_read(K6)) { 
            renew_sleep_time();
            temp1 = find_menu(HOME_MENU, 0);
            if(temp1 != NULL) {
                temp2 = find_menu(HOME_MENU, temp1->menu_flag & 0x0f);
                if(temp2->menu_flag & 0xf0) {
                    temp2->menu_flag = 0x00;
                }
            } else {
                led_state() ? led_turn_on() : led_turn_off();
            }
        }
        // delay_ms(200);
        while(!key_read(K6)) {
            delay_ms(200);
        }
    }

    if(!key_read(K3)) {
        delay_ms(10);
        if(!key_read(K3)) { 
            renew_sleep_time();
            temp1 = find_menu(HOME_MENU, 0);
            if(temp1 != NULL) {
                temp2 = find_menu(HOME_MENU, temp1->menu_flag & 0x0f);
                if(temp2 != NULL) {
                    if(temp2->menu_flag & 0xf0) {
                        target = (temp1->menu_flag & 0x0f) - 1;
                        temp = temp2->menu_flag & 0x0f;
                        oled_show_home_page(target, IS_CHARGE, NOT_IMMEDIATELY);
                        temp = oled_change_T_or_H_tip(temp, target, -1);
                        temp2->menu_flag = (0xf0 | temp);
                    }
                } else {
                    led_state() ? led_turn_on() : led_turn_off();
                }
            } else {
                led_state() ? led_turn_on() : led_turn_off();
            }
        }
        // delay_ms(200);
        while(!key_read(K3)) {
            delay_ms(200);
        }
    }

    if(!key_read(K5)) {
        delay_ms(10);
        if(!key_read(K5)) { 
            renew_sleep_time();
            temp1 = find_menu(HOME_MENU, 0);
            if(temp1 != NULL) {
                temp2 = find_menu(HOME_MENU, temp1->menu_flag & 0x0f);
                if(temp2 != NULL) {
                    if(temp2->menu_flag & 0xf0) {
                        target = (temp1->menu_flag & 0x0f) - 1;
                        temp = temp2->menu_flag & 0x0f;
                        oled_show_home_page(target, IS_CHARGE, NOT_IMMEDIATELY);
                        temp = oled_change_T_or_H_tip(temp, target, +1);
                        temp2->menu_flag = (0xf0 | temp);
                    }
                } else {
                    led_state() ? led_turn_on() : led_turn_off();
                }
            } else {
                led_state() ? led_turn_on() : led_turn_off();
            }
        }
        // delay_ms(200);
        while(!key_read(K5)) {
            delay_ms(200);
        }
    }
    
    if(!key_read(K2)) {
        delay_ms(10);
        if(!key_read(K2)) { 
            renew_sleep_time();
            temp1 = find_menu(HOME_MENU, 0);
            if(temp1 != NULL) {
                temp2 = find_menu(HOME_MENU, temp1->menu_flag & 0x0f);
                if(temp2 != NULL) {
                    if(temp2->menu_flag & 0xf0) {
                        target = (temp1->menu_flag & 0x0f) - 1;
                        temp = temp2->menu_flag & 0x0f;
                        float_temp = temp2->item_data;
                        max_value = set_max_min_T_or_H_value[temp1->menu_flag - 1][0]; 
                        min_value = set_max_min_T_or_H_value[temp1->menu_flag - 1][1];
                        switch (temp)  {
                            case 0:
                                if(float_temp >= 10) float_temp = float_temp + 10 > max_value ? max_value : float_temp + 10;
                                else {
                                    float_temp = float_temp + 1 > max_value ? max_value : float_temp + 1;
                                    if(float_temp >= 10) temp = 1;
                                }
                                break;
                            case 1:
                                if(float_temp >= 10) float_temp = float_temp + 1 > max_value ? max_value : float_temp + 1;
                                break;
                            case 2:
                                if(float_temp < 10) float_temp = float_temp + 0.1 > max_value ? max_value : float_temp + 0.1;
                                if(float_temp >= 10) temp = 3;
                                break;
                            case 3:
                                if(float_temp >= 10) float_temp = float_temp + 0.1 > max_value ? max_value : float_temp + 0.1;
                            default:
                                break;
                        }
                        temp2->menu_flag = (0xf0 | temp);
                        temp2->item_data = float_temp;
                        set_T_or_H_set_data(float_temp, target);
                        oled_change_T_or_H_tip(temp, target, 0);
                    }
                } else {
                    led_state() ? led_turn_on() : led_turn_off();
                }
            } else {
                led_state() ? led_turn_on() : led_turn_off();
            }
        }
        // delay_ms(200);
        while(!key_read(K2)) {
            delay_ms(200);
        }
    }

    if(!key_read(K4)) {
        delay_ms(10);
        if(!key_read(K4)) { 
            renew_sleep_time();
            temp1 = find_menu(HOME_MENU, 0);
            if(temp1 != NULL) {
                temp2 = find_menu(HOME_MENU, temp1->menu_flag & 0x0f);
                if(temp2 != NULL) {
                    if(temp2->menu_flag & 0xf0) {
                        target = (temp1->menu_flag & 0x0f) - 1;
                        temp = temp2->menu_flag & 0x0f;
                        float_temp = temp2->item_data;
                        max_value = set_max_min_T_or_H_value[temp1->menu_flag - 1][0]; 
                        min_value = set_max_min_T_or_H_value[temp1->menu_flag - 1][1];
                        switch (temp)  {
                            case 0:
                                if(float_temp >= 10) {
                                    float_temp = float_temp - 10 < min_value ? min_value : float_temp - 10;
                                    if(float_temp < 10) temp = 0;
                                }
                                else float_temp = float_temp - 1 < min_value ? min_value : float_temp - 1;
                                break;
                            case 1:
                                if(float_temp >= 10) {
                                    float_temp = float_temp - 1 < min_value ? min_value : float_temp - 1;
                                    if(float_temp < 10) temp = 0;
                                }
                                break;
                            case 2:
                                if(float_temp < 10) {
                                    float_temp = float_temp - 0.1 < min_value ? min_value : float_temp - 0.1;
                                }
                                break;
                            case 3:
                                if(float_temp >= 10) {
                                    float_temp = float_temp - 0.1 < min_value ? min_value : float_temp - 0.1;
                                    if(float_temp < 10) temp = 2;
                                }
                            default:
                                break;
                        }
                        temp2->menu_flag = (0xf0 | temp);
                        temp2->item_data = float_temp;
                        set_T_or_H_set_data(float_temp, target);
                        oled_change_T_or_H_tip(temp, target, 0);
                    }
                } else {
                    led_state() ? led_turn_on() : led_turn_off();
                }
            } else {
                led_state() ? led_turn_on() : led_turn_off();
            }
        }
        // delay_ms(200);
        while(!key_read(K4)) {
            delay_ms(200);
        }
    }

    return;
}

/**
  * @brief  Scan the key state in the setting menu.
  * @retval none.
  */
void key_setting_menu_scan(void) {
    unsigned char temp, target;
    menu_info_t *temp1, *temp2;
    if(!get_oled_state()) {
        temp1 = find_menu(0, 0);
        if(temp1 != NULL) {
            temp1->menu_flag &= 0x02;
            return;
        }
    }

    temp1 = find_menu(0, 0);
    if(temp1 != NULL && temp1->menu_flag & 0x80) {
        _routine_function();
        temp1->menu_flag &= 0x7f;
        temp2 = find_menu(SETTING_MENU, 0);
        if(temp2 != NULL) {
            target = (temp2->menu_flag & 0x0f) - 1;
            oled_show_setting_items(target, IS_IMMEDIATELY);
        }
    }

    if(!key_read(K6)) {
        delay_ms(10);
        if(!key_read(K6)) { 
            renew_sleep_time();
            temp1 = find_menu(0, 0);
            if(temp1 != NULL) {
                temp1->menu_flag &= 0x82;
            } else {
                led_state() ? led_turn_on() : led_turn_off();
            }
        }
        // delay_ms(200);
        while(!key_read(K6)) {
            delay_ms(200);
        }
    }

    if(!key_read(K1)) {
        delay_ms(10);
        if(!key_read(K1)) { 
            renew_sleep_time();
            temp1 = find_menu(SETTING_MENU, 0);
            if(temp1 != NULL) {
                temp2 = find_menu(SETTING_MENU, temp1->menu_flag);
                if(temp2 != NULL) {
                    target = (temp1->menu_flag & 0x0f) - 1;
                    temp1->menu_flag |= 0xf0;
                    oled_show_setting_item_detial(target, 0, NOT_IMMEDIATELY);
                    oled_normal_move(DIRECTION_LEFT);
                    delay_ms(500);
                    while(temp1->menu_flag & 0xf0) {
                        key_setting_menu_hanle_data();
                    }
                    oled_show_setting_items(target, NOT_IMMEDIATELY);
                    oled_normal_move(DIRECTION_RIGHT);
                }
            } else {
                led_state() ? led_turn_on() : led_turn_off();
            }
        }
        // delay_ms(200);
        while(!key_read(K1)) {
            delay_ms(200);
        }
    }

    if(!key_read(K2)) {
        delay_ms(10);
        if(!key_read(K2)) { 
            renew_sleep_time();
            temp1 = find_menu(SETTING_MENU, 0);
            if(temp1 != NULL) {
                temp = (temp1->menu_flag & 0x0f);
                temp = temp > 1 ? temp-1 : SETTING_MENU_ITEM_NUMBER;
                oled_show_setting_items(temp-1, IS_IMMEDIATELY);
                temp1->menu_flag = temp;
            } else {
                led_state() ? led_turn_on() : led_turn_off();
            }        
        }
        // delay_ms(200);
        while(!key_read(K2)) {
            delay_ms(200);
        }
    }

    if(!key_read(K4)) {
        delay_ms(10);
        if(!key_read(K4)) { 
            renew_sleep_time();
            temp1 = find_menu(SETTING_MENU, 0);
            if(temp1 != NULL) {
                temp = (temp1->menu_flag & 0x0f);
                temp = temp < SETTING_MENU_ITEM_NUMBER ? temp+1 : 1;
                oled_show_setting_items(temp-1, IS_IMMEDIATELY);
                temp1->menu_flag = temp;
            } else {
                led_state() ? led_turn_on() : led_turn_off();
            }
        }
        // delay_ms(200);
        while(!key_read(K4)) {
            delay_ms(200);
        }
    }
    
    return;
}

/**
  * @brief  Scan the key state in setting menu hanle data.
  * @retval none.
  */
void key_setting_menu_hanle_data(void) {
    unsigned char temp, target;
    menu_info_t *temp1, *temp2;
    if(!get_oled_state()) {
        temp1 = find_menu(SETTING_MENU, 0);
        if(temp1 != NULL) {
            temp2 = find_menu(SETTING_MENU, temp1->menu_flag & 0x0f);
            if(temp2 != NULL) {
                temp2->menu_flag &= 0x0f;
                temp1->menu_flag &= 0x0f;
                return;
            }
        }
    }

    temp1 = find_menu(0, 0);
    if(temp1 != NULL && temp1->menu_flag & 0x80) {
        _routine_function();
        temp1->menu_flag &= 0x7f;
        temp1 = find_menu(SETTING_MENU, 0);
        if(temp1 != NULL) {
            temp2 = find_menu(SETTING_MENU, temp1->menu_flag & 0x0f);
            if(temp2 != NULL) {
                target = (temp1->menu_flag & 0x0f) - 1;
                oled_show_setting_item_detial(target, temp2->menu_flag & 0xf0, IS_IMMEDIATELY);
            }
        }
    }

    if(!key_read(K6)) {
        delay_ms(10);
        if(!key_read(K6)) { 
            renew_sleep_time();
            temp1 = find_menu(SETTING_MENU, 0);
            if(temp1 != NULL) {
                temp2 = find_menu(SETTING_MENU, temp1->menu_flag & 0x0f);
                if(temp2 != NULL) {
                    target = (temp1->menu_flag & 0x0f) - 1;
                    if(temp2->menu_flag & 0xf0) {
                        temp2->menu_flag &= 0x0f;
                        oled_show_setting_item_detial(target, temp2->menu_flag & 0x0f, IS_IMMEDIATELY);
                        led_state() ? led_turn_on() : led_turn_off();
                    } else {
                        temp1->menu_flag &= 0x0f;
                    }
                }
            } else {
                led_state() ? led_turn_on() : led_turn_off();
            }
        }
        // delay_ms(200);
        while(!key_read(K6)) {
            delay_ms(200);
        }
    }

    if(!key_read(K1)) {
        delay_ms(10);
        if(!key_read(K1)) { 
            renew_sleep_time();
            temp1 = find_menu(SETTING_MENU, 0);
            if(temp1 != NULL) {
                temp2 = find_menu(SETTING_MENU, temp1->menu_flag & 0x0f);
                if(temp2 != NULL) {
                    target = (temp1->menu_flag & 0x0f) - 1;
                    temp2->menu_flag |= 0xf0;
                    oled_show_setting_item_detial(target, temp2->menu_flag & 0xf0, IS_IMMEDIATELY);
                }
            } else {
                led_state() ? led_turn_on() : led_turn_off();
            }
        }
        // delay_ms(200);
        while(!key_read(K1)) {
            delay_ms(200);
        }
    }

    if(!key_read(K2)) {
        delay_ms(10);
        if(!key_read(K2)) { 
            renew_sleep_time();
            temp1 = find_menu(SETTING_MENU, 0);
            if(temp1 != NULL) {
                temp2 = find_menu(SETTING_MENU, temp1->menu_flag & 0x0f);
                if(temp2 != NULL) {
                    if(temp2 != NULL) {
                        target = (temp1->menu_flag & 0x0f) - 1;
                        if(temp2->menu_flag & 0xf0) {
                            temp = temp2->item_data;
                            switch(target) {
                                case 0: 
                                        temp = temp < 60 ? temp+5 : 5; 
                                        set_device_sleep_time(temp);
                                        break;
                                case 1: 
                                        temp = temp < 120 ? temp+10 : 30;
                                        infrared_set_send_frequence(temp);
                                        set_device_setting_frequency(temp);
                                        break;
                                case 2: 
                                        temp = temp < 4 ? temp+1 : 1;
                                        set_device_setting_brightness(temp); 
                                        break;
                                default: break;
                            }
                            
                            oled_show_setting_item_detial(target, temp2->menu_flag & 0xf0, IS_IMMEDIATELY);
                            temp2->item_data = temp;
                        }
                    }
                }
            } else {
                led_state() ? led_turn_on() : led_turn_off();
            }
        }
        // delay_ms(200);
        while(!key_read(K2)) {
            delay_ms(200);
        }
    }

    if(!key_read(K4)) {
        delay_ms(10);
        if(!key_read(K4)) { 
            renew_sleep_time();
            temp1 = find_menu(SETTING_MENU, 0);
            if(temp1 != NULL) {
                temp2 = find_menu(SETTING_MENU, temp1->menu_flag & 0x0f);
                if(temp2 != NULL) {
                    if(temp2 != NULL) {
                        if(temp2->menu_flag &= 0xf0) {
                            target = (temp1->menu_flag & 0x0f) - 1;
                            // temp2->menu_flag &= 0x0f;
                            temp = temp2->item_data;
                            switch(target) {
                                case 0: 
                                        temp = temp > 5 ? temp-5 : 60;
                                        set_device_sleep_time(temp);
                                        break;
                                case 1: 
                                        temp = temp > 30 ? temp-10 : 120; 
                                        infrared_set_send_frequence(temp);
                                        set_device_setting_frequency(temp);
                                        break;
                                case 2: 
                                        temp = temp > 1 ? temp-1 : 4; 
                                        set_device_setting_brightness(temp); 
                                        break;
                                default: break;
                            }

                            oled_show_setting_item_detial(target, temp2->menu_flag & 0xf0, IS_IMMEDIATELY);
                            temp2->item_data = temp;
                        }
                    }
                }
            } else {
                led_state() ? led_turn_on() : led_turn_off();
            }
        }
        // delay_ms(200);
        while(!key_read(K4)) {
            delay_ms(200);
        }
    }
    return;
}

void key_test_function(void) {
    // key_first_menu_scan();
    // key_setting_menu_hanle_data();
    // key_setting_menu_hanle_data();
    return;
}


