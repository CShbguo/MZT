/*
	different of the city.
	audio,card type
	2012.5.15 by KSLi
  */
#include "string.h"
#include "stdio.h"
#include "szct.h"
#include "..\include\include.h"
#include "Font.h"

//�˰汾����MC65 TTS�������������ȡ�

//J4 ͨ��2������������ȡ�
extern void M50Audosend(unsigned int uiLen, char *str);

//��Щ����Ҫ������Ů��
extern unsigned char nnAudioflag;
extern unsigned char gchn_eng;//��Ӣ�ı�־
extern Parameter5 cardMessage;
extern stPricetable NewPriceTab;
extern unsigned int startTime,endTime,nowTime,endTimeEn;
extern unsigned char gucEndTimeFlag;
extern unsigned char gMCardCand;
extern unsigned int c_serial;
extern stcomtime SysTime;
extern unsigned char gucRestorFlag;
extern fFlagVary_1 F_FLAG_VARY;
extern unsigned char gucSoudnorm;

extern void SoundFreeManage(char *sstr);
extern unsigned char OperPublicMessage(void);
extern unsigned char getMiKey(void);
extern void error(unsigned char code1,unsigned char Index);
extern unsigned int IsInBlackList(unsigned char *c_card_no);
extern unsigned char GetDateTime(void);
extern void getCpuInfo(stCpuInfo *inP);
extern void saveCpuInfo(unsigned char mode, unsigned char *dat);

void SoundMessage(unsigned char cmd);
#ifdef COM_TEST_		//ʹ����������
#define ComTst_head 0
#define ComTst_LenH 4
#define ComTst_LenL 5
#define ComTst_CMD 6
#define ComTst_PackNo 7
#define ComTst_Data 8

volatile unsigned char ComTest_index=0;
volatile unsigned char ComTest_Ccount;

//unsigned char GPSPCibuff[100];  ʹ��irda_rxBuf
void ComTest_DealCMD(void)
{
//	unsigned char buff[32];
	unsigned short dlen;
	
	dlen = irda_rxBuf[ComTst_LenH];
	dlen <<= 8;
	dlen += irda_rxBuf[ComTst_LenL];
	irda_rxBuf[dlen-2] = 0;//������
	if(irda_rxBuf[ComTst_CMD] == 0xA1){//����
#ifdef _TTSSound_enable_
		SoundFreeManage((char*)irda_rxBuf+ComTst_Data);//������
#endif
	}
// 	debugstring("len:");
// 	debugdata((unsigned char*)&dlen, 2, 1);
}

void ComTest_hand(unsigned char value)
{
	unsigned short plen;
	unsigned char ii;
	
	ii = value;
	//	debugdata((unsigned char*)&ii, 1, 0);
	switch (ComTest_index)
	{
	case 0:
	case 1:
	case 2:
	case 3:
		if(ii != '$'){
			ComTest_index = 0;
		}
		else{
			irda_rxBuf[ComTest_index++] = ii;
		}
		break;
	case 4://���ȸ�λ
	case 5://���׵�λ
		irda_rxBuf[ComTest_index++] = ii;
		memcpy((unsigned char*)&plen, irda_rxBuf+4, 2);
		over_turn(2, (unsigned char*)&plen);
		if(plen > 2048){//���ȴ���
			ComTest_index = 0;
			memset(irda_rxBuf,0,10);
			break;
		}
		break;
	default:
		irda_rxBuf[ComTest_index++] = ii;
		memcpy((unsigned char*)&plen, irda_rxBuf+4, 2);
		over_turn(2, (unsigned char*)&plen);
		if(plen > 2048){//���ȴ���
			ComTest_index = 0;
			memset(irda_rxBuf,0,10);
			break;
		}
		if(ComTest_index >= plen){//���ݽ������
#ifdef _debug_
			debugstring("��:");
			debugdata(irda_rxBuf, ComTest_index,1);
#endif
			ComTest_DealCMD();

			ComTest_index = 0;
			memset(irda_rxBuf,0,10);
		}
		break;
	}
}
#endif    //#ifdef COM_TEST_
//������������
void KeyInputNum(unsigned char val)
{
	unsigned char i;
	char buffer[20];
	char Bakbuf[20];
	unsigned char dly;
	unsigned char nLen;
	unsigned char ret;
	unsigned char t;
	
	i=0;
	dly=0;
	
	display(6,2,"��������ֵ",DIS_CENTER);
	memset(buffer,0,10);
	sprintf(buffer, "%02d", val);
	memcpy(Bakbuf, buffer, 20);
	nLen = strlen(buffer);
	display(2,6,buffer,0);
	while(1)
	{
		if(dly++ >= 100)
			return;
		delayxms(50);
		memset(buffer,0,10);
		buffer[0]=Bakbuf[i];
		display(2,6+i,buffer,DIS_CONVERT);
		ret =getkey(1);
		if(ret != 0)
			dly = 0;
		switch(ret)
		{
#ifndef _New_Bu_mode_
		case UP:
			if((Bakbuf[i]<='0')||(Bakbuf[i]>'9'))
				Bakbuf[i]='9';
			else if((Bakbuf[i]<='9')&&(Bakbuf[i]>'0'))
				Bakbuf[i]--;
			
			break;
		case F2:  
			i++;
			if(i>= nLen)
				i = 0;
			memset(buffer,0,10);
			memcpy(buffer,Bakbuf,8);
			display(2,6,(char*)buffer,0);
			break;
		case F1:
			if(i>0)
				i--;
			else
				i = (nLen-1);
			memset(buffer,0,10);
			memcpy(buffer,Bakbuf,8);
			display(2,6,(char*)buffer,0);
			break; 
		case DOWN:
#else
		case UP:
#endif   
			if((Bakbuf[i]<'0')||(Bakbuf[i]>='9'))
				Bakbuf[i]='0';
			else if((Bakbuf[i]<'9')&&(Bakbuf[i]>='0'))
				Bakbuf[i]++;
			break;
		case ESC:   
			goto SetupDevi_exit;
		case ENTER:
#ifdef _New_Bu_mode_
			i++;
			if(i>= nLen){
#endif
			t = 0;
			for(i=0; i<strlen(Bakbuf); i++){
				if((Bakbuf[i] < '0')||(Bakbuf[i] > '9'))
					break;
				t *=10;
				t += (Bakbuf[i]-'0');
			}
#ifdef _debug_
			debugstring("Keyin:");
			debugdata((unsigned char*)&t, 1,1);
#endif
			buffer[0] = t;
			saveCpuInfo(9, (unsigned char*)&buffer);
			goto SetupDevi_exit;
#ifdef _New_Bu_mode_
			}
			else{
				memset(buffer,0,10);
				memcpy(buffer,Bakbuf,8);
				display(2,6,(char*)buffer,0);
			}
#endif
		default:
			break;
			
		}
	}
SetupDevi_exit:
		;
}

//��ȡ��������ˢ����ʱʱ��
unsigned char Waittime(void)
{
#ifdef BUS_SYBAOSHI_
	stCpuInfo cpuinfo;
	unsigned char buff[16];
	unsigned int offset;

	offset = ((unsigned char*)&cpuinfo.stopflag)-((unsigned char*)&cpuinfo);
	cpuInfoRead(buff, offset, 2);

// 	debugstring("Waittimee1:");
// 	debugdata(buff, 1, 1);
	if(buff[0] > 40){
		buff[0] = 5;
	}
// 	debugstring("Waittimee2:");
// 	debugdata(buff, 1, 1);
	return buff[0];
#elif defined BUS_DONGXIANG_	//��������꿨20���Ӻ����ˢ
	if(cardMessage.card_catalog == CARD_OLDMAN_BUS)
		return 20;
	else
		return WAIT_TIME;
#else
	return WAIT_TIME;
#endif
}


#ifdef BUS_SYBAOSHI_
void SetWaitTime(void)
{
	unsigned char waitT;
	char disbuf();

	waitT = Waittime();
	cls();
	display(0,0,"����ˢ���ӳ�", DIS_CENTER|DIS_CONVERT);
	KeyInputNum(waitT);
}
#endif

