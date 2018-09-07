
/*
	厦门盛华电子2.4G读头的手机卡，使用M1卡功能，只有三个扇区：
	1－发行区、2－钱包区、3－公共信息区

*/


#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "..\include\include.h"
#include "szct.h"		
 
#ifdef _XMSHDZ_24G_
#include "Cpucard.h"

extern unsigned char CPUMAC2(unsigned char PsamID, unsigned char *pMac, unsigned char *pData, unsigned char len);
extern unsigned char CpuCardSetMac(unsigned char PsamID, unsigned char *pMac, unsigned char *pData, unsigned char len);
extern void dis_time_mini(unsigned char x, stcomtime *time);
extern void save_normal(void);
extern void save_h_month(void);
extern unsigned char month_decide(void);
extern void CountPoint(unsigned char mode);
extern void disp_no_swipe(void);
extern void Save_delayCardinfo(unsigned char Cmode);
extern void SetTcpPacketTTLTime(unsigned char val);

extern unsigned char Delay_Normal(void);
extern unsigned char GetDateTime(void);
extern void debugstring(char *str);
extern void debugdata(unsigned char *value, unsigned int uclen, unsigned char mode);
extern unsigned int get_s_sum1(unsigned char mode);
extern void BuildRecorde(unsigned char delType, unsigned char *recBuf);
extern void money_msg(unsigned char dmode, unsigned int remM, unsigned int pucM, unsigned char cMOde);
extern unsigned char WriteRecord(unsigned char *buffer, unsigned char mode);
extern void SoundMessage(unsigned char cmd);
extern void error(unsigned char code1,unsigned char Index);
extern unsigned char getMiKey(void);
extern unsigned int IsInBlackList(unsigned char *c_card_no);
extern void Sound_cityCMD(unsigned char sMode);

void XMSH_end_card(void);

extern unsigned char restore_flag;	//显示复原
extern unsigned int g24GDisFlash;	//24G卡界面刷新
extern Parameter8 CARD_PUBLIC;
extern stcomtime SysTime;
extern unsigned int a_sum1,s_sum1;
extern Parameter5 cardMessage;
extern unsigned char gMCardCand;
extern pFistVary_1 pFistVary;
extern unsigned int c_serial;
extern unsigned char cardSound;			//刷卡没有成功标志
extern unsigned int card_ser;
extern unsigned int startTime,endTime,nowTime,endTimeEn;
extern unsigned char gucEndTimeFlag;
extern Parameter3 ComSumeFile;
extern unsigned char gucSoudnorm;
extern stPricetable NewPriceTab;
extern unsigned int a_sumR;//手机钱包中的真实金额

unsigned char gcomBuf[256];
volatile unsigned short gComindex;
volatile unsigned char flag;
unsigned int gZKXLdly;
unsigned char XMSHRFstyle;
unsigned char gG24CardNo[10];//卡号，钱包卡号

void comParaInit(void)
{
	gComindex = 0;
	flag = 0;
	gZKXLdly = 0;
}

void OcomParaInit(void)
{
	gComindex = 0;
	flag = 0;
	gZKXLdly = 0;
	XMSHRFstyle = 0;
}

void XMSH_Hand(unsigned char vl)
{
	if(gComindex == 0){
		if(vl != 0x90)
			return;
	}
	if(flag == 0x5A){//上个数据包没处理完
		comParaInit();
		return;
	}

	gcomBuf[gComindex++] = vl;
	if(gComindex > 2){//已经收完了长度，可以判断是否收完所有数据
		if(gComindex >= (gcomBuf[1]+2)){//收完成
			flag = 0x5A;
		}
	}
}

unsigned char XMSH_sndcarddat(unsigned char cmd, unsigned char *dat, unsigned short len)
{
	unsigned char buff[256];
	unsigned short i;
	
	comParaInit();
	
	i = 0;
	buff[i++] = cmd; //A0
	buff[i++] = len;
	if(len > 0){
		memcpy(buff+i, dat, len);
		i+=len;
	}	
#ifdef _debug_
	debugstring("snddat:");
	debugdata(buff, i, 1);
#endif
	com_snd(G24Com, i, buff);
	return 0;
}
//发送命令，要等应答
unsigned short XMSH_sndCmd(unsigned char *dat, unsigned short len, unsigned char *rev, unsigned char cmd)
{
	unsigned short us;
	unsigned int dlyo=0;
	
	XMSH_sndcarddat(cmd, dat, len);//发送命令
	
	while(1){//等应答
		if(flag == 0x5A){
#ifdef _debug_
			debugstring("sndCmdR:");
			debugdata(gcomBuf, gComindex, 1);
#endif
			us = gcomBuf[1];
			memcpy(rev, gcomBuf+2, us);
			comParaInit();
			return us;
		}
		delayxms(1);
		if(dlyo++ > 500){
#ifdef _debug_
			debugstring("s o:");
#endif
			return 0;

		}
	}
}

unsigned char XMSH_cmdDeal(unsigned char *PscardID)
{
	if(XMSHRFstyle == 1){//有手机卡进入
		if(memcmp(gcomBuf+2, "\x9C\x02", 2) == 0){//找到卡了
			XMSHRFstyle = 2;
			memset(PscardID, 0, 10);
			memcpy(PscardID, gcomBuf+5, 8);//900D9C021999920213006150409C02
			return 1;
		}
		else if(memcmp(gcomBuf+2, "\x9C\x03", 4) == 0){
			return 0;
		}
		else
			return 0;
	}
	else if(XMSHRFstyle == 2){//处于卡操作中,理论上是不会到这里的。
#ifdef _debug_
		debugstring("XMSHRFstyle == 2:");
		debugdata(gcomBuf, gComindex, 1);
#endif
		return 2;//有卡号
	}
	return 0;
}
/*
unsigned char XMSH_cmdDeal(void)
{
	if(XMSHRFstyle == 1){//有手机卡进入
		if(memcmp(gcomBuf+2, "\x9C\x02", 2) == 0){//找到卡了
			XMSHRFstyle = 2;
			return 1;
		}
		else if(memcmp(gcomBuf+2, "\x9C\x03", 4) == 0){
			return 0;
		}
		else
			return 0;
	}
	else if(XMSHRFstyle == 2){//处于卡操作中,理论上是不会到这里的。
#ifdef _debug_
		debugstring("XMSHRFstyle == 2:");
		debugdata(gcomBuf, gComindex, 1);
#endif
		return 2;//有卡号
	}
	return 0;
}
*/
void XMSFBreaKCard(void)//4断开rf: 80 05 90 B0 00 00 00
{
	unsigned char buffer[32];
	memcpy(buffer, "\x80\x05\x90\xB0\x00\x00\x00", 7);//关闭RF
//	memcpy(buffer, "\x80\x05\x40\x04\x00\x00\x00", 7);//断开连接
// 	debugstring("断开rf:");
// 	debugdata(buffer, 7, 1);
	XMSHRFstyle = 0;
	com_snd(G24Com, 7, buffer);
}
//
unsigned char XMSFSelectFE(void){
	
	unsigned char snddat[256];
	unsigned char revbuff[256];
	unsigned char ret;

	memcpy(snddat, "\xA0\xA4\x04\x00\x10\x41\x63\x63\x6F\x75\x6E\x74\x65\x72\x20\x41\x70\x70\x00\x00\x00", 21);//选择Accounter应用
	//	memcpy(snddat, "\x00\xA4\x04\x00\x0E\x31\x50\x41\x59\x2E\x53\x59\x53\x2E\x44\x44\x46\x30\x31", 19);//Select PSE
	//	memcpy(snddat, "\x00\xa4\x04\x04\x09\xA0\x00\x00\x00\x03\x86\x98\x07\x01", 14);//Select PSE
	ret = XMSH_sndCmd(snddat, 21, revbuff, 0x80);
	if(ret == 0){
		return 1;
	}
	if((revbuff[0] != 0x90)||(revbuff[1] != 0x00)){
		return 1;
	} //90029000
	return 0;
}

