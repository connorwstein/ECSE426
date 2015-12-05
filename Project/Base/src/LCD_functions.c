#include "LCD_functions.h"

uint16_t maximum_of_array(uint16_t* maximum_value_array, uint16_t* input_data, uint16_t size_of_array){
	uint16_t x_max = 0;
	uint16_t y_max = 0;
	
	for(int i=0;i<size_of_array;i++){
		if(i%2 == 0){
			if (input_data[i] > x_max){
				x_max = input_data[i];
			}
		}
		else{
			if (input_data[i] > y_max){
				y_max = input_data[i];
			}
		}
	}
	
	maximum_value_array[0] = x_max;
	maximum_value_array[1] = y_max;
}


void image_reverse_and_zero_offset(uint16_t* input_data, uint16_t size_of_array){
	for(int i=0;i<size_of_array;i++){
		if(i%2 == 0){
			input_data[i] += X_ZERO_OFFSET;
		}
		else{
			input_data[i] += Y_ZERO_OFFSET;
			input_data[i] = Y_REVERSE_OFFSET - input_data[i];
		}
	}
}

void scale_data_to_screen(uint16_t* input_data, uint16_t size_of_array){
	
	uint16_t max_values[] = {0,0};
	uint16_t x_scale = MAX_UINT16;
	uint16_t y_scale = MAX_UINT16;
	
	maximum_of_array(max_values,input_data,size_of_array);
	
	printf("USABLE_X_SIZE %d    USABLE_Y_SIZE %d\n",USABLE_X_SIZE,USABLE_Y_SIZE);
	
	printf("max_values[0] %d    max_values[1] %d\n",max_values[0],max_values[1]);
	
	if(max_values[0] != 0){
		x_scale = ((uint16_t)USABLE_X_SIZE)/max_values[0];
	}
	if(max_values[1] != 0){
		y_scale = ((uint16_t)USABLE_Y_SIZE)/max_values[1];
	}
	uint16_t final_scale_factor = 1;
	
	if(x_scale < y_scale){
		final_scale_factor = x_scale;
	}
	else{
		final_scale_factor = y_scale;
	}
	
	printf("x_scale = %d, y_scale %d, final_scale_factor %d\n",x_scale,y_scale,final_scale_factor);
	
	for(int i=0;i<size_of_array;i++){
			input_data[i] *= final_scale_factor;
	}
}


void draw_cross(uint16_t x_point,uint16_t y_point){
	LCD_DrawUniLine(x_point-CROSS_SIZE,y_point-CROSS_SIZE,x_point+CROSS_SIZE,y_point+CROSS_SIZE);
	LCD_DrawUniLine(x_point-CROSS_SIZE,y_point+CROSS_SIZE,x_point+CROSS_SIZE,y_point-CROSS_SIZE);
}