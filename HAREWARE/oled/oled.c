#include "oled.h"
#include "oledfont.h"
#include "temperature_number.h"
#include "battery_icon.h"
#include "connect_icon.h"
#include "human_detect_icon.h"
#include "first_menu_icon.h"

unsigned char oled_data_arrary[OLED_MAX_HIGH][OLED_MAX_WIDTH] = {0x00,};
unsigned char _oled_data_arrary[OLED_MAX_HIGH][OLED_MAX_WIDTH] = {0x00,};

state_panel_t state_panel;

const char first_menu_items[FIRST_MENU_NUMBER][9] = {"Home\0", "Settings\0", "Sync\0", "Sleep\0"};

data_form_t temprature_set_data, humidity_set_data;
data_form_t *set_data_form_array[HOME_MENU_ITEM_NUMBER];
data_form_t temprature_detect_data, humidity_detect_data;
data_form_t *detect_data_form_array[HOME_MENU_ITEM_NUMBER];
const char T_or_H_title_string[HOME_MENU_ITEM_NUMBER][12] = {"temprature\0", "humidity\0"};
const char T_or_H_head_name_string[HOME_MENU_ITEM_NUMBER][5] = {"set:\0", "now:\0"};

const char setting_items[SETTING_MENU_ITEM_NUMBER][12] = {"sleep time\0", "frequency\0", "brightness\0"};
setting_data_form_t setting_sleep, setting_frequency, setting_brightness;
setting_data_form_t *setting_data_form_array[SETTING_MENU_ITEM_NUMBER];

unsigned char oled_state = 1;  // turn on oled

/**
  * @brief  Translate the data from "_oled_data_arrary" to "oled_data_arrary". 
  * @retval None
  */
void oled_data_array_translate(void) {
    unsigned char i, j;
    for(i=0; i<OLED_MAX_HIGH; i++) {
        for(j=0; j<OLED_MAX_WIDTH; j++) {
            oled_data_arrary[i][j] = _oled_data_arrary[i][j];
        }
    }
    return;
}

/**
  * @brief  Initialize the hardware I2C of STM32. 
  * @retval None
  */
void _I2C_Configuration(void) {
	I2C_InitTypeDef    I2C_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	

	GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);

	//PB8---SCL   PB9---SDA
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	

	I2C_DeInit(I2C1);
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable; // 
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; //
	I2C_InitStructure.I2C_ClockSpeed = 400000; //
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2; //
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C; //
	I2C_InitStructure.I2C_OwnAddress1 = 0x10; //
	I2C_Init(I2C1, &I2C_InitStructure); //
	I2C_Cmd(I2C1,ENABLE); //
 
}

/**
  * @brief  Write a byte to oled by I2C interface. 
  * @retval None
  */
void _I2C_WriteByte(uint8_t addr,uint8_t data)
{
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)); //??I2C??????
	
	I2C_GenerateSTART(I2C1, ENABLE);  //??I2C1
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)); //????EV5,???
	
	I2C_Send7bitAddress(I2C1, SLAVE_ADDRESS, I2C_Direction_Transmitter);  //????(OLED)??
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
 
	I2C_SendData(I2C1, addr); //???????
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING));
	
	I2C_SendData(I2C1, data); //????
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING));
 
	I2C_GenerateSTOP(I2C1, ENABLE); //??I2C??
	return;
}

/**
  * @brief  Write a command byte to OLED. 
  * @retval None
  */
void oled_write_cmd(unsigned char oled_cmd){
	_I2C_WriteByte(0x00, oled_cmd);
	return;
}

/**
  * @brief  Write a data byte to OLED. 
  * @retval None
  */
void oled_write_data(unsigned char oled_data){
	_I2C_WriteByte(0x40, oled_data);
	return;
}

/**
  * @brief  Set the setted value.
  * @param  set_data: seted value.
  * @param is_T_or_H: choose temprature or humidity (IS_TEMPERATURE or IS_HUMIDITY)
  * @retval None
  */
void set_T_or_H_set_data(float set_data, unsigned char is_T_or_H) {
    set_data_form_array[is_T_or_H]->set_data = set_data;
    sprintf(set_data_form_array[is_T_or_H]->set_data_string, "%.1f\0" , set_data);
    set_data_form_array[is_T_or_H]->set_data_string_len = strlen(set_data_form_array[is_T_or_H]->set_data_string);
    return;
}

/**
  * @brief  Get the setted value.
  * @param is_T_or_H: choose temprature or humidity (IS_TEMPERATURE or IS_HUMIDITY)
  * @retval Setted value about temprature or humidity.
  */
