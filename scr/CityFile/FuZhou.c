
//���ݵĸ�������Ʊ�۶����������0��ʼ��
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

#define FZ_CARD_NORMAL CARD_NORMAL_BUS	//��ͨ��0��8��
#define FZ_CARD_STUDENT_BUS CARD_STUDENT_BUS	//ѧ����1��6��
#define FZ_CARD_Oldman_BUS CARD_SX_BUS	//���꿨2���ο�
#define FZ_CARD_HANDICAPPED_BUS CARD_STUFF_BUS	//�м��˿�3���ο�
#define FZ_CARD_STUFF_BUS CARD_YH_BUS	//Ա����4���ο�
#define FZ_CARD_YOUHUI_BUS CARD_YF_BUS	//�Żݿ�5���ο�

void getFixPrice_FuZhou(void)//���ݵ�����·����д���ڳ�������,ֻд���ۿ��ʡ�
{
//	memcpy(NewPriceTab.busPrice, "\x64\x00\x00\x00", 4);	//Ʊ��1Ԫ
	NewPriceTab.rate[FZ_CARD_NORMAL] = 80;	//��ͨ��0��8��
	NewPriceTab.rate[FZ_CARD_STUDENT_BUS] = 60;	//ѧ����1��6��
	NewPriceTab.rate[FZ_CARD_Oldman_BUS] = 101;	//���꿨2���ο�
	NewPriceTab.rate[FZ_CARD_HANDICAPPED_BUS] = 101;	//�м��˿�3���ο�
	NewPriceTab.rate[FZ_CARD_STUFF_BUS] = 101;	//Ա����4���ο�
	NewPriceTab.rate[FZ_CARD_YOUHUI_BUS] = 101;	//�Żݿ�5���ο�

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
	NewPriceTab.voice[CARD_RELATION_BUS-nj] = Audio_DONG;//�⿨������
	NewPriceTab.voice[CARD_YOUHUI_BUS-nj]=Audio_YH;
}

int getCardtype(char *cardD, unsigned char type)
{
	switch(type){
	case FZ_CARD_NORMAL:
		strcpy(cardD, "��ͨ��");
		break;
	case FZ_CARD_STUDENT_BUS:
		strcpy(cardD, "ѧ����");
		break;
	case FZ_CARD_Oldman_BUS:
		strcpy(cardD, "���꿨");
		break;
	case FZ_CARD_HANDICAPPED_BUS:
		strcpy(cardD, "���ſ�");
		break;
	case FZ_CARD_STUFF_BUS:
		strcpy(cardD, "Ա����");
		break;
	case FZ_CARD_YOUHUI_BUS:
		strcpy(cardD, "�Żݿ�");
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
//��������
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








