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

#include "stm32f4xx.h"                  
#include "stm32f4xx_conf.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#include <stdio.h>

#include "accelerometer.h"
#include "7_segment.h"
#include "keypad.h"

#define CHECK_BIT(var,pos) (((var) & (1<<(pos)))>>(pos))

#define USE_ROLL 1
#define MAX_GUESSES 3
#define ENTER_BUTTON 15
#define SIZE_OF_USER_GUESS 3

uint8_t user_guess[3], number_digits_entered, number_guesses_made, digit_has_been_entered, success, ready_to_update_moving_average,ready_to_refresh_and_detect;
int8_t current_key, previous_key;
int32_t accelerometer_out[3];
float board_angle;

/**
	@brief Initializes the interrupts and interrupt handler for the accelerometer 
*/
void init_interrupts(void){
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
}

/**
	@brief Handler for when data is available from the accelerometer
*/
void EXTI0_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line0) != RESET){
			EXTI_ClearITPendingBit(EXTI_Line0); // Clear the interrupt pending bit
			LIS302DL_ReadACC(accelerometer_out); // Read the accelerometers data
			ready_to_update_moving_average = 1;
	}
}

/**
	@brief Initializes the interrupt timer used for both refreshing the 7 segment display
	and reading input from the keypad
*/
void init_TIM3(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//Enable the peripheral clock

	TIM_TimeBaseInitTypeDef init;
	// Desired rate = ClockFrequency /(prescaler * period)
	// Clock frequency is 168MHz Period and prescaler are in the range [0x0000, 0xFFFF]
	// For 480Hz interrupt rate (100Hz was too slow), let Prescaler be 2083 and the period be 168
	init.TIM_Prescaler = 2083;
	init.TIM_CounterMode = TIM_CounterMode_Up;
	init.TIM_Period =  168; 
	init.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM3, &init); 	//Initialize Timer 3
	
	// Add to interrupt routine to the NVIC
	NVIC_InitTypeDef nvic;	
	nvic.NVIC_IRQChannel = TIM3_IRQn; 
	nvic.NVIC_IRQChannelCmd = ENABLE; 
	nvic.NVIC_IRQChannelPreemptionPriority = 0x00; 
	nvic.NVIC_IRQChannelSubPriority = 0x00; 
	NVIC_Init(&nvic); 
	
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); //Link interrupt and Timer
	TIM_Cmd(TIM3, ENABLE); 	//Start Timer
	
}
/**
	@brief Handler for the timer interrupt, refreshes the 7 segment display and checks for pressed keys
*/
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		//Refresh 7 segment
		ready_to_refresh_and_detect = 1;
	}
}

/**
	@brief Convert a user guess to a format suitable for drawing on the 7 segment. If they entered
	1 then 2 then 3, this will return 123. 
*/
uint8_t get_user_guess(){
	uint8_t output = 0;
	for(int i = 0; i < number_digits_entered; i++){
		output += user_guess[i]*pow(10,number_digits_entered-1-i);
	}
	return output;
}

/**
	@brief Clears the users guess, so they can enter another guess
*/
void clear_user_guess(void){
	for(int i = 0; i < SIZE_OF_USER_GUESS; i++){
		user_guess[i] = 0;
	}
}

int main(){

	clear_user_guess();
	number_digits_entered = 0;
	number_guesses_made = 0;
	board_angle = 0;
	digit_has_been_entered = 0; 
	success = 0; // Correctly guessed the angle of the board
	ready_to_update_moving_average = 0;
	ready_to_refresh_and_detect = 0;
	previous_key = NO_KEY_PRESSED;
	current_key = NO_KEY_PRESSED;
	
	// Start the accelerometer interrupts
	init_accelerometer();
	init_interrupts();
	EXTI_GenerateSWInterrupt(EXTI_Line0); 
	
	// Initialize the LED GPIO's for user feedback
	init_gpio(GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15, RCC_AHB1Periph_GPIOD, GPIOD, 0, 0);
	
	// Initialize the 7 segment display and the timer
	// Note that the keypad does not need to be initialized (GPIOs will be set on the first call)
	init_7_segment();
	init_TIM3();
	
	int print_counter = 0; // Just to slow down the debug printing

	while(1){
		
		if(ready_to_update_moving_average == 1){
			
			ready_to_update_moving_average = 0;
			
			if(USE_ROLL == 1){
				board_angle = fabs(calculate_roll_angle());
			}
			else{
				board_angle = fabs(calculate_pitch_angle());
			}
			
			update_moving_average(accelerometer_out[0], accelerometer_out[1], accelerometer_out[2]); // Update the global structures in accelerometer.c
		}
		
		
		if(ready_to_refresh_and_detect == 1){	
				ready_to_refresh_and_detect = 0;
				refresh_7_segment();
				current_key = detect_key_pressed();
		}
			
		if(print_counter%10000 == 0){
			printf("Angles: roll %f pitch %f yaw %f\n", calculate_roll_angle(), calculate_pitch_angle(), calculate_yaw_angle());
		}
		print_counter++;
		
		if(current_key != previous_key){
			// Key change detected (key press)
			previous_key = current_key;
			digit_has_been_entered = 1;
		}
		else{
			// No key press
			digit_has_been_entered = 0;
		}
		
		if(digit_has_been_entered == 1 && current_key != NO_KEY_PRESSED && number_guesses_made < MAX_GUESSES){
			if(current_key != ENTER_BUTTON && number_digits_entered < SIZE_OF_USER_GUESS){
				// User entered a digit of their guess
				user_guess[number_digits_entered++] = current_key; // Store that digit
				draw_number(get_user_guess()); // Draw that digit on the 7 segment
			}
			else{
				// User has entered a full guess
				printf("angle of board %f\n", board_angle);
				number_guesses_made++; // Make sure that no more digits will be read for current guess
				if(get_user_guess() < (board_angle - 4)){
					// Guess is too low, light up blue LED
					GPIO_SetBits(GPIOD, GPIO_Pin_15);
					GPIO_ResetBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14);
				}
				else if(get_user_guess() > (board_angle + 4)){
					// Guess is too high, light up red LED
					GPIO_SetBits(GPIOD, GPIO_Pin_13);
					GPIO_ResetBits(GPIOD,GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15);
				}
				else{
					// Guess is within 4 degrees
					number_guesses_made = MAX_GUESSES;
					success = 1;
					draw_number(board_angle); // Draw correct angle on the board
					// Set green LED
					GPIO_SetBits(GPIOD, GPIO_Pin_12);
					GPIO_ResetBits(GPIOD,GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
				}
				// Reset number of digits entered and clear the users guess
				number_digits_entered = 0;
				clear_user_guess();
			}
		}
		
		if(number_guesses_made == MAX_GUESSES && success == 0){
			// Failed to guess the correct digit, draw an error code on the 7 segment display
			draw_number(USER_FAIL);
			success = 1;
		}
	}
	
	return 0;
}


