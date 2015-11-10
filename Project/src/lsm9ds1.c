
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
  * @brief  Read LSM9DS1 output register, and calculate the acceleration 
  *         ACC[mg]=SENSITIVITY* (out_h*256+out_l)/16 (12 bit rappresentation)
  * @param  s16 buffer to store data
  * @retval None
  */
void LSM9DS1_ReadACC(int32_t* out)
{
  uint8_t buffer[6];
  uint8_t crtl, i = 0x00;
   
  LSM9DS1_Read(&crtl, LSM9DS1_CTRL_REG6_XL, 1);  
  LSM9DS1_Read(buffer, LSM9DS1_OUT_X_L_XL, 6);
  
  switch(crtl & 0x18) 
    {
    /* FS bit = 0 ==> Sensitivity typical value = 18milligals/digit*/ 
    case 0x00:
      for(i=0; i<0x03; i++)
      {
        *out =(int32_t)(LIS3DSH_SENSITIVITY_2G *  (int16_t)buffer[2*i]);
        out++;
      }
      break;
    /* FS bit = 1 ==> Sensitivity typical value = 72milligals/digit*/ 
    case 0x08:
      for(i=0; i<0x03; i++)
      {
        *out =(int32_t)(LIS3DSH_SENSITIVITY_16G * (int16_t)buffer[2*i]);
        out++;
      }         
      break;
		case 0x10:
      for(i=0; i<0x03; i++)
      {
        *out =(int32_t)(LIS3DSH_SENSITIVITY_4G * (int16_t)buffer[2*i]);
        out++;
      }         
      break;
		case 0x18:
      for(i=0; i<0x03; i++)
      {
        *out =(int32_t)(LIS3DSH_SENSITIVITY_8G * (int16_t)buffer[2*i]);
        out++;
      }         
      break;
    default:
      break;
    }
 }



/**
  * @brief  Writes one byte to the LSM9DS1.
  * @param  pBuffer : pointer to the buffer  containing the data to be written to the LSM9DS1.
  * @param  WriteAddr : LSM9DS1's internal address to write to.
  * @param  NumByteToWrite: Number of bytes to write.
  * @retval None
  */
void LSM9DS1_Write(uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite)
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
  LSM9DS1_CS_LOW();

  /* Send the Address of the indexed register */
  LSM9DS1_SendByte(WriteAddr);
  /* Send the data that will be written into the device (MSB First) */
  while(NumByteToWrite >= 0x01)
  {
		//printf("writing %d to address %d\n",*pBuffer,WriteAddr);
    LSM9DS1_SendByte(*pBuffer);
    NumByteToWrite--;
    pBuffer++;
  }
  
  /* Set chip select High at the end of the transmission */ 
  LSM9DS1_CS_HIGH();
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
		//printf("reading address %d\n",ReadAddr);
    /* Send dummy byte (0x00) to generate the SPI clock to LSM9DS1 (Slave device) */
    *pBuffer = LSM9DS1_SendByte(DUMMY_BYTE);
		//printf("Recevied a byte: %d\n", *pBuffer);
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
  /* Send a Byte through the SPI peripheral */
  SPI_I2S_SendData(LSM9DS1_SPI, byte);
  
  /* Wait to receive a Byte */
  LSM9DS1Timeout = LSM9DS1_FLAG_TIMEOUT;
  while (SPI_I2S_GetFlagStatus(LSM9DS1_SPI, SPI_I2S_FLAG_RXNE) == RESET)
  {
    if((LSM9DS1Timeout--) == 0) return LSM9DS1_TIMEOUT_UserCallback();
  }
  /* Return the Byte read from the SPI bus */
  return (uint8_t) SPI_I2S_ReceiveData(LSM9DS1_SPI);
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

