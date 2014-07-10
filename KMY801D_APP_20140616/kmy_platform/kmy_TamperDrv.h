#ifndef __KMY_TAMPERDRV_H
#define __KMY_TAMPERDRV_H




#define TAMPER_TRIGGERED	0x01
#define TAMPER_NO_TRIGGER	0x02



void kmy_BackUpInit(void);
//函数功能： 防拆机功能初始化
//函数参数：无
//返回值：无
//注意事项：无



unsigned char kmy_CheckTamper(void);
//函数功能：拆机检查
//函数参数：无
//返回值：
//	拆过机返回TAMPER_TRIGGERED
//	没拆机返回TAMPER_NO_TRIGGER
//注意事项：
//	1. 执行此函数一次后会清除拆机标志



void kmy_ClearTamper(void);
//函数功能：清除拆机标志
//函数参数：无
//返回值：无
//注意事项：无



#endif
