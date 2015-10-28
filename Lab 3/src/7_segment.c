#include <stdio.h>


#include "7_segment.h"

#define A GPIO_Pin_14
#define B GPIO_Pin_0
#define C GPIO_Pin_13
#define D GPIO_Pin_3
#define E GPIO_Pin_5
#define F GPIO_Pin_11
#define G GPIO_Pin_15
#define DP GPIO_Pin_7

uint8_t current_digit = 0;
int32_t digits[] = {-1,-1,-1,-1};
int8_t decimals[] ={-1,-1,-1,-1};

void init_7_segment(void){
	//7 segment pins 1-15: PC1 - PC15  PC0 = pin 16
	init_gpio(GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | 
	GPIO_Pin_7 | GPIO_Pin_8 |GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
	GPIO_Pin_15 | GPIO_Pin_0, RCC_AHB1Periph_GPIOC, GPIOC, 0, 0);

}
void draw_digit(uint8_t num){

	switch(num){
			case 0:
				//0 = ABCDEF
				GPIO_SetBits(GPIOC, A | B | C | D | E | F);
				GPIO_ResetBits(GPIOC, G);
				break;			
			case 1:
				//1 = BC
				GPIO_SetBits(GPIOC, B | C);
				GPIO_ResetBits(GPIOC, A | D | E | F | G);
				break;
			case 2:
				//2 = ABGED
				GPIO_SetBits(GPIOC, A | B | G | E | D);
				GPIO_ResetBits(GPIOC, C | F );
				break;
			case 3:
				//3 = ABGCD
				GPIO_SetBits(GPIOC, A | B | G | C | D);
				GPIO_ResetBits(GPIOC, E | F);
				break;
			case 4:
				//4 = FGBC
				GPIO_SetBits(GPIOC, F | G | B | C);
				GPIO_ResetBits(GPIOC, A | D | E);
				break;
			case 5:
				//5 = AFGCD
				GPIO_SetBits(GPIOC, A | F | G | C | D);
				GPIO_ResetBits(GPIOC, E | B);
				break;
			case 6:
				//6 = ACDEFG
				GPIO_SetBits(GPIOC, A | F | G | C | D | E);
				GPIO_ResetBits(GPIOC, B );
				break;
			case 7:
				//7 = ABC
				GPIO_SetBits(GPIOC, A | B | C);
				GPIO_ResetBits(GPIOC, D | E | F | G);
				break;
			case 8:
				//8 = ABCDEFG
				GPIO_SetBits(GPIOC, A | B | C | D | E | F | G);
				break;
			case 9:
				//9 = ABCFG 
				GPIO_SetBits(GPIOC, A | B | C | F | G);
				GPIO_ResetBits(GPIOC, D | E);
				break;
		}
}

void select_digit(int8_t digit){
	switch(digit){
		case 1:
			GPIO_SetBits(GPIOC, GPIO_Pin_2 | GPIO_Pin_6 | GPIO_Pin_8);
			GPIO_ResetBits(GPIOC, GPIO_Pin_1);
			break;
		case 2:
			GPIO_SetBits(GPIOC, GPIO_Pin_1 | GPIO_Pin_6 | GPIO_Pin_8);
			GPIO_ResetBits(GPIOC, GPIO_Pin_2);
			break;
		case 3: 
			GPIO_SetBits(GPIOC, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_8);
			GPIO_ResetBits(GPIOC, GPIO_Pin_6);
			break;
		case 4:
			GPIO_SetBits(GPIOC, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_6);
			GPIO_ResetBits(GPIOC, GPIO_Pin_8);
			break;
		default:
			break;
	}
}
void clear_digits(void){
	GPIO_SetBits(GPIOC, GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_6 | GPIO_Pin_8);
	digits[0] = -1;
	digits[1] = -1;
	digits[2] = -1;
}
void draw_decimal(void){
	GPIO_SetBits(GPIOC, DP); 
}
void decimal_off(void){
	GPIO_ResetBits(GPIOC, DP);

}
void reset_decimal(void){
	decimals[0] = -1;
	decimals[1] = -1;
	decimals[2] = -1;
	
}
void draw_degree(void){
	//Select degree
  GPIO_ResetBits(GPIOC, GPIO_Pin_10);
	//Enable
	GPIO_SetBits(GPIOC, GPIO_Pin_9);
}
void refresh_7_segment(void){
	current_digit = (++current_digit)%4;
	//printf("Current digit %d\n", current_digit);
	
	if(digits[current_digit]!=-1){
		select_digit(3-current_digit);
		draw_digit(digits[current_digit]);
		if(digits[0]==0 && digits[1]==0 && digits[2]==0){
			GPIO_ResetBits(GPIOC, GPIO_Pin_10);
			GPIO_ResetBits(GPIOC, GPIO_Pin_9);
		}
		else{
			draw_degree();
		}
		if(decimals[current_digit]!=-1){
			//printf("Decimal draw\n");
			draw_decimal();
		}
		else{
			decimal_off();
		}
	}
}

void draw_number(double num){
	clear_digits();
	reset_decimal();
	uint32_t num_int;
	if(num==-1){
		digits[0] = 0;
		digits[1] = 0;
		digits[2] = 0;
		return;
	}
	else if(num > 99.95){
		num_int = (uint32_t) num; //No decimal point
	}
	else if(num > 9.95 && num < 99.95){
		num_int = (uint32_t)(num*10); 
		//Need first decimal point from the right
		decimals[1] = 1;
	}
	else if(num < 9.95){
		num_int = (uint32_t)(num*100);
		//Need second decimal point from the right
		decimals[2] = 1;
	}
	else{
		printf("Error: tried to draw invalid number\n");
	}
	printf("Decimal %d\n", num_int);
	digits[0] = num_int % 10;
	num_int /= 10;
	digits[1] = num_int % 10;
	num_int /= 10;
	digits[2] = num_int;
	printf("%d %d %d\n", digits[0], digits[1], digits[2]);
}


void test_7_segment(void){
	int j;
	for(j = 1; j < 5; j++){
		select_digit(j);
		int i;
		for(i = 0; i < 10; i++){
			draw_digit(i); 
			int k;
			for (k = 0; k < 10000000; k++); //delay
		}
	}
}

