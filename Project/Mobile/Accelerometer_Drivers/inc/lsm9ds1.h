#ifndef __LSM9DS1_H
#define __LSM9DS1_H

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"

/** LSM9DS1 Init **/
typedef struct
{
  uint8_t XL_DataRate;             
	uint8_t XL_Scale;  					
	uint8_t XL_Axes;        
  uint8_t G_DataRate;               
	uint8_t G_Scale;  					
	uint8_t G_Axes;  
}LSM9DS1_InitTypeDef;



/** ACCELEROMETER **/
#define XL_DATA_RATE_10Hz 0x20
#define XL_DATA_RATE_50Hz 0x40
#define XL_DATA_RATE_119Hz 0x60
#define XL_DATA_RATE_238Hz 0x80
#define XL_DATA_RATE_476Hz 0xA0
#define XL_DATA_RATE_952Hz 0xC0

#define XL_ENABLE_X 0x08
#define XL_ENABLE_Y 0x10
#define XL_ENABLE_Z 0x20

#define XL_SCALE_2G 0x00 
#define XL_SCALE_4G 0x10
#define XL_SCALE_8G 0x18 
#define XL_SCALE_16G 0x08

#define XL_SENSITIVITY_2G    	0.061 		      
#define XL_SENSITIVITY_4G		  0.122			  
#define XL_SENSITIVITY_8G		  0.244	   	 
#define XL_SENSITIVITY_16G    	0.732 
/** END ACCELEROMETER **/

/** GYROSCOPE **/
//ctrl reg1_g
#define G_DATA_RATE_14_9 0x20
#define G_DATA_RATE_59_5 0x40
#define G_DATA_RATE_119 0x60
#define G_DATA_RATE_238 0x80
#define G_DATA_RATE_476 0xA0
#define G_DATA_RATE_952 0xC0

#define G_SCALE_245_DPS 0x00
#define G_SCALE_500_DPS 0x08
#define G_SCALE_2000_DPS 0x18

#define G_SENSITIVITY_245_DPS 8.75
#define G_SENSITIVITY_500_DPS 17.5
#define G_SENSITIVITY_2000_DPS 70

//Ctrl reg 4
#define G_ENABLE_X 0x08
#define G_ENABLE_Y 0x10
#define G_ENABLE_Z 0x20
/** END GYROSCOPE **/

/** INTERRUPT **/


/** REGISTER MAPPING **/
#define LSM9DS1_ACT_THS	0x04
#define LSM9DS1_ACT_DUR	0x05
#define LSM9DS1_INT_GEN_CFG_XL	0x06
#define LSM9DS1_INT_GEN_THS_X_XL	0x07
#define	LSM9DS1_INT_GEN_THS_Y_XL	0x08
#define	LSM9DS1_INT_GEN_THS_Z_XL	0x09
#define	LSM9DS1_INT_GEN_DUR_XL	0x0A
#define	LSM9DS1_REFERENCE_G	0x0B
/**
	INT1_CTRL: INT1_A/G pin control register.
	
	INT1_IG_G | INT1_IG_XL | INT1_FSS5 | INT1_OVR | INT1_FTH | INT1_Boot | INT1_DRDY_G | INT1_DRDY_XL
	INT1_IG_G: Gyroscope interrupt enable on INT 1_A/G pin. Default value: 0
						(0: disabled; 1: enabled)
	INT_ IG_XL Accelerometer interrupt generator on INT 1_A/G pin. Default value: 0
						(0: disabled; 1: enabled)
	INT_ FSS5 FSS5 interrupt enable on INT 1_A/G pin. Default value: 0
						(0: disabled; 1: enabled)
	INT_OVR: Overrun interrupt on INT 1_A/G pin. Default value: 0
						(0: disabled; 1: enabled)
	INT_FTH: FIFO threshold interrupt on INT 1_A/G pin. Default value: 0
						(0: disabled; 1: enabled)
	INT_ Boot: Boot status available on INT 1_A/G pin. Default value: 0
						(0: disabled; 1: enabled)
	INT_DRDY_G: Gyroscope data ready on INT 1_A/G pin. Default value: 0
						(0: disabled; 1: enabled)
	INT_DRDY_XL: Accelerometer data ready on INT 1_A/G pin. Default value: 0
						(0: disabled; 1: enabled)
*/
#define	LSM9DS1_INT1_CTRL	0x0C
#define	LSM9DS1_INT2_CTRL	0x0D
#define	LSM9DS1_WHO_AM_I	0x0F
/**
	CTRL_REG1_G: Angular rate sensor Control Register 1.
	
	ODR_G2 | ODR_G1 | ODR_G0 | FS_G1 | FS_G0 | 0 | BW_G1 | BW_G0
	
	ODR_G [2:0]: Gyroscope output data rate selection. Default value: 000, use #defines above
	FS_G [1:0]: Gyroscope full-scale selection. Default value: 00
						(00: 245 dps; 01: 500 dps; 10: Not Available; 11: 2000 dps)
	BW_G [1:0]: Gyroscope bandwidth selection. Default value: 00
*/
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

/**
	CTRL_REG4: Control Register 4
	
	0 | 0 | Zen_G | Yen_G | Xen_G | 0 | LIR_XL1 | 4D_XL1
	
	Zen_G: Gyroscope’s Yaw axis (Z) output enable. Default value: 1
				(0: Z-axis output disabled; 1: Z-axis output enabled)
	Yen_G: Gyroscope’s roll axis (Y) output enable. Default value: 1
				(0: Y-axis output disabled; 1: Y-axis output enabled)
	Xen_G: Gyroscope’s pitch axis (X) output enable. Default value: 1
				(0: X -xis output disabled; 1: X-axis output enabled)
	LIR_XL1: Latched Interrupt. Default value: 0
				(0: interrupt request not latched; 1: interrupt request latched)
	4D_XL1: 4D option enabled on Interrupt. Default value: 0
				(0: interrupt generator uses 6D for position recognition; 1: interrupt generator uses
				4D for position recognition)
*/
#define	LSM9DS1_CTRL_REG4	0x1E

