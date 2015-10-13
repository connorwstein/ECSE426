#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "system_init.h"

#define D 50 //size of the moving average buffer
#define E 60 //LED alarm threshold
#define LEDPeriod 10 //number of ticks each LED should be turned on for
#define TEST 0 //run update_buffer_test() if equal to 1
typedef struct{
	float data[D];
	int index;
	float sum;
} buffer;

uint32_t ticks;

/**
	Every tick move the motor to an angle to a that represents the temperature.
	The motor angle is set by adjusting the duty cycle of PB1 (a pin connected to the motor), controlled by GPIOB
	The duty cycle is set using and empirically determined delay.
*/
void move_motor(float temp){
	int delay=((103000-26000)*(temp-20)/40)+26000; //these values seem to work. temp-20 is measuring the temperature from 20, since 20 is supposed to be at angle 5. 60 is at angle 175
	//int delay = 27000;
	GPIO_SetBits(GPIOB, GPIO_Pin_1); //Set motor pin high
	 
	for(int i=0;i<=delay;i++); //Delay for a time according to the temperature reading
	//Delay of 900us is 0 degrees
	//Delay of 2100us is 180 degrees
	//move_motor gets called every 20ms
	
	
	GPIO_ResetBits(GPIOB, GPIO_Pin_1); //Set motor pin low
}
/**
	Add a new temperature reading to the moving average buffer
*/
void update_buffer(float new_reading, buffer* moving_average){
	int current_index=moving_average->index % D;
	float old_value=moving_average->data[current_index];
	moving_average->sum-=old_value;
	moving_average->data[current_index]=new_reading;
	moving_average->sum+=new_reading;
	moving_average->index++;
}

/**
	This performs a simple test of update_buffer()
*/
void update_buffer_test(){
	buffer moving_average;
	memset(&moving_average, 0, sizeof(moving_average));
	int listSize = D*2;
	
	for(int i=0;i<listSize;i++){
		update_buffer((float)i,&moving_average);
		
		for(int j=0;j<D;j++){
			printf("%f ", moving_average.data[j]);
		}
		
		printf("%f %f \n", moving_average.sum, moving_average.sum/D);	
	}
}

/**
	If the motor is above a certain temperature, the LEDs will turn on in circular fashion.
*/
void LED_alarm(float temp, int raw_counter){
	if (temp >= E){ // If temperature is above the emergency value
		int counter = raw_counter % (LEDPeriod*4); //turn on an led for 10 ticks. 4 LEDs means the counter should reset after 4*10 = 40.	
	
		if (counter<LEDPeriod){
			GPIO_ResetBits(GPIOD, GPIO_Pin_15);
			GPIO_SetBits(GPIOD, GPIO_Pin_12);
		}
		else if(counter<LEDPeriod*2){
			GPIO_ResetBits(GPIOD, GPIO_Pin_12);
			GPIO_SetBits(GPIOD, GPIO_Pin_13);
		}
		else if(counter<LEDPeriod*3){
			GPIO_ResetBits(GPIOD, GPIO_Pin_13);
			GPIO_SetBits(GPIOD, GPIO_Pin_14);
		}
		else{
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
	Read temperature from the ADC
	@return float which is the temperature in degrees
*/
float get_temp(){
	ADC_SoftwareStartConv(ADC1);
	
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET){};
		
	float temp1=ADC_GetConversionValue(ADC1);//returns last ADC converted value
	return (temp1*3000/4096 - 760)/2.5 + 25; //4096 is when 3V is applied, all 12 bits will be on, ADC returns what percentage of that it sees
}

/**
	This interrupt handler gets called every 20 ms.
*/
void SysTick_Handler(){
	ticks=1;
}

int main(){
	#if TEST
	update_buffer_test();
	#endif
	ticks=0;
	initialize_ADC_Temp();
	initialize_GPIO();	
	SysTick_Config(SystemCoreClock/50); //tick every 20 ms //Input value must be less than 24 bits
	buffer moving_average;
	memset(&moving_average, 0, sizeof(moving_average));
	int counter = 0; //Every LEDPeriod ticks, the LED that is ON will change, use this counter to make that happen
	float temp;
	while(1){
		if(ticks){
			counter++;
			temp = get_temp();
			update_buffer(temp,&moving_average); //Add new temperature reading to the circular buffer every tick
			//if(counter%50==0){
				printf("Temperature reading: %f, %f, %d\n", temp, moving_average.sum/D, moving_average.index);
			//}
			LED_alarm(moving_average.sum/D,counter);
			move_motor(moving_average.sum/D);
			ticks=0;
		}
	}
	return 0;
}