#ifndef __KMY_BATTARTDRV_H
#define __KMY_BATTARTDRV_H



typedef enum
{ ChargeStatus_Charge = 1,
	ChargeStatus_DisCharge,
}CHARGESTATUS;


typedef enum
{ AcStatus_Plug = 1,
	AcStatus_UnPlug,
}ACSTATUS;



void kmy_BatInit(void);
//函数功能：电池 初始化
//函数参数：无
//返回值：无
//注意事项：无



unsigned char kmy_BatGetChargeStatus(void);
//函数功能：得到充电状态
//函数参数：无
//返回值：
//	在充电返回(宏定义)ChargeStatus_Charge
//	不在充电返回(宏定义)ChargeStatus_DisCharge
//注意事项：无



unsigned char kmy_BatGetACStatus(void);
//函数功能：得到外接电源是否插入状态
//函数参数：无
//返回值：
//	外接电源插入返回(宏定义)AcStatus_Plug
//	外接电源未插入返回(宏定义)AcStatus_UnPlug
//注意事项：无



#endif
