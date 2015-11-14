#include <stdint.h>
#include <stdio.h>

#include "lsm9ds1_test.h"
#include "lsm9ds1.h"

int who_am_test(void){
	uint8_t whoami = 0;
	LSM9DS1_Read(&whoami, LSM9DS1_WHO_AM_I, 1);
	if(whoami != 104){
		return 0;
	}
	return 1;
}

int axis_enable_test(void){
	uint8_t ctrl_reg_4 = 0;
	LSM9DS1_Read(&ctrl_reg_4, LSM9DS1_CTRL_REG4, 1);
	if(ctrl_reg_4 == 56){
			printf("Default all axis enabled\n");
	}
	return 0;
}

void print_all_ctrl_regs(void){
	//Acclerometer 
	uint8_t read = 0;
	LSM9DS1_Read(&read, LSM9DS1_CTRL_REG4, 1);
	printf("LSM9DS1_CTRL_REG4: %d\n", read);
	read = 0;
	LSM9DS1_Read(&read, LSM9DS1_CTRL_REG5_XL, 1);
	printf("LSM9DS1_CTRL_REG5_XL: %d\n", read);
  read = 0;
	LSM9DS1_Read(&read, LSM9DS1_CTRL_REG6_XL, 1);
	printf("LSM9DS1_CTRL_REG6_XL: %d\n", read);

	read = 0;
	LSM9DS1_Read(&read, LSM9DS1_CTRL_REG1_G, 1);
	printf("LSM9DS1_CTRL_REG1_G: %d\n", read);
	
	
	//Interrupts
	read = 0;
	LSM9DS1_Read(&read, LSM9DS1_INT2_CTRL, 1);
	printf("LSM9DS1_INT2_CTRL: %d\n", read);
	read = 0;
	LSM9DS1_Read(&read, LSM9DS1_INT1_CTRL, 1);
	printf("LSM9DS1_INT1_CTRL: %d\n", read);
	read = 0;
	LSM9DS1_Read(&read, LSM9DS1_INT_GEN_CFG_XL, 1);
	printf("LSM9DS1_INT_GEN_CFG_XL: %d\n", read);
	
	//Status
	read = 0;
	LSM9DS1_Read(&read, LSM9DS1_STATUS_REG_1, 1);
	printf("LSM9DS1_STATUS_REG_1: %d\n", read);
}


int read_acc_test(void){
	

	int32_t accelerometer_data[3];
	LSM9DS1_Read_XL(accelerometer_data);
	printf("ACC %d %d %d\n", accelerometer_data[0], accelerometer_data[1], accelerometer_data[2]);
	return 0;
}






