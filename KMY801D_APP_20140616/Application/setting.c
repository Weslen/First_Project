
#include "setting.h"

#include "kmy_Time.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "key.h"
#include "lcd.h"
#include "main.h"
#include "global_extern_variable.h"
#include "gprs.h"
#include "MenuOperate.h"
#include "SelfTest.h"
#include "des.h"
#include "mypure_library.h"
#include "ShuRuFa.h"
#include "kmy_EEPROMDrv.h"
#include "EepromFileSystem.h"
#include "kmy_Misc.h"
#include "SyncTime.h"


const u8 efile_Config[]={"Config"};
const u8 Configfile_des3key[25]={"106QDabcdefg106QD1234567"};
u8 dataledflag;


//{"ipaddress","ipaddr",ServerIpPortBuf,"220.162.237.128,65167"},//福建南强
//{"ipaddress","ipaddr",ServerIpPortBuf,"116.255.144.110,9100"},//郑州爱卡
//{"ipaddress","ipaddr",ServerIpPortBuf,"66.249.89.104,80"},//Google
//{"ServerIp","ServerIp",ServerIp,"58.253.213.3,969"},//106HK

unsigned char LightPercent[4];				//light percent
unsigned char ServerIpPortBuf[28];			//ip 地址220.162.237.128,65167
unsigned char InitDes3key[24+1];			//初始3des 密钥
unsigned char ApnApn[21+1];
unsigned char ApnUsername[21+1];
unsigned char ApnPassword[21+1];
unsigned char EnterAdvancedSetPassWord[22];	//高级设置密码
unsigned char StorePIN[5];					//PIN name
unsigned char beepKG[2];
unsigned char ScreenOffTimeout[3];			//PIN name
unsigned char TerminalID[17];				//终端id
unsigned char TerminalPassword[17];			//终端密码
unsigned char TimeZone[15];                             //时区




const unsigned char locallanguage[sizeof(Language)]={OTHER_LANGUAGE,0};

//unsigned char ShangChangpassword[7];		//商场管理员密码
struct storeFile XStor[20]={	//需要保存的变量值
	{beepKG,"1"},
	{LightPercent,"44"},
	{Language,locallanguage},
	{ServerIpPortBuf,"185.28.21.7,80"},
	{InitDes3key,"11111111"},

	{ApnApn,"cmnet"},
	{ApnUsername,""},
	{ApnPassword,""},
	{ScreenOffTimeout,"60"},
	{EnterAdvancedSetPassWord,"000000"},

	{StorePIN,"7890"},
	{TerminalID,"12345678"},
	{TerminalPassword,"123456"},
	{TimeZone,"UTC+08:00"},
	{NULL,NULL},

	{NULL,NULL},
	{NULL,NULL},
	{NULL,NULL},
	{NULL,NULL},
	{NULL,NULL} 	//后面不用的务必赋成&SFNULL，否则会引起segmentationfault
};


u8 StorageVariable(void)
{
	static u8 buff[300];
	u8 i;
	u8 retval;
	struct EepromFile efp;

	buff[0]=0;
	for(i=0;i<STORESIZE;i++){
		strcat((s8*)buff,(sc8*)(XStor[i].Viarable));
		strcat((s8*)buff,"\t");
	}

	retval=EepromFile_Open(efile_Config,&efp);
	if(retval!=EepromFile_OpenOk){
		{printf("[StorageVariable error]=EepromFile_Open\r\n");}
		return 0;
	}

	retval=EepromFile_Write(buff,strlen((sc8*)buff)+1,&efp);
	if(retval!=EepromFile_WriteOk){
		{printf("[StorageVariable error]=EepromFile_Write\r\n");}
		return 0;
	}

	return 1;
}


u8 RestoreConfigFile(void)
{
	u8 buff[1500]={0};
	u8 i;
	u8 retval;
	//struct EepromFile efp;

	buff[0]=0;
	for(i=0;i<STORESIZE;i++){
		strcpy((s8*)(XStor[i].Viarable),(sc8*)(XStor[i].initVal));
		strcat((s8*)buff,(sc8*)(XStor[i].initVal));
		strcat((s8*)buff,"\t");
	}

	retval=EepromFile_Delete(efile_Config);
	if(retval==EepromFile_HaveNotThisFile){
		{printf("EepromFile_HaveNot Config file\r\n");}
		//return 0;
	}

	retval=EepromFile_WriteData(buff,strlen((sc8*)buff)+1,efile_Config);
	if(retval!=EepromFile_WriteOk){
		{printf("[RestoreConfigFile error]=EepromFile_Write\r\n");}
		return 0;
	}

	return 1;
}

u8 RestoreGlobalVariable(void)
{
	u8 buff[1500];
	u8 i;
	u8 retval;
	u8 *p;
	char *pt;
	struct EepromFile efp;

	retval=EepromFile_Open(efile_Config,&efp);

	if (retval != EepromFile_OpenOk)
	{
		{
			printf ("[RestoreGlobalVariable error]=EepromFile_Open\r\n");
		}
		return 0;
	}

	retval = EepromFile_read (buff, sizeof (buff) - 1, &efp);

	if (retval != EepromFile_ReadOk)
	{
		{
			printf ("[RestoreGlobalVariable error]=EepromFile_read\r\n");
		}
		return 0;
	}
	

	pt = (char*)buff;
	for (i = 0; i < STORESIZE; i++)
	{
		pt=strstr(pt,"\t");
		if(pt==NULL)
		{
			printf("Add new global varible, format Eeprom and restart\r\n");
			return 2;
		}
		pt++;
	}
	pt=strstr(pt,"\t");
	if(pt!=NULL)
	{
		printf("Global varible have been changed, format Eeprom and restart\r\n");
		return 2;
	}
	
	p = buff;
		
	for (i = 0; i < STORESIZE; i++)
	{
		my_strcpy_EncounterHexEnd ( (XStor[i].Viarable), p, 300, '\t');
		my_MovePointToHex (&p, &buff[sizeof (buff) - 1], '\t');
		p++;
		printf ("%d=%s\r\n", i, XStor[i].Viarable);
	}

	return 1;

}