unsigned char XMSFRequese(unsigned char *dat){
	unsigned char snddat[256];
	unsigned char revbuff[256];
	unsigned short ret, ss;
	
	for(ss=0; ss<5; ss++){
		delayxms(50);//80 0B 40 08 01（扇区号） 00（00:B密钥, 01:A密钥） 06  FF FF FF FF FF FF(具体的密钥值)
		memcpy(snddat, "\x40\x0A\x00\x00\x00", 5);
		//memcpy(snddat, "\x40\x08\x01\x00\x06\xFF\xFF\xFF\xFF\xFF\xFF", 0x0B);
		ret = XMSH_sndCmd(snddat, 0x05, revbuff, 0x80);
		if(ret == 0){
			return 1;
		}
		if((revbuff[0] == 0x9C)&&(revbuff[1] == 0x03)){
			continue; 
		}//902800751510000000000000000000000000000000000000000000000000000000000000000000009000
		if((revbuff[ret-2] == 0x90)&&(revbuff[ret-1] == 0x00)){
			memcpy(dat, revbuff, 4);
			break;
		}
		else
		{
			return 2;
		}
	}
	if(ss >= 5){//询卡失败
		return 3;
	}
	return 0;
}
//(00:B密钥, 01:A密钥）
unsigned char XMSFAuth(unsigned char Sec, unsigned char mode, unsigned char *cKey)
{
	unsigned char snddat[256];
	unsigned char revbuff[256];
	unsigned short ret;

	//发送：80 0B 40 08 01（扇区号） 00（00:B密钥, 01:A密钥） 06  FF FF FF FF FF FF(具体的密钥值)
	memcpy(snddat, "\x40\x08\x01\x00\x06\xFF\xFF\xFF\xFF\xFF\xFF", 0x0B);
	snddat[2] = Sec;
	snddat[3] = mode;//(00:B密钥, 01:A密钥）
	memcpy(snddat+5, cKey, 6);

	ret = XMSH_sndCmd(snddat, 0x0B, revbuff, 0x80);//800B400801000670304B197030
	if(ret == 0){
		return 1;
	}
	if((revbuff[ret-2] != 0x90)||(revbuff[ret-1] != 0x00)){
		return 2;
	}
	return 0;
}

unsigned char XMSFRead(unsigned char block, unsigned char *rdat)
{
	unsigned char snddat[256];
	unsigned char revbuff[256];
	unsigned short ret;
	
	//读块发送：80 05 40 02 01 00(块号) 00
	memcpy(snddat, "\x40\x02\x01\x00\x00", 0x05);
	snddat[3] = block;
	ret = XMSH_sndCmd(snddat, 5, revbuff, 0x80);
	if(ret == 0){
		return 1;
	}
	if((revbuff[ret-2] != 0x90)||(revbuff[ret-1] != 0x00)){
		return 2;
	}
	memcpy(rdat, revbuff, 16);
	return 0;
}

unsigned char XMSFWrite(unsigned char block, unsigned char *rdat)
{
	unsigned char snddat[256];
	unsigned char revbuff[256];
	unsigned short ret;
	
	//写块80 15 40 02 02 00(块号)10 00 00 00 00 FF FF FF FF 00 00 00 00 00 FF 00 FF(16个字节的块数据)
	memcpy(snddat, "\x40\x02\x02\x01\x10\x00\x00\x00\x00\xFF\xFF\xFF\xFF\x00\x00\x00\x00\x01\xFE\x01\xFE", 21);
	snddat[3] = block;
	memcpy(snddat+5, rdat, 16);//加入写字节
	ret = XMSH_sndCmd(snddat, 21, revbuff, 0x80);
	if(ret == 0){
		return 1;
	}
	if((revbuff[ret-2] != 0x90)||(revbuff[ret-1] != 0x00)){
		return 2;
	}
	return 0;
}

//mode == 0减，其它加
unsigned char XMSFIDvalue(unsigned char block, unsigned char mode, unsigned int value)
{
	unsigned char snddat[256];
	unsigned char revbuff[256];
	unsigned short ret;
	//Inc 增值 80 15 40 02 03 00(块号) 10 64 00 00 00(金额)+12字节其它内容[日时分秒，BCD格式+机具号+88+01+XXXX]
	//8015400203001064000000271418301111111188010000
	memcpy(snddat, "\x40\x02\x03\x00\x10", 5);//\x64\x00\x00\x00", 9);
	if(mode == 0)
		snddat[2] = 4;
	else
		snddat[2] = 3;

	snddat[3] = block;
	memcpy(snddat+5, (unsigned char*)&value, 4);
	memcpy(snddat+9, (unsigned char*)&SysTime.day, 4);
	memset(snddat+13, '1', 4);
	snddat[17] = 0x88;
	snddat[18] = 0x01;
	snddat[19] = 0x00;
	snddat[20] = 0x00;
	ret = XMSH_sndCmd(snddat, 21, revbuff, 0x80);
	if(ret == 0){
		return 1;
	}
	if((revbuff[ret-2] != 0x90)||(revbuff[ret-1] != 0x00)){
		return 2;
	}
	return 0;
}

