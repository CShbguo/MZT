
/*
	国民技术2.4G读头控制CPU卡。没有用到移动的功能

*/

#include "string.h"
#include "stdio.h"
#include "szct.h"

#ifdef _CPU_24G_
#include "..\include\include.h"
#include "E_GJGPRS.h"
#include "Cpucard.h"


#define PSAM_JSB		SAM4
typedef struct  {
	unsigned char Sec;
	unsigned char CardSno[8];
} stg24card_Delay;
stg24card_Delay g24PCard_delay;//G24卡

extern unsigned char G24PsamNo[6];

//unsigned char g24PSAMDsno[4];//终端序列号
typea_public_inf_zaozuang stuPubInfZaoZuang;
//pboc_debit_capp_purchase stuDebitPurchase;
//pboc_sam_init_purchase stuSamInitPurchase;
//pboc_init_capp_purchase_res stuInitPurchaseRet;

unsigned char gG24CardNo[10];//卡号，钱包卡号

unsigned char gG24Ver;//读头版本标识 0xA1 0xA2 0xA3 0xA4(版本号没取到)
unsigned char gG24VerBuf[21];//读头版本
unsigned char gG24ReaderErr;//读头错误次数
unsigned char g24PurError;//刷卡标志错误，需要请重刷上一张卡
volatile unsigned char g24ComFlag;//串口接收标志
unsigned char g24ComRBuf[500];
unsigned int g24ComRindex;
unsigned short com1Rlen;//接收数据的长度。
unsigned char g24GProFlag;//24G卡询卡指针 0:需要发询卡命令 其它:己发询卡命令,等回应卡信息


#define bzero(a,b) memset(a,0,b)
#define SELECT_APPY_MF	"\x00\xA4\x04\x00\x0E\xD1\x56\x00\x00\x15\x4D\x5A\x54\x2E\x44\x44\x46\x30\x31"//19字节，MF是不一样的，1001不清楚为什么和天府通定义的一样。如果我们终端先选择MF的AID，天府通双应用卡在绵阳也不受影响。
#define SELECT_APPY_1001	"\x00\xa4\x04\x00\x0d\xD1\x56\x00\x00\x15\xB9\xAB\xB9\xB2\xD3\xA6\xD3\xC3"	//18字节 绵阳的卡
#define SELECT_APPY_NEW		"\x00\xA4\x04\x00\x10\xD1\x56\x00\x00\x15\x4D\x59\x4D\x5A\x54\x2E\x41\x44\x46\x30\x31"//绵州通新卡21字节  原来的和成都的一样，为了区分用新的了，先找这个aid
// 0xD156000015 + MZT.DDF01
#define NOTENABLED 0x01
#define ENABLED    0x02
#define DISABLED   0x03
//add hb
extern unsigned char YouHui_Area_other;//控制折扣
extern unsigned char YouHui_Area ;//如果Card_Area在规定的特殊卡类区域，为1，可以实现优惠乘车；
extern unsigned char YouHui_Flag; //特殊卡类优惠交易:1    其他:0   用于统计优惠乘车记录
extern unsigned char Card_Area ;//城区代码   0x80:北川  0x40:盐亭 0x20:三台  0x10:梓潼  0x08:安县  0x04:平武县  0x02:江油市  0x01:绵阳城区
extern unsigned char YouHui_Date[4];
extern unsigned char keyIndex	;	// 本地卡索引0x11  异地卡 01

extern void SoundMessage(unsigned char cmd);
extern void money_msg(unsigned char dmode, unsigned int remM, unsigned int pucM, unsigned char cMOde);
extern void dis_time_mini(unsigned char x, stcomtime *time);
extern unsigned char WriteRecord(unsigned char *buffer, unsigned char mode);
extern void BlightOpen(void);
extern void BuildRecorde(unsigned char delType, unsigned char *recBuf);
extern void save_normal(void);
extern void save_h_month(void);
extern unsigned char month_decide(void);
extern void CountPoint(unsigned char mode);
extern unsigned int IsInBlackList(unsigned char *c_card_no);
extern unsigned char FreeConsume(void);
extern void disp_no_swipe(void);
extern void error(unsigned char code1,unsigned char Index);
extern void ControlDis(unsigned char type);
#ifndef NO_GJ_SERVISE
extern void SetTcpPacketTTLTime(unsigned char val);
#endif

//有些卡需要区分男女声
extern unsigned char nnAudioflag;

//卡类型:手机卡03\艺达卡 04\公交卡01\CPU卡02
extern unsigned char gMCardCand;
extern stPricetable NewPriceTab;
extern Parameter5 cardMessage;
extern unsigned int a_sumR;//手机钱包中的真实金额
extern unsigned char DriveCardNo[4];
extern pFistVary_1 pFistVary;
extern Parameter3 ComSumeFile;
extern unsigned int c_serial;
extern unsigned char cardSound;			//刷卡没有成功标志
extern unsigned int card_ser;
extern Parameter2 POINT_MESSAGE;
extern unsigned char gchn_eng;//中英文标志
extern unsigned char BUflag;//车载机状态 0x02 需要刷司机卡 
extern unsigned char gucSoudnorm;

extern unsigned short gErrorFlag;//错误标识,高字节表示错误编号，低字节表示错次数据 详见"*错误编码标识定义"
//extern jsb_rec jsb_recbuff;
void g24ComParaClr(void)
{
	memset(g24ComRBuf, 0, 10);
	g24ComRindex = 0;
}

void G24init(void)
{
	
	g24GProFlag = 0;//参数
	
	g24PurError = 10;// 开始不能访问手机读头。
	gG24Ver = 0;
	gG24ReaderErr = 0;
	memset(gG24VerBuf, '0', sizeof(gG24VerBuf));
	gG24VerBuf[20] = 0;
}

unsigned char g24ComSend(unsigned char *sndDat, unsigned char len)
{
	unsigned int dly = 0;
	
	unsigned char crc;
	unsigned char sendbuf[256];
	unsigned short index, i;

	index = 0;
	sendbuf[index++] = 0x02;//STX
	sendbuf[index++] = len/256;
	sendbuf[index++] = len%256;
	memcpy(sendbuf+index, sndDat, len);
	index += len;
	
	crc = 0;
	for(i=0; i<len; i++){
		crc ^= sndDat[i];
	}
	sendbuf[index++] = crc;
	sendbuf[index++] = 0x03;

	g24ComFlag = 0xaa;//在接收g24读头
	g24ComParaClr();
// #ifdef _debug_
// 	debugstring("g24ComSend dat:");
// 	debugdata(sendbuf, index, 1);
// #endif
	com_snd(G24Com, index, sendbuf);

	while(1){
		clr_wdt();
		if(g24ComFlag == 0x55)
			return g24ComRindex;
		dly++;
		if(dly > 900000){ //900000
// 			sendbuf[0] = 0x77;
// 			sendbuf[1] = g24ComFlag;
// 			com_snd(COM1, 2, sendbuf);
			return 0;
		}
	}
//	return 1;
}

unsigned char g24ComSendDat(unsigned char *sndDat, unsigned char len)
{
	unsigned int dly = 0;
	
	unsigned char crc;
	unsigned char sendbuf[256];
	unsigned short index, i;
	
	index = 0;
	sendbuf[index++] = 0x02;//STX
	sendbuf[index++] = (len+2)/256;
	sendbuf[index++] = (len+2)%256;
	sendbuf[index++] = 0xA2;
	sendbuf[index++] = 0x33;
	memcpy(sendbuf+index, sndDat, len);
	index += len;
	
	crc = 0xA2;
	crc ^= 0x33;
	for(i=0; i<len; i++){
		crc ^= sndDat[i];
	}
	sendbuf[index++] = crc;
	sendbuf[index++] = 0x03;
	
	g24ComFlag = 0xaa;//在接收g24读头
	g24ComParaClr();
#ifdef _debug_24G
	debugstring("g24Com33Send dat:");
	debugdata(sendbuf, index, 1);
#endif
	com_snd(G24Com, index, sendbuf);
	
	while(1){
		clr_wdt();
		if(g24ComFlag == 0x55)
			return g24ComRindex;
		dly++;
		if(dly > 900000){
			// 			sendbuf[0] = 0x77;
			// 			sendbuf[1] = g24ComFlag;
			// 			com_snd(COM1, 2, sendbuf);
			return 0;
		}
	}
	//	return 1;
}
unsigned char g24ComSend_only(unsigned char *sndDat, unsigned char len)
{
//	unsigned int dly = 0;
	
	unsigned char crc;
	unsigned char sendbuf[512];
	unsigned short index, i;
	
	index = 0;
	sendbuf[index++] = 0x02;//STX
	sendbuf[index++] = len/256;
	sendbuf[index++] = len%256;
	memcpy(sendbuf+index, sndDat, len);
	index += len;
	
	crc = 0;
	for(i=0; i<len; i++){
		crc ^= sndDat[i];
	}
	sendbuf[index++] = crc;
	sendbuf[index++] = 0x03;
	
	g24ComFlag = 0xaa;//在接收g24读头
	g24ComParaClr();
	com_snd(G24Com, index, sendbuf);
	return 1;
}


