#include "stm32f10x.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "oled.h"
#include "key.h"
#include "ath20_model.h"
#include "timer.h"
#include "myinterrupt.h"
#include "myadc.h"
#include "human.h"

int main(void) {	
    delay_init();
    delay_ms(1000);
	led_init();	
	oled_init(); 
	oled_clear();
    key_gpio_init();
    infrared_init();
    human_gpio_init();
    timer2_init();
    myadc_init();
    AHT20_Init();
    oled_show_home_page(IS_TEMPERATURE, IS_CHARGE, IS_IMMEDIATELY);
	while(1){
        key_home_menu_scan();
    }
}

/* uart test */
// int main(void) {    
//     uint16_t temp;
//     float voltage;
//     delay_init();
//     uart_init(9600);
//     myadc_init();
//     while(1) {
//         delay_ms(500);
//         temp = myadc_get_average(5);
//         voltage = temp * (3.3/ 4095) * 20 * 2.5;
//         printf("test number: %f\t\n", voltage);
//     }
// }

/* Test timer.*/
// int main(void) {
//     led_init();
//     timer2_init();
//     while(1){}
// }

/* Test ATH20 */
// int main(void){	
//     uint32_t CT_data[2];
// 	volatile int c1,t1;
//     delay_init();
//     uart_init(9600);
// 	led_init();	
//     AHT20_Clock_Init();
//     AHT20_Init();
//     led_turn_on();
//     Delay_1ms(500);
//     if((AHT20_Read_Status()&0x18)!=0x18) {
//         AHT20_Start_Init(); //重新初始化寄存器
//         Delay_1ms(10);
//     }
// 	while(1){
//         AHT20_Read_CTdata(CT_data);       //不经过CRC校验，直接读取AHT20的温度和湿度数据    推荐每隔大于1S读一次
//         delay_ms(1000);
//         //AHT20_Read_CTdata_crc(CT_data);  //crc校验后，读取AHT20的温度和湿度数据 
//        
//         c1 = CT_data[0]*100*10/1024/1024;  //计算得到湿度值c1（放大了10倍）
//         t1 = CT_data[1]*200*10/1024/1024-500;//计算得到温度值t1（放大了10倍）
//         ////下一步客户处理显示数据，
//         printf("c1:%d, t1:%d   ", c1, t1);
//     }
//
// }

// /* Send data. */
// int main(void){	
//     delay_init();
// 	GPIO_InitTypeDef GPIO_InitStruct;
	
// 	// 时钟使能
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
// 	// 初始化GPIOC
// 	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
// 	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
// 	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
// 	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
// 	GPIO_ResetBits(GPIOB, GPIO_Pin_0);
// 	// GPIO_SetBits(GPIOB, GPIO_Pin_0);
    
// 	while(1){
// 		// key_handle_first_menu();
//     }
// }

/* Receiver */
// int main(void){	
//     uint8_t temp;
//     delay_init();
//     led_init();
// 	GPIO_InitTypeDef GPIO_InitStruct;
	
// 	// 时钟使能
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
// 	// 初始化GPIOC
// 	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
// 	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
// 	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
// 	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
// 	GPIO_ResetBits(GPIOB, GPIO_Pin_8);
// 	// GPIO_SetBits(GPIOB, GPIO_Pin_0);
    
// 	while(1){
// 		// key_handle_first_menu();
//         delay_ms(500);
//         temp = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8);
//         if(temp) {
//             led_turn_on();
//         } else if(!temp) {
//             led_turn_off();
//         }
//     }
// }



