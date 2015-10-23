#include "gpio.h"

void init_gpio(uint32_t pin, uint32_t peripheral_clock, GPIO_TypeDef *peripheral, uint8_t is_input){
	RCC_AHB1PeriphClockCmd(peripheral_clock, ENABLE); //Enable the clock for that bank of GPIOs
	GPIO_InitTypeDef init;
	init.GPIO_Pin = pin;
	if(is_input)init.GPIO_Mode = GPIO_Mode_IN; 
	else init.GPIO_Mode = GPIO_Mode_OUT;
	init.GPIO_Speed = GPIO_Speed_100MHz;
	init.GPIO_OType = GPIO_OType_PP;
	init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(peripheral, &init); 
}