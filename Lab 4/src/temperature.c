#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"

#include "temperature.h"


#define D 50 //size of the moving average buffer

typedef struct{
	float data[D];
	int index;
	float sum;
} buffer;

buffer temperature_moving_average;

void update_temperature_buffer(float new_reading){
	int current_index=temperature_moving_average.index % D;
	float old_value=temperature_moving_average.data[current_index];
	temperature_moving_average.sum-=old_value;
	temperature_moving_average.data[current_index]=new_reading;
	temperature_moving_average.sum+=new_reading;
	temperature_moving_average.index++;
}


float get_average_temperature(){
	return temperature_moving_average.sum/D;
}


/**
	Read temperature from the ADC
*/
void store_temperature_in_buffer(){
	ADC_SoftwareStartConv(ADC1);
	
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET){};
		
	float temp1=ADC_GetConversionValue(ADC1);//returns last ADC converted value
	update_temperature_buffer((temp1*3000/4096 - 760)/2.5 + 25); //4096 is when 3V is applied, all 12 bits will be on, ADC returns what percentage of that it sees
}




void initialize_ADC_Temp(){
	ADC_InitTypeDef adc_init_s;  // Structure to initialize definitions of ADC!
	ADC_CommonInitTypeDef adc_common_init_s; // ADC Common Init structure definition!!
	
	ADC_DeInit(); //Deinitializes all ADCs peripherals registers to their default reset values.
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); // Enable Power for bus APB2 connected to peripheral ADC1.
	
	adc_common_init_s.ADC_Mode= ADC_Mode_Independent; // Configures the ADC to operate in independent mode.
	adc_common_init_s.ADC_Prescaler = ADC_Prescaler_Div2; //Select the frequency of the clock to the ADC (divide clock frequency by 2). The clock is common for all the ADCs. Div2 means we count 0 and 1. Div4 means we count 0,1,2,3 on every clock cyle
	adc_common_init_s.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //Configures the Direct memory access mode for multi ADC mode.
	adc_common_init_s.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles; //Configures the Delay between 2 sampling phases.
	
	ADC_CommonInit(&adc_common_init_s); //Initializes the ADCs peripherals according to the specified parameters in the ADC_CommonInitStruct	
	
	adc_init_s.ADC_Resolution = ADC_Resolution_12b; // Configures the ADC resolution to 12 bits.
	adc_init_s.ADC_ScanConvMode= DISABLE; // Specifies whether the conversion is performed in Scan (multichannels) or Single (one channel) mode.
	adc_init_s.ADC_ContinuousConvMode= DISABLE; // Will just convert single value when triggered. If enabled it will start a new conversion as soon as it finishes one
	adc_init_s.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; //Select the external trigger edge and enable the trigger of a regular group. Can set to rising, falling or both.
	adc_init_s.ADC_DataAlign = ADC_DataAlign_Right; // Specifies whether the ADC data  alignment is left or right. If right, MSB in register set to 0 and data is saved starting from LSB.
	adc_init_s.ADC_NbrOfConversion = 1; // Specifies the number of ADC conversions that will be done using the sequencer for regular channel group.
	
	//wake up temperature sensor
	ADC_TempSensorVrefintCmd(ENABLE);
	ADC_Cmd(ADC1, ENABLE); // Enables the specified ADC peripheral (turn it on). 
	ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_480Cycles); // Configures for the selected ADC a regular channel, rank in the sequencer and its sample time. Channel is 16 for temperature sensor. 
	ADC_Init(ADC1, &adc_init_s); // Initializes the ADCx peripheral.
	
}



