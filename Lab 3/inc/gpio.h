#ifndef __GPIO_H
#define __GPIO_H

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

/**
	Initializes a GPIO pin to run at 100MHz. 
	Example: To initialize PE1 as an input
	init_gpio(GPIO_Pin_1, RCC_AHB1Periph_GPIOE, GPIOE, 1, 0);
	Example: To initialize PD8 as an output
	init_gpio(GPIO_Pin_8, RCC_AHB1Periph_GPIOD, GPIOD, 0, 0);
	Example: Initialize PE1 PE2 and PE3 as outputs
	init_gpio(GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3, RCC_AHB1Periph_GPIOE, GPIOE, 0, 0);
*/
void init_gpio(uint32_t pin, uint32_t peripheral_clock, GPIO_TypeDef *peripheral, uint8_t is_input, uint8_t is_pullup);



#endif