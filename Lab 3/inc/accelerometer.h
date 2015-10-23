#ifndef __ACCELEROMETER_H
#define __ACCELEROMETER_H

#include <stdint.h>
#include "lis302dl.h"

float calculate_pitch_angle(void);
float calculate_roll_angle(void);
float calculate_yaw_angle(void);
void update_moving_average(int32_t Ax, int32_t Ay, int32_t Az);
void init_calibration(void);
float get_average_Ax1(void);
float get_average_Ay1(void);
float get_average_Az1(void);
void init_accelerometer(void); 

#endif