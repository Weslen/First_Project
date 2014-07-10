#ifndef __KMY_MISC_H
#define __KMY_MISC_H




void kmy_MiscInit(void);
//函数功能：杂项功能初始化
//函数参数：无
//返回值：无
//注意事项：无


void kmy_MiscGetChipUniqueId(unsigned  char *buff);
//函数功能：得到芯片唯一id
//函数参数：
//	buff: 芯片id存放buff
//返回值：无
//注意事项：请保证buff 大小大于等于25bytes


void kmy_MiscGetRandomNumber(unsigned  char *buff,unsigned int len);
//函数功能：得到随机数
//函数参数：
//	buff: 随机数存放buff
//返回值：无
//注意事项：无



void kmy_MiscGetLibraryVersion(unsigned  char *buff);
//函数功能：取得kmy  驱动库版本号
//函数参数：
//	buff: kmy  驱动库版本号存放buff
//返回值：无
//注意事项：请保证buff 大小大于等于9bytes




#endif
