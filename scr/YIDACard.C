
/*
	艺达卡消费程序
	使用亿达写的库函数：M1CardLib.lib计算密钥。
	宜春的卡更新完后，此程序无用。
  */

#include "string.h"
#include "stdio.h"
#include "szct.h"

#ifdef _YiDa_Card_
#include "..\include\include.h"
#include "E_GJGPRS.h"
#include "m1CardLib.h"
//A0A1A2A3A4A5 374AF07C0F2C C131503B8B5B B807B0BC4A71 B807B0BC4A71B807B0BC4A71B807B0BC4A71B807B0BC4A71B807B0BC4A71B807B0BC4A71
typedef struct 
{
	unsigned char effFlag;				
	unsigned char Cardno[4];			
	unsigned char ErrorStep;			
	unsigned char OpSector;				
	unsigned char OpBlock;				
	unsigned char TollValue[4];			
	unsigned char AferToll[4];			
	unsigned char ToChange[16];			
}Parameter6;
extern Parameter6 ErrorDetail[3];
extern unsigned char error_pointer;		  //缓冲区的指针
extern Parameter5 cardMessage;
extern unsigned int c_serial;
extern unsigned char cardSound;			//刷卡没有成功标志
extern unsigned int card_ser;
extern unsigned char gucSoudnorm;
extern Parameter3 ComSumeFile;
extern unsigned int a_sum1,s_sum1;
extern unsigned int dis_sum2;
extern unsigned char restore_flag;
extern stcomtime SysTime;
//卡类型:手机卡03\艺达卡 04\公交卡01\CPU卡02
extern unsigned char gMCardCand;
extern fFlagVary_1 F_FLAG_VARY;
extern unsigned char nnAudioflag;
extern pFistVary_1 pFistVary;
extern unsigned char StufferNO[4];
extern unsigned int g24GDisFlash;//24G卡界面刷新
extern stPricetable NewPriceTab;

extern unsigned char CheckSno(unsigned char mode,unsigned char ID,unsigned char *ptr);
extern void ErrorOper(unsigned char flag);
extern void SoundMessage(unsigned char cmd);
extern unsigned int get_s_sum1(unsigned char mode);
extern void error(unsigned char code1,unsigned char Index);
extern void BuildRecorde(unsigned char delType, unsigned char *recBuf);
extern void money_msg(unsigned char dmode, unsigned int remM, unsigned int pucM, unsigned char cMOde);
extern void save_h_month(void);
extern unsigned char month_decide(void);
extern void save_normal(void);
extern void disp_no_swipe(void);
extern unsigned char Delay_Normal(void);
extern unsigned char GetDateTime(void);
extern unsigned char WriteRecord(unsigned char *buffer, unsigned char mode);
extern unsigned char FreeConsume(void);
extern unsigned int get_Month_s_sum1(void);
extern unsigned int IsInBlackList(unsigned char *c_card_no);
extern int getCardtype(char *cardD, unsigned char type);

extern unsigned char DriverCardDeal(void);

#ifdef BUS_YICHUN

volatile unsigned char GPSPCindex=0;
volatile unsigned char GPSPCcount;

void dealYDcom(void)
{
	unsigned char buff[16];
	unsigned char sndbuf[64];
	unsigned char pos=0;
	unsigned char sumchk=0,i;

	sndbuf[pos++] = 0x5B;
	sndbuf[pos++] = 0x5B;
	sndbuf[pos++] = 0x5B;

	sndbuf[pos++] = 0x03;//应答 CLA
	sndbuf[pos++] = irda_rxBuf[4];//应答 INS
	
	memcpy(sndbuf+pos, irda_rxBuf+5, 3);//INDEX
	pos += 3;
	if(irda_rxBuf[4] == 0x05){//获取信息
		sndbuf[pos++] = 0;
		sndbuf[pos++] = 0x0E;//长度
		
		sndbuf[pos++] = 0;
		sndbuf[pos++] = 2;	//商户代码2

		memcpy(sndbuf+pos, pFistVary.LineNo, 3);//线路编号
		pos+=3;
		Ascii2BCD(pFistVary.DeviceNo+2, sndbuf+pos, 6);//汽车编号
		pos+=3;
		Get_SerialNum(buff);
		memcpy(sndbuf+pos, buff+1, 3);
		pos+=3;
		memcpy(sndbuf+pos, StufferNO+1, 3);
		pos+=3;
	}
	else if((irda_rxBuf[3] == 0x03)&&(irda_rxBuf[4] == 0x01)){//发送司机记录返回
		memset(sndbuf, 0, 16);
		sysfewrite(BIT_GPS_MESSAGE, 16, sndbuf);
		restore_flag = 3;
#ifdef _debug_
		debugstring("完成司机信息发送!\r\n");
#endif
		return;
	}
	else{
		sndbuf[pos++] = 0;
		sndbuf[pos++] = 2;//长度
		sndbuf[pos++] = 0x90;//信息体
		sndbuf[pos++] = 0;
	}
	for(i=3; i<pos; i++)
		sumchk += sndbuf[i];//校验和
	sndbuf[pos++] = sumchk;
	
	com_snd(COM1, pos, sndbuf);
}

