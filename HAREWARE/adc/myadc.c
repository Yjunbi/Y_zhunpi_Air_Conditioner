#include "myadc.h"

/**
  * @brief  Initialize adc gpio.
  * @retval None
  */
void myadc_init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    ADC_InitTypeDef ADC_InitStruct;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
	GPIO_InitStruct.GPIO_Pin = MYADC_GPIO_PinX; 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;   // Analog input
	GPIO_Init(MYADC_GPIOX, &GPIO_InitStruct);
	
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    ADC_DeInit(ADC1);

    ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStruct.ADC_ScanConvMode = DISABLE;
    ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStruct.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStruct);
	
    ADC_Cmd(ADC1, ENABLE);

    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1));

    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1));
    
    return;
}

/**
  * @brief  Get adc measure value once.
  * @retval return adc value
  */
uint16_t myadc_get_value(void) {
    ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_239Cycles5);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    return ADC_GetConversionValue(ADC1);
}

/**
  * @brief  Get adc measure value.
  * @param  times: measure n times and culculate the average
  * @retval return adc measured average value
  */
uint16_t myadc_get_average(uint8_t times) {
    uint32_t temp_value = 0;
    uint8_t i = 0;
    for(i=0; i<times; i++) {
        temp_value += myadc_get_value();
        delay_ms(5);
    }
    return temp_value/times;
}


