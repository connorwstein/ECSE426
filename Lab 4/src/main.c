/**
  ******************************************************************************
  * @file    main.c
  * @author  Connor Stein (connor.stein@mail.mcgill.ca) & Kevin Musgrave (takeshi.musgrave@mail.mcgill.ca)
  * @version V1.0.0
  * @date    10-28-2015
  * @brief   Main implementation of ECSE 426 Lab 3 McGill University, guessing game of angle of board
	using accelerometer, 7 segment display and 4x4 keypad
  ****************************************************************************** 
*/

#define osObjectsPublic
#include "osObjects.h" 
#include "stm32f4xx.h"                  
#include "stm32f4xx_conf.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#include <stdio.h>

//#include "accelerometer.h"
//#include "7_segment.h"
//#include "keypad.h"
//#include "temperature.h"
#include "lsm9ds1.h"

//#define CHECK_BIT(var,pos) (((var) & (1<<(pos)))>>(pos))
//#define SHOW_ROLL 1
//#define SHOW_PITCH 2
//#define ENTER_KEY 15
//#define TEMPERATURE_THRESHOLD 50
//#define ALARM_COUNTER_MAX 200
//#define ANGLE_THRESHOLD 0.1

//uint8_t display_mode;
//int32_t accelerometer_out[3];
//float angle_to_draw;
//float roll;
//float pitch;
//float temperature;


//void temperature_reader(void const *argument);
//void display_refresher(void const *argument);
//void accelerometer_reader(void const *argument);
//void keypad_detector(void const *argument);


//// ID for thread
//osThreadId temperature_reader_thread;
//osThreadId accelerometer_reader_thread;
//osThreadId display_refresher_thread;
//osThreadId keypad_detector_thread;


//osMutexId temperature_mutex;
//osMutexId accelerometer_mutex;
//osMutexId keypad_mutex;

//osThreadDef(temperature_reader, osPriorityNormal, 1, 400);
//osThreadDef(display_refresher, osPriorityNormal, 1, 400);
//osThreadDef(accelerometer_reader, osPriorityNormal, 1, 1000);
//osThreadDef(keypad_detector, osPriorityNormal, 1, 400);

//osMutexDef(temperature_mutex);
//osMutexDef(accelerometer_mutex);
//osMutexDef(keypad_mutex);

///**
//	@brief Initializes the interrupts and interrupt handler for the accelerometer 
//*/
//void init_interrupts(void){
//		GPIO_InitTypeDef GPIO_InitStruct;
//    EXTI_InitTypeDef EXTI_InitStruct;
//    NVIC_InitTypeDef NVIC_InitStruct;
//    
//    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); // Enable 
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); // Enable Syscfg clock
//    
//    // Set GPIO pin as input for receiving acceleromter data 
//    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
//    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
//    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
//    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
//    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
//    GPIO_Init(GPIOE, &GPIO_InitStruct);
//    
//    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource0); // Using PE0 for EXTI_Line0     
//    EXTI_InitStruct.EXTI_Line = EXTI_Line0; // PE0 is connected to EXTI_Line0 
//    EXTI_InitStruct.EXTI_LineCmd = ENABLE; // Enable interrupt
//    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt; // Set to interrupt mode
//    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising; //Triggers on rising edge
//    EXTI_Init(&EXTI_InitStruct); // Initialize the external interrupt
// 
//    // Add IRQ vector to NVIC 
//    NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn; // PE0 is connected to EXTI_Line0, which has EXTI0_IRQn vector 
//    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE; // Enable interrupt 
//    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00; // Set priority
//    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00; // Set sub priority
//    NVIC_Init(&NVIC_InitStruct); // Add to the NVIC
//}

///**
//	@brief Handler for when data is available from the accelerometer
//*/
//void EXTI0_IRQHandler(void){
//	if(EXTI_GetITStatus(EXTI_Line0) != RESET){
//			EXTI_ClearITPendingBit(EXTI_Line0); // Clear the interrupt pending bit
//			LIS302DL_ReadACC(accelerometer_out); // Read the accelerometers data
//			osSignalSet(accelerometer_reader_thread,1);
//	}
//}






////for the keypad detector
//void init_TIM5(void){
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);	//Enable the peripheral clock

//	TIM_TimeBaseInitTypeDef init;
//	// Desired rate = ClockFrequency /(prescaler * period)
//	// Clock frequency is 168MHz Period and prescaler are in the range [0x0000, 0xFFFF]
//	// For 100Hz interrupt rate, let Prescaler be 10000 and the period be 168
//	init.TIM_Prescaler = 10000;
//	init.TIM_CounterMode = TIM_CounterMode_Up;
//	init.TIM_Period =  168/2; 
//	init.TIM_ClockDivision = TIM_CKD_DIV1; 
//	TIM_TimeBaseInit(TIM5, &init); 	//Initialize Timer 5
//	
//	// Add to interrupt routine to the NVIC
//	NVIC_InitTypeDef nvic;	
//	nvic.NVIC_IRQChannel = TIM5_IRQn; 
//	nvic.NVIC_IRQChannelCmd = ENABLE; 
//	nvic.NVIC_IRQChannelPreemptionPriority = 0x00; 
//	nvic.NVIC_IRQChannelSubPriority = 0x00; 
//	NVIC_Init(&nvic); 
//	
//	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE); //Link interrupt and Timer
//	TIM_Cmd(TIM5, ENABLE); 	//Start Timer
//	
//}