unsigned char Rf24GPassthrough(unsigned char *revDat, unsigned char rlen, unsigned char *sndat, unsigned char slen)
{
	unsigned char ret;

	ret = g24ComSendDat(sndat, slen);
#ifdef _debug_24G
	debugstring("G24 Rf24GPassthrough ACK:");
	debugdata(g24ComRBuf, g24ComRindex, 1);
#endif
	if(ret == 0)//返回超时,错误
		return 0;
	if((g24ComRBuf[3]!=0) || (g24ComRBuf[4] !=0))
		return 0;
	memcpy(revDat, g24ComRBuf+4, ret-4);
	ret -= 6;
	return ret;
}
//查看读头版本信息
void check24GVer(void)
{
	unsigned char ret;
	unsigned char sendbuf[101];
	//早期读头 02 00 27 00 00 0500000000000000 0000000000000000 14 4E4D4652532D563531304D483336585335303435 15 03 
	//1.0新读头02 00 27 00 00 0500000000000000 0000000000000000 14 4E4D4652532D5635313048483336425335313030 0A 03 
	//BS5100 XS5045
	//	g24PurError = 10;// 开始不能访问手机读头。
	memcpy(sendbuf, "\xA1\x11", 2);
	ret = g24ComSend(sendbuf, 2);
	gG24Ver = 0xA4;
 #ifdef _debug_
// 	debugstring("check24GVer:");
 //	debugdata((unsigned char*)&gG24Ver, 1, 1);	
 #endif
	if(ret != 0){
#ifdef _debug_
		debugstring("24G Ver:");
		//		debugdata((unsigned char*)&g24ComRindex, 4, 1);
		debugdata(g24ComRBuf, g24ComRindex, 1);
#endif
		if(g24ComRindex>23)
		{
			ret = g24ComRBuf[21];
			if(ret > 20)
				ret = 20;
			memset(gG24VerBuf,0,21);
			memcpy(gG24VerBuf, g24ComRBuf+22, ret);
			
			memset(sendbuf, 0, 100);
			memcpy(sendbuf, g24ComRBuf+23, g24ComRindex-23);
			if(strstr((char*)sendbuf, "BS5100")!=0){
				gG24Ver = 0xA2;
			}
			else if(strstr((char*)sendbuf, "XS5045")!=0){
				gG24Ver = 0xA1;
			}
			else{
				gG24Ver = 0xA3;
			}
		}
		
#ifdef _debug_
		debugstring("头:");
		debugdata((unsigned char*)&gG24Ver, 1, 1);	
#endif
	}
}

void RestCPU24Card(void)
{
	//	unsigned char ret;
	unsigned char sendbuf[20];
	
	g24PurError = 10;// 开始不能访问手机读头。//
	memcpy(sendbuf, "\xA1\x12", 2);
	//	ret = 
	g24ComSend(sendbuf, 2);
	gG24Ver = 0;
}
 
void Check24Card(void)
{
	unsigned short sst=0;
	unsigned int dly=0;
	unsigned char ret;
	unsigned char sendbuf[20];

	if(g24GProFlag == 1){//让上一个命令完成
		while(1){
			if(g24ComFlag == 0x55)
				break;
			dly++;
			if(dly > 900000){
				break;
			}
		}
	}

	for(dly=0;dly<2;dly++){
		g24GProFlag = 0;
		memcpy(sendbuf, "\xE0\x09", 2);
		ret = g24ComSend(sendbuf, 2);
		if(ret != 0){
#ifdef _debug_
			debugstring("checkReader:");
			debugdata(g24ComRBuf, g24ComRindex, 1);
#endif
			memcpy((unsigned char*)&sst, g24ComRBuf+3, 2);
			if((sst == 0x09a0)||(sst == 0x55e0)||(sst == 0x05e0)){//读卡器不正常，命令重启模块,延时
				RestCPU24Card();
			}
		}
	}
}
//读24G读卡器状态
void G24ReadStyle(void)
{
	unsigned short sst=0;
	unsigned int times=0;
	unsigned char ret;
	unsigned char sendbuf[40];
	
	cls();
	if(gchn_eng == 'E'){
//		display(0,0,"check 2.4GReader",DIS_CENTER|DIS_CONVERT);
		display(6,0,"exit        test",0);
	}
	else{
//		display(0,0,"测试读头状态",DIS_CENTER|DIS_CONVERT);
		display(6,0,"退出        测试",0);
	}
	memset(sendbuf, 0, 20);
	BCD2Ascii((unsigned char*)&gG24Ver, sendbuf, 1);
	miniDispstr(1, 19, (char*)sendbuf, 0);
	miniDispstr(0, 1, (char*)gG24VerBuf, 0);

	restore_flag = 3;
	while(1){
		memcpy(sendbuf, "\xE0\x09", 2);
		ret = g24ComSend(sendbuf, 2);
		if(ret != 0){
			beep(1, 100, 100);
			if(gchn_eng == 'E'){
				sprintf((char*)sendbuf, "style:%02X%02X\x00", g24ComRBuf[3], g24ComRBuf[4]);
			}
			else
				sprintf((char*)sendbuf, "状态:%02X%02X\x00", g24ComRBuf[3], g24ComRBuf[4]);
			display(2,0,(char*)sendbuf, DIS_CENTER);
			times++;
			memcpy((unsigned char*)&sst, g24ComRBuf+3, 2);
			switch (sst)
			{
			case 0:
				if(gchn_eng == 'E'){
					display(4,0,"  success  ", DIS_CENTER);
				}
				else
					display(4,0,"  读卡正常  ", DIS_CENTER);
				break;
			case 0x09a0:
				if(gchn_eng == 'E'){
					display(4,0,"selfchekc fail", DIS_CENTER);
				}
				else
					display(4,0,"上电自检失败", DIS_CENTER);
				break;
			case 0x0aa0:
				if(gchn_eng == 'E'){
					display(4,0,"waite card", DIS_CENTER);
				}
				else
					display(4,0,"等待卡片靠近", DIS_CENTER);
				break;
			case 0x55e0:
				if(gchn_eng == 'E'){
					display(4,0,"environment unconventionality", DIS_CENTER);
				}
				else
					display(4,0,"  环境异常  ", DIS_CENTER);
				break;
			case 0xaae0:
				if(gchn_eng == 'E'){
					display(4,0,"reader locked", DIS_CENTER);
				}
				else
					display(4,0," 读卡器死锁 ", DIS_CENTER);
				break;
			case 0x05e0:
				if(gchn_eng == 'E'){
					display(4,0,"no activation", DIS_CENTER);
				}
				else
					display(4,0,"读卡器未激活", DIS_CENTER);
				break;
			}
			sprintf((char*)sendbuf, "%u", times);
			display(6,0,(char*)sendbuf, DIS_CENTER);
		}
		while(1){
			if(profile(0) !=ST_ERROR)
				return;
			ret = getkey(0);
			if(ret == ESC)
				return;
			if(ret == KEY_ENT)
				break;
			delayxms(50);
		}		
	}
//	return;
}

//对24G卡进行循卡
unsigned char G24Profile(void)
{
	unsigned char buffer[100];
//	unsigned int relen;
	unsigned short ss=0;
	
	if(g24GProFlag == 0){
		memcpy(buffer, "\xA2\x31\x01\xf4", 4);//询卡
		g24ComSend_only(buffer, 4);
		g24GProFlag = 1;
		return 0;
	}
	else{
		if(g24ComFlag == 0x55){
			g24GProFlag = 0;
 #ifdef _debug_66
  			debugstring("询卡返回:");
  			debugdata(g24ComRBuf, g24ComRindex,1);
 #endif
			memcpy((unsigned char*)&ss, g24ComRBuf+3, 2);//返回的状态码
			if(ss == 0x0000){//成功
				gG24ReaderErr = 0;
				gErrorFlag &= (~ERROR_24READER);
				memset(gG24CardNo, 0, 10);
#ifdef _debug_
				debugstring("cardNOOO:");
				debugdata(g24ComRBuf, g24ComRindex,1);
#endif
// 				if(buffer[5] <= 8)
// 					memcpy(gG24CardNo, g24ComRBuf+6, buffer[5]);
// 				else
					memcpy(gG24CardNo, g24ComRBuf+6, 8);
				return 1;
			}
			else if (ss == 0x09a0)//自检失败，重启模块
			{
				gG24ReaderErr++;
				if(gG24ReaderErr >= 50){
					gErrorFlag |= ERROR_24READER;
					RestCPU24Card();
					gG24ReaderErr = 0;
				}
			}
		}
		else{
			g24GProFlag++;
			if(g24GProFlag > 200){
//				miniDispstr(3,0,"P out!",0);
				gErrorFlag |= ERROR_24READER;
				g24GProFlag = 0;
			}
		}
	}
	return 0;
}


unsigned char CPU24GPsamInitialize(void)
{
	unsigned char ii=0;
	//	unsigned char pos=0;
	unsigned char buffer[256];

	gPsamErrorFlag |= 2;
	
	memset(G24PsamNo, 0, 6);
#ifdef _debug_
	debugstring("CPU24gPSAM init\r\n");
#endif
	ii = SamColdReset(PSAM_YD, 9600, SAM_VOL3, buffer);
	
	if(ii == 0){
		return ST_ERROR;
	}
	
#ifdef _debug_
	debugstring("CPU24gPSAM art:");
	debugdata(buffer, ii, 1);
#endif

	delayxms(5);
	if(Read_Binary(PSAM_YD, 0x16, 0, 6, buffer)==0) 
	{
#ifdef _debug_
		debugstring("CPU24gRead_Binary error\r\n");
		debugdata(buffer, 8, 1);
#endif
		return ST_ERROR;
	}
#ifdef _debug_
	debugstring("CPU24gread 16 Binary:");
	debugdata(buffer,6,1);
#endif
	memcpy(G24PsamNo,buffer,6);
	delayxms(100);
	
	
#ifdef _debug_
	debugstring("CPU24gSelect File \r\n");
#endif
	if(Select_File(PSAM_YD, 0x1001)==0) //0x3f01
	{
		debugstring("G24Select_File error\r\n");
		return ST_ERROR;
	}
	gPsamErrorFlag &= 0xFD;
	
#ifdef _debug_
	debugstring("CPU24gSelect OK\r\n");
#endif
	return ST_OK;
}

extern unsigned char CPUMAC2(unsigned char PsamID, unsigned char *pMac, unsigned char *pData, unsigned char len);
extern unsigned char CpuCardSetMac(unsigned char PsamID, unsigned char *pMac, unsigned char *pData, unsigned char len);