unsigned char XMSFtransf(unsigned char block)
{
	unsigned char snddat[256];
	unsigned char revbuff[256];
	unsigned short ret;
	//传送 80 0C 40 02 06 00(块号) 07 30 58 10 XX 01 09 00  （命令中时间为2008-9-1 10:58:30）
	//秒，分，时，星期(未用)，日，月，年(减08)
	memcpy(snddat, "\x40\x02\x06\x00\x07", 0x05);
	snddat[3] = block;
	memcpy(snddat+5, (unsigned char*)&SysTime.hours, 3);
	over_turn(3, snddat+5);
	memcpy(snddat+9, (unsigned char*)&SysTime.year_l, 3);
	over_turn(3, snddat+9);
	snddat[11] -= 0x08;

	ret = XMSH_sndCmd(snddat, 12, revbuff, 0x80);
	if(ret == 0){
		return 1;
	}
	if((revbuff[ret-2] != 0x90)||(revbuff[ret-1] != 0x00)){
		return 2;
	}
	return 0;
}

//消费程序
unsigned char XMSH_UIMm1Puchace(void)
{
	unsigned int ii;
	unsigned char buffer[256];
	unsigned char keya[6];
	unsigned char cNo[4];
	unsigned char pos;

	if(XMSFSelectFE() != 0){//90029000
		return 2;
	}
	if(XMSFRequese(buffer) != 0){//902800751510000000000000000000000000000000000000000000000000000000000000000000009000
		return 3;
	}
	memcpy((unsigned char*)&c_serial, buffer, 4);//物理卡号
	memcpy(cNo, buffer, 4);
	cNo[0] ^= 0x71;
	cNo[1] ^= 0x30;
	cNo[2] ^= 0x4B;
	cNo[3] ^= 0x19;
	memcpy(keya, cNo, 4);//1扇区默认密钥
	memcpy(keya+4, cNo, 2);//1扇区默认密钥
	
	if(Delay_Normal())
		return ST_ERROR;

	for(pos=0; pos<5; pos++){//写1扇区为发行区
		if(XMSFAuth(1, 1, keya) == 0){//认证成功
			break;
		}
	}
	if(pos >= 5)
		return 4;// 认证失败
	
	if(XMSFRead(0, buffer) != 0)
		return 5;
#ifdef _debug_
	debugstring("SMSH 1扇区信息0：\r\n");
	debugdata(buffer,16,1);
#endif	
	if(MiBlockInvalid(1,15,buffer)==ST_ERROR) 
	{
		return 6;
	}
	gMCardCand = CARDSTYLE_24XMSH;
	pos=0;
	memcpy(cardMessage.CityCode, buffer+pos, CITY_CODE_LEN);			//城市代码	2
	pos+=CITY_CODE_LEN;
	memcpy(cardMessage.TradeCode, buffer+pos, TRADE_CODE_LEN);			//行业代码	2
	pos+=TRADE_CODE_LEN;	
	memcpy((unsigned char *)cardMessage.PublishBicker, buffer+pos, PUBLISH_BICKER_LEN);	//卡流水号	4	
	memcpy(cardMessage.CardCheckCode, buffer+8, CARD_CHECH_CODE_LEN);	//mac码		4
	cardMessage.card_catalog=buffer[13];								//卡类		1
	cardMessage.Zicard_catalog = buffer[14];	//子卡类，月票类型
	
#ifdef _debug_
	debugstring("card_catalog:");
	debugdata((unsigned char*)&cardMessage.card_catalog,1,1);
#endif	
	switch(buffer[12])//启动标志
	{
	case STARTUP_OK:
		break;
	case STARTUP_STOP:
		return CARD_STOPWORK;
	case STARTUP_NO:
		return CARD_NO_USE;
	case STARTUP_BLACK:
		return CARD_BLACK_PRO;
	default:
		return CARD_NO_SYSTEM;
	}

	if(XMSFRead(1, buffer) != 0)////发行日期 有效日期 启用日期 卡押金	保留	校验
		return 7;
#ifdef _debug_
	debugstring("1扇区信息1:");
	debugdata(buffer, 16, 1);
#endif
	
	if(MiBlockInvalid(1,15,buffer)==ST_ERROR) 
	{
#ifdef _debug_
		debugstring("1扇区信息1 校验错\r\n");
#endif
		return 8;
	}
	//时间缓冲清0
	endTime = 0;
	startTime = 0;
	nowTime = 0;
	endTimeEn = 0;
	
	memcpy(buffer+50, buffer+4, 4);
	buffer[52] = bcdADD(buffer[52], 1);//有效日期开始提示年审但是能用,延时一个月。
	if(buffer[52]>0x12){
		buffer[52] = 1;
		buffer[51] = bcdADD(buffer[51], 1);
	}
	
	memcpy((unsigned char*)&endTime, buffer+50, 4);	//有效日期,处理后的。
	memcpy((unsigned char*)&endTimeEn, buffer+4, 4);//有效日期，处理前的
	memcpy((unsigned char*)&startTime, buffer+8, 4);//启动日期
	
	if(GetDateTime()==ST_ERROR) 
	{
		error(ERR_READ_TIME,0);
		return 0xF1;
	}	
#ifdef _debug_
	debugstring("systime:");
	debugdata((unsigned char*)&SysTime, 7, 1);
#endif
	memcpy((unsigned char*)&nowTime, (unsigned char *)&SysTime.year_h, 4);//读卡日期
	
#ifdef _debug_
	debugstring("chk:");
	debugdata((unsigned char*)&nowTime, 4, 1);
	debugdata((unsigned char*)&startTime, 4, 1);
	debugdata((unsigned char*)&endTime, 4, 1);
	debugdata((unsigned char*)&endTimeEn, 4, 1);
#endif
	over_turn(4, (unsigned char*)&nowTime);
	over_turn(4, (unsigned char*)&startTime);
	over_turn(4, (unsigned char*)&endTime);
	over_turn(4, (unsigned char*)&endTimeEn);

#ifdef _debug_
	debugstring("cardMessage.card_catalog:");
	debugdata((unsigned char*)&cardMessage.card_catalog, 1, 1);
#endif
	if(cardMessage.card_catalog ==CARD_WHITE_BUS)
	{
		MessageBox(1, "此卡未出售!");
		return 0xF1;
	}
	gucEndTimeFlag = 0;
	if((startTime>nowTime)||(endTime<nowTime)||(startTime>endTime)){//过期卡
		display(0,0,"警告：",0);
		display(2,1,"卡过期或未启用",0);
		audio(Audio_FLING_MONEY);	//请投币
		return 0xF1;
	}
	else if(endTimeEn<=nowTime){//需要提示“请年审了”
		gucEndTimeFlag = 1;
	}
	
	if(getMiKey()==ST_ERROR)
	{
		return 12;
	}

	if(IsInBlackList(cardMessage.PublishBicker) < FLASH_BLK_END){
		display(0,0,"警告：",0);
		display(2,1,"黑名单卡!",DIS_CENTER);
		audio(Audio_INVALID);	//请投币
		return 0xF1;
	}
	
	memcpy(keya, cardMessage.MoneyKEYA, 6);
	for(pos=0; pos<5; pos++){//3扇区为公共信息区
		if(XMSFAuth(3, 1, keya) == 0){//认证成功
			break;
		}
		if(memcmp(keya, "\xFF\xFF\xFF\xFF", 4) != 0){
			memcpy(keya, "\xFF\xFF\xFF\xFF\xFF\xFF", 6);
		}
		else{
			memcpy(keya, cardMessage.MoneyKEYA, 6);
		}
	}
	if(pos >= 5){
		debugstring("认证3扇区失败!\r\n");
		return 4;// 认证失败
	}
#ifdef _debug_
	debugstring("3扇区KEYA:");
	debugdata(keya,6,1);
#endif
	
	if(XMSFRead(0, buffer) != 0)//公共信息正本
		return 5;
#ifdef _debug_
	debugstring("公共信息正本:");
	debugdata(buffer,16,1);
#endif
	memcpy((unsigned char*)&CARD_PUBLIC, buffer, 8);
	if(CARD_PUBLIC.BlackFlag == 0x04){
		display(0,0,"警告：",0);
		display(2,1,"黑名单卡!",DIS_CENTER);
		audio(Audio_INVALID);	//请投币
		return 0xF1;
	}
	s_sum1 = get_s_sum1(0);
	
	for(pos=0; pos<5; pos++){//2扇区为钱包区
		if(XMSFAuth(2, 1, cardMessage.MoneyKEYA) == 0){//认证成功
			break;
		}
	}
	if(pos >= 5)
		return 4;// 认证失败
	
	if(XMSFRead(1, buffer) != 0)//钱包正本，钱包副本没用，因要实现加减值下一块必须是钱包格式，所以备份块不能加减值。
		return 5;
#ifdef _debug_
	debugstring("钱包:");
	debugdata(buffer,16,1);
#endif
	if(MifIsPurse(buffer)==ST_ERROR) 
	{
		display(0,0,"警告：",0);
		display(2,1,"钱包格式错误!",DIS_CENTER);
		return 0xF1;
	}
	memcpy((unsigned char*)&a_sum1, buffer, 4);//
	if(a_sum1 < s_sum1){
		display(0,0,"警告：",0);
		display(2,1,"钱包余额不足!",0);
		audio(Audio_FLING_MONEY);	//请投币
		return 0xF1;
	}
	if(XMSFIDvalue(1, 0, s_sum1) != 0){//减值出错
		return 13;
	}
	if(XMSFtransf(1) != 0){
		return 14;
	}
	money_msg(ID_REC_TOLL, a_sum1, s_sum1, 0);
	
	BuildRecorde(ID_REC_TOLL, buffer);
	card_ser = c_serial;
	ii = ((unsigned char*)&ComSumeFile.SumMoney)-((unsigned char*)&ComSumeFile);
	sysfewrite(BIT_COMSUME_FILE+ii, 4 ,(unsigned char*)&ComSumeFile.SumMoney);
	save_normal();
	SoundMessage(SOUND_DEAL);
	WriteRecord(buffer, 0);
	return 0xF1;
}

