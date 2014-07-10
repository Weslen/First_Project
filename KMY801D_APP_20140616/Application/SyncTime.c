#include "SyncTime.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "kmy_USART2Drv.h"
#include "lcd.h"
#include "gprs.h"
#include "setting.h"

/*
time.nist.gov
time.windows.com
time-nw.nist.gov
time-a.nist.gov
time-b.nist.gov
*/

const char TimeServer[]={"time.nist.gov"};

char SyncTime(char *msg)
{
	unsigned char buff[100]={0};
	unsigned char timezone;
        unsigned int year = 0;
        unsigned char year_H= 20;
        unsigned char year_L= 0;
        unsigned char month = 0;
        unsigned char day = 0;
        unsigned char hour = 0;
        unsigned char minute = 0;
        unsigned char second = 0;
	char YearofMonth[13]={0,31,28,31,30,31,30,31,31,30,31,30,31};
	char *p=NULL;

	clear_lcd();
	DrawTitle_to_row_Language2(2,"同步时间中","Sync time"); 		
	DrawTitle_to_row_Language2(3,"请稍等...","Please wait...");

    sprintf((char*)buff,"AT+QNTP=\"%s\",123\r",TimeServer);
	printf("buff:<%s>\r\n",buff);
	kmy_USART2SendString(buff);
	memset(buff,0,sizeof(buff));
	if(tcp_receive(buff,sizeof(buff),"+QNTP:",180)!=TCP_ReceiveOK) goto FailExit;
	if(strstr((char*)buff,"+QNTP: 0")==NULL) goto FailExit;
	
	kmy_USART2SendString("AT+CCLK?\r");
	printf("AT+CCLK?\r");
	if(tcp_receive(buff,sizeof(buff),"+CCLK:",10)!=TCP_ReceiveOK) goto FailExit;
	printf("buff:<%s>\r\n",buff);

//+CCLK: "14/05/30,09:36:29+00"	
        p=(char*)buff;
	p=strchr(p,'"');
	if(p==NULL)goto FailExit;
	p++;
	year_L=atoi(p);
	printf("year_L:<%d>\r\n",year_L);
	
	p=strchr(p,'/');
	if(p==NULL)goto FailExit;
	p++;
	month=atoi(p);
	printf("month:<%d>\r\n",month);

	p=strchr(p,'/');
	if(p==NULL)goto FailExit;
	p++;
	day=atoi(p);
	printf("day:<%d>\r\n",day);

	p=strchr(p,',');
	if(p==NULL)goto FailExit;
	p++;
	hour =atoi(p);
	printf("hour:<%d>\r\n",hour);

	p=strchr(p,':');
	if(p==NULL)goto FailExit;
	p++;
	minute=atoi(p);
	printf("minute:<%d>\r\n",minute);

	p=strchr(p,':');
	if(p==NULL)goto FailExit;
	p++;
	second=atoi(p);
	printf("second:<%d>\r\n",second);

	if(strchr((char*)TimeZone,'+'))
	{
		p=strchr((char*)TimeZone,'+');
		if(p!=NULL)
		{
			p++;
			timezone=atoi(p);
			hour+=timezone;
		}
		if(hour>=24)
		{
			hour-=24;
			day++;
		}
		
		year=year_H*100+year_L;
		
		if(day>YearofMonth[month])
		{
			 if(month==2)
			 {
				   if(year%4==0&&year%100!=0||year%400==0)
				   {
					   if(day>29)month++;
					   day=1;
				   }
				   else
				   {
					   month++;
					   day=1;
				   }
			 }
			 else
			 {
				   month++;
				   day=1;
			 }
		}
		
		if(month>12)
		{
			month=1;
			year++;
		}
	}
	else
	{
		p=strchr((char*)TimeZone,'-');
		if(p!=NULL)
		{
			p++;
			timezone=atoi(p);
			if(hour>=timezone)hour-=timezone;
			else
			{
				hour=24-(timezone-hour);
				day--;
			}
		}

		year=year_H*100+year_L;
		if(day==0)
		{
			month--;
			if(month==0)
			{
				month=12;
				year--;
			}	
			if(month==2)
			{
				   if(year%4==0&&year%100!=0||year%400==0)
				   {
					   day=29;
				   }
				   else
				   {
					   day=YearofMonth[month];
				   }
			 }
			 else
			 {
				   day=YearofMonth[month];
			 }
		}
	}	

	printf("Time:%4d-%02d-%02d %02d:%02d:%02d",year,month,day,hour,minute,second);
	kmy_TimeSetTime (year, month, day, hour, minute, second);
	clear_lcd();
	DrawTitle_to_row_Language2(2,"同步时间成功!","Sync time success"); 
	return WaitkeyAndAutoExit(5);	
		
FailExit:	
	printf("buff:<%s>\r\n",buff);
	clear_lcd();
	DrawTitle_to_row_Language2(2,"同步时间失败!","Sync time fail"); 
	return WaitkeyAndAutoExit(5);
}


