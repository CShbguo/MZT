
//M50模块工程专用的文件。

#include <stdarg.h>
#include "stdio.h"
#include "string.h"
//#include "board.h"
#include "..\include\include.h"
#include "Gszct.h"
#include "szct.h"

extern unsigned char gchn_eng;//中英文标志
extern stAlammessage galamessage;
extern unsigned char GPRSLinkProcess;
extern unsigned char restore_flag;
#ifdef KEY_PAD
volatile unsigned char gKEYPAD_flag;//和按键显示屏通讯标志
volatile unsigned char gkeyPadValue;//按键显示屏的按键值
volatile unsigned char gKeyPadIndex;//协议数据记数
volatile unsigned char gKeyRestorDis;//需要重新刷新KEYPAD显示

volatile unsigned char gKeyMode;	//当前KEYPAD的功能状态
extern unsigned short gSendOverTime;//发送后收应答超时计数器
extern unsigned int g24GDisFlash;//24G卡界面刷新
#endif

#ifdef _GPS_DIAODU_
extern void Save_DIAODUCMD_info(void);
extern void Load_DIAODUCMD_info(void);
extern void save_DD_YinYun_info(void);
#endif

extern unsigned char gprsRevBuf[1024];//GPRS接收缓冲

#ifdef GPRS_SOUD_ENABLE
volatile unsigned char ucMC323Mode;//GPRS模块的工作状态。A1 有电话呼入,需要显示，A2显示完成，A3，接听电话，AX的都是语音，其它未定义
extern volatile unsigned char ucMC323Mode;//GPRS模块的工作状态。A1 有电话呼入,需要显示，A2显示完成，A3，接听电话，AX的都是语音，其它未定义
#endif
extern void gprs_snd(unsigned int len, char *dat);
#ifdef _TTSSound_enable_
unsigned char xfSounding;
#endif

//以便使用 printf 函数
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
PUTCHAR_PROTOTYPE
{
	com_snd(COM1, 1, (unsigned char*)&ch);
	return ch;
}

// #ifdef _debug_
// void dprintf (const char *fmt, ...)
// {
// 	va_list args;
// 	unsigned int i;
// 	i=i;
// 	va_start (args, fmt);
// 	printf(fmt, args);
// 	va_end (args);
// }
// #endif

void debugCOM1tring(char *str)//--通过串口4
{
	debugstringC(str, 0x80);
	return;
}
void debugCOM1data(unsigned char *value, unsigned int uclen, unsigned char mode)//----通过串口4
{
	debugdata(value, uclen, (mode|0x80));
	return;
}

unsigned char CPUMAC2(unsigned char PsamID, unsigned char *pMac, unsigned char *pData, unsigned char len)
{
	ISO7816_ADPU_SEND Scmd;
	ISO7816_ADPU_RESPONSE Rcmd;
	
	Scmd.CLA = 0x80;
	Scmd.INS = 0x72;
	Scmd.P1 = 0x00;
	Scmd.P2 = 0x00;
	Scmd.LC = len;
	
	memcpy(Scmd.DATA, pData, len);
	
	Scmd.LE = 0x00;
#ifdef _debug_CPU_
	debugstring("scmd:");
	debugdata(pData, len, 1);
#endif
	IccCommand(PsamID, &Scmd, &Rcmd);
#ifdef _debug_CPU_
	debugstring("rcmd M2:");
	debugdata((unsigned char*)&Rcmd, 10, 1);
	debugdata((unsigned char*)&Rcmd.SW1, 2, 1);
#endif
	if(memcmp((unsigned char*)&Rcmd.SW1, "\x90\x00", 2)!=0)// *(unsigned int*)&Rcmd.SW1 != 0x9000 )
	{
		memcpy(pMac, &Rcmd.SW1, 2);
		return ST_ERROR;
	}
	memcpy(pMac, Rcmd.DATA, Rcmd.LE);
	return ST_OK;
}
unsigned char CpuCardSetMac(unsigned char PsamID, unsigned char *pMac, unsigned char *pData, unsigned char len)
{
	ISO7816_ADPU_SEND Scmd;
	ISO7816_ADPU_RESPONSE Rcmd;
	
	Scmd.CLA = 0x80;
	Scmd.INS = 0x70;

	Scmd.P1 = 0;//0x01;//

	Scmd.P2 = 0x00;
	Scmd.LC = len;
	
	//memset(Scmd.DATA, 0, 0x24);
	//Scmd.DATA[10] = 0x05;
	//Scmd.DATA[18] = 0x01;
	
	memcpy(Scmd.DATA, pData, len);
	
	Scmd.LE = 0x00;
#ifdef _debug_
	debugstring("CpuCardSetMac scmd:");
	debugdata(pData, len, 1);
#endif
	IccCommand(PsamID, &Scmd, &Rcmd);
#ifdef _debug_
	debugstring("CpuCardSetMac rcmd:");
	debugdata((unsigned char*)&Rcmd, 10, 1);
	debugdata((unsigned char*)&Rcmd.SW1, 2, 1);
#endif
	if(memcmp((unsigned char*)&Rcmd.SW1, "\x90\x00", 2)!=0)// *(unsigned int*)&Rcmd.SW1 != 0x9000 )
	{
		memcpy(pMac, &Rcmd.SW1, 2);
		return ST_ERROR;
	}
	memcpy(pMac, Rcmd.DATA, Rcmd.LE);
	return ST_OK;
}

#ifdef _GPS_ENABLE_
extern GPS_INFO gGpsInfo;		//GPS信息,应用程序判断的缓冲
// dsLa纬度 daLo经度    
//南北方向1度是111.31955千米！每一分111.31955km/60=1.8553258km=1855.3m，
//而每一分又有60秒,每一秒就代表1855.3m/60=30.92m
//本函数每秒计算次数:19万次, 如果有经纬度为0,返回0xFFFFFFFF
unsigned int GpsDistance(unsigned int dsLa, unsigned int dsLo)
{
	unsigned int t, t1;
// 	debugstring("============\r\n");
// 	debugdata((unsigned char*)&gGpsInfo.Latitude, 4, 1);
// 	debugdata((unsigned char*)&dsLa, 4, 1);
	if((dsLa == 0)||(dsLo == 0))
		return 0xFFFFFFFF;
	if(gGpsInfo.Latitude > dsLa)
		t = gGpsInfo.Latitude - dsLa;
	else
		t = dsLa - gGpsInfo.Latitude;
// 	debugdata((unsigned char*)&gGpsInfo.Longitude, 4, 1);
// 	debugdata((unsigned char*)&dsLo, 4, 1);
	if(gGpsInfo.Longitude > dsLo)
		t1 = gGpsInfo.Longitude - dsLo;
	else
		t1 = dsLo - gGpsInfo.Longitude;
// 	debugdata((unsigned char*)&t, 4, 1);
// 	debugdata((unsigned char*)&t1, 4, 1);
	t /=10;
	t = t*t;
	t1 /= 10;
	t1 = t1*t1;
	t = t + t1;
//	debugdata((unsigned char*)&t, 4, 1);
	t1 = sqrtint(t);
// 	debugdata((unsigned char*)&t1, 4, 1);
// 	debugstring("============\r\n");
	return t1;
}

