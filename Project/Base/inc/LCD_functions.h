#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_l3gd20.h"


//the amount of space at the edges of the screen on the x axis
#define X_ZERO_OFFSET 10

//the amount of space at the edges of the screen on the y axis
#define Y_ZERO_OFFSET 20

//the amount of space that can be used for displaying paths
#define USABLE_X_SIZE 240 - (X_ZERO_OFFSET*2)
#define USABLE_Y_SIZE 320 - (Y_ZERO_OFFSET*2)

//the size of the LCD, used for reversing image
#define X_REVERSE_OFFSET 240
#define Y_REVERSE_OFFSET 320

//size of the cross drawn at the end of the path
#define CROSS_SIZE 5

//maximum uint16 value
#define MAX_UINT16 65535


void maximum_of_array(uint16_t* maximum_value_array, uint16_t* input_data, uint16_t size_of_array);

void image_reverse_and_zero_offset(uint16_t* input_data, uint16_t size_of_array);

void scale_data_to_screen(uint16_t* input_data, uint16_t size_of_array);

void draw_cross(uint16_t x_point,uint16_t y_point);