float get_T_or_H_set_data(unsigned char is_T_or_H) {
    return set_data_form_array[is_T_or_H]->set_data;
}

/**
  * @brief  Set the detected value.
  * @param  detect_data: detect data
  * @param is_T_or_H: choose temprature or humidity (IS_TEMPERATURE or IS_HUMIDITY)
  * @retval None
  */
void set_T_or_H_detect_data(float detect_data, unsigned char is_T_or_H) {
    detect_data_form_array[is_T_or_H]->set_data = detect_data;
    sprintf(detect_data_form_array[is_T_or_H]->set_data_string, "%.1f\0" , detect_data);
    detect_data_form_array[is_T_or_H]->set_data_string_len = strlen(detect_data_form_array[is_T_or_H]->set_data_string);
    return;
}

/**
  * @brief  get the detected value.
  * @param is_T_or_H: choose temprature or humidity (IS_TEMPERATURE or IS_HUMIDITY)
  * @retval Detected value about temprature or humidity.
  */
float get_T_or_H_detect_data(unsigned char is_T_or_H) {
    return detect_data_form_array[is_T_or_H]->set_data;
}

/**
  * @brief  Set device connect state.
  * @param connect_state: connect state
  * @retval None.
  */
void set_device_connect_state(unsigned char connect_state) {
    state_panel.connect_state = connect_state;
    return;
}

/**
  * @brief  Get device connect state.
  * @retval device connect srate.
  */
unsigned char get_device_connect_state(void) {
    return state_panel.connect_state;
}

/**
  * @brief  Set device human state.
  * @param connect_state: connect state
  * @retval None.
  */
void set_device_human_state(unsigned char human_state) {
    if((state_panel.human_state & 0x01) != human_state) {
        state_panel.human_state = 0x00;
        state_panel.human_state |= (human_state | 0x80);
    }
    return;
}

/**
  * @brief  Get device human state.
  * @retval None.
  */
unsigned char get_device_human_state(void) {
    return state_panel.human_state;
}

/**
  * @brief  Set device charge state.
  * @param connect_state: connect state
  * @retval None.
  */
void set_device_charge_state(unsigned char charge_state) {
    if((state_panel.charge_state & 0x01) != charge_state) {
        state_panel.charge_state = 0x00;
        state_panel.charge_state |= (charge_state | 0x80);
    }
    return;
}

/**
  * @brief  Get device charge state.
  * @retval None.
  */
unsigned char get_device_charge_state(void) {
    return state_panel.charge_state;
}

/**
  * @brief  Set device battery state.
  * @param battery_state: battery state
  * @retval None.
  */
void set_device_battery_state(unsigned char battery_state) {
    state_panel.battery_state = battery_state;
    return;
}

/**
  * @brief  Get device battery state.
  * @retval The battery of devic.
  */
unsigned char get_device_battery_state(void) {
    return state_panel.battery_state;
}

/**
  * @brief  Initialize the state panel structure.
  * @retval None
  */
void state_panel_data_init(void) {
    state_panel.connect_state = 0x80;
    state_panel.human_state = 0x80;
    state_panel.charge_state = 0x80;
    state_panel.battery_state = 0x80;
    return;
}

/**
  * @brief  Set device sleep time.
  * @param  time: after "time" seconds turn off the OLED screen. 
  * @retval None.
  */
void set_device_sleep_time(unsigned char time) {
    setting_sleep.set_data = time;
    sprintf(setting_sleep.set_data_string, "%ds\0" , time);
    setting_sleep.set_data_string_len = strlen(setting_sleep.set_data_string);
    return;
}

/**
  * @brief  Get device sleep time.
  * @retval Return sleep time.
  */
unsigned char get_device_sleep_time(void) {
    return setting_sleep.set_data;
}

/**
  * @brief  Set device frequency of sending data.
  * @param  frequency: after "frequency" seconds send data. 
  * @retval None.
  */
void set_device_setting_frequency(unsigned char frequency) {
    setting_frequency.set_data = frequency;
    sprintf(setting_frequency.set_data_string, "%ds\0" , frequency);
    setting_frequency.set_data_string_len = strlen(setting_frequency.set_data_string);
    return;
}

/**
  * @brief  Get device frequency of sending data.
  * @retval Return frequency of sending data.
  */
unsigned char get_device_setting_frequency(void) {
    return setting_frequency.set_data;
}

/**
  * @brief  Set device OLED screen brightness.
  * @param  brightness: brightness(from 1 to 8). 
  * @retval None.
  */