// dsLa纬度 daLo经度
unsigned int GpsDistanceTow(unsigned int srLa, unsigned int srLo, unsigned int dsLa, unsigned int dsLo)
{
	unsigned int t, t1;
	// 	debugstring("============\r\n");
	// 	debugdata((unsigned char*)&gGpsInfo.Latitude, 4, 1);
	// 	debugdata((unsigned char*)&dsLa, 4, 1);
	if(srLa > dsLa)
		t = srLa - dsLa;
	else
		t = dsLa - srLa;
	// 	debugdata((unsigned char*)&gGpsInfo.Longitude, 4, 1);
	// 	debugdata((unsigned char*)&dsLo, 4, 1);
	if(srLo > dsLo)
		t1 = srLo - dsLo;
	else
		t1 = dsLo - srLo;
	// 	debugdata((unsigned char*)&t, 4, 1);
	// 	debugdata((unsigned char*)&t1, 4, 1);
	t /=100;
	t = t*t;
	t1 /= 100;
	t1 = t1*t1;
	t = t + t1;
	//	debugdata((unsigned char*)&t, 4, 1);
	t1 = sqrtint(t);
	// 	debugdata((unsigned char*)&t1, 4, 1);
	// 	debugstring("============\r\n");
	return t1;
}
#endif
extern fFlagVary_1 F_FLAG_VARY;
extern stLineInfo LineInfo[2];//线路信息定义包括站点信息 正反两组0组 正向    1组反向

void setxfSounding(unsigned char vvl)
{
#ifdef _TTSSound_enable_
	xfSounding = vvl;
#endif
}

#define IDSoundPowerOn 0xa1
#define IDSoundStartST 0xa2
#define IDSoundEndST 0xa3
#define IDSoundOUT 0xa4
#define IDSoundIN1 0xa5
#define IDSoundComm 0xa6
#define IDSoundDefin 0xa7

#define XFStartCompose	0x01		//start Compose sound
#define XFStopCompose	0x02		//stop compose sound
#define XFPauseCompose	0x03		//Pause compose soud
#define XFResumeCompose	0x04		//
#define XFQueryStatues	0x21		//Query XF-s4240 statues
#define XFPowerDown		0x88		//

#define SoundPowerOn "欢迎使用 三联众瑞智能调度系统\x0"

#define SoundStartST1 "各位乘客,欢迎乘坐\x0"
#define SoundStartST2 "路公交车,本公交车从\x0"
#define SoundStartST3 "出发,开往\x0"
#define SoundStartST4 ",祝您出行愉快\x0"

#define SoundEndST1 "各位乘客,终点站\x0"
#ifdef BUS_CIFENG_
#define SoundEndST2 "到了.\x0"
#else
#define SoundEndST2 "到了,请乘客从后门下车\x0"
#endif

#define SoundOUT1 "下一站,\x0"
#ifdef BUS_CIFENG_
#define SoundOUT2 ".\x0"
#else
#define SoundOUT2 ",下车的乘客请准备\x0"
#endif

#ifdef BUS_CIFENG_
#define SoundIN2 "到了.\x0"
#else
#define SoundIN2 "到了,到站的乘客请从后门下车\x0"
#endif
const char SoundComm[5][85]={
	{"各位乘客,请给有需要的人士让坐,谢谢合作\x0"},
	{"各位乘客,车上人多拥挤,请保管好自己的物品\x0"},
	{"各位乘客,上车后请向后门移动,谢谢合作\x0"},
	{"各位乘客,为了大家有良好的乘车环境,请不要在车内吸烟,吐痰,乱扔杂物,谢谢合作\x0"},
	{"各位乘客,非常抱歉,本车发生故障,不能继续前行,我们将安排您转乘后车,谢谢您的理解与合作\x0"}};
	//

#ifdef _TTSSound_enable_
extern void M50TTSsend(unsigned char cmd, unsigned int uiLen, char *str);
void SoundFreeManage(char *sstr)
{
	char buff[200];
	unsigned char i;
	
	memset(buff, 0, 200);
	sysferead(BIT_GPSSOUND_V, 1, (unsigned char*)&i);
	sprintf(buff, "[v%d]", i);
	strcat(buff, sstr);
 		debugstring("sondFree:");
 		debugstring(buff);
	M50TTSsend(0x01, strlen(buff), buff); 
//	xfBusydly = 0;
}
//Rstr:返回的字符
extern unsigned char GetInSound(char *Rstr);
extern void XF_Reset(void);
extern void ADgetInSound(unsigned char drict, unsigned char indd, char *HeadStr, char *endStr);//新的语音