#ifdef Card_snd1111
void snd111cmd(void)
{
	unsigned char snd11buf[10];
	memcpy(snd11buf, "\xCC\xDD\xA0\x00\x03\x00\x0D\xB0\x60", 9);
	com_snd(COM1, 9, snd11buf);
}
#endif
//----------�Ƿ�����ˢ����------------------
extern void save_h_month(void);
extern void save_normal(void);
void Save_delayCardinfo(unsigned char Cmode)
{
#ifdef MOBILE_SHIYAN_
	if((cardMessage.card_catalog!=CARD_NORMAL)&&(cardMessage.card_catalog!=CARD_COUPON))
		save_h_month();
	else
		save_normal();
#elif defined BUS_FENYI_     //2011.04.10_LWX ����ѧ������������ˢ        
	if((cardMessage.card_catalog!=CARD_NORMAL)&&(cardMessage.card_catalog!=CARD_STUDENT_BUS))
		save_h_month();
	else
		save_normal();
#elif defined BUS_SHAODONG_     //2011.05.05_LWX �۶����ſ���������ˢ        
	if((cardMessage.card_catalog!=CARD_NORMAL)&&(cardMessage.card_catalog!=CARD_TELECOM_BUS)
		&&(cardMessage.card_catalog!=CARD_TELECOM_USER)&&(cardMessage.card_catalog!=CARD_SD_YIXIN))
		save_h_month();
	else
		save_normal();
#elif defined BUS_JIUJIANG_
	if((cardMessage.card_catalog!=CARD_NORMAL)&&(cardMessage.card_catalog!=CARD_NORMAL_B)&&(cardMessage.card_catalog!=CARD_ZILU_BUS))
		save_h_month();
	else
		save_normal();
#elif defined BUS_BAOJI_
	if((cardMessage.card_catalog!=CARD_NORMAL_BUS)&&(cardMessage.card_catalog!=CARD_OLDMAN_BUS)&&(cardMessage.card_catalog!=CARD_Love_BUS))
		save_h_month();
	else
		save_normal();
#elif defined BUS_LinDe_
	if((cardMessage.card_catalog!=CARD_NORMAL_BUS)&&(cardMessage.card_catalog!=CARD_STUDENT_BUS))
		save_h_month();
	else
		save_normal();
#elif defined BUS_HANDAN_
	if(((cardMessage.card_catalog!=CARD_NORMAL)&&(cardMessage.card_catalog!=CARD_CAP_BUS)&&(cardMessage.card_catalog!=CARD_JILIAN_BUS))
		||(cardMessage.card_catalog == CARD_HANDICAPPED))
		save_h_month();
	else
		save_normal();
#elif defined BUS_DONGXIANG_	//���꿨20�����ڲ���ˢ��ѧ����������ˢ
	if((cardMessage.card_catalog!=CARD_NORMAL)&&(cardMessage.card_catalog != CARD_STUDENT_BUS)&&
		(cardMessage.card_catalog != CARD_YOUHUI_BUS)&&(cardMessage.card_catalog != CARD_YOUDAI_BUS))
		save_h_month();
	else
		save_normal();
#elif defined BUS_LAIZOUAT_	//���еĿ���������ˢ
	save_normal();
#elif defined BUS_YICHUN
	if(cardMessage.card_catalog == CARD_STUDENT_BUS) //
		save_h_month();
	else
		save_normal();
#elif defined BUS_EZOU_
	if((cardMessage.card_catalog!=CARD_NORMAL)&&(cardMessage.card_catalog!=CARD_ZILV_BUS))
		save_h_month();
	else
		save_normal();
#elif defined BUS_SDChangQing_
	if((cardMessage.card_catalog!=CARD_NORMAL)&&(cardMessage.card_catalog!=CARD_YOUHUI_BUS)&&(cardMessage.card_catalog!=CARD_YOUDAI_BUS))
		save_h_month();
	else
		save_normal();
#elif defined BUS_NanjingDZ_	//��ˮ�����п���������ˢ
//	if((cardMessage.card_catalog!=CARD_NORMAL)&&(cardMessage.card_catalog!=CARD_ZILU_BUS))
		save_h_month();
// 	else
// 		save_normal();
#elif defined Card_snd1111
// 	memset(dbuff, 0x11, 5);
// 	com_snd(COM1, 5, dbuff);
	snd111cmd();
// 	memset(dbuff, 0x11, 5);
// 	com_snd(COM1, 5, dbuff);
#elif defined BUS_LICHUANG_	//ѧ����������ˢ
	if((cardMessage.card_catalog!=CARD_NORMAL)&&(cardMessage.card_catalog!=CARD_STUDENT_BUS))
		save_h_month();
	else
		save_normal();
#elif defined BUS_KUITUN_
	if((cardMessage.card_catalog!=CARD_NORMAL)&&(cardMessage.card_catalog!=CARD_MOBILE_BUS))
		save_h_month();
	else
		save_normal();
#else
	if(cardMessage.card_catalog!=CARD_NORMAL)
		save_h_month();
	else
		save_normal();
#endif
}

//������ʾ����
void Sound_cityCMD(unsigned char sMode)
{
#ifdef _debug_
	debugstring("gucSoudnorm1=");
	debugdata((unsigned char*)&gucSoudnorm, 1, 1);
#endif
#ifdef MOBILE_SHIYAN_
	SoundMessage(SOUND_DEAL);
#elif defined BUS_BAOJI_
	if(gucSoudnorm == 0){
		if((cardMessage.card_catalog==CARD_NORMAL_BUS)||(cardMessage.card_catalog==CARD_OLDMAN_BUS)||(cardMessage.card_catalog==CARD_Love_BUS))
			audio(Audio_DONG);//��Ǯ����
		else
			SoundMessage(SOUND_DEAL);
	}
	else
		SoundMessage(SOUND_CHARGE_MONEY);	//��ʾ���ֵ
#elif defined BUS_HANDAN_
	if(gucSoudnorm == 0){
		if(sMode != 0)
			audio(Audio_DONG);//����Ʊ��Ǯ����ͨ����
		else
			SoundMessage(SOUND_DEAL);
	}
	else
		SoundMessage(SOUND_CHARGE_MONEY);	//��ʾ���ֵ
#elif defined BUS_YICHUN //-----------------------------------
	if(sMode != 0){//�ӿ���Ʊ������
		audio(Audio_DONG);
	}
	else
		SoundMessage(SOUND_DEAL);
	if(gucSoudnorm == 1){//����Ҫ��ʾ���ֵ
		delayxms(600);
		SoundMessage(SOUND_CHARGE_MONEY);
	}
#elif defined BUS_JIUJIANG_	|| defined BUS_LICHUANG_//��ʾ���ֵ
	if(gucSoudnorm == 1){//����Ҫ��ʾ���ֵ
		SoundMessage(SOUND_CHARGE_MONEY);
	}
	else{
		SoundMessage(SOUND_DEAL);
	}
#else
//	SoundMessage(0x80|SOUND_DEAL);	//ͨ��TTS
	SoundMessage(SOUND_DEAL);

#endif
}

//----------����----------------------------

//�����������ȡʵ���������
unsigned char getVoidIndex(unsigned char vall)
{
	unsigned ret;
	switch(vall){
	case 0:
#ifdef BUS_DONGXIANG_
		ret = Audio_PuTong;	//����Ҫ����ͨ��
#else
		ret = Audio_DONG;
#endif
		break;
	case 1:
		ret = Audio_STUDENT;
		break;
	case 2:
		ret = Audio_OLDMAN;
		break;
	case 3:
		ret = Audio_STUFF;
		break;
	case 4:
#ifdef Audio_YouDai
		ret = Audio_YouDai;
#else 
		ret = Audio_DONG;
#endif
		break;
	case 5:
		ret = Audio_YH;
		break;
	case 6:
#ifdef Audio_YouDai
		ret = Audio_FULI;
#else 
		ret = Audio_DONG;
#endif
		break;
	case 7:
		ret = Audio_LOVER;
		break;
	case 8:
#ifdef Audio_YouDai
		ret = Audio_Zilu;
#else 
		ret = Audio_DONG;
#endif
		break;
	case 9:
#ifdef Audio_YouDai
		ret = Audio_PuTong;
#else 
		ret = Audio_DONG;
#endif
		break;
	case 10:
#ifdef Audio_MONTH
		ret = Audio_MONTH;
#else
		ret = Audio_DONG;
#endif
		break;
	case 11:
#ifdef Audio_Man_STUFF
		ret = Audio_Man_STUFF;
#else
		ret = Audio_DONG;
#endif
		break;
	case 12:
#ifdef Audio_MAN_STUDENT
		ret = Audio_MAN_STUDENT;
#else
		ret = Audio_DONG;
#endif
		break;
	case 13:
#ifdef Audio_MAN_Month
		ret = Audio_MAN_Month;
#else
		ret = Audio_DONG;
#endif
		break;
	case 14:
#ifdef Audio_Man_FULI
		ret = Audio_Man_FULI;
#else
		ret = Audio_DONG;
#endif
		break;
	case 15:
#ifdef Audio_FAMILY
		ret = Audio_FAMILY;
#else
		ret = Audio_DONG;
#endif
		break;
	default:
		ret = Audio_DONG;
		break;
	}
	return ret;
}

