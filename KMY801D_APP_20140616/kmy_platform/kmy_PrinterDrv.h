#ifndef __KMY_PRINTERDRV_H
#define __KMY_PRINTERDRV_H



#define Printer_OK							1
#define Printer_NoPaper						2
#define Printer_TooHot						3
#define Printer_InitError					        4
#define Printer_InitOK						        5
#define Printer_15sNoResponse				6	//打印字符串时15s内打印机未作出缺纸检查响应



extern const unsigned char WidthZoom1[3];
extern const unsigned char WidthZoom2[3];
extern const unsigned char WidthZoom3[3];
extern const unsigned char WidthZoom4[3];
extern const unsigned char WidthZoom5[3];
//const unsigned char WidthZoom0[3]={"\x1B\x55\x00"};

extern const unsigned char HighZoom1[3];
extern const unsigned char HighZoom2[3];
extern const unsigned char HighZoom3[3];
extern const unsigned char HighZoom4[3];
extern const unsigned char HighZoom5[3];



unsigned char kmy_PrinterInit(void);
//函数功能：打印机初始化
//函数参数：无
//返回值：
//	初始化正常返回Printer_InitOK
//	初始化异常返回Printer_InitError
//注意事项：无


char kmy_PrinterSetcommand(const unsigned char * command,unsigned char len);
//函数功能：打印机设置命令
//函数参数：
//	command:  命令字符串
//	len: 命令长度
//返回值：
//	设置成功返回(宏定义)Printer_OK
//注意事项：无


char kmy_PrinterString(const unsigned char * ch);
//函数功能：打印机打印字符串
//函数参数：
//	ch:  字符串
//返回值：
//	打印完成返回(宏定义)Printer_OK
//	缺纸返回(宏定义)Printer_NoPaper
//	打印头太热返回(宏定义)Printer_TooHot
//	15s 未收到缺纸检测响应返回(宏定义)Printer_15sNoResponse
//注意事项：
//	1. ch缓冲区字符串长度尽量小于2048bytes



char kmy_PrinterBitmap(const unsigned char *p,unsigned int width, unsigned int height,unsigned int LeftMarginNum);
//函数功能：打印机打印位图
//函数参数：
//	p:  位图数据缓冲区首地址
//	width:  位图宽度
//	height:  位图高度
//	LeftMarginNum:  位图从左边哪点开始打起
//返回值：
//	打印完成返回(宏定义)Printer_OK
//	缺纸返回(宏定义)Printer_NoPaper
//	打印头太热返回(宏定义)Printer_TooHot
//注意事项：
//	位图格式为:单色点阵液晶字模，纵向取模，字节倒序
//	LeftMarginNum+width的值应该小于384；
//	位图最大的宽度为384



#endif
