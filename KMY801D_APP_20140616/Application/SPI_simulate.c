
#include "SPI_simulate.h"




#include "stm32f10x.h"



// 25P16 2Mbytes 串行flash特点
// 1. 大小2Mbytes, 32个扇区，每个扇区64Kbytes
// 2. 擦除只能整片擦除或者按扇区擦除。
// 3. 读可以任意地址顺序读。
// 4. 写可以按页写，1页256bytes ，总共8192页，写之前必须擦除。


/*#define uint8	unsigned char
#define uint16	unsigned short
#define uint32	unsigned int
#define SO_0 GPIO_ResetBits(GPIOA, GPIO_Pin_7)
#define SO_1 GPIO_SetBits(GPIOA, GPIO_Pin_7)
#define SCLK_0 GPIO_ResetBits(GPIOA, GPIO_Pin_5)
#define SCLK_1 GPIO_SetBits(GPIOA, GPIO_Pin_5)
#define CS_0  GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define CS_1 GPIO_SetBits(GPIOA, GPIO_Pin_4)
#define in_SI (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)==1)
*/

//GPIOA define
#define FLASH_PORT			GPIOA
#define FLASH_SPI 			SPI1
#define FLASH_CS_PIN		GPIO_Pin_4
#define FLASH_SCK_PIN		GPIO_Pin_5
#define FLASH_MISO_PIN		GPIO_Pin_6
#define FLASH_MOSI_PIN		GPIO_Pin_7

/**
* @brief  M25P SPI Flash supported commands
*/  
#define FLASH_CMD_WRITE          0x02  /*!< Write to Memory instruction */
#define FLASH_CMD_WRSR           0x01  /*!< Write Status Register instruction */
#define FLASH_CMD_WREN           0x06  /*!< Write enable instruction */
#define FLASH_CMD_READ           0x03  /*!< Read from Memory instruction */
#define FLASH_CMD_RDSR           0x05  /*!< Read Status Register instruction  */
#define FLASH_CMD_RDID           0x9F  /*!< Read identification */
#define FLASH_CMD_SE             0xD8  /*!< Sector Erase instruction */
#define FLASH_CMD_BE             0xC7  /*!< Bulk Erase instruction */
#define FLASH_WIP_FLAG           0x01  /*!< Write In Progress (WIP) flag */

#define FLASH_DUMMY_BYTE         0xA5
#define FLASH_PAGESIZE			 0x100

#define FLASH_M25P16_ID          0x202015


#define FLASH_CS_LOW()       GPIO_ResetBits(FLASH_PORT, FLASH_CS_PIN)
#define FLASH_CS_HIGH()      GPIO_SetBits(FLASH_PORT, FLASH_CS_PIN)

#define pin_SpiCs_SetLow()		GPIO_ResetBits(GPIOA, GPIO_Pin_4)
#define pin_SpiCs_SetHigh()		GPIO_SetBits(GPIOA, GPIO_Pin_4)

#define pin_SpiSck_SetLow()		GPIO_ResetBits(GPIOA, GPIO_Pin_5)
#define pin_SpiSck_SetHigh()	GPIO_SetBits(GPIOA, GPIO_Pin_5)

#define pin_SpiMosi_SetLow()	GPIO_ResetBits(GPIOA, GPIO_Pin_7)
#define pin_SpiMosi_SetHigh()	GPIO_SetBits(GPIOA, GPIO_Pin_7)

/*#define FLASH_CS_LOW()       GPIO_ResetBits(FLASH_PORT, FLASH_CS_PIN);usleep(1)
#define FLASH_CS_HIGH()      GPIO_SetBits(FLASH_PORT, FLASH_CS_PIN);usleep(1)

#define pin_SpiCs_SetLow()		GPIO_ResetBits(GPIOA, GPIO_Pin_4);usleep(1)
#define pin_SpiCs_SetHigh()		GPIO_SetBits(GPIOA, GPIO_Pin_4);usleep(1)

#define pin_SpiSck_SetLow()		GPIO_ResetBits(GPIOA, GPIO_Pin_5);usleep(1)
#define pin_SpiSck_SetHigh()	GPIO_SetBits(GPIOA, GPIO_Pin_5);usleep(1)

#define pin_SpiMosi_SetLow()	GPIO_ResetBits(GPIOA, GPIO_Pin_7);usleep(1)
#define pin_SpiMosi_SetHigh()	GPIO_SetBits(GPIOA, GPIO_Pin_7);usleep(1)
*/