#ifdef MOBILE_SHIYAN_
void setAudioSHIYAN(void)//ʮ��
{
	//	0 ��ͨ��	1 ѧ����	2 ���꿨	3 Ա����	5 ��������Ʊ��   6 ���Ŀ�
	memset(NewPriceTab.voice,Audio_DONG,32);
	NewPriceTab.voice[CARD_NORMAL] = Audio_DONG;
	NewPriceTab.voice[CARD_STUDENT] = Audio_STUDENT;
	NewPriceTab.voice[CARD_OLDMAN] = Audio_OLDMAN;
	NewPriceTab.voice[CARD_STUFF] = Audio_STUFF;
	NewPriceTab.voice[CARD_HANDICAPPED] = Audio_LOVER;
}
#elif defined BUS_PINGXIANG_
void setAudioPINGXIANG(void)//Ƽ��
{
	//	0 ��ͨ��	1 ѧ����	2 ���꿨	3 Ա����	5 ��������Ʊ��   6 ���Ŀ�
	memset(NewPriceTab.voice,Audio_DONG,32);
	NewPriceTab.voice[CARD_NORMAL] = Audio_DONG;
	NewPriceTab.voice[CARD_STUDENT] = Audio_STUDENT;
	NewPriceTab.voice[CARD_OLDMAN] = Audio_OLDMAN;
	NewPriceTab.voice[CARD_STUFF] = Audio_STUFF;
	NewPriceTab.voice[CARD_HANDICAPPED] = Audio_CY;

	NewPriceTab.voice[CARD_STUFF+20] = Audio_STUFFman;
	NewPriceTab.voice[CARD_HANDICAPPED+20] = Audio_CYman;
	NewPriceTab.voice[CARD_STUDENT+20] = Audio_STUDENTman;
	NewPriceTab.voice[CARD_OLDMAN+20] = Audio_OLDMANman;
	NewPriceTab.voice[CARD_HANDICAPPED] = Audio_YH;
}
#elif defined BUS_HANDAN_
void setAudioHANDAN(void)//����
{
	//	0 ��ͨ��	1 ѧ����	2 ���꿨	3 Ա����	5 ��������Ʊ��   6 ��ͨB��
	memset(NewPriceTab.voice,Audio_DONG,32);
	NewPriceTab.voice[CARD_NORMAL] = Audio_DONG;
	NewPriceTab.voice[CARD_STUDENT] = Audio_STUDENT;
	NewPriceTab.voice[CARD_OLDMAN] = Audio_BIRTHDAY;
	NewPriceTab.voice[CARD_STUFF] = Audio_STUFF;
	NewPriceTab.voice[CARD_COUPON] = Audio_MONTH;
	NewPriceTab.voice[CARD_RELATION_BUS] = Audio_MONTH;
	NewPriceTab.voice[CARD_HANDICAPPED] = Audio_DONG;
	NewPriceTab.voice[CARD_CAP_BUS] = Audio_DONG;		//��ͨ�ֻ���
	NewPriceTab.voice[CARD_JILIAN_BUS] = Audio_DONG;		//���п�
}
#elif defined BUS_JIUJIANG_
void setAudioJIUJIANG(void)//�Ž�
{
	memset(NewPriceTab.voice,Audio_DONG,32);
	NewPriceTab.voice[CARD_NORMAL] = Audio_DONG;
	NewPriceTab.voice[CARD_STUDENT] = Audio_YWJYu; //���������
	NewPriceTab.voice[CARD_OLDMAN] = Audio_OLDMAN;
	NewPriceTab.voice[CARD_STUFF] = Audio_STUFF;
	NewPriceTab.voice[CARD_COUPON] = Audio_STUDENT;//�Ž�
	NewPriceTab.voice[CARD_ZILU_BUS] = Audio_DONG;
}
#elif defined BUS_SHAODONG_
void setAudioSHAODONG(void)//�۶�
{
	memset(NewPriceTab.voice,Audio_DONG,32);
	NewPriceTab.voice[CARD_NORMAL_BUS-0x40] = Audio_DONG;
	NewPriceTab.voice[CARD_STUDENT_BUS-0x40] = Audio_STUDENT;
	NewPriceTab.voice[CARD_OLDMAN_BUS-0x40] = Audio_OLDMAN;

	NewPriceTab.voice[CARD_COUPON_BUS-0x40] = Audio_LOVER;
	NewPriceTab.voice[CARD_STUFF_BUS-0x40] = Audio_STUFF;
	NewPriceTab.voice[CARD_RELATION_BUS-0x40] = Audio_FAMILY;
	NewPriceTab.voice[CARD_YOUDAI_BUS-0x40] = Audio_YH;//Audio_LOVER;
	NewPriceTab.voice[CARD_ZILU_BUS-0x40]=Audio_DONG;
	NewPriceTab.voice[CARD_STUFFC_BUS-0x40]=Audio_STUFF;
	NewPriceTab.voice[CARD_YOUHUI_BUS-0x40]=Audio_FAMILY;

	NewPriceTab.voice[CARD_CONGCI_BUS-0x40]=Audio_STUDENT; //�۶�����ѧ����
	NewPriceTab.voice[CARD_SD_YIXIN-0x40]=Audio_PuTong;
}
#elif defined BUS_HEZHOU_
void setAudioHEZHOU(void)//���� ������
{
	memset(NewPriceTab.voice,Audio_DONG,32);
	NewPriceTab.voice[CARD_NORMAL-0x40] = Audio_DONG;
	NewPriceTab.voice[CARD_STUDENT-0x40] = Audio_STUDENT;
	NewPriceTab.voice[CARD_OLDMAN-0x40] = Audio_OLDMAN;
	NewPriceTab.voice[CARD_STUFF-0x40] = Audio_STUFF;
	NewPriceTab.voice[CARD_HANDICAPPED-0x40] = Audio_CY;
}
#elif defined BUS_YICHUN
void setAudioYICHUN(void)
{
	memset(NewPriceTab.voice,Audio_DONG,32);
	NewPriceTab.voice[CARD_NORMAL_BUS-0x40] = Audio_DONG;		//A ��
	NewPriceTab.voice[CARD_STUDENT_BUS-0x40] = Audio_STUDENT;	//C����"��ѧ����
	NewPriceTab.voice[CARD_OLDMAN_BUS-0x40] = Audio_FULI;		//E����"�ָ�����"
	NewPriceTab.voice[CARD_ZILU_BUS-0x40]=Audio_MONTH;		//B ����"����Ʊ��"
	NewPriceTab.voice[CARD_STUFFC_BUS-0x40]=Audio_LOVER;		//F ��Ʊ����"���ּӶ���"
	NewPriceTab.voice[CARD_STUFF_BUS - 0x40] = Audio_STUFF;	//D����"��Ա����
	
	//����
	NewPriceTab.voice[CARD_STUDENT_BUS+20-0x40] = Audio_MAN_STUDENT;	//C����"��ѧ����
	NewPriceTab.voice[CARD_OLDMAN_BUS+20-0x40] = Audio_Man_FULI;		//E����"�ָ�����"
	NewPriceTab.voice[CARD_ZILU_BUS+20-0x40]=Audio_MAN_Month;		//B ����"����Ʊ��"
	NewPriceTab.voice[CARD_STUFF_BUS+20 - 0x40] = Audio_Man_STUFF;

// 	NewPriceTab.voice[CARD_COUPON_BUS-0x40] = Audio_LOVER;
// 	NewPriceTab.voice[CARD_STUFF_BUS-0x40] = Audio_STUFF;
// 	NewPriceTab.voice[CARD_RELATION_BUS-0x40] = Audio_DONG;//�⿨������
// 	NewPriceTab.voice[CARD_YOUDAI_BUS-0x40] = Audio_YH;//Audio_LOVER;
// 	NewPriceTab.voice[CARD_ZILU_BUS-0x40]=Audio_DONG;
// 	NewPriceTab.voice[CARD_ZUL_BUS-0x40]=Audio_STUFF;
// 	NewPriceTab.voice[CARD_HETONG_BUS-0x40]=Audio_STUFF;
// 	NewPriceTab.voice[CARD_YOUHUI_BUS-0x40]=Audio_DONG;
// 	NewPriceTab.voice[CARD_YOUHUI_BUS-0x40]=Audio_YH;
// 	NewPriceTab.voice[CARD_YOUDAI_BUS-0x40] = Audio_YouDai;//Audio_LOVER;
}
#elif defined BUS_EZOU_
void setAudioEZOU(void)
{
	//	0 ��ͨ��	1 ѧ����	2 ���꿨	3 Ա����	4\5 ��Ʊ��   6 ���Ŀ�-�м���
	memset(NewPriceTab.voice,Audio_DONG,32);
	NewPriceTab.voice[0] = Audio_DONG;
	NewPriceTab.voice[1] = Audio_STUDENT;
	NewPriceTab.voice[2] = Audio_OLDMAN;
	NewPriceTab.voice[3] = Audio_STUFF;
	NewPriceTab.voice[4] = Audio_MONTH;
	NewPriceTab.voice[5] = Audio_MONTH;
	NewPriceTab.voice[6] = Audio_CY;
	NewPriceTab.voice[8] = Audio_DONG;
	NewPriceTab.voice[CARD_SCHOOL_BUS] = Audio_STUDENT;
}
#elif defined BUS_PRO_JSB
//û�У��þŽ���
void setAudioJSB(void)
{
	memset(NewPriceTab.voice,Audio_DONG,32);
	NewPriceTab.voice[CARD_NORMAL] = Audio_DONG;
	NewPriceTab.voice[CARD_STUDENT] = Audio_STUDENT; //���������
	NewPriceTab.voice[CARD_OLDMAN] = Audio_OLDMAN;
	NewPriceTab.voice[CARD_STUFF] = Audio_STUFF;
	NewPriceTab.voice[CARD_COUPON] = Audio_STUDENT;//�Ž�
	NewPriceTab.voice[CARD_ZILU_BUS] = Audio_MONTH;
	NewPriceTab.voice[CARD_HANDICAPPED] = Audio_YH;
}
#elif defined BUS_BAOJI_
void SetAudioBAOJI(void)
{
	// #define CARD_NORMAL_BUS		0				//��ͨ��
	// #define CARD_STUDENT_BUS		1				//ѧ����
	// #define CARD_SX_BUS			2				//���ǿ�
	// #define CARD_STUFF_BUS		3				//Ա����
	// #define CARD_YH_BUS			4				//�Żݿ�
	// #define CARD_YF_BUS			5				//�ŷ���
	// #define CARD_OLDMAN_BUS		6				//���꿨
	//#define CARD_Love_BUS		7				//���Ŀ�
	//#define CARD_ZILV_BUS		8				//��Ů��
	memset(NewPriceTab.voice,Audio_DONG,32);
	NewPriceTab.voice[CARD_NORMAL_BUS] = Audio_DONG;
	NewPriceTab.voice[CARD_STUDENT_BUS] = Audio_STUDENT;
	NewPriceTab.voice[CARD_SX_BUS] = Audio_BIRTHDAY;
	NewPriceTab.voice[CARD_STUFF_BUS] = Audio_STUFF;
	NewPriceTab.voice[CARD_YH_BUS] = Audio_YH;
	NewPriceTab.voice[CARD_YF_BUS] = Audio_CY;
	NewPriceTab.voice[CARD_OLDMAN_BUS] = Audio_OLDMAN;
	NewPriceTab.voice[CARD_Love_BUS] = Audio_LOVER;
	NewPriceTab.voice[CARD_ZILV_BUS] = Audio_Zilu;
}
#elif defined BUS_JNHSJY_
void SetAudioJNHSJY(void)
{
	unsigned char nj;

	memset(NewPriceTab.voice,Audio_DONG,32);
	
	nj = 0x40;
	NewPriceTab.voice[CARD_NORMAL_BUS-nj] = Audio_DONG;
	NewPriceTab.voice[CARD_STUDENT_BUS-nj] = Audio_STUDENT;
	
	NewPriceTab.voice[CARD_OLDMAN_BUS-nj] = Audio_OLDMAN;//Audio_BIRTHDAY;//Audio_OLDMAN;
	NewPriceTab.voice[CARD_STUFF_BUS-nj] = Audio_STUFF;

	NewPriceTab.voice[CARD_COUPON_BUS-nj] = Audio_LOVER;
	NewPriceTab.voice[CARD_YOUDAI_BUS-nj] = Audio_YH;//Audio_LOVER;
	NewPriceTab.voice[CARD_ZILU_BUS-nj]=Audio_DONG;
	NewPriceTab.voice[CARD_STUFFC_BUS-nj]=Audio_FAMILY;
	NewPriceTab.voice[CARD_YOUHUI_BUS-nj]=Audio_DONG;
	NewPriceTab.voice[CARD_PRACTISE_BUS-nj] = Audio_YH;//Audio_LOVER;
	NewPriceTab.voice[CARD_CONGCI_BUS-nj]=Audio_MONTH;
	NewPriceTab.voice[CARD_RELATION_BUS-nj] = Audio_DONG;//�⿨������
	NewPriceTab.voice[CARD_YOUHUI_BUS-nj]=Audio_YH;
}
#else
void setAudiocommon(void)//ͨ��
{
	unsigned char nj;
#ifdef BUS_BS_QHXLNS_
	memset(NewPriceTab.voice,Audio_DingDang,32);
#else
	memset(NewPriceTab.voice,Audio_DONG,32);
#endif

#ifdef Card_typeM_Min_is0_
	nj = 0;
#else
	nj = 0x40;
#endif
#ifdef BUS_BS_QHXLNS_
	NewPriceTab.voice[CARD_NORMAL_BUS-nj] = Audio_DingDang;
#else
	NewPriceTab.voice[CARD_NORMAL_BUS-nj] = Audio_DONG;
#endif
	NewPriceTab.voice[CARD_STUDENT_BUS-nj] = Audio_STUDENT;
	
#ifdef BUS_DEXING_
	NewPriceTab.voice[CARD_OLDMAN_BUS-nj] = Audio_BIRTHDAY;//Audio_BIRTHDAY;//Audio_OLDMAN;
#else
	NewPriceTab.voice[CARD_OLDMAN_BUS-nj] = Audio_OLDMAN;//Audio_BIRTHDAY;//Audio_OLDMAN;
#endif
	
	NewPriceTab.voice[CARD_STUFF_BUS-nj] = Audio_STUFF;
#ifndef Card_typeM_Min_is0_
	NewPriceTab.voice[CARD_COUPON_BUS-nj] = Audio_LOVER;
	NewPriceTab.voice[CARD_YOUDAI_BUS-nj] = Audio_YH;//Audio_LOVER;
	NewPriceTab.voice[CARD_ZILU_BUS-nj]=Audio_DONG;
	NewPriceTab.voice[CARD_STUFFC_BUS-nj]=Audio_STUFF;
	NewPriceTab.voice[CARD_YOUHUI_BUS-nj]=Audio_DONG;
	NewPriceTab.voice[CARD_PRACTISE_BUS-nj] = Audio_YH;//Audio_LOVER;
	NewPriceTab.voice[CARD_CONGCI_BUS-nj]=Audio_MONTH;
	NewPriceTab.voice[CARD_RELATION_BUS-nj] = Audio_DONG;//�⿨������
	NewPriceTab.voice[CARD_YOUHUI_BUS-nj]=Audio_YH;
#endif
//	NewPriceTab.voice[CARD_HETONG_BUS-nj]=Audio_STUFF;
//	NewPriceTab.voice[CARD_JILIAN_BUS-nj]=Audio_DONG;
}
#endif
void SetAudio(void)//���貿:ʮ��,
{
#ifdef AutoAudio_line
	return;
#else//#ifdef AutoAudio_line
#ifdef MOBILE_SHIYAN_
	setAudioSHIYAN();
#elif defined BUS_PINGXIANG_
	setAudioPINGXIANG();
#elif defined BUS_HANDAN_
	setAudioHANDAN();
#elif defined BUS_JIUJIANG_
	setAudioJIUJIANG();
#elif defined BUS_SHAODONG_
	setAudioSHAODONG();
#elif defined BUS_HEZHOU_
	setAudioHEZHOU();
#elif defined BUS_YICHUN
	setAudioYICHUN(); 
#elif defined BUS_EZOU_
	setAudioEZOU();
#elif defined BUS_PRO_JSB
	setAudioJSB();
#elif defined BUS_BAOJI_
	SetAudioBAOJI();
#elif defined BUS_JNHSJY_
	SetAudioJNHSJY();
#else
	setAudiocommon();
#endif
#endif//#ifdef AutoAudio_line
}

