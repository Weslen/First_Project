#ifndef __KMY_KEYDRV_H
#define __KMY_KEYDRV_H


//key define
#define KEY1				0x02
#define KEY2				0x01
#define KEY3				0x06
#define KEY4				0x05
#define KEY5				0x04
#define KEY6				0x03
#define KEY7				0x08
#define KEY8				0x0c
#define KEY9				0x07
#define KEY0				0x0a
#define KEYXING				0x0b
#define KEYJING				0x09

#define KEY_UP				0x18
#define KEY_DOWN			0x19
#define KEY_LEFT			0x17
#define KEY_RIGHT			0x16

#define KEY_L1				0x10
#define KEY_L2				0x0f

#define KEY_R1				0x15
#define KEY_R2				0x0d
#define KEY_R3				0x14
#define KEY_R4				0x13
#define KEY_R5				0x1c
#define KEY_R6				0x1e


#define KEY_pseudo					255			//pseudo key
#define KEY_WaitTimeout				254			//wait timeout pseudo key
#define KEY_NONE					0
#define KEY_ReturnPrevious			KEY_L1		//return previous key
#define KEY_ReturnInitInterface		KEY_R2		//return init interface key
#define KEY_Enter					KEY_R1		//Enter key
#define KEY_DEL						KEY_L2		//delete key
#define KEY_F1						KEY_R3
#define KEY_F2						KEY_R4
#define KEY_F3						KEY_R5
#define KEY_F4						KEY_R6
#define KEY_HandsFree				KEY_R6






void kmy_KeyInit(void);
//函数功能：key 初始化
//函数参数：无
//返回值：无
//注意事项：无


unsigned char kmy_GetKeyValue(void);
//函数功能：取按键值
//函数参数：无
//返回值：
//	上面按键宏定义
//注意事项：无


void kmy_AutoOffLcdLight(unsigned long seconds);
//函数功能：设置多少秒后关闭lcd 的背光
//函数参数：
//	seconds: 时间的秒数
//返回值：无
//注意事项：
// 	1 如果seconds==0取消设置的秒数
// 	2 如果没到时间重新设置这个秒数
// 	之前设置的秒数被取消，以本次的
// 	的秒数重新计算


void kmy_AutoPostKey(unsigned long seconds);
//函数功能：设置多少秒后自动产生KEY_WaitTimeout
//函数参数：
//	seconds: 时间的秒数
//返回值：无
//注意事项：
// 	1 如果seconds==0取消设置的秒数
// 	2 如果没到时间重新设置这个秒数
// 	之前设置的秒数被取消，以本次的
// 	的秒数重新计算
// 	3 当过完这个秒数后，用kmy_GetKeyValue()函数
// 	读按键，马上会得到KEY_WaitTimeout 按键值


void kmy_PostKey(unsigned char key);
//函数功能：直接置键值
//函数参数：
//	key: 按键值，见此文件的宏定义
//返回值：无
//注意事项：
//	1 当执行这个函数之后，用kmy_GetKeyValue()函数
// 	读按键，马上会得到形参key 的 按键值
// 	2 这个函数在一些特殊的场合用，一般用户很少用到



void kmy_EnterPowerDownMode(void);
//函数功能：进入低功耗的停止模式
//函数参数：无
//返回值：无
//注意事项：
//	进入此函数后屏幕会无任何显示
//	按任意键退出低功耗的停止模式


#endif
