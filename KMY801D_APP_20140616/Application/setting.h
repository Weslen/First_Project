#ifndef Setting_H
#define Setting_H

#include "my_stdint.h"


#define STORESIZE			14		//if XStor  added entries, the STORESIZE  must added too



struct storeFile{
	unsigned char *Viarable;		//Viarable
	const unsigned char *initVal;	//init value
};


extern struct storeFile XStor[20];
extern const u8 efile_Config[];
extern u8 dataledflag;
extern const u8 Configfile_des3key[25];



extern unsigned char LightPercent[4];				//light percent
extern unsigned char EnterAdvancedSetPassWord[22];	//∏ﬂº∂…Ë÷√√‹¬Î
extern unsigned char ServerIpPortBuf[28];			//ip µÿ÷∑220.162.237.128,65167
extern unsigned char StorePIN[5];					//PIN name
extern unsigned char InitDes3key[24+1];			//≥ı º3des √‹‘ø
extern unsigned char ApnApn[21+1];
extern unsigned char ApnUsername[21+1];
extern unsigned char ApnPassword[21+1];
extern unsigned char beepKG[2];
extern unsigned char ScreenOffTimeout[3];			//PIN name
extern unsigned char TerminalID[17];				//÷’∂Àid
extern unsigned char TerminalPassword[17];			//÷’∂À√‹¬Î
extern unsigned char TimeZone[15];  



#define KMY_printer			'0'
#define JieNa_printer		'1'




char BasicSettings(char *msg);
char MainMenuScreen_Settings(char *msg);
u8 RestoreConfigFile(void);
u8 RestoreGlobalVariable(void);
char PinSet(char *msg);


#endif