#define pin_SpiMISO_IsLow()		(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)==0)
#define pin_SpiMISO_IsHigh()	(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6)==1)


void spi_Write(unsigned char spi_dat)  
{
	unsigned  char  i;

	//pin_SpiCs_SetLow();

	for(i=0;i<8;i++){
		pin_SpiSck_SetLow();
		
		if(spi_dat & 0x80){pin_SpiMosi_SetHigh();}
		else{pin_SpiMosi_SetLow();}

		pin_SpiSck_SetHigh();

		spi_dat<<=1;
	}

	//FLASH_CS_HIGH();	发现添加这条语句会导致读不出25p16中的数据
}

unsigned char spi_Read()  
{
	unsigned char i,spi_dat;

	//pin_SpiCs_SetLow();

	for (i=0;i<8;i++){                         
		pin_SpiSck_SetLow();
		pin_SpiSck_SetHigh();
		spi_dat<<=1;
		if(pin_SpiMISO_IsHigh())spi_dat++;
	}

	//pin_SpiCs_SetHigh();

	return spi_dat;  
}


u8 spi_WriteAndRead(unsigned char spi_dat)  
{
	unsigned  char  i;
	u8 rec;

	//pin_SpiCs_SetLow();

	for(i=0;i<8;i++){
		pin_SpiSck_SetLow();

		if((spi_dat & 0x80)==0x80){pin_SpiMosi_SetHigh();}
		else{pin_SpiMosi_SetLow();}

		pin_SpiSck_SetHigh();

		rec<<=1;
		if(pin_SpiMISO_IsHigh())rec++;

		spi_dat<<=1;
	}

	//pin_SpiCs_SetHigh();

	return rec;
}


//================================API=====================================

static void kmy_FlashWriteEnable(void)
{
	FLASH_CS_LOW();
	spi_Write(FLASH_CMD_WREN);
	FLASH_CS_HIGH();
}

static void kmy_FlashWaitForWriteEnd(void)
{
	unsigned char flashstatus = 0;

	FLASH_CS_LOW();

	spi_Write(FLASH_CMD_RDSR);
	do{
		flashstatus = spi_Read();
	}
	while ((flashstatus & FLASH_WIP_FLAG) == SET);

	FLASH_CS_HIGH();
}

void kmy_FlashInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	FLASH_CS_HIGH();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = FLASH_SCK_PIN|FLASH_CS_PIN|FLASH_MOSI_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(FLASH_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = FLASH_MISO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
	GPIO_Init(FLASH_PORT, &GPIO_InitStructure);

	FLASH_CS_LOW();
	spi_Write(FLASH_CMD_WRSR);
	spi_Write(0x02);
	FLASH_CS_HIGH();
}

void kmy_FlashEraseSector(uint32_t SectorAddr)
{
	kmy_FlashWriteEnable();

	FLASH_CS_LOW();

	spi_Write(FLASH_CMD_SE);
	spi_Write((SectorAddr & 0xFF0000) >> 16);
	spi_Write((SectorAddr & 0xFF00) >> 8);
	spi_Write(SectorAddr & 0xFF);

	FLASH_CS_HIGH();

	kmy_FlashWaitForWriteEnd();
}

void kmy_FlashEraseBulk(void)
{
	kmy_FlashWriteEnable();

	FLASH_CS_LOW();
	spi_Write(FLASH_CMD_BE);
	FLASH_CS_HIGH();

	kmy_FlashWaitForWriteEnd();
}

