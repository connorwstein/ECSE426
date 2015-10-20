#include <stdio.h>
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include <arm_math.h>

#define g 1
#define BUFFER_DEPTH 50




typedef struct{
	float data[BUFFER_DEPTH];
	int index;
	float sum;
} buffer;



void update_buffer(float new_reading, buffer* moving_average){
	int current_index=moving_average->index % BUFFER_DEPTH;
	float old_value=moving_average->data[current_index];
	moving_average->sum-=old_value;
	moving_average->data[current_index]=new_reading;
	moving_average->sum+=new_reading;
	moving_average->index++;
}



float get_pitch_angle(Ax1,Ay1,Az1){
	return atan(Ax1/sqrt((Ay1*Ay1)+(Az1*Az1)));
}


float get_roll_angle(Ax1,Ay1,Az1){
	return atan(Ay1/sqrt((Ax1*Ax1)+(Az1*Az1)));
}


float get_yaw_angle(Az1){
	return acos(Az1/g);
}


void update_normalized_accelerometer(arm_matrix_instance_f32 raw_accelerometer, arm_matrix_instance_f32 calibration_matrix, arm_matrix_instance_f32 calibration_matrix_2, arm_matrix_instance_f32 normalized_accelerometer){
	
	arm_mat_mult_f32(&calibration_matrix,&raw_accelerometer,&normalized_accelerometer);
	
	arm_mat_add_f32(&normalized_accelerometer,&calibration_matrix_2,&normalized_accelerometer);
	
}


int main(){
	
	float32_t calibration_data[] = {1, 1, 1, 1, 1, 1, 1, 1, 1}; 
	float32_t calibration_data_2[] = {1, 1, 1};
	float32_t accelerometer_data[] = {1, 1, 1};
	
	arm_matrix_instance_f32 calibration_matrix;
	calibration_matrix.numRows = 3;
	calibration_matrix.numCols = 3;
	calibration_matrix.pData = calibration_data;
	
	arm_matrix_instance_f32 calibration_matrix_2;
	calibration_matrix_2.numRows = 3;
	calibration_matrix_2.numCols = 1;
	calibration_matrix_2.pData = calibration_data_2;
	
	arm_matrix_instance_f32 raw_accelerometer;
	raw_accelerometer.numRows = 3;
	raw_accelerometer.numCols = 1;
	raw_accelerometer.pData = accelerometer_data;
	
	arm_matrix_instance_f32 normalized_accelerometer;
	normalized_accelerometer.numRows = 3;
	normalized_accelerometer.numCols = 1;
	
	arm_mat_mult_f32(&calibration_matrix,&raw_accelerometer,&normalized_accelerometer);
	
	arm_mat_add_f32(&normalized_accelerometer,&calibration_matrix_2,&normalized_accelerometer);
	
	
	printf("%f", atan(0));
	
	while(1){
	}
	
	return 0;
}