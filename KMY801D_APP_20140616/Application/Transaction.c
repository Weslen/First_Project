#include "Transaction.h"
#include "lcd.h"
#include "gprs.h"
#include <string.h>
#include "setting.h"
#include <stdio.h>
#include "key.h"


static char tcp_connect_send_recieve(unsigned char *SendBuff,unsigned int Sendlen,unsigned char *recBuff,unsigned int recBuffSize)
{
	char retVal;

	clear_lcd();
	DrawTitle_to_row_Language2(2,"正在通讯……","Communicating");

	clear_area_to_row(3,3);
	DrawTitle_to_row_Language2(3,"GPRS网络连接中…","Connecting");
//	retVal=tcp_Connect("31.170.167.97,80"); 	//IP
	retVal=tcp_Connect("weslen.yupage.com");	//Domain
	if(retVal!=TCP_OK){goto failreturn;}

	clear_area_to_row(3,3);
	DrawTitle_to_row_Language2(3,"发送数据中…","Sending");
	retVal=tcp_send(SendBuff,Sendlen);
	if(retVal!=TCP_SendOK){goto failreturn;}

	clear_area_to_row(3,3);
	DrawTitle_to_row_Language2(3,"接受数据中…","Receiving");
	retVal=tcp_receive(recBuff,recBuffSize,"HTTP/1.1 200 OK",60);
	if(retVal!=TCP_ReceiveOK){goto failreturn;} 	
	tcp_DisConnect();
	return TCP_CONNECT_AND_RIGHT_RECEIVE;

	failreturn:
	tcp_DisConnect();
	return retVal;
}


static void DisplayTcpConnectSendRecieveError(unsigned char retval)
{
	clear_lcd();
	if(retval==TCP_ConnectFailure){
		DrawTitle_to_row_Language2(2,"连接服务器失败!","Connect error");
	}else if(retval==TCP_SendFailure){
		DrawTitle_to_row_Language2(2,"发送失败!","Send error");
	}else if(retval==TCP_UserSelfReturn){
		DrawTitle_to_row_Language2(2,"用户返回!","User return");
	}else if(retval==TCP_ReceiveBuffNotEnough){
		DrawTitle_to_row_Language2(2,"接收缓冲区不够","Buff Not enough");
	}else if(retval==TCP_ReceiveTimeOut){
		DrawTitle_to_row_Language2(2,"接收超时","Receive time out");
	}else if(retval==TCP_GETFailure){
		DrawTitle_to_row_Language2(2,"HTTP GET失败","HTTP GET Fail");
	}else{
		DrawTitle_to_row_Language2(2,"未知错误!","Undefined error");
	}
}


char Transaction(char *msg)
{
	unsigned char SendBuff[1000]={"GET http://weslen.yupage.com/OrderRequest.php HTTP/1.1\r\nHost: weslen.yupage.com\r\nPragma: no-cache\r\nAccept: */*\r\nProxy-Connection: Keep-Alive\r\n\r\n"};
	unsigned char retval;
	unsigned char recBuff[3048];
	unsigned int SendLen;

    printf("SendBuff:%s\r\n",SendBuff);
	
	SendLen=strlen((char*)SendBuff);
	
	retval=tcp_connect_send_recieve(SendBuff,SendLen,recBuff,sizeof(recBuff));
	printf("retval=<%2x>\r\n",retval);
	if(retval!=TCP_CONNECT_AND_RIGHT_RECEIVE){	
		DisplayTcpConnectSendRecieveError(retval);
		WaitkeyAndAutoExit(10);
	    return retval;
	}
	printf("\r\n-------------------\r\nrecBuff:%s\r\n",recBuff);
	printf("\r\n-------------------\r\n");
	clear_lcd();
	DrawTitle_to_row_Language2(2,"接收成功","Receive ok");
	WaitkeyAndAutoExit(10);
	return retval;
}