void kmy_FlashReadBuffer(unsigned char* pBuffer, unsigned int ReadAddr, unsigned int NumByteToRead)
{
	FLASH_CS_LOW();

	spi_Write(FLASH_CMD_READ);
	spi_Write((ReadAddr & 0xFF0000) >> 16);
	spi_Write((ReadAddr& 0xFF00) >> 8);
	spi_Write(ReadAddr & 0xFF);

	while (NumByteToRead--){
		*pBuffer = spi_Read();
		pBuffer++;
		ReadAddr++;
	}

	FLASH_CS_HIGH();
}

void kmy_FlashWritePage(const unsigned char* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	kmy_FlashWriteEnable();

	FLASH_CS_LOW();
	spi_Write(FLASH_CMD_WRITE);
	spi_Write((WriteAddr & 0xFF0000) >> 16);
	spi_Write((WriteAddr & 0xFF00) >> 8);
	spi_Write(WriteAddr & 0xFF);

	while (NumByteToWrite--){
		spi_Write(*pBuffer);
		pBuffer++;
	}

	FLASH_CS_HIGH();

	kmy_FlashWaitForWriteEnd();
}


void kmy_FlashWriteBuffer(const unsigned char* pBuffer, uint32_t WriteAddr, uint16_t NumByteToWrite)
{
	uint8_t NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;

	Addr = WriteAddr % FLASH_PAGESIZE;
	count = FLASH_PAGESIZE - Addr;
	NumOfPage =  NumByteToWrite / FLASH_PAGESIZE;
	NumOfSingle = NumByteToWrite % FLASH_PAGESIZE;

	if (Addr == 0) 	/*!< WriteAddr is sFLASH_PAGESIZE aligned  */
	{
		if (NumOfPage == 0) 		/*!< NumByteToWrite < sFLASH_PAGESIZE */
		{
			kmy_FlashWritePage(pBuffer, WriteAddr, NumByteToWrite);
		}
		else 		/*!< NumByteToWrite > sFLASH_PAGESIZE */
		{
			while (NumOfPage--)
			{
				kmy_FlashWritePage(pBuffer, WriteAddr, FLASH_PAGESIZE);
				WriteAddr +=  FLASH_PAGESIZE;
				pBuffer += FLASH_PAGESIZE;
			}

			kmy_FlashWritePage(pBuffer, WriteAddr, NumOfSingle);
		}
	}
	else 	/*!< WriteAddr is not sFLASH_PAGESIZE aligned  */
	{
		if (NumOfPage == 0) 		/*!< NumByteToWrite < sFLASH_PAGESIZE */
		{
			if (NumOfSingle > count) 			/*!< (NumByteToWrite + WriteAddr) > sFLASH_PAGESIZE */
			{
				temp = NumOfSingle - count;

				kmy_FlashWritePage(pBuffer, WriteAddr, count);
				WriteAddr +=  count;
				pBuffer += count;

				kmy_FlashWritePage(pBuffer, WriteAddr, temp);
			}
			else
			{
				kmy_FlashWritePage(pBuffer, WriteAddr, NumByteToWrite);
			}
		}
		else 		/*!< NumByteToWrite > sFLASH_PAGESIZE */
		{
			NumByteToWrite -= count;
			NumOfPage =  NumByteToWrite / FLASH_PAGESIZE;
			NumOfSingle = NumByteToWrite % FLASH_PAGESIZE;

			kmy_FlashWritePage(pBuffer, WriteAddr, count);
			WriteAddr +=  count;
			pBuffer += count;

			while (NumOfPage--)
			{
				kmy_FlashWritePage(pBuffer, WriteAddr, FLASH_PAGESIZE);
				WriteAddr +=  FLASH_PAGESIZE;
				pBuffer += FLASH_PAGESIZE;
			}

			if (NumOfSingle != 0)
			{
				kmy_FlashWritePage(pBuffer, WriteAddr, NumOfSingle);
			}
		}
	}
}