void set_device_setting_brightness(unsigned char brightness) {
    setting_brightness.set_data = brightness;
    sprintf(setting_brightness.set_data_string, "%d\0" , brightness);
    setting_brightness.set_data_string_len = strlen(setting_brightness.set_data_string);
    oled_write_cmd(0x81); // contract control
	oled_write_cmd((128/8) * brightness);//--128   
    return;
}

/**
  * @brief  Get device OLED screen brightness.
  * @retval Return OLED screen brightness.
  */
unsigned char get_device_setting_brightness(void) {
    return setting_brightness.set_data;
}

/**
  * @brief  Initialize the data from structure.
  * @retval None
  */
void data_form_array_init(void) {
    set_data_form_array[IS_TEMPERATURE] = &temprature_set_data;
    set_data_form_array[IS_HUMIDITY] = &humidity_set_data;

    detect_data_form_array[IS_TEMPERATURE] = &temprature_detect_data;
    detect_data_form_array[IS_HUMIDITY] = &humidity_detect_data;

    set_T_or_H_detect_data(0.0, IS_TEMPERATURE);
    set_T_or_H_detect_data(0.0, IS_HUMIDITY);

    set_T_or_H_set_data(26, IS_TEMPERATURE);
    set_T_or_H_set_data(50, IS_HUMIDITY);

    setting_data_form_array[0] = &setting_sleep;
    setting_data_form_array[1] = &setting_frequency;
    setting_data_form_array[2] = &setting_brightness;
    set_device_sleep_time(60);
    set_device_setting_frequency(60); infrared_set_send_frequence(60);
    set_device_setting_brightness(4);
    return;
}

/**
  * @brief  Intialize oled by writing some particular command.
  * @retval None
  */
void oled_init(void) {
	_I2C_Configuration();
	
    data_form_array_init();
    state_panel_data_init();
    delay_ms(500);

	oled_write_cmd(0xAE);//--display off
	oled_write_cmd(0x00);//---set low column address
	oled_write_cmd(0x10);//---set high column address
	oled_write_cmd(0x40);//--set start line address  
	oled_write_cmd(0xB0);//--set page address
	oled_write_cmd(0x81); // contract control
	oled_write_cmd(0xFF);//--128   
	oled_write_cmd(0xA1);//set segment remap 
	oled_write_cmd(0xA6);//--normal / reverse
	oled_write_cmd(0xA8);//--set multiplex ratio(1 to 64)
	oled_write_cmd(0x3F);//--1/32 duty
	oled_write_cmd(0xC8);//Com scan direction
	oled_write_cmd(0xD3);//-set display offset
	oled_write_cmd(0x00);//

	oled_write_cmd(0xD5);//set osc division
	oled_write_cmd(0x80);//

	oled_write_cmd(0xD8);//set area color mode off
	oled_write_cmd(0x05);//

	oled_write_cmd(0xD9);//Set Pre-Charge Period
	oled_write_cmd(0xF1);//

	oled_write_cmd(0xDA);//set com pin configuartion
	oled_write_cmd(0x12);//

	oled_write_cmd(0xDB);//set Vcomh
	oled_write_cmd(0x30);//

	oled_write_cmd(0x8D);//set charge pump enable
	oled_write_cmd(0x14);//

    oled_write_cmd(0x20);//Set Memory Addressing Mode
	oled_write_cmd(0x00);//Horizontal Addressing Mode

	oled_write_cmd(0xAF);//--turn on oled panel
	return;
}

/**
  * @brief  Get device OLED screen state. ("0" represents the oled screen has been turn off, contraily, "1" reprsents it has been turn on)
  * @retval Return OLED screen state.(only return "0" or "1")
  */
unsigned char get_oled_state(void) {
    return oled_state;
}

/**
  * @brief  Turn on the OLED screen if it has been turn off.
  * @retval None.
  */
void oled_turn_on(void) {
    if(!oled_state) {
        oled_write_cmd(0xAF);//--turn on oled panel
        oled_state = 1;
    }
    return;
}

/**
  * @brief  Turn off the OLED screen if it has been turn on.
  * @retval None.
  */
void oled_turn_off(void) {
    if(oled_state) {
        oled_write_cmd(0xAE);//--turn on oled panel
        oled_state = 0;
    }
    
    return;
}

/**
  * @brief  Show the "oled_data_arrary" value what has wirtten in the array.
  * @retval None
  */
