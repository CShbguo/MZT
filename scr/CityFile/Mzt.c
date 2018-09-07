#include "string.h"
#include "stdio.h"
#include "..\szct.h"
#include "..\..\include\include.h"
#include "..\Font.h"
//#include "SLZR_TEST.h"
#include "CityFile.h"
#include "..\Mydes.h"

#include "..\usbFile.h"


#define SETION3   3
#define SETION5   5
#define SETION6   6
#define SETION7   7

//第二次扣款标志，语音用dong
unsigned char second_money_flag=0;

const unsigned char Dkeya[5][8]=
{{0xFF,0x15,0x97,0xf3,0x4b,0x13,0x03,0xc0},
{0xc9,0xf2,0xa7,0xfd,0x2f,0xfe,0x41,0x12},
{0x76,0x69,0xf8,0x40,0x6d,0x6c,0x0a,0x85},
{0x7f,0x18,0xbb,0x8a,0xec,0xa3,0xfd,0x48},
{0x30,0x2e,0xdb,0xfe,0xd8,0xda,0x7e,0x03}};
const unsigned char Dkeyb[5][8]=
{{0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37},
{0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37},
{0x01,0x89,0x9f,0x57,0x2a,0xf0,0xb2,0xc9},
{0xa3,0x0d,0x65,0x91,0x62,0x25,0x99,0xf6},
{0xac,0x03,0x78,0xdc,0xa9,0x65,0x72,0x89}};
const unsigned char Dkeyc[5][8]=
{{0xb3,0x7d,0x55,0x43,0x77,0x74,0x89,0x43},
{0x6a,0x1d,0x5a,0xf8,0xc1,0x2f,0x88,0x20},
{0x44,0x9e,0x64,0xd9,0x45,0x8f,0x76,0x1e},
{0x8f,0x7a,0xd2,0x57,0x49,0x50,0x5a,0x00},
{0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}};


////绵州通  现有的M1卡,本地CPU卡和交通部的卡

#ifdef BUS_MZT

extern Parameter5 cardMessage;
extern stPricetable NewPriceTab;
extern pFistVary_1 pFistVary;
extern Parameter3 ComSumeFile;
extern unsigned int c_serial;
extern unsigned int card_ser;
extern unsigned char cardSound;			//刷卡没有成功标志
extern stcomtime SysTime;
extern unsigned int a_sum1,s_sum1;
extern stBusVerIfo gBusVerInfo;
extern unsigned int startTime,endTime,nowTime,endTimeEn;

extern unsigned char gucRestorFlag;
extern fFlagVary_1 F_FLAG_VARY;
extern unsigned char gMCardCand;




extern unsigned char Xor8(const unsigned char *pData, unsigned long DataLen);
extern unsigned int get_s_sum1(unsigned char mode);
extern void Sound_cityCMD(unsigned char sMode);
extern void SoundMessage(unsigned char cmd);
extern void money_msg(unsigned char dmode, unsigned int remM, unsigned int pucM, unsigned char cMOde);
//extern void dis_time_mini(unsigned char x, stcomtime *time);
extern unsigned char WriteRecord(unsigned char *buffer, unsigned char mode);
extern void SoundFreeManage(char *sstr);
extern unsigned char OperPublicMessage(void);
extern unsigned char getMiKey(void);
extern void error(unsigned char code1,unsigned char Index);
extern unsigned int IsInBlackList(unsigned char *c_card_no);
extern unsigned char GetDateTime(void);
extern void getCpuInfo(stCpuInfo *inP);
extern void saveCpuInfo(unsigned char mode, unsigned char *dat);
extern void BuildRecorde(unsigned char delType, unsigned char *recBuf);
extern void save_normal(void);
extern void save_h_month(void);
extern unsigned char month_decide(void);
extern void Save_delayCardinfo(unsigned char Cmode);
extern unsigned char getMiKey(void);
extern unsigned char Delay_Normal(void);
extern void disp_no_swipe(void);
extern void error(unsigned char code1,unsigned char Index);
extern void ControlDis(unsigned char type);
extern unsigned char checkBusInfo(void);
extern void savesysbupara(void);
extern void wRiteControlRecord(unsigned char ID);


