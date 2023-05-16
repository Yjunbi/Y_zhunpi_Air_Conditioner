#include "myinterrupt.h"

uint16_t timer2_counter = 0;
uint16_t sync_counter = 0;
uint16_t sleep_counter = 0;

/**
  * @brief  Renew the sleep time.
  * @retval None
  */
void renew_sleep_time(void) {
    sleep_counter = 0;
    return;
}
 
void TIM2_IRQHandler(void)  {
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) {
        timer2_counter += 1;
        
        if(get_oled_state()) sleep_counter += 1;
        if(!get_infrared_send_flag()) sync_counter += 1;
        if(sleep_counter >= (get_device_sleep_time()*2)) {
            sleep_counter = 0;
            oled_turn_off();
        } 

        if(!(timer2_counter % 2)) { 
            timer2_counter = 0;
            first_menu_info_change_flag();
        }
		
        if(sync_counter >= infrared_get_send_frequece()) {
            sync_counter = 0;
            set_infrared_send_flag(ABLE_SEND);
        }
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update); 
	}
}


