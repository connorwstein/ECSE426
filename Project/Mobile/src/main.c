/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include <stdio.h>

#include "lsm9ds1_test.h"
#include "lsm9ds1.h"
#include "sensor.h"

int32_t accelerometer_out[3], gyro_out[3];
void sensor_reader(void const *argument);

osThreadId sensor_reader_thread;
osThreadDef(sensor_reader, osPriorityNormal, 1, 1000);

/**
	@brief Handler for when data is available from the accelerometer
*/
void EXTI0_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line0) != RESET){
			EXTI_ClearITPendingBit(EXTI_Line0); // Clear the interrupt pending bit
			LSM9DS1_Read_XL(accelerometer_out); 
			LSM9DS1_Read_G(gyro_out);
			
			printf("ACC %d %d %d GYRO %d %d %d\n", accelerometer_out[0], accelerometer_out[1], accelerometer_out[2], gyro_out[0], gyro_out[1], gyro_out[2]);

		  osSignalSet(sensor_reader_thread,1);
	}
}

void sensor_reader(void const *argument){
	
	while(1){
		osSignalWait(1,osWaitForever);
		update_moving_average_xl(accelerometer_out[0], accelerometer_out[1], accelerometer_out[2]); // Update the global structures in accelerometer.c
		update_moving_average_g(gyro_out[0], gyro_out[1], gyro_out[2]);
		printf("ACC %d %d %d GYRO %d %d %d\n", accelerometer_out[0], accelerometer_out[1], accelerometer_out[2], gyro_out[0], gyro_out[1], gyro_out[2]);

	}
}
/**
	@brief Initializes the interrupts and interrupt handler for the accelerometer 
*/
void init_EXT10_interrupts(void){
			
	
		GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); // Enable 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); // Enable Syscfg clock
    
    // Set GPIO pin as input for receiving acceleromter data 
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOE, &GPIO_InitStruct);
    
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource0); // Using PE0 for EXTI_Line0     
    EXTI_InitStruct.EXTI_Line = EXTI_Line0; // PE0 is connected to EXTI_Line0 
    EXTI_InitStruct.EXTI_LineCmd = ENABLE; // Enable interrupt
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt; // Set to interrupt mode
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising; //Triggers on rising edge
    EXTI_Init(&EXTI_InitStruct); // Initialize the external interrupt
 
    // Add IRQ vector to NVIC 
    NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn; // PE0 is connected to EXTI_Line0, which has EXTI0_IRQn vector 
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE; // Enable interrupt 
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00; // Set priority
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00; // Set sub priority
    NVIC_Init(&NVIC_InitStruct); // Add to the NVIC
		printf("interrupt init complete\n");
		
		
}




/*
 * main: initialize and start the system
 */
int main (void) {
	
	//osKernelInitialize();                    // initialize CMSIS-RTOS
	
	init_sensor();
	init_EXT10_interrupts();
	
	EXTI_GenerateSWInterrupt(EXTI_Line0);	// start thread execution 
	
	//sensor_reader_thread = osThreadCreate(osThread(sensor_reader),NULL);
	
	//osKernelStart();   


}


