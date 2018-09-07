#include "string.h"
#include "stdio.h"
#include "..\szct.h"
#include "..\..\include\include.h"
#include "..\Font.h"
//#include "SLZR_TEST.h"
#include "CityFile.h"

#ifdef BUS_GUANGXI

extern stPricetable NewPriceTab;




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
{//优惠卡显示int getCardtypecommon(char *cardD, unsigned char type)
	
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
#ifdef SCARD_HETONG_BUS
	else if(type == SCARD_HETONG_BUS){
		strcpy(cardD, "军人卡");
	}
#endif
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
#ifdef CARD_MOBILE_BUS
	else if(type == CARD_MOBILE_BUS){
		strcpy(cardD, "手机卡");
	}
#endif
	else 
		cardD[0] = 0;
	if(strlen(cardD) == 0)
		return ST_ERROR;
	else
		return ST_OK;
}

#endif