unsigned char CPU_24GRead_Money(unsigned char *remb, unsigned char mode)
{
	unsigned char sndbuf[64];
	unsigned char revbuf[64];
	unsigned char ret;
#ifdef _debug_
	unsigned int temp;
#endif

	memcpy(sndbuf,"\x80\x5c\x00\x02\x04",5);
	
	ret = Rf24GPassthrough(revbuf, 2, sndbuf, 5);
#ifdef _debug_
	debugstring("G24 read money ACK:");
	debugdata(g24ComRBuf, g24ComRindex, 1);
#endif
	if(ret == 0)//返回超时,错误
		return 1;
#ifdef _debug_
	debugstring("---remb:====");
	debugdata(revbuf,ret, 1);
	memcpy((unsigned char*)&temp, revbuf+1, 4);
	over_turn(4, (unsigned char*)&temp);
	sprintf((char *)sndbuf, "余额:%d.%02d", temp/100, temp%100);
	debugstring((char*)sndbuf);
#endif
	memcpy(remb, revbuf+1, 4);
	
	return 0;
}

unsigned char CPU_24Gmonth_deal(void)
{
	unsigned char errorf=0,i=0;				  
	unsigned char sndbuf[100];
	unsigned char revbuf[100];
	unsigned char TACC[8];
	unsigned char psRandom[10];
//	unsigned char PSAMDsno[4];
	unsigned int revLen,ret;
//	unsigned int value;
//	unsigned int pmoney;
//	unsigned int itemp;
	unsigned short ii;
	unsigned char month[2][64];
	unsigned char mothFlag;
	unsigned char monthstyle;
	
#ifdef _debug_
	debugstring("CPU _MONTH _DEAL::\r\n");
#endif
	if(cardSound == 0xaa){//钱包需要重刷，去钱包扣钱
		if(card_ser!=c_serial){
			card_ser = 0;
			cardSound = 0;
			g24PurError = 0;
			//return ST_ERROR;
		}
		else
			return 3;
	}
	
	if(cardSound == 0xbb){//月票需要重刷
		if(card_ser!=c_serial){
			card_ser = 0;
			cardSound = 0;
			g24PurError = 0;
			//return ST_ERROR;
		}
	}

	if(cardMessage.card_catalog >= 0x40)
		mothFlag = cardMessage.card_catalog - 0x40;
	else
		mothFlag = cardMessage.card_catalog;
	if(month_decide()) // 第二次去扣钱包(次数不能连刷)
	{
		disp_no_swipe();
		return ST_OK;
	}

	memcpy(sndbuf,"\x00\xB0\x99\x00\x20",5);//读月票正本
	ret = Rf24GPassthrough(revbuf, 2, sndbuf, 5);
	if(ret == 0)
		return ST_ERROR;

	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return ST_ERROR;
	memcpy(month[0], revbuf+1, 32);
	
	memcpy(sndbuf,"\x00\xB0\x99\x20\x20",5);//读应用序列号作为分散因子
	ret = Rf24GPassthrough(revbuf, 2, sndbuf, 5);
	if(ret == 0)
		return ST_ERROR;

	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return ST_ERROR;
	memcpy(month[0]+0x20, revbuf+1, 32);
#ifdef _debug_
	debugstring("month[0]:");
	debugdata(month[0], 64, 1);
#endif
	
	memcpy(sndbuf,"\x00\xB0\x9A\x00\x20",5);//读应用序列号作为分散因子
	ret = Rf24GPassthrough(revbuf ,2, sndbuf, 5);
	if(ret == 0)
		return ST_ERROR;
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return ST_ERROR;
	memcpy(month[1], revbuf+1, 32);
	
	memcpy(sndbuf,"\x00\xB0\x9A\x20\x20",5);//读应用序列号作为分散因子

	ret = Rf24GPassthrough(revbuf, 2, sndbuf, 5);
	if(ret < 2)
		return ST_ERROR;
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return ST_ERROR;
	memcpy(month[1]+0x20, revbuf+1, 32);
	
#ifdef _debug_
	debugstring("month[1]:");
	debugdata(month[1], 64, 1);
#endif
	if(month[0][0] == SysTime.year_l)
		mothFlag = 0;  //当前年在0块
	else if(month[0][32] == SysTime.year_l)
		mothFlag = 32;	//当前年在1块
	else
		return 3;//月票不正确，去扣钱,没有当前年份的。

	monthstyle = month[0][mothFlag+1];
	if(monthstyle >= 3){//0 是月票  1 是年票  2 是季票
		return 3;//月票不正确，去扣钱，不知道是什么票
	}
	
	memset(TACC, 0, 4);
	for(i=0;i<31;i++){
		TACC[0] ^= month[0][i];
		TACC[1] ^= month[0][32+i];
		TACC[2] ^= month[1][i];
		TACC[3] ^= month[1][32+i];
	}
	if((TACC[0] != month[0][31])||(TACC[0] != month[0][63])){
		errorf |= 8;
	}
	if((TACC[2] != month[1][31])||(TACC[3] != month[1][63])){
		errorf |= 4;
	}	

	ii = ((SysTime.month>>4)&0x0f)*10 + (SysTime.month&0x0f);
	if(monthstyle == 0){//0 是月票  
		a_sum1 = 0;
		memcpy((unsigned char *)&a_sum1, month[0]+(mothFlag+ii*2), 2);//正本

		revLen = 0;
		memcpy((unsigned char *)&revLen, month[1]+(mothFlag+ii*2), 2);//副本
	}
	else if(monthstyle == 1){//1 是年票
		a_sum1 = 0;
		memcpy((unsigned char *)&a_sum1, month[0]+(mothFlag+26), 4);//正本
		
		revLen = 0;
		memcpy((unsigned char *)&revLen, month[1]+(mothFlag+26), 4);//副本
	}
	else if(monthstyle == 2){//1 是季票
		a_sum1 = 0;
		memcpy((unsigned char *)&a_sum1, month[0]+(mothFlag+(ii-1)*6+2), 4);//正本
		
		revLen = 0;
		memcpy((unsigned char *)&revLen, month[1]+(mothFlag+(ii-1)*6+2), 4);//副本
	}
#ifdef _debug_
	debugstring("钱包\r\n");
	debugdata((unsigned char*)&a_sum1, 4, 1);
	debugdata((unsigned char*)&revLen, 4, 1);
#endif
	if((errorf&8)&&(errorf&4)){
		debugstring("月票正副本校验错\r\n");
		return 3;
	}

	if(errorf&8)
		a_sum1 = revLen;
	else if((errorf&4) == 0){
		if(a_sum1 > revLen)
			a_sum1 = revLen;
	}
	if(a_sum1 == 0)
		return 3;

	s_sum1 = 1;
	a_sum1 -= 1;
	if(cardSound == 0xbb){//月票需要重刷
		if(a_sum1 <= dis_sum2)
			goto cpuMonthEnd;//月票扣款成功
	}
	dis_sum2 = a_sum1;

#ifdef _debug_
	debugstring("钱包2\r\n");
	debugdata((unsigned char*)&a_sum1, 4, 1);
	debugdata((unsigned char*)&revLen, 4, 1);
#endif
	ii = ((SysTime.month>>4)&0x0f)*10 + (SysTime.month&0x0f);
	if(monthstyle == 0){//0 是月票
		memcpy(month[0]+(mothFlag+ii*2), (unsigned char *)&a_sum1, 2);//正本
	}
	else if(monthstyle == 1){//1 是年票
		memcpy(month[0]+(mothFlag+26), (unsigned char *)&a_sum1, 4);//正本
	}
	else if(monthstyle == 2){//1 是季票
		memcpy(month[0]+(mothFlag+(ii-1)*6+2), (unsigned char *)&a_sum1, 4);//正本
	}
	memset(TACC, 0, 4);
	for(i=0;i<31;i++){
		TACC[0] ^= month[0][i];
		TACC[1] ^= month[0][32+i];
	}
	month[0][31] = TACC[0];
	month[0][63] = TACC[1];
	memcpy(month[1], month[0], 64);

#ifdef _debug_
	debugstring("month1:");
	debugdata(month[0], 64, 1);
	debugdata(month[1], 64, 1);
#endif
	
	memcpy(sndbuf,"\x00\x84\x00\x00\x04",5);////取随机数，算MAC
	ret = Rf24GPassthrough(revbuf ,2, sndbuf, 5);
	if(ret == 0)
		return ST_ERROR;
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return ST_ERROR;
	memset(psRandom, 0, 8);
	memcpy(psRandom, revbuf+1, 4);
	
	if(mothFlag == 32)
		memcpy(sndbuf, "\x04\xD6\x99\x20\x24", 56);
	else
		memcpy(sndbuf, "\x04\xD6\x99\x00\x24", 5);
	memcpy(sndbuf+5, month[0]+mothFlag, 32);
	CountMac("\x36\xDF\xAF\xC7\x9B\x1F\xB5\x5A\xD0\x83\xFC\x78\xE1\x58\x47\x73", psRandom, sndbuf, 37, TACC);
#ifdef _debug_
	debugstring("MAC::");
	debugdata(psRandom, 8, 1);
	debugdata(sndbuf, 37, 1);
	debugdata(TACC, 8, 1);
#endif
	
	memcpy(sndbuf+37, TACC, 4);
#ifdef _debug_
	debugstring("write CMD::");
	debugdata(sndbuf, 41, 1);
#endif

	ret = Rf24GPassthrough(revbuf ,2, sndbuf, 41);
	if(ret == 0){
			cardSound = 0xbb;//出错，需要重刷卡
			card_ser = c_serial;
			g24PurError = 0xaa;

			audio(Audio_TRY_AGAIN);		//请重刷

			return 7;
	}
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return 6;
#ifdef _debug_
	debugstring("month write 1:");
	debugdata(revbuf, ret, 1);
#endif
	
	memcpy(sndbuf,"\x00\x84\x00\x00\x04",5);//取随机数，算MAC
	ret = Rf24GPassthrough(revbuf ,2, sndbuf, 5);
	if(ret == 0)
		return ST_ERROR;
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return ST_ERROR;
	memset(psRandom, 0, 8);
	memcpy(psRandom, revbuf+1, 4);

	if(mothFlag == 32)
		memcpy(sndbuf, "\x04\xD6\x9A\x20\x24", 5);
	else
		memcpy(sndbuf, "\x04\xD6\x9A\x00\x24", 5);
	memcpy(sndbuf+5, month[0]+mothFlag, 32);
	CountMac("\x36\xDF\xAF\xC7\x9B\x1F\xB5\x5A\xD0\x83\xFC\x78\xE1\x58\x47\x73", psRandom, sndbuf, 36, TACC);
#ifdef _debug_
	debugstring("MAC 2::");
	debugdata(psRandom, 8, 1);
	debugdata(sndbuf, 36, 1);
	debugdata(TACC, 8, 1);
#endif
	
	memcpy(sndbuf+37, TACC, 4);
#ifdef _debug_
	debugstring("write CMD 2::");
	debugdata(sndbuf, 41, 1);
#endif
	
	ret = Rf24GPassthrough(revbuf ,2, sndbuf, 41);
// 	if(ret == 0)
// 		return ST_ERROR;
// 	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
// 		return 6;
#ifdef _debug_
	debugstring("month write 2:");
	debugdata(revbuf, ret, 1);
#endif

cpuMonthEnd:
	card_ser = 0;
	cardSound = 0;
	g24PurError = 0;
	money_msg(ID_REC_MON, a_sum1, s_sum1, 2);
	SoundMessage(SOUND_DEAL);

#ifdef _debug_
	debugstring("-------\r\n");
	debugstring("消费成功:\r\n");
	sprintf((char*)revbuf, "余额:%d次\r\n", a_sum1);
	debugstring((char*)revbuf);
	sprintf((char*)revbuf, "扣款:%d次\r\n", s_sum1);
	debugstring((char*)revbuf);
	debugstring("-------\r\n");
#endif
	
	save_h_month();
	memset(revbuf, 0, 128);

	BuildRecorde(ID_REC_MON, revbuf);
	WriteRecord(revbuf, 0);

	return ST_OK;//消费成功
}