void SoundMsgTTS(char *TTS_Str)
{
	M50Audosend(strlen(TTS_Str), TTS_Str);
}

//cmd ���λΪ1����ʹ��TTS������
void SoundMsgCommon(unsigned char cmd)
{
	unsigned char css;
	unsigned char TTsflag=0;
	const char TTSList[34][11]={
"��ͨ��\0", "��ͨ��\0", "ѧ����\0", "�Ŵ���\0",  "��Ʊ��\0",  "��Ʊ��\0",  "���꿨\0",  "���ſ�\0",  "Ա����\0",  "�Żݿ�\0",  
"������\0", "���\0", "�м��˿�\0", "Ա����\0",  "ѧԱ��\0",  "���鿨\0",  "�Ŵ���\0",  "�Ŵ���\0",  "�Ŵ���\0",  "�Ŵ���\0", 
"�Ŵ���\0", "�Ŵ���\0", "�Ŵ���\0", "�Ŵ���\0",  "�Ŵ���\0",  "�Ŵ���\0",  "�Ŵ���\0",  "�Ŵ���\0",  "�Ŵ���\0",  "�Ŵ���\0",  
"�Ŵ���\0",  "�Ŵ���\0",  "�Ŵ���\0", "\0"
	};
	
	SetAudio();
	css = cardMessage.card_catalog;
	if(css >= 0x40)
		css -= 0x40;
#ifdef _debug_
	debugstring("SoundMessage cmd=");
	debugdata(&cmd, 1, 0);
	debugstring("css=");
	debugdata((unsigned char *)&css, 1, 1);
	debugstring("Sound Port=");
	debugdata(NewPriceTab.voice+css, 1, 1);
#endif
	if(gchn_eng == 'E'){//��û��Ӣ������
		if((cmd != SOUND_DEAL)&&(cmd != SOUND_DI_BUS))
			cmd = ERR_NO_PSAM;
	}
	TTsflag = (cmd&0x80);
	cmd &= 0x7F;
	switch(cmd)
	{
	case INVALID_CARD:
		audio(Audio_INVALID);	//�Ƿ���
		break;
	case SOUND_FLING_MONEY:
		audio(Audio_FLING_MONEY);	//��Ͷ��
		break;
	case SOUND_CHARGE_MONEY:
		audio(Audio_PLUS_MONEY);
		break;
	case SOUND_DEAL:
		if(NewPriceTab.voice[css] == 0)
			css = 0;
		if(TTsflag != 0){
			if(css > 32)
				css = 0;
			SoundMsgTTS((char*)TTSList[css]);
		}
		else
			audio(NewPriceTab.voice[css]);	//��������
		beep(1,100,50);
		break;
	case SOUND_DI_BUS:			//
		beep(1,100,50);
		break;
	case ERR_NO_PSAM:
		beep(8,400,40);		//PSAM��������
		break;
	default:
		break;
	}
}

//cmd ���λΪ1����ʹ��TTS������
void SoundMessage(unsigned char cmd)
{
	SoundMsgCommon(cmd);
}

