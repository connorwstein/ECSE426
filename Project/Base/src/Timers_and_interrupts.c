/**
  ******************************************************************************
  * @file    Timers_and_interrupts.c
  * @author  Connor Stein (connor.stein@mail.mcgill.ca), Kevin Musgrave (takeshi.musgrave@mail.mcgill.ca),
						 Faisal Al-Kabariti (faisal.al-kabariti@mail.mcgill.ca), & Kamil Ahmad (kamil.ahmad@mail.mcgill.ca)
  * @version V1.0.0
  * @date    12-01-2015
  * @brief   Timers and interrupts needed for the LCD board
  ****************************************************************************** 
*/

#include "Timers_and_interrupts.h"


/**
  * @brief  Initializes the interrupt for GDO0. Falling edge so that it triggers when packet is received.
  * @param  None
  * @retval None
  */
void init_EXTI_interrupt(void){
		GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); // Enable 
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); // Enable Syscfg clock
    
    // Set GPIO pin as input for GDO0
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOE, &GPIO_InitStruct);
    
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4); // Using PE4 for EXTI_Line4     
    EXTI_InitStruct.EXTI_Line = EXTI_Line4; // PE4 is connected to EXTI_Line4
    EXTI_InitStruct.EXTI_LineCmd = ENABLE; // Enable interrupt
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt; // Set to interrupt mode
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling; //Triggers on falling edge (packet received)
    EXTI_Init(&EXTI_InitStruct); // Initialize the external interrupt
 
    // Add IRQ vector to NVIC 
    NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn; // PE4 is connected to EXTI_Line4, which has EXTI4_IRQn vector 
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE; // Enable interrupt 
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00; // Set priority
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00; // Set sub priority
    NVIC_Init(&NVIC_InitStruct); // Add to the NVIC

}
