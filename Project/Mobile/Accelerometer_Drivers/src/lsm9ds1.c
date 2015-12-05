/**
  ******************************************************************************
  * @file    cc2500.c
	* @author  Connor Stein (connor.stein@mail.mcgill.ca)
  * @version V1.0.0
  * @date    12-01-2015
  * @brief   Implementation of the LSM9DS1 driver. All communication is done 
	through the SPI interface. 
  ****************************************************************************** 
*/
#include "lsm9ds1.h"

__IO uint32_t  LSM9DS1Timeout = LSM9DS1_FLAG_TIMEOUT;

/**
  * @brief  Get the accelerometer readings from the LSM9DS1, fills the parameter buffer
		with the X, Y, and Z accelerations in milli-gs. 
  * @param  3 32bit int buffer to store data
  * @retval None
  */
void LSM9DS1_Read_XL(int32_t* out)
{
  uint8_t buffer[6];
  uint8_t crtl, i;
   
	//Read the raw accelerometer registers
  LSM9DS1_Read(&crtl, LSM9DS1_CTRL_REG6_XL, 1); 
	LSM9DS1_Read(buffer, LSM9DS1_OUT_X_H_XL, 1);	
	LSM9DS1_Read(buffer+1, LSM9DS1_OUT_X_L_XL, 1);
	LSM9DS1_Read(buffer+2, LSM9DS1_OUT_Y_H_XL, 1);
	LSM9DS1_Read(buffer+3, LSM9DS1_OUT_Y_L_XL, 1);
	LSM9DS1_Read(buffer+4, LSM9DS1_OUT_Z_H_XL, 1);
	LSM9DS1_Read(buffer+5, LSM9DS1_OUT_Z_L_XL, 1);
	
	//Apply the appropriate sensitivity factors depending on the scale
	switch(crtl & 0x18) 
  {
    case XL_SCALE_2G:
      for(i = 0; i < 6; i+=2){
        *out =(int32_t)(XL_SENSITIVITY_2G * (int16_t)(buffer[i+1]+(buffer[i]<<8)));
        out++;
      }
      break; 
    case XL_SCALE_16G:
      for(i = 0; i < 6; i+=2){
        *out =(int32_t)(XL_SENSITIVITY_16G * (int16_t)(buffer[i+1]+(buffer[i]<<8)));
        out++;
      }
      break;
		case XL_SCALE_4G:
      for(i = 0; i < 6; i+=2){
        *out =(int32_t)(XL_SENSITIVITY_4G * (int16_t)(buffer[i+1]+(buffer[i]<<8)));
        out++;
      }
      break;    
		case XL_SCALE_8G:
			for(i = 0; i < 6; i+=2){
        *out =(int32_t)(XL_SENSITIVITY_8G * (int16_t)(buffer[i+1]+(buffer[i]<<8)));
        out++;
      }
      break;      
    default:
      break;
    }
}

/**
  * @brief  Get the gyroscope readings from the LSM9DS1, fills the parameter buffer
	with X, Y and Z readings in millidegrees per second
  * @param  3 32bit int buffer to store data
  * @retval None
  */
void LSM9DS1_Read_G(int32_t* out){
	uint8_t buffer[6];
	uint8_t crtl, i;
	
	//Read all the raw register values
	LSM9DS1_Read(&crtl, LSM9DS1_CTRL_REG1_G, 1); 
	LSM9DS1_Read(buffer, LSM9DS1_OUT_X_H_G, 1);	
	LSM9DS1_Read(buffer+1, LSM9DS1_OUT_X_L_G, 1);
	LSM9DS1_Read(buffer+2, LSM9DS1_OUT_Y_H_G, 1);
	LSM9DS1_Read(buffer+3, LSM9DS1_OUT_Y_L_G, 1);
	LSM9DS1_Read(buffer+4, LSM9DS1_OUT_Z_H_G, 1);
	LSM9DS1_Read(buffer+5, LSM9DS1_OUT_Z_L_G, 1);
	
	//Fill the parameter buffer with the appropriately scaled values
	//(concatenating the low and high bytes into a single value)
	switch(crtl & 0x18) 
  {
    case G_SCALE_245_DPS:
      for(i = 0; i < 6; i+=2){
        *out =(int32_t)(G_SENSITIVITY_245_DPS * (int16_t)(buffer[i+1]+(buffer[i]<<8)));
        out++;
      }
      break;
    case G_SCALE_500_DPS:
      for(i = 0; i < 6; i+=2){
        *out =(int32_t)(G_SENSITIVITY_500_DPS * (int16_t)(buffer[i+1]+(buffer[i]<<8)));
        out++;
      }
      break;
		case G_SCALE_2000_DPS:
      for(i = 0; i < 6; i+=2){
        *out =(int32_t)(G_SENSITIVITY_2000_DPS * (int16_t)(buffer[i+1]+(buffer[i]<<8)));
        out++;
      }
      break;   
    default:
      break;
  }
	
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

	// Turn the PE3 high, to disable the internal accelerometer in order to use 
	// the external one 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); 
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_SetBits(GPIOE, GPIO_Pin_3);
	printf("PE3 %d\n", GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3));
}
/**
  * @brief  Set LSM9DS1 Initialization.
  * @param  LSM9DS1_Config_Struct: pointer to a LSM9DS1_Config_TypeDef structure 
  *         that contains the configuration setting for the LSM9DS1.
  * @retval None
  */
void LSM9DS1_Init(LSM9DS1_InitTypeDef *init)
{
	// Configure the low level interface 
  LSM9DS1_LowLevel_Init();
	
	// Configure acclerometer data rate and scale
  uint8_t ctrl_reg_5_xl = init->XL_DataRate | init->XL_Scale; 
	LSM9DS1_Write(&ctrl_reg_5_xl,LSM9DS1_CTRL_REG6_XL, 1);
	
	// Configure accelerometer axes
	uint8_t ctrl_reg_6_xl = init->XL_Axes; 
	LSM9DS1_Write(&ctrl_reg_6_xl,LSM9DS1_CTRL_REG5_XL, 1);
	
	// Configure gyro data rate and scale
  uint8_t ctrl_reg1_g = init->G_DataRate | init->G_Scale; 
	LSM9DS1_Write(&ctrl_reg1_g,LSM9DS1_CTRL_REG1_G, 1);
	
	// Configure gyro axes
	uint8_t ctrl_reg_4 = init->G_Axes; 
	LSM9DS1_Write(&ctrl_reg_4,LSM9DS1_CTRL_REG4, 1);
  
}

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
    /* Send dummy byte (0x00) to generate the SPI clock to LSM9DS1 (Slave device) */
    *pBuffer = LSM9DS1_SendByte(DUMMY_BYTE);
    NumByteToRead--;
    pBuffer++;
  }
  
  /* Set chip select High at the end of the transmission */ 
  LSM9DS1_CS_HIGH();
}

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