void oled_show_array_data(void) {
    unsigned char m,n;
    unsigned char addr = 0x40;

    if(!get_oled_state()) return;

    // 设置起始
    oled_write_cmd(0xb0);		    //page0-page1
	oled_write_cmd(0x00);			//low column start address
	oled_write_cmd(0x10);			//high column start address

    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)); //??I2C??????
	
	I2C_GenerateSTART(I2C1, ENABLE);  //??I2C1
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)); //????EV5,???
	
	I2C_Send7bitAddress(I2C1, SLAVE_ADDRESS, I2C_Direction_Transmitter);  //????(OLED)??
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
 
	I2C_SendData(I2C1, addr); //???????
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING));
	
    // 直接给n个数据， 减少时间的延迟
    for(m=0; m<OLED_MAX_HIGH; m++) {
		for(n=0; n<128; n++){
            I2C_SendData(I2C1, oled_data_arrary[m][n]); //????
	        while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING));
		}
	}
 
	I2C_GenerateSTOP(I2C1, ENABLE); //??I2C??
	return;
}

/**
  * @brief Show oled animation.
  * @param start_x: animation start x
  * @param start_y: animation start y
  * @param width: animation move width
  * @param high: animation move high
  * @param direction: oled screen slip direction
  *                   (DIRECTION_UP, DIRECTION_DOWN, DIRECTION_LEFT, DIRECTION_RIGHT, DIRECTION_NONE)
  * @param speed: animation move speed
  * @retval None 
  */
void oled_animation(unsigned char start_x, unsigned char start_y, unsigned char width, unsigned char high, unsigned char direction, unsigned char speed) {
    unsigned char n, i, j;
    timer2_turn_off();
    if(start_x + width > OLED_MAX_WIDTH) width = OLED_MAX_WIDTH - start_x;
    if(start_y + high > OLED_MAX_HIGH) high = OLED_MAX_WIDTH - start_y;
    if(direction == DIRECTION_LEFT) {
        // left
        for(n=0; n<width; n++) {
            for(i=start_y; i<start_y+high; i++) {
                for(j=start_x; j<start_x+width - 1; j++) {
                    oled_data_arrary[i][j] = oled_data_arrary[i][j + 1];
                }
                oled_data_arrary[i][j] = _oled_data_arrary[i][start_x + n];
            }
            if(!(n%speed)) oled_show_array_data();
        }
        oled_show_array_data();
    } else if(direction == DIRECTION_RIGHT) {
        // right
        for(n=0; n<width; n++) {
            for(i=start_y; i<start_y+high; i++) {
                for(j=start_x+width - 1; j>start_x; j--) {
                    oled_data_arrary[i][j] = oled_data_arrary[i][j - 1];
                }
                oled_data_arrary[i][j] = _oled_data_arrary[i][start_x + width - 1 - n];
            }
            if(!(n%speed)) oled_show_array_data();
        }
        oled_show_array_data();
    } else if(direction == DIRECTION_UP) {
        // up
        for(n=0; n<high*8; n++) {
            for(j=start_x; j<start_x+width; j++) {
                for(i=start_y; i<start_y+high - 1; i++) {
                    oled_data_arrary[i][j] >>= 1;
                    if(oled_data_arrary[i + 1][j] & 0x01) {
                        oled_data_arrary[i][j] |= 0x80;
                    }
                }
                oled_data_arrary[i][j] >>= 1;
                if(_oled_data_arrary[start_y + n/8][j] & (0x01 << n%8)) {
                    oled_data_arrary[i][j] |= 0x80;
                }
            }        
            if(!(n%speed)) oled_show_array_data();
        }
        oled_show_array_data();
    } else if(direction == DIRECTION_DOWN) {
        // down
        for(n=0; n<high*8; n++) {
            for(j=start_x; j<start_x+width; j++) {
                for(i=start_y+high - 1; i>start_y; i--) {
                    oled_data_arrary[i][j] <<= 1;
                    if(oled_data_arrary[i - 1][j] & 0x80) {
                        oled_data_arrary[i][j] |= 0x01;
                    }
                }
                oled_data_arrary[i][j] <<= 1;
                if(_oled_data_arrary[start_y + high - 1 - n/8][j] & (0x80 >> n%8)) {
                    oled_data_arrary[i][j] |= 0x01;
                }
            }        
            if(!(n%speed)) oled_show_array_data();
        }
        oled_show_array_data();
    } else {
        oled_data_array_translate();
        oled_show_array_data();
    }
    timer2_turn_on();
    return;
}

/**
  * @brief Defaule animation zone is the whold OLED screen.
  * @param direction: oled screen slip direction
  *                   (DIRECTION_UP, DIRECTION_DOWN, DIRECTION_LEFT, DIRECTION_RIGHT, DIRECTION_NONE)
  * @retval None 
  */