//发送司机记录
void SendYDcom(unsigned char *recBuf)
{
	unsigned char sndbuf[80];
	unsigned char pos=0;
	unsigned char sumchk=0,i;
	
	sndbuf[pos++] = 0x5B;
	sndbuf[pos++] = 0x5B;
	sndbuf[pos++] = 0x5B;
	
	sndbuf[pos++] = 0x01;//应答 CLA
	sndbuf[pos++] = 0x01;//应答 INS
	
	memcpy(sndbuf+pos, irda_rxBuf+5, 3);//INDEX
	pos += 3;
	sndbuf[pos++] = 0;
	sndbuf[pos++] = 0x40;//长度
	memcpy(sndbuf+pos, recBuf, 64);
	pos += 0x40;
	
	for(i=3; i<pos; i++)
		sumchk += sndbuf[i];//校验和
	sndbuf[pos++] = sumchk;
#ifdef _debug_
	debugstring("司机信息发送:");
	debugdata(sndbuf, pos, 1);
#endif
	com_snd(COM1, pos, sndbuf);
}

void YICHUNGpsHand(unsigned char value)
{
	if((GPSPCindex == 0)&&(value != 0x5B)){
		GPSPCindex = 0;
		return;
	}
	if((GPSPCindex == 1)&&(value != 0x5B)){
		GPSPCindex = 0;
		return;
	}
	if((GPSPCindex == 2)&&(value != 0x5B)){
		GPSPCindex = 0;
		return;
	}

	irda_rxBuf[GPSPCindex] = value;
	GPSPCindex++;
	if(GPSPCindex == 9)//长度高字节
		GPSPCcount = value;
	else if(GPSPCindex == 10){//长度低字节
		GPSPCcount *= 256;
		GPSPCcount += value;
		if(GPSPCcount > 1024){
			GPSPCcount = 0;
			GPSPCindex = 0;
			return;
		}
	}
	else if(GPSPCindex >= GPSPCcount+11){//接收完成
		dealYDcom();
		GPSPCindex = 0;
		return;
	}
}

/*unsigned char base_crc(unsigned char X1, unsigned char X2)
{
//'两个字节的CRC8
	unsigned char X3;
	unsigned char i;
	for(i=0; i<8; i++){// i = 0 To 7
		X3 = ((X1 ^ X2) & 0x01);
		if(X3 > 0)
			X2 = (X2 ^ 0x18);
		X2 = (X2>>1);
		if(X3 > 0)
			X2 = (X2 | 0x80);
		X1 = (X1>>1);
	}
	return (X2^0xFF);
}
unsigned char CRC8(unsigned char *byteData, unsigned char dlen){
//'数组中从任意位子开始到比任意位子大的位子结束的多个字节的CRC8
	unsigned char x;
	unsigned char LL;
	unsigned char KK;

	LL = dlen;
	x = 0;
	for(KK = 0; KK<LL; KK++){// To LL - 1
		x = base_crc(byteData[KK], x);
		debugstring("||"); 
		debugdata((unsigned char*)&x, 1, 0);
	}
	return x;
}*/

