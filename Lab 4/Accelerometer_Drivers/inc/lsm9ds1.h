#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

#define LSM9DS1_CS_LOW()       GPIO_ResetBits(LSM9DS1_SPI_CS_GPIO_PORT, LSM9DS1_SPI_CS_PIN)
#define LSM9DS1_CS_HIGH()      GPIO_SetBits(LSM9DS1_SPI_CS_GPIO_PORT, LSM9DS1_SPI_CS_PIN)

/* Read/Write command */
#define READWRITE_CMD              ((uint8_t)0x80) 
/* Multiple byte read/write command */ 
#define MULTIPLEBYTE_CMD           ((uint8_t)0x40)
/* Dummy Byte Send by the SPI Master device in order to generate the Clock to the Slave device */
#define DUMMY_BYTE                 ((uint8_t)0x00)

/*******************************************************************************
*  FF_WU_SRC_1 Register: Interrupt 1 source register.
*  Reading at this address clears FF_WU_SRC_1 register and the FF, WU 1 interrupt
*  and allow the refreshment of data in the FF_WU_SRC_1 register if the latched option
*  was chosen.
*  Read only register
*  Default value: 0x00
*  7 Reserved
*  6 IA: Interrupt active.
*        0: no interrupt has been generated
*        1: one or more interrupts have been generated
*  5 ZH: Z high. 
*        0: no interrupt
*        1: ZH event has occurred 
*  4 ZL: Z low.
*        0: no interrupt
*        1: ZL event has occurred
*  3 YH: Y high.
*        0: no interrupt
*        1: YH event has occurred 
*  2 YL: Y low.
*        0: no interrupt
*        1: YL event has occurred
*  1 YH: X high.
*        0: no interrupt
*        1: XH event has occurred 
*  0 YL: X low.
*        0: no interrupt
*        1: XL event has occurred
*******************************************************************************/



#define LSM9DS1_ACT_THS	0x04
#define LSM9DS1_ACT_DUR	0x05
#define LSM9DS1_INT_GEN_CFG_XL	0x06
#define LSM9DS1_INT_GEN_THS_X_XL	0x07
#define	LSM9DS1_INT_GEN_THS_Y_XL	0x08
#define	LSM9DS1_INT_GEN_THS_Z_XL	0x09
#define	LSM9DS1_INT_GEN_DUR_XL	0x0A
#define	LSM9DS1_REFERENCE_G	0x0B
#define	LSM9DS1_INT1_CTRL	0x0C
#define	LSM9DS1_INT2_CTRL	0x0D
#define	LSM9DS1_WHO_AM_I	0x0F
#define	LSM9DS1_CTRL_REG1_G	0x10
#define	LSM9DS1_CTRL_REG2_G	0x11
#define	LSM9DS1_CTRL_REG3_G	0x12
#define	LSM9DS1_ORIENT_CFG_G	0x13
#define	LSM9DS1_INT_GEN_SRC_G	0x14
#define	LSM9DS1_OUT_TEMP_L	0x15
#define	LSM9DS1_OUT_TEMP_H	0x16
#define	LSM9DS1_STATUS_REG_1	0x17
#define	LSM9DS1_OUT_X_L_G	0x18
#define	LSM9DS1_OUT_X_H_G	0x19
#define	LSM9DS1_OUT_Y_L_G	0x1A
#define	LSM9DS1_OUT_Y_H_G	0x1B
#define	LSM9DS1_OUT_Z_L_G	0x1C
#define	LSM9DS1_OUT_Z_H_G	0x1D
#define	LSM9DS1_CTRL_REG4	0x1E
#define	LSM9DS1_CTRL_REG5_XL	0x1F
#define	LSM9DS1_CTRL_REG6_XL	0x20
#define	LSM9DS1_CTRL_REG7_XL	0x21
#define	LSM9DS1_CTRL_REG8	0x22
#define	LSM9DS1_CTRL_REG9	0x23
#define	LSM9DS1_CTRL_REG10	0x24
#define	LSM9DS1_INT_GEN_SRC_XL	0x26
#define	LSM9DS1_STATUS_REG_2	0x27
#define	LSM9DS1_OUT_X_L_XL	0x28
#define	LSM9DS1_OUT_X_H_XL	0x29
#define	LSM9DS1_OUT_Y_L_XL	0x2A
#define	LSM9DS1_OUT_Y_H_XL	0x2B
#define	LSM9DS1_OUT_Z_L_XL	0x2C
#define	LSM9DS1_OUT_Z_H_XL	0x2D
#define	LSM9DS1_FIFO_CTRL	0x2E
#define	LSM9DS1_FIFO_SRC	0x2F
#define	LSM9DS1_INT_GEN_CFG_G	0x30
#define	LSM9DS1_INT_GEN_THS_XH_G	0x31
#define	LSM9DS1_INT_GEN_THS_XL_G	0x32
#define	LSM9DS1_INT_GEN_THS_YH_G	0x33
#define	LSM9DS1_INT_GEN_THS_YL_G	0x34
#define	LSM9DS1_INT_GEN_THS_ZH_G	0x35
#define	LSM9DS1_INT_GEN_THS_ZL_G	0x36
#define	LSM9DS1_INT_GEN_DUR_G	0x37


