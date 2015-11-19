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
void init_calibration(void);
float get_average_Ax1(void);
float get_average_Ay1(void);
float get_average_Az1(void);
float get_average_Gx1(void);
float get_average_Gy1(void);
float get_average_Gz1(void);
void init_sensor(void); 

#endif