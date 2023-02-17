#ifndef FLASH_H
#define FLASH_H
#include <stdio.h>
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_spi.h"
#include "stm32f4xx_rcc.h"
#include "type.h"

/* Base address of the Flash sectors */ 
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base address of Sector 0, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base address of Sector 1, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base address of Sector 2, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base address of Sector 3, 16 Kbytes   */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base address of Sector 4, 64 Kbytes   */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base address of Sector 5, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base address of Sector 6, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base address of Sector 7, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base address of Sector 8, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base address of Sector 9, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base address of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base address of Sector 11, 128 Kbytes */

#define DATA_32                 ((uint32_t)0x12345678)

#define  FLASH_SPI_PORT            GPIOB
#define  FLASH_CS_GPIO_PORT        GPIOB
#define  FLASH_SPI_SCK_SOURCE      GPIO_PinSource3
#define  FLASH_SPI_MISO_SOURCE     GPIO_PinSource4
#define  FLASH_SPI_MOSI_SOURCE     GPIO_PinSource5
#define  FLASH_SPI_NSS_PIN         GPIO_Pin_4
#define  FLASH_SPI_SCK_PIN         GPIO_Pin_3
#define  FLASH_SPI_MOSI_PIN        GPIO_Pin_5
#define  FLASH_SPI_MISO_PIN        GPIO_Pin_4
#define  FLASH_CS_PIN              GPIO_Pin_0     
#define  FLASH_CS_STATUS           PBout(0)



void Flash_Write(void);

U8 	  Spi1_Read_Send(U8 data);
U8    W25Q16_Read_SR(void);
void  W25Q16_Read_ID(U8 *m_id, U8 *d_id);
void  SpiFlash_Iint(void);
void  W25Q16_Init(void);
void  W25Q16_Read_Data(U32 address, U16 size, U8 *data);
void  W25Q16_Write_Data(U32 address, U16 size, U8 *data);
void  W25Q16_Erase_Sector(U32 address);

#endif
