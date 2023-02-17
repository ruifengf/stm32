#include  "stm32f4xx_flash.h"
#include  "flash.h"
#include  "usart.h"	
#include  "delay.h"
#include "includes.h"


static U8  W25Q16_Send_Byte(U8 byte)
{	
	U8 data = 0;
	int32_t i;
	U8 count = 0;

	for(i=7; i>=0; i--)
	{

		if(byte & (1<<i))
		{
			PBout(5) = 1;
		}
		else
		{
			PBout(5) = 0;
		}
		
		PBout(3) = 1;                          
		Delay_us(2);
		if(PBin(4))
		{
			data |= (1 << i);
		}
		else
		{
			count++;
		}
		PBout(3) = 0;
		Delay_us(2);

	}	
	return data;
	  
}
void Flash_Write(void)
{
	uint32_t  uwData32;
	FLASH_Unlock();

	/* Clear pending flags (if any) */  
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
	                FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR); 
	if (FLASH_EraseSector(FLASH_Sector_1, VoltageRange_3) != FLASH_COMPLETE)
	{
		printf("FLASH_Sector_1 erase error\n");
	}

	if (FLASH_ProgramWord(ADDR_FLASH_SECTOR_1, DATA_32) != FLASH_COMPLETE)
	{
		printf("FLASH_Sector_1 write error\n");
	}
	uwData32 = *(__IO uint32_t*)ADDR_FLASH_SECTOR_1;
	FLASH_Lock(); 
	uwData32 = *(__IO uint32_t*)ADDR_FLASH_SECTOR_1;
	if(DATA_32 == uwData32)
	{
		printf("write success\n");
	}
	else
	{
		printf("write error\n");
	}
	Delay_ms(1000);
}


void SpiFlash_Iint(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
  
    /*!< Enable GPIO clocks */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	/*!< Enable the SPI clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    
    /*!< SPI pins configuration *************************************************/
    GPIO_InitStructure.GPIO_Pin = FLASH_SPI_SCK_PIN | FLASH_SPI_MOSI_PIN | FLASH_SPI_MISO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(FLASH_SPI_PORT, &GPIO_InitStructure);
	
    /*!< Connect SPI pins to AF5 */  
    GPIO_PinAFConfig(FLASH_SPI_PORT, FLASH_SPI_SCK_SOURCE, GPIO_AF_SPI1);
    GPIO_PinAFConfig(FLASH_SPI_PORT, FLASH_SPI_MISO_SOURCE, GPIO_AF_SPI1);
    GPIO_PinAFConfig(FLASH_SPI_PORT, FLASH_SPI_MOSI_SOURCE, GPIO_AF_SPI1);
  
    /*!< Configure sFLASH Card CS pin in output pushpull mode ********************/
    GPIO_InitStructure.GPIO_Pin = FLASH_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(FLASH_CS_GPIO_PORT, &GPIO_InitStructure);
    PBout(0) = 1;
  

    
	/*!< SPI configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;					  //主机模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;			      //八位数据位
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;					      //高电平时时钟总线为空闲
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;					  //第二跳变沿进行采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;						  //片选引脚由代码进行控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;//时钟分频，APB2/16
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;				  //高位先出
	//SPI_InitStructure.SPI_CRCPolynomial = 7;						  		
    SPI_Init(SPI1, &SPI_InitStructure);
   
    /*!< Enable the sFLASH_SPI  */
    SPI_Cmd(SPI1, ENABLE);
}


void  W25Q16_Read_ID(U8 *m_id, U8 *d_id)
{
	PBout(0) = 0;
#if  1

	W25Q16_Send_Byte(0x90);
	W25Q16_Send_Byte(0x00);
	W25Q16_Send_Byte(0x00);
	W25Q16_Send_Byte(0x00);

	*m_id = W25Q16_Send_Byte(0x00);
	*d_id = W25Q16_Send_Byte(0x00);
#else
	Spi1_Read_Send(0x90);
	Spi1_Read_Send(0x00);
	Spi1_Read_Send(0x00);
	Spi1_Read_Send(0x00);

	*m_id = Spi1_Read_Send(0x00);
	*d_id = Spi1_Read_Send(0x00);

#endif
	
	PBout(0) = 1;
}

U8  W25Q16_Read_SR(void)
{
	U8 rs;

	FLASH_CS_STATUS = 0;
	W25Q16_Send_Byte(0x05);
	rs = W25Q16_Send_Byte(0xFF);
	FLASH_CS_STATUS = 1;
	return rs;
	
}

void  W25Q16_Read_Data(U32 address, U16 size, U8 *data)
{
	U16 loop;

	FLASH_CS_STATUS = 0;

	W25Q16_Send_Byte(0x03);
	W25Q16_Send_Byte((U8)address>>16);
	W25Q16_Send_Byte((U8)address>>8);
	W25Q16_Send_Byte((U8)address);
	for(loop = 0; loop < size; loop++)
	{
		data[loop] = W25Q16_Send_Byte(0x00);
	}

	FLASH_CS_STATUS = 1;
	
}

void  W25Q16_Write_Data(U32 address, U16 size, U8 *data)
{
	U16 loop;

	FLASH_CS_STATUS = 0;
	W25Q16_Send_Byte(0x06);
	FLASH_CS_STATUS = 1;
	
	FLASH_CS_STATUS = 0;
	W25Q16_Send_Byte(0x02);
	W25Q16_Send_Byte((U8)address>>16);
	W25Q16_Send_Byte((U8)address>>8);
	W25Q16_Send_Byte((U8)address);

	for(loop = 0; loop < size; loop++)
	{
		W25Q16_Send_Byte(data[loop]);
	}
	FLASH_CS_STATUS = 1;
	while((W25Q16_Read_SR()&0x01) == 0x01);

	FLASH_CS_STATUS = 0;
	W25Q16_Send_Byte(0x04);
	FLASH_CS_STATUS = 1;
	
}

void  W25Q16_Erase_Sector(U32 address)
{
	/* Write Enable */
	FLASH_CS_STATUS = 0;
	W25Q16_Send_Byte(0x06);
	FLASH_CS_STATUS = 1;
	FLASH_CS_STATUS = 0;
	W25Q16_Send_Byte(0x20);
	W25Q16_Send_Byte((U8)address>>16);
	W25Q16_Send_Byte((U8)address>>8);
	W25Q16_Send_Byte((U8)address);	
	FLASH_CS_STATUS = 1;

	while((W25Q16_Read_SR()&0x01) == 0x01);
	



}




U8 Spi1_Read_Send(U8 data)
{
	/*!< Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);	
	/*!< Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(SPI1, data);	
	/*!< Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	/*!< Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(SPI1);
}

void  W25Q16_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
  
    /*!< Enable GPIO clocks */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    
    /*!< SPI pins configuration *************************************************/
    GPIO_InitStructure.GPIO_Pin = FLASH_SPI_SCK_PIN | FLASH_SPI_MOSI_PIN| FLASH_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(FLASH_SPI_PORT, &GPIO_InitStructure);	

    /*!< SPI pins configuration *************************************************/
    GPIO_InitStructure.GPIO_Pin = FLASH_SPI_MISO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(FLASH_SPI_PORT, &GPIO_InitStructure);	

	PBout(0) = 1;
	PBout(3) = 0;
	PBout(5) = 1;



}


