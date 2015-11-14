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


void Blinky_GPIO_Init(void){
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);
	
}

void Blinky(void const *argument){
	while(1){
		GPIO_ToggleBits(GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
		printf("hello world\n");
		osDelay(250);
	}
}

osThreadDef(Blinky, osPriorityNormal, 1, 0);

/**
	@brief Initializes the interrupts and interrupt handler for the accelerometer 
*/
void init_interrupts(void){
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


/**
	@brief Handler for when data is available from the accelerometer
*/
void EXTI0_IRQHandler(void){
	if(EXTI_GetITStatus(EXTI_Line0) != RESET){
			EXTI_ClearITPendingBit(EXTI_Line0); // Clear the interrupt pending bit
			int32_t accelerometer_out[3], gyro_out[3];
			LSM9DS1_Read_XL(accelerometer_out); 
			LSM9DS1_Read_G(gyro_out);
			printf("ACC %d %d %d GYRO %d %d %d\n", accelerometer_out[0], accelerometer_out[1], accelerometer_out[2], gyro_out[0], gyro_out[1], gyro_out[2]);
	}
}

/*
 * main: initialize and start the system
 */
int main (void) {
	
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
	print_all_ctrl_regs();
	init_interrupts();
	EXTI_GenerateSWInterrupt(EXTI_Line0); 
	while(1){
	}

//  osKernelInitialize ();                    // initialize CMSIS-RTOS
//	
//	// ID for thread
//	osThreadId	Blinky_thread;
//	
//  // initialize peripherals here
//	Blinky_GPIO_Init();
//	
//  // create 'thread' functions that start executing,
//  // example: tid_name = osThreadCreate (osThread(name), NULL);
//	Blinky_thread = osThreadCreate(osThread(Blinky), NULL);
//	
//	osKernelStart ();                         // start thread execution 
}


