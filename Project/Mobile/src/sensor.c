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
#include <string.h>
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"

#include "sensor.h"
#include "moving_average.h"

#define NUM_MG_PER_G 1000
#define BUFFER_DEPTH_XL 50
#define BUFFER_DEPTH_G 50
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

#define GYR11 1 
#define GYR12 0 
#define GYR13 0
#define GYR10 0 //Add to x
#define GYR21 0
#define GYR22 1
#define GYR23 0
#define GYR20 0 //Add to y
#define GYR31 0
#define GYR32 0
#define GYR33 1
#define GYR30 0 //Add to the z 

#define DEGREES(x) (180.0*x/PI)

moving_average_buffer Ax1_moving_average;
moving_average_buffer Ay1_moving_average;
moving_average_buffer Az1_moving_average;
moving_average_buffer Gx1_moving_average;
moving_average_buffer Gy1_moving_average;
moving_average_buffer Gz1_moving_average;

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
	update_buffer(Ax*ACC11 + Ay*ACC12 + Az*ACC13 + ACC10,&Ax1_moving_average, BUFFER_DEPTH_XL);
	update_buffer(Ax*ACC21 + Ay*ACC22 + Az*ACC23 + ACC20,&Ay1_moving_average, BUFFER_DEPTH_XL);
	update_buffer(Ax*ACC31 + Ay*ACC32 + Az*ACC33 + ACC30,&Az1_moving_average, BUFFER_DEPTH_XL);
}

/**
	@brief Updates the buffers for all three axis
*/
void update_moving_average_g(int32_t Gx, int32_t Gy, int32_t Gz){
	update_buffer(Gx*GYR11 + Gy*GYR12 + Gz*GYR13 + GYR10,&Gx1_moving_average, BUFFER_DEPTH_G);
	update_buffer(Gx*GYR21 + Gy*GYR22 + Gz*GYR23 + GYR20,&Gy1_moving_average, BUFFER_DEPTH_G);
	update_buffer(Gx*GYR31 + Gy*GYR32 + Gz*GYR33 + GYR30,&Gz1_moving_average, BUFFER_DEPTH_G);
}

float get_average_Ax1(void){
	return Ax1_moving_average.sum/BUFFER_DEPTH_XL;
}

float get_average_Ay1(void){
	return Ay1_moving_average.sum/BUFFER_DEPTH_XL;
}

float get_average_Az1(void){
	return Az1_moving_average.sum/BUFFER_DEPTH_XL;
}

float get_average_Gx1(void){
	return Gx1_moving_average.sum/BUFFER_DEPTH_G;
}

float get_average_Gy1(void){
	return Gy1_moving_average.sum/BUFFER_DEPTH_G;
}

float get_average_Gz1(void){
	return Gz1_moving_average.sum/BUFFER_DEPTH_G;
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
	
	// Initialize buffers
	init_buffer(&Ax1_moving_average, BUFFER_DEPTH_XL);
	init_buffer(&Ay1_moving_average, BUFFER_DEPTH_XL);
	init_buffer(&Az1_moving_average, BUFFER_DEPTH_XL);
	
	init_buffer(&Gx1_moving_average, BUFFER_DEPTH_G);
	init_buffer(&Gy1_moving_average, BUFFER_DEPTH_G);
	init_buffer(&Gz1_moving_average, BUFFER_DEPTH_G);
	
}