unsigned char YD_GPSMsginfoTLL;
void YD_BuildGPSMsginfo(unsigned char *buff)
{
	struct  GPSdriv
	{
		unsigned char type;//RECORD_DEAL 0xba
		unsigned char Rec[64];// Record 64byte;
		unsigned char Revser[3];
		unsigned char uCrc32[4];
	}YDGPSdriv;
	unsigned int crc32;
//	unsigned char i;
//DB6A0002566320120625061608000578000005001406127679000000000000D6120625070128000000000000050014061276790005780000000000000000B2E3
	YDGPSdriv.type = RECORD_DEAL;
	memcpy(YDGPSdriv.Rec, "\xDB\x6A\x00\x02\x56\x63\x20\x12\x06\x25"\
		"\x06\x16\x08\x00\x05\x78\x00\x00\x05\x00"\
		"\x14\x06\x12\x76\x79\x00\x00\x00\x00\x00"\
		"\x00\xD6\x12\x06\x25\x07\x01\x28\x00\x00"\
		"\x00\x00\x00\x00\x05\x00\x14\x06\x12\x76"\
		"\x79\x00\x05\x78\x00\x00\x00\x00\x00\x00"\
		"\x00\x00\xB2\xE3", 64);

// 	i = CRC8(YDGPSdriv.Rec, 63);
// 	debugdata(YDGPSdriv.Rec, 64, 1);
// 	debugstring("CRC8:");
// 	debugdata((unsigned char*)&i, 1, 1);

	YDGPSdriv.Rec[16] = 0;
	memcpy(YDGPSdriv.Rec+17, pFistVary.LineNo, 2);
	Ascii2BCD(pFistVary.DeviceNo+2, YDGPSdriv.Rec+19, 6);
	YDGPSdriv.Rec[42] = 0;
	memcpy(YDGPSdriv.Rec+43, pFistVary.LineNo, 2);
	Ascii2BCD(pFistVary.DeviceNo+2, YDGPSdriv.Rec+45, 6);
	memcpy(YDGPSdriv.Rec+6, (unsigned char*)&SysTime, 7);
	memcpy(YDGPSdriv.Rec+13, buff, 3);//司机号
	memcpy(YDGPSdriv.Rec+51, buff, 3);//司机号
	memset(YDGPSdriv.Revser, 0, 3);
	crc32 = __cpuCrc32((unsigned char*)&YDGPSdriv, 68);
	memcpy(YDGPSdriv.uCrc32, (unsigned char*)&crc32, 4);
	YD_GPSMsginfoTLL = 0;
	sysfewrite(BIT_GPS_MESSAGE, sizeof(YDGPSdriv), (unsigned char*)&YDGPSdriv);
	
//	debugdata(YDGPSdriv.Rec, 64, 1);
}
//检查并传输给GPS的司机信息,信息为64字节的记录。
void YD_GPSMsginfo(void)
{
	struct  GPSdriv
	{
		unsigned char type;//RECORD_DEAL 0xba
		unsigned char Rec[64];// Record 64byte;
		unsigned char Revser[3];
		unsigned char uCrc32[4];
	}YDGPSdriv;
	unsigned int crc32;

	if(YD_GPSMsginfoTLL > 100)
		YD_GPSMsginfoTLL = 10;
	if(YD_GPSMsginfoTLL != 0){
		YD_GPSMsginfoTLL--;
		return; 
	}
	YD_GPSMsginfoTLL = 10;
	sysferead(BIT_GPS_MESSAGE, sizeof(YDGPSdriv), (unsigned char*)&YDGPSdriv);
	if(YDGPSdriv.type == RECORD_DEAL){//有效
		crc32 = __cpuCrc32((unsigned char*)&YDGPSdriv, 68);
		if(memcmp((unsigned char*)&crc32, YDGPSdriv.uCrc32, 4) == 0){//校验有效
			SendYDcom(YDGPSdriv.Rec);
		}
		else{
			YDGPSdriv.type = 0;
		}
	}
}
#endif

//S1 data buffer
unsigned char S1buff[2][16];

int bcd2int(unsigned char *ibcd, unsigned char len)
{
	int i=0;
	int e = 0;

	for(i=0; i<len; i++){
		e *= 100;
		e += ((((ibcd[i]>>4)&0x0F)*10) + (ibcd[i] & 0x0F));
	}
	return e;

}

//转换卡类，传入主卡类和子卡类
unsigned char YiDaCardTypeChange(unsigned char Mt, unsigned char St)
{
	if((Mt == 0x50)&&(St == 0x01))
		return CARD_NORMAL_BUS;	//普通卡
	else if((Mt == 0x50)&&(St == 0xA5))
		return CARD_OLDMAN_BUS;	//老年卡
	else if((Mt == 0x50)&&(St == 0xB4))
		return CARD_PRACTISE_BUS;//低保学生卡
	else if((Mt == 0x50)&&(St == 0xA5))
		return CARD_COUPON_BUS;	//残疾人卡
	else if((Mt == 0x50)&&(St == 0xA8))
		return CARD_HANDICAPPED;//伤残军人卡
	else if((Mt == 0x50)&&(St == 0xA7))
		return CARD_JILIAN_BU;	//离休人员卡
	else if((Mt == 0x50)&&(St == 0xA3))
		return CARD_YOUHUI_BUS;	//成人优惠卡
	else if((Mt == 0x50)&&(St == 0xA4))
		return CARD_ZILU_BUS;	//学生优惠卡
	else if((Mt == 0x50)&&(St == 0xB3))
		return CARD_RELATION_BUS;	//低保成人卡
	else if((Mt == 0x51)&&(St == 0xB0))
		return CARD_STUFF_BUS;		//员工卡
	else if((Mt == 0x52)&&(St == 0xC5))
		return CARD_STUFF_BUS;		//线路设置卡
	else
		return 0xff;
}

