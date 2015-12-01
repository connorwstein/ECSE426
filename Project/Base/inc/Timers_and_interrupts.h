
#include "stm32f4xx.h"                  // Device header
#include "stm32f4xx_conf.h"

void init_EXTI_interrupt(void);
void init_TIM(TIM_TypeDef* TIMx, uint8_t NVIC_IRQChannel, uint16_t desired_frequency, uint32_t RCC_APB1Periph);