///**
//	@brief Handler for the timer interrupt
//*/
//void TIM5_IRQHandler(void)
//{
//	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)
//	{
//		TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
//		osSignalSet(keypad_detector_thread,1);
//	}
//}
////for the ADC
//void init_TIM4(void){
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//Enable the peripheral clock

//	TIM_TimeBaseInitTypeDef init;
//	// Desired rate = ClockFrequency /(prescaler * period)
//	// Clock frequency is 168MHz Period and prescaler are in the range [0x0000, 0xFFFF]
//	// For 50Hz interrupt rate, let Prescaler be 20000 and the period be 168
//	init.TIM_Prescaler = 20000;
//	init.TIM_CounterMode = TIM_CounterMode_Up;
//	init.TIM_Period =  168/2; 
//	init.TIM_ClockDivision = TIM_CKD_DIV1; 
//	TIM_TimeBaseInit(TIM4, &init); 	//Initialize Timer 4
//	
//	// Add to interrupt routine to the NVIC
//	NVIC_InitTypeDef nvic;	
//	nvic.NVIC_IRQChannel = TIM4_IRQn; 
//	nvic.NVIC_IRQChannelCmd = ENABLE; 
//	nvic.NVIC_IRQChannelPreemptionPriority = 0x00; 
//	nvic.NVIC_IRQChannelSubPriority = 0x00; 
//	NVIC_Init(&nvic); 
//	
//	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); //Link interrupt and Timer
//	TIM_Cmd(TIM4, ENABLE); 	//Start Timer
//	
//}

///**
//	@brief Handler for the timer interrupt
//*/
//void TIM4_IRQHandler(void)
//{
//	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET)
//	{
//		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
//		osSignalSet(temperature_reader_thread,1);
//	}
//}

///**
//	@brief Initializes the interrupt timer used for both refreshing the 7 segment display
//	and reading input from the keypad
//*/
//void init_TIM3(void){
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//Enable the peripheral clock

//	TIM_TimeBaseInitTypeDef init;
//	// Desired rate = ClockFrequency /(prescaler * period)
//	// Clock frequency is 168MHz Period and prescaler are in the range [0x0000, 0xFFFF]
//	// For 480Hz interrupt rate (100Hz was too slow), let Prescaler be 2083 and the period be 168
//	init.TIM_Prescaler = 2083;
//	init.TIM_CounterMode = TIM_CounterMode_Up;
//	init.TIM_Period = 168/2; 
//	init.TIM_ClockDivision = TIM_CKD_DIV1; 
//	TIM_TimeBaseInit(TIM3, &init); 	//Initialize Timer 3
//	
//	// Add to interrupt routine to the NVIC
//	NVIC_InitTypeDef nvic;	
//	nvic.NVIC_IRQChannel = TIM3_IRQn; 
//	nvic.NVIC_IRQChannelCmd = ENABLE; 
//	nvic.NVIC_IRQChannelPreemptionPriority = 0x00; 
//	nvic.NVIC_IRQChannelSubPriority = 0x00; 
//	NVIC_Init(&nvic); 
//	
//	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //Link interrupt and Timer
//	TIM_Cmd(TIM3, ENABLE); 	//Start Timer
//	
//}
///**
//	@brief Handler for the timer interrupt, refreshes the 7 segment display and checks for pressed keys
//*/
//void TIM3_IRQHandler(void)
//{
//	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
//	{
//		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
//		//Refresh 7 segment
//		osSignalSet(display_refresher_thread,1);
//	}
//}



//void temperature_reader(void const *argument){
//	temperature_mutex = osMutexCreate(osMutex(temperature_mutex));
//	while(1){
//		osSignalWait(1,osWaitForever);
//		store_temperature_in_buffer();
//		osMutexWait(temperature_mutex,osWaitForever);
//		temperature = get_average_temperature();
//		osMutexRelease(temperature_mutex);
//	}
//}

//void accelerometer_reader(void const *argument){
//	accelerometer_mutex = osMutexCreate(osMutex(accelerometer_mutex));
//	while(1){
//		osSignalWait(1,osWaitForever);
//		update_moving_average(accelerometer_out[0], accelerometer_out[1], accelerometer_out[2]); // Update the global structures in accelerometer.c
//		osMutexWait(accelerometer_mutex,osWaitForever);
//		roll = fabs(calculate_roll_angle());
//		pitch = fabs(calculate_pitch_angle());
//		osMutexRelease(accelerometer_mutex);
//	}
//}