unsigned char YiDagetCardtype(void)
{
	unsigned char M1buf[64];
	unsigned char buffer[64];
	unsigned int nowtime,endtime;
	int ii;
	
#ifdef _debug_
	debugstring("YiDa S0:\r\n");
#endif
	if (MifareRead(0, M1buf) ==0) 
	{
		return ST_ERROR;
	}
#ifdef _debug_
	debugstring("YiDa S1:\r\n");
#endif
	if (MifareRead(1, M1buf+16) ==0) 
	{
		return ST_ERROR;
	}
#ifdef _debug_
	debugstring("YiDa S2:\r\n");
#endif
	if (MifareRead(2, M1buf+32) ==0) 
	{
		return ST_ERROR;
	}
	
#ifdef BUS_GUILIN_
	memcpy(S1buff[0], M1buf+32, 16);	//桂林的次卡信息区在0扇区第2块。
#endif

	ii = calc_key(M1buf, buffer);

	MSG_LOG("S0 data:");
	BCD_LOG(M1buf, 48, 1);
	MSG_LOG("KEY:");
	BCD_LOG(buffer, 60, 1);


	memcpy(cardMessage.PublishKEYA, buffer+6, 6);//一扇区密钥
	memcpy(cardMessage.MoneyKEYA, buffer+12, 6);//二扇区密钥
	memcpy(cardMessage.PublichKEYA, buffer+18, 6);//其它密钥
	
	if (MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,cardMessage.PublishKEYA, 4)==0)// 读S1B0
	{
		return ST_ERROR;
	}
	for(ii=0; ii<3; ii++){
		memset(buffer, 0, sizeof(buffer));
		if (MifareRead(4,buffer) ==0) //S1B0
		{
			return ST_ERROR;
		}
		if (MifareRead(4,buffer+20) ==0) //S1B0
		{
			return ST_ERROR;
		}
		if(memcmp(buffer, buffer+20, 16) == 0)
			break;
	}
#ifdef _debug_
	debugstring("S1B0 data:");
	debugdata(buffer, 16, 1);
#endif
	if((buffer[0] != 0x50)&&(buffer[0] != 0x51)&&(buffer[0] != 52))
		return ST_ERROR;
	
	ii = bcd2int(buffer+2, 4);//2 ~ 5：卡编号，CBCD
	memcpy(cardMessage.PublishBicker, (unsigned char*)&ii, 4);
	cardMessage.card_catalog = buffer[1];//卡类
	
#ifndef BUS_GUILIN_   //桂林的次卡信息区在0扇区第2块。这里不能读
	if (MifareRead(5,S1buff[0]) ==0) //S1B1  
	{
		return ST_ERROR;
	}
#endif
	
	cardMessage.stuffNO[0] = 0;
	memcpy(cardMessage.stuffNO+1, S1buff[0]+12, 3);//12 ~ 14：员工编号，CBCD
	
#ifdef _debug_
	debugstring("PublishBicker:");
	debugdata(cardMessage.PublishBicker, 4, 1);
	debugstring("stuffNO:");
	debugdata(cardMessage.stuffNO, 4, 1);
	debugstring("S1B1 data:");
	debugdata(S1buff[0], 16, 1);
#endif
	if (MifareRead(6,S1buff[1]) ==0) //S1B2
	{
		return ST_ERROR;
	}
#ifdef _debug_
	debugstring("S1B2 data:");
	debugdata(S1buff[1], 16, 1);
#endif

	M1buf[0] = 0x20;
	memcpy(M1buf+1, S1buff, 3);
	memcpy((unsigned char*)&endtime, M1buf, 4);
	over_turn(4, (unsigned char*)&endtime);
	memcpy((unsigned char*)&nowtime, (unsigned char*)&SysTime, 4);
// 	if((endtime <= nowtime)&&(cardMessage.card_catalog!=0x01)){//过期卡
// 		cls();
// 		display(0,0,"错误",1);
// 		display(3,0,"此卡没有年审!", DIS_CENTER);
// 		beep(3, 100, 200);
// 		led_on(LED_RED);
// 		audio(Audio_YWJYu);//请年审,需要更改
// 		restore_flag = 3;
// 		end_card();
// 		return ST_ERROR;
// 	}

	if(IsInBlackList(cardMessage.PublishBicker) < FLASH_BLK_END){
		cls();
		display(0,0,"错误",1);
		display(3,0,"黑名单卡!", DIS_CENTER);
		audio(Audio_GuaShi);
		led_on(LED_RED);
		restore_flag = 3;
		end_card();
		return ST_ERROR;
	}

	cardMessage.card_catalog = YiDaCardTypeChange(S1buff[0][0], S1buff[0][1]);
	
	if(cardMessage.card_catalog == CARD_STUFF_BUS){//普通 员工卡
		if(DriverCardDeal() == ST_OK){
			end_card();
			return ST_ERROR;//员工司机卡签到，签退正常。
		}
	}


	
	MSG_LOG("KEYo:");
	BCD_LOG(cardMessage.PublishKEYA, 6, 1);
	BCD_LOG(cardMessage.MoneyKEYA, 6, 1);
	BCD_LOG(cardMessage.PublichKEYA, 6, 1);

	return ST_OK;

}

