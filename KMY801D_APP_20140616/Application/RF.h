
#ifndef RF_H
#define RF_H






int dc_init(int port, long baud);
int dc_exit(int icdev);
int dc_card(int icdev, unsigned char _Mode, unsigned long *_Snr);
int dc_request(int icdev, unsigned char _Mode, unsigned int *TagType);
int dc_anticoll(int icdev,unsigned char _Bcnt, unsigned long *_Snr);
int dc_select(int icdev, unsigned long _Snr, unsigned char *_Size);
int dc_load_key(int icdev,unsigned char _Mode,unsigned char _SecNr,unsigned char *_NKey);
int dc_authentication(int icdev,unsigned char _Mode,unsigned char _SecNr);
int dc_read(int icdev,unsigned char _Adr,unsigned char *_Data);
int dc_write(int icdev,unsigned char _Adr,unsigned char *_Data);
int dc_halt(int icdev);
int dc_changeb3(int icdev,unsigned char _SecNr,unsigned char *_KeyA,unsigned char _B0,unsigned char
_B1,unsigned char _B2,unsigned char _B3,unsigned char _Bk,unsigned char *_KeyB);
int dc_initval(int icdev,unsigned char _Adr,unsigned long _Value);
int dc_increment(int icdev,unsigned char _Adr,unsigned long _Value);
int dc_readval(int icdev,unsigned char _Adr,unsigned long *_Value);
int dc_decrement(int icdev,unsigned char _Adr,unsigned long _Value);
int dc_HL_authentication(int icdev,unsigned char reqmode,unsigned long snr,unsigned char authmode,unsigned
char secnr);
int dc_HL_read(int icdev,unsigned char _Mode,unsigned char _Adr,unsigned long _Snr,unsigned char *_Data,
unsigned long *_NSnr);
int dc_HL_write(int icdev,unsigned char _Mode,unsigned char _Adr,unsigned long *_Snr,unsigned char *_Data);
int dc_restore(int icdev,unsigned char _Adr);
int dc_transfer(int icdev,unsigned char _Adr);
int dc_authentication_2(int icdev,unsigned char _Mode,unsigned char KeyNr,unsigned char Adr);
int dc_gettime(int icdev,unsigned char *time);
int dc_getver(int icdev,unsigned char *buff);
int dc_settime(int icdev,unsigned char *time);
int dc_srd_eeprom(int icdev,int offset,int length,unsigned char *rec_buffer);
int dc_swr_eeprom(int icdev,int offset,int length,unsigned char* buffer);

int dc_anticoll2(int icdev,unsigned char _Bcnt,unsigned long *_Snr);
int dc_select2(int icdev,unsigned long _Snr,unsigned char *_Size);


int dc_config_card(int icdev,unsigned char cardtype);
int dc_pro_command(int idComDev,
unsigned char slen, unsigned char * sendbuffer,
unsigned char *rlen, unsigned char * databuffer,
unsigned char timeout);
int dc_pro_commandsource(int idComDev,unsigned char slen,
unsigned char * sendbuffer,unsigned char *rlen,
unsigned char * databuffer,unsigned char timeout);
int dc_pro_commandlink(int idComDev, unsigned char slen,
unsigned char * sendbuffer, unsigned char *rlen,
unsigned char * databuffer, unsigned char timeout,
unsigned char FG);
int dc_pro_reset(int icdev,unsigned char *rlen,unsigned char *receive_data);
int dc_pro_halt(int icdev);
int dc_request_b( int icdev,unsigned char _Mode, unsigned char AFI, unsigned char N, unsigned char *ATQB);
int dc_attrib(int icdev, unsigned char *PUPI, unsigned char CID);








#endif