unsigned char XMSH_CPU_Read_Money(unsigned char *remb, unsigned char mode)
{
	unsigned char sndbuf[64];
	unsigned char revbuf[64];
	unsigned char ret;
#ifdef _debug_
	unsigned int temp;
#endif
	
	memcpy(sndbuf,"\x80\x5c\x00\x02\x04",56);
	ret = XMSH_sndCmd(sndbuf, 5, revbuf, 0xA0);
	if(ret == 0){
		return 1;
	}
#ifdef _debug_
	debugstring("---remb:====");
	debugdata(revbuf,ret, 1);
	memcpy((unsigned char*)&temp, revbuf, 4);
	over_turn(4, (unsigned char*)&temp);
	sprintf((char *)sndbuf, "余额:%d.%02d", temp/100, temp%100);
	debugstring((char*)sndbuf);
#endif
	memcpy(remb, revbuf, 4);
	
	return 0;
}

unsigned char XMSH_UIMcpuPuchace(void)
{
#ifdef NO_CPU_PSAM_
	unsigned char CPUPKEY[16];
	unsigned char CPUpInData1[8];
	unsigned char CPUinitdat[8];
#endif
	unsigned short ii;
	unsigned char snddat[100];
	unsigned char revbuff[256];
	unsigned char TACC[10];
	unsigned short ret;
	unsigned int i, pmoney, itemp, value;
//	pboc_init_capp_purchase_res stuInitPurchaseRet;
	//wangzhiyong@xunfang.com

//	checkSimVer();
	
	gMCardCand = CARDSTYLE_CPU;//CARDSTYLE_24XMSH;
	
	GetDateTime();

	//memcpy(snddat, "\x00\xA4\x04\x00\x09\xA0\x00\x00\x00\x03\x86\x98\x07\x01", 14);//Select PSE
	//ret = XMSH_sndCmd(snddat, 14, revbuff, 0xA0);
// 	memcpy(snddat, "\x00\xA4\x00\x00\x02\x3F\x00", 7);//Select PSE
// 	ret = XMSH_sndCmd(snddat, 7, revbuff, 0xA0);
// 	if(ret == 0){
// 		return ST_ERROR;
// 	}
// #ifdef _debug_
// 	debugstring("3F00选PSE:");
// 	debugdata(revbuff, ret, 1);
// #endif
// 	if(memcmp(revbuff+ret-2, "\x90\x00", 2)!=0)
// 		return ST_ERROR;

	memcpy(snddat, "\x00\xA4\x04\x00\x09\xA0\x00\x00\x00\x03\x86\x98\x07\x01", 14);//Select DF01
	ret = XMSH_sndCmd(snddat, 14, revbuff, 0xA0);
//	memcpy(snddat, "\x00\xA4\x00\x00\x02\x3F\x01", 7);//Select DF01
//	ret = XMSH_sndCmd(snddat, 7, revbuff, 0xA0);
	if(ret == 0){
		return ST_ERROR;
	}
#ifdef _debug_
	debugstring("3F01选PSE:");
	debugdata(revbuff, ret, 1);
#endif
	if(memcmp(revbuff+ret-2, "\x90\x00", 2)!=0)
		return ST_ERROR;
	
	memcpy(snddat,"\x00\xB0\x95\x00\x1E",6);//读应用序列号作为分散因子 //读15文件
	ret = XMSH_sndCmd(snddat, 6, revbuff, 0xA0);
	if(ret == 0){
		return ST_ERROR;
	}
#ifdef _debug_
	debugstring("读95文件:");
	debugdata(revbuff, ret, 1);
#endif	//A00007555D2A785203010003D2283301000000002013032720200101A9119000
	if(memcmp(revbuff+ret-2, "\x90\x00", 2)!=0)
		return ST_ERROR;
	
	cardMessage.card_catalog = revbuff[10];//卡类
	if(cardMessage.card_catalog == 0x50)//还没有售出的卡
		return 8;
	if(cardMessage.card_catalog < 32)
		cardMessage.card_catalog += CARD_NORMAL;
	
	memcpy((unsigned char*)&stuPubInfZaoZuang, revbuff+12, 8);//卡序列号
	memcpy(stuPubInfZaoZuang.cApp_sn, revbuff+11, 10);	//卡应用序列号，完整的，备用
	memcpy(cardMessage.CityCode, revbuff+2, 2);//城市代码
	memcpy(cardMessage.CardCheckCode, revbuff+4, 4);//MAC码
	memcpy(cardMessage.TradeCode, revbuff+8, 2);//行业代码

	memcpy(cardMessage.PublishBicker, revbuff+12, 4);//发卡流水号
#ifndef NO_CPU_PSAM_
	pmoney  = BCD2HEX(stuPubInfZaoZuang.cPublisherSign, 8);
	memcpy(cardMessage.PublishBicker, (unsigned char*)&pmoney, 4);
#endif
	
#ifdef _debug_
	debugstring("cardMessage.card_catalog:");
	debugdata((unsigned char*)&cardMessage.card_catalog,1, 1);
	debugdata((unsigned char*)&stuPubInfZaoZuang,50, 1);
	debugdata(NewPriceTab.rate+cardMessage.card_catalog, 1, 1);
#endif
	
	memcpy((unsigned char*)&itemp, (unsigned char *)&SysTime, 4);//当前日期
	over_turn(4, (unsigned char*)&itemp);
	memcpy((unsigned char*)&value, revbuff+20, 4);//启用日期
	over_turn(4, (unsigned char*)&value);
	memcpy((unsigned char*)&pmoney, revbuff+24, 4);//有效日期
	over_turn(4, (unsigned char*)&pmoney);
#ifdef _debug_
	debugdata(cardMessage.PublishBicker, 4, 1);
	debugdata((unsigned char*)&value, 4, 1);
	debugdata((unsigned char*)&pmoney, 4, 1);
#endif
	if(IsInBlackList(cardMessage.PublishBicker) < FLASH_BLK_END){
		return CARD_BLACK_PRO;
	}
	memcpy((unsigned char*)&c_serial, cardMessage.PublishBicker, 4);//物理卡号
//---------------------------------------------------------
//	消费过程开始	
	cardMessage.card_catalog = CARD_NORMAL_BUS;
	if(cardMessage.card_catalog >= 0x40)
		i = cardMessage.card_catalog - 0x40;
	else
		i = cardMessage.card_catalog;
	
	if((NewPriceTab.rate[i] == 104)||(NewPriceTab.rate[i] == 204)){
		return CARD_forbid;//此卡被禁止使用
	}

// 	if(Delay_Normal()){
// 		return ST_ERROR;
// 	}
// 	if(month_decide()) // 第二次去扣钱包(次数不能连刷)
// 	{
// 		disp_no_swipe();
// 		return ST_ERROR;
// 	}

// 	if(month_decide()) // 第二次去扣钱包(次数不能连刷)   需要处理，没有对物理卡号==
// 	{
// 		disp_no_swipe();
// 		return ST_OK;	
// 	}

	if(cardSound == 0xaa){//需要重刷
		if(card_ser!=c_serial){//不是同一张卡，不能处理
			return ST_ERROR;
		}
//		debugstring("Raa1:");
//		debugdata((unsigned char*)&card_ser, 4, 1);
//		debugdata((unsigned char*)&c_serial, 4, 1);
// 		
// #ifdef _debug_CPU_
// 		debugstring("需要重刷 ZBGGQB:开始\r\n");
// #endif
// 		memcpy(sndbuf, "\x02\x00\xA4\x04\x00\x06",6);
// 		memcpy(sndbuf+6, "ZBGGQB", 6);
// 		deal2or3(sndbuf);
// 		ret = RfPassthrough(revbuf, 2, sndbuf, 12);
// 		if(ret == 0)
// 			return ST_ERROR;
// 		if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
// 			return ST_ERROR;
// 		
// #ifdef _debug_CPU_
// 		debugstring("需要重刷 ZBGGQB:");
// 		debugdata(revbuf, ret, 1);
// #endif
		
		XMSH_CPU_Read_Money((unsigned char*)&value, 3);
		over_turn(4, (unsigned char*)&value);
#ifdef _debug_
		debugstring("aa余额:");
		debugdata((unsigned char*)&value, 4, 1);
		debugdata((unsigned char*)&a_sumR, 4, 1);
#endif
		if(value < a_sumR){//上次已经扣成功
			memcpy(snddat, "\x80\x5A\x00\x06\x02", 5);//memcpy(snddat, "\x80\x5A\x00\x05\x02", 5);
			memcpy((unsigned char*)&ii, stuInitPurchaseRet.cSnq, 2);
			over_turn(2, (unsigned char*)&ii);
			ii += 1;
			over_turn(2, (unsigned char*)&ii);
			memcpy(snddat+5, (unsigned char*)&ii, 2);
			snddat[8] = 8;
#ifdef _debug_
			debugstring("防拔:");
			debugdata(stuInitPurchaseRet.cSnq, 2, 1);
			debugdata(snddat, 9, 1);
#endif
			ret = XMSH_sndCmd(snddat, 9, revbuff, 0xA0);
			if(ret == 0)
				return ST_ERROR;
#ifdef _debug_
			debugdata(revbuff, ret, 1);//02 00 0C 00 00 ED F2 97 9B 6E 94 37 BC 90 00 F2 03
#endif
			if(ret == 0)//返回超时,错误
				return ST_ERROR;
			if((memcmp(revbuff+ret-2, "\x90\x00", 2)!=0) &&\
				(memcmp(revbuff+ret-2, "\x6C\x08", 2)!=0))
				goto CPUMAC2deal;//取不成功02 69 85 
			
			memcpy(TACC, revbuff+ret-6, 4);//返回的TAC码
			memcpy(snddat, revbuff+ret-10, 4);//用MAC2码
			//			return ST_ERROR;
			goto CPUMAC2deal0;
		}
// 		memcpy(sndbuf, "\x02\x00\xa4\x00\x00\x02\xdf\x01", 8);
// #ifdef _debug_CPU_
// 		debugstring("选主文件:");
// 		debugdata(sndbuf, 8, 1);
// #endif
// 		deal2or3(sndbuf);
// 		revLen = RfPassthrough(revbuf, 2, sndbuf, 8);
// #ifdef _debug_CPU_
// 		debugdata(revbuf, revLen, 1);
// #endif
// 		if(revLen == 0)
// 			return ST_ERROR;
	}
	//读余额 
	XMSH_CPU_Read_Money((unsigned char*)&a_sumR, 2);
	over_turn(4, (unsigned char*)&a_sumR); 
	
#ifdef _debug_
	debugstring("a_sumR:");
	debugdata((unsigned char*)&a_sumR, 4, 1);
#endif
	
#ifdef NO_CPU_PSAM_
	memset(G24PsamNo, 0, 6);
	Get_SerialNum(G24PsamNo+2);
#ifdef _debug_CPU_
	debugstring("G24 PsamNo:");
	debugdata(G24PsamNo, 6, 1);
#endif
#endif

//8050 0101 0B||密钥标识符 [1]（0x01）||交易金额[4](0x00000001)||终端机编号[6]
	memcpy(snddat, "\x80\x50\x01\x02\x0B", 5); i = 5;//11 INITIALIZE FOR PURCHASE命令
	snddat[i++] = 0x01;
	s_sum1 = get_s_sum1(0);//取得消费金额
	if((s_sum1 == 0) || (s_sum1 > 9900))
		return 22;
	value = s_sum1;
	over_turn(4, (unsigned char*)&value);
	memcpy(snddat+i, (unsigned char*)&value, 4); i+=4;
	memcpy(snddat+i, G24PsamNo, 6); i += 6;
//	snddat[i++] = 0x0f;
#ifdef _debug_
	debugstring("INITIALIZE FOR PURCHASE:");
	debugdata(snddat, i, 1);
#endif
	ret = XMSH_sndCmd(snddat, i, revbuff, 0xA0);
	if(ret == 0){
		return 4;
	}
#ifdef _debug_
	debugdata((unsigned char*)&ret,4,1);
	debugdata(revbuff, ret, 1);
#endif
	if(ret == 0)
		return ST_ERROR;
	else{
		if(memcmp(revbuff+ret-2, "\x94\x01", 2)==0){
			return 3;
		}
		else if(memcmp(revbuff+ret-2, "\x90\x00", 2)!=0)
			return 5;
	}
	
	memcpy((unsigned char*)&stuInitPurchaseRet, revbuff, sizeof(stuInitPurchaseRet));
#ifdef _debug_
	debugdata((unsigned char*)&stuInitPurchaseRet, sizeof(stuInitPurchaseRet), 1);
	debugstring("MAC1计算指令:");
#endif
	memcpy(stuSamInitPurchase.cRand, stuInitPurchaseRet.cRand, 4);
	memcpy(stuSamInitPurchase.cSnq, stuInitPurchaseRet.cSnq, 2);
	memcpy(stuSamInitPurchase.cMoney, (unsigned char*)&value, 4);
	
	memcpy((unsigned char*)&a_sum1, stuInitPurchaseRet.cBalance, 4);//余额
	over_turn(4, (unsigned char*)&a_sum1);
	if (a_sum1 >= 500000) 
	{
		return 21;
	}
	
	gucSoudnorm = 0;
#ifdef BUS_JIUJIANG_ //当钱包金额小于3元时	请充值
	if((a_sum1>=s_sum1)&&(a_sum1<300)){
		gucSoudnorm = 1;
	}
#else
	if((a_sum1>=s_sum1)&&(a_sum1<500)){
		gucSoudnorm = 1;
	}
#endif

	stuSamInitPurchase.cType = 0x06;
	
	memcpy(stuSamInitPurchase.cDateTime, (unsigned char*)&SysTime, 7);

	stuSamInitPurchase.cKeyVer = 0x01;//stuInitPurchaseRet.cKeyVer;//0x05

	stuSamInitPurchase.cKeyEn = stuInitPurchaseRet.cKeyEn;

	memcpy(stuSamInitPurchase.cKeyFactor, stuPubInfZaoZuang.cPublisherSign, 8);//有28个数据
//	memcpy(stuSamInitPurchase.cPublisherSign, stuPubInfZaoZuang.cCitycode, 2);//城市代码
//	memcpy(stuSamInitPurchase.cPublisherSign+2,"\xFF\x00\x00\x00\x00\x00", 6);//有36个数据
//	memcpy(stuSamInitPurchase.cPublisherSign, stuPubInfZaoZuang.cPublisherSign, 8);//这个...
#ifdef _debug_
	debugdata((unsigned char*)&stuSamInitPurchase,(28), 1);//+12
#endif

	//52B58B730005000000010620110402154217 05000000000090002030
	//4ACF30AF 0005 00000001 06 20110402154422 01000000000090002030 
	memset(revbuff, 0xff, 50);
	memset((unsigned char*)&stuDebitPurchase, 0, sizeof(stuDebitPurchase));
#ifdef NO_CPU_PSAM_
	if(getMiKey() != ST_OK)	//验证卡的认证码
		return 8;
//----------计算KEY----------
#ifdef BUS_PRO_JSB  //建设部的ISAM卡算不出KEYa,所以用默认的。
	memcpy(snddat, cardMessage.CardCheckCode, 4);//数据源：MAC+卡流水+卡流水前2字节+6个0
	memcpy(snddat+4, cardMessage.PublishBicker, 4);
	memcpy(snddat+8, cardMessage.PublishBicker, 2);
	memset(snddat+10, 0, 6);
	getJiuJiang_CPUKey(snddat, CPUPKEY);
#else
	memcpy(CPUPKEY, cardMessage.MoneyKEYA, 6);
	memcpy(CPUPKEY+6, cardMessage.RecodeKEYA, 6);
	memcpy(CPUPKEY+12, cardMessage.MonthKEYA, 4);
#endif
//----------计算KEY----------
	memcpy(CPUpInData1, stuInitPurchaseRet.cRand, 4);//随机数
	memcpy(CPUpInData1+4, stuInitPurchaseRet.cSnq, 2);
	memcpy(CPUpInData1+6, "\x00\x25", 2);
	memset(CPUinitdat, 0, 8);
	
	memcpy(snddat, (unsigned char*)&s_sum1, 4);
	over_turn(4, snddat);
	snddat[4] = 0x06;
	memcpy(snddat+5, G24PsamNo, 6);
	memcpy(snddat+11, (unsigned char*)&SysTime, 7);//共18
#ifdef _debug_
	debugstring("Mac --:");
	debugdata(CPUPKEY, 16, 1);
#endif
	Mac(CPUPKEY, CPUpInData1, snddat, 18, CPUinitdat, revbuff);//1BE5F163F50843699E120D74FAF08E76 消费
	memmove(revbuff+4, revbuff, 4);
	memcpy(revbuff, "\x00\x00\x00\x25", 4);
#else	//#ifdef NO_CPU_PSAM_ 上面计算完成

	if(CpuCardSetMac(PSAM_YD, revbuff, (unsigned char*)&stuSamInitPurchase, 28) == ST_ERROR)
	{
		//		DeactiveCard(PSAM_YD);
		return 6;
	}
#endif	//#ifdef NO_CPU_PSAM_
#ifdef _debug_
	debugstring("MAC OK!:");
	debugdata(revbuff, 8, 1);
#endif
	memcpy(stuDebitPurchase.cTermSnq, revbuff, 4);
	memcpy(stuDebitPurchase.cMac, revbuff+4, 4);
	memcpy(stuDebitPurchase.cDateTime, (unsigned char*)&SysTime, 7);
	memcpy(g24PSAMDsno, revbuff, 4);//--4字节的终端脱机交易序号  和 MAC1
	
	memcpy(snddat, "\x80\x54\x01\x00\x0F", 5); i = 5;
	memcpy(snddat+i, (unsigned char*)&stuDebitPurchase, 0x0f); i+=0x0f;
	memset(revbuff, 0xff, 50);
#ifdef _debug_
	debugstring("chek Mac1:");
	debugdata(snddat, i, 1);
#endif
	
	ret = XMSH_sndCmd(snddat, i, revbuff, 0xA0);
	if(ret == 0){
		audio(Audio_TRY_AGAIN);		//请重刷
		cardSound = 0xaa;//出错，需要重刷卡
		card_ser = c_serial;
		return 7;
	}
	
#ifdef _debug_
	debugstring("chek Mac1 OK:");
	debugdata((unsigned char*)&ret, 4, 1);
	debugdata(revbuff, ret ,1);
#endif
	if(memcmp(revbuff+ret-2, "\x93\x02", 2)==0)
		return 8;
	if(memcmp(revbuff+ret-2, "\x90\x00", 2)!=0){
//		debugdata(revbuf, 20, 1);
		cardSound = 0xaa;//出错，需要重刷卡
		card_ser = c_serial;
		return 7;
	}//其它错误
	
	memcpy(TACC, revbuff+ret-10, 4);//返回的TAC码 SPI -13
	memcpy(snddat, revbuff+ret-6, 4);
#ifdef _debug_
//	debugdata((unsigned char*)&pmoney, 4, 1);
	debugdata(snddat, 4, 1);
	debugstring("消费成功:\r\n");
#endif

#ifdef _debug_
	debugstring("TAC:");
	debugdata(TACC, 4, 1);
	debugstring("MAC2:");
	debugdata(snddat, 4, 1);
	debugdata((unsigned char*)&a_sum1, 4, 1);
#endif

CPUMAC2deal0:
#ifndef NO_CPU_PSAM_
	CPUMAC2(PSAM_YD, revbuff, snddat, 4);
#endif
	
CPUMAC2deal:
#ifdef _debug_
	debugstring("计算MAC2:");
	debugdata(revbuff, 10, 1);
#endif
	a_sum1 -= s_sum1;
	card_ser = 0;
	cardSound = 0;
	money_msg(ID_REC_TOLL, a_sum1, s_sum1, 2);

	Sound_cityCMD(0);

#ifdef _debug_
	debugstring("-------\r\n");
	debugstring("消费成功:\r\n");
	sprintf((char*)revbuff, "余额:%d.%02d元\r\n", (a_sum1/100), (a_sum1%100));
	debugstring((char*)revbuff);
	sprintf((char*)revbuff, "扣款:%d.%02d元\r\n", (s_sum1/100), (s_sum1%100));
	debugstring((char*)revbuff);
	debugstring("-------\r\n");
#endif
	
	Save_delayCardinfo(0);
	memset(revbuff, 0, 128);
	BuildRecorde(ID_REC_TOLL, revbuff);

	WriteRecord(revbuff, 0);
	return ST_OK;//消费成功
}

