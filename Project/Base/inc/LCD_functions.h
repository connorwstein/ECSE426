#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_l3gd20.h"

#define X_ZERO_OFFSET 10
#define Y_ZERO_OFFSET 20
#define USABLE_X_SIZE 240 - (X_ZERO_OFFSET*2)
#define USABLE_Y_SIZE 320 - (Y_ZERO_OFFSET*2)
#define SCALE_CONSTANT 5
#define X_REVERSE_OFFSET 240
#define Y_REVERSE_OFFSET 320
#define CROSS_SIZE 5
#define MAX_UINT16 65535


uint16_t maximum_of_array(uint16_t* maximum_value_array, uint16_t* input_data, uint16_t size_of_array);

void image_reverse_and_zero_offset(uint16_t* input_data, uint16_t size_of_array);

void scale_data_to_screen(uint16_t* input_data, uint16_t size_of_array);

void draw_cross(uint16_t x_point,uint16_t y_point);