void oled_normal_move(unsigned char direction) {
    if(direction == DIRECTION_DOWN || direction == DIRECTION_UP) {
        oled_animation(0, 1, 128, 7, direction, 5);
    } else if(direction == DIRECTION_LEFT || direction == DIRECTION_RIGHT) {
        oled_animation(0, 1, 128, 7, direction, 9);
    } else {
        oled_data_array_translate();
        oled_show_array_data();
    }
    return;
}

/**
  * @brief  The OLED shows the consistent in every line.
  * @retval None
  */
void fill_picture(unsigned char fill_data) {
	unsigned char m,n;
	for(m=0;m<OLED_MAX_HIGH;m++) {
		for(n=0;n<OLED_MAX_WIDTH;n++){
            _oled_data_arrary[m][n] = fill_data;
		}
	}
    return;
}

/**
  * @brief  The OLED shows nothing.
  * @retval None
  */
void oled_clear(void) {
	fill_picture(0x00);
	return;
}

/**
  * @brief  The OLED shows nothing.
  * @param n: which line need to be cleared
  * @retval None
  */
void oled_clear_line_n(unsigned char n) {
    unsigned char j;
    for(j=0; j<OLED_MAX_WIDTH; j++) {
        _oled_data_arrary[n][j] = 0x00;
    }
    return;
}

/**
  * @brief  Show a char in OLED.
  * @param  x: x coordinate
  * @param  y: y coordinate
  * @param  chr: which char need to be showed in OLED
  * @retval None
  */
void oled_show_char(unsigned char x, unsigned char y, unsigned char chr) {
    unsigned char i, j, width, high;
    width = NORMAL_FONT_WIDTH;
    high = NORMAL_FONT_HIGH / 8;
    chr -= ' ';
    for(i=0; i<high; i++) {
        for(j=0; j<width; j++) {
            _oled_data_arrary[y+i][x+j] = F6x8[chr+i][j];
        }
    }
    return;
}

/**
  * @brief  Show a string in OLED.
  * @param  x: x coordinate
  * @param  y: y coordinate
  * @param  str: which str need to be showed in OLED
  * @retval None
  */
void oled_show_string(unsigned char x, unsigned char y, unsigned char *str) {
    unsigned char i;
    i = 0;
    while(str[i] != '\0') {
        oled_show_char(x, y, str[i]);
        x += NORMAL_FONT_WIDTH;
        i++;
    }	
    return;
}

/**
  * @brief  Show battery state in OLED.
  * @retval None
  */
void oled_show_battery_state(void) {	
	unsigned char j, battery_capacity;
    battery_capacity = state_panel.battery_state & 0x0f;
    if(battery_capacity > 4) battery_capacity = 4;
    for(j=0; j<BATTERY_ICON_W; j++) {
        _oled_data_arrary[BETTERY_ICON_Y][BETTERY_ICON_X+j] = battery_icon[battery_capacity][j];
    }
    state_panel.battery_state &= 0x7f;
	return;
}

/**
  * @brief  Show charge state in OLED.
  * @retval None
  */
void oled_show_charge_state(void) {
	unsigned char j, is_charge;
    is_charge = state_panel.charge_state & 0x01;
    for(j=0; j<CHARGE_ICON_W_H; j++) {
        _oled_data_arrary[CHARGE_ICON_Y][CHARGE_ICON_X+j] = charge_icon[is_charge][j];
    }
    state_panel.charge_state &= 0x7f;
    return;
}

/**
  * @brief  Show device connect state in OLED.
  * @retval None
  */
void oled_show_connect_state(void) {
	unsigned char j, is_connect;
    is_connect = state_panel.connect_state & 0x01;
    for(j=0; j<CONNECT_ICON_W_H; j++) {
        _oled_data_arrary[CONNECT_ICON_Y][CONNECT_ICON_X+j] = connect_icon[is_connect][j];
    }
    state_panel.connect_state &= 0x7f;
    return;
}

/**
  * @brief  Show human detect state in OLED.
  * @retval None
  */
void oled_show_human_detect_state(void) {
    unsigned char j, is_exist;
    is_exist = state_panel.human_state & 0x01;
    for(j=0; j<HUMAN_DETECT_ICON_W_AND_H; j++) {
        _oled_data_arrary[HUMAN_ICON_Y][HUMAN_ICON_X+j] = human_detect_icon[is_exist][j];
    }
    state_panel.human_state &= 0x7f;

    return;
}

/**
  * @brief  Show setted temprature or humidity data in OLED.
  * @param is_T_or_H: choose temprature or humidity (IS_TEMPERATURE or IS_HUMIDITY)
  * @retval None
  */
