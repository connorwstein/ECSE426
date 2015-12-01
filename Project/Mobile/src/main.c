/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include <stdio.h>
#include <stdlib.h>

//#include "lsm9ds1_test.h"
#include "lsm9ds1.h"
#include "sensor.h"
#include "cc2500.h"
#include "Timers_and_interrupts.h"

#define MAX_PATH_LENGTH 100
#define SIZE_OF_TEST_DATA 100
#define MAX_INT16 32767

int32_t accelerometer_out[3], gyro_out[3];
int16_t path_data[MAX_PATH_LENGTH];
uint16_t actual_path_length;

uint8_t test_data[SIZE_OF_TEST_DATA];
uint8_t button_has_been_pressed;

osThreadId sensor_reader_thread;
osThreadId transmission_thread;
osThreadId button_detector_thread;

void transmission(void const *argument);
void button_detector(void const *argument);
void sensor_reader(void const *argument);

osThreadDef(transmission, osPriorityNormal, 1, 400);
osThreadDef(button_detector, osPriorityNormal, 1, 400);
osThreadDef(sensor_reader, osPriorityNormal, 1, 1000);

/**
	@brief Handler for when data is available from the accelerometer
*/
void EXTI0_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line0) != RESET){
			EXTI_ClearITPendingBit(EXTI_Line0); // Clear the interrupt pending bit
			LSM9DS1_Read_XL(accelerometer_out); 
			LSM9DS1_Read_G(gyro_out);	
			osSignalSet(sensor_reader_thread,1);
	}
}


void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		osSignalSet(button_detector_thread,1);
	}
}


void button_detector(void const *argument){

	uint8_t current_key = 0; 
	uint8_t previous_key = 0;
	uint32_t delay_counter = 0;
	
	button_has_been_pressed = 0;
	
	while(1){
		osSignalWait(1,osWaitForever);
		
		delay_counter++;
		
		if(delay_counter>10){
		
			current_key = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);

			if(current_key != previous_key){
				// Key change detected (key press)
				previous_key = current_key;
				button_has_been_pressed = 1;
			}
			else{
				// No key press
				button_has_been_pressed = 0;
			}
			
			if(button_has_been_pressed == 1 && current_key != 0){
				printf("button pressed \n");
				delay_counter = 0;
				osSignalSet(transmission_thread,1);
			}
		}
	}
}



void sixteen_to_eight(uint8_t* output_array, uint16_t* input_array,uint16_t size_of_array){
	
	uint16_t current_number;
	
	for(int i=0;i<size_of_array;i++){
		current_number = input_array[i];
		output_array[i*2] = (current_number >> 8);
		output_array[(i*2)+1] = (current_number & 0xff);
		printf("path_data_eight_bit[%d] = %d\n", i*2, output_array[i*2]);
		printf("path_data_eight_bit[%d] = %d\n", (i*2)+1, output_array[(i*2)+1]);
	}
	
}


int16_t minimum_of_array(int16_t* input_array,uint16_t size_of_array){
	int16_t min_number = MAX_INT16;
	int16_t current_number;
	
	for(int i = 0; i<size_of_array; i++){
		current_number = input_array[i];
		if(current_number < min_number){
			min_number = current_number;
		}
	}
	
	return min_number;
	
}



void convert_to_unsigned(uint16_t* output_array, int16_t* input_array,uint16_t size_of_array){
	int16_t min_number = minimum_of_array(input_array,size_of_array);
	
	printf("min_number = %d\n", min_number);
	
	for(int i = 0; i<size_of_array; i++){
		output_array[i] = input_array[i] - min_number;
		printf("path_data_unsigned[%d] = %d\n", i, output_array[i]);
	}
}

void transmission(void const *argument){
	
	while(1){
		osSignalWait(1,osWaitForever);
			
		uint16_t path_data_unsigned[actual_path_length];
		uint8_t path_data_eight_bit[actual_path_length*2];
		
		convert_to_unsigned(path_data_unsigned,path_data,actual_path_length);
		sixteen_to_eight(path_data_eight_bit,path_data_unsigned,actual_path_length);
		
//		for(int i=0;i<SIZE_OF_TEST_DATA;i++){
//			test_data[i] = i;
//		}
		
		cc2500_Transmit_Data(path_data_eight_bit,actual_path_length*2);
	}
}

void sensor_reader(void const *argument){
	
	while(1){
		osSignalWait(1,osWaitForever);
		update_moving_average_xl(accelerometer_out[0], accelerometer_out[1], accelerometer_out[2]); // Update the global structures in accelerometer.c
		update_moving_average_g(gyro_out[0], gyro_out[1], gyro_out[2]);
		printf("Moving average ACC %d %d %d GYRO %d %d %d\n", get_average_Ax1(), get_average_Ay1(), get_average_Az1(),get_average_Gx1(), get_average_Gy1(), get_average_Gz1());
	}
}
/**
	@brief Initializes the interrupts and interrupt handler for the accelerometer 
*/
void init_EXT10_interrupts(void){
			
		GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); // Enable 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); // Enable Syscfg clock
    
    // Set GPIO pin as input for receiving acceleromter data 
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOE, &GPIO_InitStruct);
    
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource0); // Using PE0 for EXTI_Line0     
    EXTI_InitStruct.EXTI_Line = EXTI_Line0; // PE0 is connected to EXTI_Line0 
    EXTI_InitStruct.EXTI_LineCmd = ENABLE; // Enable interrupt
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt; // Set to interrupt mode
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising; //Triggers on rising edge
    EXTI_Init(&EXTI_InitStruct); // Initialize the external interrupt
 
    // Add IRQ vector to NVIC 
    NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn; // PE0 is connected to EXTI_Line0, which has EXTI0_IRQn vector 
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE; // Enable interrupt 
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00; // Set priority
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00; // Set sub priority
    NVIC_Init(&NVIC_InitStruct); // Add to the NVIC
		printf("interrupt init complete\n");
		
		
}

/*
 * main: initialize and start the system
 */
int main (void) {

  osKernelInitialize();                    // initialize CMSIS-RTOS
	sensor_reader_thread = osThreadCreate(osThread(sensor_reader),NULL);
	if(sensor_reader_thread == NULL) printf("Error creating sensor thread\n");
	osKernelStart(); 
	init_sensor();
	init_EXT10_interrupts();
	EXTI_GenerateSWInterrupt(EXTI_Line0);	// start thread execution 
	
	
	memset(&test_data, 0, sizeof(test_data));
	
	cc2500_start_up_procedure();

	uint8_t test = 0;
	
	cc2500_Read_Status_Register(&test,CC2500_PARTNUM);
	
	printf("after reset and initialization\n");
	printf("partnum %d\n", test);

//	init_gpio(GPIO_Pin_0, RCC_AHB1Periph_GPIOA, GPIOA, 1, 0);
//	
//	//this is just for testing transmit
//	init_TIM(TIM3,TIM3_IRQn,100,RCC_APB1Periph_TIM3);
//	
//	actual_path_length = 5;
//	
//	for(int i=0;i<actual_path_length;i++){
//		path_data[i] = (i-1)*1000;
//	}
//	
//	
//	transmission_thread = osThreadCreate(osThread(transmission),NULL);
//	button_detector_thread = osThreadCreate(osThread(button_detector),NULL);
//	
//	osKernelStart(); 


}


