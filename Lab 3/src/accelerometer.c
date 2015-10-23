#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "math.h"


#include "accelerometer.h"

#define NUM_MG_PER_G 1000
#define BUFFER_DEPTH 50
#define PI 3.14159265358

#define ACC11 1 
#define ACC12 0 
#define ACC13 0
#define ACC10 0 //Add to x
#define ACC21 0
#define ACC22 1
#define ACC23 0
#define ACC20 0 //Add to y
#define ACC31 0
#define ACC32 0
#define ACC33 1
#define ACC30 70 //Add to the z 

#define DEGREES(x) (180.0*x/PI)

typedef struct{
	float data[BUFFER_DEPTH];
	int index;
	float sum;
} buffer;

buffer Ax1_moving_average;
buffer Ay1_moving_average;
buffer Az1_moving_average;

void update_buffer(float new_reading, buffer* m_a){
	int current_index=m_a->index % BUFFER_DEPTH;
	float old_value=m_a->data[current_index];
	m_a->sum-=old_value;
	m_a->data[current_index]=new_reading;
	m_a->sum+=new_reading;
	m_a->index++;
}

float calculate_pitch_angle(void){
	float inputAx1, inputAy1, inputAz1;
	inputAx1 = Ax1_moving_average.sum / BUFFER_DEPTH;
	inputAy1 = Ay1_moving_average.sum / BUFFER_DEPTH;
	inputAz1 = Az1_moving_average.sum / BUFFER_DEPTH;
	//printf("PITCH RAW: %f %f %f\n", inputAx1, inputAy1, inputAz1);
	return DEGREES(atan(inputAx1/sqrt((inputAy1*inputAy1)+(inputAz1*inputAz1))));
}

float calculate_roll_angle(void){
	float inputAx1, inputAy1, inputAz1;
	inputAx1 = Ax1_moving_average.sum / BUFFER_DEPTH;
	inputAy1 = Ay1_moving_average.sum / BUFFER_DEPTH;
	inputAz1 = Az1_moving_average.sum / BUFFER_DEPTH;
	return DEGREES(atan(inputAy1/sqrt((inputAx1*inputAx1)+(inputAz1*inputAz1))));
}

float calculate_yaw_angle(void){
	float inputAz1;
	inputAz1 = Az1_moving_average.sum / BUFFER_DEPTH;
	return DEGREES(acos(inputAz1/NUM_MG_PER_G));
}

void update_moving_average(int32_t Ax, int32_t Ay, int32_t Az){
	update_buffer(Ax*ACC11 + Ay*ACC12 + Az*ACC13 + ACC10,&Ax1_moving_average);
	update_buffer(Ax*ACC21 + Ay*ACC22 + Az*ACC23 + ACC20,&Ay1_moving_average);
	update_buffer(Ax*ACC31 + Ay*ACC32 + Az*ACC33 + ACC30,&Az1_moving_average);
}

float get_average_Ax1(void){
	return Ax1_moving_average.sum/BUFFER_DEPTH;
}

float get_average_Ay1(void){
	return Ay1_moving_average.sum/BUFFER_DEPTH;
}

float get_average_Az1(void){
	return Az1_moving_average.sum/BUFFER_DEPTH;
}

void init_calibration(void){
	memset(&Ax1_moving_average, 0, sizeof(Ax1_moving_average));
	memset(&Ay1_moving_average, 0, sizeof(Ay1_moving_average));
	memset(&Az1_moving_average, 0, sizeof(Az1_moving_average));
}

void init_accelerometer(void){
	//General Configuration
	LIS302DL_InitTypeDef init;
	init.Power_Mode = LIS302DL_LOWPOWERMODE_ACTIVE; //Set to low power mode
	init.Output_DataRate = LIS302DL_DATARATE_100; //Set data rate to 100Hz
	init.Axes_Enable = LIS302DL_XYZ_ENABLE; //Enable all axes
	init.Full_Scale = LIS302DL_FULLSCALE_2_3; //Use full scale range of 2 g
	init.Self_Test = LIS302DL_SELFTEST_NORMAL; //Not sure what the self test means, but leave as normal
	LIS302DL_Init(&init);
	
	//Interrupt configuration
	uint8_t data_ready = 4;
	LIS302DL_Write(&data_ready, LIS302DL_CTRL_REG3_ADDR, 1);
}
