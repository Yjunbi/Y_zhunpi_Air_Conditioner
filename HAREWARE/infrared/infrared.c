#include "infrared.h"

uint16_t infrared_send_frequence = 120;
uint8_t sync_flag = 0;

/**
  * @brief  Initialize infrared gpio.
  * @retval None
  */
void infrared_init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(INFRARED_RCC_GPIOX, ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = INFRARED_GPIO_Pin_X;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(INFRARED_GPIOX, &GPIO_InitStruct);
	
	GPIO_ResetBits(INFRARED_GPIOX, INFRARED_GPIO_Pin_X);

    infrared_send_frequence = 120;
    return;
}

/**
  * @brief  Making the infrared gpio becomes hight level.
  * @retval None
  */
void infrared_output_high(void) {
    GPIO_SetBits(INFRARED_GPIOX, INFRARED_GPIO_Pin_X);
    return;
}

/**
  * @brief  Making the infrared gpio becomes low level.
  * @retval None
  */
void infrared_output_low(void){
    GPIO_ResetBits(INFRARED_GPIOX, INFRARED_GPIO_Pin_X);
    return;
}

/**
  * @brief  Set the frequence of send infrared signal.
  * @param  second: how many seconds send once 
  *                 (max_second:INFRARED_SEND_MAX_SECONE)
  *                 (min_second:INFRARED_SEND_MIN_SECONE)
  * @retval None
  */
void infrared_set_send_frequence(uint16_t second) {
    if(second > INFRARED_SEND_MAX_SECONE) {
        infrared_send_frequence = INFRARED_SEND_MAX_SECONE * 2; 
    } else if(second < INFRARED_SEND_MIN_SECONE) {
        infrared_send_frequence = INFRARED_SEND_MIN_SECONE *2;
    }
    else {
        infrared_send_frequence = second * 2; 
    }
    return;
}

/**
  * @brief  Set infrared send flag.
  * @param  flag: the flag is allow to send data or not.(DISABLE_SEND or ABLE_SEND)
  * @retval None
  */
void set_infrared_send_flag(uint8_t flag) {
    sync_flag = flag;
    return;
}

/**
  * @brief  Get infrared send flag.
  * @retval Return infrared send flag.
  */
uint8_t get_infrared_send_flag(void) {
    return sync_flag;
}

/**
  * @brief  Get the frequence of send infrared signal.
  * @retval None.
  */
uint16_t infrared_get_send_frequece(void) {
    return infrared_send_frequence;
}

/**
  * @brief  Get the device addresss.
  * @retval None.
  */
unsigned char infrared_get_device_address(void) {
    return DEVICE_ADDRESS;
}

/**
  * @brief  Send temprature an humidity data by using infrared.
  * @retval None.
  */
void infrared_send_data(void) {
    float temp = 0;
    unsigned char i = 0, j = 0, send_byte; 
    unsigned char send_data[5][2] = {0,};  // integrated number + decimal number
    if(get_infrared_send_flag() == DISABLE_SEND) return;
    set_infrared_send_flag(DISABLE_SEND);  // Clear the flag
    timer2_turn_off();
    set_device_connect_state(1);
    oled_show_state_panel(IS_IMMEDIATELY);
    // get data from oled defined functions.
    temp = get_T_or_H_set_data(IS_TEMPERATURE) + 0.01;
    send_data[0][0] = temp; send_data[0][1] = (temp - (int)temp) * 10.0;

    temp = get_T_or_H_set_data(IS_HUMIDITY) + 0.01;
    send_data[1][0] = temp; send_data[1][1] = (temp - (int)temp) * 10.0;

    temp = get_T_or_H_detect_data(IS_TEMPERATURE) + 0.01;
    send_data[2][0] = temp; send_data[2][1] = (temp - (int)temp) * 10.0;

    temp =  get_T_or_H_detect_data(IS_HUMIDITY) + 0.01;
    send_data[3][0] = temp; send_data[3][1] = (temp - (int)temp) * 10.0;

    if(get_human_detect()) {
        send_data[4][0] = 0xff; send_data[4][1] = 0xff;
    }
    
    for(i=0; i<5; i++) {
        // guide 
        infrared_output_high();
        delay_us(900);
        infrared_output_low();
        delay_us(450);

        // device address code 0
        send_byte = infrared_get_device_address();
        for(j=0; j<8; j++) {
            infrared_output_high();
            delay_us(560);
            infrared_output_low();
            if(send_byte & (0x80 >> j)) delay_us(1680);
            else delay_us(560);
        }
        // device address contrary code 1
        send_byte = ~send_byte;
        for(j=0; j<8; j++) {
            infrared_output_high();
            delay_us(560);
            infrared_output_low();
            if(send_byte & (0x80 >> j)) delay_us(1680);
            else delay_us(560);
        }

        // 2
        // 0x80 set-temprature      // 0x40 set-humidity
        // 0x20 detect-temprature   // 0x10 detect-humidity
        // 0x08 detect-human
        send_byte = (0x80 >> i);
        for(j=0; j<8; j++) {
            infrared_output_high();
            delay_us(560);
            infrared_output_low();
            if(send_byte & (0x80 >> j)) delay_us(1680);
            else delay_us(560);
        }

        // 3
        // what is the data mean?
        // i = 0 set-temprature
        // i = 1 set-humidity
        // i = 2 detect-temprature
        // i = 3 detect-humidity
        // i = 4 detect-human
        send_byte = send_data[i][0];
        for(j=0; j<8; j++) {
            infrared_output_high();
            delay_us(560);
            infrared_output_low();
            if(send_byte & (0x80 >> j)) delay_us(1680);
            else delay_us(560);
        }
        
        // 4
        // internal between integrated number and decimal number;
        send_byte = 0x18;
        for(j=0; j<8; j++) {
            infrared_output_high();
            delay_us(560);
            infrared_output_low();
            if(send_byte & (0x80 >> j)) delay_us(1680);
            else delay_us(560);
        }

        // 5
        send_byte = send_data[i][1];
        for(j=0; j<8; j++) {
            infrared_output_high();
            delay_us(560);
            infrared_output_low();
            if(send_byte & (0x80 >> j)) delay_us(1680);
            else delay_us(560);
        }

        // 6
        // 10101 1010 
        send_byte = 0xaa;
        for(j=0; j<8; j++) {
            infrared_output_high();
            delay_us(560);
            infrared_output_low();
            if(send_byte & (0x80 >> j)) delay_us(1680);
            else delay_us(560);
        }

        // 7
        // 0101 0101
        send_byte = 0x55;
        for(j=0; j<8; j++) {
            infrared_output_high();
            delay_us(560);
            infrared_output_low();
            if(send_byte & (0x80 >> j)) delay_us(1680);
            else delay_us(560);
        }
    }
    set_device_connect_state(0);
    oled_show_state_panel(IS_IMMEDIATELY);
    timer2_turn_on();
    return;
}

