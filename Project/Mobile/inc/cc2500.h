 #include "stm32f4xx.h"
 #include "stm32f4xx_conf.h"

//Pin 1: Ground -> GND
//Pin 2: VCC -> VCC
//Pin 13: GDO0 -> PB11
//Pin 14: GDO2 -> PB12
//SPI2 PIN CANNOT CHANGE RANDOMLY SPECIFIED IN THE DISCOVERY BOARD MANUAL FOR SPI2
//Pin 15: SPI MISO -> PB14 	
//Pin 16: SPI Clock -> PB13
//Pin 17: SPI Slave enable -> PB12
//Pin 18: SPI MOSI -> PB15

#define CC2500_IOCFG2 0x00
#define CC2500_IOCFG1 0x01
#define CC2500_IOCFG0 0x02
#define CC2500_FIFOTHR 0x03
#define CC2500_SYNC1 0x04
#define CC2500_SYNC0 0x05
#define CC2500_PKTLEN 0x06
#define CC2500_PKTCTRL1 0x07
#define CC2500_PKTCTRL0 0x08
#define CC2500_ADDR 0x09
#define CC2500_CHANNR 0x0A
#define CC2500_FSCTRL1 0x0B
#define CC2500_FSCTRL0 0x0C
#define CC2500_FREQ2 0x0D
#define CC2500_FREQ1 0x0E
#define CC2500_FREQ0 0x0F
#define CC2500_MDMCFG4 0x10
#define CC2500_MDMCFG3 0x11
#define CC2500_MDMCFG2 0x12
#define CC2500_MDMCFG1 0x13
#define CC2500_MDMCFG0 0x14
#define CC2500_DEVIATN 0x15
#define CC2500_MCSM2 0x16
#define CC2500_MCSM1 0x17
#define CC2500_MCSM0 0x18
#define CC2500_FOCCFG 0x19
#define CC2500_BSCFG 0x1A
#define CC2500_AGCTRL2 0x1B
#define CC2500_AGCTRL1 0x1C
#define CC2500_AGCTRL0 0x1D
#define CC2500_WOREVT1 0x1E
#define CC2500_WOREVT0 0x1F
#define CC2500_WORCTRL 0x20
#define CC2500_FREND1 0x21
#define CC2500_FREND0 0x22
#define CC2500_FSCAL3 0x23
#define CC2500_FSCAL2 0x24
#define CC2500_FSCAL1 0x25
#define CC2500_FSCAL0 0x26
#define CC2500_RCCTRL1 0x27
#define CC2500_RCCTRL0 0x28
#define CC2500_FSTEST 0x29
#define CC2500_PTEST 0x2A
#define CC2500_AGCTEST 0x2B
#define CC2500_TEST2 0x2C
#define CC2500_TEST1 0x2D
#define CC2500_TEST0 0x2E
#define CC2500_PARTNUM 0x30
#define CC2500_VERSION 0x31
#define CC2500_FREQEST 0x32
#define CC2500_LQI 0x33
#define CC2500_RSSI 0x34
#define CC2500_MARCSTATE 0x35
#define CC2500_WORTIME1 0x36
#define CC2500_WORTIME0 0x37
#define CC2500_PKTSTATUS 0x38
#define CC2500_VCO_VC_DAC 0x39
#define CC2500_TXBYTES 0x3A
#define CC2500_RXBYTES 0x3B
#define CC2500_RCCTRL1_STATUS 0x3C
#define CC2500_RCCTRL0_STATUS 0x3D


/**
	@brief command  strobe registers
*/
#define CC2500_SRES 0x30
#define CC2500_SFSTXON 0x31
#define CC2500_SXOFF 0x32
#define CC2500_SCAL 0x33
#define CC2500_SRX 0x34
#define CC2500_STX 0x35
#define CC2500_SIDLE 0x36

#define CC2500_SWOR 0x38
#define CC2500_SPWD 0x39
#define CC2500_SFRX 0x3A
#define CC2500_SFTX 0x3B
#define CC2500_SWORRST 0x3C
#define CC2500_SNOP 0x3D
#define CC2500_PATABLE 0x3E
#define CC2500_FIFO 0x3F

/**
  * @brief  CC2500 Register settings
  */

#define VAL_CC2500_IOCFG2 	0x29 		
#define VAL_CC2500_IOCFG0 	0x06	

//threshold at which signal is set on GDO pin
#define VAL_CC2500_FIFOTHR 	0x0E

//maximum length of each packet (set to 255 bytes right now)
#define VAL_CC2500_PKTLEN 	0xFF

//autoflush enabled, do not append status, no address check
#define VAL_CC2500_PKTCTRL1 0x08	

//CRC calculation and CRC check enabled, variable packet length mode
#define VAL_CC2500_PKTCTRL0 0x05	

//device address
#define VAL_CC2500_ADDR 		0x00	

//channel number
#define VAL_CC2500_CHANNR 	0x00

#define VAL_CC2500_FSCTRL1 	0x0C	
#define VAL_CC2500_FSCTRL0 	0x00	



//New numbers for our group 2 frequency
//01011101 0x5D
//10010011 0x93
//11011010 0xDA

#define VAL_CC2500_FREQ2 		0x5D	
#define VAL_CC2500_FREQ1 		0x93	
#define VAL_CC2500_FREQ0 		0xDA	


