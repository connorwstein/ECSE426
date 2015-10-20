#include <stdio.h>
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "lis302dl.h"
#include <string.h>
//LIS3DSH is our accelerometer

#define CHECK_BIT(var,pos) (((var) & (1<<(pos)))>>(pos))
#define POLL_RATE 200 //Sample rate in hz, note that the data only becomes available every 100hz

uint8_t ticks = 0;

void init_accelerometer(void){
	LIS302DL_InitTypeDef init;
	init.Power_Mode = LIS302DL_LOWPOWERMODE_ACTIVE; //Set to low power mode
	init.Output_DataRate = LIS302DL_DATARATE_100; //Set data rate to 100Hz
	init.Axes_Enable = LIS302DL_XYZ_ENABLE; //Enable all axes
	init.Full_Scale = LIS302DL_FULLSCALE_9_2; //Use full scale range for now
	init.Self_Test = LIS302DL_SELFTEST_NORMAL; //Not sure what the self test means, but leave as normal
	
	LIS302DL_Init(&init);
}

void SysTick_Handler(){
	ticks=1;
}

int main(){

	init_accelerometer();
	SysTick_Config(SystemCoreClock/POLL_RATE); //tick every 20 ms //Input value must be less than 24 bits
	
	//Set up reading buffers
	uint8_t status_reg_buffer[1];
	memset(status_reg_buffer, 0, sizeof(status_reg_buffer));
	uint8_t accelerometer_data_buffer[3];
	memset(accelerometer_data_buffer, 0, sizeof(accelerometer_data_buffer));
	
	while(1){
		if(ticks){
			LIS302DL_Read(status_reg_buffer, LIS302DL_STATUS_REG_ADDR, 1);
			if(!CHECK_BIT(status_reg_buffer[0],3)) continue;
			if(CHECK_BIT(status_reg_buffer[0],7)){
				LIS302DL_Read(accelerometer_data_buffer, LIS302DL_OUT_X_ADDR, 1);
				LIS302DL_Read(accelerometer_data_buffer+1, LIS302DL_OUT_Y_ADDR, 1);
				LIS302DL_Read(accelerometer_data_buffer+2, LIS302DL_OUT_Z_ADDR, 1);
				printf("X: %d Y: %d Z: %d\n", accelerometer_data_buffer[0],accelerometer_data_buffer[1],accelerometer_data_buffer[2]);
			}
			ticks = 0;
		}
	}	
	
	return 0;
}


