/**
  ******************************************************************************
  * @file    lsm9ds1.h
	* @author  Connor Stein (connor.stein@mail.mcgill.ca)
  * @version V1.0.0
  * @date    12-01-2015
  * @brief   Module for handling all readings from the external LSM9DS1. Init_sensor function must
	be called first. New readings are fed into the internal data structures with update_moving_average\
	functions. The moving average can be obtained with the get_average functions.
  ****************************************************************************** 
*/
#ifndef __SENSOR_H
#define __SENSOR_H

#include <stdint.h>
#include "lsm9ds1.h"
#include "math.h"

float calculate_pitch_angle(void);
float calculate_roll_angle(void);
float calculate_yaw_angle(void);
void update_moving_average_xl(int32_t Ax, int32_t Ay, int32_t Az);
void update_moving_average_g(int32_t Ax, int32_t Ay, int32_t Az);
int get_average_Ax1(void);
int get_average_Ay1(void);
int get_average_Az1(void);
int get_average_Gx1(void);
int get_average_Gy1(void);
int get_average_Gz1(void);
void init_sensor(void); 

#endif