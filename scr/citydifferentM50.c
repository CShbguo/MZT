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

//此版本增加MC65 TTS语音到内置喇叭。

//J4 通道2输出到内置喇叭。
extern void M50Audosend(unsigned int uiLen, char *str);

//有些卡需要区分男女声
extern unsigned char nnAudioflag;
extern unsigned char gchn_eng;//中英文标志
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
#ifdef COM_TEST_		//使用语音功能
#define ComTst_head 0
#define ComTst_LenH 4
#define ComTst_LenL 5
#define ComTst_CMD 6
#define ComTst_PackNo 7
#define ComTst_Data 8

volatile unsigned char ComTest_index=0;
volatile unsigned char ComTest_Ccount;

//unsigned char GPSPCibuff[100];  使用irda_rxBuf
void ComTest_DealCMD(void)
{
//	unsigned char buff[32];
	unsigned short dlen;
	
	dlen = irda_rxBuf[ComTst_LenH];
	dlen <<= 8;
	dlen += irda_rxBuf[ComTst_LenL];
	irda_rxBuf[dlen-2] = 0;//结束符
	if(irda_rxBuf[ComTst_CMD] == 0xA1){//语音
#ifdef _TTSSound_enable_
		SoundFreeManage((char*)irda_rxBuf+ComTst_Data);//报语音
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
	case 4://长度高位
	case 5://长底低位
		irda_rxBuf[ComTest_index++] = ii;
		memcpy((unsigned char*)&plen, irda_rxBuf+4, 2);
		over_turn(2, (unsigned char*)&plen);
		if(plen > 2048){//长度错误
			ComTest_index = 0;
			memset(irda_rxBuf,0,10);
			break;
		}
		break;
	default:
		irda_rxBuf[ComTest_index++] = ii;
		memcpy((unsigned char*)&plen, irda_rxBuf+4, 2);
		over_turn(2, (unsigned char*)&plen);
		if(plen > 2048){//长度错误
			ComTest_index = 0;
			memset(irda_rxBuf,0,10);
			break;
		}
		if(ComTest_index >= plen){//数据接收完成
#ifdef _debug_
			debugstring("收:");
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
//输入整型数字
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
	
	display(6,2,"请输入数值",DIS_CENTER);
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

//获取不能连续刷卡延时时间
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
#elif defined BUS_DONGXIANG_	//东乡的老年卡20分钟后才能刷
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
	display(0,0,"设置刷卡延迟", DIS_CENTER|DIS_CONVERT);
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
//----------是否能连刷处理------------------
extern void save_h_month(void);
extern void save_normal(void);
void Save_delayCardinfo(unsigned char Cmode)
{
#ifdef MOBILE_SHIYAN_
	if((cardMessage.card_catalog!=CARD_NORMAL)&&(cardMessage.card_catalog!=CARD_COUPON))
		save_h_month();
	else
		save_normal();
#elif defined BUS_FENYI_     //2011.04.10_LWX 分宜学生卡可以连续刷        
	if((cardMessage.card_catalog!=CARD_NORMAL)&&(cardMessage.card_catalog!=CARD_STUDENT_BUS))
		save_h_month();
	else
		save_normal();
#elif defined BUS_SHAODONG_     //2011.05.05_LWX 邵东电信卡可以连续刷        
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
#elif defined BUS_DONGXIANG_	//老年卡20分钟内不能刷，学生卡可以连刷
	if((cardMessage.card_catalog!=CARD_NORMAL)&&(cardMessage.card_catalog != CARD_STUDENT_BUS)&&
		(cardMessage.card_catalog != CARD_YOUHUI_BUS)&&(cardMessage.card_catalog != CARD_YOUDAI_BUS))
		save_h_month();
	else
		save_normal();
#elif defined BUS_LAIZOUAT_	//所有的卡都可以连刷
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
#elif defined BUS_NanjingDZ_	//修水的所有卡都不能连刷
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
#elif defined BUS_LICHUANG_	//学生卡可以连刷
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

//语音提示函数
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
			audio(Audio_DONG);//扣钱报咚
		else
			SoundMessage(SOUND_DEAL);
	}
	else
		SoundMessage(SOUND_CHARGE_MONEY);	//提示请充值
#elif defined BUS_HANDAN_
	if(gucSoudnorm == 0){
		if(sMode != 0)
			audio(Audio_DONG);//是月票扣钱按普通卡音
		else
			SoundMessage(SOUND_DEAL);
	}
	else
		SoundMessage(SOUND_CHARGE_MONEY);	//提示请充值
#elif defined BUS_YICHUN //-----------------------------------
	if(sMode != 0){//从扣月票过来的
		audio(Audio_DONG);
	}
	else
		SoundMessage(SOUND_DEAL);
	if(gucSoudnorm == 1){//还需要提示请充值
		delayxms(600);
		SoundMessage(SOUND_CHARGE_MONEY);
	}
#elif defined BUS_JIUJIANG_	|| defined BUS_LICHUANG_//提示请充值
	if(gucSoudnorm == 1){//还需要提示请充值
		SoundMessage(SOUND_CHARGE_MONEY);
	}
	else{
		SoundMessage(SOUND_DEAL);
	}
#else
//	SoundMessage(0x80|SOUND_DEAL);	//通过TTS
	SoundMessage(SOUND_DEAL);

#endif
}

//----------语音----------------------------

//跟据语音编号取实际语音编号
unsigned char getVoidIndex(unsigned char vall)
{
	unsigned ret;
	switch(vall){
	case 0:
#ifdef BUS_DONGXIANG_
		ret = Audio_PuTong;	//东乡要报普通卡
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
void setAudioSHIYAN(void)//十堰
{
	//	0 普通卡	1 学生卡	2 老年卡	3 员工卡	5 成人市月票卡   6 爱心卡
	memset(NewPriceTab.voice,Audio_DONG,32);
	NewPriceTab.voice[CARD_NORMAL] = Audio_DONG;
	NewPriceTab.voice[CARD_STUDENT] = Audio_STUDENT;
	NewPriceTab.voice[CARD_OLDMAN] = Audio_OLDMAN;
	NewPriceTab.voice[CARD_STUFF] = Audio_STUFF;
	NewPriceTab.voice[CARD_HANDICAPPED] = Audio_LOVER;
}
#elif defined BUS_PINGXIANG_
void setAudioPINGXIANG(void)//萍乡
{
	//	0 普通卡	1 学生卡	2 老年卡	3 员工卡	5 成人市月票卡   6 爱心卡
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
void setAudioHANDAN(void)//邯郸
{
	//	0 普通卡	1 学生卡	2 老年卡	3 员工卡	5 成人市月票卡   6 普通B卡
	memset(NewPriceTab.voice,Audio_DONG,32);
	NewPriceTab.voice[CARD_NORMAL] = Audio_DONG;
	NewPriceTab.voice[CARD_STUDENT] = Audio_STUDENT;
	NewPriceTab.voice[CARD_OLDMAN] = Audio_BIRTHDAY;
	NewPriceTab.voice[CARD_STUFF] = Audio_STUFF;
	NewPriceTab.voice[CARD_COUPON] = Audio_MONTH;
	NewPriceTab.voice[CARD_RELATION_BUS] = Audio_MONTH;
	NewPriceTab.voice[CARD_HANDICAPPED] = Audio_DONG;
	NewPriceTab.voice[CARD_CAP_BUS] = Audio_DONG;		//联通手机卡
	NewPriceTab.voice[CARD_JILIAN_BUS] = Audio_DONG;		//建行卡
}
#elif defined BUS_JIUJIANG_
void setAudioJIUJIANG(void)//九江
{
	memset(NewPriceTab.voice,Audio_DONG,32);
	NewPriceTab.voice[CARD_NORMAL] = Audio_DONG;
	NewPriceTab.voice[CARD_STUDENT] = Audio_YWJYu; //义务教育卡
	NewPriceTab.voice[CARD_OLDMAN] = Audio_OLDMAN;
	NewPriceTab.voice[CARD_STUFF] = Audio_STUFF;
	NewPriceTab.voice[CARD_COUPON] = Audio_STUDENT;//九江
	NewPriceTab.voice[CARD_ZILU_BUS] = Audio_DONG;
}
#elif defined BUS_SHAODONG_
void setAudioSHAODONG(void)//邵东
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

	NewPriceTab.voice[CARD_CONGCI_BUS-0x40]=Audio_STUDENT; //邵东异型学生卡
	NewPriceTab.voice[CARD_SD_YIXIN-0x40]=Audio_PuTong;
}
#elif defined BUS_HEZHOU_
void setAudioHEZHOU(void)//贺州 老语音
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
	NewPriceTab.voice[CARD_NORMAL_BUS-0x40] = Audio_DONG;		//A 卡
	NewPriceTab.voice[CARD_STUDENT_BUS-0x40] = Audio_STUDENT;	//C卡报"嘀学生卡
	NewPriceTab.voice[CARD_OLDMAN_BUS-0x40] = Audio_FULI;		//E卡报"嘀福利卡"
	NewPriceTab.voice[CARD_ZILU_BUS-0x40]=Audio_MONTH;		//B 卡报"嘀月票卡"
	NewPriceTab.voice[CARD_STUFFC_BUS-0x40]=Audio_LOVER;		//F 季票卡报"长嘀加短嘀"
	NewPriceTab.voice[CARD_STUFF_BUS - 0x40] = Audio_STUFF;	//D卡报"嘀员工卡
	
	//男声
	NewPriceTab.voice[CARD_STUDENT_BUS+20-0x40] = Audio_MAN_STUDENT;	//C卡报"嘀学生卡
	NewPriceTab.voice[CARD_OLDMAN_BUS+20-0x40] = Audio_Man_FULI;		//E卡报"嘀福利卡"
	NewPriceTab.voice[CARD_ZILU_BUS+20-0x40]=Audio_MAN_Month;		//B 卡报"嘀月票卡"
	NewPriceTab.voice[CARD_STUFF_BUS+20 - 0x40] = Audio_Man_STUFF;

// 	NewPriceTab.voice[CARD_COUPON_BUS-0x40] = Audio_LOVER;
// 	NewPriceTab.voice[CARD_STUFF_BUS-0x40] = Audio_STUFF;
// 	NewPriceTab.voice[CARD_RELATION_BUS-0x40] = Audio_DONG;//这卡响两声
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
	//	0 普通卡	1 学生卡	2 老年卡	3 员工卡	4\5 月票卡   6 爱心卡-残疾卡
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
//没有，用九江的
void setAudioJSB(void)
{
	memset(NewPriceTab.voice,Audio_DONG,32);
	NewPriceTab.voice[CARD_NORMAL] = Audio_DONG;
	NewPriceTab.voice[CARD_STUDENT] = Audio_STUDENT; //义务教育卡
	NewPriceTab.voice[CARD_OLDMAN] = Audio_OLDMAN;
	NewPriceTab.voice[CARD_STUFF] = Audio_STUFF;
	NewPriceTab.voice[CARD_COUPON] = Audio_STUDENT;//九江
	NewPriceTab.voice[CARD_ZILU_BUS] = Audio_MONTH;
	NewPriceTab.voice[CARD_HANDICAPPED] = Audio_YH;
}
#elif defined BUS_BAOJI_
void SetAudioBAOJI(void)
{
	// #define CARD_NORMAL_BUS		0				//普通卡
	// #define CARD_STUDENT_BUS		1				//学生卡
	// #define CARD_SX_BUS			2				//寿星卡
	// #define CARD_STUFF_BUS		3				//员工卡
	// #define CARD_YH_BUS			4				//优惠卡
	// #define CARD_YF_BUS			5				//优扶卡
	// #define CARD_OLDMAN_BUS		6				//老年卡
	//#define CARD_Love_BUS		7				//爱心卡
	//#define CARD_ZILV_BUS		8				//子女卡
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
	NewPriceTab.voice[CARD_RELATION_BUS-nj] = Audio_DONG;//这卡响两声
	NewPriceTab.voice[CARD_YOUHUI_BUS-nj]=Audio_YH;
}
#else
void setAudiocommon(void)//通用
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
	NewPriceTab.voice[CARD_RELATION_BUS-nj] = Audio_DONG;//这卡响两声
	NewPriceTab.voice[CARD_YOUHUI_BUS-nj]=Audio_YH;
#endif
//	NewPriceTab.voice[CARD_HETONG_BUS-nj]=Audio_STUFF;
//	NewPriceTab.voice[CARD_JILIAN_BUS-nj]=Audio_DONG;
}
#endif
void SetAudio(void)//建设部:十堰,
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

//cmd 最高位为1，则使用TTS语音。
void SoundMsgCommon(unsigned char cmd)
{
	unsigned char css;
	unsigned char TTsflag=0;
	const char TTSList[34][11]={
"普通卡\0", "普通卡\0", "学生卡\0", "优待卡\0",  "月票卡\0",  "季票卡\0",  "老年卡\0",  "残优卡\0",  "员工卡\0",  "优惠卡\0",  
"家属卡\0", "纪念卡\0", "残疾人卡\0", "员工卡\0",  "学员卡\0",  "稽查卡\0",  "优待卡\0",  "优待卡\0",  "优待卡\0",  "优待卡\0", 
"优待卡\0", "优待卡\0", "优待卡\0", "优待卡\0",  "优待卡\0",  "优待卡\0",  "优待卡\0",  "优待卡\0",  "优待卡\0",  "优待卡\0",  
"优待卡\0",  "优待卡\0",  "优待卡\0", "\0"
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
	if(gchn_eng == 'E'){//还没有英文语音
		if((cmd != SOUND_DEAL)&&(cmd != SOUND_DI_BUS))
			cmd = ERR_NO_PSAM;
	}
	TTsflag = (cmd&0x80);
	cmd &= 0x7F;
	switch(cmd)
	{
	case INVALID_CARD:
		audio(Audio_INVALID);	//非法卡
		break;
	case SOUND_FLING_MONEY:
		audio(Audio_FLING_MONEY);	//请投币
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
			audio(NewPriceTab.voice[css]);	//正常交易
		beep(1,100,50);
		break;
	case SOUND_DI_BUS:			//
		beep(1,100,50);
		break;
	case ERR_NO_PSAM:
		beep(8,400,40);		//PSAM卡检测错误
		break;
	default:
		break;
	}
}

//cmd 最高位为1，则使用TTS语音。
void SoundMessage(unsigned char cmd)
{
	SoundMsgCommon(cmd);
}

#ifdef MOBILE_SHIYAN_
int getCardtypeShiyan(char *cardD, unsigned char type)
{
	if(type == CARD_NORMAL){
		strcpy(cardD, "普通卡");
	}
	else if(type == CARD_STUDENT){
		strcpy(cardD, "学生卡");
	}
	else if(type == CARD_COUPON){
		strcpy(cardD, "优惠卡");
	}
	else if(type == CARD_OLDMAN){
		strcpy(cardD, "老年卡");
	}
	else if(type == CARD_RELATION_BUS){
		strcpy(cardD, "家属卡");
	}
	else if(type == CARD_STUFF){
		strcpy(cardD, "员工卡");
	}
	else if(type == CARD_DRIVER_BUS){
		strcpy(cardD, "司机卡");
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
		strcpy(cardD, "普通卡");
	}
	else if(type == CARD_STUDENT){
		strcpy(cardD, "义务卡");
	}
	else if(type == CARD_HANDICAPPED){
		strcpy(cardD, "低保卡");
	}
	else if(type == CARD_COUPON){
		strcpy(cardD, "学生卡");
	}
	else if(type == CARD_CAP_BUS){
		strcpy(cardD, "执勤卡");
	}
	else if(type == CARD_OLDMAN){
		strcpy(cardD, "老年卡");
	}
	else if(type == CARD_JILIAN_BUS){
		strcpy(cardD, "月票卡");
	}
	else if(type == CARD_ZILU_BUS){
		strcpy(cardD, "银行卡");
	}
	else if(type == CARD_17RD_BUS){
		strcpy(cardD, "17路卡");
	}
	else if(type == CARD_RELATION_BUS){
		strcpy(cardD, "家属卡");
	}
	else if(type == CARD_NORMAL_B){
		strcpy(cardD, "普通B ");
	}
	else if(type == CARD_STUFF){
		strcpy(cardD, "员工卡");
	}
	else if(type == CARD_DRIVER_BUS){
		strcpy(cardD, "司机卡");
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
		strcpy(cardD, "普通卡");
	}
	else if(type == CARD_STUDENT){
		strcpy(cardD, "学生卡");
	}
	else if(type == CARD_COUPON){
		strcpy(cardD, "优惠卡");
	}
	else if(type == CARD_OLDMAN){
		strcpy(cardD, "老年卡");
	}
	else if(type == CARD_RELATION_BUS){
		strcpy(cardD, "家属卡");
	}
	else if(type == CARD_STUFF){
		strcpy(cardD, "员工卡");
	}
	else if(type == CARD_DRIVER_BUS){
		strcpy(cardD, "司机卡");
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
		strcpy(cardD, "普通卡");
	}
	else if(type == CARD_STUDENT){
		strcpy(cardD, "学生卡");
	}
	else if(type == CARD_COUPON){
		strcpy(cardD, "月票卡");
	}
	else if(type == CARD_OLDMAN){
		strcpy(cardD, "敬老卡");
	}
	else if(type == CARD_HANDICAPPED){
		strcpy(cardD, "普通Ｂ");
	}
	else if(type == CARD_RELATION_BUS){
		strcpy(cardD, "月票卡");
	}
	else if(type == CARD_CAP_BUS){
		strcpy(cardD, "联通卡");
	}
	else if(type == CARD_JILIAN_BUS){
		strcpy(cardD, "建行卡");
	}
	else if(type == CARD_STUFF){
		strcpy(cardD, "员工卡");
	}
	else if(type == CARD_DRIVER_BUS){
		strcpy(cardD, "司机卡");
	}
	else{
		sprintf(cardD, "%d号卡", type);
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
		strcpy(cardD, "普通卡");
		break;
	case CARD_STUDENT_BUS:
		strcpy(cardD, "学生卡");
		break;
	case 4:
		strcpy(cardD, "月票卡");
		break;
	case 5:
		strcpy(cardD, "月票卡");
		break;
	case 6:
		strcpy(cardD, "优扶卡");
		break;
	case 8:
		strcpy(cardD, "大众卡");
		break;
	case 9:
		strcpy(cardD, "校园卡");
		break;
	case CARD_STUFF_BUS:
		strcpy(cardD, "员工卡");
		break;
	case CARD_DRIVER_BUS:
		strcpy(cardD, "司机卡");
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
		strcpy(cardD, "普通卡");
	}
	else if(type == CARD_STUDENT_BUS){
		strcpy(cardD, "学生卡");
	}
	else if(type == CARD_ZILU_BUS){
		strcpy(cardD, "月票卡");
	}
	else if(type == CARD_OLDMAN_BUS){
		strcpy(cardD, "福利卡");
	}
	else if(type == CARD_STUFFC_BUS){
		strcpy(cardD, "爱心卡");//季票卡
	}
	else if(type == CARD_STUFF_BUS){
		strcpy(cardD, "员工卡");
	}	
	else if(type == CARD_COUPON_BUS){
		strcpy(cardD, "员工卡");
	}
	else if(type == CARD_DRIVER_BUS){
		strcpy(cardD, "司机卡");
	}
	else if(type == CARD_MOBILE_BUS){
		strcpy(cardD, "手机卡");
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
		strcpy(cardD, "普通卡");
	}
	else if(type == CARD_STUDENT){
		strcpy(cardD, "义务卡");
	}
	else if(type == CARD_HANDICAPPED){
		strcpy(cardD, "优惠卡");
	}
	else if(type == CARD_COUPON){
		strcpy(cardD, "学生卡");
	}
	else if(type == CARD_CAP_BUS){
		strcpy(cardD, "执勤卡");
	}
	else if(type == CARD_OLDMAN){
		strcpy(cardD, "老年卡");
	}
	else if(type == CARD_JILIAN_BUS){
		strcpy(cardD, "月票卡");
	}
	else if(type == CARD_ZILU_BUS){
		strcpy(cardD, "卡点卡");
	}
	else if(type == CARD_17RD_BUS){
		strcpy(cardD, "17路卡");
	}
	else if(type == CARD_RELATION_BUS){
		strcpy(cardD, "家属卡");
	}
	else if(type == CARD_STUFF){
		strcpy(cardD, "员工卡");
	}
	else if(type == CARD_DRIVER_BUS){
		strcpy(cardD, "司机卡");
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
		strcpy(cardD, "普通卡");
	}
	else if(type == CARD_STUDENT_BUS){
		strcpy(cardD, "学生卡");
	}
	else if(type == CARD_SX_BUS){
		strcpy(cardD, "寿星卡");
	}
	else if(type == CARD_STUFF_BUS){
		strcpy(cardD, "员工卡");
	}
	else if(type == CARD_YH_BUS){
		strcpy(cardD, "优惠卡");
	}
	else if(type == CARD_YF_BUS){
		strcpy(cardD, "优待卡");
	}
	else if(type == CARD_OLDMAN_BUS){
		strcpy(cardD, "老年卡");
	}
	else if(type == CARD_Love_BUS){
		strcpy(cardD, "爱心卡");
	}
	else if(type == CARD_ZILV_BUS){
		strcpy(cardD, "子女卡");
	}
	else if(type == CARD_DRIVER_BUS){
		strcpy(cardD, "司机卡");
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
		strcpy(cardD, "普通卡");
	}
	else if(type == CARD_STUDENT_BUS){
		strcpy(cardD, "学生卡");
	}
	else if(type == CARD_OLDMAN_BUS){
		strcpy(cardD, "老年卡");
	}
	else if(type == CARD_STUFFC_BUS){
		strcpy(cardD, "家属卡");
	}
	else if(type == CARD_DRIVER_BUS){
		strcpy(cardD, "司机卡");
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
	sprintf(cardD, "%d类卡", type);
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
		strcpy(cardD, "联名卡");
#else
		strcpy(cardD, "月票卡");
#endif
	}
#endif
	else if(type == CARD_OLDMAN_BUS){
		strcpy(cardD, Card_Name_OLDMAN);
	}
//	else if(type == CARD_ZUL_BUS){
//		strcpy(cardD, "爱心卡");季票卡
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
		strcpy(cardD, "手机卡");
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
{//优惠卡显示
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

	F_FLAG_VARY.stop_flag = 0;//一直是消费状态

// 	11扇区(发行区)密码A：物理卡号+"0337"    密码B：物理卡号+"0755"
// 	12、13、14、15扇区(12钱包区，13扇区为公共信息区，14、15月票区)密码A：物理卡号与密码因子前4字节异或+物理卡前2字节    密码B：物理卡号与密码因子后4字节异或+物理卡前2字节
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
	debugstring("1扇区信息0：\r\n");
	debugdata(buffer,16,1);
#endif	
	if(MiBlockInvalid(1,15,buffer)==ST_ERROR) 
	{
		return ST_ERROR;
	}
	
	gMCardCand = CARDSTYLE_ANLONG;
	pos=0;
	memcpy(cardMessage.CityCode, buffer+pos, CITY_CODE_LEN);			//城市代码	2
	pos+=CITY_CODE_LEN;
	memcpy(cardMessage.TradeCode, buffer+pos, TRADE_CODE_LEN);			//行业代码	2
	pos+=TRADE_CODE_LEN;	
	memcpy((unsigned char *)cardMessage.PublishBicker, buffer+pos, PUBLISH_BICKER_LEN);	//卡流水号	4	
	memcpy(cardMessage.CardCheckCode, buffer+8, CARD_CHECH_CODE_LEN);	//mac码		4
	cardMessage.card_catalog=buffer[13];								//卡类		1
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

	if(MifareRead(cardMessage.publishSector*4+1, buffer)==0)//发行日期 有效日期 启用日期 卡押金	保留	校验
	{
		return ST_ERROR;
	}

#ifdef _debug_
	debugstring("1扇区信息1:");
	debugdata(buffer, 16, 1);
#endif
	
	if(MiBlockInvalid(1,15,buffer)==ST_ERROR) 
	{
#ifdef _debug_
		debugstring("1扇区信息1 校验错\r\n");
#endif
	
		return ST_ERROR;
	}
//时间缓冲清0
	endTime = 0;
	startTime = 0;
	nowTime = 0;
	endTimeEn = 0;

	memcpy(cardMessage.stuffNO, buffer, 4);//员工卡的卡号,主要是司机卡使用

	nnAudioflag = buffer[14];//性别
	
#ifdef BUS_SHAODONG_	//绍东原来没有年审的卡要加入年审。给了他们后，此段代码可以不用。
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
	buffer[52] = bcdDEC(buffer[52], 1);//有效日期,提前一个月提示。
	if(buffer[52] == 0){
		buffer[52] = 0x12;
		buffer[51] = bcdDEC(buffer[51], 1);
	}

	memcpy((unsigned char*)&endTimeEn, buffer+50, 4);	//有效日期,处理后的。
	memcpy((unsigned char*)&endTime, buffer+4, 4);//有效日期，处理前的
	memcpy((unsigned char*)&startTime, buffer+8, 4);//启动日期
	
	if(GetDateTime()==ST_ERROR) 
	{
		error(ERR_READ_TIME,0);
		return ST_ERROR;
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
	getCpuInfo((stCpuInfo*)buffer); //在读到卡时更新	
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
			else if(endTimeEn<=nowTime){//需要提示“请年审了”
				gucEndTimeFlag = 1;
			}
		}
		if((NewPriceTab.rate[cardMessage.card_catalog-0x40] == 103)||(NewPriceTab.rate[cardMessage.card_catalog-0x40] == 203)){
			return CARD_MONEY; //此卡不能扣次，只能扣钱
		}
		if((NewPriceTab.rate[cardMessage.card_catalog-0x40] == 104)||(NewPriceTab.rate[cardMessage.card_catalog-0x40] == 204)){
			return CARD_forbid;//此卡被禁止使用
		}
		if(NewPriceTab.rate[cardMessage.card_catalog-0x40]<=100)
		{
			return CARD_MONEY;				//现金卡
		}
		if((NewPriceTab.rate[cardMessage.card_catalog-0x40]==101)||(NewPriceTab.rate[cardMessage.card_catalog-0x40]==201))
		{
			return MONTH_CARD;//扣次后不需要扣附加费，纯月票卡；
		}
		if((NewPriceTab.rate[cardMessage.card_catalog-0x40] ==102)||(NewPriceTab.rate[cardMessage.card_catalog-0x40] == 202))	
		{
			return CARD_STUFF_BUS;//员工卡
		}
		return ST_ERROR;
	}
	else {
		return CONTROL_CARD;
	}
}
#endif

//判断整型数时的日期是否正确 2010.03.11
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
	memcpy(dis, "卡号:", 5);
	memcpy(dis+5, rRecordRS232P->rCardID, 16);
	display(0,0,dis, 0);
	memset(dis, 0, 32);
	memcpy(dis, "姓名:", 5);
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
	memcpy(dis[1], "姓名:", 5);
	memcpy(dis[2], "卡号:", 5);
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
	display(0,0,"刷卡成功:",0);
	display(2, 0, dis[1], 0);
	display(4, 0, dis[2], 0);
	miniDispstr(6, 0, dis[0], 0);
	audio(Audio_SuaKaOK);
		
	memset((unsigned char*)&rRecordRS232, 0, sizeof(DEALRECODE_CS456_ASCii));
	Get_SerialNum(rRecordRS232.rDeviceNo);//设备唯一号
	rRecordRS232.rDealType = ID_REC_CS456_ASCii;
	memcpy(rRecordRS232.rDealTime, (unsigned char*)&SysTime, 7);//交易日期时间
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
	return getbussinessNO(dat);//两字节商户编号
#endif
}

//比较是否是同一个商户的卡,只有宜宾使用了
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
	memcpy(buffer, cNo, 4);//0扇区默认密钥
	memcpy(buffer+4, cNo, 2);//0扇区默认密钥
#else //#ifdef BUS_Cloud_
	memcpy(cNo, (unsigned char *)&c_serial, 4);
	cardMessage.listSector = 0;
	memcpy(buffer, "\xa0\xa1\xa2\xa3\xa4\xa5", 6);//0扇区默认密钥
#endif

	flag = 0xff;
	cardMessage.publishSector = 1;//the bussiness  card used
	for(i=0; i<2; i++){
#ifdef _debug_
		debugstring("card type proce:");
		debugdata((unsigned char*)&cardMessage.listSector, 1, 1);
		debugdata(buffer, 6, 1);
#endif
		pos = ((cardMessage.listSector*4)+1);//读目录区第一块。
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
			if(memcmp(buffer, "\xAA\xAA\xAA\xAA\xAA\xAA\xAA\xAA", 8) == 0){//用于考勤，是这张卡
				CS456_ASDisplay();
				return ST_ERROR;
			}
#endif
			flag = 0;
			break;
		}
#ifdef _YiDa_Card_
		if(buffer[0] != 0xA0){ //用A0密钥认证，宜春卡0扇区的密钥。
			memcpy(buffer, "\xa0\xa1\xa2\xa3\xa4\xa5", 6);//0扇区默认密钥
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
	else{//读1块正确
#ifdef _YiDa_Card_ //判断是不是艺达的卡
		ii = CRC8_Tab(buffer, 15);
#ifdef _debug_
		debugstring("0扇区1信息:");
		debugdata(buffer,16,1);
		debugstring("CRC:");
		debugdata((unsigned char*)&ii,1,1);
#endif
		if(((buffer[14] == 0xAA)||(buffer[14] == 0xBB))&& \
			((buffer[0] == 0x50)||(buffer[0] == 0x51)||(buffer[0] == 0x52)))
		{
			return 0xA3;//是艺达的卡
		}
#endif // _YiDa_Card_ //判断是不是艺达的卡
// 		if(memcmp(buffer+14, "\xDA\xDA", 2)==0)//用于SAM2刷建设部，SAM1刷自己公司卡
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
	debugstring("1扇区信息0：\r\n");
	debugdata(buffer,16,1);
#endif	
	if(MiBlockInvalid(1,15,buffer)==ST_ERROR) 
	{
		return ST_ERROR;
	}
	
	gMCardCand = CARDSTYLE_NORM1;
	pos=0;
	memcpy(cardMessage.CityCode, buffer+pos, CITY_CODE_LEN);			//城市代码	2
	pos+=CITY_CODE_LEN;
	memcpy(cardMessage.TradeCode, buffer+pos, TRADE_CODE_LEN);			//行业代码	2
	pos+=TRADE_CODE_LEN;	
	memcpy((unsigned char *)cardMessage.PublishBicker, buffer+pos, PUBLISH_BICKER_LEN);	//卡流水号	4	
	memcpy(cardMessage.CardCheckCode, buffer+8, CARD_CHECH_CODE_LEN);	//mac码		4
	cardMessage.card_catalog=buffer[13];								//卡类		1
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
	if((mode == 0xA1)&&(cardMessage.card_catalog < 0x40)){//读到卡类就返回, 当控制卡返回
		return CONTROL_CARD;
	}

	if(cardMessage.card_catalog == CARD_Business){
		if(MifareRead(cardMessage.publishSector*4+1,buffer)==0)//取目录区所在位置
		{
			return ST_ERROR;
		}
		
#ifdef _debug_
		debugstring("1扇区信息1：\r\n");
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
		cardMessage.catalogPOST = buffer[0];//写目录
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

	if(MifareRead(cardMessage.publishSector*4+1, buffer)==0)//发行日期 有效日期 启用日期 卡押金	保留	校验
	{
		return ST_ERROR;
	}

#ifdef _debug_
	debugstring("1扇区信息1:");
	debugdata(buffer, 16, 1);
#endif
	
	if(MiBlockInvalid(1,15,buffer)==ST_ERROR) 
	{
#ifdef _debug_
		debugstring("1扇区信息1 校验错\r\n");
#endif
	
		return ST_ERROR;
	}
//时间缓冲清0
	endTime = 0;
	startTime = 0;
	nowTime = 0;
	endTimeEn = 0;

	memcpy(cardMessage.stuffNO, buffer, 4);//员工卡的卡号,主要是司机卡使用

	nnAudioflag = buffer[14];//性别
	
#ifdef BUS_SHAODONG_	//绍东原来没有年审的卡要加入年审。给了他们后，此段代码可以不用。
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
	buffer[52] = bcdDEC(buffer[52], 1);//有效日期,提前一个月提示。
	if(buffer[52] == 0){
		buffer[52] = 0x12;
		buffer[51] = bcdDEC(buffer[51], 1);
	}

	memcpy((unsigned char*)&endTimeEn, buffer+50, 4);	//有效日期,处理后的。
	memcpy((unsigned char*)&endTime, buffer+4, 4);//有效日期，处理前的
	memcpy((unsigned char*)&startTime, buffer+8, 4);//启动日期
	
	if(GetDateTime()==ST_ERROR) 
	{
		error(ERR_READ_TIME,0);
		return ST_ERROR;
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
		if(getMiKey()==ST_ERROR)	//九江，现在的控制卡也全部取密钥
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
		getCpuInfo((stCpuInfo*)buffer); //在读到卡时更新	
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
			if(cardMessage.card_catalog<0x20){//用户卡返回错误，不是用户卡不返回
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
			if(NewPriceTab.rate[cardMessage.card_catalog] == 0){//折扣为0，不能消费
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
			if(NewPriceTab.rate[cardMessage.card_catalog]==202)//免费消费
			{
				return MONTH_CARD;	
			}
			else if((NewPriceTab.rate[cardMessage.card_catalog] == 104)||(NewPriceTab.rate[cardMessage.card_catalog] == 204)){
				return CARD_forbid;//此卡被禁止使用
			}
			if(cardMessage.oddMonthSector!=0xff)//月票
			{
				return MONTH_CARD;		   
			}
			else 
			{
				return CARD_MONEY;			//钱包
			}
		}
		else {
			return	CONTROL_CARD;
		}
#elif defined Card_typeM_Min_is0_ //这里有鄂州、宝鸡
		if(cardMessage.card_catalog<0x20)
		{
#ifdef BUS_EZOU_	//普通卡，学生卡不年审
			if((cardMessage.card_catalog!=CARD_NORMAL_BUS)&&(cardMessage.card_catalog!=CARD_STUDENT_BUS))//
#else
			if(cardMessage.card_catalog!=CARD_NORMAL_BUS)//
#endif
			{
				// 如果读出卡内时间或POS机时间错误，则不判断有效期 2010.03.11
				if((chekMdate(startTime)==ST_OK)&&(chekMdate(endTime)==ST_OK)&&(chekMdate(nowTime)==ST_OK)){
					if((endTime<nowTime)||(startTime>nowTime))
						return CARD_NO_TIME;
				}
			}

			if(cardMessage.card_catalog == CARD_OLDMAN_BUS){//老年卡，属于年卡
				return CARD_YEAR;//年票
			}

			if(NewPriceTab.rate[cardMessage.card_catalog]==202)//免费消费
			{
				return MONTH_CARD;	
			}
			else if(NewPriceTab.rate[cardMessage.card_catalog] == 203){//不能使用次数
				return CARD_MONEY;			//钱包
			}
			else if(NewPriceTab.rate[cardMessage.card_catalog]==204){//年票
				return CARD_YEAR;
			}
			else if(NewPriceTab.rate[cardMessage.card_catalog] == 207){//都不能使用
				return CARD_forbid;
			}
			if(cardMessage.oddMonthSector!=0xff)//月票
			{
				return MONTH_CARD;		   
			}
			else 
			{
				return CARD_MONEY;			//钱包
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
#ifdef BUS_YICHUN	//宜春用 CARD_COUPON_BUS 作为员工司机卡
			if(cardMessage.card_catalog == CARD_COUPON_BUS){
				if(DriverCardDeal() == ST_OK)
					return ST_OK;//员工司机卡签到，签退正常。
			}//不能签到，签退。按季票（折扣率扣）
#endif
			gucEndTimeFlag = 0;
			if(cardMessage.card_catalog!=CARD_NORMAL)//
			{
				if((startTime>nowTime)||(endTime<nowTime)||(startTime>endTime)){
					return CARD_NO_TIME;
				}
				else if(endTimeEn<=nowTime){//需要提示“请年审了”
					gucEndTimeFlag = 1;
				}
			}
			if((NewPriceTab.rate[cardMessage.card_catalog-0x40] == 103)||(NewPriceTab.rate[cardMessage.card_catalog-0x40] == 203)){
				return CARD_MONEY; //此卡不能扣次，只能扣钱
			}
			if((NewPriceTab.rate[cardMessage.card_catalog-0x40] == 104)||(NewPriceTab.rate[cardMessage.card_catalog-0x40] == 204)){
				return CARD_forbid;//此卡被禁止使用
			}
			if(NewPriceTab.rate[cardMessage.card_catalog-0x40]<=100)
			{
				return CARD_MONEY;				//现金卡
			}
			if((NewPriceTab.rate[cardMessage.card_catalog-0x40]==101)||(NewPriceTab.rate[cardMessage.card_catalog-0x40]==201))
			{
				return MONTH_CARD;//扣次后不需要扣钱包附加费，纯月票卡；101要加一次附加费，110不加附加费，111加一次，112加两次，依次到119.
			}
			if((NewPriceTab.rate[cardMessage.card_catalog-0x40]>=110) && (NewPriceTab.rate[cardMessage.card_catalog-0x40]<=119))
			{
				return MONTH_CARD;//扣次后不需要扣附加费，纯月票卡；101要加一次附加费，110不加附加费，111加一次，112加两次，依次到119.
			}
			if((NewPriceTab.rate[cardMessage.card_catalog-0x40] ==102)||(NewPriceTab.rate[cardMessage.card_catalog-0x40] == 202))	
			{
				return CARD_STUFF_BUS;//员工卡
			}
			return ST_ERROR;
		}
		else {
			return CONTROL_CARD;
		}
#endif  //#ifdef BUS_PRO_JSB
}