static char ContrastSet(char *msg)	//2.screen constract setting
{
	#if 0
	return(abeyant());
	#else
	signed int NewPercent	=atoi((sc8*)LightPercent);
	signed int 	CurCursor	= 39+((NewPercent-56)/4*3);	
	signed int  OldCursor   = CurCursor;
	unsigned char Buf[50]={'\0'};
	unsigned char keyval;

	clear_lcd();
	text_out_to_row_x_Language2(0,28,"【亮度设定】","Contrast Set");

	text_out(17,25,"-");
	text_out(105,25,"+");
	draw_rect(26,25,77,10, mode_OnlyEdge);
	draw_line(26,30,26+75+1,30,1);

	fill_rect(26+1+CurCursor,26,3,8,mode_Reverse);	//3*8

	memset(Buf,'\0',sizeof(Buf));
	sprintf((s8*)Buf,"%d",NewPercent);
	strcat((s8*)Buf,"%");
	text_out(58,48,Buf);

	while(1)
	{
		keyval=GetKeyValue();
		if(keyval == KEY_NONE) continue ;
		KeyEventPost();
		switch(keyval){
			case KEY_UP:
			if(NewPercent+4 <= 100)
			{
				NewPercent += 4;			
				CurCursor += 3;			
			}		
			break;

			case KEY_DOWN:
			if(NewPercent-4 >= 4)
			{
				NewPercent -= 4;
				CurCursor -= 3;			
			}		
			break;

			case KEY_Enter:
			sprintf((s8*)Buf,"%d",NewPercent);
			Buf[3]=0;
			strcpy((s8*)LightPercent,(sc8*)Buf);
			if(!StorageVariable()){
				DrawTitle_to_row_Language2(3,"更改失败!","change failure!");
				WaitkeyAndAutoExit(10);
			}
			return KEY_Enter;

			case KEY_ReturnPrevious:
			NewPercent=atoi((sc8*)LightPercent);
			set_contrast(NewPercent);
			return KEY_ReturnPrevious;

			case KEY_ReturnInitInterface:
			case KEY_WaitTimeout:
			NewPercent=atoi((sc8*)LightPercent);
			set_contrast(NewPercent);
			return KEY_ReturnInitInterface;

			default:
			break;	
		}
		memset(Buf,'\0',sizeof(Buf));
		text_out(58,48,"    ");
		sprintf((s8*)Buf,"%d",NewPercent);
		strcat((s8*)Buf,"%");
		text_out(58,48,Buf);
		KeyIsResponse();

		if(OldCursor != CurCursor)
		{
			fill_rect(26+1+OldCursor,26,3,8,mode_Reverse);	//3*8
			fill_rect(26+1+CurCursor,26,3,8,mode_Reverse);	//3*8
			OldCursor = CurCursor;
		}

		#ifdef DEBUG_PRINTF
		printf("NewPercent=%d\n",NewPercent);
		#endif

		set_contrast(NewPercent);		// 24---52
	}
	#endif
}


static char Polish_Display_Keytone_Close(char *msg)
{
	memset(beepKG,0,sizeof(beepKG));
	beepKG[0]='0';
	if(!StorageVariable()){
		DrawTitle_to_row_Language2(3,"更改失败!","change failure!");
		WaitkeyAndAutoExit(10);
	}

	return 0;
}

static char Polish_Display_Keytone_Open(char *msg)
{
	memset(beepKG,0,sizeof(beepKG));
	beepKG[0]='1';
	if(!StorageVariable()){
		DrawTitle_to_row_Language2(3,"更改失败!","change failure!");
		WaitkeyAndAutoExit(10);
	}

	return 0;
}

static char BasicKeyTone(char *msg)	//key tone setting
{
	static unsigned char ToneClose1[20]={"1.关闭   "};
	static unsigned char ToneClose2[20]={"1.Close  "};
	static unsigned char ToneOpen1[20]={"2.打开   "};
	static unsigned char ToneOpen2[20]={"2.Open   "};

	static struct MenuFuc_Language2 Menu[2]={
		{ToneClose1,ToneClose2,Polish_Display_Keytone_Close},
		{ToneOpen1,ToneOpen2,Polish_Display_Keytone_Open}
	};
	struct MenuOperate_Language2 MenuOp={
		Menu,
		2,
		0,
		0,
		0,
		MaxRowOneScr,
		1,
		0,
		0
	};

	my_strdel(ToneClose1,'*');
	my_strdel(ToneClose2,'*');
	my_strdel(ToneOpen1,'*');
	my_strdel(ToneOpen2,'*');

	if(beepKG[0]=='0'){
		strcat((s8*)ToneClose1,"*");
		strcat((s8*)ToneClose2,"*");
	}else{
		strcat((s8*)ToneOpen1,"*");
		strcat((s8*)ToneOpen2,"*");
	}

	while(1){
		MenuHandler_Language2(&MenuOp);
		if(MenuOp.RetVal==KEY_Enter){
			MenuOp.FucRetVal=MenuOp.Menu->MenuFuc(msg);
			if(MenuOp.FucRetVal==KEY_WaitTimeout||MenuOp.FucRetVal==KEY_ReturnInitInterface){CurProc=CurProc_Return;return MenuOp.FucRetVal;}  
			else{
				clear_lcd();
				MenuOp.flashflag=1;
				my_strdel(ToneClose1,'*');
				my_strdel(ToneClose2,'*');
				my_strdel(ToneOpen1,'*');
				my_strdel(ToneOpen2,'*');
				if(beepKG[0]=='0'){
					strcat((s8*)ToneClose1,"*");
					strcat((s8*)ToneClose2,"*");
				}else{
					strcat((s8*)ToneOpen1,"*");
					strcat((s8*)ToneOpen2,"*");
				}
			}
		}else{
			return MenuOp.RetVal;
		}
	}
}


static char PowerOffScreenTime(char *msg)	//key tone setting
{
	unsigned char temp[20];
	unsigned char retval;
	PasswordInputArgument arg;

	clear_lcd();
	DrawTitle_to_row_Language2(0,"【屏保时间设置】","Timeout set");

	if(Language[0]==LOCAL_LANGUAGE){
		strcpy((s8*)temp,"当前屏保时间:");
	}else{
		strcpy((s8*)temp,"Current time");
	}
	strcat((s8*)temp,(sc8*)ScreenOffTimeout);
	DrawTitle_to_row_Language2(1,temp,temp);

	PassWordArgDeInit(&arg,temp,2);
	arg.DiplayRow=2;
	arg.LocalTitle=NULL;
	arg.xingflag=0;
	arg.clearlcdflag=0;
	retval=PassWordInput_MiddledDisplay(&arg);
	if(retval==KEY_Enter){
		strcpy((s8*)ScreenOffTimeout,(sc8*)temp);
		retval=atoi((sc8*)temp);
		if(retval==0)strcpy((s8*)ScreenOffTimeout,"30");

		if(!StorageVariable()){
			DrawTitle_to_row_Language2(3,"更改失败!","change failure!");
		}else{
			DrawTitle_to_row_Language2(3,"更改成功!","change success!");
		}

		retval=WaitkeyAndAutoExit(10);
	}

	return retval;
}