#define VAL_CC2500_MDMCFG4 	0x0E
#define VAL_CC2500_MDMCFG3 	0x3B
#define VAL_CC2500_MDMCFG2 	0x73	
#define VAL_CC2500_MDMCFG1 	0x42	
#define VAL_CC2500_MDMCFG0 	0xF8	

#define VAL_CC2500_DEVIATN 	0x00	


#define VAL_CC2500_MCSM1 		0x00	
#define VAL_CC2500_MCSM0 		0x18	

#define VAL_CC2500_FOCCFG 	0x1D	
#define VAL_CC2500_BSCFG 		0x1C	

#define VAL_CC2500_AGCTRL2 	0xC7	 
#define VAL_CC2500_AGCTRL1 	0x40	
#define VAL_CC2500_AGCTRL0 	0xB0

#define VAL_CC2500_FREND1 	0xB6	 
#define VAL_CC2500_FREND0 	0x10	

#define VAL_CC2500_FSCAL3 	0xEA	 
#define VAL_CC2500_FSCAL2 	0x0A	
#define VAL_CC2500_FSCAL1 	0x00	 
#define VAL_CC2500_FSCAL0 	0x19	

#define VAL_CC2500_FSTEST 	0x59

#define VAL_CC2500_TEST2 		0x88	
#define VAL_CC2500_TEST1 		0x31	
#define VAL_CC2500_TEST0 		0x0B



/**
  * @brief  cc2500 SPI Interface pins
  */
#define CC2500_SPI                       SPI2
#define CC2500_SPI_CLK                   RCC_APB1Periph_SPI2

#define CC2500_SPI_SCK_PIN               GPIO_Pin_13                  /* PB14 */
#define CC2500_SPI_SCK_GPIO_PORT         GPIOB                       /* GPIOB */
#define CC2500_SPI_SCK_GPIO_CLK          RCC_AHB1Periph_GPIOB
#define CC2500_SPI_SCK_SOURCE            GPIO_PinSource13
#define CC2500_SPI_SCK_AF                GPIO_AF_SPI2

#define CC2500_SPI_MISO_PIN              GPIO_Pin_14                /* PB13 */
#define CC2500_SPI_MISO_GPIO_PORT        GPIOB                       /* GPIOB */
#define CC2500_SPI_MISO_GPIO_CLK         RCC_AHB1Periph_GPIOB
#define CC2500_SPI_MISO_SOURCE           GPIO_PinSource14
#define CC2500_SPI_MISO_AF               GPIO_AF_SPI2

#define CC2500_SPI_MOSI_PIN              GPIO_Pin_15                 /* PB8 */
#define CC2500_SPI_MOSI_GPIO_PORT        GPIOB                       /* GPIOB */
#define CC2500_SPI_MOSI_GPIO_CLK         RCC_AHB1Periph_GPIOB
#define CC2500_SPI_MOSI_SOURCE           GPIO_PinSource15
#define CC2500_SPI_MOSI_AF               GPIO_AF_SPI2

#define CC2500_SPI_CS_PIN                GPIO_Pin_12                  /* PB15 */
#define CC2500_SPI_CS_GPIO_PORT          GPIOB                       /* GPIOB */
#define CC2500_SPI_CS_GPIO_CLK           RCC_AHB1Periph_GPIOB

#define CC2500_SPI_GDO0_PIN              GPIO_Pin_11                  /* PB11 */
#define CC2500_SPI_GDO0_GPIO_PORT        GPIOB                       /* GPIOB */
#define CC2500_SPI_GDO0_GPIO_CLK         RCC_AHB1Periph_GPIOB
#define CC2500_SPI_GDO0_EXTI_LINE        EXTI_Line11
#define CC2500_SPI_GDO0_EXTI_PORT_SOURCE EXTI_PortSourceGPIOB
#define CC2500_SPI_GDO0_EXTI_PIN_SOURCE  EXTI_PinSource11
#define CC2500_SPI_GDO0_EXTI_IRQn        EXTI11_IRQn 



#define CC2500_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define CC2500_CS_LOW()       GPIO_ResetBits(CC2500_SPI_CS_GPIO_PORT, CC2500_SPI_CS_PIN)
#define CC2500_CS_HIGH()      GPIO_SetBits(CC2500_SPI_CS_GPIO_PORT, CC2500_SPI_CS_PIN)
#define AMOUNT_OF_WRITABLE_PATH_DATA 62
#define PAYLOAD_LENGTH 63
#define SIZE_OF_FIFO 64
#define BASE_ADDRESS 0x3b

uint8_t cc2500_SendByte(uint8_t byte);
void cc2500_Write(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite);
void cc2500_Read(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead);
void cc2500_Send_Command_Strobe(uint8_t* pBuffer, uint8_t ReadAddr);
void cc2500_Read_Status_Register(uint8_t* pBuffer, uint8_t ReadAddr);
void cc2500_One_Byte_Read(uint8_t* pBuffer, uint8_t ReadAddr);
void cc2500_LowLevel_Init(void);
uint32_t cc2500_TIMEOUT_UserCallback(void);
void cc2500_configure_registers(void);
void cc2500_Transmit_Data(uint8_t* input_array,uint16_t num_bytes);
void cc2500_start_up_procedure(void);