#ifdef MOBILE_SHIYAN_
int getCardtypeShiyan(char *cardD, unsigned char type)
{
	if(type == CARD_NORMAL){
		strcpy(cardD, "��ͨ��");
	}
	else if(type == CARD_STUDENT){
		strcpy(cardD, "ѧ����");
	}
	else if(type == CARD_COUPON){
		strcpy(cardD, "�Żݿ�");
	}
	else if(type == CARD_OLDMAN){
		strcpy(cardD, "���꿨");
	}
	else if(type == CARD_RELATION_BUS){
		strcpy(cardD, "������");
	}
	else if(type == CARD_STUFF){
		strcpy(cardD, "Ա����");
	}
	else if(type == CARD_DRIVER_BUS){
		strcpy(cardD, "˾����");
	}
	else 
		cardD[0] = 0;
	if(strlen(cardD) == 0)
		return ST_ERROR;
	else
		return ST_OK;
}
#elif defined BUS_JIUJIANG_
int getCardtypeJIUJIANG(char *cardD, unsigned char type)
{
	if(type == CARD_NORMAL){
		strcpy(cardD, "��ͨ��");
	}
	else if(type == CARD_STUDENT){
		strcpy(cardD, "����");
	}
	else if(type == CARD_HANDICAPPED){
		strcpy(cardD, "�ͱ���");
	}
	else if(type == CARD_COUPON){
		strcpy(cardD, "ѧ����");
	}
	else if(type == CARD_CAP_BUS){
		strcpy(cardD, "ִ�ڿ�");
	}
	else if(type == CARD_OLDMAN){
		strcpy(cardD, "���꿨");
	}
	else if(type == CARD_JILIAN_BUS){
		strcpy(cardD, "��Ʊ��");
	}
	else if(type == CARD_ZILU_BUS){
		strcpy(cardD, "���п�");
	}
	else if(type == CARD_17RD_BUS){
		strcpy(cardD, "17·��");
	}
	else if(type == CARD_RELATION_BUS){
		strcpy(cardD, "������");
	}
	else if(type == CARD_NORMAL_B){
		strcpy(cardD, "��ͨB ");
	}
	else if(type == CARD_STUFF){
		strcpy(cardD, "Ա����");
	}
	else if(type == CARD_DRIVER_BUS){
		strcpy(cardD, "˾����");
	}
	else 
		cardD[0] = 0;
	if(cardD[0] == 0)
		return ST_ERROR;
	else
		return ST_OK;
}
#elif defined BUS_PINGXIANG_
int getCardtypePINGXIANG(char *cardD, unsigned char type)
{
	if(type == CARD_NORMAL){
		strcpy(cardD, "��ͨ��");
	}
	else if(type == CARD_STUDENT){
		strcpy(cardD, "ѧ����");
	}
	else if(type == CARD_COUPON){
		strcpy(cardD, "�Żݿ�");
	}
	else if(type == CARD_OLDMAN){
		strcpy(cardD, "���꿨");
	}
	else if(type == CARD_RELATION_BUS){
		strcpy(cardD, "������");
	}
	else if(type == CARD_STUFF){
		strcpy(cardD, "Ա����");
	}
	else if(type == CARD_DRIVER_BUS){
		strcpy(cardD, "˾����");
	}
	else 
		cardD[0] = 0;
	if(strlen(cardD) == 0)
		return ST_ERROR;
	else
		return ST_OK;
}
#elif defined BUS_HANDAN_
int getCardtypeHANDAN(char *cardD, unsigned char type)
{
	if(type == CARD_NORMAL){
		strcpy(cardD, "��ͨ��");
	}
	else if(type == CARD_STUDENT){
		strcpy(cardD, "ѧ����");
	}
	else if(type == CARD_COUPON){
		strcpy(cardD, "��Ʊ��");
	}
	else if(type == CARD_OLDMAN){
		strcpy(cardD, "���Ͽ�");
	}
	else if(type == CARD_HANDICAPPED){
		strcpy(cardD, "��ͨ��");
	}
	else if(type == CARD_RELATION_BUS){
		strcpy(cardD, "��Ʊ��");
	}
	else if(type == CARD_CAP_BUS){
		strcpy(cardD, "��ͨ��");
	}
	else if(type == CARD_JILIAN_BUS){
		strcpy(cardD, "���п�");
	}
	else if(type == CARD_STUFF){
		strcpy(cardD, "Ա����");
	}
	else if(type == CARD_DRIVER_BUS){
		strcpy(cardD, "˾����");
	}
	else{
		sprintf(cardD, "%d�ſ�", type);
		//cardD[0] = 0;
		return ST_ERROR;
	}
	return ST_OK;
}
#elif defined BUS_EZOU_
int getCardtypeEZOU(char *cardD, unsigned char type)
{
	switch(type){
	case CARD_NORMAL_BUS:
		strcpy(cardD, "��ͨ��");
		break;
	case CARD_STUDENT_BUS:
		strcpy(cardD, "ѧ����");
		break;
	case 4:
		strcpy(cardD, "��Ʊ��");
		break;
	case 5:
		strcpy(cardD, "��Ʊ��");
		break;
	case 6:
		strcpy(cardD, "�ŷ���");
		break;
	case 8:
		strcpy(cardD, "���ڿ�");
		break;
	case 9:
		strcpy(cardD, "У԰��");
		break;
	case CARD_STUFF_BUS:
		strcpy(cardD, "Ա����");
		break;
	case CARD_DRIVER_BUS:
		strcpy(cardD, "˾����");
		break;
	default:
		cardD[0] = 0;
		break;
	}
	if(strlen(cardD) == 0)
		return ST_ERROR;
	else
		return ST_OK;
}
#elif defined BUS_YICHUN
int getCardtypeYICHUN(char *cardD, unsigned char type)
{
	if(type == CARD_NORMAL_BUS){
		strcpy(cardD, "��ͨ��");
	}
	else if(type == CARD_STUDENT_BUS){
		strcpy(cardD, "ѧ����");
	}
	else if(type == CARD_ZILU_BUS){
		strcpy(cardD, "��Ʊ��");
	}
	else if(type == CARD_OLDMAN_BUS){
		strcpy(cardD, "������");
	}
	else if(type == CARD_STUFFC_BUS){
		strcpy(cardD, "���Ŀ�");//��Ʊ��
	}
	else if(type == CARD_STUFF_BUS){
		strcpy(cardD, "Ա����");
	}	
	else if(type == CARD_COUPON_BUS){
		strcpy(cardD, "Ա����");
	}
	else if(type == CARD_DRIVER_BUS){
		strcpy(cardD, "˾����");
	}
	else if(type == CARD_MOBILE_BUS){
		strcpy(cardD, "�ֻ���");
	}
	else 
		cardD[0] = 0;
	if(strlen(cardD) == 0)
		return ST_ERROR;
	else
		return ST_OK;
}
#elif defined BUS_PRO_JSB
int getCardtypeJSB(char *cardD, unsigned char type)
{
	if(type == CARD_NORMAL){
		strcpy(cardD, "��ͨ��");
	}
	else if(type == CARD_STUDENT){
		strcpy(cardD, "����");
	}
	else if(type == CARD_HANDICAPPED){
		strcpy(cardD, "�Żݿ�");
	}
	else if(type == CARD_COUPON){
		strcpy(cardD, "ѧ����");
	}
	else if(type == CARD_CAP_BUS){
		strcpy(cardD, "ִ�ڿ�");
	}
	else if(type == CARD_OLDMAN){
		strcpy(cardD, "���꿨");
	}
	else if(type == CARD_JILIAN_BUS){
		strcpy(cardD, "��Ʊ��");
	}
	else if(type == CARD_ZILU_BUS){
		strcpy(cardD, "���㿨");
	}
	else if(type == CARD_17RD_BUS){
		strcpy(cardD, "17·��");
	}
	else if(type == CARD_RELATION_BUS){
		strcpy(cardD, "������");
	}
	else if(type == CARD_STUFF){
		strcpy(cardD, "Ա����");
	}
	else if(type == CARD_DRIVER_BUS){
		strcpy(cardD, "˾����");
	}
	else 
		cardD[0] = 0;
	if(strlen(cardD) == 0)
		return ST_ERROR;
	else
		return ST_OK;
}
#elif defined BUS_BAOJI_
int getCardtypeBAOJI(char *cardD, unsigned char type)
{
	if(type == CARD_NORMAL_BUS){
		strcpy(cardD, "��ͨ��");
	}
	else if(type == CARD_STUDENT_BUS){
		strcpy(cardD, "ѧ����");
	}
	else if(type == CARD_SX_BUS){
		strcpy(cardD, "���ǿ�");
	}
	else if(type == CARD_STUFF_BUS){
		strcpy(cardD, "Ա����");
	}
	else if(type == CARD_YH_BUS){
		strcpy(cardD, "�Żݿ�");
	}
	else if(type == CARD_YF_BUS){
		strcpy(cardD, "�Ŵ���");
	}
	else if(type == CARD_OLDMAN_BUS){
		strcpy(cardD, "���꿨");
	}
	else if(type == CARD_Love_BUS){
		strcpy(cardD, "���Ŀ�");
	}
	else if(type == CARD_ZILV_BUS){
		strcpy(cardD, "��Ů��");
	}
	else if(type == CARD_DRIVER_BUS){
		strcpy(cardD, "˾����");
	}
	else 
		cardD[0] = 0;
	if(strlen(cardD) == 0)
		return ST_ERROR;
	else
		return ST_OK;
}
#elif defined BUS_JNHSJY_
int getCardtypeJNHSJY(char *cardD, unsigned char type)
{
	if(type == CARD_NORMAL_BUS){
		strcpy(cardD, "��ͨ��");
	}
	else if(type == CARD_STUDENT_BUS){
		strcpy(cardD, "ѧ����");
	}
	else if(type == CARD_OLDMAN_BUS){
		strcpy(cardD, "���꿨");
	}
	else if(type == CARD_STUFFC_BUS){
		strcpy(cardD, "������");
	}
	else if(type == CARD_DRIVER_BUS){
		strcpy(cardD, "˾����");
	}
	else 
		cardD[0] = 0;
	if(strlen(cardD) == 0)
		return ST_ERROR;
	else
		return ST_OK;
}
#elif defined ZongKong_CPU_
int getZongKongCardtype(char *cardD, unsigned char type)
{
	sprintf(cardD, "%d�࿨", type);
	return ST_OK;
}
#else
int getCardtypecommon(char *cardD, unsigned char type)
{
	if(type == CARD_NORMAL_BUS){
		strcpy(cardD, Card_Name_NORMAL);
	}
	else if(type == CARD_STUDENT_BUS){
		strcpy(cardD, Card_Name_STUDENT);
	}
#ifdef CARD_ZILU_BUS
	else if(type == CARD_ZILU_BUS){
#ifdef BUS_NanjingDZ_
		strcpy(cardD, "������");
#else
		strcpy(cardD, "��Ʊ��");
#endif
	}
#endif
	else if(type == CARD_OLDMAN_BUS){
		strcpy(cardD, Card_Name_OLDMAN);
	}
//	else if(type == CARD_ZUL_BUS){
//		strcpy(cardD, "���Ŀ�");��Ʊ��
//	}

#ifdef CARD_COUPON_BUS
	else if(type == CARD_COUPON_BUS){
		strcpy(cardD, Card_Name_COUPON);
	} 
	else if(type == CARD_YOUDAI_BUS){
		strcpy(cardD, Card_Name_YOUDAI);
	}  
	else if(type == CARD_YOUHUI_BUS){
		strcpy(cardD, Card_Name_YOUHUI);
	}  
#endif
#ifdef CARD_PRACTISE_BUS
	else if(type == CARD_PRACTISE_BUS){
		strcpy(cardD, Card_Name_YOUHUI);
	}
#endif
	else if(type == CARD_STUFF_BUS){
		strcpy(cardD, Card_Name_STUFF);
	}
	else if(type == CARD_DRIVER_BUS){
		strcpy(cardD, Card_Name_Driver);
	}
	else if(type == CARD_MOBILE_BUS){
		strcpy(cardD, "�ֻ���");
	}
	else 
		cardD[0] = 0;
	if(strlen(cardD) == 0)
		return ST_ERROR;
	else
		return ST_OK;
}
#endif

int getCardtype(char *cardD, unsigned char type)
{//�Żݿ���ʾ
#ifdef MOBILE_SHIYAN_
	return getCardtypeShiyan(cardD, type);
#elif defined BUS_JIUJIANG_
	return getCardtypeJIUJIANG(cardD, type);
#elif defined BUS_PINGXIANG_
	return getCardtypePINGXIANG(cardD, type);
#elif defined BUS_HANDAN_
	return getCardtypeHANDAN(cardD, type);
#elif defined BUS_EZOU_
	return getCardtypeEZOU(cardD, type);
#elif defined ZongKong_CPU_
	return getZongKongCardtype(cardD, type);
#elif defined BUS_YICHUN
	if(type < 0x40)
		type += 0x40;
	return getCardtypeYICHUN(cardD, type);
#elif defined BUS_PRO_JSB
	return getCardtypeJSB(cardD, type);
#elif defined BUS_BAOJI_
	return getCardtypeBAOJI(cardD, type);
#elif defined BUS_JNHSJY_
	return getCardtypeJNHSJY(cardD, type);
#else
#ifndef Card_typeM_Min_is0_
	if(type < 32)
		type += 0x40;
#endif
	return getCardtypecommon(cardD, type);
#endif
	
}

//-------------------------------------------
#ifdef BUS_ANLONG_
unsigned char Card_typeProcessANLONG(void)	
{
	unsigned char pos;
	unsigned char buffer[256];
	unsigned char cNo[8];

	F_FLAG_VARY.stop_flag = 0;//һֱ������״̬

// 	11����(������)����A��������+"0337"    ����B��������+"0755"
// 	12��13��14��15����(12Ǯ������13����Ϊ������Ϣ����14��15��Ʊ��)����A������������������ǰ4�ֽ����+����ǰ2�ֽ�    ����B�����������������Ӻ�4�ֽ����+����ǰ2�ֽ�
	memcpy(cNo, (unsigned char*)&c_serial, 4);
	cNo[4] = 0x03;
	cNo[5] = 0x37;
	
	memset((unsigned char*)&cardMessage.listSector, 0xff, 9);			
	cardMessage.publishSector=11;		
	cardMessage.publicMessageSector=13;	
	cardMessage.oddMonthSector=14;		
	cardMessage.dodMonthSector=15;		
	cardMessage.publicMoneySector=12;

	memcpy(cardMessage.PublishKEYA, cNo, 6);
	memcpy(cardMessage.PublichKEYA, cNo, 6);
	memcpy(cardMessage.MoneyKEYA, cNo, 6);
	memcpy(cardMessage.MonthKEYA, cNo, 6);
	memcpy(cardMessage.RecodeKEYA, cNo, 6);
	
#ifdef _debug_
	debugdata((unsigned char *)&c_serial, 4, 1);
	debugdata((unsigned char *)&cardMessage.publishSector, 1, 1);
	debugdata(cardMessage.PublishKEYA, 6, 1);
#endif
	if (MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,cardMessage.PublishKEYA,cardMessage.publishSector*4)==0)
	{
		debugstring("MifareAuthKey error\r\n ");
		return ST_ERROR; 
	}
