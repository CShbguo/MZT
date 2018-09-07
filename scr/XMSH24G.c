
/*
	����ʢ������2.4G��ͷ���ֻ�����ʹ��M1�����ܣ�ֻ������������
	1����������2��Ǯ������3��������Ϣ��

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

extern unsigned char restore_flag;	//��ʾ��ԭ
extern unsigned int g24GDisFlash;	//24G������ˢ��
extern Parameter8 CARD_PUBLIC;
extern stcomtime SysTime;
extern unsigned int a_sum1,s_sum1;
extern Parameter5 cardMessage;
extern unsigned char gMCardCand;
extern pFistVary_1 pFistVary;
extern unsigned int c_serial;
extern unsigned char cardSound;			//ˢ��û�гɹ���־
extern unsigned int card_ser;
extern unsigned int startTime,endTime,nowTime,endTimeEn;
extern unsigned char gucEndTimeFlag;
extern Parameter3 ComSumeFile;
extern unsigned char gucSoudnorm;
extern stPricetable NewPriceTab;
extern unsigned int a_sumR;//�ֻ�Ǯ���е���ʵ���

unsigned char gcomBuf[256];
volatile unsigned short gComindex;
volatile unsigned char flag;
unsigned int gZKXLdly;
unsigned char XMSHRFstyle;
unsigned char gG24CardNo[10];//���ţ�Ǯ������

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
	if(flag == 0x5A){//�ϸ����ݰ�û������
		comParaInit();
		return;
	}

	gcomBuf[gComindex++] = vl;
	if(gComindex > 2){//�Ѿ������˳��ȣ������ж��Ƿ�������������
		if(gComindex >= (gcomBuf[1]+2)){//�����
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
//�������Ҫ��Ӧ��
unsigned short XMSH_sndCmd(unsigned char *dat, unsigned short len, unsigned char *rev, unsigned char cmd)
{
	unsigned short us;
	unsigned int dlyo=0;
	
	XMSH_sndcarddat(cmd, dat, len);//��������
	
	while(1){//��Ӧ��
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
	if(XMSHRFstyle == 1){//���ֻ�������
		if(memcmp(gcomBuf+2, "\x9C\x02", 2) == 0){//�ҵ�����
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
	else if(XMSHRFstyle == 2){//���ڿ�������,�������ǲ��ᵽ����ġ�
#ifdef _debug_
		debugstring("XMSHRFstyle == 2:");
		debugdata(gcomBuf, gComindex, 1);
#endif
		return 2;//�п���
	}
	return 0;
}
/*
unsigned char XMSH_cmdDeal(void)
{
	if(XMSHRFstyle == 1){//���ֻ�������
		if(memcmp(gcomBuf+2, "\x9C\x02", 2) == 0){//�ҵ�����
			XMSHRFstyle = 2;
			return 1;
		}
		else if(memcmp(gcomBuf+2, "\x9C\x03", 4) == 0){
			return 0;
		}
		else
			return 0;
	}
	else if(XMSHRFstyle == 2){//���ڿ�������,�������ǲ��ᵽ����ġ�
#ifdef _debug_
		debugstring("XMSHRFstyle == 2:");
		debugdata(gcomBuf, gComindex, 1);
#endif
		return 2;//�п���
	}
	return 0;
}
*/
void XMSFBreaKCard(void)//4�Ͽ�rf: 80 05 90 B0 00 00 00
{
	unsigned char buffer[32];
	memcpy(buffer, "\x80\x05\x90\xB0\x00\x00\x00", 7);//�ر�RF
//	memcpy(buffer, "\x80\x05\x40\x04\x00\x00\x00", 7);//�Ͽ�����
// 	debugstring("�Ͽ�rf:");
// 	debugdata(buffer, 7, 1);
	XMSHRFstyle = 0;
	com_snd(G24Com, 7, buffer);
}
//
unsigned char XMSFSelectFE(void){
	
	unsigned char snddat[256];
	unsigned char revbuff[256];
	unsigned char ret;

	memcpy(snddat, "\xA0\xA4\x04\x00\x10\x41\x63\x63\x6F\x75\x6E\x74\x65\x72\x20\x41\x70\x70\x00\x00\x00", 21);//ѡ��AccounterӦ��
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
		delayxms(50);//80 0B 40 08 01�������ţ� 00��00:B��Կ, 01:A��Կ�� 06  FF FF FF FF FF FF(�������Կֵ)
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
	if(ss >= 5){//ѯ��ʧ��
		return 3;
	}
	return 0;
}
//(00:B��Կ, 01:A��Կ��
unsigned char XMSFAuth(unsigned char Sec, unsigned char mode, unsigned char *cKey)
{
	unsigned char snddat[256];
	unsigned char revbuff[256];
	unsigned short ret;

	//���ͣ�80 0B 40 08 01�������ţ� 00��00:B��Կ, 01:A��Կ�� 06  FF FF FF FF FF FF(�������Կֵ)
	memcpy(snddat, "\x40\x08\x01\x00\x06\xFF\xFF\xFF\xFF\xFF\xFF", 0x0B);
	snddat[2] = Sec;
	snddat[3] = mode;//(00:B��Կ, 01:A��Կ��
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
	
	//���鷢�ͣ�80 05 40 02 01 00(���) 00
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
	
	//д��80 15 40 02 02 00(���)10 00 00 00 00 FF FF FF FF 00 00 00 00 00 FF 00 FF(16���ֽڵĿ�����)
	memcpy(snddat, "\x40\x02\x02\x01\x10\x00\x00\x00\x00\xFF\xFF\xFF\xFF\x00\x00\x00\x00\x01\xFE\x01\xFE", 21);
	snddat[3] = block;
	memcpy(snddat+5, rdat, 16);//����д�ֽ�
	ret = XMSH_sndCmd(snddat, 21, revbuff, 0x80);
	if(ret == 0){
		return 1;
	}
	if((revbuff[ret-2] != 0x90)||(revbuff[ret-1] != 0x00)){
		return 2;
	}
	return 0;
}