extern unsigned int g24GDisFlash;//24G卡界面刷新
extern unsigned char restore_flag;


//用于USB优盘更新参数文件 by pfuqan
//>0 成功，<0 失败  =0 其他参数不对
int set_line_mzt(char *infor)
{
	char *lineinfor=NULL;
	char buff[257];
#ifdef _debug_
	unsigned char *p=NULL;
#endif
	char ctempbuff[16];
//	unsigned char ucbuff[16];
	unsigned int i=0,count=0;

	unsigned char lineNO[3]={0};
	unsigned int price=0,plusprice=0;
	unsigned char rate[32]={0},rateplus[32]={0};

	MSG_LOG("do %s\r\n",__FUNCTION__);

	memset(buff,0,sizeof(buff));
	memcpy(buff,infor,sizeof(buff));
	buff[256] =0;

	MSG_LOG("Line infor:%s\r\n",buff);


	memset(rate,204,32);
	memset(rateplus,100,32);



	//线路 票价，附加票价，折扣，附加折扣，?个 0~31 折扣附加折扣...
	//LINO,000100,PRICE,100,0,RATE,10,100,100,90,100,80,100,70,100,60,100,50,100,40,100,30,100,20,100,10,100
	lineinfor = strtok(buff, ",");	//找到LINE后面的, 
    if (lineinfor == NULL)  
	{
		return -1;
	}

	MSG_LOG("find KEY:%s\r\n",lineinfor);

	lineinfor = strtok(NULL, ",");  
    if (lineinfor == NULL)  
	{
		return -2;
	}
	
	MSG_LOG("find LINO:%s\r\n",lineinfor);

	count = __atol(lineinfor);
	memset(ctempbuff,0,sizeof(ctempbuff));
	sprintf(ctempbuff,"%06d",count);
	Ascii2BCD((unsigned char *)ctempbuff,lineNO,6);

	if (memcmp(pFistVary.LineNo,lineNO,2) != 0)
	{
		MSG_LOG("线路不一致,退出\r\n");
		BCD_LOG(pFistVary.LineNo,3,1);
		BCD_LOG(lineNO,3,1);
		return 0;
	}


	

	lineinfor = strtok(NULL, ","); 
    if (lineinfor == NULL)  
	{
		return -3;
	}
	MSG_LOG("find KEY:%s\r\n",lineinfor);	//PRICE

	lineinfor = strtok(NULL, ","); 
    if (lineinfor == NULL)  
	{
		return -4;
	}
	price = __atol(lineinfor);

	MSG_LOG("find price:%s:%d\r\n",lineinfor,price);

	lineinfor = strtok(NULL, ","); 
    if (lineinfor == NULL)  
	{
		return -5;
	}
	plusprice = __atol(lineinfor);
	
	MSG_LOG("find pluseprice:%s:%d\r\n",lineinfor,plusprice);


	lineinfor = strtok(NULL, ","); 
    if (lineinfor == NULL)  
	{
		return -6;
	}
	MSG_LOG("find KEY:%s\r\n",lineinfor);	//RATE

	lineinfor = strtok(NULL, ","); 
	if (lineinfor == NULL)  
	{
		return -7;

	}
	count = __atol(lineinfor);
	MSG_LOG("find rateNUM:%s,count=%d\r\n",lineinfor,count);

	for (i=0;i<count;i++)
	{
		lineinfor = strtok(NULL, ","); 
		if (lineinfor == NULL)  
		{
			return -8;
		}
		rate[i] = __atol(lineinfor);
		MSG_LOG("find card:%d rate:%s ",i,lineinfor);

		lineinfor = strtok(NULL, ","); 
		if (lineinfor == NULL)  
		{
			return -9;
		}
		rateplus[i] = __atol(lineinfor);
		MSG_LOG(" plusrate:%s\r\n",lineinfor);
	
	}


	MSG_LOG("price:%d,plus:%d\r\n",price,plusprice);
	MSG_LOG("line:");
	BCD_LOG(lineNO,3,1);

	MSG_LOG("Rate:");
	BCD_LOG(rate,32,1);
	MSG_LOG("pluseRate:");
	BCD_LOG(rateplus,32,1);


	memcpy(pFistVary.LineNo,lineNO,3);
	memcpy(NewPriceTab.busPrice,&price,4);
	memcpy(NewPriceTab.plusPrice,&plusprice,4);

	memcpy(&NewPriceTab.rate[1],rate,31);
	NewPriceTab.rate[0] = rate[31];
	memcpy(&NewPriceTab.plusrate[1],rateplus,31);
	NewPriceTab.plusrate[0] = rateplus[31];




	sysfewrite(BIT_LINENO,3,pFistVary.LineNo);	
	sysfewrite(BIT_PRICETAB,sizeof(stPricetable), (unsigned char*)&NewPriceTab);	
	sysfewrite(BIT_GPSChildLine, 2, pFistVary.LineNo);//刚开始时，子线路和主线路一致
	
	savesysbupara();
	saveCpuInfo(1, (unsigned char *)ctempbuff);
	
	
	
	wRiteControlRecord(ID_REC_LINENO);




	return 1;


}