//	debugstring("E ");
	
	if(MifareRead(cardMessage.publishSector*4,buffer)==0)
	{
		debugstring("read 4 error\r\n ");
		return ST_ERROR;
	}

#ifdef _debug_
	debugstring("1������Ϣ0��\r\n");
	debugdata(buffer,16,1);
#endif	
	if(MiBlockInvalid(1,15,buffer)==ST_ERROR) 
	{
		return ST_ERROR;
	}
	
	gMCardCand = CARDSTYLE_ANLONG;
	pos=0;
	memcpy(cardMessage.CityCode, buffer+pos, CITY_CODE_LEN);			//���д���	2
	pos+=CITY_CODE_LEN;
	memcpy(cardMessage.TradeCode, buffer+pos, TRADE_CODE_LEN);			//��ҵ����	2
	pos+=TRADE_CODE_LEN;	
	memcpy((unsigned char *)cardMessage.PublishBicker, buffer+pos, PUBLISH_BICKER_LEN);	//����ˮ��	4	
	memcpy(cardMessage.CardCheckCode, buffer+8, CARD_CHECH_CODE_LEN);	//mac��		4
	cardMessage.card_catalog=buffer[13];								//����		1
#ifdef New_Times_Card__
	cardMessage.Zicard_catalog = buffer[14];
#else
	cardMessage.Zicard_catalog = 0;
#endif

#ifdef _debug_
	debugstring("card_catalog:");
	debugdata((unsigned char*)&cardMessage.card_catalog,1,1);
#endif	

	nnAudioflag = 0;

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

	if(MifareRead(cardMessage.publishSector*4+1, buffer)==0)//�������� ��Ч���� �������� ��Ѻ��	����	У��
	{
		return ST_ERROR;
	}

#ifdef _debug_
	debugstring("1������Ϣ1:");
	debugdata(buffer, 16, 1);
#endif
	
	if(MiBlockInvalid(1,15,buffer)==ST_ERROR) 
	{
#ifdef _debug_
		debugstring("1������Ϣ1 У���\r\n");
#endif
	
		return ST_ERROR;
	}
//ʱ�仺����0
	endTime = 0;
	startTime = 0;
	nowTime = 0;
	endTimeEn = 0;

	memcpy(cardMessage.stuffNO, buffer, 4);//Ա�����Ŀ���,��Ҫ��˾����ʹ��

	nnAudioflag = buffer[14];//�Ա�
	
#ifdef BUS_SHAODONG_	//�ܶ�ԭ��û������Ŀ�Ҫ�������󡣸������Ǻ󣬴˶δ�����Բ��á�
	if((buffer[4] == 0x20)&&(buffer[5] == 0x20)){
		if((cardMessage.card_catalog != CARD_NORMAL_BUS)&&
			(cardMessage.card_catalog != CARD_SD_YIXIN)&&
			(cardMessage.card_catalog != CARD_TELECOM_USER)&&
			(cardMessage.card_catalog != CARD_TELECOM_BUS)){
			memcpy(buffer+4, "\x20\x12\x09\x01", 4);
		}
	}
#endif

	memcpy(buffer+50, buffer+4, 4);
	buffer[52] = bcdDEC(buffer[52], 1);//��Ч����,��ǰһ������ʾ��
	if(buffer[52] == 0){
		buffer[52] = 0x12;
		buffer[51] = bcdDEC(buffer[51], 1);
	}

	memcpy((unsigned char*)&endTimeEn, buffer+50, 4);	//��Ч����,�����ġ�
	memcpy((unsigned char*)&endTime, buffer+4, 4);//��Ч���ڣ�����ǰ��
	memcpy((unsigned char*)&startTime, buffer+8, 4);//��������
	
	if(GetDateTime()==ST_ERROR) 
	{
		error(ERR_READ_TIME,0);
		return ST_ERROR;
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
		return ST_OK;
	}
	if(getMiKey()==ST_ERROR)
	{
		return ST_ERROR;
	}

	gucRestorFlag = 0;

#ifdef _debug_
	debugstring("Oper ---------\r\n");
	debugdata((unsigned char*)&pos, 1, 1);
#endif
	pos=OperPublicMessage(); 
#ifdef _debug_
	debugstring("OperPublicMessage pos:");
	debugdata((unsigned char*)&pos, 1, 1);
#endif
	getCpuInfo((stCpuInfo*)buffer); //�ڶ�����ʱ����	
	if(pos==CARD_PUBLIC_BAD) {
		return CARD_PUBLIC_BAD;
	}
	else if(pos==CARD_BLACK)
	{
		return CARD_BLACK;	
	}
	else if(pos == CARD_CHAGEERR){
		return CARD_CHAGEERR;
	}
	if(IsInBlackList(cardMessage.PublishBicker) < FLASH_BLK_END){
		return CARD_BLACK_PRO;
	}
	if(cardMessage.card_catalog>=0x40)
	{
		gucEndTimeFlag = 0;
		if(cardMessage.card_catalog!=CARD_NORMAL)//
		{
			if((startTime>nowTime)||(endTime<nowTime)||(startTime>endTime)){
				return CARD_NO_TIME;
			}
			else if(endTimeEn<=nowTime){//��Ҫ��ʾ���������ˡ�
				gucEndTimeFlag = 1;
			}
		}
		if((NewPriceTab.rate[cardMessage.card_catalog-0x40] == 103)||(NewPriceTab.rate[cardMessage.card_catalog-0x40] == 203)){
			return CARD_MONEY; //�˿����ܿ۴Σ�ֻ�ܿ�Ǯ
		}
		if((NewPriceTab.rate[cardMessage.card_catalog-0x40] == 104)||(NewPriceTab.rate[cardMessage.card_catalog-0x40] == 204)){
			return CARD_forbid;//�˿�����ֹʹ��
		}
		if(NewPriceTab.rate[cardMessage.card_catalog-0x40]<=100)
		{
			return CARD_MONEY;				//�ֽ�
		}
		if((NewPriceTab.rate[cardMessage.card_catalog-0x40]==101)||(NewPriceTab.rate[cardMessage.card_catalog-0x40]==201))
		{
			return MONTH_CARD;//�۴κ���Ҫ�۸��ӷѣ�����Ʊ����
		}
		if((NewPriceTab.rate[cardMessage.card_catalog-0x40] ==102)||(NewPriceTab.rate[cardMessage.card_catalog-0x40] == 202))	
		{
			return CARD_STUFF_BUS;//Ա����
		}
		return ST_ERROR;
	}
	else {
		return CONTROL_CARD;
	}
}
#endif

//�ж�������ʱ�������Ƿ���ȷ 2010.03.11
unsigned char chekMdate(unsigned int ddtt)
{
	unsigned char buff[10];
	
	memcpy(buff, (unsigned char*)&ddtt, 4);
	if(buff[0] > 0x31)
		return ST_ERROR;
	if((buff[1] == 0) || (buff[1] > 0x12))
		return ST_ERROR;
	if(buff[2] > 0x99)
		return ST_ERROR;
	if((buff[3] < 0x19) || (buff[3] > 0x20))
		return ST_ERROR;
	return ST_OK;
}

#ifdef CS456_ASCii_
extern void dis_time_mini(unsigned char x, stcomtime *time);
extern unsigned char WriteRecord(unsigned char *buffer, unsigned char mode);
extern unsigned char restore_flag;
void disCS456_ASCIIRec(unsigned char *RRebuf)
{
	DEALRECODE_CS456_ASCii* rRecordRS232P;
	char dis[32];

	rRecordRS232P = (DEALRECODE_CS456_ASCii*)RRebuf;
	cls();
	
	memset(dis, 0, 32);
	memcpy(dis, "����:", 5);
	memcpy(dis+5, rRecordRS232P->rCardID, 16);
	display(0,0,dis, 0);
	memset(dis, 0, 32);
	memcpy(dis, "����:", 5);
	memcpy(dis+5, rRecordRS232P->rName, 16);
	display(2,0,dis, 0);
	memset(dis, 0, 32);
	memcpy(dis, rRecordRS232P->rNameID, 16);
	display(4,0,dis, 0);
	dis_time_mini(6, (stcomtime*)&rRecordRS232P->rDealTime);
}

void CS456_ASDisplay(void)
{
	DEALRECODE_CS456_ASCii rRecordRS232;
	unsigned char buffer[32];
	char dis[3][32];

	GetDateTime();
	memset(dis[0], 0, 32);
	memset(dis[1], 0, 32);
	memset(dis[2], 0, 32);
	memcpy(dis[0], "ID:", 3);
	memcpy(dis[1], "����:", 5);
	memcpy(dis[2], "����:", 5);
	memcpy(buffer, "\xA0\xA1\xA2\xA3\xA4\xA5", 6);

	MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,buffer, 4);

	Delay(1);
	if(MifareRead(4, buffer) ==0) 
	{
		debugstring("read 4 error\r\n ");
		return;
	}
	memcpy(dis[0]+3, buffer, 16);
	if(MifareRead(5, buffer) ==0) 
	{
		debugstring("read 5 error\r\n ");
		return;
	}
	memcpy(dis[1]+5, buffer, 16);
	if(MifareRead(6, buffer) ==0) 
	{
		debugstring("read 6 error\r\n ");
		return;
	}
	memcpy(dis[2]+5, buffer, 16);
	cls();
	display(0,0,"ˢ���ɹ�:",0);
	display(2, 0, dis[1], 0);
	display(4, 0, dis[2], 0);
	miniDispstr(6, 0, dis[0], 0);
	audio(Audio_SuaKaOK);
		
	memset((unsigned char*)&rRecordRS232, 0, sizeof(DEALRECODE_CS456_ASCii));
	Get_SerialNum(rRecordRS232.rDeviceNo);//�豸Ψһ��
	rRecordRS232.rDealType = ID_REC_CS456_ASCii;
	memcpy(rRecordRS232.rDealTime, (unsigned char*)&SysTime, 7);//��������ʱ��
	memcpy(rRecordRS232.rName, dis[1]+5, 16);
	memcpy(rRecordRS232.rNameID, dis[0]+3, 16);
	memcpy(rRecordRS232.rCardID, dis[2]+5, 16);
#ifdef _debug_
	debugstring("rRecordRS232 REC:");
	debugdata((unsigned char*)&rRecordRS232, 64, 1);
#endif
	WriteRecord((unsigned char*)&rRecordRS232, 0);
	restore_flag = 3;
	end_card();
	return;
}
#endif //#ifdef CS456_ASCii_

