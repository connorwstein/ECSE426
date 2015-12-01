/**
  ******************************************************************************
  * @file    accelerometer.c
  * @author  Connor Stein (connor.stein@mail.mcgill.ca) & Kevin Musgrave (takeshi.musgrave@mail.mcgill.ca)
  * @version V1.0.0
  * @date    10-28-2015
  * @brief   Handles the buffering of accelerometer data and angle calculations
  ****************************************************************************** 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"

#include "sensor.h"
//#include "moving_average.h"

#define NUM_MG_PER_G 1000
#define BUFFER_DEPTH_XL 10
#define BUFFER_DEPTH_G 10
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
#define ACC30 0 //Add to the z 

#define GYR11 1 
#define GYR12 0 
#define GYR13 0
#define GYR10 1800 //Add to x
#define GYR21 0
#define GYR22 1
#define GYR23 0
#define GYR20 -5500 //Add to y
#define GYR31 0
#define GYR32 0
#define GYR33 1
#define GYR30 -750 //Add to the z 

#define DEGREES(x) (180.0*x/PI)

typedef struct{
	float data[BUFFER_DEPTH_XL];
	int index;
	float sum;
} xl_buffer;

typedef struct{
	float data[BUFFER_DEPTH_G];
	int index;
	float sum;
} g_buffer;

xl_buffer Ax1_moving_average, Ay1_moving_average, Az1_moving_average;
g_buffer Gx1_moving_average, Gy1_moving_average, Gz1_moving_average;

void update_g_buffer(float new_reading, g_buffer* m_a){
	int current_index=m_a->index % BUFFER_DEPTH_G;
	float old_value=m_a->data[current_index];
	m_a->sum-=old_value;
	m_a->data[current_index]=new_reading;
	m_a->sum+=new_reading;
	m_a->index++;
}

void update_xl_buffer(float new_reading, xl_buffer* m_a){	
	int current_index=m_a->index % BUFFER_DEPTH_XL;
	float old_value=m_a->data[current_index];
	m_a->sum-=old_value;
	m_a->data[current_index]=new_reading;
	m_a->sum+=new_reading;
	m_a->index++;

}

void init_xl_buffer(xl_buffer* buffer){
	memset(buffer->data, 0, BUFFER_DEPTH_XL*sizeof(float));
	buffer->index = 0;
	buffer->sum = 0;	
}
void init_g_buffer(g_buffer* buffer){
	memset(buffer->data, 0, BUFFER_DEPTH_G*sizeof(float));
	buffer->index = 0;
	buffer->sum = 0;	
}

float calculate_pitch_angle(void){
	float inputAx1, inputAy1, inputAz1;
	inputAx1 = Ax1_moving_average.sum / BUFFER_DEPTH_XL;
	inputAy1 = Ay1_moving_average.sum / BUFFER_DEPTH_XL;
	inputAz1 = Az1_moving_average.sum / BUFFER_DEPTH_XL;
	return DEGREES(atan(inputAx1/sqrt((inputAy1*inputAy1)+(inputAz1*inputAz1))));
}

float calculate_roll_angle(void){
	float inputAx1, inputAy1, inputAz1;
	inputAx1 = Ax1_moving_average.sum / BUFFER_DEPTH_XL;
	inputAy1 = Ay1_moving_average.sum / BUFFER_DEPTH_XL;
	inputAz1 = Az1_moving_average.sum / BUFFER_DEPTH_XL;
	return DEGREES(atan(inputAy1/sqrt((inputAx1*inputAx1)+(inputAz1*inputAz1))));
}

float calculate_yaw_angle(void){
	float inputAz1;
	inputAz1 = Az1_moving_average.sum / BUFFER_DEPTH_XL;
	return DEGREES(acos(inputAz1/NUM_MG_PER_G));
}

/**
	@brief Updates the buffers for all three axis for the accelerometer
*/
void update_moving_average_xl(int32_t Ax, int32_t Ay, int32_t Az){
	update_xl_buffer(Ax*ACC11 + Ay*ACC12 + Az*ACC13 + ACC10,&Ax1_moving_average);
	update_xl_buffer(Ax*ACC21 + Ay*ACC22 + Az*ACC23 + ACC20,&Ay1_moving_average);
	update_xl_buffer(Ax*ACC31 + Ay*ACC32 + Az*ACC33 + ACC30,&Az1_moving_average);
}

/**
	@brief Updates the buffers for all three axis
*/
void update_moving_average_g(int32_t Gx, int32_t Gy, int32_t Gz){
	update_g_buffer(Gx*GYR11 + Gy*GYR12 + Gz*GYR13 + GYR10,&Gx1_moving_average);
	update_g_buffer(Gx*GYR21 + Gy*GYR22 + Gz*GYR23 + GYR20,&Gy1_moving_average);
	update_g_buffer(Gx*GYR31 + Gy*GYR32 + Gz*GYR33 + GYR30,&Gz1_moving_average);
}

int get_average_Ax1(void){
	return (int)Ax1_moving_average.sum/BUFFER_DEPTH_XL;
}

int get_average_Ay1(void){
	return (int)Ay1_moving_average.sum/BUFFER_DEPTH_XL;
}

int get_average_Az1(void){
	return (int)Az1_moving_average.sum/BUFFER_DEPTH_XL;
}

int get_average_Gx1(void){
	return (int)Gx1_moving_average.sum/BUFFER_DEPTH_G;
}

int get_average_Gy1(void){
	return (int)Gy1_moving_average.sum/BUFFER_DEPTH_G;
}

int get_average_Gz1(void){
	return (int)Gz1_moving_average.sum/BUFFER_DEPTH_G;
}

void init_sensor(void){

	LSM9DS1_InitTypeDef init;
	init.XL_DataRate = XL_DATA_RATE_119Hz;
	init.XL_Axes = XL_ENABLE_X | XL_ENABLE_Y | XL_ENABLE_Z;
	init.XL_Scale = XL_SCALE_2G;
	init.G_DataRate = G_DATA_RATE_119;
	init.G_Axes = G_ENABLE_X | G_ENABLE_Y | G_ENABLE_Z;
	init.G_Scale =  G_SCALE_500_DPS;
	LSM9DS1_Init(&init);	
	
	//Enable interrupts
	uint8_t interrupts = 0x02; //enable both accelerometer and gyro 
	LSM9DS1_Write(&interrupts,LSM9DS1_INT1_CTRL, 1);
	printf("Enabled interrupts\n");
	// Initialize buffers
	init_xl_buffer(&Ax1_moving_average);
	init_xl_buffer(&Ay1_moving_average);
	init_xl_buffer(&Az1_moving_average);
	
	init_g_buffer(&Gx1_moving_average);
	init_g_buffer(&Gy1_moving_average);
	init_g_buffer(&Gz1_moving_average);
	
}