void SoundManage(unsigned char mode, unsigned char index, unsigned char direct)
{
	char strtstar[120];
	char strend[120];
	char strSound[200];
	unsigned int pos=0;
	unsigned int ulV=0;
	unsigned char i=9;
	
	sysferead(BIT_GPSSOUND_V, 1, (unsigned char*)&i);
	memset(strSound, 0, 200);
	sprintf(strSound, "[v%d]", i);

	if(direct > 1)
		direct = 1;	//只有正反两个方向
// 	if(index >= LineInfo[direct].ucStationMax){
// 		return;		//站序号不正确
// 	}
	switch(mode) {
	case IDSoundPowerOn:
		strcat(strSound, SoundPowerOn);
		M50TTSsend(XFStartCompose, strlen(strSound), strSound);
		break;
	case IDSoundStartST:
		strcat(strSound, SoundStartST1);
		pos+=strlen(strSound);
#ifdef BUS_NanjingDZ_
		ulV = BCD2HEX(LineInfo[direct].lineNo, 1);
#else
		ulV = BCD2HEX(LineInfo[direct].lineNo, 2);
#endif
		if(ulV > 99)
			sprintf(strSound+pos, "[n1]%u\x0", ulV);
		else
			sprintf(strSound+pos, "[n2]%u\x0", ulV);
		pos = strlen(strSound);
		strcat(strSound, SoundStartST2);
		strcat(strSound, LineInfo[direct].StationInfo[0].name);
		strcat(strSound, SoundStartST3);
//		i = LineInfo[direct].ucStationMax-1;
		strcat(strSound, LineInfo[direct].StationInfo[LineInfo[direct].ucStationMax-1].name);
		strcat(strSound, SoundStartST4);
		M50TTSsend(XFStartCompose, strlen(strSound), strSound);
		break;
	case IDSoundEndST:
		strcat(strSound, SoundEndST1);
		strcat(strSound, LineInfo[direct].StationInfo[LineInfo[direct].ucStationMax-1].name);
		strcat(strSound, SoundEndST2);
		M50TTSsend(XFStartCompose, strlen(strSound), strSound);
		break;
	case IDSoundOUT:
		//GetInSound(strtemp);
		ADgetInSound(direct, LineInfo[direct].StationInfo[index+1].ucId, strtstar, strend);
		//ADgetInSound(direct, index, strtstar, strend);
		if(strlen(strtstar) == 0){
			strcpy(strtstar, "各位乘客,");
		}
		else{
// 			strcat(strtstar, "提醒您,");
// 		}
//		strcat(strSound, "soundb|");
		strcat(strSound, strtstar);
		strcat(strSound, SoundOUT1);
		strcat(strSound, LineInfo[direct].StationInfo[index+1].name);
		strcat(strSound, SoundOUT2);
		M50TTSsend(XFStartCompose, strlen(strSound), strSound);
		break;
	case IDSoundIN1:
		//GetInSound(strtemp);
		ADgetInSound(direct, LineInfo[direct].StationInfo[index+1].ucId, strtstar, strend);
		//ADgetInSound(direct, index, strtstar, strend);
		if(strlen(strtstar) == 0){
			strcpy(strtstar, "各位乘客,");
		}
// 		else{
// 			strcat(strtstar, "提醒您,");
// 		}
//		strcat(strSound, "soundb,");
		strcat(strSound, strtstar);
		strcat(strSound, LineInfo[direct].StationInfo[index].name);
		if(strlen(strend) == 0){
			strcat(strSound, SoundIN2);	//没有语音尾，用默认的。
		}
		else{//有
			strcat(strSound, "到了,");
			strcat(strSound, strend);
		}
		M50TTSsend(XFStartCompose, strlen(strSound), strSound);
		break;
	case IDSoundComm:
		//GetInSound(strtemp);
		ADgetInSound(2, 2, strSound, strend);//取循环语音，第一参数必须为2，第二参数可以是大于1的任意值。循环在函数内实现。
		if(strlen(strSound) == 0)
			return;
 		M50TTSsend(XFStartCompose, strlen(strSound), strSound);
		break;
	case IDSoundDefin:
		break;
	case IDSoundRound:
		strcpy(strSound, "前方转弯,请站好,扶稳.");
		M50TTSsend(XFStartCompose, strlen(strSound), strSound);
		break;
	default:
		break;
	}
	
#ifdef _debug_
	debugstring("播放:");
	debugstring(strSound);
	debugstring("\r\n");
#endif
}
#endif

unsigned char Keybuffer[10];
volatile unsigned char itKEYB_value;

void uart1_send_byte(unsigned char val)
{
#if defined _New_Bu_mode_ && !defined _GPS_Station_ExKey_enable_
	com_snd(COM4, 1, (unsigned char *)&val);
#else
	com_snd(COM1, 1, (unsigned char *)&val);
#endif
}

void SendCommandkey(unsigned char p1,unsigned char p2)
{
	//	bp_SerialCtl_1(0, 19200);	// 初始化串口1, 0---对外接口, 1---对内接口(连接内部器件, 如IRDA)
	uart1_send_byte(0x1b);	
	uart1_send_byte(p1);
	uart1_send_byte(p2);
}
void LedDisplay(unsigned char *ptr)
{
	unsigned char i=0;
	SendCommandkey('C','0');
	SendCommandkey('S','0'); 
	for(i=0;i<6;i++)
		uart1_send_byte(ptr[i]);
	uart1_send_byte(0x0d);
}
void keyboard_open(void)
{
	SendCommandkey('K','0');
	
	SendCommandkey(0x50,0x33);
	itKEYB_value = 0xff;
}

void KeyBoardDeal(unsigned char vdat)
{
	itKEYB_value = vdat;
}

#ifdef KEYBOARD
/////////////////对键盘进行处理////////////////////
unsigned char KeyMange(unsigned char kkvv);
void KeyDis(unsigned int value,unsigned char *ptr);
extern stPricetable NewPriceTab;

void KeyBoardDeal(unsigned char vdat)
{
	itKEYB_value = vdat;
}
unsigned char KeyDeal(void)
{
	unsigned char j=0, dat;

	if(itKEYB_value == 0xFF){
		return ST_ERROR;
	}
	else
		dat = itKEYB_value;
	if((dat>='0')&&(dat<='9'))
	{
		for(j=0;j<5;j++)
		{
			if(memcmp(Keybuffer,"\x20\x20\x20\x20\x20",5)==0)
			if(Keybuffer[5]=='0')
				Keybuffer[5]=0x20;
			Keybuffer[j]=Keybuffer[j+1];
		}
		Keybuffer[5]=dat;
	}
	else if(dat==0x08)//清除键
	{
		for(j=0;j<5;j++)
		{
			Keybuffer[5-j]=Keybuffer[4-j];
		}
		if(Keybuffer[0]!=0x20)
			Keybuffer[0]=0x20;

		if(Keybuffer[5]==0x20)
		{
			memset(Keybuffer,0x20,5);
			Keybuffer[5]='0';
		}
	}
	else if(dat==0x0d)//确定键
	{
		if((Keybuffer[5]&0x80)==0x80)
		{
			memset(Keybuffer,0x20,5);
			Keybuffer[5]='0';
		}
		
	}
	else if(dat==0x2e)
	{
		Keybuffer[5]|=0x80;
	}//  41=F1键  42=F2键
	else{
		itKEYB_value = 0xff;
		return ST_ERROR;
	}
	itKEYB_value = 0xff;
	if(KeyMange(dat) == ST_OK){
		LedDisplay(Keybuffer);
	}
	return ST_OK;
}
void KeyDis(unsigned int value,unsigned char *ptr)
{
	unsigned char i;
	unsigned char buffer1[20];

	memset(buffer1,0,20);
	sprintf((char*)buffer1,"%-7u",value);
	for(i=0;i<14;i++)
	{
		if(buffer1[i]==0x20)
			break;
	}
	
	if(i==2)
	{
		ptr[0]='0';
		ptr[0]|=0x80;
		memcpy(ptr+1,buffer1,2);
		ptr[4]=0x20;
	}
	else if(i==1)
	{
		ptr[0]='0';
		ptr[0]|=0x80;
		ptr[1]='0';
		ptr[2]=buffer1[0];
		ptr[3]=0x20;
	}
	else if(i>2)
	{
//		i-=2;
		memcpy(ptr,buffer1,i);
		i-=3;
		ptr[i]|=0x80;
	}
	memmove(ptr+3,ptr,3);
	memset(ptr,0x20,3);	
}
unsigned char KeyMange(unsigned char kkvv)
{
	unsigned char i=0,a=0,j=0;
	unsigned int ii=0,KeyVa;
	a=0;
	//小数点不能超过两个
	for(i=0;i<6;i++)
	{
		if((Keybuffer[5-i]&0x80)==0x80)
			a++;
	}
	//寻找小数点的位置
	for(i=0;i<6;i++)
	{
		if((Keybuffer[5-i]&0x80)==0x80)
			break;
	}
	if(((i>2)&&(i!=6))||(a>1))
	{
		beep(2,50,50);
		memset(Keybuffer,0x20,5);
		Keybuffer[5]='0';
		KeyVa=0;
	} 
	else
	{
		a=0;
		KeyVa=0;
		for(j=0;j<6;j++)
		{
			if(Keybuffer[5-j]!=0x20)
				a++;
		}
		if(i==1)
		{
			KeyVa=(unsigned int)((Keybuffer[5]-'0')*10);

		}
		else if(i==2)
		{
			KeyVa=(unsigned int)((Keybuffer[5]-'0')+((Keybuffer[4]-'0')*10));

		}
		else if(i==6)
		{
             i=0;
		}
		ii=10;
		for(j=i;j<a;j++)
		{
			ii=ii*10;
			KeyVa+=(unsigned int)(((Keybuffer[5-j]-'0')&0x7f)*ii);
		}
#ifdef BUS_HANDAN_
		if(KeyVa>=1000)
#else
		if(KeyVa>10000)
#endif
		{
			beep(2,50,50);
			memset(Keybuffer,0x20,5);
			Keybuffer[5]='0';
			KeyVa=0;
			KeyDis(KeyVa,Keybuffer);
			LedDisplay(Keybuffer);
			memset(Keybuffer,0x20,5);
			Keybuffer[5]='0';
			KeyVa=0;
			return ST_ERROR;
		}

	}
	if(kkvv==0x0d)	//确定键
	{
		if(KeyVa==0)
		{
			beep(2,50,50);	
			memcpy((unsigned char*)&ii, NewPriceTab.busPrice, 4);
			KeyDis(ii,Keybuffer);
			LedDisplay(Keybuffer);

			memset(Keybuffer,0x20,6);
			Keybuffer[5]='0';
			return ST_ERROR;
		}
		else
		{
			memcpy(NewPriceTab.busPrice, (unsigned char*)&KeyVa, 4);
			beep(1,500,25);
			KeyDis(KeyVa,Keybuffer);
			LedDisplay(Keybuffer);
			memset(Keybuffer,0x20,5);
			Keybuffer[5]='0';
			restore_flag=1;
			
			return ST_ERROR;
		}
	}
	else
	{
		KeyVa=0;
	}
	return ST_OK;
}
#endif

void debugstringCOM(unsigned char com, char *str)
{
	com_snd(com, strlen(str),(unsigned char *)str);
}
void debugdataCOM(unsigned char com, unsigned char *value, unsigned int uclen, unsigned char mode)
{
	unsigned char debugbuf[2];
	unsigned int i;
	
	if(uclen == 0)
		return;
	//	BCD2Ascii(value,debugbuf,uclen);
	for(i = 0;i < uclen; i++)
	{
		BCD2Ascii(value, debugbuf, 1);
		com_snd(com, 2,debugbuf);
		com_snd(com, 1,(unsigned char*)" ");
		value++;
	}
	if(mode == 1)
		com_snd(com, 2,(unsigned char*)"\r\n");
}

void debugstringC(char *str, unsigned char mode)
{
	if(mode & 0x80)
		com_snd(COM4, strlen(str),(unsigned char *)str);
	else
		com_snd(COM1, strlen(str),(unsigned char *)str);
}

#if(GPS_COM==3)//UART1 被占用，使用URT4
void debugstring(char *str)
{
	com_snd(COM4, strlen(str),(unsigned char *)str);
}
void debugdata(unsigned char *value, unsigned int uclen, unsigned char mode)
{
	unsigned char debugbuf[2];
	unsigned int i;
	unsigned char comm;
	
	comm = COM4;
	
	if(uclen == 0)
		return;
	
//	BCD2Ascii(value,debugbuf,uclen);
	for(i = 0;i < uclen; i++)
	{
		clr_wdt();
		BCD2Ascii(value, debugbuf, 1);
		com_snd(comm, 2,debugbuf);
		if(mode&0x20)
			com_snd(comm, 1,(unsigned char*)" ");
		value++;
	}
	if((mode&0x01) == 1)
		com_snd(comm, 2,(unsigned char*)"\r\n");
}
#else
void debugstring4(char *str)
{
	com_snd(COM4, strlen(str),(unsigned char *)str);
}
void debugstring(char *str)
{
	com_snd(COM1, strlen(str),(unsigned char *)str);
}
void debugdata(unsigned char *value, unsigned int uclen, unsigned char mode)
{
	unsigned char debugbuf[2];
	unsigned int i;
	unsigned char comm;
	
	if(mode & 0x80)
		comm = COM4;
	else
		comm = COM1;
	
	mode &= 0x7f;//去掉最高位
	
	if(uclen == 0)
		return;
	
	//	BCD2Ascii(value,debugbuf,uclen);
	for(i = 0;i < uclen; i++)
	{
		BCD2Ascii(value, debugbuf, 1);
		com_snd(comm, 2,debugbuf);
		if(mode&0x20)
			com_snd(comm, 1,(unsigned char*)" ");
		value++;
	}
	if((mode&0x01) == 1)
		com_snd(comm, 2,(unsigned char*)"\r\n");
}

#endif

#ifdef KEY_PAD
#ifdef _GPS_DIAODU_
//调度信息
extern stDiaoDuCMD gDiaoDuCMD;
//调度营运信息
extern stDiaoDuinfo gDiaoDuinfo;
extern void Clr_DiaoDuCmd_info(void);
#endif
extern stcomtime SysTime;

//ba 03 9b 01 23  其中01代表100ms 23是校验和可以改成别的时间ba 03 9b不变 02代表200ms 以此类推校验和也要算过
void KeyPad_hander(unsigned char val)//08 2E 0D 43 42 41
{
	if(gKeyPadIndex != 0){
		gKeyPadIndex++;
		if(gKeyPadIndex < 5)
			return;//协义数据
		else{//协议数据完成,不解析应答数据。
			gKeyPadIndex = 0;
			gKEYPAD_flag = 11;
			return;
		}
	}

	if(val == 0xBD)
	{
		gKeyPadIndex = 1;
	}
	else  if((val>='0')&&(val<='9')){//只接收按键数据。
		gkeyPadValue =val;
	}
	else if((val == 0x08)||(val == 0x2E)||(val ==0x0D)||((val>=0x41)&&(val<=0x43))){
		gkeyPadValue =val;
	}

}
void KeyPadDis(unsigned char line,unsigned char col, char*dat, unsigned char len)
{
	unsigned char t;
	unsigned char buff[40];
	unsigned char maxnum;
	unsigned int dly=0;

	if(line&0x80){//中文一次只能发一个字
		t = 1;
		maxnum = 5;
	}
	else{
		t = len;
		if(t>15)
			t = 15;
		maxnum = t+4;//20;
	}
	if(t <= 0)
		return;

	memset(buff, 0, 40);
	buff[0] = 0xba;
	buff[1] = maxnum;//16+4
	if(line&0x80)
		buff[2] = 0x95;
	else
		buff[2] = 0x92;
	buff[3] = (line&0x7F);
	buff[4] = col;

	memcpy(buff+5, dat, t);
	buff[maxnum+1] = 0;
	for(t=0;t<(maxnum+1);t++){
		buff[maxnum+1] ^= buff[t];
	}
	gKEYPAD_flag = 0;
	gKeyPadIndex = 0;
	com_snd(COM4, maxnum+2, buff);	
//	com_snd(COM1, maxnum+2, buff);
	while(gKEYPAD_flag == 0){
		dly++;
		if(dly >= 0x0003DC80){//timeout
//			debugstring("E");
			break;
		}
	}
}

void KeyPadClr(void)
{
	unsigned char buff[20];
	
	buff[0] = 0xBA;
	buff[1] = 0x02;
	buff[2] = 0x93;
	buff[3] = 0x2B;
	
	com_snd(COM4, 4, buff);//清除显示命令
	memset(buff, 0, 20);
	KeyPadDis(0, 0, (char*)buff, 15);//清除显示存内容
	KeyPadDis(1, 0, (char*)buff, 15);
	gKeyPadIndex = 0;
	gkeyPadValue = 0;
	gKEYPAD_flag = 0;
	gKeyMode = 0;
}
void KeyPad_restordis(void)
{
	unsigned char k;
	unsigned char i;
	char disbuff[128];
	unsigned char tbuff[20];

	KeyPadClr();

	sprintf(disbuff, "Code:%02d", galamessage.alamFlag);
	if(galamessage.style ==0x5A){//正在发送
		i = 0;
		KeyPadDis(0x01, i, disbuff, 7);
		i=8;
		k=0x26;//发
		KeyPadDis(0x80, i, (char*)&k, 1); i+=16;
		k=0x27;//送
		KeyPadDis(0x80, i, (char*)&k, 1); i+=16;
		k=0x25;//中
		KeyPadDis(0x80, i, (char*)&k, 1); i+=16;
// 		memset(disbuff, 0, 10);
// 		disbuff[0] = '.';
// 		disbuff[1] = '.';
// 		disbuff[2] = '.';
// 		KeyPadDis(0x00, i, disbuff, 3); i+=24;
	}
	else if((galamessage.style >= 0xE0)&&(galamessage.style <= 0xE2)){//E0发送完成 E1失败 E2 取消
		i = 0;
		KeyPadDis(0x01, i, disbuff, 7);
		i=8;
		k=0x26;//发
		KeyPadDis(0x80, i, (char*)&k, 1); i+=16;
		k=0x27;//送
		KeyPadDis(0x80, i, (char*)&k, 1); i+=16;
		if(galamessage.style == 0xE0)
			k=0x2A;//成
		else if(galamessage.style == 0xE1)
			k=0x2C;//失
		else// if(galamessage.style == 0xE2)
			k=0x02;//取
		KeyPadDis(0x80, i, (char*)&k, 1); i+=16;
		if(galamessage.style == 0xE0){
			k=0x2B;//功
			strcpy(disbuff, "信息发送成功");
			M50TTSsend(XFStartCompose, strlen(disbuff), disbuff);
		}
		else if(galamessage.style == 0xE1)
			k=0x2D;//败
		else// if(galamessage.style == 0xE2)
			k=0x03;//消
		KeyPadDis(0x80, i, (char*)&k, 1); i+=16;
		memset((unsigned char*)&galamessage, 0, sizeof(stAlammessage));//完成，清除
	}
	else if((gDiaoDuCMD.ucCMD == 0x13)||(gDiaoDuCMD.ucCMD == 0x14)){//需要显示文字,现在的设备显示不了,语音
		i=8;
		k=0x1A;//有
		KeyPadDis(0x81, i, (char*)&k, 1); i+=16;
		k=0x3D;//信
		KeyPadDis(0x81, i, (char*)&k, 1); i+=16;
		k=0x17;//息
		KeyPadDis(0x81, i, (char*)&k, 1); i+=16;

		memset(disbuff, 0, 128);
		sysferead(BIT_GPS_MESSAGE, 65, (unsigned char*)disbuff);
		sysferead(BIT_GPS_MESSAGE+65, 7, tbuff);
//		display(0,0,disbuff,0);
		M50TTSsend(XFStartCompose, strlen(disbuff), disbuff);
		gDiaoDuCMD.ucCMD = 0;//清除显示状态，可以按确定键再次显示
		Save_DIAODUCMD_info();
	}
	else if((gDiaoDuCMD.ucCMD == 0x10)||(gDiaoDuCMD.ucCMD == 0x11)||\
		(gDiaoDuCMD.ucDDstyle == 0x10)||(gDiaoDuCMD.ucDDstyle == 0x11)){//调度信息显示
		i = 0;
		k=0x28;//请
		KeyPadDis(0x81, i, (char*)&k, 1); i+=16;
		sprintf(disbuff, "%02X:%02X",gDiaoDuCMD.FCTime[0],gDiaoDuCMD.FCTime[1]);
		KeyPadDis(0x01, i, disbuff, 5); i+=(5*8);
		k = 0x26;//发
		KeyPadDis(0x81, i, (char*)&k, 1); i+=16;
		k = 0x11;//车
		KeyPadDis(0x81, i, (char*)&k, 1); i+=16;
		
		sprintf(disbuff, "soundb,请于%02X点 %02X分 发车",gDiaoDuCMD.FCTime[0],gDiaoDuCMD.FCTime[1]);
		M50TTSsend(XFStartCompose, strlen(disbuff), disbuff);
	}
	else{
		if(gGPsPar.EnableGps == 0){
			memcpy(disbuff, "Line info Err!", 14);
			KeyPadDis(0x00, 0, disbuff, 14);
			return;
		}
		if(RunStation.ucDirect<2){
			i = 0;
			k=0x30;//正
			KeyPadDis(0x81, i, (char*)&k, 1); i+=16;
			k=0x32;//向
			KeyPadDis(0x81, i, (char*)&k, 1); i+=16;

			sprintf(disbuff, "Station:%d ", RunStation.ucStation);
			KeyPadDis(0x00, 0, disbuff, strlen(disbuff));
			return;
		}
		else{
			i = 8;
			k=0x39;//等
			KeyPadDis(0x81, i, (char*)&k, 1); i+=16;
			k=0x3A;//待
			KeyPadDis(0x81, i, (char*)&k, 1); i+=16;
			k=0x3B;//调
			KeyPadDis(0x81, i, (char*)&k, 1); i+=16;
			k=0x3C;//度
			KeyPadDis(0x81, i, (char*)&k, 1); i+=16;
			k=0x3D;//信
			KeyPadDis(0x81, i, (char*)&k, 1); i+=16;
			k=0x17;//息
			KeyPadDis(0x81, i, (char*)&k, 1); i+=16;
			k='!';//!
			KeyPadDis(0x01, i, (char*)&k, 1); i+=8;
		}
	}

} 
//
unsigned char GPRS_Ring_sta(void)
{
	unsigned char k;
	unsigned char i;
	char buff[20];

	KeyPadClr();
	gKeyMode = 0xF9;//处于F3功能
	i = 0;
	k = 0x1A;//有
	KeyPadDis(0x81, i, (char*)&k, 1); i+=16;
	k = 0x33;//来
	KeyPadDis(0x81, i, (char*)&k, 1); i+=16;
	k = 0x34;//电
	KeyPadDis(0x81, i, (char*)&k, 1); i+=16;
	
	i = 0;
	strcpy(buff, "1.");
	KeyPadDis(0x00, i, buff, 2); i += 16;
	k = 0x35;//接
	KeyPadDis(0x80, i, (char*)&k, 1); i+=16;
	k = 0x36;//听
	KeyPadDis(0x80, i, (char*)&k, 1); i+=16;
	strcpy(buff, " 9.");
	KeyPadDis(0x00, i, buff, 3); i += 24;
	k = 0x37;//挂
	KeyPadDis(0x80, i, (char*)&k, 1); i+=16;
	k = 0x38;//断
	KeyPadDis(0x80, i, (char*)&k, 1); i+=16;
	
	cls();
	display(0,0,"有来电!",DIS_CENTER);
	display(3,0,"C:挂断  OK:接听",0);

//ba 03 9b 01 23  其中01代表100ms 23是校验和可以改成别的时间ba 03 9b不变 02代表200ms 以此类推校验和也要算过
	memcpy(buff, "\xba\x03\x9b\x05\x27", 5);//测试键盘声音（调度用键盘）
	com_snd(COM4, 5, (unsigned char*)buff);
	return 0;
}
//
unsigned char GPRS_Ring_ing(void)
{
	unsigned char k;
	unsigned char i;
	char buff[20];
	
	KeyPadClr();
	i = 0;
	k = 0x1F;//通
	KeyPadDis(0x81, i, (char*)&k, 1); i+=16;
	k = 0x20;//话
	KeyPadDis(0x81, i, (char*)&k, 1); i+=16;
	k = 0x25;//中
	KeyPadDis(0x81, i, (char*)&k, 1); i+=16;
	
	i = 0;
	k = 0x02;//取
	KeyPadDis(0x80, i, (char*)&k, 1); i+=16;
	k = 0x03;//消
	KeyPadDis(0x80, i, (char*)&k, 1); i+=16;
	strcpy(buff, " : ");
	KeyPadDis(0x00, i, buff, 3); i += 24;
	k = 0x37;//挂
	KeyPadDis(0x80, i, (char*)&k, 1); i+=16;
	k = 0x38;//断
	KeyPadDis(0x80, i, (char*)&k, 1); i+=16;
	
	cls();
	display(0,0,"通话中!",DIS_CENTER);
	return 0;
}
unsigned char GPSQQPBmenu(void)
{
	unsigned char k;
	unsigned char i;
	char buff[20];

	KeyPadClr();
	strcpy(buff, "1.");
	i = 0;
	KeyPadDis(0x01, i, buff, 2); i += 16;
	k = 0x0E;//盗
	KeyPadDis(0x81, i, (char*)&k, 1); i+=16;
	k = 0x0F;//抢
	KeyPadDis(0x81, i, (char*)&k, 1); i+=16;
	strcpy(buff, " 2.");
	KeyPadDis(0x01, i, buff, 3); i += 24;
	k = 0x0C;//拥
	KeyPadDis(0x81, i, (char*)&k, 1); i+=16;
	k = 0x0D;//挤
	KeyPadDis(0x81, i, (char*)&k, 1); i+=16;
	
	strcpy(buff, "4.");
	i = 0;
	KeyPadDis(0x00, i, buff, 2); i += 16;
	k = 0x10;//塞
	KeyPadDis(0x80, i, (char*)&k, 1); i+=16;
	k = 0x11;//车
	KeyPadDis(0x80, i, (char*)&k, 1); i+=16;
	strcpy(buff, " 5.");
	KeyPadDis(0x00, i, buff, 3); i += 24;
	k = 0x12;//故
	KeyPadDis(0x80, i, (char*)&k, 1); i+=16;
	k = 0x13;//障
	KeyPadDis(0x80, i, (char*)&k, 1); i+=16;
	return 0;
}
unsigned char GPSYinYunmenu(void)
{
	unsigned char k;
	unsigned char i;
	char buff[20];
	
	KeyPadClr();
	strcpy(buff, "1.");
	i = 0;
	KeyPadDis(0x01, i, buff, 2); i += 16;
	k = 0x06;//停
	KeyPadDis(0x81, i, (char*)&k, 1); i+=16;
	k = 0x07;//运
	KeyPadDis(0x81, i, (char*)&k, 1); i+=16;
	strcpy(buff, " 2.");
	KeyPadDis(0x01, i, buff, 3); i += 24;
	k = 0x14;//排
	KeyPadDis(0x81, i, (char*)&k, 1); i+=16;
	k = 0x15;//班
	KeyPadDis(0x81, i, (char*)&k, 1); i+=16;
	strcpy(buff, "4.");
	i = 0;
	KeyPadDis(0x00, i, buff, 2); i += 16;
	k = 0x1F;//通
	KeyPadDis(0x80, i, (char*)&k, 1); i+=16;
	k = 0x20;//话
	KeyPadDis(0x80, i, (char*)&k, 1); i+=16;
	strcpy(buff, " 5.");
	KeyPadDis(0x00, i, buff, 3); i += 24;
	k = 0x16;//休
	KeyPadDis(0x80, i, (char*)&k, 1); i+=16;
	k = 0x17;//息
	KeyPadDis(0x80, i, (char*)&k, 1); i+=16;
	return 0;
}

unsigned char GPSHuangXiangmenu(void)
{
	unsigned char k;
	unsigned char i;
	char buff[20];
	
	KeyPadClr();
	strcpy(buff, "1.");
	i = 0;
	KeyPadDis(0x01, i, buff, 2); i += 16;
	k = 0x30;//正
	KeyPadDis(0x81, i, (char*)&k, 1); i+=16;
	k = 0x32;//向
	KeyPadDis(0x81, i, (char*)&k, 1); i+=16;
	strcpy(buff, " 9.");
	KeyPadDis(0x01, i, buff, 3); i += 24;
	k = 0x31;//反
	KeyPadDis(0x81, i, (char*)&k, 1); i+=16;
	k = 0x32;//向
	KeyPadDis(0x81, i, (char*)&k, 1); i+=16;
	return 0;
}
void KeyPad_main(void)
{
	unsigned char buff[20];
	char disbuff[128];
	int i,t;
	
	if((gDiaoDuCMD.ucCMD == 0x10)||(gDiaoDuCMD.ucCMD == 0x11)||\
		(gDiaoDuCMD.ucDDstyle == 0x10)||(gDiaoDuCMD.ucDDstyle == 0x11))//有调度信息，需要判断是否过期
	{
		if(memcmp(gDiaoDuCMD.Date, (unsigned char*)&SysTime, 4) != 0){//不是当天的信息
			Clr_DiaoDuCmd_info();
		}
		else{
			i=((gDiaoDuCMD.FCTime[0]>>4)*10);
			i+=(gDiaoDuCMD.FCTime[0]&0x0F);
			i*=60;
			i+=((gDiaoDuCMD.FCTime[1]>>4)*10);
			i+=(gDiaoDuCMD.FCTime[1]&0x0F);
			
			t=((SysTime.hours>>4)*10);
			t+=(SysTime.hours&0x0F);
			t*=60;
			t+=((SysTime.minutes>>4)*10);
			t+=(SysTime.minutes&0x0F);
			t -= i;
			if(t >= 0){//超过时间，已经无效
				Clr_DiaoDuCmd_info();
			}
		}
	}

	if(gKeyRestorDis != 0){
		KeyPad_restordis();
		gKeyRestorDis = 0;
	}
	switch(gkeyPadValue)
	{
	case '0':
		break;
	case '1':
		if(gKeyMode == 0xF1){//F1盗抢
			gKeyRestorDis = 3;
			gKeyMode = 0;
			galamessage.alamFlag = ALARM_DQ;
			galamessage.style = 0x5A;
			galamessage.reSendTimes = 0;
		}
		else if(gKeyMode == 0xF2){//停运
			gKeyRestorDis = 3;
			gKeyMode = 0;
			galamessage.alamFlag = ALARM_TC;
			galamessage.style = 0x5A;
			galamessage.reSendTimes = 0;
			gDiaoDuCMD.ucStartYorN = 2;
			Save_DIAODUCMD_info();
		}
		else if(gKeyMode == 0xF3){//正向
			gKeyRestorDis = 3;
			gKeyMode = 0;
		}
		else if(gKeyMode == 0xF9){//接听电话
#ifdef GPRS_SOUD_ENABLE
			memcpy(buff, "ATA\r\n",5);
			gprs_snd(5, (char*)buff);
			gKeyMode = 0;
			GPRS_Ring_ing();
#endif
		}
		break;
	case '2':
		if(gKeyMode == 0xF1){//F1拥挤
			gKeyRestorDis = 3;
			gKeyMode = 0;
			galamessage.alamFlag = ALARM_YJ;
			galamessage.style = 0x5A;
			galamessage.reSendTimes = 0;
		}
		else if(gKeyMode == 0xF2){//排班
			gKeyRestorDis = 3;
			gKeyMode = 0;
			galamessage.alamFlag = ALARM_YY;
			galamessage.style = 0x5A;
			galamessage.reSendTimes = 0;
			gDiaoDuCMD.ucStartYorN = 0;
			Save_DIAODUCMD_info();
		}
		break;
	case '4':
		if(gKeyMode == 0xF1){//F1塞车
			gKeyRestorDis = 3;
			gKeyMode = 0;
			galamessage.alamFlag = ALARM_SC;
			galamessage.style = 0x5A;
			galamessage.reSendTimes = 0;
		}
		else if(gKeyMode == 0xF2){//F2通话
			gKeyRestorDis = 3;
			gKeyMode = 0;
			galamessage.alamFlag = ALARM_TH;
			galamessage.style = 0x5A;
			galamessage.reSendTimes = 0;
		}
		break;
	case '5':
		if(gKeyMode == 0xF1){//F1故障
			gKeyRestorDis = 3;
			gKeyMode = 0;
			galamessage.alamFlag = ALARM_GZ;
			galamessage.style = 0x5A;
			galamessage.reSendTimes = 0;
		}
		else if(gKeyMode == 0xF2){//F2休息
			gKeyRestorDis = 3;
			gKeyMode = 0;
			galamessage.alamFlag = ALARM_XX;
			galamessage.style = 0x5A;
			galamessage.reSendTimes = 0;
			gDiaoDuCMD.ucStartYorN = 2;
			Save_DIAODUCMD_info();
		}
		break;
	case '9':
		if(gKeyMode == 0xF3){//反向
			gKeyRestorDis = 3;
			gKeyMode = 0;
		}
		else if(gKeyMode == 0xF9){//挂断电话
#ifdef GPRS_SOUD_ENABLE
			ucMC323Mode = MC323_Ring_END;
			gKeyRestorDis = 3;
			restore_flag = 3;
			gKeyMode = 0;
#else
			;
#endif
		}
		break;
	case '3':
	case '6':
	case '7':
	case '8':
		break;
	case 0x0d:
		sysferead(BIT_GPS_MESSAGE+65, 7, buff);
		if(time_invalid(buff) == 1){//有效
			sysferead(BIT_GPS_MESSAGE, 65, (unsigned char*)disbuff);
//			display(0,0,disbuff, DIS_Cls);
			M50TTSsend(XFStartCompose, strlen(disbuff), disbuff);
			memset(disbuff, 0, 20);
			BCD2Ascii(buff, (unsigned char*)disbuff, 7);
			KeyPadDis(0,0,disbuff,14);
			g24GDisFlash = 2;
		}
		break;
	case 0x2E:
		break;
	case 0x43:
		if(galamessage.style == 0x5A)
			break;
		GPSQQPBmenu();
		gKeyMode = 0xF1;//处于F1功能
		break;
	case 0x42:
		if(galamessage.style == 0x5A)
			break;
		GPSYinYunmenu();
		gKeyMode = 0xF2;//处于F2功能
		break;
	case 0x41:
// 		if(galamessage.style == 0x5A)
// 			break;
		GPSHuangXiangmenu();
		gKeyMode = 0xF3;//处于F3功能
		break;
	case 0x08:
		if(gKeyMode == 0xF9){//挂断电话
#ifdef GPRS_SOUD_ENABLE
			ucMC323Mode = MC323_Ring_END;
			restore_flag = 3;
#endif
		}
#ifdef GPRS_SOUD_ENABLE
		if((ucMC323Mode&0xF0) == 0xA0){
			ucMC323Mode = MC323_Ring_END;
			restore_flag = 3;
		}
#endif
		if(galamessage.style == 0x5A){//有报警命令发送
			galamessage.style = 0xE2;//取消了
			galamessage.reSendTimes = 0;
			gSendOverTime = 0;
			if((GPRSLinkProcess != 0x16)&&(GPRSLinkProcess != 0xA0)){//还没有登陆
				gKeyRestorDis = 3;
			}
			break;
		}
		gKeyMode = 0;
		gKeyRestorDis = 3;
		break;
	default:
		break;
	}
	gkeyPadValue = 0;
}
#else 
void KeyPad_restordis(void)
{
	return;
}
unsigned char GPRS_Ring_sta(void)
{
	return 0;
}
#endif
void in_deal_ring(void)
{
#ifdef GPRS_SOUD_ENABLE
	char buff[20];
	if(ucMC323Mode == MC323_Ring_sta){//有来电，需要显示提醒
		GPRS_Ring_sta();
		beep(1,50,0);
		ucMC323Mode = MC323_Ring_key;
	}
	else if(ucMC323Mode == MC323_Ring_END){//通话结束，挂断
		memcpy(buff, "AT+CHUP\r\n",9);
		gprs_snd(9, (char*)buff);
		ucMC323Mode = 0;
		restore_flag = 3;
		
		set_timer0(2000, 1);
	}
#endif //GPRS_SOUD_ENABLE
	return;
}

int in_dealSoundIn(void)
{
#ifdef GPRS_SOUD_ENABLE
	if(strstr((char*)gprsRevBuf, "RING") != 0)//有电话呼入
	{
		ucMC323Mode = MC323_Ring_sta;
#ifdef _debug_gprs
		debugstring("Ring...\r\n");
#endif
	}
	if((ucMC323Mode&0xF0) == 0xA0){
		if(strstr((char*)gprsRevBuf, "NO CARRIER") != 0)//电话呼入挂断
		{
			ucMC323Mode = 0;
			restore_flag = 3;
			KeyPad_restordis();
#ifdef _debug_gprs
			debugstring("ring end!!\r\n");
#endif
		}
		if((GPRSLinkProcess < 0xB0)&&(GPRSLinkProcess > 5))
			return 1;
	}
	else{
		if(strstr((char*)gprsRevBuf, "NO CARRIER") != 0)//电话呼入挂断
		{
			ucMC323Mode = MC323_Ring_END;
			restore_flag = 3;
#ifdef _debug_gprs
			debugstring("ring end WOkA!!\r\n");
#endif
		}
	}
	if((GPRSLinkProcess == GPRS_AT_WAIT)||((ucMC323Mode&0xF0) == 0xA0))
		return 1;
#else
	if(GPRSLinkProcess == GPRS_AT_WAIT)
		return 1;
#endif
	return 0;
}

int in_checkRingstyle(void)
{
#ifdef GPRS_SOUD_ENABLE
	if(((ucMC323Mode&0xF0) == 0xA0)&&(GPRSLinkProcess < 0xB0)&&(GPRSLinkProcess > 5)){//有电话时不做其它事。
		return 1;	
	}
#endif
	return 0;
}

void setM50TTS(unsigned char vi)
{
	return;//MC65
// 	char gbuf[32];		//M50,需要
// 	
// 	if(vi == 0)
// 		vi = 5;
// 	else if(vi > 10)
// 		vi = 10;
// 		vi *= 10;
// 	sprintf(gbuf, "AT+CLVL=%d\r\n", vi);  /* 增益0-100 */
// 	gprs_snd(strlen(gbuf), gbuf);
// 	delayxms(100);
}