unsigned char XMSH_MONEY_CPU(void)
{
	unsigned char pos;
	char disbuf[32];

	pos = XMSH_UIMcpuPuchace();//电子钱包
	switch(pos){
	case ST_ERROR:
		restore_flag = 3;
		return 0xF1;
	case CARD_BLACK_PRO:
		cls();
		display(0,0,"警告：黑名单卡!",0);
		display(2,4,"请投币!!",0);
		SoundMessage(INVALID_CARD);
		restore_flag=3;
		break;
	case 21:
	case RET_MONEY_SN_EROR:																//两个钱包都错误
		error(ERR_MONEY_SN, 0);
		break;
	case 22:
		display(0,0,"提示:", DIS_Cls);
		display(2,0,"票价信息不正确",DIS_CENTER);
		restore_flag = 3;
		break;
	case CARD_forbid:
		MessageBox(1, "此卡禁止刷卡!");
		led_on(LED_RED);
		restore_flag=3;
		break;
	case CARD_NO_TIME:
		cls();
		display(0, 0, disbuf, 0);
		display(3, 0, "卡过期或没启用", DIS_CENTER);
		break;
	case 3:
		cls();
		display(0, 0, disbuf, 0);
		display(3, 0, "余额不足", DIS_CENTER);
		
		SoundMessage(SOUND_FLING_MONEY);
		break;
	case 4:
		cls();
		display(0, 0, disbuf, 0);
		display(3, 0, "扣次不能连刷", DIS_CENTER);
		SoundMessage(SOUND_FLING_MONEY);
		break;
	case 6:
		cls();
		display(0, 0, disbuf, 0);
		display(3, 0, "MAC1计算错误", DIS_CENTER);
		break;
	case 9:
	case 8:
		cls();
		display(0, 0, disbuf, 0);
		display(3, 0, "非法卡", DIS_CENTER);
		SoundMessage(INVALID_CARD);
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
	XMSH_end_card();
	g24GDisFlash = 3;
	restore_flag = 0;
	return 0xF1;
}
extern void face_time(void);

int Profile_XMSH24G(unsigned char *scardID)
{
	unsigned char ret;
	unsigned char snddd[64];

	if(gZKXLdly == 0){
		comParaInit();
		if(XMSHRFstyle == 0){//打开RF
			// 			memcpy(snddd, "\x80\x1d\x90\xB0\x01\x00\x18\x00\x00\x00"\
			// 				"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08"\
			// 				"\x00\x00\x03\x00\x00\x01\x00\x00\x00\x00\x00", 31);
			// 			com_snd(G24Com, 31, snddd);//PBOC
			memcpy(snddd, "\x80\x05\x90\xB0\x01\x00\x00", 7);//Mifare
			com_snd(G24Com, 7, snddd);//
			XMSHRFstyle = 1;
		}
		else{
			//			memcpy(snddd, "\x80\x05\x40\x0A\x00\x00\x00", 7);//Request 寻卡 没用啊。。。Logic Mifare应用时，请使用Request来寻卡，断开RF请使用Halt指令。
			//			memcpy(snddd, "\x80\x05\x90\xB0\x04\x00\x00", 7);//查询卡是否询到
			memcpy(snddd, "\x80\x05\x90\xB0\x04\x26\x00", 7);//0x26：寻卡模式要求操作的卡在读写完成后要离开感应区才能再读写
			//			memcpy(snddd, "\x80\x05\x90\xB0\x05\x00\x00", 7);//读卡ID。不查询，直接读[查询到连接成功后，可通过指令取卡ID值]
			com_snd(G24Com, 7, snddd);
		}
#ifdef _debug_
		debugdata(snddd, 7, 1);
#endif
		gZKXLdly = 1;
	}
	else{
		if(flag == 0x5A){
#ifdef _debug_
			debugstring("ROK:");
			debugdata(gcomBuf, gComindex, 1);//5A10000902271720510318020043CA
#endif
			ret = XMSH_cmdDeal(scardID);
			return ret;
		}
		else{
			if(++gZKXLdly > 20){
				gZKXLdly = 0;
			}
		}
	}
	return -1;
}

void XMSH_end_card(void)
{
	unsigned char csrcard[10];
	int i;
	int ret;
	
	i = 0;
	XMSFBreaKCard();
	while(1){
		delayxms(100);
		ret = Profile_XMSH24G(csrcard);
		if(ret == 1){// 有卡
// 			debugstring("end card csrcard:");
// 			debugdata(csrcard, 10, 1);
// 			debugdata(gG24CardNo, 10, 1);

			XMSFBreaKCard();
			comParaInit();
			if(memcmp(csrcard, gG24CardNo, 8) != 0)
				return;
			i = 0;
		}
		else if(ret >= 0){//收到读头数据
//			debugstring("end card csrcard:无卡\r\n");

			comParaInit();
			if((ret > 0)||(XMSHRFstyle > 1)){
				XMSFBreaKCard();
			}
			if(i++ > 2)
				return;
		}
	}
}
void XMSH_Main(void)
{
	unsigned char csrcard[10];
//	unsigned char pos;
	int ret;
	
	if(F_FLAG_VARY.stop_flag == 1){
		return;
	}
	
	ret = Profile_XMSH24G(csrcard);
	if(ret == 1){// 有卡
#ifdef _debug_
		debugstring("csrcard:");
		debugdata(csrcard, 8, 1);
#endif
		memcpy(gG24CardNo, csrcard, 10);
		XMSH_MONEY_CPU();//电子钱包
		//pos = XMSH_UIMm1Puchace();//手机卡内的M1区消费
		XMSFBreaKCard();
		gComindex = 0;
		flag = 0;
	}
	else if(ret >= 0){//收到读头数据
		gComindex = 0;
		flag = 0;
		if((ret > 0)||(XMSHRFstyle > 1)){ //如果有连接成功的卡。处理过后需要断开。因为卡一旦连接上，有效距离会有1米，所以一定要断开.
			XMSFBreaKCard();
		}
	}
}
/*
void XMSH_Main(void)
{
	unsigned char ret, pos;
	unsigned char snddd[64];
	
	if(F_FLAG_VARY.stop_flag == 1){
		return;
	}

	if(gZKXLdly == 0){
		comParaInit();
		if(XMSHRFstyle == 0){//打开RF
// 			memcpy(snddd, "\x80\x1d\x90\xB0\x01\x00\x18\x00\x00\x00"\
// 				"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08"\
// 				"\x00\x00\x03\x00\x00\x01\x00\x00\x00\x00\x00", 31);
// 			com_snd(G24Com, 31, snddd);//PBOC
			memcpy(snddd, "\x80\x05\x90\xB0\x01\x00\x00", 7);//Mifare
			com_snd(G24Com, 7, snddd);//
			XMSHRFstyle = 1;
		}
		else{
//			memcpy(snddd, "\x80\x05\x40\x0A\x00\x00\x00", 7);//Request 寻卡 没用啊。。。Logic Mifare应用时，请使用Request来寻卡，断开RF请使用Halt指令。
//			memcpy(snddd, "\x80\x05\x90\xB0\x04\x00\x00", 7);//查询卡是否询到
			memcpy(snddd, "\x80\x05\x90\xB0\x04\x26\x00", 7);//0x26：寻卡模式要求操作的卡在读写完成后要离开感应区才能再读写
//			memcpy(snddd, "\x80\x05\x90\xB0\x05\x00\x00", 7);//读卡ID。不查询，直接读[查询到连接成功后，可通过指令取卡ID值]
			com_snd(G24Com, 7, snddd);
		}
// #ifdef _debug_
// 		debugdata(snddd, 7, 1);
// #endif
		gZKXLdly = 1;
	}
	else{
		if(flag == 0x5A){
// #ifdef _debug_
// 			debugstring("ROK:");
// 			debugdata(gcomBuf, gComindex, 1);//5A10000902271720510318020043CA
// #endif
			ret = XMSH_cmdDeal();
			if(ret == 1){
				//CPUPuchace34();
				pos = XMSH_MONEY_CPU();//电子钱包
				//pos = XMSH_UIMm1Puchace();//手机卡内的M1区消费
				XMSFBreaKCard();
				if(pos == 0xF1){
					delayxs(1);
					g24GDisFlash = 3;
					restore_flag = 0;
				}
			}
			if((ret != 0)||(XMSHRFstyle > 1)){ //如果有连接成功的卡。处理过后需要断开。因为卡一旦连接上，有效距离会有1米，所以一定要断开.
				XMSFBreaKCard();
			}
			gComindex = 0;
			flag = 0;
		}
		else{
			if(++gZKXLdly > 20){
				gZKXLdly = 0;
			}
		}
	}
}
*/
#endif

