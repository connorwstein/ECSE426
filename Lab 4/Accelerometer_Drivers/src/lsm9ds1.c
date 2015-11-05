
#include "lsm9ds1.h"

__IO uint32_t  LSM9DS1Timeout = LSM9DS1_FLAG_TIMEOUT;

//#ifndef USE_DEFAULT_TIMEOUT_CALLBACK
/**
  * @brief  Basic management of the timeout situation.
  * @param  None.
  * @retval None.
  */
uint32_t LSM9DS1_TIMEOUT_UserCallback(void)
{
  /* Block communication and all processes */
  while (1)
  {   
  }
}
/**
  * @brief  Reads a block of data from the LSM9DS1.
  * @param  pBuffer : pointer to the buffer that receives the data read from the LSM9DS1.
  * @param  ReadAddr : LSM9DS1's internal address to read from.
  * @param  NumByteToRead : number of bytes to read from the LSM9DS1.
  * @retval None
  */
void LSM9DS1_Read(uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead)
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
  LSM9DS1_CS_LOW();
  
  /* Send the Address of the indexed register */
  LSM9DS1_SendByte(ReadAddr);
  
  /* Receive the data that will be read from the device (MSB First) */
  while(NumByteToRead > 0x00)
  {
    /* Send dummy byte (0x00) to generate the SPI clock to LSM9DS1 (Slave device) */
    *pBuffer = LSM9DS1_SendByte(DUMMY_BYTE);
    NumByteToRead--;
    pBuffer++;
  }
  
  /* Set chip select High at the end of the transmission */ 
  LSM9DS1_CS_HIGH();
}

//#endif /* USE_DEFAULT_TIMEOUT_CALLBACK */
/**
  * @brief  Sends a Byte through the SPI interface and return the Byte received 
  *         from the SPI bus.
  * @param  Byte : Byte send.
  * @retval The received byte value
  */
uint8_t LSM9DS1_SendByte(uint8_t byte)
{
  /* Loop while DR register in not emplty */
  LSM9DS1Timeout = LSM9DS1_FLAG_TIMEOUT;
  while (SPI_I2S_GetFlagStatus(LSM9DS1_SPI, SPI_I2S_FLAG_TXE) == RESET)
  {
    if((LSM9DS1Timeout--) == 0) return LSM9DS1_TIMEOUT_UserCallback();
  }
  printf("Sending byte\n");
  /* Send a Byte through the SPI peripheral */
  SPI_I2S_SendData(LSM9DS1_SPI, byte);
  
  /* Wait to receive a Byte */
  LSM9DS1Timeout = LSM9DS1_FLAG_TIMEOUT;
  while (SPI_I2S_GetFlagStatus(LSM9DS1_SPI, SPI_I2S_FLAG_RXNE) == RESET)
  {
    if((LSM9DS1Timeout--) == 0) return LSM9DS1_TIMEOUT_UserCallback();
  }
  
  /* Return the Byte read from the SPI bus */
  return (uint8_t)SPI_I2S_ReceiveData(LSM9DS1_SPI);
}

/**
  * @brief  Initializes the low level interface used to drive the LSM9DS1
  * @param  None
  * @retval None
  */
void LSM9DS1_LowLevel_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;

  /* Enable the SPI periph */
  RCC_APB2PeriphClockCmd(LSM9DS1_SPI_CLK, ENABLE);

  /* Enable SCK, MOSI and MISO GPIO clocks */
  RCC_AHB1PeriphClockCmd(LSM9DS1_SPI_SCK_GPIO_CLK | LSM9DS1_SPI_MISO_GPIO_CLK | LSM9DS1_SPI_MOSI_GPIO_CLK, ENABLE);

  /* Enable CS  GPIO clock */
  RCC_AHB1PeriphClockCmd(LSM9DS1_SPI_CS_GPIO_CLK, ENABLE);
  
  /* Enable INT1 GPIO clock */
  RCC_AHB1PeriphClockCmd(LSM9DS1_SPI_INT1_GPIO_CLK, ENABLE);
  
  /* Enable INT2 GPIO clock */
  RCC_AHB1PeriphClockCmd(LSM9DS1_SPI_INT2_GPIO_CLK, ENABLE);

  GPIO_PinAFConfig(LSM9DS1_SPI_SCK_GPIO_PORT, LSM9DS1_SPI_SCK_SOURCE, LSM9DS1_SPI_SCK_AF);
  GPIO_PinAFConfig(LSM9DS1_SPI_MISO_GPIO_PORT, LSM9DS1_SPI_MISO_SOURCE, LSM9DS1_SPI_MISO_AF);
  GPIO_PinAFConfig(LSM9DS1_SPI_MOSI_GPIO_PORT, LSM9DS1_SPI_MOSI_SOURCE, LSM9DS1_SPI_MOSI_AF);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

  /* SPI SCK pin configuration */
  GPIO_InitStructure.GPIO_Pin = LSM9DS1_SPI_SCK_PIN;
  GPIO_Init(LSM9DS1_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);

  /* SPI  MOSI pin configuration */
  GPIO_InitStructure.GPIO_Pin =  LSM9DS1_SPI_MOSI_PIN;
  GPIO_Init(LSM9DS1_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure);

  /* SPI MISO pin configuration */
  GPIO_InitStructure.GPIO_Pin = LSM9DS1_SPI_MISO_PIN;
  GPIO_Init(LSM9DS1_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

  /* SPI configuration -------------------------------------------------------*/
  SPI_I2S_DeInit(LSM9DS1_SPI);
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_Init(LSM9DS1_SPI, &SPI_InitStructure);

  /* Enable SPI1  */
  SPI_Cmd(LSM9DS1_SPI, ENABLE);

  /* Configure GPIO PIN for Lis Chip select */
  GPIO_InitStructure.GPIO_Pin = LSM9DS1_SPI_CS_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LSM9DS1_SPI_CS_GPIO_PORT, &GPIO_InitStructure);

  /* Deselect : Chip Select high */
  GPIO_SetBits(LSM9DS1_SPI_CS_GPIO_PORT, LSM9DS1_SPI_CS_PIN);
  
  /* Configure GPIO PINs to detect Interrupts */
  GPIO_InitStructure.GPIO_Pin = LSM9DS1_SPI_INT1_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(LSM9DS1_SPI_INT1_GPIO_PORT, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = LSM9DS1_SPI_INT2_PIN;
  GPIO_Init(LSM9DS1_SPI_INT2_GPIO_PORT, &GPIO_InitStructure);
	printf("Done low level init\n");
}

