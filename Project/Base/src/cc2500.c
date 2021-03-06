/**
  ******************************************************************************
  * @file    cc2500.c
  * @author  Connor Stein (connor.stein@mail.mcgill.ca), Kevin Musgrave (takeshi.musgrave@mail.mcgill.ca),
						 Faisal Al-Kabariti (faisal.al-kabariti@mail.mcgill.ca), & Kamil Ahmad (kamil.ahmad@mail.mcgill.ca)
  * @version V1.0.0
  * @date    12-01-2015
  * @brief   cc2500 wireless driver
  ****************************************************************************** 
*/


#include "cc2500.h"
#include "math.h"
#include "osObjects.h"

__IO uint32_t  CC2500Timeout = CC2500_FLAG_TIMEOUT;   

/* Read/Write command */
#define READWRITE_CMD              ((uint8_t)0x80) 
/* Multiple byte read/write command */ 
#define MULTIPLEBYTE_CMD           ((uint8_t)0x40)
/* Dummy Byte Send by the SPI Master device in order to generate the Clock to the Slave device */
#define DUMMY_BYTE                 ((uint8_t)0x00)

//variables used for receiving data
uint8_t command_strobe_response,num_bytes_in_FIFO;
/**
  * @brief  Reads data from the RX FIFO, and writes it to the output_array. 
  * @param  output_array : pointer to the buffer that receives the data read from the RX FIFO.
  * @retval number of bytes that were read
  */
uint8_t cc2500_Receive_Data(uint8_t* output_array){
	//code for overflow handling
	cc2500_Send_Command_Strobe(&command_strobe_response, CC2500_SRX);
	
	//shift the strobe response to ignore the last 4 bits
	if(command_strobe_response>>4 == RX_OVERFLOW){
		printf("overflow\n");
		cc2500_Send_Command_Strobe(&command_strobe_response, CC2500_SFRX);
		num_bytes_in_FIFO = 0;
	}
	
	//read the RX FIFO and write to output_array
	else{	
		cc2500_Send_Command_Strobe(&command_strobe_response, CC2500_SRX);
		//find out how much data is in RX FIFO
		cc2500_Read_Status_Register(&num_bytes_in_FIFO, CC2500_RXBYTES);
		printf("RXBYTES %d\n", num_bytes_in_FIFO);
		cc2500_Read(output_array, CC2500_FIFO,num_bytes_in_FIFO);
	}
	return num_bytes_in_FIFO;
}



/**
  * @brief  Writes a block of data to the cc2500.
  * @param  pBuffer : pointer to the buffer  containing the data to be written to the cc2500.
  * @param  WriteAddr : cc2500's internal address to write to.
  * @param  NumByteToWrite: Number of bytes to write.
  * @retval None
  */
