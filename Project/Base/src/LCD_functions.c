/**
  ******************************************************************************
  * @file    LCD_functions.c
  * @author  Connor Stein (connor.stein@mail.mcgill.ca), Kevin Musgrave (takeshi.musgrave@mail.mcgill.ca),
						 Faisal Al-Kabariti (faisal.al-kabariti@mail.mcgill.ca), & Kamil Ahmad (kamil.ahmad@mail.mcgill.ca)
  * @version V1.0.0
  * @date    12-01-2015
  * @brief   Helper functions for drawing on the LCD
  ****************************************************************************** 
*/


#include "LCD_functions.h"


/**
  * @brief  Finds the maximum x and y values of the path data array
  * @param  maximum_value_array : pointer to the array that the x and y max values will be written to
						input_data : the data for which we are finding the maximum values
						size_of_array : size of input_data array
  * @retval None
  */
void maximum_of_array(uint16_t* maximum_value_array, uint16_t* input_data, uint16_t size_of_array){
	uint16_t x_max = 0;
	uint16_t y_max = 0;
	
	for(int i=0;i<size_of_array;i++){
		
		//find max of x values
		if(i%2 == 0){
			if (input_data[i] > x_max){
				x_max = input_data[i];
			}
		}
		
		//find max of y values
		else{
			if (input_data[i] > y_max){
				y_max = input_data[i];
			}
		}
	}
	
	//set values of output array
	maximum_value_array[0] = x_max;
	maximum_value_array[1] = y_max;
}

/**
  * @brief  Reverses the path and shifts it so that it displays properly
  * @param  input_data : the data to reverse
						size_of_array : size of input_data array
  * @retval None
  */
void image_reverse_and_zero_offset(uint16_t* input_data, uint16_t size_of_array){
	for(int i=0;i<size_of_array;i++){
		
		//shift the x values
		if(i%2 == 0){
			input_data[i] += X_ZERO_OFFSET;
		}
		
		//reverse and shift the y values
		else{
			input_data[i] += Y_ZERO_OFFSET;
			input_data[i] = Y_REVERSE_OFFSET - input_data[i];
		}
	}
}


/**
  * @brief  scales the data so that it takes up a large part of the screen
  * @param  input_data : the data to reverse
						size_of_array : size of input_data array
  * @retval None
  */
void scale_data_to_screen(uint16_t* input_data, uint16_t size_of_array){
	
	uint16_t max_values[] = {0,0};
	uint16_t x_scale = MAX_UINT16;
	uint16_t y_scale = MAX_UINT16;
	
	//find max values
	maximum_of_array(max_values,input_data,size_of_array);
	
	printf("USABLE_X_SIZE %d    USABLE_Y_SIZE %d\n",USABLE_X_SIZE,USABLE_Y_SIZE);
	
	printf("max_values[0] %d    max_values[1] %d\n",max_values[0],max_values[1]);
	
	//find scaling factors
	if(max_values[0] != 0){
		x_scale = ((uint16_t)USABLE_X_SIZE)/max_values[0];
	}
	if(max_values[1] != 0){
		y_scale = ((uint16_t)USABLE_Y_SIZE)/max_values[1];
	}
	uint16_t final_scale_factor = 1;
	
	//pick minimum scaling factor so that there is no distortion of the path
	if(x_scale < y_scale){
		final_scale_factor = x_scale;
	}
	else{
		final_scale_factor = y_scale;
	}
	
	printf("x_scale = %d, y_scale %d, final_scale_factor %d\n",x_scale,y_scale,final_scale_factor);
	
	//apply the scaling factor
	for(int i=0;i<size_of_array;i++){
			input_data[i] *= final_scale_factor;
	}
}

/**
  * @brief  draws a cross at a specified point
  * @param  x_point : the x coordinate
						y_point : the y coordinate
  * @retval None
  */
void draw_cross(uint16_t x_point,uint16_t y_point){
	LCD_DrawUniLine(x_point-CROSS_SIZE,y_point-CROSS_SIZE,x_point+CROSS_SIZE,y_point+CROSS_SIZE);
	LCD_DrawUniLine(x_point-CROSS_SIZE,y_point+CROSS_SIZE,x_point+CROSS_SIZE,y_point-CROSS_SIZE);
}