//U盘取线路信息，返回0：返回<0:失败，返回>0成功
int usb_loadlineinfor(void)
{	
	FRESULT res;
	unsigned char buff[4096];
// 	unsigned char buf1[16];
	char *lineinfor=NULL;
	FILINFO finf;
	FIL rf1;
	int ret = 0;
//	unsigned int count=0;



// 	char ctempbuff[16];
// 	//	unsigned char ucbuff[16];
// 	
// 	char *lineinfor_temp=NULL;
// 	unsigned char lineNO[3]={0};
// 	unsigned int price=0,plusprice=0;
// 	unsigned char rate[32]={0},rateplus[32]={0};
	
	MSG_LOG("do %s\r\n",__FUNCTION__);
	
	memset(buff,0,sizeof(buff));




	cls();
	display(2, 0, "更新线路参数", DIS_CENTER);
	
	res = f_open(&rf1, "0:/pra/LINEINFOR.txt", FA_READ);
	if(res != FR_OK)//if(access("0:/current/Rindex.txt",0)!=0)
	{
		dis_messgebox("线路文件错", res, 12);
		return 0;
	}
	
	res = f_stat("0:/pra/LINEINFOR.txt", &finf);
	if(res != FR_OK){
		dis_messgebox("线路文件信息错", res, 12);
		return -1;
	}

	memset(buff,0,sizeof(buff));
	ret = _Fread(buff, sizeof(buff), &rf1) ;
//	ret = fread(buffer,1,1024,fp);	//size_t fread ( void *buffer, size_t size, size_t count, FILE *stream) ;

	MSG_LOG("ret:%d\r\n",ret);
	
//	MSG_LOG(buff);
	
//	MSG_LOG("find line\r\n");
	
	
// 	count = 0;
// 	
// 	lineinfor = strtok((char *)buff, "\r\n");  
//     if (lineinfor != NULL)  
// 	{
// 		
// 		while (lineinfor !=NULL)
// 		{
// 			MSG_LOG("\r\n**************Line:%d  :%s***********\r\n",count,lineinfor);
// 			lineinfor = strtok(NULL, "\r\n"); 
// 
// 			count ++;
// 		}
// 	}
// 
// 
// 	MSG_LOG("有%d个\r\n",count);

	
	lineinfor = NULL;
	lineinfor = strstr((char *)buff,"\r\n");

	if (lineinfor ==NULL)
	{
		return -2;
	}

	lineinfor +=2;	//跳\r\n  这个是头注释

	while(1)
	{
		ret = set_line_mzt(lineinfor);
		if (ret >0)
		{
			return 2;	//成功
		}
		clr_wdt();
		lineinfor = strstr((char *)lineinfor,"\r\n");
		if (lineinfor ==NULL)
		{
			break;
		}
		lineinfor +=2;	//跳\r\n

	}


	



	return 0;



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
	NewPriceTab.voice[CARD_ZILU_BUS-nj]=Audio_Zilu;
	NewPriceTab.voice[CARD_Love_BUS-nj]=Audio_LOVER;
	NewPriceTab.voice[CARD_Love_BUS_2-nj]=Audio_LOVER;

}

