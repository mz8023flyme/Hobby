#include "bsp_spi.h"
#include "bsp_usart.h"

static void GPIO_SPI_Config (void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;      //配置连接到的GPIO
		
	FLASH_SPI_CS_APBxClock_FUN (FLASH_SPI_CS_CLK,ENABLE);
	FLASH_SPI_SCK_APBxClock_FUN(FLASH_SPI_SCK_CLK,ENABLE);
	FLASH_SPI_APBxClock_FUN(FLASH_SPI_CLK,ENABLE);
	
	/*MISO MOSI  SCK*/
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = FLASH_SPI_MISO_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(FLASH_SPI_MISO_PORT,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = FLASH_SPI_MOSI_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(FLASH_SPI_MOSI_PORT,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = FLASH_SPI_SCK_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(FLASH_SPI_SCK_PORT,&GPIO_InitStruct);
	
	//配置CS片选引脚
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = FLASH_SPI_CS_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(FLASH_SPI_CS_PORT,&GPIO_InitStruct);
	
	SPI_FLASH_CS_HIGH();
}

static void SPI_MODE(void)
{
	SPI_InitTypeDef   SPI_InitStruct;
	
	
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_2Edge ; 
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStruct.SPI_CRCPolynomial = 0;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b ;
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB ;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master ;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft ;

	SPI_Init(FLASH_SPIx,&SPI_InitStruct);
	SPI_Cmd(FLASH_SPIx,ENABLE);
}

void SPI_FLASH_Init(void)
{
	GPIO_SPI_Config();
	SPI_MODE();
}


uint8_t SPI_FLASH_SendByte(uint8_t data)
{
	uint32_t SPITIME;
	
	SPITIME=SPI_TIMEOUT;
	
	while(SPI_I2S_GetFlagStatus(FLASH_SPIx,SPI_I2S_FLAG_TXE)==RESET)
	{
		if((SPITIME--) == 0) 
		{
			printf ("SPI 等待超时!");
			return 0;
		}
	}
	SPI_I2S_SendData(FLASH_SPIx,data);
	while(SPI_I2S_GetFlagStatus(FLASH_SPIx,SPI_I2S_FLAG_RXNE)==RESET)
	{
		if((SPITIME--) ==0) 
		{
			printf ("SPI 等待超时!");
			return 0;
		}
	}
	
	return SPI_I2S_ReceiveData(FLASH_SPIx);
}

uint32_t SPI_FLASH_ReadByte(void)
{
	uint32_t ID; 
	
	SPI_FLASH_CS_LOW();
	
	SPI_FLASH_SendByte( READ_JEDEC_ID);
	
	ID =SPI_FLASH_SendByte( DUMMY);
	ID <<= 8;
	
	ID |=SPI_FLASH_SendByte( DUMMY);
	ID <<= 8;
	
	ID |=SPI_FLASH_SendByte( DUMMY);
	
	SPI_FLASH_CS_HIGH();
	
	return ID;
}

void SPI_Write_Enable (void)
{
	
	SPI_FLASH_CS_LOW();
	
	SPI_FLASH_SendByte( WRITE_ENABLE);
	
	SPI_FLASH_CS_HIGH();
	
}



//擦除函数
void SPI_Erase (uint32_t addr)
{
	
	SPI_Write_Enable();
	
	SPI_FLASH_CS_LOW();
	
	SPI_FLASH_SendByte( ERASE_SECTRO);
	
	SPI_FLASH_SendByte( (addr>>16)&0xff);
	
	SPI_FLASH_SendByte( (addr>>8)&0xff);
	
	SPI_FLASH_SendByte(addr&0xff);
	
	SPI_FLASH_CS_HIGH();
	
}

void SPI_Wite_Erase(void)
{
	uint8_t status=0;
	
	SPI_FLASH_CS_LOW();
	
	SPI_FLASH_SendByte( STATUS);
	do
	{
	status=SPI_FLASH_SendByte( DUMMY);
	}while((status&0x01)==1);//忙碌
	
	SPI_FLASH_CS_HIGH();
	
}

//读取flash内容
void SPI_Read_Data(uint32_t addr , uint8_t *buff,uint32_t num)
{
	SPI_FLASH_CS_LOW();
	
	SPI_FLASH_SendByte( READ_DATA);
	
	SPI_FLASH_SendByte( (addr>>16)&0xff);
	
	SPI_FLASH_SendByte( (addr>>8)&0xff);
	
	SPI_FLASH_SendByte(addr&0xff);
	
	while(num--)
	{
		*buff=SPI_FLASH_SendByte( DUMMY);
		buff++;
	}
	
	SPI_FLASH_CS_HIGH();
}

//void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
//{
//  /* 选择FLASH: CS低电平 */
//  SPI_FLASH_CS_LOW();

//  /* 发送 读 指令 */
//  SPI_FLASH_SendByte(READ_DATA);

//  /* 发送 读 地址高位 */
//  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
//  /* 发送 读 地址中位 */
//  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
//  /* 发送 读 地址低位 */
//  SPI_FLASH_SendByte(ReadAddr & 0xFF);
//	
//	/* 读取数据 */
//  while (NumByteToRead--) /* while there is data to be read */
//  {
//    /* 读取一个字节*/
//    *pBuffer = SPI_FLASH_SendByte(Dummy_Byte);
//    /* 指向下一个字节缓冲区 */
//    pBuffer++;
//  }

//  /* 停止信号 FLASH: CS 高电平 */
//  SPI_FLASH_CS_HIGH();
//}

/**
  * @brief  读取FLASH数据
  * @param 	pBuffer，存储读出数据的指针
  * @param   ReadAddr，读取地址
  * @param   NumByteToRead，读取数据长度
  * @retval 无
  */
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
  /* 选择FLASH: CS低电平 */
  SPI_FLASH_CS_LOW();

  /* 发送 读 指令 */
  SPI_FLASH_SendByte(READ_DATA);

  /* 发送 读 地址高位 */
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /* 发送 读 地址中位 */
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /* 发送 读 地址低位 */
  SPI_FLASH_SendByte(ReadAddr & 0xFF);
	
	/* 读取数据 */
  while (NumByteToRead--) /* while there is data to be read */
  {
    /* 读取一个字节*/
    *pBuffer = SPI_FLASH_SendByte(Dummy_Byte);
    /* 指向下一个字节缓冲区 */
    pBuffer++;
  }

  /* 停止信号 FLASH: CS 高电平 */
  SPI_FLASH_CS_HIGH();
}