/**
	CTRL_REG5_XL: Linear acceleration sensor Control Register 5.
	
	DEC_1 | DEC_0 | Zen_XL | Yen_XL | Xen_XL | 0 | 0 | 0
	
	DEC_ [0:1]: Decimation of acceleration data on OUT REG and FIFO. Default value: 00
				00: no decimation;
				01: update every 2 samples;
				10: update every 4 samples;
				11: update every 8 samples
	Zen_XL: Accelerometer’s Z-axis output enable. Default value: 1
				(0: Z-axis output disabled; 1: Z-axis output enabled)
	Yen_XL: Accelerometer’s Y-axis output enable. Default value: 1
				(0: Y-axis output disabled; 1: Y-axis output enabled)
	Xen_XL: Accelerometer’s X-axis output enable. Default value: 1
				(0: X-axis output disabled; 1: X-axis output enabled)
*/
#define	LSM9DS1_CTRL_REG5_XL	0x1F
/**
	CTRL_REG6_XL: Linear acceleration sensor Control Register 6.
	
	ODR_XL2 | ODR_XL1 | ODR_XL0 | FS1_XL | FS0_XL | BW_SCAL_ODR | BW_XL1 | BW_XL0
	
	ODR_XL[2:0] Output data rate and power mode selection. default value: 000, use #defines above
	FS_XL[1:0] Accelerometer full-scale selection. Default value: 00
						(00: ±2g; 01: ±16 g; 10: ±4 g; 11: ±8 g)
	BW_SCAL_ODR Bandwidth selection. Default value: 0
						(0: bandwidth determined by ODR selection:
						BW = 408 Hz when ODR = 952 Hz, 50 Hz, 10 Hz;
						BW = 211 Hz when ODR = 476 Hz;
						BW = 105 Hz when ODR = 238 Hz;
						BW = 50 Hz when ODR = 119 Hz;
						1: bandwidth selected according to BW_XL [2:1] selection)
	BW_XL[1:0]: Anti-aliasing filter bandwidth selection. Default value: 00
						(00: 408 Hz; 01: 211 Hz; 10: 105 Hz; 11: 50 Hz)
*/
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
/** END REGISTER MAPPING **/

/** SPI INTERFACE **/
#define LSM9DS1_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define LSM9DS1_CS_LOW()       GPIO_ResetBits(LSM9DS1_SPI_CS_GPIO_PORT, LSM9DS1_SPI_CS_PIN)
#define LSM9DS1_CS_HIGH()      GPIO_SetBits(LSM9DS1_SPI_CS_GPIO_PORT, LSM9DS1_SPI_CS_PIN)

/* Read/Write command */
#define READWRITE_CMD              ((uint8_t)0x80) 
/* Multiple byte read/write command */ 
#define MULTIPLEBYTE_CMD           ((uint8_t)0x40)
/* Dummy Byte Send by the SPI Master device in order to generate the Clock to the Slave device */
#define DUMMY_BYTE                 ((uint8_t)0x00)

#define LSM9DS1_SPI                       SPI1
#define LSM9DS1_SPI_CLK                   RCC_APB2Periph_SPI1

//SCL --> PA5
#define LSM9DS1_SPI_SCK_PIN               GPIO_Pin_5                  /* PA.05 */
#define LSM9DS1_SPI_SCK_GPIO_PORT         GPIOA                       /* GPIOA */
#define LSM9DS1_SPI_SCK_GPIO_CLK          RCC_AHB1Periph_GPIOA
#define LSM9DS1_SPI_SCK_SOURCE            GPIO_PinSource5
#define LSM9DS1_SPI_SCK_AF                GPIO_AF_SPI1
//SDO --> PA6
#define LSM9DS1_SPI_MISO_PIN              GPIO_Pin_6                  /* PA.6 */
#define LSM9DS1_SPI_MISO_GPIO_PORT        GPIOA                       /* GPIOA */
#define LSM9DS1_SPI_MISO_GPIO_CLK         RCC_AHB1Periph_GPIOA
#define LSM9DS1_SPI_MISO_SOURCE           GPIO_PinSource6
#define LSM9DS1_SPI_MISO_AF               GPIO_AF_SPI1

//SDA --> PA7
#define LSM9DS1_SPI_MOSI_PIN              GPIO_Pin_7                  /* PA.7 */
#define LSM9DS1_SPI_MOSI_GPIO_PORT        GPIOA                       /* GPIOA */
#define LSM9DS1_SPI_MOSI_GPIO_CLK         RCC_AHB1Periph_GPIOA
#define LSM9DS1_SPI_MOSI_SOURCE           GPIO_PinSource7
#define LSM9DS1_SPI_MOSI_AF               GPIO_AF_SPI1

/// REUSE SPI1 but with our own chip select
// CSA/G --> PD7
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
/** END SPI INTERFACE **/


/** FUNCTION PROTOTYPES **/
void LSM9DS1_Read_XL(int32_t* out);
void LSM9DS1_Read_G(int32_t* out);
void LSM9DS1_Write(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite);
void LSM9DS1_Read(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);
uint8_t LSM9DS1_SendByte(uint8_t byte);
void LSM9DS1_Init(LSM9DS1_InitTypeDef* init);

/** END FUNCTION PROTOTYPES **/

#endif