unsigned char SYSgetbussinessNO(unsigned char *dat)
{
#ifdef CLOUD_BUSSINESSNO
	memcpy(dat, CLOUD_BUSSINESSNO, 2);
	return ST_OK;
#else
	return getbussinessNO(dat);//���ֽ��̻����
#endif
}

//�Ƚ��Ƿ���ͬһ���̻��Ŀ�,ֻ���˱�ʹ����
unsigned char cmpBussineesNo(void)
{
#if (defined(BUS_YIBING_) || defined(BUS_YIBING_two_) || defined BUS_YIBING_3_)
	unsigned char bnbuf[4];
	SYSgetbussinessNO(bnbuf);
#ifdef _debug_
	debugstring("Cmp BN::");
	debugdata(bnbuf, 2, 1);
	debugdata(cardMessage.TradeCode, 2, 1);
#endif
	if(memcmp(bnbuf, cardMessage.TradeCode, 2) == 0)
		return ST_OK;
	else
		return ST_ERROR;
#else
	return ST_OK;
#endif

}
extern unsigned char DriverCardDeal(void);
unsigned char Card_typeProcess_nomal(unsigned char mode)	
{
	unsigned char pos;
	unsigned char buffer[256];
	unsigned char cNo[8];
	unsigned char i,ii,flag;


#ifdef BUS_Cloud_
	unsigned int crc32;
	stCpuInfo cpuinfo;
						  
	getCpuInfo(&cpuinfo);
	if(cpuinfo.catalogPOST > 15)
		cpuinfo.catalogPOST = 0;
	cardMessage.listSector = cpuinfo.catalogPOST;
	memcpy(cNo, (unsigned char *)&c_serial, 4);
	cNo[0] ^= 0x71;
	cNo[1] ^= 0x30;
	cNo[2] ^= 0x4B;
	cNo[3] ^= 0x19;
	memcpy(buffer, cNo, 4);//0����Ĭ����Կ
	memcpy(buffer+4, cNo, 2);//0����Ĭ����Կ
#else //#ifdef BUS_Cloud_
	memcpy(cNo, (unsigned char *)&c_serial, 4);
	cardMessage.listSector = 0;
	memcpy(buffer, "\xa0\xa1\xa2\xa3\xa4\xa5", 6);//0����Ĭ����Կ
#endif

	flag = 0xff;
	cardMessage.publishSector = 1;//the bussiness  card used
	for(i=0; i<2; i++){
#ifdef _debug_
		debugstring("card type proce:");
		debugdata((unsigned char*)&cardMessage.listSector, 1, 1);
		debugdata(buffer, 6, 1);
#endif
		pos = ((cardMessage.listSector*4)+1);//��Ŀ¼����һ�顣
		MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,buffer,cardMessage.listSector*4);
#ifdef _debug_
		debugstring("card type proce2:");
		debugdata((unsigned char*)&pos, 1, 1);
#endif
		Delay(1);
		if(MifareRead(pos, buffer) ==0) 
		{
			debugstring("read 1 error\r\n ");
			profile(0);
		}
		else{
#ifdef CS456_ASCii_
			if(memcmp(buffer, "\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA", 8) == 0){//���ڿ��ڣ������ſ�
				CS456_ASDisplay();
				return ST_ERROR;
			}
#endif
			flag = 0;
			break;
		}
#ifdef _YiDa_Card_
		if(buffer[0] != 0xA0){ //��A0��Կ��֤���˴���0��������Կ��
			memcpy(buffer, "\xa0\xa1\xa2\xa3\xa4\xa5", 6);//0����Ĭ����Կ
		}
#endif
	}

	if(flag != 0){
#ifndef BUS_Cloud_
		return 0xA2;
#elif defined BUS_ANLONG_
		return 0xA2;
#else
		profile(0);
#endif
	}
	else{//��1����ȷ
#ifdef _YiDa_Card_ //�ж��ǲ����մ�Ŀ�
		ii = CRC8_Tab(buffer, 15);
#ifdef _debug_
		debugstring("0����1��Ϣ:");
		debugdata(buffer,16,1);
		debugstring("CRC:");
		debugdata((unsigned char*)&ii,1,1);
#endif
		if(((buffer[14] == 0xAA)||(buffer[14] == 0xBB))&& \
			((buffer[0] == 0x50)||(buffer[0] == 0x51)||(buffer[0] == 0x52)))
		{
			return 0xA3;//���մ�Ŀ�
		}
#endif // _YiDa_Card_ //�ж��ǲ����մ�Ŀ�
// 		if(memcmp(buffer+14, "\xDA\xDA", 2)==0)//����SAM2ˢ���貿��SAM1ˢ�Լ���˾��
// 			isSLZR = 0xDA;
// 		else
// 			isSLZR = 0;
////////////////////////////
		
#ifdef _debug_
		debugstring("ListSectBUff:");
		debugdata(buffer, 16, 1);
#endif
		memset((unsigned char*)&cardMessage.listSector, 0xff, 9);
		ii=0;
		for(i=0;i<16;i++)
		{
			switch(buffer[i])
			{
			case LIST_AREA:
				cardMessage.listSector=i;			
				break;
			case PUBLISH_AREA:
				cardMessage.publishSector=i;		
				break;
			case DEALRECORD_AREA:
				cardMessage.dealRecordSector[ii]=i;	
				ii++;
				break;
			case PBULICMESSAGE_AREA:
				cardMessage.publicMessageSector=i;	
				break;
			case ODDMONTH_AREA:
				cardMessage.oddMonthSector=i;		
				break;
			case DODMONTH_AREA:
				cardMessage.dodMonthSector=i;		
				break;
			case PUBLICMONEY_AREA:
				cardMessage.publicMoneySector=i;	
				break;
			default:
				break;
			}
		}
	}

	memcpy(cardMessage.PublishKEYA, cNo, 4);
	memcpy(cardMessage.PublishKEYA+4, cNo, 2);
	memcpy(cardMessage.PublichKEYA, cNo, 4);
	memcpy(cardMessage.PublichKEYA+4, cNo, 2);
	memcpy(cardMessage.MoneyKEYA, cNo, 4);
	memcpy(cardMessage.MoneyKEYA+4, cNo, 2);
	memcpy(cardMessage.MonthKEYA, cNo, 4);
	memcpy(cardMessage.MonthKEYA+4, cNo, 2);
	memcpy(cardMessage.RecodeKEYA, cNo, 4);
	memcpy(cardMessage.RecodeKEYA+4, cNo, 2);
	
#ifdef _debug_
	debugdata((unsigned char *)&c_serial, 4, 1);
	debugdata((unsigned char *)&cardMessage.publishSector, 1, 1);
	debugdata(cardMessage.PublishKEYA, 6, 1);
#endif
	if (MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,cardMessage.PublishKEYA,cardMessage.publishSector*4)==0)
	{
		debugstring("MifareAuthKey error\r\n ");
		return ST_ERROR; 
	}
//	debugstring("E ");
	
	if(MifareRead(cardMessage.publishSector*4,buffer)==0)
	{
		debugstring("read 4 error\r\n ");
		return ST_ERROR;
	}

#ifdef _debug_
	debugstring("1������Ϣ0��\r\n");
	debugdata(buffer,16,1);
#endif	
	if(MiBlockInvalid(1,15,buffer)==ST_ERROR) 
	{
		return ST_ERROR;
	}
	
	gMCardCand = CARDSTYLE_NORM1;
	pos=0;
	memcpy(cardMessage.CityCode, buffer+pos, CITY_CODE_LEN);			//���д���	2
	pos+=CITY_CODE_LEN;
	memcpy(cardMessage.TradeCode, buffer+pos, TRADE_CODE_LEN);			//��ҵ����	2
	pos+=TRADE_CODE_LEN;	
	memcpy((unsigned char *)cardMessage.PublishBicker, buffer+pos, PUBLISH_BICKER_LEN);	//����ˮ��	4	
	memcpy(cardMessage.CardCheckCode, buffer+8, CARD_CHECH_CODE_LEN);	//mac��		4
	cardMessage.card_catalog=buffer[13];								//����		1
#ifdef New_Times_Card__
	cardMessage.Zicard_catalog = buffer[14];
#else
	cardMessage.Zicard_catalog = 0;
#endif
	
	if(cardMessage.card_catalog ==CARD_WHITE_BUS)
	{
		return CARD_WHITE_BUS;
	}
	
#ifdef BUS_Cloud_
	if((mode == 0xA1)&&(cardMessage.card_catalog < 0x40)){//��������ͷ���, �����ƿ�����
		return CONTROL_CARD;
	}

	if(cardMessage.card_catalog == CARD_Business){
		if(MifareRead(cardMessage.publishSector*4+1,buffer)==0)//ȡĿ¼������λ��
		{
			return ST_ERROR;
		}
		
#ifdef _debug_
		debugstring("1������Ϣ1��\r\n");
		debugdata(buffer,16,1);
#endif	
		
		crc32 = Getcrc16(buffer, 8, 0xFF, 0xFF);
//		crc32 = __cpuCrc32(buffer, 8);
		if(memcmp((unsigned char*)&crc32, buffer+8, 4) != 0){
			debugstring("CRCerror:");
			debugdata((unsigned char*)&crc32, 4, 1);
			cardMessage.catalogPOST = 0;
			return ST_ERROR;
		}
// 		if(MiBlockInvalid(1,15,buffer)==ST_ERROR) 
// 		{
// 			cardMessage.catalogPOST = 0;
// 			return ST_ERROR;
// 		}
		cardMessage.catalogPOST = buffer[0];//дĿ¼
		return CONTROL_CARD;
	}
	if(cardMessage.card_catalog == CARD_FENDUAN_Line){
		return CONTROL_CARD;
	}
#endif //#ifdef BUS_Cloud_

#ifdef _debug_
	debugstring("card_catalog:");
	debugdata((unsigned char*)&cardMessage.card_catalog,1,1);
#endif	

	nnAudioflag = 0;

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

	if(MifareRead(cardMessage.publishSector*4+1, buffer)==0)//�������� ��Ч���� �������� ��Ѻ��	����	У��
	{
		return ST_ERROR;
	}

#ifdef _debug_
	debugstring("1������Ϣ1:");
	debugdata(buffer, 16, 1);
#endif
	
	if(MiBlockInvalid(1,15,buffer)==ST_ERROR) 
	{
#ifdef _debug_
		debugstring("1������Ϣ1 У���\r\n");
#endif
	
		return ST_ERROR;
	}
