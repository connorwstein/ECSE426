#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "system_init.h"

#define D 10

typedef struct{
	float data[D];
	int index;
	float sum;
} buffer;

uint32_t ticks;

void move_motor(float temp){
	//int delay=((Y-X)/(100/temp))+X;
	GPIO_SetBits(GPIOB, GPIO_Pin_1);
	
	//for(int i=0;i<=delay;i++);
	GPIO_ResetBits(GPIOB, GPIO_Pin_1);

}

void prinf_buffer(){
		
	
}

void update_buffer(float new_reading, buffer* moving_average){
	float moving_average_sum=moving_average->sum;
	int moving_average_index=moving_average->index;
	int current_index=moving_average_index % D;
	float old_value=moving_average->data[current_index];
	moving_average->sum=moving_average_sum-old_value;
	moving_average->data[current_index]=new_reading;
	moving_average->sum=moving_average->sum+new_reading;
	moving_average->index=moving_average_index+1;
}


/**
	@return float which is the temperature in degrees
*/
float get_temp(){
	ADC_SoftwareStartConv(ADC1);
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET){};
	float temp1=ADC_GetConversionValue(ADC1);//returns last ADC converted value
	return (temp1*3000/4096 - 760)/2.5 + 25; //4096 is when 3V is applied, all 12 bits will be on, ADC returns what percentage of that it sees
}


void SysTick_Handler(){
	ticks=1;
}

int main(){
	ticks=0;
	initialize_ADC_Temp();
	initialize_GPIO();	
	SysTick_Config(SystemCoreClock/50); //tick every 20 ms //Input value must be less than 24 bits
	buffer moving_average;
	memset(&moving_average, 0, sizeof(moving_average));
	
	while(1){
		if(ticks){
			//motor
			//temp reading
			//led control and alarm
			update_buffer(get_temp(),&moving_average);
			printf("Temperature reading: %f, %f, %d\n", get_temp(), moving_average.sum, moving_average.index);
			ticks=0;
		}
	}
	return 0;
}