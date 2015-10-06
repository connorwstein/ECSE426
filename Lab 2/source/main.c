#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "system_init.h"

#define D 50
#define E 45

typedef struct{
	float data[D];
	int index;
	float sum;
} buffer;

uint32_t ticks;

void move_motor(float temp){
	int delay=((2300-900)*temp/100)+900; //values given in tutorial, we need to change them to suit our motor
	GPIO_SetBits(GPIOB, GPIO_Pin_1);
	
	for(int i=0;i<=delay;i++);
	GPIO_ResetBits(GPIOB, GPIO_Pin_1);

}


	
	

void update_buffer(float new_reading, buffer* moving_average){
	//float moving_average_sum=moving_average->sum;
	//int moving_average_index=moving_average->index;
	int current_index=moving_average->index % D;
	float old_value=moving_average->data[current_index];
	moving_average->sum-=old_value;
	moving_average->data[current_index]=new_reading;
	moving_average->sum+=new_reading;
	moving_average->index++;
}

void update_buffer_test()
{
	buffer moving_average;
	memset(&moving_average, 0, sizeof(moving_average));
	
	int listSize = D*2;
	
	for(int i=0;i<listSize;i++)
	{
		update_buffer((float)i,&moving_average);
		
		for(int j=0;i<D;i++)
		{
			printf("%f ", moving_average.data[j]);
		}
		printf("\n");
		
	}
}

void LED_alarm(float temp, int raw_counter)
{
	if (temp >= E){ // If temperature is above the emergency value
		
	int counter = raw_counter % 40; //turn on an led for 10 ticks. 4 LEDs means the counter should reset after 4*10 = 40.
		
	 // First time
	 if (counter<10)
	 {
			GPIO_ResetBits(GPIOD, GPIO_Pin_15);
			GPIO_SetBits(GPIOD, GPIO_Pin_12);
	 }
		 // After 120 msec
	 else if(counter<20)
	 {
			GPIO_ResetBits(GPIOD, GPIO_Pin_12);
			GPIO_SetBits(GPIOD, GPIO_Pin_13);
	 }
	 
	 else if(counter<30)
	 {
			GPIO_ResetBits(GPIOD, GPIO_Pin_13);
			GPIO_SetBits(GPIOD, GPIO_Pin_14);
	 }
	 	 
	 else
	 {
			GPIO_ResetBits(GPIOD, GPIO_Pin_14);
			GPIO_SetBits(GPIOD, GPIO_Pin_15);
	 }
	}
	else{
		GPIO_ResetBits(GPIOD, GPIO_Pin_12);
		GPIO_ResetBits(GPIOD, GPIO_Pin_13);
		GPIO_ResetBits(GPIOD, GPIO_Pin_14);
		GPIO_ResetBits(GPIOD, GPIO_Pin_15);
	}
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
	int counter = 0;
	while(1){
		if(ticks){
			//motor
			//temp reading
			//led control and alarm
			counter++;
			update_buffer(get_temp(),&moving_average);
			printf("Temperature reading: %f, %f, %d\n", get_temp(), moving_average.sum/D, moving_average.index);
			LED_alarm(moving_average.sum/D,counter);
			move_motor(moving_average.sum/D);
			ticks=0;
		}
	}
	return 0;
}