void G24End_Card(void)
{
	unsigned char ret;
	//	unsigned int revl;
	unsigned int dly=0;
	unsigned char sendbuf[20];
	
	while(1){
		memcpy(sendbuf, "\xE0\x02", 2);
		ret = g24ComSend(sendbuf, 2);
		if(ret != 0){
			//			debugdata(g24ComRBuf, g24ComRindex, 1);
			if(g24ComRBuf[5] != 1)
				return;
			delayxms(50);
			dly = 0;
		}
		dly++;
		if(dly > 10)
			break;
	}
}

void g24save_normal(void)
{
	memcpy(g24PCard_delay.CardSno,gG24CardNo,8);
	g24PCard_delay.Sec=SysTime.seconds;
}

unsigned char g24Delay_Normal(void)
{
	int i;
	unsigned char c,c1;
	
	if(memcmp(g24PCard_delay.CardSno,gG24CardNo,4))
	{
		memset(g24PCard_delay.CardSno,0,4);
		return 0;
	}
	
	c = (SysTime.seconds>>4)*10+(SysTime.seconds&0x0f);
	c1 = (g24PCard_delay.Sec>>4)*10+(g24PCard_delay.Sec&0x0f);
	
	i=((c+60)-c1)%60;
	if(i>1)
	{
		memset(g24PCard_delay.CardSno,0,4);
		return 0;
	}
	else 
		return 1;
}
extern unsigned char dis_overdate;	//提示过期日期 

extern unsigned char check_time(void);
extern unsigned char StufferNO[4];
extern unsigned int startTime,endTime,nowTime,endTimeEn;

extern unsigned char CARD_TIME(unsigned char *YouHui_Date);
	