//mode == 0����������
unsigned char XMSFIDvalue(unsigned char block, unsigned char mode, unsigned int value)
{
	unsigned char snddat[256];
	unsigned char revbuff[256];
	unsigned short ret;
	//Inc ��ֵ 80 15 40 02 03 00(���) 10 64 00 00 00(���)+12�ֽ���������[��ʱ���룬BCD��ʽ+���ߺ�+88+01+XXXX]
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
	//���� 80 0C 40 02 06 00(���) 07 30 58 10 XX 01 09 00  ��������ʱ��Ϊ2008-9-1 10:58:30��
	//�룬�֣�ʱ������(δ��)���գ��£���(��08)
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

//���ѳ���
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
	memcpy((unsigned char*)&c_serial, buffer, 4);//������
	memcpy(cNo, buffer, 4);
	cNo[0] ^= 0x71;
	cNo[1] ^= 0x30;
	cNo[2] ^= 0x4B;
	cNo[3] ^= 0x19;
	memcpy(keya, cNo, 4);//1����Ĭ����Կ
	memcpy(keya+4, cNo, 2);//1����Ĭ����Կ
	
	if(Delay_Normal())
		return ST_ERROR;

	for(pos=0; pos<5; pos++){//д1����Ϊ������
		if(XMSFAuth(1, 1, keya) == 0){//��֤�ɹ�
			break;
		}
	}
	if(pos >= 5)
		return 4;// ��֤ʧ��
	
	if(XMSFRead(0, buffer) != 0)
		return 5;
#ifdef _debug_
	debugstring("SMSH 1������Ϣ0��\r\n");
	debugdata(buffer,16,1);
#endif	
	if(MiBlockInvalid(1,15,buffer)==ST_ERROR) 
	{
		return 6;
	}
	gMCardCand = CARDSTYLE_24XMSH;
	pos=0;
	memcpy(cardMessage.CityCode, buffer+pos, CITY_CODE_LEN);			//���д���	2
	pos+=CITY_CODE_LEN;
	memcpy(cardMessage.TradeCode, buffer+pos, TRADE_CODE_LEN);			//��ҵ����	2
	pos+=TRADE_CODE_LEN;	
	memcpy((unsigned char *)cardMessage.PublishBicker, buffer+pos, PUBLISH_BICKER_LEN);	//����ˮ��	4	
	memcpy(cardMessage.CardCheckCode, buffer+8, CARD_CHECH_CODE_LEN);	//mac��		4
	cardMessage.card_catalog=buffer[13];								//����		1
	cardMessage.Zicard_catalog = buffer[14];	//�ӿ��࣬��Ʊ����
	
#ifdef _debug_
	debugstring("card_catalog:");
	debugdata((unsigned char*)&cardMessage.card_catalog,1,1);
#endif	
	switch(buffer[12])//������־
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

	if(XMSFRead(1, buffer) != 0)////�������� ��Ч���� �������� ��Ѻ��	����	У��
		return 7;
#ifdef _debug_
	debugstring("1������Ϣ1:");
	debugdata(buffer, 16, 1);
#endif
	
	if(MiBlockInvalid(1,15,buffer)==ST_ERROR) 
	{
#ifdef _debug_
		debugstring("1������Ϣ1 У���\r\n");
#endif
		return 8;
	}
	//ʱ�仺����0
	endTime = 0;
	startTime = 0;
	nowTime = 0;
	endTimeEn = 0;
	
	memcpy(buffer+50, buffer+4, 4);
	buffer[52] = bcdADD(buffer[52], 1);//��Ч���ڿ�ʼ��ʾ����������,��ʱһ���¡�
	if(buffer[52]>0x12){
		buffer[52] = 1;
		buffer[51] = bcdADD(buffer[51], 1);
	}
	
	memcpy((unsigned char*)&endTime, buffer+50, 4);	//��Ч����,�����ġ�
	memcpy((unsigned char*)&endTimeEn, buffer+4, 4);//��Ч���ڣ�����ǰ��
	memcpy((unsigned char*)&startTime, buffer+8, 4);//��������
	
	if(GetDateTime()==ST_ERROR) 
	{
		error(ERR_READ_TIME,0);
		return 0xF1;
	}	
#ifdef _debug_
	debugstring("systime:");
	debugdata((unsigned char*)&SysTime, 7, 1);
#endif
	memcpy((unsigned char*)&nowTime, (unsigned char *)&SysTime.year_h, 4);//��������
	
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
		MessageBox(1, "�˿�δ����!");
		return 0xF1;
	}
	gucEndTimeFlag = 0;
	if((startTime>nowTime)||(endTime<nowTime)||(startTime>endTime)){//���ڿ�
		display(0,0,"���棺",0);
		display(2,1,"�����ڻ�δ����",0);
		audio(Audio_FLING_MONEY);	//��Ͷ��
		return 0xF1;
	}
	else if(endTimeEn<=nowTime){//��Ҫ��ʾ���������ˡ�
		gucEndTimeFlag = 1;
	}
	
	if(getMiKey()==ST_ERROR)
	{
		return 12;
	}

	if(IsInBlackList(cardMessage.PublishBicker) < FLASH_BLK_END){
		display(0,0,"���棺",0);
		display(2,1,"��������!",DIS_CENTER);
		audio(Audio_INVALID);	//��Ͷ��
		return 0xF1;
	}
	
	memcpy(keya, cardMessage.MoneyKEYA, 6);
	for(pos=0; pos<5; pos++){//3����Ϊ������Ϣ��
		if(XMSFAuth(3, 1, keya) == 0){//��֤�ɹ�
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
		debugstring("��֤3����ʧ��!\r\n");
		return 4;// ��֤ʧ��
	}
#ifdef _debug_
	debugstring("3����KEYA:");
	debugdata(keya,6,1);
#endif
	
	if(XMSFRead(0, buffer) != 0)//������Ϣ����
		return 5;
#ifdef _debug_
	debugstring("������Ϣ����:");
	debugdata(buffer,16,1);
#endif
	memcpy((unsigned char*)&CARD_PUBLIC, buffer, 8);
	if(CARD_PUBLIC.BlackFlag == 0x04){
		display(0,0,"���棺",0);
		display(2,1,"��������!",DIS_CENTER);
		audio(Audio_INVALID);	//��Ͷ��
		return 0xF1;
	}
	s_sum1 = get_s_sum1(0);
	
	for(pos=0; pos<5; pos++){//2����ΪǮ����
		if(XMSFAuth(2, 1, cardMessage.MoneyKEYA) == 0){//��֤�ɹ�
			break;
		}
	}
	if(pos >= 5)
		return 4;// ��֤ʧ��
	
	if(XMSFRead(1, buffer) != 0)//Ǯ��������Ǯ������û�ã���Ҫʵ�ּӼ�ֵ��һ�������Ǯ����ʽ�����Ա��ݿ鲻�ܼӼ�ֵ��
		return 5;
#ifdef _debug_
	debugstring("Ǯ��:");
	debugdata(buffer,16,1);
#endif
	if(MifIsPurse(buffer)==ST_ERROR) 
	{
		display(0,0,"���棺",0);
		display(2,1,"Ǯ����ʽ����!",DIS_CENTER);
		return 0xF1;
	}
	memcpy((unsigned char*)&a_sum1, buffer, 4);//
	if(a_sum1 < s_sum1){
		display(0,0,"���棺",0);
		display(2,1,"Ǯ������!",0);
		audio(Audio_FLING_MONEY);	//��Ͷ��
		return 0xF1;
	}
	if(XMSFIDvalue(1, 0, s_sum1) != 0){//��ֵ����
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
	sprintf((char *)sndbuf, "���:%d.%02d", temp/100, temp%100);
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
// 	debugstring("3F00ѡPSE:");
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
	debugstring("3F01ѡPSE:");
	debugdata(revbuff, ret, 1);
#endif
	if(memcmp(revbuff+ret-2, "\x90\x00", 2)!=0)
		return ST_ERROR;
	
	memcpy(snddat,"\x00\xB0\x95\x00\x1E",6);//��Ӧ�����к���Ϊ��ɢ���� //��15�ļ�
	ret = XMSH_sndCmd(snddat, 6, revbuff, 0xA0);
	if(ret == 0){
		return ST_ERROR;
	}
#ifdef _debug_
	debugstring("��95�ļ�:");
	debugdata(revbuff, ret, 1);
#endif	//A00007555D2A785203010003D2283301000000002013032720200101A9119000
	if(memcmp(revbuff+ret-2, "\x90\x00", 2)!=0)
		return ST_ERROR;
	
	cardMessage.card_catalog = revbuff[10];//����
	if(cardMessage.card_catalog == 0x50)//��û���۳��Ŀ�
		return 8;
	if(cardMessage.card_catalog < 32)
		cardMessage.card_catalog += CARD_NORMAL;
	
	memcpy((unsigned char*)&stuPubInfZaoZuang, revbuff+12, 8);//�����к�
	memcpy(stuPubInfZaoZuang.cApp_sn, revbuff+11, 10);	//��Ӧ�����кţ������ģ�����
	memcpy(cardMessage.CityCode, revbuff+2, 2);//���д���
	memcpy(cardMessage.CardCheckCode, revbuff+4, 4);//MAC��
	memcpy(cardMessage.TradeCode, revbuff+8, 2);//��ҵ����

	memcpy(cardMessage.PublishBicker, revbuff+12, 4);//������ˮ��
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
	
	memcpy((unsigned char*)&itemp, (unsigned char *)&SysTime, 4);//��ǰ����
	over_turn(4, (unsigned char*)&itemp);
	memcpy((unsigned char*)&value, revbuff+20, 4);//��������
	over_turn(4, (unsigned char*)&value);
	memcpy((unsigned char*)&pmoney, revbuff+24, 4);//��Ч����
	over_turn(4, (unsigned char*)&pmoney);
#ifdef _debug_
	debugdata(cardMessage.PublishBicker, 4, 1);
	debugdata((unsigned char*)&value, 4, 1);
	debugdata((unsigned char*)&pmoney, 4, 1);
#endif
	if(IsInBlackList(cardMessage.PublishBicker) < FLASH_BLK_END){
		return CARD_BLACK_PRO;
	}
	memcpy((unsigned char*)&c_serial, cardMessage.PublishBicker, 4);//������
//---------------------------------------------------------
//	���ѹ��̿�ʼ	
	cardMessage.card_catalog = CARD_NORMAL_BUS;
	if(cardMessage.card_catalog >= 0x40)
		i = cardMessage.card_catalog - 0x40;
	else
		i = cardMessage.card_catalog;
	
	if((NewPriceTab.rate[i] == 104)||(NewPriceTab.rate[i] == 204)){
		return CARD_forbid;//�˿�����ֹʹ��
	}

// 	if(Delay_Normal()){
// 		return ST_ERROR;
// 	}
// 	if(month_decide()) // �ڶ���ȥ��Ǯ��(����������ˢ)
// 	{
// 		disp_no_swipe();
// 		return ST_ERROR;
// 	}

// 	if(month_decide()) // �ڶ���ȥ��Ǯ��(����������ˢ)   ��Ҫ����û�ж�������==
// 	{
// 		disp_no_swipe();
// 		return ST_OK;	
// 	}

	if(cardSound == 0xaa){//��Ҫ��ˢ
		if(card_ser!=c_serial){//����ͬһ�ſ������ܴ���
			return ST_ERROR;
		}
//		debugstring("Raa1:");
//		debugdata((unsigned char*)&card_ser, 4, 1);
//		debugdata((unsigned char*)&c_serial, 4, 1);
// 		
// #ifdef _debug_CPU_
// 		debugstring("��Ҫ��ˢ ZBGGQB:��ʼ\r\n");
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
// 		debugstring("��Ҫ��ˢ ZBGGQB:");
// 		debugdata(revbuf, ret, 1);
// #endif
		
		XMSH_CPU_Read_Money((unsigned char*)&value, 3);
		over_turn(4, (unsigned char*)&value);
#ifdef _debug_
		debugstring("aa���:");
		debugdata((unsigned char*)&value, 4, 1);
		debugdata((unsigned char*)&a_sumR, 4, 1);
#endif
		if(value < a_sumR){//�ϴ��Ѿ��۳ɹ�
			memcpy(snddat, "\x80\x5A\x00\x06\x02", 5);//memcpy(snddat, "\x80\x5A\x00\x05\x02", 5);
			memcpy((unsigned char*)&ii, stuInitPurchaseRet.cSnq, 2);
			over_turn(2, (unsigned char*)&ii);
			ii += 1;
			over_turn(2, (unsigned char*)&ii);
			memcpy(snddat+5, (unsigned char*)&ii, 2);
			snddat[8] = 8;
#ifdef _debug_
			debugstring("����:");
			debugdata(stuInitPurchaseRet.cSnq, 2, 1);
			debugdata(snddat, 9, 1);
#endif
			ret = XMSH_sndCmd(snddat, 9, revbuff, 0xA0);
			if(ret == 0)
				return ST_ERROR;
#ifdef _debug_
			debugdata(revbuff, ret, 1);//02 00 0C 00 00 ED F2 97 9B 6E 94 37 BC 90 00 F2 03
#endif
			if(ret == 0)//���س�ʱ,����
				return ST_ERROR;
			if((memcmp(revbuff+ret-2, "\x90\x00", 2)!=0) &&\
				(memcmp(revbuff+ret-2, "\x6C\x08", 2)!=0))
				goto CPUMAC2deal;//ȡ���ɹ�02 69 85 
			
			memcpy(TACC, revbuff+ret-6, 4);//���ص�TAC��
			memcpy(snddat, revbuff+ret-10, 4);//��MAC2��
			//			return ST_ERROR;
			goto CPUMAC2deal0;
		}
// 		memcpy(sndbuf, "\x02\x00\xa4\x00\x00\x02\xdf\x01", 8);
// #ifdef _debug_CPU_
// 		debugstring("ѡ���ļ�:");
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
	//����� 
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

//8050 0101 0B||��Կ��ʶ�� [1]��0x01��||���׽��[4](0x00000001)||�ն˻����[6]
	memcpy(snddat, "\x80\x50\x01\x02\x0B", 5); i = 5;//11 INITIALIZE FOR PURCHASE����
	snddat[i++] = 0x01;
	s_sum1 = get_s_sum1(0);//ȡ�����ѽ��
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
	debugstring("MAC1����ָ��:");
#endif
	memcpy(stuSamInitPurchase.cRand, stuInitPurchaseRet.cRand, 4);
	memcpy(stuSamInitPurchase.cSnq, stuInitPurchaseRet.cSnq, 2);
	memcpy(stuSamInitPurchase.cMoney, (unsigned char*)&value, 4);
	
	memcpy((unsigned char*)&a_sum1, stuInitPurchaseRet.cBalance, 4);//���
	over_turn(4, (unsigned char*)&a_sum1);
	if (a_sum1 >= 500000) 
	{
		return 21;
	}
	
	gucSoudnorm = 0;
#ifdef BUS_JIUJIANG_ //��Ǯ�����С��3Ԫʱ	���ֵ
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

	memcpy(stuSamInitPurchase.cKeyFactor, stuPubInfZaoZuang.cPublisherSign, 8);//��28������
//	memcpy(stuSamInitPurchase.cPublisherSign, stuPubInfZaoZuang.cCitycode, 2);//���д���
//	memcpy(stuSamInitPurchase.cPublisherSign+2,"\xFF\x00\x00\x00\x00\x00", 6);//��36������
//	memcpy(stuSamInitPurchase.cPublisherSign, stuPubInfZaoZuang.cPublisherSign, 8);//���...
#ifdef _debug_
	debugdata((unsigned char*)&stuSamInitPurchase,(28), 1);//+12
#endif

	//52B58B730005000000010620110402154217 05000000000090002030
	//4ACF30AF 0005 00000001 06 20110402154422 01000000000090002030 
	memset(revbuff, 0xff, 50);
	memset((unsigned char*)&stuDebitPurchase, 0, sizeof(stuDebitPurchase));
#ifdef NO_CPU_PSAM_
	if(getMiKey() != ST_OK)	//��֤������֤��
		return 8;
//----------����KEY----------
#ifdef BUS_PRO_JSB  //���貿��ISAM���㲻��KEYa,������Ĭ�ϵġ�
	memcpy(snddat, cardMessage.CardCheckCode, 4);//����Դ��MAC+����ˮ+����ˮǰ2�ֽ�+6��0
	memcpy(snddat+4, cardMessage.PublishBicker, 4);
	memcpy(snddat+8, cardMessage.PublishBicker, 2);
	memset(snddat+10, 0, 6);
	getJiuJiang_CPUKey(snddat, CPUPKEY);
#else
	memcpy(CPUPKEY, cardMessage.MoneyKEYA, 6);
	memcpy(CPUPKEY+6, cardMessage.RecodeKEYA, 6);
	memcpy(CPUPKEY+12, cardMessage.MonthKEYA, 4);
#endif
//----------����KEY----------
	memcpy(CPUpInData1, stuInitPurchaseRet.cRand, 4);//�����
	memcpy(CPUpInData1+4, stuInitPurchaseRet.cSnq, 2);
	memcpy(CPUpInData1+6, "\x00\x25", 2);
	memset(CPUinitdat, 0, 8);
	
	memcpy(snddat, (unsigned char*)&s_sum1, 4);
	over_turn(4, snddat);
	snddat[4] = 0x06;
	memcpy(snddat+5, G24PsamNo, 6);
	memcpy(snddat+11, (unsigned char*)&SysTime, 7);//��18
#ifdef _debug_
	debugstring("Mac --:");
	debugdata(CPUPKEY, 16, 1);
#endif
	Mac(CPUPKEY, CPUpInData1, snddat, 18, CPUinitdat, revbuff);//1BE5F163F50843699E120D74FAF08E76 ����
	memmove(revbuff+4, revbuff, 4);
	memcpy(revbuff, "\x00\x00\x00\x25", 4);
#else	//#ifdef NO_CPU_PSAM_ ����������

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
	memcpy(g24PSAMDsno, revbuff, 4);//--4�ֽڵ��ն��ѻ��������  �� MAC1
	
	memcpy(snddat, "\x80\x54\x01\x00\x0F", 5); i = 5;
	memcpy(snddat+i, (unsigned char*)&stuDebitPurchase, 0x0f); i+=0x0f;
	memset(revbuff, 0xff, 50);
#ifdef _debug_
	debugstring("chek Mac1:");
	debugdata(snddat, i, 1);
#endif
	
	ret = XMSH_sndCmd(snddat, i, revbuff, 0xA0);
	if(ret == 0){
		audio(Audio_TRY_AGAIN);		//����ˢ
		cardSound = 0xaa;//������Ҫ��ˢ��
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
		cardSound = 0xaa;//������Ҫ��ˢ��
		card_ser = c_serial;
		return 7;
	}//��������
	
	memcpy(TACC, revbuff+ret-10, 4);//���ص�TAC�� SPI -13
	memcpy(snddat, revbuff+ret-6, 4);
#ifdef _debug_
//	debugdata((unsigned char*)&pmoney, 4, 1);
	debugdata(snddat, 4, 1);
	debugstring("���ѳɹ�:\r\n");
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
	debugstring("����MAC2:");
	debugdata(revbuff, 10, 1);
#endif
	a_sum1 -= s_sum1;
	card_ser = 0;
	cardSound = 0;
	money_msg(ID_REC_TOLL, a_sum1, s_sum1, 2);

	Sound_cityCMD(0);

#ifdef _debug_
	debugstring("-------\r\n");
	debugstring("���ѳɹ�:\r\n");
	sprintf((char*)revbuff, "���:%d.%02dԪ\r\n", (a_sum1/100), (a_sum1%100));
	debugstring((char*)revbuff);
	sprintf((char*)revbuff, "�ۿ�:%d.%02dԪ\r\n", (s_sum1/100), (s_sum1%100));
	debugstring((char*)revbuff);
	debugstring("-------\r\n");
#endif
	
	Save_delayCardinfo(0);
	memset(revbuff, 0, 128);
	BuildRecorde(ID_REC_TOLL, revbuff);

	WriteRecord(revbuff, 0);
	return ST_OK;//���ѳɹ�
}

unsigned char XMSH_MONEY_CPU(void)
{
	unsigned char pos;
	char disbuf[32];

	pos = XMSH_UIMcpuPuchace();//����Ǯ��
	switch(pos){
	case ST_ERROR:
		restore_flag = 3;
		return 0xF1;
	case CARD_BLACK_PRO:
		cls();
		display(0,0,"���棺��������!",0);
		display(2,4,"��Ͷ��!!",0);
		SoundMessage(INVALID_CARD);
		restore_flag=3;
		break;
	case 21:
	case RET_MONEY_SN_EROR:																//����Ǯ��������
		error(ERR_MONEY_SN, 0);
		break;
	case 22:
		display(0,0,"��ʾ:", DIS_Cls);
		display(2,0,"Ʊ����Ϣ����ȷ",DIS_CENTER);
		restore_flag = 3;
		break;
	case CARD_forbid:
		MessageBox(1, "�˿���ֹˢ��!");
		led_on(LED_RED);
		restore_flag=3;
		break;
	case CARD_NO_TIME:
		cls();
		display(0, 0, disbuf, 0);
		display(3, 0, "�����ڻ�û����", DIS_CENTER);
		break;
	case 3:
		cls();
		display(0, 0, disbuf, 0);
		display(3, 0, "����", DIS_CENTER);
		
		SoundMessage(SOUND_FLING_MONEY);
		break;
	case 4:
		cls();
		display(0, 0, disbuf, 0);
		display(3, 0, "�۴β�����ˢ", DIS_CENTER);
		SoundMessage(SOUND_FLING_MONEY);
		break;
	case 6:
		cls();
		display(0, 0, disbuf, 0);
		display(3, 0, "MAC1�������", DIS_CENTER);
		break;
	case 9:
	case 8:
		cls();
		display(0, 0, disbuf, 0);
		display(3, 0, "�Ƿ���", DIS_CENTER);
		SoundMessage(INVALID_CARD);
		break;
	case ST_OK://�ɹ�
		restore_flag = 3;
		SetTcpPacketTTLTime(RECORD_TTL);
		break;
	default:
// 		cls();
// 		display(0, 0, disbuf, 0);
// 		display(3, 0, "ĩ�������", DIS_CENTER);
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
		if(XMSHRFstyle == 0){//��RF
			// 			memcpy(snddd, "\x80\x1d\x90\xB0\x01\x00\x18\x00\x00\x00"\
			// 				"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08"\
			// 				"\x00\x00\x03\x00\x00\x01\x00\x00\x00\x00\x00", 31);
			// 			com_snd(G24Com, 31, snddd);//PBOC
			memcpy(snddd, "\x80\x05\x90\xB0\x01\x00\x00", 7);//Mifare
			com_snd(G24Com, 7, snddd);//
			XMSHRFstyle = 1;
		}
		else{
			//			memcpy(snddd, "\x80\x05\x40\x0A\x00\x00\x00", 7);//Request Ѱ�� û�ð�������Logic MifareӦ��ʱ����ʹ��Request��Ѱ�����Ͽ�RF��ʹ��Haltָ�
			//			memcpy(snddd, "\x80\x05\x90\xB0\x04\x00\x00", 7);//��ѯ���Ƿ�ѯ��
			memcpy(snddd, "\x80\x05\x90\xB0\x04\x26\x00", 7);//0x26��Ѱ��ģʽҪ������Ŀ��ڶ�д��ɺ�Ҫ�뿪��Ӧ�������ٶ�д
			//			memcpy(snddd, "\x80\x05\x90\xB0\x05\x00\x00", 7);//����ID������ѯ��ֱ�Ӷ�[��ѯ�����ӳɹ��󣬿�ͨ��ָ��ȡ��IDֵ]
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
		if(ret == 1){// �п�
// 			debugstring("end card csrcard:");
// 			debugdata(csrcard, 10, 1);
// 			debugdata(gG24CardNo, 10, 1);

			XMSFBreaKCard();
			comParaInit();
			if(memcmp(csrcard, gG24CardNo, 8) != 0)
				return;
			i = 0;
		}
		else if(ret >= 0){//�յ���ͷ����
//			debugstring("end card csrcard:�޿�\r\n");

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
	if(ret == 1){// �п�
#ifdef _debug_
		debugstring("csrcard:");
		debugdata(csrcard, 8, 1);
#endif
		memcpy(gG24CardNo, csrcard, 10);
		XMSH_MONEY_CPU();//����Ǯ��
		//pos = XMSH_UIMm1Puchace();//�ֻ����ڵ�M1������
		XMSFBreaKCard();
		gComindex = 0;
		flag = 0;
	}
	else if(ret >= 0){//�յ���ͷ����
		gComindex = 0;
		flag = 0;
		if((ret > 0)||(XMSHRFstyle > 1)){ //��������ӳɹ��Ŀ������������Ҫ�Ͽ�����Ϊ��һ�������ϣ���Ч�������1�ף�����һ��Ҫ�Ͽ�.
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
		if(XMSHRFstyle == 0){//��RF
// 			memcpy(snddd, "\x80\x1d\x90\xB0\x01\x00\x18\x00\x00\x00"\
// 				"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08"\
// 				"\x00\x00\x03\x00\x00\x01\x00\x00\x00\x00\x00", 31);
// 			com_snd(G24Com, 31, snddd);//PBOC
			memcpy(snddd, "\x80\x05\x90\xB0\x01\x00\x00", 7);//Mifare
			com_snd(G24Com, 7, snddd);//
			XMSHRFstyle = 1;
		}
		else{
//			memcpy(snddd, "\x80\x05\x40\x0A\x00\x00\x00", 7);//Request Ѱ�� û�ð�������Logic MifareӦ��ʱ����ʹ��Request��Ѱ�����Ͽ�RF��ʹ��Haltָ�
//			memcpy(snddd, "\x80\x05\x90\xB0\x04\x00\x00", 7);//��ѯ���Ƿ�ѯ��
			memcpy(snddd, "\x80\x05\x90\xB0\x04\x26\x00", 7);//0x26��Ѱ��ģʽҪ������Ŀ��ڶ�д��ɺ�Ҫ�뿪��Ӧ�������ٶ�д
//			memcpy(snddd, "\x80\x05\x90\xB0\x05\x00\x00", 7);//����ID������ѯ��ֱ�Ӷ�[��ѯ�����ӳɹ��󣬿�ͨ��ָ��ȡ��IDֵ]
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
				pos = XMSH_MONEY_CPU();//����Ǯ��
				//pos = XMSH_UIMm1Puchace();//�ֻ����ڵ�M1������
				XMSFBreaKCard();
				if(pos == 0xF1){
					delayxs(1);
					g24GDisFlash = 3;
					restore_flag = 0;
				}
			}
			if((ret != 0)||(XMSHRFstyle > 1)){ //��������ӳɹ��Ŀ������������Ҫ�Ͽ�����Ϊ��һ�������ϣ���Ч�������1�ף�����һ��Ҫ�Ͽ�.
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