//钱块减值
unsigned char DECSector(unsigned char sector, unsigned int value)		
{  //mode=0
	unsigned char keyAA[6];
	unsigned char i=0,t;
	unsigned char buffer[60];
	unsigned char buffer_9[16];
	unsigned char Check_TRANSFER=ST_OK;
	unsigned char buffer_10[16];
	unsigned char block9=ST_OK;
	unsigned char block10=ST_ERROR;
//	unsigned int price,plus;
//	unsigned char price_ratio,plus_ratio;//,r;
	unsigned char block910=ST_OK;
	unsigned int eemoney=0;//,temp10,tempp;

#ifdef _debug_
	debugstring("Purse DECSector s\r\n");
#endif
	//检测错误机制
	for(i=0;i<error_pointer;i++)
	{
		if(ErrorDetail[i].effFlag!=IS_EFFECT_MONEY) continue;
		else
		{
			if(memcmp(ErrorDetail[i].Cardno,&c_serial,4)==0)
			{
				switch(ErrorDetail[i].ErrorStep)
				{
				case OPER_TRANSFER_NO:
					{
						memcpy((unsigned char *)&eemoney,ErrorDetail[i].AferToll,4);
						memcpy((unsigned char *)&s_sum1, ErrorDetail[i].TollValue, 4);
						Check_TRANSFER=ST_ERROR;
						break;
					}
				case OPER_RECORD_PROFILE:
				case OPER_PUBLIC_MESSAGE:
				case OPER_PUBLIC_MESSAGE_W:
					return 5;
				default:
					break;
				}
			}
		}
	}

	if(sector == 2)
		memcpy(keyAA, cardMessage.MoneyKEYA, 6);
	else
		memcpy(keyAA, cardMessage.PublichKEYA, 6);
#ifdef _debug_
	debugstring("KEYAA:");
	debugdata(keyAA, 6, 1);
#endif

	for(t=0; t<3; t++){
#ifdef _debug_
		debugstring("read Money Block:;");
#endif
	for(i=0;i<3;i++){
		if (MifareAuthKey(PICC_AUTHENT1A,(unsigned char *)&c_serial,keyAA,sector*4)!=0)
		{
			break;
		}
		debugstring("Purse Process MifareAuthKey error\r\n ");
		InitRC531();
		profile(0);
	}
	if(i>=3){
		debugstring("Purse Process MifareAuthKey error return\r\n ");
		return ST_ERROR;
	}
	//读钱包信息块
// 	if(MifareRead(sector*4,buffer)==0)
// 	{
// #ifdef _DEBUG
// 		error(ERR_MIFARE_READ,sector*4);
// #endif
// 		return ST_ERROR;
// 	}
// 	
	//读钱包的正本
	if(MifareRead(sector*4,buffer)==0)
	{
		debugstring("MifareRead money*4+1 ERROR!\r\n");
		continue;;
	}
#ifdef _debug_
	debugstring("钱包块正本：\r\n");
	debugdata(buffer,16,1);
#endif
	memcpy(buffer_9,buffer,16);
		
	if(MifareRead(sector*4+1,buffer)==0)
	{
		debugstring("MifareRead money*4+2 ERROR!\r\n");
		continue;
	}
	memcpy(buffer_10,buffer,16);
	break;
	}
	if(t>=3){
		debugstring("MifareRead money*4 ERROR return!\r\n");
		return ST_ERROR;
	}
#ifdef _debug_
	debugstring("钱包块副本：\r\n");
	debugdata(buffer,16,1);
#endif

	if(MifIsPurse(buffer_9)==ST_ERROR) 
	{
		block910=ST_ERROR;
		goto ydmey_step6;
	}
	else {
		block9=ST_OK;
		block910=ST_OK;
	}
	if(MifIsPurse(buffer_10)==ST_ERROR) 	
	{
		block910=ST_OK;
		goto ydmey_step6;
	}
	else 
		block10=ST_OK;
	
	if((block9==ST_ERROR)&&(block10==ST_ERROR)) return RET_MONEY_SN_EROR;
	
	if((block9 == ST_OK)&&(block10 == ST_OK)){
		for(i=0;i<4;i++)
		{
			if(buffer_9[3-i]>buffer_10[3-i])
			{
				block910=ST_ERROR;
				break;
			}
			if(buffer_9[3-i]<buffer_10[3-i])
			{
				block910=ST_OK;
				break;
			}
		}
	}
ydmey_step6:
	if(block910==ST_ERROR) 
		memcpy(buffer_9,buffer_10,4);
	
	memcpy((unsigned char*)&a_sum1, buffer_9, 4);

	s_sum1 = value;

	//用户原额
#ifdef _debug_
	debugstring("卡原额:");
	debugdata((unsigned char *)&a_sum1, 4, 1);
#endif
	if(Check_TRANSFER==ST_ERROR)
	{
		if(eemoney>=a_sum1)
			goto mey_step10;
	}
	
	gucSoudnorm = 0;
	if((a_sum1>=s_sum1)&&(a_sum1<=200)){//当钱包金额小于2元时	请充值
		gucSoudnorm = 1;
	}

	if((s_sum1 == 0) || (s_sum1 > 90000))
		return 22;

	if (a_sum1 >= 500000) 
	{
		return 21;
	}
	
	if(a_sum1 >= s_sum1)
		a_sum1 -= s_sum1;										//计算余额
	else{
		return 3;
	}
	if(s_sum1 == 0)
		return 5;
	
	memcpy(buffer_9+4, (unsigned char*)&s_sum1, 4);
	
	for(i=0;i<3;i++)
	{
		if(block910==ST_ERROR)
		{
			memcpy(buffer,buffer_9+4,4);
			if(MifareValueDebit(PICC_DECREMENT,sector*4+1,buffer))
				goto ydmey_step9;
		}
		else
		{
			memcpy(buffer,buffer_9+4,4);
			if(MifareValueDebit(PICC_DECREMENT,sector*4,buffer))
				goto ydmey_step9;
		}
	}
	debugstring("MifareValueDebit ERROR!\r\n");
	return ST_ERROR;
ydmey_step9:
	for(i=0;i<1;i++)
	{
		if(MifareTransfer(sector*4))
			break;
	}
	for(i=0;i<3;i++)
	{
		if(MifareRead(sector*4,buffer))
		{
			if(memcmp(buffer,(unsigned char*)&a_sum1,4)==0)
				goto ydmey_step90;
		}
	}
	if(i==3)
	{
		CheckSno(ERROR_MONEY,OPER_TRANSFER_NO,buffer);
		return 4;
	}
ydmey_step90:
	for(i=0;i<3;i++)
	{
		memset(buffer,0,4);
		if(MifareValueDebit(PICC_DECREMENT,sector*4,buffer))
			break;
	}

	for(i=0;i<1;i++)
	{
		if(MifareTransfer(sector*4+1))
			break;
	}
	
#ifdef _debug_
	debugstring("Purse over!\r\n");
#endif
mey_step10:
#ifdef _debug_
	debugstring("Purse OK!\r\n");
#endif
	ErrorOper(IS_EFFECT_MONEY);
	return 5;
}