void oled_show_set_T_or_H(unsigned char is_T_or_H) {
    oled_show_temperature_humidity(SETTED_T_A_H_X, SETTED_T_A_H_Y, set_data_form_array[is_T_or_H]->set_data_string, is_T_or_H);
    return;
}

/**
  * @brief  Show detected temprature or humidity data in OLED.
  * @param is_T_or_H: choose temprature or humidity (IS_TEMPERATURE or IS_HUMIDITY)
  * @retval None
  */
void oled_show_detect_T_or_H(unsigned char is_T_or_H) {
    oled_show_temperature_humidity(SDETECTED_T_A_H_X, SDETECTED_T_A_H_Y, detect_data_form_array[is_T_or_H]->set_data_string, is_T_or_H);
    return;
}

/**
  * @brief  Show temprature or humidity item in OLED.
  * @retval None
  */
void oled_show_T_or_H_item_name(void) {
    oled_show_string(4, 4, (unsigned char *)T_or_H_head_name_string[0]);
    oled_show_string(4, 7, (unsigned char *)T_or_H_head_name_string[1]);
    return;
}

/**
  * @brief  Use new data to refresh OLED.
  * @param is_T_or_H: choose temprature or humidity (IS_TEMPERATURE or IS_HUMIDITY)
  * @retval None
  */
void oled_refresh_T_or_H(unsigned char is_T_or_H) {
    oled_show_set_T_or_H(is_T_or_H);
    oled_show_detect_T_or_H(is_T_or_H);
    oled_show_T_or_H_item_name();
    return;
}

/**
  * @brief Showing user which single number is changing.
  * @param number: which number
  * @param is_T_or_H: choose temprature or humidity
  * @param direction: choose slip to left, hold or right (-1, 0, 1) 
  * @retval Return ("number" + "direction")
  */
int oled_change_T_or_H_tip(int number, unsigned char is_T_or_H, int direction) {
    unsigned char j, tip_line_location;

    oled_show_set_T_or_H(is_T_or_H);
    number += direction;
    if(number == set_data_form_array[is_T_or_H]->set_data_string_len - 2) number += direction;
    
    if(direction > 0) {
        if(number > set_data_form_array[is_T_or_H]->set_data_string_len - 1) {
            number = 0;
        } 
    } else {
        if(number < 0) {
            number = set_data_form_array[is_T_or_H]->set_data_string_len - 1;
        } 
    }
     
    
    if(number >= 0 && number <= set_data_form_array[is_T_or_H]->set_data_string_len - 1) {
        tip_line_location = NUMBER_WRITH * number;
        for(j=0; j<NUMBER_WRITH; j++) {
            _oled_data_arrary[SETTED_T_A_H_Y + NUMBER_HIGH/8 - 1][SETTED_T_A_H_X + tip_line_location + j] |= 0x80;
        }
    }

    oled_data_array_translate();
    oled_show_array_data();
    return number;
}

/**
  * @brief Show state panel in OLED.
  * @param immediately: show immediately or not (NOT_IMMEDIATELY or IS_IMMEDIATELY)
  * @retval None 
  */
void oled_show_state_panel(unsigned char immediately) {
    oled_show_connect_state();
    oled_show_human_detect_state();
    oled_show_battery_state();
    if(immediately) oled_normal_move(0);
    return;
}

/**
  * @brief Show the first menu item in OLED.
  * @param n: which item need to be showed in the middle of the OLED
  *          (the value range is from "0" to the menu max itme numbr)
  * @retval None.
  */
void oled_show_first_menu_item(unsigned char n) {
    unsigned char x, y, width, high; 
    width = strlen(first_menu_items[n]) * NORMAL_FONT_WIDTH;
    high = FIRST_MENU_ICON_HIGH / 8;
    x = (OLED_MAX_WIDTH - width) / 2;
    y = 7;
    oled_clear_line_n(y);
    oled_show_string(x, y, (unsigned char *)first_menu_items[n]);
    return;
}

/**
  * @brief Show the first menu icon in OLED.
  * @param n: which icon need to be showed in the middle of the OLED
  *          (the value range is from "0" to "n-1" ("n" is the menu max itme numbr))
  * @retval None.
  */