static char LanguageSet_English(char *msg)
{
	memset(Language,0,sizeof(Language));
	Language[0]=OTHER_LANGUAGE;
	if(!StorageVariable()){
		DrawTitle_to_row_Language2(3,"更改失败!","change failure!");
		WaitkeyAndAutoExit(10);
	}
	return 0;
}
static char LanguageSet_Chinese(char *msg)
{
	memset(Language,0,sizeof(Language));
	Language[0]=LOCAL_LANGUAGE;
	if(!StorageVariable()){
		DrawTitle_to_row_Language2(3,"更改失败!","change failure!");
		WaitkeyAndAutoExit(10);
	}

	return 0;
}
static char LanguageSet(char *msg)	//key tone setting
{
	static unsigned char ToneClose1[20]={"1.中文   "};
	static unsigned char ToneClose2[20]={"1.Chinese  "};
	static unsigned char ToneOpen1[20]= {"2.英文   "};
	static unsigned char ToneOpen2[20]= {"2.English  "};

	static struct MenuFuc_Language2 Menu[2]={
		{ToneClose1,ToneClose2,LanguageSet_Chinese},
		{ToneOpen1,ToneOpen2,LanguageSet_English}
	};
	struct MenuOperate_Language2 MenuOp={
		Menu,2,0,0,0,MaxRowOneScr,1,0,0
	};

	my_strdel(ToneClose1,'*');
	my_strdel(ToneClose2,'*');
	my_strdel(ToneOpen1,'*');
	my_strdel(ToneOpen2,'*');

	if(Language[0]==LOCAL_LANGUAGE){
		strcat((s8*)ToneClose1,"*");
		strcat((s8*)ToneClose2,"*");
	}else{
		strcat((s8*)ToneOpen1,"*");
		strcat((s8*)ToneOpen2,"*");
	}

	while(1){
		MenuHandler_Language2(&MenuOp);
		if(MenuOp.RetVal==KEY_Enter){
			MenuOp.FucRetVal=MenuOp.Menu->MenuFuc(msg);
			if(MenuOp.FucRetVal==KEY_WaitTimeout||MenuOp.FucRetVal==KEY_ReturnInitInterface){CurProc=CurProc_Return;return MenuOp.FucRetVal;}  
			else{
				clear_lcd();
				MenuOp.flashflag=1;
				my_strdel(ToneClose1,'*');
				my_strdel(ToneClose2,'*');
				my_strdel(ToneOpen1,'*');
				my_strdel(ToneOpen2,'*');
				if(Language[0]==LOCAL_LANGUAGE){
					strcat((s8*)ToneClose1,"*");
					strcat((s8*)ToneClose2,"*");
				}else{
					strcat((s8*)ToneOpen1,"*");
					strcat((s8*)ToneOpen2,"*");
				}
			}
		}else{
			return MenuOp.RetVal;
		}
	}
}



static char DisplayVersion(char *msg)
{
	unsigned char buff[30];
	unsigned char buffother[30];

	strcpy((s8*)buff,"软件版本:");
	strcat((s8*)buff,(sc8*)softwareversion);
	strcpy((s8*)buffother,"softVer:");
	strcat((s8*)buffother,(sc8*)softwareversion);

	clear_lcd();
	DrawTitle_to_row_Language2(0,"终端机信息","Machine Ver");
	text_out_to_row_Language2(1,buff,buffother);
	text_out_to_row_Language2(2,"硬件版本:20100525","HardVer:20100525");
	text_out_to_row_Language2(3,"备注:KMY801D","Mark:KMY801D");
	return(WaitkeyAndAutoExit(10));
}



static char DisplayCID(char *msg)
{
	#if 0
	return(abeyant());
	#else
	unsigned char CmdBuff[50];
	unsigned char temp[50];

	//ReadDatasFromtty(fd_ttyS1,CmdBuff,sizeof(CmdBuff),0,50000);
	
	clear_lcd();
	DrawTitle_to_row_Language2(0,"SIM卡CID码","SIM cid");

	GetCCID(temp);

	if(strlen((sc8*)temp)>16){
		memset(CmdBuff,0,30);
		memcpy(CmdBuff,temp,16);
		DrawTitle_to_row(1,CmdBuff);
		DrawTitle_to_row(2,temp+16);
	}else{
		DrawTitle_to_row(1,temp);
	}

	WaitkeyAndAutoExit(10);
	return 0;
	#endif
}

static char Display_IMEI(char *msg)
{
	unsigned char buff[50];

	clear_lcd();
	DrawTitle_to_row(0,"IMEI");

	GetIMEI(buff);

	DrawTitle_to_row(2,buff);
	return(WaitkeyAndAutoExit(10));
}

static char Display_ChipId(char *msg)
{
	unsigned char buff[50];

	clear_lcd();
	DrawTitle_to_row_Language2(0,"芯片id","Chip ID");

	kmy_MiscGetChipUniqueId(buff);

	DrawTitle_to_row(3,&buff[12]);
	buff[12]=0;
	DrawTitle_to_row(2,&buff[0]);
	return(WaitkeyAndAutoExit(30));
}


static char Display_RandomNumber(char *msg)
{
	unsigned char buff[21];

	clear_lcd();
	DrawTitle_to_row_Language2(0,"随机数","Random num");

	kmy_MiscGetRandomNumber(buff,sizeof(buff)-1);

	DrawTitle_to_row(2,buff);
	return(WaitkeyAndAutoExit(30));
}

static char Display_KMY_LibVersion(char *msg)
{
	unsigned char buff[21];

	clear_lcd();
	DrawTitle_to_row_Language2(0,"KMY库版本","KMY LibVersion");

	kmy_MiscGetLibraryVersion(buff);

	DrawTitle_to_row(2,buff);
	return(WaitkeyAndAutoExit(30));
}

static char BasicApnSetApn(char *msg)
{
	unsigned char buff[sizeof(ApnApn)];
	unsigned char retval;

	clear_lcd();
	strcpy((s8*)buff,(sc8*)ApnApn);
	DrawTitle_to_row(0,"Apn is:");
	text_out_to_row(1,buff);
	text_out_to_row(3,"Change: Confirm");
	text_out_to_row(4,"Exit: Return");

	retval=WaitkeyAndAutoExit(10);
	if(retval!=KEY_Enter)return retval;

	retval=GetString_abcABC123PYWuBi_Language2("输入Apn","Input Apn",SRF_abc|SRF_ABC|SRF_123,SRF_abc,buff,sizeof(buff),1);

	if(retval!=KEY_Enter)return retval;

	clear_lcd();
	strcpy((s8*)ApnApn,(sc8*)buff);
	if(!StorageVariable()){
		DrawTitle_to_row(3,"Change falure!");
	}else{
		DrawTitle_to_row(2,"setting,pleas wait...");
		retval=set_apn();
		if(retval!=TCP_OK){
			DrawTitle_to_row(3,"Set APN error");
		}else{
			DrawTitle_to_row(3,"Set APN success");
		}
	}

	return (WaitkeyAndAutoExit(10));
}
static char BasicApnSetUsername(char *msg)
{
	unsigned char buff[sizeof(ApnUsername)];
	unsigned char retval;

	clear_lcd();
	strcpy((s8*)buff,(sc8*)ApnUsername);
	DrawTitle_to_row(0,"ApnUsername is:");
	text_out_to_row(1,buff);
	text_out_to_row(3,"Change: Confirm");
	text_out_to_row(4,"Exit: Return");

	retval=WaitkeyAndAutoExit(10);
	if(retval!=KEY_Enter)return retval;

	retval=GetString_abcABC123PYWuBi_Language2("输入Apn用户名","Input ApnUsername",SRF_abc|SRF_ABC|SRF_123,SRF_abc,buff,sizeof(buff),1);

	if(retval!=KEY_Enter)return retval;

	clear_lcd();
	strcpy((s8*)ApnUsername,(sc8*)buff);
	if(!StorageVariable()){
		DrawTitle_to_row(3,"Change falure!");
	}else{
		DrawTitle_to_row(2,"setting,pleas wait...");
		retval=set_apn();
		if(retval!=TCP_OK){
			DrawTitle_to_row(3,"Set APN error");
		}else{
			DrawTitle_to_row(3,"Set APN success");
		}
	}

	return (WaitkeyAndAutoExit(10));
}
static char BasicApnSetPassword(char *msg)
{
	unsigned char buff[sizeof(ApnPassword)];
	unsigned char retval;

	clear_lcd();
	strcpy((s8*)buff,(sc8*)ApnPassword);
	DrawTitle_to_row(0,"ApnPassword is:");
	text_out_to_row(1,buff);
	text_out_to_row(3,"Change: Confirm");
	text_out_to_row(4,"Exit: Return");

	retval=WaitkeyAndAutoExit(10);
	if(retval!=KEY_Enter)return retval;

	retval=GetString_abcABC123PYWuBi_Language2("I输入Apn密码","Input ApnPassword",SRF_abc|SRF_ABC|SRF_123,SRF_abc,buff,sizeof(buff),1);

	if(retval!=KEY_Enter)return retval;

	clear_lcd();
	strcpy((s8*)ApnPassword,(sc8*)buff);
	if(!StorageVariable()){
		DrawTitle_to_row(3,"Change falure!");
	}else{
		DrawTitle_to_row(2,"setting,pleas wait...");
		retval=set_apn();
		if(retval!=TCP_OK){
			DrawTitle_to_row(3,"Set APN error");
		}else{
			DrawTitle_to_row(3,"Set APN success");
		}
	}

	return (WaitkeyAndAutoExit(10));
}

