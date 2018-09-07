
//抚州的个性需求，票价定死，卡类从0开始。
#include "string.h"
#include "stdio.h"
#include "..\szct.h"
#include "..\..\include\include.h"
#include "..\Font.h"
#include "CityFile.h"
#ifdef BUC_FuZhou_

extern stPricetable NewPriceTab;
extern pFistVary_1 pFistVary;

const unsigned int gHandList[HandListlen+1] = {
	621416, 0
};

#define FZ_CARD_NORMAL CARD_NORMAL_BUS	//普通卡0：8折
#define FZ_CARD_STUDENT_BUS CARD_STUDENT_BUS	//学生卡1：6折
#define FZ_CARD_Oldman_BUS CARD_SX_BUS	//老年卡2：次卡
#define FZ_CARD_HANDICAPPED_BUS CARD_STUFF_BUS	//残疾人卡3：次卡
#define FZ_CARD_STUFF_BUS CARD_YH_BUS	//员工卡4：次卡
#define FZ_CARD_YOUHUI_BUS CARD_YF_BUS	//优惠卡5：次卡

void getFixPrice_FuZhou(void)//抚州的无线路卡，写死在程序里面,只写死折扣率。
{
//	memcpy(NewPriceTab.busPrice, "\x64\x00\x00\x00", 4);	//票价1元
	NewPriceTab.rate[FZ_CARD_NORMAL] = 80;	//普通卡0：8折
	NewPriceTab.rate[FZ_CARD_STUDENT_BUS] = 60;	//学生卡1：6折
	NewPriceTab.rate[FZ_CARD_Oldman_BUS] = 101;	//老年卡2：次卡
	NewPriceTab.rate[FZ_CARD_HANDICAPPED_BUS] = 101;	//残疾人卡3：次卡
	NewPriceTab.rate[FZ_CARD_STUFF_BUS] = 101;	//员工卡4：次卡
	NewPriceTab.rate[FZ_CARD_YOUHUI_BUS] = 101;	//优惠卡5：次卡

//	memcpy(pFistVary.LineNo, "\x00\x01\x00", 3);
}

void SetAudio(void)
{
	unsigned char nj;
	
	memset(NewPriceTab.voice,Audio_DONG,32);
	
#ifdef Card_typeM_Min_is0_
	nj = 0;
#else
	nj = 0x40;
#endif
	
	NewPriceTab.voice[CARD_NORMAL_BUS-nj] = Audio_DONG;
	NewPriceTab.voice[CARD_STUDENT_BUS-nj] = Audio_STUDENT;
	
	NewPriceTab.voice[CARD_OLDMAN_BUS-nj] = Audio_OLDMAN;//Audio_BIRTHDAY;//Audio_OLDMAN;
	
	NewPriceTab.voice[CARD_STUFF_BUS-nj] = Audio_STUFF;
	NewPriceTab.voice[CARD_COUPON_BUS-nj] = Audio_LOVER;
	NewPriceTab.voice[CARD_YOUDAI_BUS-nj] = Audio_YH;//Audio_LOVER;
	NewPriceTab.voice[CARD_ZILU_BUS-nj]=Audio_DONG;
	NewPriceTab.voice[CARD_STUFFC_BUS-nj]=Audio_STUFF;
	NewPriceTab.voice[CARD_YOUHUI_BUS-nj]=Audio_DONG;
	NewPriceTab.voice[CARD_PRACTISE_BUS-nj] = Audio_YH;//Audio_LOVER;
	NewPriceTab.voice[CARD_CONGCI_BUS-nj]=Audio_MONTH;
	NewPriceTab.voice[CARD_RELATION_BUS-nj] = Audio_DONG;//这卡响两声
	NewPriceTab.voice[CARD_YOUHUI_BUS-nj]=Audio_YH;
}

int getCardtype(char *cardD, unsigned char type)
{
	switch(type){
	case FZ_CARD_NORMAL:
		strcpy(cardD, "普通卡");
		break;
	case FZ_CARD_STUDENT_BUS:
		strcpy(cardD, "学生卡");
		break;
	case FZ_CARD_Oldman_BUS:
		strcpy(cardD, "老年卡");
		break;
	case FZ_CARD_HANDICAPPED_BUS:
		strcpy(cardD, "残优卡");
		break;
	case FZ_CARD_STUFF_BUS:
		strcpy(cardD, "员工卡");
		break;
	case FZ_CARD_YOUHUI_BUS:
		strcpy(cardD, "优惠卡");
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
//抚州语音
void setAudio_FuZhou(void)
{
	unsigned char nj;
	
	memset(NewPriceTab.voice,Audio_DONG,32);
	
#ifdef Card_typeM_Min_is0_
	nj = 0;
#else
	nj = 0x40;
#endif
	
	NewPriceTab.voice[FZ_CARD_NORMAL-nj] = Audio_DONG;
	NewPriceTab.voice[FZ_CARD_STUDENT_BUS-nj] = Audio_STUDENT;
	NewPriceTab.voice[FZ_CARD_Oldman_BUS-nj] = Audio_OLDMAN;//Audio_BIRTHDAY;//Audio_OLDMAN;
	NewPriceTab.voice[FZ_CARD_STUFF_BUS-nj] = Audio_STUFF;
	NewPriceTab.voice[FZ_CARD_HANDICAPPED_BUS-nj]=Audio_CY;
	NewPriceTab.voice[FZ_CARD_YOUHUI_BUS-nj] = Audio_YH;//Audio_LOVER;Audio_YouDai
}



#endif








