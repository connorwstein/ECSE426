#include "stm32f4xx.h"                  // Device header
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

int userGuess[3];
int numberOfDigitsEntered;
int numberOfGuessesMade;
float angleOfBoard;
int8_t currentKey;
int8_t previousKey;
int digitHasBeenEntered;
int32_t accelerometer_out[3];

uint8_t ticks = 0;

void init_interrupts(void){
		GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    
    /* Enable clock for GPIOE */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    /* Enable clock for SYSCFG */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    
    /* Set pin as input */
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOE, &GPIO_InitStruct);
    
    /* Tell system that you will use PE0 for EXTI_Line0 */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource0);
    
    /* PD0 is connected to EXTI_Line0 */
    EXTI_InitStruct.EXTI_Line = EXTI_Line0;
    /* Enable interrupt */
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    /* Interrupt mode */
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    /* Triggers on rising and falling edge */
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
    /* Add to EXTI */
    EXTI_Init(&EXTI_InitStruct);
 
    /* Add IRQ vector to NVIC */
    /* PD0 is connected to EXTI_Line0, which has EXTI0_IRQn vector */
    NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
		/* Enable interrupt */
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    /* Set priority */
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
    /* Set sub priority */
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
    /* Add to NVIC */
    NVIC_Init(&NVIC_InitStruct);
}

void EXTI0_IRQHandler(void){
	
	if(EXTI_GetITStatus(EXTI_Line0) != RESET){
			EXTI_ClearITPendingBit(EXTI_Line0);
			LIS302DL_ReadACC(accelerometer_out);
			update_moving_average(accelerometer_out[0], accelerometer_out[1], accelerometer_out[2]);
	}
}

void init_TIM3(void){
	//Enable the peripheral clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	TIM_TimeBaseInitTypeDef init;
	//Desired rate = ClockFrequency /(prescaler * period)
	//Clock frequency is 168MHz Period and prescaler are in the range [0x0000, 0xFFFF]
	// For 1000Hz interrupt rate (100Hz was too slow), let Prescaler be 1000 and the period be 168
	init.TIM_Prescaler = 1000;
	init.TIM_CounterMode = TIM_CounterMode_Up;
	init.TIM_Period =  168; 
	init.TIM_ClockDivision = TIM_CKD_DIV1; 
	//Initialize Timer 3
	TIM_TimeBaseInit(TIM3, &init); 
	
	//Add to interrupt routine to the NVIC
	NVIC_InitTypeDef nvic;	
	nvic.NVIC_IRQChannel = TIM3_IRQn; 
	nvic.NVIC_IRQChannelCmd = ENABLE; 
	nvic.NVIC_IRQChannelPreemptionPriority = 0x00; 
	nvic.NVIC_IRQChannelSubPriority = 0x00; 
	
	NVIC_Init(&nvic); 
	//Link interrupt and Timer
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE); 
	//Start Timer
	TIM_Cmd(TIM3, ENABLE); 
	
}

int getUserGuess(){
	int output = 0;
	for(int i=0; i<numberOfDigitsEntered; i++){
		output += userGuess[i]*pow(10,numberOfDigitsEntered-1-i);
	}
	return output;
}

int setUserGuess(int value){
	for(int i=0; i<SIZE_OF_USER_GUESS; i++){
		userGuess[i] = value;
	}
}

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
		//Refresh 7 segment
		refresh_7_segment();
		currentKey = detect_key_pressed();
	}
}



int main(){

	setUserGuess(0);
	numberOfDigitsEntered = 0;
	numberOfGuessesMade = 0;
	angleOfBoard = 0;
	digitHasBeenEntered = 0;
	previousKey = NO_KEY_PRESSED;
	currentKey = NO_KEY_PRESSED;
	
	
	init_accelerometer();
	init_interrupts();
	init_calibration();
	EXTI_GenerateSWInterrupt(EXTI_Line0); //start the reading
	
	init_gpio(GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15, RCC_AHB1Periph_GPIOD, GPIOD, 0, 0);
	
	//PE0 is used for accel interrupt
	init_7_segment();
	init_TIM3();
	double test1 = 112.120938, test2 = 75.679, test3 = 1.2348;
	
	int counter = 0;

	while(1){
		
		if(USE_ROLL==1){
			angleOfBoard = fabs(calculate_roll_angle());
		}
		else{
			angleOfBoard = fabs(calculate_pitch_angle());
		}
		if(counter%10000==0){
			printf("Angles: roll %f pitch %f yaw %f\n", calculate_roll_angle(), calculate_pitch_angle(), calculate_yaw_angle());
		}
		counter++;
		
		if(currentKey!=previousKey){
			//printf("KEY PRESS %d\n",currentKey);
			previousKey = currentKey;
			digitHasBeenEntered = 1;
		}
		else{
			digitHasBeenEntered = 0;
		}
		
		
		if(digitHasBeenEntered==1 && currentKey != NO_KEY_PRESSED && numberOfGuessesMade<MAX_GUESSES){
			if(currentKey != ENTER_BUTTON && numberOfDigitsEntered<SIZE_OF_USER_GUESS){
				userGuess[numberOfDigitsEntered++] = currentKey;
				draw_number(getUserGuess());
			}
			else{
				printf("angle of board %f\n", angleOfBoard);
				numberOfGuessesMade++;
				if(getUserGuess() < (angleOfBoard - 4)){
					GPIO_SetBits(GPIOD, GPIO_Pin_15);
					GPIO_ResetBits(GPIOD,GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14);
				}
				else if(getUserGuess() > (angleOfBoard + 4)){
					GPIO_SetBits(GPIOD, GPIO_Pin_13);
					GPIO_ResetBits(GPIOD,GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15);
				}
				else{
					numberOfGuessesMade = 0;
					draw_number(angleOfBoard);
					GPIO_SetBits(GPIOD, GPIO_Pin_12);
					GPIO_ResetBits(GPIOD,GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
				}
				numberOfDigitsEntered = 0;
				setUserGuess(0);
			}
		}
	}
	
	return 0;
}


