#include <stdio.h>
#include "gpio.h"

void init_keypad(){
//	First the row bits are set to output, with the column bits as input.
//	The output rows are set to logic 0. 
//	If no button is pressed, all column line inputs will be read as 
//	logic 1 due to the action of the weak pull-up resistors.
//	If, however, a button is pressed then its corresponding
//	switch will connect column and row lines, 
//		and the corresponding column line will be read as low.
	//Keypad pins are in bank A 0-7 
	//Ports 0-3 are the rows and 4-7 are the columns
	//Set the columns to pull up (active low) and input
	init_gpio(GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7, RCC_AHB1Periph_GPIOA, GPIOA, 1, 1);
	//Set the rows to no pull output
	init_gpio(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3, RCC_AHB1Periph_GPIOA, GPIOA, 0, 0);
	//Set output rows to logic 0
	GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
}

void detect_key_pressed(void){
	
	uint8_t column1 = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4);
	uint8_t column2 = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5);
	uint8_t column3 = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
	uint8_t column4 = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7);
	printf("Columns %d %d %d %d\n", column1, column2, column3, column4);

}




