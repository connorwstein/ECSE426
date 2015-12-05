#include "LCD_functions.h"

#define small_square_length 10
#define large_diamond_length 10
uint16_t small_square[] = {0,0,0,1,1,1,1,0,0,0};
uint16_t large_diamond[] = {110,0,0,110,110,220,220,110,110,0};
uint8_t test_string[] = {'G','R','O','U','P','2',' ','T','E','S','T'};
uint16_t cross_location[] = {110,110};

void test_scaling_and_shifting(uint16_t* input_array,uint8_t length_of_array){
	scale_data_to_screen(input_array,length_of_array);
	image_reverse_and_zero_offset(input_array,length_of_array);

	for (int i=0;i<=length_of_array-4;i+=2){
		LCD_DrawUniLine(input_array[i],input_array[i+1],input_array[i+2],input_array[i+3]);
	}
}

void test_text_printing(uint8_t* input_string){
	LCD_DisplayStringLine(0,input_string);
}


//int main(void){
//	
//	/* LCD initiatization */
//  LCD_Init();
//  
//  /* LCD Layer initiatization */
//  LCD_LayerInit();
//    
//  /* Enable the LTDC controler */
//  LTDC_Cmd(ENABLE);
//  
//  /* Set LCD foreground layer as the current layer */
//  LCD_SetLayer(LCD_FOREGROUND_LAYER);
//	
//	LCD_Clear(LCD_COLOR_WHITE);
//	
//	LCD_SetTextColor(LCD_COLOR_BLACK);
//	
//	test_scaling_and_shifting(small_square,small_square_length);
//	test_scaling_and_shifting(large_diamond,large_diamond_length);
//	test_text_printing(test_string);
//	image_reverse_and_zero_offset(cross_location,2);
//	draw_cross(cross_location[0],cross_location[1]);
//}