int getCardtype(char *cardD, unsigned char type)
{//优惠卡显示int getCardtypecommon(char *cardD, unsigned char type)

// 	unsigned char temp=0;
	MSG_LOG("zcard:%02X,type:0x%02x\r\n",cardMessage.Zicard_catalog,type);
	 if (cardMessage.Zicard_catalog &0x80)
	{
		if(type >= 0x40)
			type = type-0x40;	
	}

	MSG_LOG("type:%02x\r\n",type);

	if(type == CARD_NORMAL_BUS){
		strcpy(cardD, Card_Name_NORMAL);
	}
	else if(type == CARD_STUDENT_BUS){
		strcpy(cardD, Card_Name_STUDENT);
	}
	else if(type == CARD_OLDMAN_BUS){
		strcpy(cardD, Card_Name_OLDMAN);
	}

	else if(type == CARD_STUFF_BUS){
		strcpy(cardD, Card_Name_STUFF);
	}
	else if(type == CARD_DRIVER_BUS){
		strcpy(cardD, Card_Name_Driver);
	}
	else if(type == CARD_Love_BUS){
		strcpy(cardD, "爱心卡1");
	}
	else if(type == CARD_Love_BUS_2){
		strcpy(cardD, "爱心卡2");
	}
	else if(type == CARD_ZILU_BUS){
		strcpy(cardD, "子女卡");
	}
	else if(type == CARD_UNION_BUS){
		strcpy(cardD, "交通部卡");
	}
	else if(type == CARD_QR_BUS){
		strcpy(cardD, "二维码");
	}
	else if(type == CARD_QPBOC_BUS){
		strcpy(cardD, "银联卡");
	}
	else if(type == CARD_QR_PBOC){
		strcpy(cardD, "银联二维码");
	}
	else if(type == CARD_PBOC_CASH){
		strcpy(cardD, "电子现金");
	}
	else 
		cardD[0] = 0;


	if(strlen(cardD) == 0)
		return ST_ERROR;
	else
		return ST_OK;
}




unsigned char getSetionKey(unsigned char sector)
{

	unsigned char i;
//	unsigned char keyno;
	unsigned char buffer_1[16];
	unsigned char key_encryp[8];


	switch(sector)
	{
	case 2:
//		keyno=1;
		break;
	case 3:
//		keyno=3;
		i=0;
		break;
	case 5:
//		keyno=5;
		i=1;
		break;
	case 6:
//		keyno=8;
		i=2;
		break;
	case 7:
//		keyno=11;
		i=3;
		break;
	default:
		return ST_ERROR;
	}

	i +=1;

 	memcpy(buffer_1,Dkeya[i],8);
	buffer_1[8]=*((unsigned char *)&c_serial);
	buffer_1[9]=Dkeyc[i][0];
	buffer_1[10]=*(((unsigned char *)&c_serial)+1);
	buffer_1[11]=Dkeyc[i][1];
	buffer_1[12]=*(((unsigned char *)&c_serial)+2);
	buffer_1[13]=Dkeyc[i][2];
	buffer_1[14]=*(((unsigned char *)&c_serial)+3);
	buffer_1[15]=sector;

// 	MSG_LOG("%d keyc:",i);
// 	BCD_LOG(Dkeyc[i],3,1);
// 	MSG_LOG("%s:%d,buff:",__FUNCTION__,sector);
// 	BCD_LOG(&buffer_1[8],8,1);

	
	DES_encrypt((unsigned char *)&buffer_1[8],buffer_1,key_encryp);

	MSG_LOG("MZT:%02d扇区密钥:\r\n",sector);
	BCD_LOG(key_encryp,6,1);

	switch(sector)
	{
	case 2:
		
		break;
	case 3:
		memcpy(cardMessage.MoneyKEYA,  key_encryp, 6);				//公共钱包,信息2 6

		break;
	case 5:
		memcpy(cardMessage.PublishKEYA, key_encryp, 6);				//发行区
		break;
	case 6:
		memcpy(cardMessage.PublichKEYA, key_encryp, 6);				//公共

		break;
	case 7:

		break;
	default:
		return ST_ERROR;
	}






	return ST_OK;
}