int YidaPuchase(void)
{
	unsigned char pabuf[72];
	unsigned char ret;
	unsigned char Msec;//需要扣的扇区
	unsigned int Dmoney;//需要扣的钱

	if(month_decide()) // 第二次去扣钱包(次数不能连刷)
	{
		cls();
		led_on(LED_RED);
		restore_flag = 3;

		getCardtype((char*)pabuf, cardMessage.card_catalog);
		display(0, 0, (char*)pabuf, DIS_CENTER);

		display(4,1,"此卡不能带人乘车",DIS_CENTER);
#ifdef SOUND_New_B
		audio(Audio_NODRen);
#else
		audio(Audio_FLING_MONEY);
#endif
		beep(3, 100, 200);
		end_card();
		return -1;	
	}
	if(Delay_Normal())
		return -1;

	GetDateTime();
	if(cardMessage.card_catalog == CARD_NORMAL){//01：A 卡, 成人卡, 电子钱包, S2 ,可带   就是普通卡
		Msec = 2;
		Dmoney = get_s_sum1(0);
	}
	else if(cardMessage.card_catalog == CARD_YOUHUI_BUS){//A3: B 卡, 月票卡,当月有效,过月作废,扣S4、S5、S6、S7,不可带人； 月票无效时扣钱包S2,可带人
		if((SysTime.month == 1)||(SysTime.month == 5)||(SysTime.month == 9)){
			ret = bcd2int((unsigned char*)&SysTime.month, 1);
			debugstring("S0M:");
			debugdata((unsigned char*)&ret, 1, 1);
			if((((S1buff[0][2]>>4)&0x0F) == ret)&&(S1buff[0][0] == SysTime.year_l)){
				Msec = 4;
				Dmoney = get_Month_s_sum1();
				Dmoney *= 100;//扣次
			}
			else{
				Msec = 2;//扣钱?
				Dmoney = get_s_sum1(0);
			}
		}
		else if((SysTime.month == 2)||(SysTime.month == 6)||(SysTime.month == 0x10)){
			ret = bcd2int((unsigned char*)&SysTime.month, 1);
			debugstring("S1M:");
			debugdata((unsigned char*)&ret, 1, 1);
			if(((S1buff[0][2]&0x0F) == ret)&&(S1buff[0][1] == SysTime.year_l)){
				Msec = 5;
				Dmoney = get_Month_s_sum1();
				Dmoney *= 100;//扣次
			}
			else{
				Msec = 2;//扣钱?
				Dmoney = get_s_sum1(0);
			}
		}
		else if((SysTime.month == 3)||(SysTime.month == 7)||(SysTime.month == 0x11)){
			ret = bcd2int((unsigned char*)&SysTime.month, 1);
			debugstring("S2M:");
			debugdata((unsigned char*)&ret, 1, 1);
			if((((S1buff[0][5]>>4)&0x0F) == ret)&&(S1buff[0][3] == SysTime.year_l)){
				Msec = 6;
				Dmoney = get_Month_s_sum1();
				Dmoney *= 100;//扣次
			}
			else{
				Msec = 2;//扣钱?
				Dmoney = get_s_sum1(0);
			}
		}
		else if((SysTime.month == 4)||(SysTime.month == 8)||(SysTime.month == 0x12)){
			ret = bcd2int((unsigned char*)&SysTime.month, 1);
			debugstring("S4M:");
			debugdata((unsigned char*)&ret, 1, 1);
			if(((S1buff[0][5]&0x0F) == ret)&&(S1buff[0][4] == SysTime.year_l)){
				Msec = 7;
				Dmoney = get_Month_s_sum1();
				Dmoney *= 100;//扣次
			}
			else{
				Msec = 2;//扣钱?
				Dmoney = get_s_sum1(0);
			}
		}
		else{
			return -100;
		}
	}
	else if(cardMessage.card_catalog == CARD_ZILU_BUS){//C 卡, 学生优惠卡, 年审期内扣S4,电子钱包,不可带人；过年审期扣钱包S2,可带人
		Msec = 4;
		Dmoney = get_s_sum1(0);
	}
// 	else if((cardMessage.card_catalog == 0xA5)||(cardMessage.card_catalog == 0xA7)
// 		||(cardMessage.card_catalog == 0xB0)||(cardMessage.card_catalog == 0xB1)||(cardMessage.card_catalog == 0xB3)||(cardMessage.card_catalog == 0xB2)){//A5 : E 卡, 福利(老人)卡,季票卡,当季有效,过季作废,扣S4、S5、S6、S7,不可带人；季票无效时扣钱包S2,可带人
	else if(cardMessage.card_catalog != 0xFF){		
		if((SysTime.month == 1)||(SysTime.month == 2)||(SysTime.month == 3)){//第一季度
			debugstring("J0M:");
			if((((S1buff[0][2]>>4)&0x0F) == 1)&&(S1buff[0][0] == SysTime.year_l)){
				Msec = 4;
				Dmoney = get_Month_s_sum1();
				Dmoney *= 100;//扣次
			}
			else{
				Msec = 2;//扣钱?
				Dmoney = get_s_sum1(0);
			}
		}
		else if((SysTime.month == 4)||(SysTime.month == 5)||(SysTime.month == 6)){
			if(((S1buff[0][2]&0x0F) == 2)&&(S1buff[0][1] == SysTime.year_l)){
				Msec = 5;
				Dmoney = get_Month_s_sum1();
				Dmoney *= 100;//扣次
			}
			else{
				Msec = 2;//扣钱?
				Dmoney = get_s_sum1(0);
			}
		}
		else if((SysTime.month == 7)||(SysTime.month == 8)||(SysTime.month == 9)){
			if((((S1buff[0][5]>>4)&0x0F) == 3)&&(S1buff[0][3] == SysTime.year_l)){
				Msec = 6;
				Dmoney = get_Month_s_sum1();
				Dmoney *= 100;//扣次
			}
			else{
				Msec = 2;//扣钱?
				Dmoney = get_s_sum1(0);
			}
		}
		else if((SysTime.month == 0x10)||(SysTime.month == 0x11)||(SysTime.month == 0x12)){
			if(((S1buff[0][5]&0x0F) == 4)&&(S1buff[0][4] == SysTime.year_l)){
				Msec = 7;
				Dmoney = get_Month_s_sum1();
				Dmoney *= 100;//扣次
			}
			else{
				Msec = 2;//扣钱?
				Dmoney = get_s_sum1(0);
			}
		}
		else{
			return 100;
		}
	}
	else
		return -1;

	if(((Msec < 4)||(Msec > 7))&&(Msec != 2))
	{
		led_on(LED_RED);
		MessageBox(0, "余额不足!");
		SoundMessage(SOUND_FLING_MONEY);//请投币
		end_card();
		return 100;
	}

	if((NewPriceTab.rate[cardMessage.card_catalog-0x40] == 104)||(NewPriceTab.rate[cardMessage.card_catalog-0x40] == 204)){
		MessageBox(1, "此卡禁止刷卡!");
		led_on(LED_RED);
		restore_flag=3;
		end_card();
		return CARD_forbid;//此卡被禁止使用
	}

	ret = DECSector(Msec, Dmoney);

	if((ret == 2)||(ret == 3)){//余额不足
		if(Msec != 2){			//不是扣钱包，
			Msec = 2;
			Dmoney = get_s_sum1(0);
			ret = DECSector(Msec, Dmoney);
		}
		else{
			led_on(LED_RED);
			MessageBox(0, "余额不足!");
			SoundMessage(SOUND_FLING_MONEY);//请投币
			end_card();
			return -8;
		}
	}

	switch(ret)    //96ms
	{
	case ST_ERROR:						//普通错误
		break;
	case 21:
	case RET_MONEY_SN_EROR:																//两个钱包都错误
		error(ERR_MONEY_SN, 0);
		end_card();
		return -2;
	case 22:
		display(0,0,"提示:", DIS_Cls);
		display(2,0,"票价信息不正确",DIS_CENTER);
		end_card();
		return -3;
	case 2:
	case 3://余额不足
		led_on(LED_RED);
		MessageBox(0, "余额不足!");
		SoundMessage(SOUND_FLING_MONEY);//请投币
		end_card();
		return -8;
	case 4:
	case 6:
		audio(Audio_TRY_AGAIN);		//请重刷
		card_ser = c_serial;
		cardSound = 0xaa;
		break;
	case 5:
	case 7:
	case 8:
		if((Msec == 2)||(cardMessage.card_catalog == CARD_STUDENT_BUS)){
			ComSumeFile.SumMoney+=s_sum1; //当前消费总额
			BuildRecorde(ID_REC_TOLL, pabuf);
			Dmoney = ((unsigned char*)&ComSumeFile.SumMoney)-((unsigned char*)&ComSumeFile);
			sysfewrite(BIT_COMSUME_FILE+Dmoney, 4 ,(unsigned char*)&ComSumeFile.SumMoney);
		}
		else{
			a_sum1 /= 100;//扣次.
			s_sum1 /= 100;
			dis_sum2 = a_sum1;
			ComSumeFile.SumMti+=1; //当前消费总额
			BuildRecorde(ID_REC_MON, pabuf);
			Dmoney = ((unsigned char*)&ComSumeFile.SumMti)-((unsigned char*)&ComSumeFile);
			sysfewrite(BIT_COMSUME_FILE+Dmoney, 4 ,(unsigned char*)&ComSumeFile.SumMti);
		}
		card_ser = 0;
		cardSound=0;
 		//if((cardMessage.card_catalog!=CARD_NORMAL))
		if(Msec == 2)//只要是扣钱包区都可带人
			save_normal();
		else
			save_h_month();

		if((Msec == 2)||(cardMessage.card_catalog == CARD_STUDENT_BUS))
			money_msg(ID_REC_TOLL, a_sum1, s_sum1, 0);
		else
			money_msg(ID_REC_MON, a_sum1, s_sum1, 0);
#ifdef _debug_
		debugstring("gucSoudnorm1=");
		debugdata((unsigned char*)&gucSoudnorm, 1, 1);
#endif
		if((Msec == 2)&&(cardMessage.card_catalog != CARD_NORMAL))
			audio(Audio_DONG);
		else
			SoundMessage(SOUND_DEAL);
		if(gucSoudnorm == 1){//还需要提示请充值
			delayxms(600);
			SoundMessage(SOUND_CHARGE_MONEY);
		}

		WriteRecord(pabuf, 0);
#ifdef _debug_
		debugstring("money puchase ok!!\r\n");
#endif
		end_card();
		g24GDisFlash = 5;
		restore_flag = 0;
//			PleaseYearCheck((struct com_time *)nowTime.charbuffer,(struct com_time *)endTime.charbuffer);
		return 0;
	default:
		break;
	}		
	return -5;
}

void YiDaMainCard(void)
{
	unsigned char repornd;
#ifdef _debug_
	debugstring("yi Da card deal~!!\r\n");
#endif

	repornd = YiDagetCardtype();

	if(F_FLAG_VARY.stop_flag)
		return;
	if(repornd != ST_OK)
		return;
	nnAudioflag = 0;
	gMCardCand = CARDSTYLE_YIDAM1;
	restore_flag = 3;
	YidaPuchase();
}

#endif
