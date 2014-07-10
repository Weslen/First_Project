
//����ļ�ϵͳ��˼����������:



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

#define SPI_FLASH_MAX_NAME_LENGTH		11 //eeprom �ļ�������ֽ���



extern const u8 InitKong[64];
extern const u8 Buff0xff[64];



//buff:Ҫ�������ݴ�ŵ�buff��M24128 ÿҳ64byte��M2416,M2432,M2464��32byte
//page: ���ڼ�ҳ(128*1024/8)/64=256ҳ���ܹ�256ҳ��������page���EEPROM_TOTALPAGE��
u8 SF_readPage(u8 *buff,u16 page)
{
	//return kmy_SPIReadSequent(page*64,buff,64);
	return 0;
}
//buff:Ҫ�������ݴ�ŵ�buff��M24128 ÿҳ64byte��16,32,64��32byte
//page: ���ڼ�ҳ(128*1024/8)/64=256ҳ���ܹ�256ҳ��������page���EEPROM_TOTALPAGE��
u8 SF_WritePage(uc8 *buff,u16 page)
{
	//return kmy_SPIWritePage(page*64,buff,64);
	return 0;
}


//filename: �ļ���
//����ֵ: 
//�д��ļ�����SF_HaveThisFile
//û������ļ�����SF_HaveNotThisFile
//�ļ����ַ�����>11byte,����SF_FileNameToLong
u8 SF_CheckFileExist(uc8 *name)
{
	return SF_FileNameToLong;
}


u8 SF_GetFileStruct(SF_FILE *fp)
{
	return SF_FileNameToLong;
}

//-----------------------------------------��һ���װAPI------------------------------------------

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

//-----------------------------------------�ڶ����װAPI------------------------------------------
//buff: Ҫд��Դbuff
//wLen: Ҫд�ĳ���
//filename: �ļ���
//д�ɹ�����SF_WriteOk��
//���ɹ����صĺ궨��������:
//SF_FileNameToLong---->�����ļ���̫��
//SF_Error_FilesMax20---->�����Ѿ���������20���ļ�������
//SF_Error_AreaNotEnough---->����û���㹻�Ŀռ��ŵ�ǰ���ļ�
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


//����buff :ΪҪ�����ļ��Ļ�����
//����rLen:ΪҪ�����ļ�����
//����filename: �ļ���
//���ɹ�����SF_ReadOk��
//���ɹ����صĺ궨��������:
//SF_FileNameToLong---->�����ļ���̫��
//SF_HaveNotThisFile---->�����ļ�������
//SF_Error_NoContext---->�����ļ�û������
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


//	ɾ���ɹ�����SF_DeleteOk
//	û������ļ�����SF_HaveNotThisFile
//�������ܣ�ɾ���ļ�
//����������
//		1. Name: Ҫɾ�����ļ���
//����ֵ��
//		1. ɾ���ɹ�����SF_fdeleteOK
//		2. û������ļ�����SF_HaveNotThisFile
//		3. ɾ�����ɹ���������ֵ
//ע�������
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

