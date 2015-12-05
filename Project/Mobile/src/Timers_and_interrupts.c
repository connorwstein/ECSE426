/**
  ******************************************************************************
  * @file    Timers_and_interrupts.c
  * @author  Kevin Musgrave (takeshi.musgrave@mail.mcgill.ca)
  * @version V1.0.0
  * @date    12-01-2015
  * @brief   Timers and interrupt setup needed for the Mobile board
  ****************************************************************************** 
*/


#include "Timers_and_interrupts.h"


/**
  * @brief  initializes a gpio pin
  */
void init_gpio(uint32_t pin, uint32_t peripheral_clock, GPIO_TypeDef *peripheral, uint8_t is_input, uint8_t is_pullup){
	RCC_AHB1PeriphClockCmd(peripheral_clock, ENABLE); //Enable the clock for that bank of GPIOs
	GPIO_InitTypeDef init;
	init.GPIO_Pin = pin;
	if(is_input){
		init.GPIO_Mode = GPIO_Mode_IN; 
	}
	else{
		init.GPIO_Mode = GPIO_Mode_OUT;
		init.GPIO_Speed = GPIO_Speed_100MHz;
		init.GPIO_OType = GPIO_OType_PP;
	}		
	if(is_pullup)init.GPIO_PuPd = GPIO_PuPd_UP;
	else init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(peripheral, &init); 
}



/**
	@brief Initializes a timer interrupt.
	@param The input timer (Tim2, Tim3... etc), the timer interrupt channel, the desired frequency in hertz
*/
void init_TIM(TIM_TypeDef* TIMx, uint8_t NVIC_IRQChannel, uint16_t desired_frequency, uint32_t RCC_APB1Periph){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph, ENABLE);	//Enable the peripheral clock

	TIM_TimeBaseInitTypeDef init;
	// Desired rate = ClockFrequency /(prescaler * period)
	// Clock frequency is 168MHz Period and prescaler are in the range [0x0000, 0xFFFF]
	init.TIM_CounterMode = TIM_CounterMode_Up;
	init.TIM_Period = 16800;
	init.TIM_Prescaler = (168000000/2)/(desired_frequency*init.TIM_Period);	
	init.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIMx, &init); 	//Initialize Timer 5
	
	// Add to interrupt routine to the NVIC
	NVIC_InitTypeDef nvic;	
	nvic.NVIC_IRQChannel = NVIC_IRQChannel; 
	nvic.NVIC_IRQChannelCmd = ENABLE; 
	nvic.NVIC_IRQChannelPreemptionPriority = 0x00; 
	nvic.NVIC_IRQChannelSubPriority = 0x00; 
	NVIC_Init(&nvic); 
	
	TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE); //Link interrupt and Timer
	TIM_Cmd(TIMx, ENABLE); 	//Start Timer
	
}