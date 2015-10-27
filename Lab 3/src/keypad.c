#include <stdio.h>
#include "gpio.h"

#include "keypad.h"
#define NO_ROW_OR_COLUMN_PRESSED 0

/**
	Sets the columns as inputs
*/
void init_columns(){
	//Keypad pins are in bank A 0-7 
	//columns are in 0-3 rows 4-7
	init_gpio(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3, RCC_AHB1Periph_GPIOA, GPIOA, 1, 1);
	//Set the rows to no pull output
	init_gpio(GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7, RCC_AHB1Periph_GPIOA, GPIOA, 0, 0);
	//Set output rows to logic 0
	GPIO_ResetBits(GPIOA, GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
}
/**
	Sets the rows as inputs
*/
void init_rows(){
	//Set the rows to input
	init_gpio(GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7, RCC_AHB1Periph_GPIOA, GPIOA, 1, 1);
	//Set the rows to no pull output
	init_gpio(GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3, RCC_AHB1Periph_GPIOA, GPIOA, 0, 0);
	//Set output rows to logic 0
	GPIO_ResetBits(GPIOA, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);
}
/**
	Returns an integer representing which row is pressed
*/
uint8_t check_rows(void){
	if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)){
		return 1;
	}
	if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5)){
		return 2;
	}
	if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)){
		return 3;
	}
	if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7)){
		return 4;
	}
	else{
		return NO_ROW_OR_COLUMN_PRESSED;
	}
}
/**
	Returns an integer representing which column is pressed
*/
uint8_t check_columns(void){
	// Active LOW
	if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)){
		return 1;
	}
	if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)){
		return 2;
	}
	if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2)){
		return 3;
	}
	if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)){
		return 4;
	}
	else{
		return NO_ROW_OR_COLUMN_PRESSED;
	}
}

int8_t map_key(uint8_t row, uint8_t column){
	// Key 11 = row 1 column 1, 12 = row 1 column 2 ....
	uint8_t key = row*10 + column;
	
	switch(key){
		case 11:
			return 1;
		case 12:
			return 2;
		case 13:
			return 3;
		case 14:
			return 10; //A
		case 21:
			return 4;
		case 22:
			return 5;
		case 23:
			return 6;
		case 24:
			return 11; //B
		case 31:
			return 7;
		case 32:
			return 8;
		case 33:
			return 9;
		case 34:
			return 12;
		case 41:
			return 14; //*
		case 42:
			return 0; 
		case 43:
			return 15; //#
		case 44:
			return 13; //D
		default:
			printf("Error mapping key\n");
	}
}

int8_t detect_key_pressed(void){
	init_columns();
	uint8_t column = check_columns();
	init_rows();
	uint8_t row = check_rows();
	if(row != NO_ROW_OR_COLUMN_PRESSED && column != NO_ROW_OR_COLUMN_PRESSED)
		return map_key(row, column);
	else
		return NO_KEY_PRESSED;
}