/** SPI INTERFACE **/

/**
  * @brief  LSM9DS1 SPI Interface pins
  */
	

#define LSM9DS1_FLAG_TIMEOUT         ((uint32_t)0x1000)
	//  PD1 = INT1 interrupt
//	PD2 = SCL 	SPI clock
//	PD3 = SDA 	SPI input
//	PD6 = CSAG    Chip select for acclerometer/ gyro
//	PD7 = SDOAG   SPI ouput from accelerometer  / gyro
	//Using SPI2 for the interface to external accelerometer sensor
/**
  * @brief  LSM9DS1 SPI Interface pins
  */
#define LSM9DS1_SPI                       SPI1
#define LSM9DS1_SPI_CLK                   RCC_APB2Periph_SPI1

#define LSM9DS1_SPI_SCK_PIN               GPIO_Pin_5                  /* PA.05 */
#define LSM9DS1_SPI_SCK_GPIO_PORT         GPIOA                       /* GPIOA */
#define LSM9DS1_SPI_SCK_GPIO_CLK          RCC_AHB1Periph_GPIOA
#define LSM9DS1_SPI_SCK_SOURCE            GPIO_PinSource5
#define LSM9DS1_SPI_SCK_AF                GPIO_AF_SPI1

#define LSM9DS1_SPI_MISO_PIN              GPIO_Pin_6                  /* PA.6 */
#define LSM9DS1_SPI_MISO_GPIO_PORT        GPIOA                       /* GPIOA */
#define LSM9DS1_SPI_MISO_GPIO_CLK         RCC_AHB1Periph_GPIOA
#define LSM9DS1_SPI_MISO_SOURCE           GPIO_PinSource6
#define LSM9DS1_SPI_MISO_AF               GPIO_AF_SPI1

#define LSM9DS1_SPI_MOSI_PIN              GPIO_Pin_7                  /* PA.7 */
#define LSM9DS1_SPI_MOSI_GPIO_PORT        GPIOA                       /* GPIOA */
#define LSM9DS1_SPI_MOSI_GPIO_CLK         RCC_AHB1Periph_GPIOA
#define LSM9DS1_SPI_MOSI_SOURCE           GPIO_PinSource7
#define LSM9DS1_SPI_MOSI_AF               GPIO_AF_SPI1

/// REUSE SPI1 but with our own chip select
#define LSM9DS1_SPI_CS_PIN                GPIO_Pin_7
#define LSM9DS1_SPI_CS_GPIO_PORT          GPIOD                      
#define LSM9DS1_SPI_CS_GPIO_CLK           RCC_AHB1Periph_GPIOD

#define LSM9DS1_SPI_INT1_PIN              GPIO_Pin_0                  /* PE.00 */
#define LSM9DS1_SPI_INT1_GPIO_PORT        GPIOE                       /* GPIOE */
#define LSM9DS1_SPI_INT1_GPIO_CLK         RCC_AHB1Periph_GPIOE
#define LSM9DS1_SPI_INT1_EXTI_LINE        EXTI_Line0
#define LSM9DS1_SPI_INT1_EXTI_PORT_SOURCE EXTI_PortSourceGPIOE
#define LSM9DS1_SPI_INT1_EXTI_PIN_SOURCE  EXTI_PinSource0
#define LSM9DS1_SPI_INT1_EXTI_IRQn        EXTI0_IRQn 

#define LSM9DS1_SPI_INT2_PIN              GPIO_Pin_1                  /* PE.01 */
#define LSM9DS1_SPI_INT2_GPIO_PORT        GPIOE                       /* GPIOE */
#define LSM9DS1_SPI_INT2_GPIO_CLK         RCC_AHB1Periph_GPIOE
#define LSM9DS1_SPI_INT2_EXTI_LINE        EXTI_Line1
#define LSM9DS1_SPI_INT2_EXTI_PORT_SOURCE EXTI_PortSourceGPIOE
#define LSM9DS1_SPI_INT2_EXTI_PIN_SOURCE  EXTI_PinSource1
#define LSM9DS1_SPI_INT2_EXTI_IRQn        EXTI1_IRQn 


uint8_t LSM9DS1_SendByte(uint8_t byte);
void LSM9DS1_LowLevel_Init(void);