void oled_show_first_menu_icon(unsigned char n) {
    unsigned char i, j, left, middle, right, icon_sum_number;
    unsigned char x, y, width, high, interval;
    icon_sum_number = FIRST_MENU_NUMBER - 1;
    width = FIRST_MENU_ICON_WIDTH;
    high = FIRST_MENU_ICON_HIGH / 8;
    interval = (OLED_MAX_WIDTH - 3*width) / (3-1); // show three icons in the OLED;
    y = 2;

    left = n == 0 ? icon_sum_number : n - 1;
    middle = n;
    right = n == icon_sum_number ? 0 : n + 1;

    x = interval*0 + FIRST_MENU_ICON_WIDTH*0;
    for(i=0; i<high; i++) {
        for(j=0; j<width; j++) {
            _oled_data_arrary[y + i][x + j] = first_menu_icon[left][i][j];
        }
    }

    x = interval*1 + FIRST_MENU_ICON_WIDTH*1;
    for(i=0; i<high; i++) {
        for(j=0; j<width; j++) {
            _oled_data_arrary[y + i][x + j] = first_menu_icon[middle][i][j];
        }
    }

    x = interval*2 + FIRST_MENU_ICON_WIDTH*2;
    for(i=0; i<high; i++) {
        for(j=0; j<width; j++) {
            _oled_data_arrary[y + i][x + j] = first_menu_icon[right][i][j];
        }
    }
    return;
}

/**
  * @brief Show the first menu icon animation.
  * @param n: the next icon need to be showed in the middle of the OLED
  *          (the value range is from "0" to "n-1" ("n" is the menu max itme numbr))
  *          ("n" need to out greater than or equal "3")
  *          (if the first middle icon is 0, so you just can set "n" is "1" or "n-1")
  * @param direction: icon slip direction
  *                   (DIRECTION_LEFT, DIRECTION_RIGHT, DIRECTION_NONE)
  * @retval None.
  */
void oled_show_first_menu_icon_move(unsigned char n, unsigned char direction) {
    unsigned char i, j, left, right, icon_sum_number;
    unsigned char x, y, width, high, interval;
    timer2_turn_off();
    icon_sum_number = FIRST_MENU_NUMBER - 1;
    width = FIRST_MENU_ICON_WIDTH;
    high = FIRST_MENU_ICON_HIGH / 8;
    interval = (OLED_MAX_WIDTH - 3*width) / (3-1); // show three icons in the OLED
    x = 0; y = 2;
    left = n == 0 ? icon_sum_number : n - 1;
    right = n == icon_sum_number ? 0 : n + 1;
    if(direction == DIRECTION_LEFT) {
        // left
        for(n=0; n<width+interval; n++) {
            for(i=y; i<y+high; i++) {
                for(j=x; j<x+OLED_MAX_WIDTH-1; j++) {
                    oled_data_arrary[i][j] = oled_data_arrary[i][j + 1];
                }
                if(n < interval) {
                    oled_data_arrary[i][j] = 0x00;
                } else {
                    oled_data_arrary[i][j] = first_menu_icon[right][i - y][n - interval];
                }
            }
            if(!(n%4)) oled_show_array_data();
        }
        oled_show_array_data();
    } else if(direction == DIRECTION_RIGHT) {
        // right
        for(n=0; n<width+interval; n++) {
            for(i=y; i<y+high; i++) {
                for(j=x+OLED_MAX_WIDTH-1; j>x; j--) {
                    oled_data_arrary[i][j] = oled_data_arrary[i][j-1];
                }
                if(n < interval) {
                    oled_data_arrary[i][j] = 0x00;
                } else {
                    oled_data_arrary[i][j] = first_menu_icon[left][i - y][width - n + interval - 1];
                }
            }
            if(!(n%4)) oled_show_array_data();
        }
        oled_show_array_data();
    } else {
        oled_normal_move(0);
    }
    timer2_turn_on();
    return;
}

/**
  * @brief  Show first menu selected icon and item in OLED.
  * @param n: which icon and item name need to be showed in the middle of the OLED
  *          (the value range is from "0" to "n-1" ("n" is the menu max itme numbr))
  * @param immediately: show immediately or not (NOT_IMMEDIATELY or IS_IMMEDIATELY)
  * @retval None.
  */
void oled_first_menu_select(unsigned char n, unsigned char immediately) {
    oled_clear();
    oled_show_first_menu_item(n);
    oled_show_first_menu_icon(n);
    if(immediately) oled_show_state_panel(immediately);
    return;
}

/**
  * @brief  Show temprature or humidity title in OLED.
  * @param is_T_or_H: choose temprature or humidity (IS_TEMPERATURE or IS_HUMIDITY)
  * @retval None.
  */
void oled_show_T_or_H_title(unsigned char is_T_or_H) {
    unsigned char title_string_len, x, y, j;
    title_string_len = strlen(T_or_H_title_string[is_T_or_H]) * NORMAL_FONT_WIDTH;
    x = (OLED_MAX_WIDTH - title_string_len - 1) / 2;
    y = 1;
    for(j=0; j<OLED_MAX_WIDTH; j++) _oled_data_arrary[y][j] = 0x00; 
    oled_show_string(x, y, (unsigned char *)T_or_H_title_string[is_T_or_H]);
    return;
}