static char BasicApnSet(char *msg)
{
	static struct MenuFuc_Language2 Menu[3]={
		{"1.APN","1.APN",BasicApnSetApn},
		{"2.APN用户名","2.User",BasicApnSetUsername},
		{"3.APN密码","3.Password",BasicApnSetPassword},
	};

	struct MenuOperate_Language2 MenuOp={
		Menu,
		3,
		0,
		0,
		0,
		MaxRowOneScr,
		1,
		0,
		0
	};

	while(1){
		MenuHandler_Language2(&MenuOp);
		if(MenuOp.RetVal==KEY_Enter){
			MenuOp.FucRetVal=MenuOp.Menu->MenuFuc(msg);
			if(MenuOp.FucRetVal==KEY_ReturnInitInterface){return KEY_ReturnInitInterface;} 
			else{clear_lcd();MenuOp.flashflag=1;}
		}else{
			return MenuOp.RetVal;
		}
	}
}





char ReadSomeMessage(char *msg)
{
	static struct MenuFuc_Language2 Menu[6]={
		{"1.版本信息","1.Version",DisplayVersion},
		{"2.SIM卡CID码","2.SIM card CID",DisplayCID},
		{"3.IMEI号","3.IMEI number",Display_IMEI},
		{"4.读芯片唯一ID","4.Chip unique ID",Display_ChipId},
		{"5.读随机数","5.Random number",Display_RandomNumber},
		{"6.读KMY库版本","6.KMY_LibVersion",Display_KMY_LibVersion},
	};
	struct MenuOperate_Language2 MenuOp={
		Menu,
		6,
		0,
		0,
		0,
		MaxRowOneScr,
		1,
		0,
		0
	};

	while(1){
		MenuHandler_Language2(&MenuOp);
		if(MenuOp.RetVal==KEY_Enter){
			MenuOp.FucRetVal=MenuOp.Menu->MenuFuc(msg);
			if(MenuOp.FucRetVal==KEY_WaitTimeout||MenuOp.FucRetVal==KEY_ReturnInitInterface){CurProc=CurProc_Return;return MenuOp.FucRetVal;}
			else{clear_lcd();MenuOp.flashflag=1;}
		}else{
			CurProc=CurProc_Return;
			return MenuOp.RetVal;
		}
	}
}

unsigned char TranslateKeyToDecimal(unsigned char KeyValue)
{
    switch(KeyValue)
    {
        case KEY0:
            return 0;
         
        case KEY1:
            return 1;
         
        case KEY2:
            return 2;
         
        case KEY3:
            return 3;
         
        case KEY4:
            return 4;
         
        case KEY5:
            return 5;
         
        case KEY6:
            return 6;
         
        case KEY7:
            return 7;
         
        case KEY8:
            return 8;
         
        case KEY9:
            return 9;
         
        default:
            break;
    }
    return 0;
}

unsigned char SetBitOfDecimal(int OldDecimal, unsigned char ValueOfBit, unsigned char bit)
{
    unsigned char NewDecimal=0;
    if(bit == 0)
    {
        NewDecimal = OldDecimal%10;
        NewDecimal += ValueOfBit*10;
    }
    else if(bit == 1)
    {
        NewDecimal = (OldDecimal/10)*10;
        NewDecimal += ValueOfBit;
    }
    return NewDecimal;
}

unsigned char GetBitOfDecimal(unsigned char Decimal, unsigned char bit)
{
    if(bit == 0)
    {
        return ((Decimal/10)%10);
    }
    else if(bit == 1)
    {
        return ((Decimal/1)%10);
    }
    return 0;
}
 
 
 
