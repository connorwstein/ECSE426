#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

void init_TIM(TIM_TypeDef* TIMx, uint8_t NVIC_IRQChannel, uint16_t desired_frequency, uint32_t RCC_APB1Periph);
void init_gpio(uint32_t pin, uint32_t peripheral_clock, GPIO_TypeDef *peripheral, uint8_t is_input, uint8_t is_pullup);
void init_EXTI_interrupt_PushButton(void);