void cc2500_Write(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
{
  /* Configure the MS bit: 
       - When 0, the address will remain unchanged in multiple read/write commands.
       - When 1, the address will be auto incremented in multiple read/write commands.
  */
  if(NumByteToWrite > 0x01)
  {
    WriteAddr |= (uint8_t)MULTIPLEBYTE_CMD;
  }
  /* Set chip select Low at the start of the transmission */
  CC2500_CS_LOW();
  
  /* Send the Address of the indexed register */
  cc2500_SendByte(WriteAddr);
  /* Send the data that will be written into the device (MSB First) */
  while(NumByteToWrite >= 0x01)
  {
    cc2500_SendByte(*pBuffer);
    NumByteToWrite--;
    pBuffer++;
  }
  
  /* Set chip select High at the end of the transmission */ 
  CC2500_CS_HIGH();
}

/**
  * @brief  Reads a block of data from the cc2500.
  * @param  pBuffer : pointer to the buffer that receives the data read from the cc2500.
  * @param  ReadAddr : cc2500's internal address to read from.
  * @param  NumByteToRead : number of bytes to read from the cc2500.
  * @retval None
  */
void cc2500_Read(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
{  
  if(NumByteToRead > 0x01)
  {
    ReadAddr |= (uint8_t)(READWRITE_CMD | MULTIPLEBYTE_CMD);
  }
  else
  {
    ReadAddr |= (uint8_t)READWRITE_CMD;
  }
  /* Set chip select Low at the start of the transmission */
  CC2500_CS_LOW();
  
  /* Send the Address of the indexed register */
  cc2500_SendByte(ReadAddr);
  
  /* Receive the data that will be read from the device (MSB First) */
  while(NumByteToRead > 0x00)
  {
    /* Send dummy byte (0x00) to generate the SPI clock to cc2500 (Slave device) */
    *pBuffer = cc2500_SendByte(DUMMY_BYTE);
    NumByteToRead--;
    pBuffer++;
  }
  
  /* Set chip select High at the end of the transmission */ 
  CC2500_CS_HIGH();
}



/**
  * @brief  Sends a command strobe
	* @param  pBuffer: where the read data is written
	* @param  ReadAddr: the read address.
  * @retval None
  */
void cc2500_Send_Command_Strobe(uint8_t* pBuffer, uint8_t ReadAddr)
{ 
	ReadAddr |= (uint8_t)(READWRITE_CMD);
	cc2500_One_Byte_Read(pBuffer, ReadAddr);
}


/**
  * @brief  Reads a status register (it sends a burst read command).
	* @param  pBuffer: where the read data is written
	* @param  ReadAddr: the read address.
  * @retval None
  */
void cc2500_Read_Status_Register(uint8_t* pBuffer, uint8_t ReadAddr)
{ 
	ReadAddr |= (uint8_t)(READWRITE_CMD | MULTIPLEBYTE_CMD);
	cc2500_One_Byte_Read(pBuffer, ReadAddr);
}


/**
  * @brief  Reads one byte from address. Used for sending command strobes and reading status registers.
	* @param  pBuffer: where the read data is written
	* @param  ReadAddr: the read address.
  * @retval None
  */
void cc2500_One_Byte_Read(uint8_t* pBuffer, uint8_t ReadAddr)
{
	 /* Set chip select Low at the start of the transmission */
  CC2500_CS_LOW();
  
  /* Send the Address of the indexed register */
  cc2500_SendByte(ReadAddr);
  
	/* Send dummy byte (0x00) to generate the SPI clock to cc2500 (Slave device) */
	*pBuffer = cc2500_SendByte(DUMMY_BYTE);
	pBuffer++;
	
  /* Set chip select High at the end of the transmission */ 
  CC2500_CS_HIGH();
}


/**
  * @brief  Resets the wireless chip, and initializes SPI and GPIO.
  * @param  None
  * @retval None
  */
void cc2500_start_up_procedure(){
	command_strobe_response = 0;
	num_bytes_in_FIFO = 0;
	
	//before sending a reset command strobe, the driver has to be initialized
	cc2500_LowLevel_Init();
	
	//delay
	osDelay(100);
	
	//then reset the driver
	cc2500_Send_Command_Strobe(&command_strobe_response, CC2500_SRES);
	
	//configure registers
	cc2500_LowLevel_Init();
	
	//delay
	osDelay(100);
}


/**
  * @brief  Initializes the low level interface used to drive the cc2500
  * @param  None
  * @retval None
  */
void cc2500_LowLevel_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;

  /* Enable the SPI periph */
  RCC_APB2PeriphClockCmd(CC2500_SPI_CLK, ENABLE);

  /* Enable SCK, MOSI, MISO, CS, GDO0 GPIO clocks */
  RCC_AHB1PeriphClockCmd(CC2500_SPI_SCK_GPIO_CLK | CC2500_SPI_MISO_GPIO_CLK | 
											CC2500_SPI_MOSI_GPIO_CLK | CC2500_SPI_CS_GPIO_CLK | CC2500_SPI_GDO0_GPIO_CLK, ENABLE);

  /* Enable GDO0 GPIO clock */
  RCC_AHB1PeriphClockCmd(CC2500_SPI_GDO0_GPIO_CLK, ENABLE);
  
  GPIO_PinAFConfig(CC2500_SPI_SCK_GPIO_PORT, CC2500_SPI_SCK_SOURCE, CC2500_SPI_SCK_AF);
  GPIO_PinAFConfig(CC2500_SPI_MISO_GPIO_PORT, CC2500_SPI_MISO_SOURCE, CC2500_SPI_MISO_AF);
  GPIO_PinAFConfig(CC2500_SPI_MOSI_GPIO_PORT, CC2500_SPI_MOSI_SOURCE, CC2500_SPI_MOSI_AF);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  /* SPI SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = CC2500_SPI_SCK_PIN;
  GPIO_Init(CC2500_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /* SPI  MOSI pin configuration */
  GPIO_InitStructure.GPIO_Pin =  CC2500_SPI_MOSI_PIN;
  GPIO_Init(CC2500_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /* SPI MISO pin configuration */
  GPIO_InitStructure.GPIO_Pin = CC2500_SPI_MISO_PIN;
  GPIO_Init(CC2500_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  /* SPI configuration -------------------------------------------------------*/
  SPI_I2S_DeInit(CC2500_SPI);
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;	// Baudrate set to 8, gives a frequency of 5.25 MHz by dividing bus fequency of 42MHz by 8. The maximum possible frequency is 6.5 MHz for burst access as specified in the data sheet of the ez430 CC2500 chipset
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
//  SPI_InitStructure.SPI_CRCPolynomial = 7;	//// what is this why do we need it????
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_Init(CC2500_SPI, &SPI_InitStructure);

  /* Enable SPI4  */
  SPI_Cmd(CC2500_SPI, ENABLE);

  /* Configure GPIO PIN for Lis Chip select */
  GPIO_InitStructure.GPIO_Pin = CC2500_SPI_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(CC2500_SPI_CS_GPIO_PORT, &GPIO_InitStructure);

  /* Deselect : Chip Select high */
  GPIO_SetBits(CC2500_SPI_CS_GPIO_PORT, CC2500_SPI_CS_PIN);
  
  /* Configure GPIO PINs to detect Interrupts */
  GPIO_InitStructure.GPIO_Pin = CC2500_SPI_GDO0_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(CC2500_SPI_GDO0_GPIO_PORT, &GPIO_InitStructure);
	
	cc2500_configure_registers();
}

/**
	* @brief Configures the CC2500 register values to required configuration
	* @param  None
  * @retval None
  */
void cc2500_configure_registers(){
		//delay before setting register values
		osDelay(100);
		uint8_t configuration_values[36]={VAL_CC2500_IOCFG2,	VAL_CC2500_IOCFG0,	VAL_CC2500_FIFOTHR,	VAL_CC2500_PKTLEN,	VAL_CC2500_PKTCTRL1,
						VAL_CC2500_PKTCTRL0, VAL_CC2500_ADDR, VAL_CC2500_CHANNR,	VAL_CC2500_FSCTRL1,	VAL_CC2500_FSCTRL0,	
						VAL_CC2500_FREQ2,	VAL_CC2500_FREQ1,	VAL_CC2500_FREQ0,	VAL_CC2500_MDMCFG4,	VAL_CC2500_MDMCFG3,
						VAL_CC2500_MDMCFG2,	VAL_CC2500_MDMCFG1,	VAL_CC2500_MDMCFG0,	VAL_CC2500_DEVIATN,	VAL_CC2500_MCSM1, VAL_CC2500_MCSM0,
						VAL_CC2500_FOCCFG,	VAL_CC2500_BSCFG,	VAL_CC2500_AGCTRL2,	VAL_CC2500_AGCTRL1,	VAL_CC2500_AGCTRL0,	VAL_CC2500_FREND1,
						VAL_CC2500_FREND0,	VAL_CC2500_FSCAL3,	VAL_CC2500_FSCAL2,	VAL_CC2500_FSCAL1,	VAL_CC2500_FSCAL0, VAL_CC2500_FSTEST,
						VAL_CC2500_TEST2,	VAL_CC2500_TEST1,	VAL_CC2500_TEST0};
	
	uint8_t configuration_adresses[36]= {CC2500_IOCFG2,	CC2500_IOCFG0,	CC2500_FIFOTHR,	CC2500_PKTLEN,	CC2500_PKTCTRL1,	
						CC2500_PKTCTRL0,CC2500_ADDR,CC2500_CHANNR,CC2500_FSCTRL1,CC2500_FSCTRL0,CC2500_FREQ2,	
						CC2500_FREQ1,	CC2500_FREQ0,	CC2500_MDMCFG4,	CC2500_MDMCFG3,	CC2500_MDMCFG2,	
						CC2500_MDMCFG1,	CC2500_MDMCFG0,	CC2500_DEVIATN,	CC2500_MCSM1,	CC2500_MCSM0,
						CC2500_FOCCFG,	CC2500_BSCFG,	CC2500_AGCTRL2,	CC2500_AGCTRL1,	CC2500_AGCTRL0,
						CC2500_FREND1,	CC2500_FREND0,	CC2500_FSCAL3,	CC2500_FSCAL2,	CC2500_FSCAL1,
						CC2500_FSCAL0,	CC2500_FSTEST,	CC2500_TEST2,	CC2500_TEST1,	CC2500_TEST0};
						
	for(int i=0; i<36; i++){
		cc2500_Write(configuration_values+i,configuration_adresses[i],1);
	}	
				
}


/**
  * @brief  Sends a Byte through the SPI interface and return the Byte received 
  *         from the SPI bus.
  * @param  Byte : Byte send.
  * @retval The received byte value
  */
uint8_t cc2500_SendByte(uint8_t byte)
{
  /* Loop while DR register is not empty */
  CC2500Timeout = CC2500_FLAG_TIMEOUT;
		
  while (SPI_I2S_GetFlagStatus(CC2500_SPI, SPI_I2S_FLAG_TXE) == RESET)
  {
		if((CC2500Timeout--) == 0) return cc2500_TIMEOUT_UserCallback(); 
	}
  
  /* Send a Byte through the SPI peripheral */
  SPI_I2S_SendData(CC2500_SPI, byte);
  
  /* Wait to receive a Byte */
  CC2500Timeout = CC2500_FLAG_TIMEOUT;
  while (SPI_I2S_GetFlagStatus(CC2500_SPI, SPI_I2S_FLAG_RXNE) == RESET)
  {
    if((CC2500Timeout--) == 0) return cc2500_TIMEOUT_UserCallback();
  }
	
  /* Return the Byte read from the SPI bus */
  return (uint8_t)SPI_I2S_ReceiveData(CC2500_SPI);
}


/**
  * @brief  Basic management of the timeout situation.
  * @param  None.
  * @retval None.
  */
uint32_t cc2500_TIMEOUT_UserCallback(void)
{
  /* Block communication and all processes */
  while (1)
  {   
  }
}