static char TimeDateSet (char *msg) //1.time data setting
{
    unsigned char FlashFlag = 0x01 | 0x02 | 0x04;
    unsigned int year = 0;
    static unsigned char year_H=0;
    static unsigned char year_L=0;
    static unsigned char month = 0;
    static unsigned char day = 0;
    static unsigned char hour = 0;
    static unsigned char minute = 0;
    static unsigned char second = 0;
    unsigned char sBuff[100]={0};
     
    unsigned char TempKey = KEY_NONE;
    unsigned char Index=0;
    unsigned char MoveFlag=0;
    unsigned char ret=0;
    #define startx 7
    #define Row 2
    #define MaxArray 14
    char YearofMonth[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
     
    unsigned char CursorArray[MaxArray][4] = {
                                              //Min, x, y, Max
                                              {0, startx+6*0, (Row+1)*13-2, 9},
                                              {0, startx+6*1, (Row+1)*13-2, 9}, 
                                              {0, startx+6*2, (Row+1)*13-2, 9},
                                              {0, startx+6*3, (Row+1)*13-2, 9},
                                               
                                              {0, startx+6*5, (Row+1)*13-2, 9},
                                              {0, startx+6*6, (Row+1)*13-2, 9},
                                               
                                              {0, startx+6*8, (Row+1)*13-2, 9},
                                              {0, startx+6*9, (Row+1)*13-2, 9},
                                               
                                              {0, startx+6*11, (Row+1)*13-2, 9},
                                              {0, startx+6*12, (Row+1)*13-2, 9}, //4
                                               
                                              {0, startx+6*14, (Row+1)*13-2, 9},
                                              {0, startx+6*15, (Row+1)*13-2, 9},
                                               
                                              {0, startx+6*17, (Row+1)*13-2, 9},
                                              {0, startx+6*18, (Row+1)*13-2, 9},
                                             };
    typedef struct
    {
        unsigned char *DestValue;
        unsigned char Subtract;
    }DT;
     
    DT dt[] = {
               {&year_H, 0},
               {&year_H, 0},
               {&year_L, 2},
               {&year_L, 2},
                
               {&month, 4},
               {&month, 4},
                
               {&day, 6},
               {&day, 6},
                
               {&hour, 8},
               {&hour, 8},
                
               {&minute, 10},
               {&minute, 10},
                
               {&second, 12},
               {&second, 12},
              };
     
    kmy_TimeGetTime (&year, &month, &day, &hour, &minute, &second, NULL);
    year_H=year/100;
    year_L=year%100;
    clear_lcd();
    DrawTitle_to_row_Language2 (0, "Time Date", "Time Date");
    kmy_PostKey(KEY_pseudo);
    while (1)
    {
        TempKey = kmy_GetKeyValue();
         
        if (TempKey == KEY_NONE) continue;
         
        switch (TempKey)
        {
            case KEY_pseudo:
                if((FlashFlag & 0x01) == 0x01)
                {
                    kmy_LcdClear();
                    DrawTitle_to_row_Language2 (0, "Time Date", "Time Date");
                      
                    FlashFlag &= ~0x01;
                }
                if((FlashFlag & 0x02) == 0x02)
                {
                    memset(sBuff, 0, sizeof(sBuff));
                    sprintf((char *)sBuff, "%02d%02d-%02d-%02d %02d:%02d:%02d", year_H,year_L, month, day, hour, minute, second);
                    text_out_to_row_x(Row, startx, sBuff);
                     
                    FlashFlag &= ~0x02;
                }
                if((FlashFlag & 0x04) == 0x04)
                {
                    #define CURSOR_HEIGHT 5
                    clear_area(0, CursorArray[Index][2], 132, CursorArray[Index][2]+CURSOR_HEIGHT);
                     
                    draw_rect(CursorArray[Index][1], CursorArray[Index][2], 5, CURSOR_HEIGHT, mode_Black);
                     
                    FlashFlag &= ~0x04;
                }
                if(MoveFlag)
                {
                    kmy_PostKey (KEY_RIGHT);//move Index to next
                    MoveFlag = 0;
                }
                break;
            case KEY0:
            case KEY1:
            case KEY2:
            case KEY3:
            case KEY4:
            case KEY5:
            case KEY6:
            case KEY7:
            case KEY8:
            case KEY9:
                if((TranslateKeyToDecimal(TempKey) >= CursorArray[Index][0]) && (TranslateKeyToDecimal(TempKey) <= CursorArray[Index][3]))
                {
                    *dt[Index].DestValue = SetBitOfDecimal(*dt[Index].DestValue, TranslateKeyToDecimal(TempKey),Index-dt[Index].Subtract);
                    FlashFlag |= 0x02;
                    kmy_PostKey(KEY_pseudo);
                }
                MoveFlag = 1;
                break;
            case KEY_LEFT:
                BeepNormal();
                if ( (Index - 1) >= 0)
                {
                    Index -= 1;
                }
                else
                {
                    Index = MaxArray - 1;
                }
                FlashFlag |= 0x04;
                kmy_PostKey(KEY_pseudo);
                break;
             
            case KEY_RIGHT:
                BeepNormal();
                if ( (Index + 1) < MaxArray)
                {
                    Index += 1;
                }
                else
                {
                    Index = 0;
                }
                FlashFlag |= 0x04;
                kmy_PostKey(KEY_pseudo);
                break;
            case KEY_UP:
            case KEY_DOWN:
                BeepNormal();
                if(TempKey == KEY_UP)
                {
                    ret = GetBitOfDecimal(*dt[Index].DestValue, Index-dt[Index].Subtract);
                    if(ret +1 <= CursorArray[Index][3])
                    {
                        ret += 1;
                        *dt[Index].DestValue = SetBitOfDecimal(*dt[Index].DestValue, ret, Index-dt[Index].Subtract);
                    } 
                }
                else
                {
                    ret = GetBitOfDecimal(*dt[Index].DestValue, Index-dt[Index].Subtract);
                    if(ret -1 >= CursorArray[Index][0])
                    {
                        ret -= 1;
                        *dt[Index].DestValue = SetBitOfDecimal(*dt[Index].DestValue, ret, Index-dt[Index].Subtract);
                    } 
                }
                FlashFlag |= 0x02;
                kmy_PostKey(KEY_pseudo);
                break;
            case KEY_Enter:
                BeepNormal();
                year=year_H*100+year_L;
                 
                if(day>YearofMonth[month])
                {
                    if(month==2)
                    {
                        if(year%4==0&&year%100!=0||year%400==0)
                        {
                            if(day>29)month=0;
                        }
                        else
                        {
                            month=0;
                        }
                    }
                    else
                    {
                        month=0;
                    }
                }
                if(month==0||month>12||hour>24||minute>60||second>60)
                {
                    kmy_LcdClear();
                    DrawTitle_to_row(1,"Time date format");
                    DrawTitle_to_row(2," error");
                    WaitkeyAndAutoExit(2);
                }
                else
                {
                    kmy_TimeSetTime (year, month, day, hour, minute, second);
                    msleep(500);
                }
             
                return KEY_Enter;
            case KEY_ReturnInitInterface:
            case KEY_ReturnPrevious:
                BeepNormal();
                return TempKey;
            default:break;
        }
    }
}

static char TimeZoneSet(char *msg)	
{
	unsigned char buff[sizeof(TimeZone)];
	unsigned char retval;

	clear_lcd();
	strcpy((s8*)buff,(sc8*)TimeZone);
	DrawTitle_to_row(0,"Time Zone is:");
	text_out_to_row(1,buff);
	text_out_to_row(3,"Change: Confirm");
	text_out_to_row(4,"Exit: Return");

	retval=WaitkeyAndAutoExit(10);
	if(retval!=KEY_Enter)return retval;

	retval=GetString_abcABC123PYWuBi_Language2("输入Time Zone","Input Time Zone",SRF_abc|SRF_ABC|SRF_123,SRF_123,buff,sizeof(buff),1);

	if(retval!=KEY_Enter)return retval;

	clear_lcd();
	strcpy((s8*)TimeZone,(sc8*)buff);
	if(!StorageVariable())
	{
		DrawTitle_to_row(3,"Change falure!");
	}
	else
	{
		DrawTitle_to_row(3,"Change success");
	}

	return (WaitkeyAndAutoExit(10));

}


char TimeSet(char *msg)
{
	static struct MenuFuc_Language2 Menu[3]={
		{"1.手动时间设置","1.Manual time setting",TimeDateSet},
		{"2.时区设置","2.Time Zone setting",TimeZoneSet},
		{"3.同步网络时间","3.Sync Internet Time",SyncTime},
	};
	struct MenuOperate_Language2 MenuOp={
		Menu,
		3,
		0,
		0,
		0,
		MaxRowOneScr,
		1,
		0,
		0
	};

	while(1){
		MenuHandler_Language2(&MenuOp);
		if(MenuOp.RetVal==KEY_Enter){
			MenuOp.FucRetVal=MenuOp.Menu->MenuFuc(msg);
			if(MenuOp.FucRetVal==KEY_WaitTimeout||MenuOp.FucRetVal==KEY_ReturnInitInterface){CurProc=CurProc_Return;return MenuOp.FucRetVal;}
			else{clear_lcd();MenuOp.flashflag=1;}
		}else{
			CurProc=CurProc_Return;
			return MenuOp.RetVal;
		}
	}	
}

char BasicSettings(char *msg)
{
	static struct MenuFuc_Language2 Menu[10]={
		{"1.屏幕对比度设置","1.Contrast set",ContrastSet},
		{"2.按键音设置","2.Keytone set",BasicKeyTone},
		{"3.时间设置","3.Time set",TimeSet},
		{"4.APN设置","4.APN set",BasicApnSet},
		{"5.屏保时间设置","5.Timeout set",PowerOffScreenTime},
		{"6.语言设置","6.Language set",LanguageSet},
		{"7.读取相关信息","7.Read Misc",ReadSomeMessage},
		{"8.自测","8.Selftest",SelfTest},
	};
	struct MenuOperate_Language2 MenuOp={
		Menu,
		8,
		0,
		0,
		0,
		MaxRowOneScr,
		1,
		0,
		0
	};

	while(1){
		MenuHandler_Language2(&MenuOp);
		if(MenuOp.RetVal==KEY_Enter){
			MenuOp.FucRetVal=MenuOp.Menu->MenuFuc(msg);
			if(MenuOp.FucRetVal==KEY_WaitTimeout||MenuOp.FucRetVal==KEY_ReturnInitInterface){CurProc=CurProc_Return;return MenuOp.FucRetVal;}
			else{clear_lcd();MenuOp.flashflag=1;}
		}else{
			CurProc=CurProc_Return;
			return MenuOp.RetVal;
		}
	}
}






static char InitDeskeySet(char *msg)//
{
	unsigned char buff[24];
	unsigned char retval;

	clear_lcd();
	strcpy((s8*)buff,(sc8*)InitDes3key);
	DrawTitle_to_row_Language2(0,"【当前密钥】","Current key");
	text_out_to_row_Language2(1,buff,buff);
	text_out_to_row_Language2(3,"更改:请按确认","Change: Confirm");
	text_out_to_row_Language2(4,"退出:请按取消","Exit: Return");

	retval=WaitkeyAndAutoExit(10);
	if(retval!=KEY_Enter)return retval;

	retval=GetString_abcABC123PYWuBi_Language2("【输入初始密钥】","Initkey edit",SRF_123|SRF_ABC|SRF_abc,SRF_abc,buff,sizeof(buff)-1,1);
	if(retval!=KEY_Enter)return retval;

	clear_lcd();
	if(strlen((sc8*)buff)!=24){
		DrawTitle_to_row_Language2(2,"密钥长度不对!","Key length error!");
	}else{
		memset(InitDes3key,0,sizeof(InitDes3key));
		strcpy((s8*)InitDes3key,(sc8*)buff);
		if(!StorageVariable()){
			DrawTitle_to_row_Language2(3,"更改失败!","change failure!");
		}else{
			DrawTitle_to_row_Language2(3,"更改成功!","change success!");
		}
	}

	return (WaitkeyAndAutoExit(10));
}


static char TerminalIDSet(char *msg)//
{
	unsigned char buff[24];
	unsigned char retval;

	clear_lcd();
	strcpy((s8*)buff,(sc8*)TerminalID);
	DrawTitle_to_row_Language2(0,"【当前终端ID】","Current ID");
	text_out_to_row_Language2(1,buff,buff);
	text_out_to_row_Language2(3,"更改:请按确认","Change: Confirm");
	text_out_to_row_Language2(4,"退出:请按取消","Exit: Return");

	retval=WaitkeyAndAutoExit(10);
	if(retval!=KEY_Enter)return retval;

	retval=GetString_abcABC123PYWuBi_Language2("【输入终端ID】","TerminalID edit",SRF_123|SRF_ABC|SRF_abc,SRF_123,buff,sizeof(buff)-1,1);
	if(retval!=KEY_Enter)return retval;

	clear_lcd();
	if(strlen((sc8*)buff)==0){
		DrawTitle_to_row_Language2(2,"长度不能为0","Length error!");
	}else{
		memset(TerminalID,0,sizeof(TerminalID));
		strcpy((s8*)TerminalID,(sc8*)buff);
		if(!StorageVariable()){
			DrawTitle_to_row_Language2(3,"更改失败!","change failure!");
		}else{
			DrawTitle_to_row_Language2(3,"更改成功!","change success!");
		}
	}

	return (WaitkeyAndAutoExit(10));
}

static char TerminalPasswordSet(char *msg)//
{
	unsigned char buff[24];
	unsigned char retval;

	clear_lcd();
	strcpy((s8*)buff,(sc8*)TerminalPassword);
	DrawTitle_to_row_Language2(0,"【当前终端密码】","TerminalPassword");
	text_out_to_row_Language2(1,buff,buff);
	text_out_to_row_Language2(3,"更改:请按确认","Change: Confirm");
	text_out_to_row_Language2(4,"退出:请按取消","Exit: Return");

	retval=WaitkeyAndAutoExit(10);
	if(retval!=KEY_Enter)return retval;

	retval=GetString_abcABC123PYWuBi_Language2("【输入终端密码】","TerminalPassword",SRF_123|SRF_ABC|SRF_abc,SRF_123,buff,sizeof(buff)-1,1);
	if(retval!=KEY_Enter)return retval;

	clear_lcd();
	if(strlen((sc8*)buff)==0){
		DrawTitle_to_row_Language2(2,"长度不能为0","Length error!");
	}else{
		memset(TerminalPassword,0,sizeof(TerminalPassword));
		strcpy((s8*)TerminalPassword,(sc8*)buff);
		if(!StorageVariable()){
			DrawTitle_to_row_Language2(3,"更改失败!","change failure!");
		}else{
			DrawTitle_to_row_Language2(3,"更改成功!","change success!");
		}
	}

	return (WaitkeyAndAutoExit(10));
}


static char IpAddSet(char *msg)//IP地址设置
{
	unsigned char buff[24];
	unsigned char retval;
	abcABC123PYWuBiArgument argAa1;

	clear_lcd();
	strcpy((s8*)buff,(sc8*)ServerIpPortBuf);
	DrawTitle_to_row_Language2(0,"【当前IP地址】","Current IP");
	text_out_to_row_Language2(1,buff,buff);
	text_out_to_row_Language2(3,"更改:请按确认","Change: Confirm");
	text_out_to_row_Language2(4,"退出:请按取消","Exit: Return");

	retval=WaitkeyAndAutoExit(10);
	if(retval!=KEY_Enter)return retval;

	abcABC123PYWuBiArgDeInit(&argAa1,buff,28);
	argAa1.srfFlag=SRF_123|SRF_ABC|SRF_abc|SRF_PY|SRF_BiHua;
	argAa1.LocalTitle="【输入IP地址】";
	argAa1.HaveoldFlag=1;
	retval=GetString_abcABC123PYWuBi2(&argAa1);
	if(retval!=KEY_Enter)return retval;

	clear_lcd();
	if(strlen((sc8*)buff)>21||strlen((sc8*)buff)<9){
		DrawTitle_to_row_Language2(2,"错误:IP地址不对!","Error:IP wrong!");
	}else{
		memset(ServerIpPortBuf,0,sizeof(ServerIpPortBuf));
		strcpy((s8*)ServerIpPortBuf,(sc8*)buff);
		if(!StorageVariable()){
			DrawTitle_to_row_Language2(3,"更改失败!","change failure!");
		}else{
			DrawTitle_to_row_Language2(3,"更改成功!","change success!");
		}
	}

	return (WaitkeyAndAutoExit(10));
}



static char PIN_OpenClose(char *msg)
{
	#if 0
	char retval,buff[1024];
	char temp[20];
	char PinTimes[2],PukTimes[3];

	loop:
	retval=ReadPINPUKTimes(PinTimes,PukTimes);
	if(retval==0){
		if(PinTimes[0]>'0'){
			clear_lcd();
			DrawTitle_to_row_Language2(0,"输入PIN","Input PIN");
			if(Language[0]=='1'){
				strcpy(buff,"remain times:");
			}else{
				strcpy(buff,"剩余次数:");
			}
			strcat(buff,PinTimes);
			DrawTitle_to_row_Language2(1,buff,buff);
			retval=PassWordInput2(NULL,temp,5,1,0,KEY_NONE,2);
			if(retval!=KEY_Enter)return 0;
			if(strlen(temp)!=4){
				clear_lcd();
				DrawTitle_to_row_Language2(1,"长度错误","length error");
				sleep(2);
				return 0;
			}

			strcpy(buff,"AT+CLCK=\"SC\",");			//AT+CLCK="SC",1,"0907"
			if(RequirePin==NO){
				strcat(buff,"1");
			}else{
				strcat(buff,"0");
			}
			strcat(buff,",\"");
			strcat(buff,temp);
			strcat(buff,"\"\r");

			if(send_at_cmd_and_receive(buff,sizeof(buff),2000,"OK\r")!=TCP_OK){
				clear_lcd();
				DrawTitle_to_row_Language2(2,"PIN 错误","PIN error");
				sleep(2);
				goto loop;
			}else{
				clear_lcd();
				if(RequirePin==0){
					DrawTitle_to_row_Language2(2,"启用pin成功","Enble PIN success");
					RequirePin=1;
				}else{
					DrawTitle_to_row_Language2(2,"禁用pin成功","Disable PIN success");
					RequirePin=0;
				}
				sleep(2);
				return 0;
			}
		}else{
			clear_lcd();
			DrawTitle_to_row_Language2(2,"PIN 被锁","PIN locked");
			sleep(2);
			PUK_Required();
			return 0;
		}
	}

	return 0;
	#else
	return 0;
	#endif
}
static char PIN_Store(char *msg)//AT+CPWD="SC","0909","0505"
{
	unsigned char ret;
	unsigned char pin[7];

	clear_lcd();
	strcpy((s8*)pin,(sc8*)StorePIN);
	DrawTitle_to_row_Language2(0,"PIN","PIN");
	text_out_to_row_Language2(1,"****","****");
	text_out_to_row_Language2(3,"修改:请按确认键","change:confirm");
	text_out_to_row_Language2(4,"返回:请按返回键","exit:return");

	ret=WaitkeyAndAutoExit(10);
	if(ret!=KEY_Enter)return ret;

	if(Language[0]=='1'){
		ret=PassWordInput2("Input PIN",pin,6,1,1,KEY_NONE,1);
	}else{
		ret=PassWordInput2("【输入PIN】",pin,6,1,1,KEY_NONE,1);
	}
	if(ret!=KEY_Enter)return ret;

	if(strlen((sc8*)pin)!=4){
		clear_lcd();
		DrawTitle_to_row_Language2(2,"长度错误","longth error");
		return(WaitkeyAndAutoExit(10));
	}

	clear_lcd();
	strcpy((s8*)StorePIN,(sc8*)pin);
	if(!StorageVariable()){
		DrawTitle_to_row_Language2(3,"修改失败","Change falure!");
	}else{
		DrawTitle_to_row_Language2(3,"修改成功","Change success!");
	}

	return (WaitkeyAndAutoExit(10));
}
static char PIN_Change(char *msg)//AT+CPWD="SC","0909","0505"
{
	#if 0
	PasswordInputArgument arg;
	unsigned char oldpassword[5];
	unsigned char newpassword1[5];
	unsigned char newpassword2[200];
	unsigned char retval;

	if(RequirePin==YES){	//启用pin  之后才能改pin
		PassWordArgDeInit(&arg,oldpassword,4);
		arg.LocalTitle="【输入原PIN】";
		retval=BT_PassWordInput_MiddledDisplay(&arg);
		if(retval!=KEY_Enter)return retval;

		again:
		PassWordArgDeInit(&arg,newpassword1,4);
		arg.LocalTitle="【输入新PIN】";
		retval=BT_PassWordInput_MiddledDisplay(&arg);
		if(retval!=KEY_Enter)return retval;

		PassWordArgDeInit(&arg,newpassword2,4);
		arg.LocalTitle="【再次输入新PIN】";
		retval=BT_PassWordInput_MiddledDisplay(&arg);
		if(retval!=KEY_Enter)return retval;

		if(strcmp(newpassword1,newpassword2)!=0){
			clear_lcd();
			DrawTitle_to_row(1,"你两次输入的");
			DrawTitle_to_row(2,"新PIN不一致");
			DrawTitle_to_row(3,"请重新输入");
			retval=WaitkeyAndAutoExit(10);
			if(retval==KEY_Enter)goto again;
			else {return retval;}
		}

		//AT+CPWD="SC","0909","0505"
		strcpy(newpassword2,"AT+CPWD=\"SC\",\"");
		strcat(newpassword2,oldpassword);
		strcat(newpassword2,"\",\"");
		strcat(newpassword2,newpassword1);
		strcat(newpassword2,"\"\r");
		retval=send_at_cmd_and_return(newpassword2,5000,"OK","ERROR");

		clear_lcd();
		if(retval==TCP_OK){
			DrawTitle_to_row(2,"修改成功");
		}
		else{
			DrawTitle_to_row(2,"修改失败");	
		}
		return WaitkeyAndAutoExit(10);
	}
	else{
		clear_lcd();
		DrawTitle_to_row(2,"请先启用PIN");
		return WaitkeyAndAutoExit(10);
	}

	return 0;
	#else
	return 0;
	#endif
}

char PinSet(char *msg)
{
	#if 0
	return(abeyant());
	#else
	static unsigned char BuffLocal[30],BuffOther[30];


	static struct MenuFuc_Language2 Menu[3]={
		{BuffLocal,BuffOther,PIN_OpenClose},
		{"2.修改 PIN","2.Change PIN",PIN_Change},
		{"3.存储 PIN","3.Store PIN",PIN_Store},
	};
	struct MenuOperate_Language2 MenuOp={
		Menu,
		3,
		0,
		0,
		0,
		MaxRowOneScr,
		1,
		0,
		0
	};

	while(1){
		if(RequirePin==0){
			strcpy((s8*)BuffLocal,"1.禁用 PIN");
			strcpy((s8*)BuffOther,"1.Close PIN");
		}else{
			strcpy((s8*)BuffLocal,"1.启用 PIN");
			strcpy((s8*)BuffOther,"1.Open PIN");
		}
		MenuHandler_Language2(&MenuOp);
		if(MenuOp.RetVal==KEY_Enter){
			MenuOp.FucRetVal=MenuOp.Menu->MenuFuc(msg);
			if(MenuOp.FucRetVal==KEY_WaitTimeout||MenuOp.FucRetVal==KEY_ReturnInitInterface){CurProc=CurProc_Return;return MenuOp.FucRetVal;}  
			else{clear_lcd();MenuOp.flashflag=1;}
		}else{
			return MenuOp.RetVal;
		}
	}
	#endif
}

static char RestoreChuChangSet(char *msg)
{
	unsigned char retval;

	clear_lcd();
	DrawTitle_to_row_Language2(0,"特别警告:","Warning:");
	DrawTitle_to_row_Language2(1,"恢复出厂设置将导致,","Are you sure");
	DrawTitle_to_row_Language2(2,"此终端机成为一新机", "reset to the");
	DrawTitle_to_row_Language2(3,"确定恢复:请按确认","factory settings?");
	DrawTitle_to_row_Language2(4,"退出:请按任意键",NULL);

	retval=WaitkeyAndAutoExit(20);
	if(retval!=KEY_Enter)return retval;

	retval=RestoreConfigFile();
	clear_lcd();
	if(retval==1){
		DrawTitle_to_row_Language2(2,"出厂设置恢复成功!","Reset success");
	}else{
		DrawTitle_to_row_Language2(2,"出厂设置恢复失败!","Reset error");
	}
	return (WaitkeyAndAutoExit(10));
}



static char OpPsword(char *msg)//终端操作密码设置，最多20  位密码
{
	unsigned char buff[16],buff2[16];
	char retval;
	PasswordInputArgument arg;

	PassWordArgDeInit(&arg,buff,sizeof(buff));
	arg.LocalTitle="【输入新密码】";
	arg.OtherTitle="Input new password";
	retval=PassWordInput_MiddledDisplay(&arg);
	if(retval!=KEY_Enter)return retval;

	PassWordArgDeInit(&arg,buff2,sizeof(buff2));
	arg.LocalTitle="【再次确认新密码】";
	arg.OtherTitle="Again Input";
	retval=PassWordInput_MiddledDisplay(&arg);
	if(retval!=KEY_Enter)return retval;

	clear_lcd();
	if(strcmp((sc8*)buff,(sc8*)buff2)==0){
		memset(EnterAdvancedSetPassWord,0,sizeof(EnterAdvancedSetPassWord));
		strcpy((s8*)EnterAdvancedSetPassWord,(sc8*)buff);
		if(!StorageVariable()){
			DrawTitle_to_row_Language2(3,"更改失败!","change failure!");
		}else{
			DrawTitle_to_row_Language2(3,"更改成功!","change success!");
		}
	}else{
		DrawTitle_to_row_Language2(2,"错误!你两次输入","Error,Two times");
		DrawTitle_to_row_Language2(3,"的密码不一致!","Input not same");
	}

	return (WaitkeyAndAutoExit(10));
}



static char Delete_log(char *msg)
{
	#if 0
	unsigned char *p;
	unsigned char retval;

	p=(unsigned char *)(malloc(sizeof(JiaoFeiLogPath)+20));

	if(p!=NULL){
		strcpy(p,"rm -rf ");
		strcat(p,JiaoFeiLogPath);

		system(p);

		free(p);
		p=NULL;

		clear_lcd();
		DrawTitle_to_row(2,"日志记录删除成功");
	}
	else{
		clear_lcd();
		DrawTitle_to_row(2,"无法分配内存空间");
	}

	retval=WaitkeyAndAutoExit(10);

	return retval;
	#else
	return 0;
	#endif
}
static char Console(char *msg)
{
	//exit(1);
	return 0;
}

static char FormatEEPROM(char *msg)
{
	u8 retval;

	clear_lcd();
	text_out_to_row_Language2(1,"格式化EEPROM,将导致","Format EEPROM will");
	text_out_to_row_Language2(2,"EEPROM中的所有数据都","Lose all EEPROM data");
	text_out_to_row_Language2(3,"丢失,确认请按confirm","Can you sure Format");
	text_out_to_row_Language2(4,"键","it?");
	retval=WaitkeyAndAutoExit(10);
	if(retval!=KEY_Enter)return retval;

	clear_lcd();
	DrawTitle_to_row_Language2(2,"正在格式化","Formating");
	EepromFile_FileSystemFormat();
	DrawTitle_to_row_Language2(3,"格式化完毕","Format Complete");
	retval=WaitkeyAndAutoExit(10);

	return retval;
}


char AdvancedSet(char *msg)
{
	unsigned char OperateCode[30];
	char retval;
	PasswordInputArgument arg;
	static struct MenuFuc_Language2 Menu[15]={
		{"1.高级设置密码设置","1.This Password set",OpPsword},		//注意:  第一个字符串的长度不能大于MaxByteOneLine-1
		{"2.IP地址设置","2.IP address set",IpAddSet},
		{"3.PIN设置","3.PIN set",PinSet},
		{"4.恢复出厂设置","4.Reset Factory",RestoreChuChangSet},
		{"5.删除日志记录","5.Delete Record",Delete_log},
		{"6.初始密钥设置","6.Init 3deskey set",InitDeskeySet},
		{"7.终端ID设置","7.Terminal ID set",TerminalIDSet},
		{"8.初始密钥设置","8.Terminal Password",TerminalPasswordSet},
		{"9.格式化EEPROM","9.Format EEPROM",FormatEEPROM},
		{"10.控制台","10.Console",Console}
	};
	struct MenuOperate_Language2 MenuOp={Menu,10,0, 0,0,MaxRowOneScr, 1,0,0};

	PassWordArgDeInit(&arg,OperateCode,16);
	arg.LocalTitle="【请输入密码】";
	retval=PassWordInput_MiddledDisplay(&arg);
	if(retval!=KEY_Enter)return retval;

	if(strcmp((sc8*)EnterAdvancedSetPassWord,(sc8*)OperateCode)!=0){
		DrawTitle_to_row_Language2(2,"密码错误!","Password error");
		return(WaitkeyAndAutoExit(10));
	}

	while(1){
		MenuHandler_Language2(&MenuOp);
		if(MenuOp.RetVal==KEY_Enter){
			MenuOp.FucRetVal=MenuOp.Menu->MenuFuc(NULL);
			if(MenuOp.FucRetVal==KEY_WaitTimeout||MenuOp.FucRetVal==KEY_ReturnInitInterface){CurProc=CurProc_Return;return MenuOp.FucRetVal;} 
			else{clear_lcd();MenuOp.flashflag=1;}
		}else{
			return (MenuOp.RetVal);
		}
	}
}



char MainMenuScreen_Settings(char *msg)
{
	static struct MenuFuc_Language2 Menu[3]={
		{"1.基本设置","1.BasicSettings",BasicSettings},
		{"2.高级设置","2.AdvancedSet",AdvancedSet},
	};
	struct MenuOperate_Language2 MenuOp={
		Menu,
		2,
		0,
		0,
		0,
		MaxRowOneScr,
		1,
		0,
		0
	};

	while(1){
		MenuHandler_Language2(&MenuOp);
		if(MenuOp.RetVal==KEY_Enter){
			MenuOp.FucRetVal=MenuOp.Menu->MenuFuc(msg);
			if(MenuOp.FucRetVal==KEY_WaitTimeout||MenuOp.FucRetVal==KEY_ReturnInitInterface){CurProc=CurProc_Return;return MenuOp.FucRetVal;}
			else{clear_lcd();MenuOp.flashflag=1;}
		}else{
			CurProc=CurProc_Return;
			return MenuOp.RetVal;
		}
	}
}


