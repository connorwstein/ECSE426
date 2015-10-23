#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#include <stdio.h>
#include "accelerometer.h"
#include "7_segment.h"

#define CHECK_BIT(var,pos) (((var) & (1<<(pos)))>>(pos))

uint8_t ticks = 0;

void init_interrupts(void){
		GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    
    /* Enable clock for GPIOE */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    /* Enable clock for SYSCFG */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    
    /* Set pin as input */
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOE, &GPIO_InitStruct);
    
    /* Tell system that you will use PE0 for EXTI_Line0 */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource0);
    
    /* PD0 is connected to EXTI_Line0 */
    EXTI_InitStruct.EXTI_Line = EXTI_Line0;
    /* Enable interrupt */
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    /* Interrupt mode */
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    /* Triggers on rising and falling edge */
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
    /* Add to EXTI */
    EXTI_Init(&EXTI_InitStruct);
 
    /* Add IRQ vector to NVIC */
    /* PD0 is connected to EXTI_Line0, which has EXTI0_IRQn vector */
    NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
    /* Set priority */
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
    /* Set sub priority */
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
    /* Enable interrupt */
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    /* Add to NVIC */
    NVIC_Init(&NVIC_InitStruct);
}

void EXTI0_IRQHandler(void){
	
	if(EXTI_GetITStatus(EXTI_Line0) != RESET){
			EXTI_ClearITPendingBit(EXTI_Line0);
			int32_t out[3];
			LIS302DL_ReadACC(out);
			//printf("%d %d %d\n", out[0],out[1],out[2]);
			update_moving_average(out[0], out[1], out[2]);
			//printf("Averaged Values %f %f %f\n", get_average_Ax1(),get_average_Ay1(), get_average_Az1()); 
			printf("Angles: roll %f pitch %f yaw %f\n", calculate_roll_angle(), calculate_pitch_angle(), calculate_yaw_angle());
	}
}

void SysTick_Handler(){
	ticks = 1;
}

int main(){

	//PE0 is used for accel interrupt
	init_7_segment();
//	test_7_segment(); //Test before the interrupts turn on
	SysTick_Config(SystemCoreClock/50);
	double test1 = 112.120938, test2 = 75.679, test3 = 1.2348;
	draw_number(test1);
//	draw_number(test2);
//	draw_number(test3);
	
//	init_accelerometer();
//	init_interrupts();
//	init_calibration();
//	EXTI_GenerateSWInterrupt(EXTI_Line0); //start the reading
	
	
	while(1){
		if(ticks){
			refresh_7_segment();
		}
	}
	
	return 0;
}


