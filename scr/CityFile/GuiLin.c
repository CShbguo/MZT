#include "string.h"
#include "stdio.h"
#include "..\szct.h"
#include "..\..\include\include.h"
#include "..\Font.h"
//#include "SLZR_TEST.h"
#include "CityFile.h"

#ifdef BUS_GUILIN_



extern Parameter5 cardMessage;
extern unsigned int c_serial;
extern stPricetable NewPriceTab;

const unsigned int gHandList[HandListlen+1] = {
	0
};


void SetAudio(void)
{
	unsigned char nj;
	
	memset(NewPriceTab.voice,Audio_DONG,32);
	
	nj = 0;
	
	NewPriceTab.voice[CARD_NORMAL_BUS-nj] = Audio_DONG;
	NewPriceTab.voice[CARD_STUDENT_BUS-nj] = Audio_STUDENT;
	NewPriceTab.voice[CARD_ZILU_BUS-nj] = Audio_STUDENT;
	
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
{//�Żݿ���ʾint getCardtypecommon(char *cardD, unsigned char type)
	
	switch(type){
	case CARD_NORMAL_BUS:
		strcpy(cardD, Card_Name_NORMAL);
		break;
	case CARD_OLDMAN_BUS:
		strcpy(cardD, "���꿨");
		break;
	case CARD_PRACTISE_BUS:
		strcpy(cardD, "�ͱ�ѧ����");
		break;
	case CARD_COUPON_BUS:
		strcpy(cardD, "�м��˿�");
		break;
	case CARD_HANDICAPPED:
		strcpy(cardD, "�˲о��˿�");
		break;
	case CARD_JILIAN_BU:
		strcpy(cardD, "������Ա��");
		break;
	case CARD_YOUHUI_BUS:
		strcpy(cardD, "�����Żݿ�");
		break;
	case CARD_ZILU_BUS:
		strcpy(cardD, "ѧ���Żݿ�");
		break;
	case CARD_RELATION_BUS:
		strcpy(cardD, "�ͱ����˿�");
		break;
	case CARD_STUFF_BUS:
		strcpy(cardD, "Ա����");
		break;
	case CARD_LINE_SET:
		strcpy(cardD, "��·���ÿ�");
		break;
	default:
		cardD[0] = 0;
		break;
	}
	if(type == CARD_NORMAL_BUS){
		strcpy(cardD, Card_Name_NORMAL);
	}
	else if(type == CARD_STUDENT_BUS){
		strcpy(cardD, Card_Name_STUDENT);
	}
	else if(type == CARD_ZILU_BUS){
		strcpy(cardD, "ѧ���Żݿ�");
	}
	else if(type == CARD_OLDMAN_BUS){
		strcpy(cardD, Card_Name_OLDMAN);
	}
	else 
		cardD[0] = 0;
	if(strlen(cardD) == 0)
		return ST_ERROR;
	else
		return ST_OK;
}

int calc_key(unsigned char *m1buff, unsigned char *okey)
{
	memcpy(okey, "\x10\x1E\x95\x65\xB7\x60", 6);
	memcpy(okey+6, "\x10\x1E\x95\x65\xB7\x60", 6);
	memcpy(okey+12, "\x10\x1E\x95\x65\xB7\x60", 6);
	memcpy(okey+18, "\x10\x1E\x95\x65\xB7\x60", 6);
	return ST_OK;
}

unsigned char Card_typeProcess_GuiLin(void)
{
	unsigned char pos,ii, t;
	unsigned char buffer[32];
	
	cardMessage.listSector = 0;
	memcpy(buffer, "\x10\x1E\x95\x65\xB7\x60", 6);

	pos = ((cardMessage.listSector*4)+1);//��Ŀ¼����һ�顣
	for(t=0; t<3; t++){
		MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,buffer,cardMessage.listSector*4);

		
		Delay(1);
		if(MifareRead(pos, buffer) ==0) 
		{
			MSG_LOG("ReadError:%d\r\n", pos);
			BCD_LOG(buffer, 16, 1);
			profile(0);
		}
		else
			break;
	}
	if(t>=3)
		return ST_ERROR;
	
	ii = crc_ck(buffer, 16);

	MSG_LOG("GuiLing 0����1��Ϣ:");
	BCD_LOG(buffer,16,1);
	MSG_LOG("CRC:%02X\r\n", ii);
	
	if(ii)
		return ST_ERROR;	//У�����
	return 0xA3;//���մ�Ŀ�

}

#endif

