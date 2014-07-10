
#ifndef SpiFlashFileSystem_h
#define SpiFlashFileSystem_h


#include "stm32f10x.h"

#include <stdint.h>



#define SF_Error_FilesMax20		1
#define SF_CreateOK				2
#define SF_HaveThisFile			3
#define SF_HaveNotThisFile		4
#define SF_OpenOk					5
#define SF_SEEK_CUR				6
#define SF_SEEK_END				7
#define SF_SEEK_SET				8
#define SF_Error_SeekFlag			9
#define SF_SeekOK					10
#define SF_Error_SeekOutofArea	11
#define SF_Error_NoContext		12
#define SF_ReadOk					13
#define SF_WriteOk				14
#define SF_DeleteOk				15
#define SF_Error_AreaNotEnough	16
#define SF_OperateNormal			17
#define SF_FileNameToLong			18
#define SF_GetFileName_NO			19
#define SF_GetFileName_HAVE		20
#define SF_fdeleteOK			21



struct SpiFlash_file{
	unsigned char Attribute;
	unsigned char CreateTime[14];
	unsigned char ChangeTime[14];
	unsigned int  FileStartPage;
	unsigned int  FileSize;
	unsigned char FilenameSize;
	unsigned char Filename;
	unsigned char UserdefineSize;
	unsigned char UserdefineData;
};


struct SpiFlash_directory{
	unsigned char Attribute;
	unsigned char CreateTime[14];
	unsigned char ChangeTime[14];
	unsigned int  DirStartPage;
	unsigned int  DirSize;
	unsigned char DirnameSize;
	unsigned char Dirname;
	unsigned char UserdefineSize;
	unsigned char UserdefineData;
};


struct SpiFlashFile{
	uint8_t Name[12];
	uint32_t Size;
	uint32_t StartPage;

	uint32_t CurPos;	//这个不需要存在eeprom中
};


//typedef struct SF *P_SF_FILE;	//Pointer Spi Flash File
typedef struct SpiFlashFile SF_FILE;		//Spi Flash File




#endif