//ʱ�仺����0
	endTime = 0;
	startTime = 0;
	nowTime = 0;
	endTimeEn = 0;

	memcpy(cardMessage.stuffNO, buffer, 4);//Ա�����Ŀ���,��Ҫ��˾����ʹ��

	nnAudioflag = buffer[14];//�Ա�
	
#ifdef BUS_SHAODONG_	//�ܶ�ԭ��û������Ŀ�Ҫ�������󡣸������Ǻ󣬴˶δ�����Բ��á�
	if((buffer[4] == 0x20)&&(buffer[5] == 0x20)){
		if((cardMessage.card_catalog != CARD_NORMAL_BUS)&&
			(cardMessage.card_catalog != CARD_SD_YIXIN)&&
			(cardMessage.card_catalog != CARD_TELECOM_USER)&&
			(cardMessage.card_catalog != CARD_TELECOM_BUS)){
			memcpy(buffer+4, "\x20\x12\x09\x01", 4);
		}
	}
#endif

	memcpy(buffer+50, buffer+4, 4);
	buffer[52] = bcdDEC(buffer[52], 1);//��Ч����,��ǰһ������ʾ��
	if(buffer[52] == 0){
		buffer[52] = 0x12;
		buffer[51] = bcdDEC(buffer[51], 1);
	}

	memcpy((unsigned char*)&endTimeEn, buffer+50, 4);	//��Ч����,�����ġ�
	memcpy((unsigned char*)&endTime, buffer+4, 4);//��Ч���ڣ�����ǰ��
	memcpy((unsigned char*)&startTime, buffer+8, 4);//��������
	
	if(GetDateTime()==ST_ERROR) 
	{
		error(ERR_READ_TIME,0);
		return ST_ERROR;
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

#ifdef BUS_PRO_JSB
	if(cardMessage.card_catalog < 0x20){
#endif
// 		if(isSLZR == 0xDA){
// 			if(getMiKey_SLZR() == ST_ERROR){
// 				return ST_ERROR;
// 			}
// 		}
// 		else{
			if(getMiKey()==ST_ERROR)
			{
				return ST_ERROR;
			}
//		}
#ifdef BUS_PRO_JSB
	}
	else if((cardMessage.card_catalog>=61)&&(cardMessage.card_catalog<74))
	{
#if defined BUS_JIUJIANG_ || defined BUS_HANDAN_
		if(getMiKey()==ST_ERROR)	//�Ž������ڵĿ��ƿ�Ҳȫ��ȡ��Կ
		{
			return ST_ERROR;
		}
#endif
		if(IsInBlackList(cardMessage.PublishBicker) < FLASH_BLK_END){
			return CARD_BLACK_PRO;
		}
// 		return	CONTROL_CARD;
	}
	else
	{
		return CARD_NO_SYSTEM;
	}
#endif
		gucRestorFlag = 0;

#ifdef BUS_Cloud_
		if(cardMessage.card_catalog == CARD_KEY_BUS)
			return ST_OK;
#endif

		if(cmpBussineesNo() == ST_ERROR){
			return CARD_NO_Busins;
		}

#ifdef Card_typeM_Min_is0_
		if(IsInBlackList(cardMessage.PublishBicker) < FLASH_BLK_END){
			return CARD_BLACK_PRO;
		}

		if((cardMessage.card_catalog>=61)&&(cardMessage.card_catalog<74))
		{
			return	CONTROL_CARD;
		}
		else if(cardMessage.card_catalog >= 0x20)
		{
			return CARD_NO_SYSTEM;
		}
#endif	//#ifdef BUS_BAOJI_

#ifdef _debug_
		debugstring("Oper ---------\r\n");
		debugdata((unsigned char*)&pos, 1, 1);
#endif
		pos=OperPublicMessage(); 
#ifdef _debug_
		debugstring("OperPublicMessage pos:");
		debugdata((unsigned char*)&pos, 1, 1);
#endif
		getCpuInfo((stCpuInfo*)buffer); //�ڶ�����ʱ����	
		if(pos==CARD_PUBLIC_BAD) {
			return CARD_PUBLIC_BAD;
		}
		else if(pos==CARD_BLACK)
		{
			return CARD_BLACK;	
		}
		else if(pos == CARD_CHAGEERR){
			return CARD_CHAGEERR;
		}
		else if(pos==ST_ERROR) {
#ifdef BUS_PRO_JSB
			if(cardMessage.card_catalog<0x20){//�û������ش��󣬲����û���������
				return ST_ERROR;
			}
#endif
		}
#ifdef BUS_PRO_JSB
		if(cardMessage.card_catalog<0x20)
		{
			if(IsInBlackList(cardMessage.PublishBicker) < FLASH_BLK_END){
				return CARD_BLACK_PRO;
			}
			if(NewPriceTab.rate[cardMessage.card_catalog] == 0){//�ۿ�Ϊ0����������
				return ST_OK;
			}
			if(cardMessage.card_catalog!=CARD_NORMAL)
			{
#ifdef BUS_JIUJIANG_
				if(endTime<nowTime)
#else
				if((startTime>nowTime)||(endTime<nowTime)||(startTime>endTime))
#endif
				{
					return CARD_NO_TIME;
				} 
			}
			if(NewPriceTab.rate[cardMessage.card_catalog]==202)//�������
			{
				return MONTH_CARD;	
			}
			else if((NewPriceTab.rate[cardMessage.card_catalog] == 104)||(NewPriceTab.rate[cardMessage.card_catalog] == 204)){
				return CARD_forbid;//�˿�����ֹʹ��
			}
			if(cardMessage.oddMonthSector!=0xff)//��Ʊ
			{
				return MONTH_CARD;		   
			}
			else 
			{
				return CARD_MONEY;			//Ǯ��
			}
		}
		else {
			return	CONTROL_CARD;
		}
#elif defined Card_typeM_Min_is0_ //�����ж��ݡ�����
		if(cardMessage.card_catalog<0x20)
		{
#ifdef BUS_EZOU_	//��ͨ����ѧ����������
			if((cardMessage.card_catalog!=CARD_NORMAL_BUS)&&(cardMessage.card_catalog!=CARD_STUDENT_BUS))//
#else
			if(cardMessage.card_catalog!=CARD_NORMAL_BUS)//
#endif
			{
				// �����������ʱ���POS��ʱ��������ж���Ч�� 2010.03.11
				if((chekMdate(startTime)==ST_OK)&&(chekMdate(endTime)==ST_OK)&&(chekMdate(nowTime)==ST_OK)){
					if((endTime<nowTime)||(startTime>nowTime))
						return CARD_NO_TIME;
				}
			}

			if(cardMessage.card_catalog == CARD_OLDMAN_BUS){//���꿨�������꿨
				return CARD_YEAR;//��Ʊ
			}

			if(NewPriceTab.rate[cardMessage.card_catalog]==202)//�������
			{
				return MONTH_CARD;	
			}
			else if(NewPriceTab.rate[cardMessage.card_catalog] == 203){//����ʹ�ô���
				return CARD_MONEY;			//Ǯ��
			}
			else if(NewPriceTab.rate[cardMessage.card_catalog]==204){//��Ʊ
				return CARD_YEAR;
			}
			else if(NewPriceTab.rate[cardMessage.card_catalog] == 207){//������ʹ��
				return CARD_forbid;
			}
			if(cardMessage.oddMonthSector!=0xff)//��Ʊ
			{
				return MONTH_CARD;		   
			}
			else 
			{
				return CARD_MONEY;			//Ǯ��
			}
		}
		else 
			return INVALID_CARD;
#else  //#ifdef BUS_PRO_JSB
		if(IsInBlackList(cardMessage.PublishBicker) < FLASH_BLK_END){
			return CARD_BLACK_PRO;
		}
		if(cardMessage.card_catalog>=0x40)
		{
#ifdef BUS_YICHUN	//�˴��� CARD_COUPON_BUS ��ΪԱ��˾����
			if(cardMessage.card_catalog == CARD_COUPON_BUS){
				if(DriverCardDeal() == ST_OK)
					return ST_OK;//Ա��˾����ǩ����ǩ��������
			}//����ǩ����ǩ�ˡ�����Ʊ���ۿ��ʿۣ�
#endif
			gucEndTimeFlag = 0;
			if(cardMessage.card_catalog!=CARD_NORMAL)//
			{
				if((startTime>nowTime)||(endTime<nowTime)||(startTime>endTime)){
					return CARD_NO_TIME;
				}
				else if(endTimeEn<=nowTime){//��Ҫ��ʾ���������ˡ�
					gucEndTimeFlag = 1;
				}
			}
			if((NewPriceTab.rate[cardMessage.card_catalog-0x40] == 103)||(NewPriceTab.rate[cardMessage.card_catalog-0x40] == 203)){
				return CARD_MONEY; //�˿����ܿ۴Σ�ֻ�ܿ�Ǯ
			}
			if((NewPriceTab.rate[cardMessage.card_catalog-0x40] == 104)||(NewPriceTab.rate[cardMessage.card_catalog-0x40] == 204)){
				return CARD_forbid;//�˿�����ֹʹ��
			}
			if(NewPriceTab.rate[cardMessage.card_catalog-0x40]<=100)
			{
				return CARD_MONEY;				//�ֽ�
			}
			if((NewPriceTab.rate[cardMessage.card_catalog-0x40]==101)||(NewPriceTab.rate[cardMessage.card_catalog-0x40]==201))
			{
				return MONTH_CARD;//�۴κ���Ҫ��Ǯ�����ӷѣ�����Ʊ����101Ҫ��һ�θ��ӷѣ�110���Ӹ��ӷѣ�111��һ�Σ�112�����Σ����ε�119.
			}
			if((NewPriceTab.rate[cardMessage.card_catalog-0x40]>=110) && (NewPriceTab.rate[cardMessage.card_catalog-0x40]<=119))
			{
				return MONTH_CARD;//�۴κ���Ҫ�۸��ӷѣ�����Ʊ����101Ҫ��һ�θ��ӷѣ�110���Ӹ��ӷѣ�111��һ�Σ�112�����Σ����ε�119.
			}
			if((NewPriceTab.rate[cardMessage.card_catalog-0x40] ==102)||(NewPriceTab.rate[cardMessage.card_catalog-0x40] == 202))	
			{
				return CARD_STUFF_BUS;//Ա����
			}
			return ST_ERROR;
		}
		else {
			return CONTROL_CARD;
		}
#endif  //#ifdef BUS_PRO_JSB
}



