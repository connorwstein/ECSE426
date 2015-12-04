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

#define MAX_PATH_LENGTH 500 //20 means 10 (X, Y) points

#define STEP_THRESHOLD -75000
#define RIEMANN_SUM_THRESHOLD 50

#define DEBOUNCE_CONSTANT 100

// Step counting
uint16_t step_count = 0;
uint8_t step_start = 0;

// Orientation (Yaw)
float yaw = 0;
uint16_t state = 0;
int32_t yaw_buffer[2]; // At index 0 we have the new value and index 1 is the old value	

// Path information
int8_t path_data[MAX_PATH_LENGTH]; // Coordinates X, Y, X, Y ...
uint16_t path_index = 2; // Start path index at 2, first point is (0,0)

// Acceleromter and Gyro Outputs
int32_t accelerometer_out[3], gyro_out[3];

// Enables
uint8_t button_has_been_pressed, enable_sensor_interrupt = 0;

// Threading
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
		
			if(enable_sensor_interrupt){
				LSM9DS1_Read_XL(accelerometer_out); 
				LSM9DS1_Read_G(gyro_out);	
				osSignalSet(sensor_reader_thread,1);
			}
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
	enable_sensor_interrupt = 0;
	
	while(1){
		osSignalWait(1,osWaitForever);
		
		delay_counter++;

		if(delay_counter>DEBOUNCE_CONSTANT){
		
			current_key = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
						
			if(current_key == 1 && previous_key == 0){
				// Key change detected (key press)
				button_has_been_pressed += 1;
				
				printf("button pressed \n");
				delay_counter = 0;
				if(button_has_been_pressed > 1){
					printf("transmission thread signal set\n");
					enable_sensor_interrupt = 0;
					osSignalSet(transmission_thread,1);
				}
				else{
					printf("start path recording\n");
					memset(path_data, 0, sizeof(path_data)); 
					path_index = 2;
					yaw = 0;
					state = 0;
					memset(yaw_buffer, 0, sizeof(yaw_buffer));
					enable_sensor_interrupt = 1;
					EXTI_GenerateSWInterrupt(EXTI_Line0);	// start thread execution 
				}
			}
		}
		
	previous_key = current_key;
	}
}




void scale_path(void){
	int8_t min_x = path_data[0], min_y = path_data[1];
	int i;
	for(i = 0; i < MAX_PATH_LENGTH; i++){
		if(i % 2 == 0){
			//X values
			if(path_data[i] < min_x) min_x = path_data[i];
		}
		else{
			//Y values
			if(path_data[i] < min_y) min_y = path_data[i];
		}
	}
	for(i = 0; i < MAX_PATH_LENGTH; i++){
		if(i % 2 == 0){
			//X values
			path_data[i] -= min_x;
		}
		else{
			//Y values
			path_data[i] -= min_y;
		}
	}
}

void transmission(void const *argument){
	
	while(1){
		osSignalWait(1,osWaitForever);
		//scale_path();			
		//cc2500_Transmit_Data((uint8_t*)path_data,path_index);
		
		for(int i = 0; i<200; i++){
			path_data[i] = i;
		}
		
		cc2500_Transmit_Data((uint8_t*)path_data,200);
	}
}


void update_path(uint8_t new_state){
	printf("STATE %d PATH_INDEX %d YAW %f\n", new_state, path_index, yaw/1000);	
	switch(new_state){
		case 0:
			yaw = 0;  // Correct the heading --> remove gyro drift
			path_data[path_index] = path_data[path_index - 2]; // X coordinate stays the same
			path_data[path_index + 1] = path_data[path_index - 1] + 1; // Add 1 to the previous Y coordinate
			break;
		case 1:
			yaw = 45000; 
			path_data[path_index] = path_data[path_index - 2] + 1; // Add 1 to previous X coordinate
			path_data[path_index + 1] = path_data[path_index - 1] + 1; // Add 1 to the previous Y coordin
			break;
		case 2:
			yaw = 90000;
			path_data[path_index] = path_data[path_index - 2] + 1; // Add 1 to the previous X coordinate
			path_data[path_index + 1] = path_data[path_index - 1]; // Y coordinate stays the same
			break;
		case 3:
			yaw = 135000;
			path_data[path_index] = path_data[path_index - 2] + 1; // Add 1 to the previous X coordinate
			path_data[path_index + 1] = path_data[path_index - 1] - 1; // Subtract 1 from the previous Y coordinate
			break;
		case 4:
			yaw = 180000;
			path_data[path_index] = path_data[path_index - 2]; // X coordinate stays the same
			path_data[path_index + 1] = path_data[path_index - 1] - 1; // Subtract 1 from the previous Y coordin
			break;
		case 5:
			yaw = 225000;
			path_data[path_index] = path_data[path_index - 2] - 1; // Subtract 1 from the X coordinate 
			path_data[path_index + 1] = path_data[path_index - 1] - 1; // Subtract 1 from the Y coordinate
			break;
		case 6:
			yaw = 270000;
			path_data[path_index] = path_data[path_index - 2] - 1; // Subtract 1 from the X coordinate
			path_data[path_index + 1] = path_data[path_index - 1]; // Y coordinate stays the same
			break;
		case 7:
			yaw = 315000;
			path_data[path_index] = path_data[path_index - 2] - 1; // Subtract 1 from the X coordinate
			path_data[path_index + 1] = path_data[path_index - 1] + 1; // Add 1 to the previous y coordin
			break;
	}
	path_index+=2; //on to the next X,Y point
	
	
}