//void keypad_detector(void const *argument){
//	keypad_mutex = osMutexCreate(osMutex(keypad_mutex));
//	int8_t current_key = NO_KEY_PRESSED; 
//	int8_t previous_key = NO_KEY_PRESSED;
//	uint8_t digit_has_been_entered = 0;
//	while(1){
//		osSignalWait(1,osWaitForever);
//		current_key = detect_key_pressed();
//		
//		if(current_key != previous_key){
//			// Key change detected (key press)
//			previous_key = current_key;
//			digit_has_been_entered = 1;
//		}
//		else{
//			// No key press
//			digit_has_been_entered = 0;
//		}
//		
//		if(digit_has_been_entered == 1 && current_key != NO_KEY_PRESSED){
//			
//			osMutexWait(keypad_mutex,osWaitForever);
//			
//			if(current_key == ENTER_KEY){
//				display_mode = !display_mode;
//			}
//		
//			if(display_mode == 0){
//				if(current_key == SHOW_ROLL){
//					angle_to_draw = SHOW_ROLL;
//				}
//				else if(current_key == SHOW_PITCH){
//					angle_to_draw = SHOW_PITCH;
//				}
//			}
//			
//			osMutexRelease(keypad_mutex);
//		}
//	}
//}

//void display_refresher(void const *argument){
//	int32_t alarm_flag_counter = 0;
//	float previous_roll = 0;
//	float previous_pitch = 0;
//	uint8_t previous_display_mode = 0;
//	while(1){
//		osSignalWait(1,osWaitForever);
//		if(++alarm_flag_counter>=ALARM_COUNTER_MAX){
//			alarm_flag_counter=0;
//		}
//		
//		osMutexWait(temperature_mutex,osWaitForever);
//		osMutexWait(keypad_mutex,osWaitForever);
//		
//		if(temperature > TEMPERATURE_THRESHOLD && alarm_flag_counter<ALARM_COUNTER_MAX/2){
//			draw_number(ALARM);
//		}
//		else if(display_mode == 0){
//			osMutexWait(accelerometer_mutex,osWaitForever);
//			//printf("displayMode %d     previousDisplayMode %d\n", display_mode, previous_display_mode);
//			if(angle_to_draw == SHOW_ROLL && ((fabs(roll - previous_roll) > ANGLE_THRESHOLD) || (display_mode != previous_display_mode))){
//				previous_roll = roll;
//				draw_number(roll);
//			}
//			else if(angle_to_draw == SHOW_PITCH && ((fabs(pitch - previous_pitch) > ANGLE_THRESHOLD) || (display_mode != previous_display_mode))){
//				previous_pitch = pitch;
//				draw_number(pitch);
//			}
//			previous_display_mode = display_mode;
//			osMutexRelease(accelerometer_mutex);
//		}
//		else{
//			draw_number(temperature);
//			previous_display_mode = display_mode;
//		}
//		
//		osMutexRelease(keypad_mutex);
//		osMutexRelease(temperature_mutex);

//		refresh_7_segment();
//	}
//}



int main(){
	LSM9DS1_LowLevel_Init();
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); 
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_SetBits(GPIOE, GPIO_Pin_3);
	printf("PE3 %d\n", GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3));
	
	uint8_t byte_write = 96; //119 Hz and 2g range
	LSM9DS1_Write(&byte_write, LSM9DS1_CTRL_REG6_XL, 1); 
	printf("Sent %d on the bus\n", byte_write);
	
	uint8_t byte_read = 0;
	LSM9DS1_Read(&byte_read, LSM9DS1_CTRL_REG6_XL, 1); 
	printf("Received %d on the bus\n", byte_read);
	
	
	
	byte_write = 56; //enable all axes
	LSM9DS1_Write(&byte_write, LSM9DS1_CTRL_REG5_XL, 1); 
	printf("Sent %d on the bus\n", byte_write);
	
	byte_read = 0;
	LSM9DS1_Read(&byte_read, LSM9DS1_CTRL_REG5_XL, 1); 
	printf("Received %d on the bus\n", byte_read);
	
	int32_t accelerometer_out[3];
	LSM9DS1_ReadACC(accelerometer_out);
	
	printf("%d %d %d\n", accelerometer_out[0],accelerometer_out[1],accelerometer_out[2]);
//	osKernelInitialize ();                    // initialize CMSIS-RTOS

//	display_mode = 0;
//	angle_to_draw = SHOW_ROLL;
//	
//  // initialize peripherals here
//	initialize_ADC_Temp();
//	init_accelerometer();
//	init_interrupts();
//	init_7_segment();
//	init_TIM3();
//	init_TIM4();
//	init_TIM5();
//	EXTI_GenerateSWInterrupt(EXTI_Line0); 
//	
//  // create 'thread' functions that start executing,
//  // example: tid_name = osThreadCreate (osThread(name), NULL);
//	temperature_reader_thread = osThreadCreate(osThread(temperature_reader),NULL);
//	display_refresher_thread = osThreadCreate(osThread(display_refresher),NULL);
//	accelerometer_reader_thread = osThreadCreate(osThread(accelerometer_reader),NULL);
//	keypad_detector_thread = osThreadCreate(osThread(keypad_detector),NULL);
//	
//	osKernelStart();                         // start thread execution 
	
}