/**
  * @brief  Show temprature or humidity data in OLED.
  * @param  x: x coordinate
  * @param  y: y coordinate
  * @param number_string: date number string
  * @param is_T_or_H: choose temprature or humidity (IS_TEMPERATURE or IS_HUMIDITY)
  * @retval None.
  */
void oled_show_temperature_humidity(unsigned char x, unsigned char y, char *number_string, unsigned char is_T_or_H) {
	char temp;
    unsigned char single_number, chr, i, j;	
	unsigned char width, high;
	
	width = NUMBER_WRITH;
	high = NUMBER_HIGH / 8;

    for(chr=0; chr<5; chr++) {
        temp = number_string[chr];
		if(!temp) break;
		single_number = temp - '0';
		if(temp == '.') single_number = 10;
		for(i=0; i<high; i++) {
			for(j=0; j<width; j++) {
				_oled_data_arrary[y+i][x+j] = temperature_number[single_number*high+i][j];
			}
		}
		x += NUMBER_WRITH;
	}
	
	if(is_T_or_H > 1) return;
	for(i=0; i<high; i++) {
		for(j=0; j<width; j++) {
			_oled_data_arrary[y+i][x+j] = T_or_H_symbol[is_T_or_H*high+i][j];
		}
	}
    x += NUMBER_WRITH;

    // Delet the old number if the length of "set_data_string" is greater than 4.
    if(x < OLED_MAX_WIDTH) {
        for(i=0; i<high; i++) {
            for(j=0; j<width; j++) {
                _oled_data_arrary[y+i][x+j] = 0x00;
            }
        }
    }
	return;
}

/**
  * @brief Show home page in OLED.
  * @param is_T_or_H: choose temprature or humidity (IS_TEMPERATURE or IS_HUMIDITY)
  * @param is_charge: the device is charging (NO_CHARGE or IS_CHARGE) (developing)
  * @param immediately: show immediately or not (NOT_IMMEDIATELY or IS_IMMEDIATELY)
  * @retval None. 
  */
void oled_show_home_page(unsigned char is_T_or_H, unsigned char is_charge, unsigned char immediately) {
    oled_clear();
    oled_show_T_or_H_title(is_T_or_H);
    oled_refresh_T_or_H(is_T_or_H);
    oled_show_state_panel(immediately);
    return;
}

/**
  * @brief Show slected setting item in OLED.
  * @param selected_item: which item be selected
  * @param immediately: show immediately or not (NOT_IMMEDIATELY or IS_IMMEDIATELY)
  * @retval None.
  */
void oled_show_setting_items(int selected_item, unsigned char immediately) {
    unsigned char start_x, start_y, i;
    start_x = 16; start_y = 2;
    oled_clear();
    for(i=0; i<SETTING_MENU_ITEM_NUMBER; i++) {
        if(selected_item >= 0 && i == selected_item) {
            oled_show_string(0, start_y, "=>");
        } else {
            oled_show_string(0, start_y, "  ");
        }
        oled_show_string(start_x, start_y, (unsigned char *)setting_items[i]);
        start_y += 2; 
        if(start_y >= OLED_MAX_HIGH -1) break;
    }
    oled_show_state_panel(immediately);
    return;
}

/**
  * @brief Show slected setting item in OLED.
  * @param item: which item need to show detial
  * @param is_change: show change item detial tip ("0"-not change, "1"-change)
  * @param immediately: show immediately or not (NOT_IMMEDIATELY or IS_IMMEDIATELY)
  * @retval None.
  */
void oled_show_setting_item_detial(unsigned char item, unsigned char is_change, unsigned char immediately) {
    unsigned char start_x, start_y;
       
    oled_clear();
    start_x = (OLED_MAX_WIDTH - strlen(setting_items[item]) * NORMAL_FONT_WIDTH) / 2; 
    start_y = 2; 
    oled_show_string(start_x, start_y, (unsigned char *)setting_items[item]);

    start_x = (OLED_MAX_WIDTH - strlen(setting_data_form_array[item]->set_data_string) * NORMAL_FONT_WIDTH) / 2; 
    start_y = 5; 
    oled_show_string(start_x, start_y, (unsigned char *)setting_data_form_array[item]->set_data_string);

    start_x -= 8;
    if(is_change) {
        oled_show_char(start_x, start_y, '>');
    } else {
        oled_show_char(start_x, start_y, ' ');
    }
    oled_show_state_panel(immediately);
    return;
}