uint8_t get_state(void){
		float yaw_degrees = yaw/1000;
		if((yaw_degrees < 22.5 && yaw_degrees >= 0) || yaw_degrees > 337.5){
			return 0;
		}
		if(yaw_degrees <= 67.5 && yaw_degrees > 22.5){
			return 1;
		}
		if(yaw_degrees <= 112.5 && yaw_degrees > 67.5){
			return 2;
		}
		if(yaw_degrees <= 157.5 && yaw_degrees > 112.5){
			return 3;
		}
		if(yaw_degrees <= 202.5 && yaw_degrees > 157.5){
			return 4;
		}
		if(yaw_degrees <= 247.5 && yaw_degrees > 202.5){
			return 5;
		}
		if(yaw_degrees <= 292.5 && yaw_degrees > 247.5){
			return 6;
		}
		if(yaw_degrees <= 337.5 && yaw_degrees > 292.5){
			return 7;
		}
}

void update_yaw(int32_t new_reading){
	yaw_buffer[0] = yaw_buffer[1]; //copy previous value into old spot
	yaw_buffer[1] = new_reading; //put new value in
	//Add new riemman sum to yaw value
	float riemann_sum = (yaw_buffer[0] + yaw_buffer[1])/2*0.01;
	if(riemann_sum > RIEMANN_SUM_THRESHOLD || riemann_sum < -RIEMANN_SUM_THRESHOLD){
		yaw+=riemann_sum;
		if(yaw < 0){
			yaw = 360000 - abs(yaw);
		}
		if(yaw > 360000){
			yaw = yaw - 360000;
		}
	}
}

void sensor_reader(void const *argument){

	while(1){
		osSignalWait(1,osWaitForever);
		update_moving_average_xl(accelerometer_out[0], accelerometer_out[1], accelerometer_out[2]); // Update the global structures in accelerometer.c
		update_moving_average_g(gyro_out[0], gyro_out[1], gyro_out[2]);
		//printf("Moving average ACC %d %d %d GYRO %d %d %d\n", get_average_Ax1(), get_average_Ay1(), get_average_Az1(),get_average_Gx1(), get_average_Gy1(), get_average_Gz1());
		if(get_average_Gz1() < STEP_THRESHOLD && !step_start){
			step_start = 1;
			continue;
		}
		if(step_start && get_average_Gz1() > STEP_THRESHOLD){
			step_start = 0;
			step_count++;
			printf("STEP! Count is now %d\n", step_count);
			update_path(get_state());
			printf("Points: ");
			int i;
			for(i = 0; i < MAX_PATH_LENGTH; i+=2){
				printf("(%d, %d), ", path_data[i], path_data[i+1]);
			}
			printf("\n");
			if(step_count == 6){
				scale_path();
				int i;
				for(i = 0; i < MAX_PATH_LENGTH; i+=2){
					printf("(%d, %d), ", path_data[i], path_data[i+1]);
				}
				printf("\n");
			}
		}
		update_yaw(get_average_Gx1());
		
	}
}
/**
	@brief Initializes the interrupts and interrupt handler for the accelerometer 
*/
void init_EXTI0_interrupts(void){
			
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
		printf("Interrupt init complete\n");
	
}



/*
 * main: initialize and start the system
 */
int main (void) {

  osKernelInitialize();                    // initialize CMSIS-RTOS
	sensor_reader_thread = osThreadCreate(osThread(sensor_reader),NULL);
	if(sensor_reader_thread == NULL) printf("Error creating sensor thread\n"); 
	init_sensor();
	init_EXTI0_interrupts();
	
	cc2500_start_up_procedure();

	uint8_t test = 0;
	
	cc2500_Read_Status_Register(&test,CC2500_PARTNUM);
	
	printf("after reset and initialization\n");
	printf("partnum %d\n", test);

	init_gpio(GPIO_Pin_0, RCC_AHB1Periph_GPIOA, GPIOA, 1, 0);
	
	//this is just for testing transmit
	init_TIM(TIM3,TIM3_IRQn,100,RCC_APB1Periph_TIM3);
	
	transmission_thread = osThreadCreate(osThread(transmission),NULL);
	button_detector_thread = osThreadCreate(osThread(button_detector),NULL);
	
	osKernelStart(); 


}


