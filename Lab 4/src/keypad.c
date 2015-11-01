/**
  ******************************************************************************
  * @file    keypad.c
  * @author  Connor Stein (connor.stein@mail.mcgill.ca) & Kevin Musgrave (takeshi.musgrave@mail.mcgill.ca)
  * @version V1.0.0
  * @date    10-28-2015
  * @brief   All functions related to the 4x4 keypad user input. Detects key presses by 
						 setting the columns as inputs and detecting the column index pressed then
						 setting the rows as inputs and detecting the row index. Note that
						 Pin 0-7 on the keypad is mapped to PE7 - PE14 on the board.
  ****************************************************************************** 
*/

#include <stdio.h>
#include "gpio.h"

#include "keypad.h"
#define NO_ROW_OR_COLUMN_PRESSED 0

/**
	@brief Initializes the columns as inputs, ready to detect the column index. This must be called before
	check_columns().
*/
void init_columns(){
	//PE7 - PE10 mapped to columns 1 - 4
	//Set columns to inputs, active low
	init_gpio(GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10, RCC_AHB1Periph_GPIOE, GPIOE, 1, 1);
	//Set the rows to no pull output
	init_gpio(GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14, RCC_AHB1Periph_GPIOE, GPIOE, 0, 0);
	//Set output rows to logic 0
	GPIO_ResetBits(GPIOE, GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14);
}

/**
	@brief Initializes the rows as inputs, ready to detect the row index. This must be called before
	check_rows().
*/
void init_rows(){
	//PE11 - PE14 mapped to rows 1 - 4
	//Set the rows to input, active low
	init_gpio(GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14, RCC_AHB1Periph_GPIOE, GPIOE, 1, 1);
	//Set the columns to no pull output
	init_gpio(GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10, RCC_AHB1Periph_GPIOE, GPIOE, 0, 0);
	//Set output columns to logic 0
	GPIO_ResetBits(GPIOE, GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10);
}

/**
	@return An integer representing which row is pressed
*/
uint8_t check_rows(void){
	// Active LOW
	if(!GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_11)){
		return 1;
	}
	if(!GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_12)){
		return 2;
	}
	if(!GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_13)){
		return 3;
	}
	if(!GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_14)){
		return 4;
	}
	else{
		return NO_ROW_OR_COLUMN_PRESSED;
	}
}
/**
	@return An integer representing which column is pressed
*/
uint8_t check_columns(void){
	// Active LOW
	if(!GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_7)){
		return 1;
	}
	if(!GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_8)){
		return 2;
	}
	if(!GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_9)){
		return 3;
	}
	if(!GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_10)){
		return 4;
	}
	else{
		return NO_ROW_OR_COLUMN_PRESSED;
	}
}
/**
	@param Two integers representing the column and row
	@return Integer representing the key pressed
*/
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
/**
	@return Integer representing the key pressed, or an error value NO_KEY_PRESSED
*/
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




