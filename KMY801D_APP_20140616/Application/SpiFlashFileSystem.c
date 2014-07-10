
//这个文件系统的思想是这样的:



#include "SpiFlashFileSystem.h"


#include "kmy_FlashDrv.h"
#include <string.h>
#include <stdio.h>



#define SPI_PAGE_BLACLK				0xffffffff
#define SPI_PAGE_SUPER_PAGE			0x00000000
#define SPI_PAGE_BROKEN_PAGE		0x00000001
#define SPI_PAGE_FILE_START_PAGE	0x00000002
#define SPI_PAGE_FILE_MIDDLE_PAGE	0x00000003
#define SPI_PAGE_FILE_END_PAGE		0x00000004
#define SPI_PAGE_DIR_START_PAGE		0x00000005
#define SPI_PAGE_DIR_MIDDLE_PAGE	0x00000006
#define SPI_PAGE_DIR_END_PAGE		0x00000007

#define SPI_FLASH_PAGE_SIZE				(256)
#define SPI_FLASH_SECTOR_SIZE			(64*1024)
#define SPI_FLASH_TOTAL_SIZE			(2*1024*1024)

#define SPI_FLASH_TOTAL_PAGE			(SPI_FLASH_TOTAL_SIZE/SPI_FLASH_PAGE_SIZE)
#define SPI_FLASH_TOTAL_SECTOR			(SPI_FLASH_TOTAL_SIZE/SPI_FLASH_SECTOR_SIZE)

#define SPI_FLASH_MAX_NAME_LENGTH		11 //eeprom 文件名最大字节数



extern const u8 InitKong[64];
extern const u8 Buff0xff[64];



//buff:要读出数据存放的buff，M24128 每页64byte，M2416,M2432,M2464的32byte
//page: 读第几页(128*1024/8)/64=256页，总共256页所以这里page最大EEPROM_TOTALPAGE。
u8 SF_readPage(u8 *buff,u16 page)
{
	//return kmy_SPIReadSequent(page*64,buff,64);
	return 0;
}
//buff:要读出数据存放的buff，M24128 每页64byte，16,32,64的32byte
//page: 读第几页(128*1024/8)/64=256页，总共256页所以这里page最大EEPROM_TOTALPAGE。
u8 SF_WritePage(uc8 *buff,u16 page)
{
	//return kmy_SPIWritePage(page*64,buff,64);
	return 0;
}


//filename: 文件名
//返回值: 
//有此文件返回SF_HaveThisFile
//没有这个文件返回SF_HaveNotThisFile
//文件名字符长度>11byte,返回SF_FileNameToLong
u8 SF_CheckFileExist(uc8 *name)
{
	return SF_FileNameToLong;
}


u8 SF_GetFileStruct(SF_FILE *fp)
{
	return SF_FileNameToLong;
}

//-----------------------------------------第一层封装API------------------------------------------

//FILE * fopen(const char * path,const char * mode);
u8 SF_fopen(uc8 Name[12],SF_FILE *fp)
{
	u8 retval;
	//u8 TheFirstIdlePosition;

	if(strlen((const char*)Name)>SPI_FLASH_MAX_NAME_LENGTH)return SF_FileNameToLong;

	if(SF_CheckFileExist(Name)==SF_HaveNotThisFile){
		return SF_HaveNotThisFile;
	}

	fp->CurPos=0;

	retval=SF_GetFileStruct(fp);

	return retval;
}

u8 SF_fcreate(uc8 Name[12],SF_FILE *fp)
{
	

	return SF_CreateOK;
}

u8 SF_fseek(SF_FILE *fp,s32 size,u8 flag)
{
	s16 temp;

	if(flag==SF_SEEK_CUR){
		temp=fp->CurPos+size;
		if(temp>fp->Size||temp<0){
			return SF_Error_SeekOutofArea;
		}else{
			fp->CurPos=temp;
		}
	}
	else if(flag==SF_SEEK_SET){
		temp=size;
		if(temp>fp->Size||temp<0){
			return SF_Error_SeekOutofArea;
		}else{
			fp->CurPos=temp;
		}
	}
	else if(flag==SF_SEEK_END){
		temp=fp->Size+size;
		if(temp>fp->Size||temp<0){
			return SF_Error_SeekOutofArea;
		}else{
			fp->CurPos=temp;
		}
	}
	else{
		return SF_Error_SeekFlag;
	}

	return SF_SeekOK;
}

u8 SF_fwrite(uc8 *buffer, u32 size,  SF_FILE *fp)
{
	return SF_WriteOk;
}

u8 SF_fread(u8 *buffer, u32 size,  SF_FILE *fp)
{
	return SF_ReadOk;
}

u8 SF_fdelete(SF_FILE *fp)
{
	return SF_fdeleteOK;
}

//-----------------------------------------第二层封装API------------------------------------------
//buff: 要写的源buff
//wLen: 要写的长度
//filename: 文件名
//写成功返回SF_WriteOk，
//不成功返回的宏定义有以下:
//SF_FileNameToLong---->代表文件名太长
//SF_Error_FilesMax20---->代表已经到了最多存20个文件的上限
//SF_Error_AreaNotEnough---->代表没有足够的空间存放当前的文件
u8 SF_WriteData(const unsigned char *buff,unsigned int wLen,const unsigned char *filename)
{
	unsigned int retval;
	SF_FILE fp;

	retval=SF_fopen(filename,&fp);
	if(retval==SF_FileNameToLong)return retval;

	if(retval==SF_HaveNotThisFile){
		{printf("SF_fopen error\r\n");}

		retval=SF_fcreate(filename,&fp);
		if(retval!=SF_CreateOK){
			{printf("SF_Create error\r\n");}
			return retval;
		}
	}

	retval=SF_fwrite(buff,wLen,&fp);
	if(retval!=SF_WriteOk){
		{printf("SF_fwrite error\r\n");}
		
	}

	return retval;
}


//参数buff :为要读出文件的缓冲区
//参数rLen:为要读出文件长度
//参数filename: 文件名
//读成功返回SF_ReadOk，
//不成功返回的宏定义有以下:
//SF_FileNameToLong---->代表文件名太长
//SF_HaveNotThisFile---->代表文件不存在
//SF_Error_NoContext---->代表文件没有内容
u8 SF_ReadData(unsigned char *buff,unsigned int rLen,const unsigned char *filename)
{
	u8 retval;
	SF_FILE fp;

	retval=SF_fopen(filename,&fp);
	if(retval!=SF_OpenOk){
		{printf("SF_fopen error\r\n");}
		return retval;
	}

	retval=SF_fread(buff,rLen,&fp);
	if(retval!=SF_ReadOk){
		{printf("SF_read error\r\n");}
	}

	return retval;
}


//	删除成功返回SF_DeleteOk
//	没有这个文件返回SF_HaveNotThisFile
//函数功能：删除文件
//函数参数：
//		1. Name: 要删除的文件名
//返回值：
//		1. 删除成功返回SF_fdeleteOK
//		2. 没有这个文件返回SF_HaveNotThisFile
//		3. 删除不成功返回其它值
//注意事项：无
u8 SF_Delete(uc8 *Name)
{
	u8 retval;
	SF_FILE fp;

	retval=SF_CheckFileExist(Name);
	if(retval==SF_HaveNotThisFile)return retval;

	retval=SF_fopen(Name,&fp);
	if(retval!=SF_OpenOk)return retval;

	retval=SF_fdelete(&fp);
	return retval;
}



u8 SF_GetCurrentPathFileName(u8 *NameBuff,u16 NameBuffSize,u16 FilesIndex)
{
	return SF_GetFileName_NO;
}