unsigned char CPU24GcardType(void)
{
	//	unsigned char PSAMDsno[4];
	unsigned int itemp;//,value;
	unsigned int ret=0;
	unsigned int start_time,end_time;

	unsigned char sndbuf[100];
	unsigned char revbuf[100];
	unsigned char sw[2];
	MF_PublicBacinformation1 *Root_File15;
	stJSBF15 *JSBFile15;
	SaleInformation1 *file14;

	unsigned char CITYNO[2];
	Card_Area = 0;
	YouHui_Area_other=0;

	if(check_time())
		return ST_ERROR;


	if(F_FLAG_VARY.stop_flag)
		return ST_ERROR;

	memcpy(sndbuf, "\xA2\x33\x00\xA4\x04\x00\x09\xA0\x00\x00\x00\x03\x86\x98\x07\x01", 16);
#ifdef _debug_
	debugstring("G24 Select:");
	debugdata(sndbuf, 16, 1);
#endif
	ret = Rf24GPassthrough(revbuf, 2, sndbuf, 16);
#ifdef _debug_
	debugstring("G24 Select ACK:");
	debugdata(revbuf, ret, 1);
#endif
	if(ret == 0)//返回超时,错误
		return ST_ERROR;	
	if(memcmp(revbuf+ret-2, "\x6A\x81", 2) == 0)
	{
		//	if(gMCardCand==CARDSTYLE_NORM1)
		//	return ST_ERROR;
		return APP_LOCK;
	}	

#ifdef _debug_
	debugstring("选择应用 3F01:开始\r\n");
#endif
	memcpy(sndbuf, "\x00\xA4\x00\x00\x02\x3F\x01",7);
	ret = Rf24GPassthrough(revbuf, 2, sndbuf, 7);
	if(ret == 0)//返回超时,错误
		return ST_ERROR;
	
#ifdef _debug_
	debugstring("选择应用 :");
	debugdata(revbuf, ret, 1);
#endif


	//2、读根目录15file
/*

	memcpy(sndbuf,"\x00\xB0\x95\x00\x33",5);//  里面有卡类
	ret = Rf24GPassthrough(revbuf, 2, sndbuf, 5);
	if(ret == 0)
		return ST_ERROR;

	memcpy(sw,revbuf+ret-2,2);
	MSG_LOG("sw:");
	BCD_LOG(sw,2,1);
	MSG_LOG("revbuf::");
	BCD_LOG(revbuf,ret,1);
	if((memcmp(sw,"\x62\x83",2)==0)||(memcmp(sw,"\x6A\x81",2)==0)||(memcmp(sw,"\x6A\x86",2)==0)||(memcmp(sw,"\x6A\x82",2)==0)||(memcmp(sw,"\x6D\x00",2)==0))
	{
		//没有选择到绵州通钱包，走交通部钱包
		printf("select MZT wrong!\n");
		return NO_CITY_UNION;
	}
	else if(memcmp(sw,"\x90\x00",2)!=0)
        return ST_ERROR;

	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return ST_ERROR;
	
	
#ifdef _debug_CPU_
	debugstring("sndbuf:");
	debugdata(sndbuf,5,1);
	debugdata((unsigned char*)&ret, 4,1);
	debugstring("Read 05:");
	debugdata(revbuf, ret, 1);
#endif

	Root_File15 = (MF_PublicBacinformation1 *)(revbuf+1);

	memcpy(CITYNO,Root_File15->cityCode,2);                 //6210
	cardMessage.card_catalog = Root_File15->cardType[1];	//	81
	cardMessage.Zicard_catalog = Root_File15->cardType[0];  //  89
	if ((cardMessage.card_catalog <0x81) || (cardMessage.card_catalog >0xA0))		//32 种折扣，普通M1 0x41 开始，CPU卡 0x81 开始
	{
		MSG_LOG("卡类超范围:%02X\r\n",cardMessage.card_catalog);
		cardMessage.card_catalog = CARD_NORMAL_BUS;
		cardMessage.Zicard_catalog = 0;
	}


*/


		//4、选电子钱包应用
	bzero(sndbuf, sizeof(sndbuf));
	bzero(revbuf, sizeof(revbuf));
	memcpy(sndbuf,SELECT_APPY_MF, 19);//为了区分成都卡，因为成都的下面1001 和绵阳一样的

	MSG_LOG("SecAPP_MF:\r\n");
	BCD_LOG(sndbuf,19,1);
	ret = Rf24GPassthrough(revbuf, 2, sndbuf, 19);
	if(ret == 0)
        return ST_ERROR;

	

	MSG_LOG("REC:\r\n");
	BCD_LOG(revbuf,ret,1);
	

	memcpy(sw,revbuf+ret-2,2);
	if((memcmp(sw,"\x62\x83",2)==0)||
		(memcmp(sw,"\x6A\x81",2)==0)||
		(memcmp(sw,"\x6A\x82",2)==0)
		
				)
	{
// 		beep_err(BEEP_DELAY_ERR);
// 		Audio(SOUND_INVALID);
		return NO_CITY_UNION;
	}
	else if(memcmp(sw,"\x90\x00",2)!=0)
        return ST_ERROR;

	//4、选电子钱包应用
	bzero(sndbuf, sizeof(sndbuf));
	bzero(revbuf, sizeof(revbuf));
	memcpy(sndbuf,SELECT_APPY_NEW, 21);		//绵阳新
// 	sendlen = 21;
	
	MSG_LOG("先选新的 \"MYMZT.ADF01 \":\r\n");
	BCD_LOG(sndbuf,21,1);
	ret = Rf24GPassthrough(revbuf, 2, sndbuf, 21);
	if(ret == 0)
        return ST_ERROR;

	MSG_LOG("REC:\r\n");
	BCD_LOG(revbuf,ret,1);

	memcpy(sw,revbuf+ret-2,2);
	if(memcmp(sw,"\x90\x00",2)!=0)
	{
		MSG_LOG("选不到选旧的--\r\n");
			////////////////////////////////
			//4、选电子钱包应用
			bzero(sndbuf, sizeof(sndbuf));
			bzero(revbuf, sizeof(revbuf));
			memcpy(sndbuf,SELECT_APPY_1001, 18);		//绵阳旧的
			ret = Rf24GPassthrough(revbuf, 2, sndbuf, 18);
			if(ret == 0)
				return ST_ERROR;

#ifdef _debug_CPU_
			printf("SELECT_APPY ret=%d\n", ret);
			debugdata(revbuf,ret,1);
			printf("\n");
#endif
			memcpy(sw,revbuf+ret-2,2);
			if((memcmp(sw,"\x62\x83",2)==0)||
			   (memcmp(sw,"\x6A\x81",2)==0)||
			   (memcmp(sw,"\x6A\x82",2)==0)

				)
			{
				// 		beep_err(BEEP_DELAY_ERR);
				// 		Audio(SOUND_INVALID);
				return NO_CITY_UNION;
			}
			else if(memcmp(sw,"\x90\x00",2)!=0)
				return ST_ERROR;
	}
	

	
	//5、读卡状态信息文件0x15

	MSG_LOG("读15文件\r\n");
	BCD_LOG(sndbuf,5,1);


	memcpy(sndbuf,"\x00\xb0\x95\x00\x1f",5);
	ret = Rf24GPassthrough(revbuf, 2, sndbuf, 5);
	if(ret == 0)
		return ST_ERROR;
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return ST_ERROR;

	MSG_LOG("REC:\r\n");
	BCD_LOG(revbuf,ret,1);


	

	JSBFile15 = (stJSBF15 *)(revbuf+1);
	
	memcpy(cardMessage.TradeCode, JSBFile15->FCcode, 2);//发卡方标识
	memcpy(cardMessage.CityCode, JSBFile15->citycode, 2);//城市代码
	// 	memcpy(cardMessage.CardCheckCode,JSBFile15.MAC,CARD_CHECH_CODE_LEN);
	memcpy(cardMessage.PublishBicker, JSBFile15->FYFlag,10);//发卡流水号
	memcpy((unsigned char*)&start_time, JSBFile15->StartDate, 4);//启用日期
//	over_turn(4, (unsigned char*)&start_time);
	memcpy((unsigned char*)&end_time, JSBFile15->EndDate, 4);//有效日期
//	over_turn(4, (unsigned char*)&end_time);
	memcpy(CITYNO,JSBFile15->citycode,2);
	cardMessage.card_catalog = JSBFile15->card_catalog;
	cardMessage.Zicard_catalog = JSBFile15->Zicard_catalog;
	if ((cardMessage.card_catalog <0x81) || (cardMessage.card_catalog >0xA0))		//32 种折扣，普通M1 0x41 开始，CPU卡 0x81 开始
	{
		MSG_LOG("卡类超范围:%02X\r\n",cardMessage.card_catalog);
		cardMessage.card_catalog = CARD_NORMAL_BUS;
		cardMessage.Zicard_catalog = 0;
	}

	cardMessage.card_catalog_bak=cardMessage.card_catalog; //备份

// 	memcpy((unsigned char*)&stuPubInfZaoZuang.cPublisherSign, JSBFile15->YYNO, 8);//卡序列号  卡应用序列号，完整的，备用
// 	memcpy(stuPubInfZaoZuang.cApp_sn, JSBFile15->FYFlag, 10);	//卡应用序列号，完整的，备用
// 	memcpy(stuPubInfZaoZuang.cCitycode,JSBFile15->citycode, 2);	//
	
	
	
#ifdef _debug_CPU_
	debugstring("cardMessage.card_catalog:0x");
	debugdata((unsigned char*)&cardMessage.card_catalog,1, 1);
	debugstring("zcard-talog:0x");
	debugdata((unsigned char*)&cardMessage.Zicard_catalog,1, 1);
	debugstring(".PublishBicker:::");
	debugdata(cardMessage.PublishBicker, 10, 1);
	debugstring("启用日期::");
	debugdata((unsigned char*)&start_time, 4, 1);
	debugstring("有效期::");
	debugdata((unsigned char*)&end_time, 4, 1);
#endif
	
	
// 	if((itemp < start_time)||(itemp > end_time)){
// 		return CARD_NO_TIME;//过期或末到期
// 	}


	dis_overdate = 0xff;

	if(memcmp(G24PsamNo,CITYNO,2)==0)    //绵阳本地卡,
	{

		keyIndex = 0x11;
		//6、读公共应用基本信息文件0x15
		bzero(sndbuf, sizeof(sndbuf));
		bzero(revbuf, sizeof(revbuf));
		memcpy(sndbuf,"\x00\xb0\x94\x00\x20",5);
		ret = Rf24GPassthrough(revbuf, 2, sndbuf, 5);
		if(ret == 0)
			return ST_ERROR;
		if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
			return ST_ERROR;

#ifdef _debug_CPU_
			printf("read 94 ret=%d\n", ret);
			debugdata(revbuf,ret,1);
			printf("\n");
#endif
		

			
		file14 = (SaleInformation1 *)(revbuf+1);


		switch(file14->enable)
		{
		case NOTENABLED:
			return CARD_NO_USE;
		case DISABLED:
			return CARD_NO_TIME;	//过期卡		
		case ENABLED:
			break;
		default:
			return CARD_NO_SYSTEM;
		}

/*
		ret = cardMessage.card_catalog -0x40;		//CPU 卡81 开始的 M1 卡41开始


		//老人卡，残疾卡，子女卡需要进行年审判断20150514
		if ((ret==CARD_OLDMAN_BUS)||(ret==CARD_Love_BUS)||(ret==CARD_ZILU_BUS))
		{


			memcpy((unsigned char*)&end_time, file14->cardExpirationDate, 4);//有效日期
			over_turn(4, (unsigned char*)&end_time);
			
			
			
			memcpy((unsigned char*)&itemp, (unsigned char *)&SysTime, 4);//当前日期
			over_turn(4, (unsigned char*)&itemp);

			if(end_time<itemp)
				return CARD_NO_TIME;	//过期卡
		}*/

		//(3)读0x16公共应用基本数据文件
		bzero(sndbuf, sizeof(sndbuf));
		bzero(revbuf, sizeof(revbuf));
		memcpy(sndbuf,"\x00\xb0\x96\x00\x50",5);
		ret = Rf24GPassthrough(revbuf, 2, sndbuf, 5);
		if(ret == 0)
			return ST_ERROR;
		if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
			return ST_ERROR;
		
#ifdef _debug_CPU_
		printf("read 96 ret=%d\n", ret);
		debugdata(revbuf,ret,1);
		printf("\n");
#endif
		
		
        Card_Area = revbuf[57];//县区号
		
		//如果卡类不在特殊卡类定义的区域内，就按照普通卡折扣进行交易
		MSG_LOG("Card_Area = %02X\n",Card_Area);
#ifndef JIULONG 

//判断是否是本郊县
		
#ifdef MZT_JL
	if(Card_Area == code_96_57 ||(Card_Area==1&&(cardMessage.card_catalog -0x40==CARD_OLDMAN_BUS ||cardMessage.card_catalog -0x40==CARD_Love_BUS||cardMessage.card_catalog -0x40==CARD_Love_BUS_2)))  //九龙比较特殊,现在是01的老人卡,残疾人可以刷，其他01都当普通卡
		
#else
		if(Card_Area == code_96_57)  //本郊县
#endif
	{
		YouHui_Area_other=0xae;
		YouHui_Area = 1;//可以优惠区域
		
		memcpy(YouHui_Date,revbuf+58,4);
		MSG_LOG("YouHui_Date:");
		BCD_LOG(revbuf,58,4);
		//判断有效期
		//城区代码   0x80:北川  0x40:盐亭 0x20:三台  0x10:梓潼  0x08:安县  0x04:平武县
		ret = cardMessage.card_catalog -0x40;		//CPU 卡81 开始的 M1 卡41开始
		memcpy(YouHui_Date,revbuf+58,4);
		//看有效期
		//看有效期
		switch(Card_Area)
		{
		case 0x80:
			if(CARD_TIME(YouHui_Date)==CARD_NO_TIME &&((ret==CARD_STUDENT_BUS)||(ret==CARD_Love_BUS)||(ret==CARD_Love_BUS_2)||(ret == CARD_STUFF_BUS)||(ret==CARD_ZILU_BUS)))
			{
#ifdef _debug_CPU_
				debugstring("过期:\r\n");
				debugdata(YouHui_Date,4,1);
#endif
				return CARD_NO_TIME;
			}
			break;
		case 0x20:
			if(CARD_TIME(YouHui_Date)==CARD_NO_TIME &&((ret==CARD_Love_BUS)||(ret==CARD_Love_BUS_2)||(ret == CARD_STUFF_BUS)||(ret==CARD_ZILU_BUS)))
			{
#ifdef _debug_CPU_
				debugstring("过期:\r\n");
				debugdata(YouHui_Date,4,1);
#endif
				return CARD_NO_TIME;
			}
			break;
		case 0x40:
			if(CARD_TIME(YouHui_Date)==CARD_NO_TIME &&((ret==CARD_OLDMAN_BUS)||(ret==CARD_STUDENT_BUS)||(ret==CARD_Love_BUS)||(ret==CARD_Love_BUS_2)||(ret == CARD_STUFF_BUS)||(ret==CARD_ZILU_BUS)))
			{
#ifdef _debug_CPU_
				debugstring("过期:\r\n");
				debugdata(YouHui_Date,4,1);
#endif
				return CARD_NO_TIME;
			}
			break;
		case 0x01:
		case 0x03:
			if(CARD_TIME(YouHui_Date)==CARD_NO_TIME &&((ret==CARD_OLDMAN_BUS)||(ret==CARD_STUDENT_BUS)||(ret==CARD_Love_BUS)||(ret==CARD_Love_BUS_2)||(ret == CARD_STUFF_BUS)||(ret==CARD_ZILU_BUS)))
			{
#ifdef _debug_CPU_
				debugstring("过期:\r\n");
				debugdata(YouHui_Date,4,1);
#endif
				return CARD_NO_TIME;
			}
			break;
		default:
			if(CARD_TIME(YouHui_Date)==CARD_NO_TIME &&((ret==CARD_OLDMAN_BUS)||(ret==CARD_STUDENT_BUS)||(ret==CARD_Love_BUS)||(ret==CARD_Love_BUS_2)||(ret == CARD_STUFF_BUS)||(ret==CARD_ZILU_BUS)))
			{
#ifdef _debug_CPU_
				debugstring("过期:\r\n");
				debugdata(YouHui_Date,4,1);
#endif
				return CARD_NO_TIME;
			}
			break;
}
		}
	else
	{	
		YouHui_Area_other=0xad;
		MSG_LOG("其他郊县当普通卡处理\r\n");
			ret = cardMessage.card_catalog -0x40;
		if(ret!=CARD_NORMAL_BUS)
		{	cardMessage.card_catalog=CARD_NORMAL_BUS+0x41;
			//cardMessage.Zicard_catalog = 0;
		}
			YouHui_Area = 0;
	
	}
	
		
	


#else  //九龙公交
		//只有绵阳城区的卡可以实现优惠
		if(Card_Area == 0x01)
		{
			YouHui_Area = 1;//可以优惠区域
            memcpy(YouHui_Date,revbuf+58,4);
			
			MSG_LOG("YouHui_Date:");
			BCD_LOG(revbuf,58,4);
			
       	}
		else
			YouHui_Area = 0;

		ret = cardMessage.card_catalog -0x40;		//CPU 卡81 开始的 M1 卡41开始
		
		//老 残 员工卡要看有效期
		if((Card_Area == 0x03) &&((ret==CARD_OLDMAN_BUS)||(ret==CARD_Love_BUS)||(ret == CARD_STUFF_BUS)))
		{
#ifdef _debug_CPU_
			debugstring("看过期:\r\n");
			debugdata(YouHui_Date,4,1);
#endif
			
			
			memcpy(sndbuf,YouHui_Date,4);
			memcpy(sndbuf+4,"\x11\x59\x59",3);
			
			if (time_invalid(sndbuf) == 1)
			{
				memcpy((unsigned char*)&end_time, YouHui_Date, 4);//有效日期
				over_turn(4, (unsigned char*)&end_time);
				
				
				
				memcpy((unsigned char*)&itemp, (unsigned char *)&SysTime, 4);//当前日期
				over_turn(4, (unsigned char*)&itemp);
				
				if(end_time<itemp)
					return CARD_NO_TIME;	//过期卡
				
				nowTime = BCDTime2Long((unsigned char *)&SysTime);
				
				itemp = BCDTime2Long((unsigned char *)sndbuf);
				
				if (nowTime>itemp)
				{
					ret = nowTime-itemp;
				}else
				{
					ret = itemp-nowTime;
				}
				
				MSG_LOG("nowTime:%u\r\n",nowTime);
				MSG_LOG("itemp  :%u\r\n",itemp);
				
				dis_overdate = ret/86400;  //1天(d)=86400秒(s)
				
				
				MSG_LOG("dis_overdate:%d\r\n",dis_overdate);
			}else{
				return CARD_NO_TIME;	//过期卡
			}
			
			
			
			
		}
#endif
		if(IsInBlackList(cardMessage.PublishBicker+2) < FLASH_BLK_END){
			return CARD_BLACK_PRO;
		}	

	}else{

		keyIndex = 0x01;    //异地卡，使用01密钥索引

	}

	ret=cardMessage.card_catalog;

	if(cardMessage.card_catalog >= 0x80){
		ret = cardMessage.card_catalog - 0x80;
	}
	else if(cardMessage.card_catalog >= 0x40)
	{
		ret = cardMessage.card_catalog-0x40;
	}else{
		
		ret = cardMessage.card_catalog;
		
	}
	
	
	MSG_LOG("ret:0x%02X\r\n",ret);


	if(month_decide()) // 第二次去扣钱包(次数不能连刷)
	{
		return CARD_MONEY;
		
	}

	
	if((NewPriceTab.rate[ret]==204)||(NewPriceTab.rate[ret]==104))//禁止
	{
		return CARD_forbid;	
	}	
	
	
	if((NewPriceTab.rate[ret]==205)||(NewPriceTab.rate[ret] ==105)){//免费消费 且只有一次消费
		
		if(month_decide()) // 第二次去扣钱包(次数不能连刷)
		{
			disp_no_swipe();
			return ST_OK;
			
		}
		return CARD_FREE;
	}
	if((NewPriceTab.rate[ret]==202)||(NewPriceTab.rate[ret] ==102) )
	{
		
		return CARD_FREE;	
	
	}
	
	return CARD_MONEY;
}
extern void Save_delayCardinfo(unsigned char Cmode);
extern unsigned char getMiKey(void);
unsigned char CPU24GDealCard(unsigned char mode, unsigned char cool)
{
	unsigned char i;
	unsigned char sndbuf[64];
	unsigned char revbuf[RECORD_LEN+32];
	unsigned char TACC[4];
//	unsigned char PSAMDsno[4];
	unsigned int revLen,ret;
	unsigned int value;
	unsigned short ii;
#ifdef NO_CPU_PSAM_
	unsigned char CPUPKEY[16];
	unsigned char CPUpInData1[8];
	unsigned char CPUinitdat[8];
	unsigned char snddat[64];
#endif

	if(F_FLAG_VARY.stop_flag) 
		return ST_ERROR;
	//选择应用目录 3F01   03 6F 0C 84 06 5A 42 47 47 51 42 A5 04 9F 08 90 00

// 		if(mode==MONEY_CARD){//月票进来需要切换
// #ifdef _debug_CPU_
// 			debugstring("选择应用 3F01:开始\r\n");
// 			MSG_LOG("mode %d\r\n",mode);
// #endif
// 			memcpy(sndbuf, "\x02\x00\xA4\x00\x00\x02\x3F\x01",8);//有此卡需要用文件名，所以取不到时需要处理一下。
// 			deal2or3(sndbuf);
// 			ret = RfPassthrough(revbuf, 2, sndbuf, 8);
// #ifdef _debug_CPU_
// 			
// 		debugdata(sndbuf, 15, 1);
// 		debugdata(revbuf, ret, 1);
// #endif
// 			if(ret == 0)
// 				return ST_ERROR;
// 			else if(memcmp(revbuf+ret-2, "\x90\x00", 2)!=0)
// 				return 5;
// 		}

	memset(TACC,0,sizeof(TACC));

	if(Delay_Normal()){
		return ST_ERROR;
	}


// 	if(month_decide()) // 第二次去扣钱包(次数不能连刷)
// 	{
// 		disp_no_swipe();
// 		return ST_OK;	
// 	}

	if(CPU_24GRead_Money((unsigned char*)&value, 3))
		return ST_ERROR;
	over_turn(4, (unsigned char*)&value);



/*

	memcpy(sndbuf,"\x00\xB2\x01\xC4\x17",5);//取上笔记录 18文件
	ret = Rf24GPassthrough(revbuf, 2, sndbuf, 5);
	if(ret == 0)
		return ST_ERROR;
#ifdef _debug_CPU_
	debugstring("取18记录文件----\r\nsnd:\r\n");
	debugdata(sndbuf, 5, 1);
	debugdata(revbuf, ret, 1);
#endif


	memcpy(sndbuf, "\x02\x80\x5A\x00\x06\x02", 6);//memcpy(sndbuf, "\x02\x80\x5A\x00\x05\x02", 6);
	memcpy((unsigned char*)&ii, stuInitPurchaseRet.cSnq, 2);
	over_turn(2, (unsigned char*)&ii);
	ii += 1;
	over_turn(2, (unsigned char*)&ii);
	memcpy(sndbuf+6, (unsigned char*)&ii, 2);
	sndbuf[8] = 8;
#ifdef _debug_CPU_
	display(0,0,"放拔",0);
	delayxms(500);
	beep(3,50,50);
	debugstring("防拔:");
	debugdata(stuInitPurchaseRet.cSnq, 2, 1);
	debugdata(sndbuf, 9, 1);
#endif
	deal2or3(sndbuf);
	revLen = Rf24GPassthrough(revbuf, 2, sndbuf, 9);
	// 	if(revLen == 0)
	// 		return ST_ERROR;
#ifdef _debug_CPU_
	debugdata(revbuf, revLen, 1);//02 00 0C 00 00 ED F2 97 9B 6E 94 37 BC 90 00 F2 03
#endif

*/

	
	if(cardSound == 0xaa||cardSound==0xbb){//需要重刷	
#ifdef _debug_CPU_
		debugstring("aa余额:");
		debugdata((unsigned char*)&value, 4, 1);
		debugdata((unsigned char*)&a_sumR, 4, 1);
#endif
		if(value < a_sumR){//上次已经扣成功
			memcpy(sndbuf, "\x02\x80\x5A\x00\x06\x02", 6);//memcpy(sndbuf, "\x02\x80\x5A\x00\x05\x02", 6);
			memcpy((unsigned char*)&ii, stuInitPurchaseRet.cSnq, 2);
			over_turn(2, (unsigned char*)&ii);
			ii += 1;
			over_turn(2, (unsigned char*)&ii);
			memcpy(sndbuf+6, (unsigned char*)&ii, 2);
			sndbuf[8] = 8;
#ifdef _debug_CPU_
			display(0,0,"放拔",0);
			delayxms(500);
			beep(3,50,50);
			debugstring("防拔:");
			debugdata(stuInitPurchaseRet.cSnq, 2, 1);
			debugdata(sndbuf, 9, 1);
#endif
			deal2or3(sndbuf);

			revLen = Rf24GPassthrough(revbuf, 2, sndbuf, 9);
			if(revLen == 0)
				return ST_ERROR;
#ifdef _debug_CPU_
			debugdata(revbuf, revLen, 1);//02 00 0C 00 00 ED F2 97 9B 6E 94 37 BC 90 00 F2 03
#endif
			if(revLen == 0)//返回超时,错误
				return ST_ERROR;
			if((memcmp(revbuf+revLen-2, "\x90\x00", 2)!=0) &&\
				(memcmp(revbuf+revLen-2, "\x6C\x08", 2)!=0))
				goto CPUMAC2deal;//取不成功02 69 85 
			
			memcpy(TACC, revbuf+revLen-6, 4);//返回的TAC码
			memcpy(sndbuf, revbuf+revLen-10, 4);//用MAC2码
			//			return ST_ERROR;
			goto CPUMAC2deal0;
		}

	}
	a_sumR = value;	
#ifdef _debug_CPU_
	debugstring("a_sumR:");
	debugdata((unsigned char*)&a_sumR, 4, 1);
#endif
	
#ifdef NO_CPU_PSAM_
	memset(G24PsamNo, 0, 6);
	Get_SerialNumF4(G24PsamNo+2);
#ifdef _debug_CPU_
	debugstring("G24 PsamNo:");
	debugdata(G24PsamNo, 6, 1);
#endif
#endif


#ifndef BUS_MZT
	memcpy(sndbuf, "\x80\xca\x00\x00\x09", 5);
#ifdef _debug_CPU_
	debugstring("取安全认证码:");
	debugdata(sndbuf, 5, 1);
#endif
	ret = Rf24GPassthrough(revbuf, 2, sndbuf, 5);//获取用户卡安全认证识别码
	
	if(ret == 0)
		return ST_ERROR;
	if(memcmp(revbuf+ret-2, "\x90\x00", 2)==0){
		//		memcpy(Safe_Code,revbuf+1,9);
	}
#ifdef _debug_CPU_
	debugdata((unsigned char*)&ret, 1, 1);
	debugdata(revbuf, ret, 1);
	//	debugstring("Safe_Code:");
	// 	debugdata(Safe_Code,9,1);
#endif
	
	memcpy(sndbuf, revbuf+1, 9);
	if(CpuCardCheck(PSAM_YD, revbuf, sndbuf, 9)==ST_ERROR)//PSAM 卡验证安全认证码别码,新加
		return ST_ERROR;
#endif	//#ifdef BUS_MZT




	debugstring("keyIndex::");
	debugdata((unsigned char *)&keyIndex,1,1);


//8050 0101 0B||密钥标识符 [1]（0x01）||交易金额[4](0x00000001)||终端机编号[6]
	memset(sndbuf,0,64);
	memcpy(sndbuf, "\x80\x50\x01\x02\x0B", 5); i = 5;//11 INITIALIZE FOR PURCHASE命令
	sndbuf[i++] = keyIndex;//0x01;
	
//	cardMessage.card_catalog = CARD_NORMAL; CPU卡中有卡类
	if(mode== MONEY_CARD){//
		s_sum1 = get_s_sum1(0);//取得消费金额
		if((s_sum1 == 0) || (s_sum1 > 9900))
			return 22;

	}else if (mode == FREE_CARD)
	{
		s_sum1 = 0;
	}else
	{
		s_sum1=1;
	}

	value = s_sum1;
	over_turn(4, (unsigned char*)&value);
	memcpy(sndbuf+i, (unsigned char*)&value, 4); i+=4;
	memcpy(sndbuf+i, G24PsamNo, 6); i += 6;

//	sndbuf[i++] = 0x0f;
#ifdef _debug_CPU_
	debugstring("消费初始化:");
	debugdata(sndbuf, i, 1);
#endif
	ret = Rf24GPassthrough(revbuf, 2, sndbuf, i);
#ifdef _debug_CPU_
	debugdata((unsigned char*)&ret,4,1);
	debugdata(revbuf, ret, 1);
#endif
	if(ret == 0)
		return ST_ERROR;
	else{
		if(memcmp(revbuf+ret-2, "\x94\x01", 2)==0){
			if(cool == 4)//月票连刷进来，不提示余额不足
				return 4;
			else
				return 3;
		}
		else if(memcmp(revbuf+ret-2, "\x90\x00", 2)!=0)
			return 5;
	}
	
	memcpy((unsigned char*)&stuInitPurchaseRet, revbuf+1, sizeof(stuInitPurchaseRet));
#ifdef _debug_CPU_
	debugdata((unsigned char*)&stuInitPurchaseRet, sizeof(stuInitPurchaseRet), 1);
//	debugstring("MAC1计算指令:");
#endif
	memcpy(stuSamInitPurchase.cRand, stuInitPurchaseRet.cRand, 4);
	memcpy(stuSamInitPurchase.cSnq, stuInitPurchaseRet.cSnq, 2);
	memcpy(stuSamInitPurchase.cMoney, (unsigned char*)&value, 4);
	
	memcpy((unsigned char*)&a_sum1, stuInitPurchaseRet.cBalance, 4);//余额
	over_turn(4, (unsigned char*)&a_sum1);
	if (a_sum1 >= 5000000)
	{
		return 21;
	}
	gucSoudnorm = 0;

	if((a_sum1>=s_sum1)&&(a_sum1<500)){
		gucSoudnorm = 1;
	}
	stuSamInitPurchase.cType = 0x06;
	
	memcpy(stuSamInitPurchase.cDateTime, (unsigned char*)&SysTime, 7);


#ifdef _debug_CPU_
	debugstring("MAC1计算指令:");
	debugdata((unsigned char*)&stuSamInitPurchase,(28), 1);//+12
#endif

	stuSamInitPurchase.cKeyVer = stuInitPurchaseRet.cKeyVer;//0x05
	stuSamInitPurchase.cKeyEn = stuInitPurchaseRet.cKeyEn;


	memcpy(stuSamInitPurchase.cKeyFactor, cardMessage.PublishBicker+2, 8);//有28个数据
//	memcpy(stuSamInitPurchase.cPublisherSign, cardMessage.CityCode, 2);//城市代码
//	memcpy(stuSamInitPurchase.cPublisherSign+2,"\xFF\x00\x00\x00\x00\x00", 6);//有36个数据
//	memcpy(stuSamInitPurchase.cPublisherSign, stuPubInfZaoZuang.cPublisherSign, 8);//这个...

	//52B58B730005000000010620110402154217 05000000000090002030
	//4ACF30AF 0005 00000001 06 20110402154422 01000000000090002030 
	memset(revbuf, 0xff, 50);
	memset((unsigned char*)&stuDebitPurchase, 0, sizeof(stuDebitPurchase));



	if(CpuCardSetMac(PSAM_GJ, revbuf, (unsigned char*)&stuSamInitPurchase, 28) == ST_ERROR)
	{
		//		DeactiveCard(PSAM_YD);
		return 6;
	}
	
#ifdef _debug_CPU_
	debugstring("MAC OK!:");
	debugdata(revbuf, 8, 1);
#endif
	memcpy(stuDebitPurchase.cTermSnq, revbuf, 4);
	memcpy(stuDebitPurchase.cMac, revbuf+4, 4);
	memcpy(stuDebitPurchase.cDateTime, (unsigned char*)&SysTime, 7);
	memcpy(g24PSAMDsno, revbuf, 4);//--4字节的终端脱机交易序号  和 MAC1

		memcpy(sndbuf, "\x80\x54\x01\x00\x0F", 5); i = 5;

	memcpy(sndbuf+i, (unsigned char*)&stuDebitPurchase, 0x0f); i+=0x0f;
	memset(revbuf, 0xff, 50);
// 	deal2or3(sndbuf);
#ifdef _debug_CPU_
	debugstring("chek Mac1:");
	debugdata(sndbuf, i, 1);
#endif

	ret = Rf24GPassthrough(revbuf, 2, sndbuf, i);
	if(ret == 0){
//		debugdata(revbuf, 20, 1);
//		debugdata(revbuf, 20, 1);
#ifndef BUS_PINGXIANG_
#ifdef AUDIO_SOUND_OLD
		audio_old(Audio_TRY_AGAIN);		//请重刷
#else
		audio(Audio_TRY_AGAIN);		//请重刷
#endif
#endif 
		if(mode == MONTH_CARD)
			cardSound = 0xbb;//出错，需要重刷卡
		else
			cardSound = 0xaa;//出错，需要重刷卡
		card_ser = c_serial;
		return 7;
	}
	

#ifdef _debug_CPU_
	debugstring("chek Mac1 OK:");
	debugdata((unsigned char*)&ret, 4, 1);
	debugdata(revbuf, ret ,1);
#endif


	if(memcmp(revbuf+ret-2, "\x93\x02", 2)==0)
		return 9;
	if(memcmp(revbuf+ret-2, "\x90\x00", 2)!=0){
		if(mode == MONTH_CARD)
			cardSound = 0xbb;//出错，需要重刷卡
		else
			cardSound = 0xaa;//出错，需要重刷卡
		card_ser = c_serial;
		return 7;
	}//其它错误
	
	memcpy(TACC, revbuf+ret-10, 4);//返回的TAC码 SPI -13
	memcpy(sndbuf, revbuf+ret-6, 4);
#ifdef _debug_CPU_
//	debugdata((unsigned char*)&pmoney, 4, 1);
	debugdata(sndbuf, 4, 1);
	debugstring("消费成功:\r\n");
#endif
#ifdef _debug_CPU_
	debugstring("TAC:");
	debugdata(TACC, 4, 1);
	debugstring("MAC2:");
	debugdata(sndbuf, 4, 1);
	debugdata((unsigned char*)&a_sum1, 4, 1);
#endif
	a_sum1 -= s_sum1;//应该放在这里
CPUMAC2deal0:

#ifndef NO_CPU_PSAM_
	CPUMAC2(PSAM_YD, revbuf, sndbuf, 4);
#endif	
CPUMAC2deal:
	memcpy(cardMessage.PbocTac, TACC, 4);	// TAC码
#ifdef _debug_CPU_
	debugstring("计算MAC2:");
	debugdata(revbuf, 10, 1);
#endif

	memset(revbuf, 0, 70);
	memcpy((unsigned char*)&dis_sum2,(unsigned char*)&a_sum1,4);
// 	if(mode==MONEY_CARD){
// 		BuildRecorde(ID_REC_TOLL, revbuf);
// 		money_msg(ID_REC_TOLL, a_sum1, s_sum1, 2);
// 	}
// 	else{
// 		BuildRecorde(ID_REC_MON, revbuf);
// 		money_msg(ID_REC_MON, a_sum1, s_sum1, 2);//月票
// 	}

	BuildRecorde(ID_REC_TOLL, revbuf);
	money_msg(ID_REC_TOLL, a_sum1, s_sum1, 2);

	Save_delayCardinfo(0);	
	
	WriteRecord(revbuf, 0);
	Sound_cityCMD(0);

	card_ser = 0;
	cardSound = 0;	
	gMCardCand=0;
		
#ifdef _debug_CPU_
	debugstring("-------\r\n");
	debugstring("消费成功:\r\n");
	if(mode==MONTH_CARD)
		sprintf((char*)revbuf, "余次:%d次\r\n", (a_sum1));
	else
		sprintf((char*)revbuf, "余额:%d.%02d元\r\n", (a_sum1/100), (a_sum1%100));

	debugstring((char*)revbuf);
	if(mode==MONTH_CARD)
		sprintf((char*)revbuf, "扣次:%d次\r\n", s_sum1);
	else
		sprintf((char*)revbuf, "扣款:%d.%02d元\r\n", (s_sum1/100), (s_sum1%100));
	debugstring((char*)revbuf);
	debugstring("-------\r\n");
#endif
	return ST_OK;//消费成功
}