unsigned char Card_typeProcess_MZT(unsigned char mode)	
{
//	unsigned char pos;
	unsigned char buffer[256];
	unsigned char cNo[8];
//	unsigned char flag;
	ICCard_block20 *block20;
	ICCard_block26 *block26;
	unsigned char ret=0;


	memcpy(cNo, (unsigned char *)&c_serial, 4);
	cardMessage.listSector = 0;
	


	//1、读第六扇区
	getSetionKey(SETION6);

	getSetionKey(SETION3);
	//2.读第5扇区
	getSetionKey(SETION5);
	getSetionKey(SETION7);

	


	
	// 	memcpy(cardMessage.RecodeKEYA, buffer+6, 6);			//交易记录	03 04 05
	// 	memcpy(cardMessage.MonthKEYA,  buffer+12, 6);			//月票区域 7 8
	
	MSG_LOG(" Money KEYA:\r\n");
	BCD_LOG(cardMessage.MoneyKEYA, 6, 1);
	MSG_LOG(" Publich KEYA:\r\n");
	BCD_LOG(cardMessage.PublichKEYA, 6, 1);
	MSG_LOG(" Recode KEYA:\r\n");
	BCD_LOG(cardMessage.RecodeKEYA, 6, 1);
	MSG_LOG(" Month KEYA:\r\n");
	BCD_LOG(cardMessage.RecodeKEYA, 6, 1);

	MSG_LOG(" Publish KEYA:\r\n");
	BCD_LOG(cardMessage.PublishKEYA, 6, 1);



//	flag = 0xff;
	cardMessage.publishSector = SETION5;//the bussiness  card used
	cardMessage.publicMoneySector = SETION3;
	cardMessage.publicMessageSector = SETION6;
	cardMessage.dealRecordSector[0] = SETION5;	//线路卡
	cardMessage.dealRecordSector[1] = SETION6;	//线路卡
	cardMessage.dealRecordSector[2] = SETION7;	//线路卡




	
#ifdef _debug_
	debugstring("CARD MZT0-::----");
	debugdata((unsigned char *)&c_serial, 4, 1);
// 	debugdata((unsigned char *)&cardMessage.publishSector, 1, 1);
// 	debugdata(cardMessage.PublishKEYA, 6, 1);
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
	debugstring("5扇区信息0：\r\n");
	debugdata(buffer,16,1);
#endif	

	block20 = (ICCard_block20 *)buffer;

	
	gMCardCand = CARDSTYLE_NORM1;

// 	pos=0;
// 	memcpy(cardMessage.CityCode, buffer+pos, CITY_CODE_LEN);			//城市代码	2
// 	pos+=CITY_CODE_LEN;
// 	memcpy(cardMessage.TradeCode, buffer+pos, TRADE_CODE_LEN);			//行业代码	2
// 	pos+=TRADE_CODE_LEN;	
//	memcpy(cardMessage.CardCheckCode, buffer+8, CARD_CHECH_CODE_LEN);	//mac码		4


	memcpy((unsigned char *)cardMessage.PublishBicker, block20->publishBicker, 5);	//卡流水号 4	
	cardMessage.card_catalog=block20->cardType;								//卡类		1

	cardMessage.Zicard_catalog = 0;
	
	if(cardMessage.card_catalog ==CARD_WHITE_BUS)
	{
		return CARD_WHITE_BUS;
	}
	

   if(block20->cardType > 0x80) //黑名单判断 20150514
   {
	   return CARD_BLACK;
	}


   if(cardMessage.card_catalog>0x40)
   {
	   if(IsInBlackList(cardMessage.PublishBicker) < FLASH_BLK_END){

		   block20->cardType |= 0x80;//写黑卡操作  20150514

		   if(MifareWrite(cardMessage.publishSector*4,buffer)==0)
		   {
			   debugstring("write 20 error\r\n ");
			   return ST_ERROR;
			}
		   memset(buffer,0,sizeof(buffer));
		   BuildRecorde(ID_REC_BLKLIST, buffer);
		   WriteRecord(buffer,0);

		   return CARD_BLACK;
	   }
	}
	


   


	if (MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,cardMessage.PublichKEYA,cardMessage.publicMessageSector*4)==0)
	{
		debugstring("MifareAuthKey error\r\n ");
		return ST_ERROR; 
	}