void delay_card(void)
{
	unsigned char i;
	unsigned int sno1;
	unsigned char buffer[10];
	sno1 = c_serial;
	i = 0;

	do {
		i++;
		if (GetTypeAUID(buffer)!=0) 
		{
			if (memcmp((unsigned char *)&sno1,buffer,4) !=0) 
			{
				return;
			}
			else{
				i = 0;
			}
		}
		delayxms(10);
	} while(i<50);
	return;
}


extern void DisRetry2(void);
extern void DisRetry(void);
void CPU24GMainCard(void)
{
	unsigned char ret=0,ret_mzt=0;//,ret_JTB=0;
	unsigned char ret_pboc = 0;
	char disbuf[20];

	if(F_FLAG_VARY.stop_flag) 
		return;
	gMCardCand = CARDSTYLE_24CPU;
	if((g24PurError>0)&&(g24PurError<0xa0))//等手机模块初始化完成
	{
		g24PurError--;
	//	debugstring("等待初始化-1\r\n");
		delayxms(500);
	//	delayxms(10);
		return;
	}
	
	if(g24PurError == 0xaa)
	{
		//debugstring("等待初始化-2\r\n");
		DisRetry();
	}
	if((gG24Ver <= 0xA0)||(gG24Ver > 0xA3)){//没取到版本
			//	debugstring("等待初始化-3\r\n");
		check24GVer();
	//	return ;
	}
	if(G24Profile() != 1){
		
			//	debugstring("等待初始化-4\r\n");
		
		return;
	}
	BlightOpen();
	
#ifdef _debug_
	debugstring("询卡成功:CSNO:");
	debugdata(gG24CardNo, 8, 1);
#endif
	
// 	if(g24Delay_Normal())
// 		return;

	ret = CPU24GcardType();//取卡信息
	ret_mzt = ret;

// 	if(gchn_eng == 'E'){
// 		sprintf(disbuf, "code:%02d ", ret);
// 	}
// 	else{
// 		sprintf(disbuf, "错误:%02d ", ret);
// 	}

#ifdef _debug_CPU_
	debugstring("ret:");
	debugdata((unsigned char*)&ret, 1, 1);
#endif

// 	if(gchn_eng == 'E'){
// 		sprintf(disbuf, "code:%02d ", ret);
// 	}
// 	else{
// 		sprintf(disbuf, "错误:%02d ", ret);
// 	}
	switch(ret)
	{
	case ST_OK:
		g24GDisFlash = 3;
		restore_flag = 0;
		SetTcpPacketTTLTime(RECORD_TTL);
		end_close_card(1);//这个不起作用。
		return;

	case CARD_STOPWORK:		
		error(ERR_CARD_STOP_WORK,0);
		break;

	case APP_LOCK:	
	case NO_JTB_UNION:

		MSG_LOG("走银联流程试试\r\n");
		//ret_pboc = 
			cpuPBOCmain();
// 		if ((ret_pboc == ST_ERROR)&&(ret_mzt == NO_CITY_UNION))
// 		{
// 			ret = APP_LOCK;	//
// 			break;
// 		}

		break;
		
	case CARD_NO_SYSTEM:	
		error(ERR_CARD_NO_SYSTEM,0);
		break;
	case CARD_WHITE_BUS:				//白卡
		error(ERR_CARD_WHITE_BUS,0);
		break;
	case CARD_NO_USE:
		error(ERR_CARD_NO_USE,0);
		break;

	case MONTH_CARD:
// 		ret = CPU_month_deal();//公司月票
// #ifdef _debug_CPU_
// 		debugstring("ret:");
// 		debugdata((unsigned char*)&ret, 1, 1);
// #endif
// 		if((ret == 3)||(ret == 4))//余额不足  
// 			ret = CPUDealCard(MONEY_CARD, 3);//扣钱

		break;
	case FREE_CARD:
	case MONEY_CARD:
		ret = CPU24GDealCard(ret, 0);

		break;
		
//		goto CPUMEnd;

	case NO_WriteNUM:
		MessageBox(1, "城市未在白名单!");
		led_on(LED_RED);
		restore_flag=3;
		goto CPUMEnd;

	case CARD_forbid:
		led_off(ALL_LED);
		delayxms(5);
		MessageBox(1, "此卡禁止刷卡!");
		led_on(LED_RED);
		SoundMessage(SOUND_FLING_MONEY);//请投币
		restore_flag=3;
		goto CPUMEnd;
// 	case CARD_STOPWORK:
// 		led_off(ALL_LED);
// 		delayxms(5);
// 		led_on(LED_RED);
// 		restore_flag=3;
// 		goto CPUMEnd;
	case 9:
	case 8:
		cls();
		display(0, 0, disbuf, 0);
		if(gchn_eng == 'E'){
			display(3,0,"illegal card",DIS_CENTER);
		}
		else
			display(3, 0, "非法卡", DIS_CENTER);
		SoundMessage(INVALID_CARD);
		goto CPUMEnd;
	case CARD_NO_TIME:
		cls();
		display(0, 0, disbuf, 0);
		if(gchn_eng == 'E'){
			display(3,0,"out date card",DIS_CENTER);
		}
		else{
			display(3, 0, "卡过期或没启用", DIS_CENTER);
		}
		memset(disbuf,0,sizeof(disbuf));
		strcpy(disbuf,"过期日:");
		BCD2Ascii(YouHui_Date,disbuf+7,4);
		display(5, 0, disbuf, DIS_CENTER);
		restore_flag = 3;
		
		SoundMessage(SOUND_FLING_MONEY);//请投币

		goto CPUMEnd;
	case CARD_BLACK_PRO:
		cls();
		display(0,0,"警告：黑名单卡!",0);
		display(2,4,"请投币!!",0);
		SoundMessage(INVALID_CARD);
		restore_flag=3;
		if (gMCardCand== CARDSTYLE_CPU)
		{
			operat_Black_MZT();
		}else 
		{
			operat_Black();
		}


		goto CPUMEnd;

	default:
		return;
	}
	//、、if(gchn_eng == 'E'){
// 		sprintf(disbuf, "code:%02d ", ret);
// 	}
// 	else{
// 		sprintf(disbuf, "错误:%02d ", ret);
// 	}
	switch (ret)
	{
	case ST_ERROR:
		return;

	case CARD_STOPWORK:		
		error(ERR_CARD_STOP_WORK,0);
		break;
	case APP_LOCK:		
		cls();
		display(0,0,"警告：应用锁定!",0);
		display(2,4,"请投币!!",0);
		SoundMessage(INVALID_CARD);
		restore_flag=3;
		break;


	case CARD_BLACK_PRO:
		cls();
		display(0,0,"警告：黑名单卡!",0);
		display(2,4,"请投币!!",0);
		SoundMessage(INVALID_CARD);
		restore_flag=3;
		end_close_card(1);//这个不起作用。
		break;
	case 21:
	case RET_MONEY_SN_EROR:																//两个钱包都错误
		error(ERR_MONEY_SN, 0);
		end_close_card(1);//这个不起作用。
		break;
	case 22:
		if(gchn_eng == 'E'){
			display(0,0,"hint:", DIS_Cls);
			display(2,0,"pos price error",DIS_CENTER);
		}
		else{
			display(0,0,"提示:", DIS_Cls);
			display(2,0,"票价信息不正确",DIS_CENTER);
		}
		end_close_card(1);//这个不起作用。
		restore_flag = 3;
		break;
	case CARD_NO_TIME:
		cls();
		display(0, 0, disbuf, 0);
		if(gchn_eng == 'E'){
			display(3,0,"out date card",DIS_CENTER);
		}
		else{
			display(3, 0, "卡过期或没启用", DIS_CENTER);
		}
		end_close_card(1);//这个不起作用。
		break;
	case 3:
		cls();
		display(0, 0, disbuf, 0);
		if(gchn_eng == 'E'){
			display(3,0,"NO SAVE ACCOUNT",DIS_CENTER);
		}
		else{
			display(3, 0, "余额不足", DIS_CENTER);
		}
		SoundMessage(SOUND_FLING_MONEY);
		end_close_card(1);//这个不起作用。
		break;
	case 4:
		cls();
		display(0, 0, disbuf, 0);
		display(3, 0, "扣次不能连刷", DIS_CENTER);
		SoundMessage(SOUND_FLING_MONEY);
		end_close_card(1);//这个不起作用。
		break;
	case 6:
	case 7:
		cls();
		display(0, 0, disbuf, 0);
		if(gchn_eng == 'E'){
			display(3,0,"MAC1 eror",DIS_CENTER);
		}
		else{
			//display(3, 0, "MAC1计算错误", DIS_CENTER);
			ERR_DIS("EP ERRO A0",&ret);
		}
		//		CPUPsamInitialize();
		
		break;
	case 9:
	case 8:
		cls();
		display(0, 0, disbuf, 0);
		if(gchn_eng == 'E'){
			display(3,0,"illegal card",DIS_CENTER);
		}
		else
			display(3, 0, "非法卡", DIS_CENTER);
		SoundMessage(INVALID_CARD);
		end_close_card(1);//这个不起作用。
		break;
	case ST_OK://成功
		restore_flag = 3;
		SetTcpPacketTTLTime(RECORD_TTL);
		break;
	default:
		// 		cls();
		// 		display(0, 0, disbuf, 0);
		// 		display(3, 0, "末定义错误", DIS_CENTER);
		break;
	}
CPUMEnd:
	end_close_card(1);
	g24GDisFlash = 2;
	//	delayxs(3);
	restore_flag = 0;
}
#endif