//	debugstring("E ");
	
	if(MifareRead(cardMessage.publicMessageSector*4+2,buffer)==0)
	{
		debugstring("read 4 error\r\n ");
		return ST_ERROR;
	}

	block26 = (ICCard_block26 *)buffer;


	
//时间缓冲清0
	endTime = 0;
	startTime = 0;
	nowTime = 0;
	endTimeEn = 0;



// 	memcpy((unsigned char*)&endTimeEn, buffer+50, 4);	//有效日期,处理后的。
	memcpy((unsigned char*)&endTime, block26->evaluationTime, 4);//有效日期，处理前的
//	memcpy((unsigned char*)&startTime, buffer+8, 4);//启动日期
	
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
	debugstring("cardMessage.card_catalog:0x");
	debugdata((unsigned char*)&cardMessage.card_catalog, 1, 1);
#endif


	//4、返回信息
    if(cardMessage.card_catalog <0x40)
		return CONTROL_CARD;


	if (memcmp(pFistVary.LineNo,"\x09\x05",2) != 0)
	{
		MessageBox(1,"非905线路不能刷M1卡!");
		SoundMessage(SOUND_FLING_MONEY);//请投币
		restore_flag = 3;
		return ST_OK;
	}




	//老人卡，残疾卡，子女卡需要进行年审判断20150514
	if ((cardMessage.card_catalog==CARD_OLDMAN_BUS)||(cardMessage.card_catalog==CARD_Love_BUS)||(cardMessage.card_catalog==CARD_ZILU_BUS))
	{
		if(endTime<nowTime)
			return CARD_NO_TIME;	//过期卡
	}
	


	ret=cardMessage.card_catalog;
	BCD_LOG((unsigned char *)cardMessage.card_catalog,1,1);
	if(cardMessage.card_catalog >= 0x40)
		ret = cardMessage.card_catalog - 0x40;
	else
		ret = cardMessage.card_catalog;
	
	// 除了普通卡和优惠卡其他都不能连刷
	if ( (ret  != CARD_NORMAL_BUS))
	{
		if(month_decide()) // 第二次去扣钱包(次数不能连刷)
		{
			disp_no_swipe();
			SoundMessage(SOUND_FLING_MONEY);//请投币

			return ST_OK;
			
		}
	}


	
	if((NewPriceTab.rate[ret]==204)||(NewPriceTab.rate[ret]==104))//禁止
	{
		return CARD_forbid;	
	}	


	if((NewPriceTab.rate[ret]==205)||(NewPriceTab.rate[ret] ==105)){//免费消费 且只有一次消费
		
		if(month_decide()) // 第二次去扣钱包(次数不能连刷)
		{
			disp_no_swipe();
			SoundMessage(SOUND_FLING_MONEY);//请投币

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











































#endif

