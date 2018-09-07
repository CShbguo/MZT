
/*
	建设部CPU卡模式 

	pfquan

	2015-12-10

  注意：本文件应该和CPUCard.C 文件不同时使用，也就是不能同时打开   BUS_CPU_  和 CPU_JSB
  记录格式采用的是64字节的格式，和最早十堰的类似

  */
#include "szct.h"

#if ((defined CPU_JSB) )



#include "Cpucard.h"
#include "E_CPUCard.h"


#include "citydifferent.h"
#include "E_GJGPRS.h"
#include "Mydes.h"

#include "Font.h"
#include "citydifferent.h"
#include "Gszct.h"



#include "CPU_Union.h"


#define PSAM_JSB		SAM4

#define RECORD_LEN_CPU  RECORD_LEN
#define CARD_CITY_CODE	"\x44\x20"			//城市编号  

#define WHITELEN		 1000
#define WHITE_SNO_LEN		2 
//不适用白名单就打开这个
#define NO_WHITE_LIST









//下面是和24G手机卡共用的变量

// sam_pub SamPubInf_LL;
unsigned int a_sum1_M=0;		//月票上一次余额



extern unsigned int g24GDisFlash;//24G卡界面刷新
extern unsigned char g24PSAMDsno[4];//终端序列号
//extern typea_public_inf_zaozuang stuPubInfZaoZuang;
extern pboc_debit_capp_purchase stuDebitPurchase;
extern pboc_sam_init_purchase stuSamInitPurchase;
extern pboc_init_capp_purchase_res stuInitPurchaseRet;

extern sam_pub SamPubInf_LL;


jsb_rec jsb_recbuff;		//
// unsigned char Safe_Code[9];		//安全认证码

unsigned short WhiteLength;		////下载白名单的时记录条数用








extern void Sound_cityCMD(unsigned char sMode);
extern void SoundMessage(unsigned char cmd);
extern void money_msg(unsigned char dmode, unsigned int remM, unsigned int pucM, unsigned char cMOde);
extern void dis_time_mini(unsigned char x, stcomtime *time);
extern unsigned char WriteRecord(unsigned char *buffer, unsigned char mode);
extern void BlightOpen(void);
extern void save_normal(void);
extern void save_h_month(void);
extern unsigned char month_decide(void);
extern void CountPoint(unsigned char mode);
extern unsigned int IsInBlackList(unsigned char *c_card_no);
extern unsigned char FreeConsume(void);
extern void disp_no_swipe(void);
extern void error(unsigned char code1,unsigned char Index);
extern unsigned char GetDateTime(void);

extern void ControlDis(unsigned char type);
extern unsigned char Delay_Normal(void);
extern void SetTcpPacketTTLTime(unsigned char val);
	extern void ControlMagane(void);

//取CPU中的线路号信息.
extern int getCpuLineNo(void);
extern void getCpuInfo(stCpuInfo *inP);
extern void saveCpuInfo(unsigned char mode, unsigned char *dat);
extern void savesysbupara(void);

extern void addStatMoney(unsigned char mode, unsigned int moneyv);
extern void Save_delayCardinfo(unsigned char Cmode);
extern unsigned int IsInWhileList(unsigned char *c_card_no);
extern unsigned char CpuCardSetMac(unsigned char PsamID, unsigned char *pMac, unsigned char *pData, unsigned char len);
unsigned char CPUMAC2(unsigned char PsamID, unsigned char *pMac, unsigned char *pData, unsigned char len);
extern unsigned char CPU_Read_Money(unsigned char *remb, unsigned char mode);
extern unsigned char gPSAMin;//psam卡所在位置
extern unsigned char GetDateTime(void);
extern void deal2or3(unsigned char*sndd);
extern int getCardtype(char *cardD, unsigned char type);
extern void getbusticketVer(void);
extern void getCpuDeviceNO(unsigned char *dat);

extern void file_15_line(unsigned char *file_15);

extern unsigned char PsamInitialize(void);


unsigned int IsWhiteBlock(unsigned char *c_card_no )
{
	unsigned int i_start=0;
	unsigned int i_end;
	unsigned int i_cur, addr;
	unsigned char i;
	unsigned char temp[WHITE_SNO_LEN];
	unsigned int invalid_point;
	unsigned char buffer[WHITE_SNO_LEN];
#ifdef _debug_white
	char disbuff[64];
#endif
	
	memcpy(temp,c_card_no,WHITE_SNO_LEN);
	
	i_end = WhiteLength -1;
	
#ifdef _debug_white
	debugstring("White==  current Card NO.:");
	debugdata(temp,WHITE_SNO_LEN,1);
	debugstring("\r\n");
	debugdata((unsigned char *)&i_end,4,1);
#endif 
	
	while(1)
	{
		i_cur=i_start+(i_end-i_start)/2;
		invalid_point=i_cur*(WHITE_SNO_LEN +2);			//暂时只用了前2个字节
	
		addr = FLASH_WHITE_START + 6 +invalid_point;//
		
		
#ifdef _debug_white
// 		debugstring("invalid_point:");
// 		debugdata((unsigned char *)&invalid_point, 4, 1);
		memset(disbuff,0,sizeof(disbuff));
		sprintf(disbuff,"start:%d\r\n",i_start);
		debugstring(disbuff);
		memset(disbuff,0,sizeof(disbuff));
		sprintf(disbuff,"i_cur:%d\r\n",i_cur);
		debugstring(disbuff);
		memset(disbuff,0,sizeof(disbuff));
		sprintf(disbuff,"i_end:%d\r\n",i_end);
		debugstring(disbuff);
#endif
		flashread(addr, buffer, WHITE_SNO_LEN);
#ifdef _debug_white
		debugstring("read:");
		debugdata(buffer,WHITE_SNO_LEN,1);
#endif
		for(i=0;i<WHITE_SNO_LEN;i++)
		{
// 			if(temp[WHITE_SNO_LEN-1-i]==buffer[WHITE_SNO_LEN-1-i])
// 				continue;
// 			else if(temp[WHITE_SNO_LEN-1-i]>buffer[WHITE_SNO_LEN-1-i])
			if(temp[i]==buffer[i]){
#ifdef _debug_white
				debugstring("Y");
#endif
				continue;
			}
			else if(temp[i]>buffer[i])

			{
				if(i_end==i_cur)
					return 0xffffffff;
				else
				{
					i_start=i_cur+1;
#ifdef _debug_white
					debugstring("往后---\r\n");
#endif

					break;
				}
			}
			else
			{
				if(i_start==i_cur)
					return 0xffffffff;
				else
				{
#ifdef _debug_white
					debugstring("往前---\r\n");
#endif

					i_end=i_cur;
					break;
				}
			}
		}
		if(i==WHITE_SNO_LEN){
			return addr;
		}
	}
}
//0 不在   1 在
unsigned int IsInWhileList(unsigned char *c_card_no)
{
//	unsigned char i;
	unsigned int addr;

#ifdef _debug_white
	debugstring("city_code::");
	debugdata(c_card_no,2,1);
	debugstring("WhiteLength:");
	debugdata((unsigned char *)&WhiteLength,2,1);
#endif

#ifdef NO_WHITE_LIST
	debugstring("return !!!!!!!!!!!!!!!!!!!!!!!\r\n");
	return 1;

#else


	if (memcmp(c_card_no,CARD_CITY_CODE,WHITE_SNO_LEN) == 0)
	{
		return 1;
	}else{
		if (WhiteLength == 0)
			return 0;
		
		addr = IsWhiteBlock(c_card_no);
		#ifdef _debug_white
			debugstring("addr:::");
			debugdata((unsigned char *)&addr,4,1);
		#endif
		if(addr < FLASH_WHITE_END)
			return 1;
	}	
	
	return 0;
	
#endif
	
	
}
//无效返回0  有效返回条数
unsigned int load_white_list(void)
{
	unsigned char buftemp[32];
	unsigned int addr;
	unsigned short len;
#ifdef _debug_white
	unsigned int i;
#endif
#ifdef _debug_
	debugstring("加载白名单:BCD::\r\n");
#endif

	addr = FLASH_WHITE_START;
	flashread(addr,buftemp,6);
#ifdef _debug_
	debugstring("read:");
	debugdata(buftemp,6,1);
#endif
	if ( memcmp(buftemp,"有效",4)==0 )
	{
#ifdef _debug_
		debugstring("白名单有效=====\r\n");
#endif
		memcpy((unsigned char*)&len,buftemp+4,2);
		over_turn(2, (unsigned char *)&len);
		len = len/(WHITE_SNO_LEN+2);		//flash中存的是4字节的，实际用前两字节，

		WhiteLength =len;
		
		if ( WhiteLength > WHITELEN)
		{
			WhiteLength = 0;
			len = 0;
			flasherase(ERASE4KBYTESECTOR,addr);
			memcpy(buftemp,"无效",4);
			memcpy(buftemp+4,&WhiteLength,2);
			flashwrite(addr,buftemp,6);
		}
		
	}else{
#ifdef _debug_
		debugstring("没有白名单::\r\n");
#endif
		WhiteLength = 0;
	}


#ifdef _debug_white
	if (WhiteLength != 0)
	{

		debugstring("WhiteLength:");
		debugdata((unsigned char *)&WhiteLength,2,1);
		for (i=0;i<WhiteLength;i++)
		{
			addr = FLASH_WHITE_START + 6 + i*(WHITE_SNO_LEN+2);	//地址增4byte，存的是4字节的，用2字节
			flashread(addr,buftemp,WHITE_SNO_LEN+2);
			if (i%300 == 0)
			{
				clr_wdt();
			}
			debugdata(buftemp,WHITE_SNO_LEN+2,1);
		}
	}
#endif 



	return WhiteLength;


}





extern unsigned int get_Month_s_sum1(void);


#ifdef _debug_quan			//调试各个时间段用
unsigned int time =0;
char distime[32];
unsigned char flag_dis=0;

#endif



extern stBusVerIfo gBusVerInfo;

extern unsigned char mode2or3;
//卡类型:手机卡03\艺达卡 04\公交卡01\CPU卡02
extern unsigned char gMCardCand;
extern stPricetable NewPriceTab;
extern Parameter5 cardMessage;
extern unsigned int a_sumR;//手机钱包中的真实金额
extern unsigned char DriveCardNo[4];
extern pFistVary_1 pFistVary;
extern Parameter3 ComSumeFile;
extern unsigned int c_serial;
extern unsigned char cardSound;			//刷卡没有成功标志
extern unsigned int card_ser;
extern Parameter2 POINT_MESSAGE;
extern unsigned char gchn_eng;//中英文标志
extern unsigned char BUflag;//车载机状态 0x02 需要刷司机卡 
extern unsigned char gucSoudnorm;

extern unsigned char gPSAMin;//psam卡所在位置

extern volatile unsigned char StufferNO_valid;		//消费记录中的员工卡编号有效则为1，这个标志由员工卡来更改，默认为0
extern unsigned char StufferNO[4];
extern unsigned char DriveCardNo[4];


void load_unknow(void)
{
	unsigned char buffer[RECORD_LEN_CPU+2];

	feread(BIT_UNKNOW_RECORD,2,buffer);
#ifdef _debug_
	debugstring("读灰记录------\r\n");
	debugstring("记录头:");
	debugdata(buffer,2,1);
#endif
	
	if (memcmp(buffer,"\x55\xAA",2)==0)
	{
		
		feread(BIT_UNKNOW_RECORD+2,RECORD_LEN_CPU,buffer);
		debugstring("写正式记录=====\r\n");
		debugdata(buffer,RECORD_LEN_CPU,1);
		WriteRecord(buffer, 0);
		
		
	}

}
//操作灰记录，写1,删3
unsigned char Unknow_Record(unsigned char mode ,unsigned char *record,unsigned int rlen)
{
	unsigned char buffer[RECORD_LEN_CPU];
	unsigned int i=0;
	
	
	
	switch(mode)
	{
		
	case 1:
#ifdef _debug_
		debugstring("写灰记录====\r\n");
#endif
		
		
		for (i=0;i<3;i++)
		{
			
			memset(buffer,0,sizeof(buffer));
			fewrite(BIT_UNKNOW_RECORD,2,"\x55\xaa");
			fewrite(BIT_UNKNOW_RECORD+2,RECORD_LEN_CPU,record);
			feread(BIT_UNKNOW_RECORD+2,RECORD_LEN_CPU,buffer);
			
			if (memcmp(record,buffer,RECORD_LEN_CPU)==0)
			{
#ifdef _debug_
				debugstring("写灰完成:");
				debugdata(buffer,RECORD_LEN_CPU,1);
#endif
				return ST_OK;
			}
		}
		
#ifdef _debug_
		debugstring("写灰失败----\r\n");
#endif
		
		
		break;
	case 3:
#ifdef _debug_
		debugstring("删成功====\r\n");
#endif
		memset(buffer,0,2);
		fewrite(BIT_UNKNOW_RECORD,2,buffer);
		
		//fewrite(BIT_UNKNOW_RECORD,RECORD_LEN_ZZ,buffer);
		
		return ST_OK;
		
		
	default:
		break;
	}
	
	return ST_ERROR;
	
	
}

extern unsigned int ghisCur;//历史当前指针


unsigned char CPU_JSB_history(unsigned char *dat,unsigned int Index)
{
	unsigned char buffer[80];
	unsigned int temp;//,temp1;
//	unsigned char i;
	Recode_CPU_JSB  *historyCheck;
//	char btff[20];

	MSG_LOG("CPU_JSB_histo***-\r\n");

// 	memcpy(&historyCheck,dat,RECORD_LEN_CPU);
	historyCheck = (Recode_CPU_JSB *)dat;
	temp=0;


	if (memcmp(historyCheck->record_Flag,"\x55\xaa",2)!=0)
	{
		return ST_ERROR;
	}

	MSG_LOG("rDealType:%d\r\n",historyCheck->rDealType);
	switch(historyCheck->rDealType)
	{

	case ID_REC_TOLL:
		cls();

		memset(buffer,0,20);
		temp=0;
		memcpy((unsigned char*)&temp,historyCheck->rDealMoney,3);
		sprintf((char*)buffer, _Font_KouKuandian, temp/100, temp%100);
// 		memset(btff, 0, 20);//加入卡类
//		getCardtype(btff, historyCheck->rCardType);
//		strcat((char*)buffer, btff);

		display(2,0,(char*)buffer,0);
		memset(buffer,0,20);
		temp=0;
		memcpy((unsigned char*)&temp, historyCheck->rAfterMoney, 4);
		
		sprintf((char*)buffer, _Font_Balancedian, temp/100, temp%100);
		display(4,0,(char*)buffer,0);
		
		dis_time_mini(6, (stcomtime*)historyCheck->rDealTime);
		break;
	case ID_REC_MON:
		cls();
		memset(buffer,0,20);
		temp=0;
		memcpy((unsigned char*)&temp,historyCheck->rAfterMoney, 4);

	//	memcpy(buffer,(unsigned char *)"扣次:1",6);
		//sprintf(buffer+5,"%lu",temp->longbuffer);
		temp = 0;
		memcpy((unsigned char*)&temp,historyCheck->rDealMoney,3);
		if(temp == 0){
			getCardtype((char*)buffer, historyCheck->rCardType);
			display(3,0,(char*)buffer, DIS_CENTER);
		}
		else{
			temp=0;
			memcpy((unsigned char*)&temp,historyCheck->rDealMoney,3);
			sprintf((char*)buffer, _Font_KouCIdian, temp);

//			memset(btff, 0, 20);//加入卡类
//			getCardtype(btff, historyCheck->rCardType);
//			strcat((char*)buffer, btff);
			display(2,0,(char*)buffer,0);

			memset(buffer,0,20);
			memcpy((unsigned char*)&temp, historyCheck->rAfterMoney, 4);
			sprintf((char*)buffer, _Font_BalanceCI, temp);
			display(4,0,(char*)buffer,0);
		}
		break;

	case ID_REC_CLERK:
		cls();
		display(3, 0, Card_Name_ZWYcard, DIS_CENTER);
		break;
	case ID_REC_DRIVENO:
		cls();
		display(3, 0, Card_Name_Driver, DIS_CENTER);
		break;
	case ID_REC_LEVBUS:
		cls();
		display(3, 0, Card_Name_departcard, DIS_CENTER);
		break;
	case ID_REC_ARVBUS:
		cls();
		display(3, 0, Card_Name_stopcard, DIS_CENTER);
		break;
	case ID_REC_BLKLIST:
		cls();
		display(3, 0, Card_Name_Blkcard, DIS_CENTER);
		break;
	default:
		return ST_ERROR;
	}
	sprintf((char*)buffer, "%d ", Index+1);
	miniDispstr(7, (21-strlen((char*)buffer)), (char*)buffer, 0);
	memset(buffer,0,20);
	temp=0;
//	sprintf((char*)buffer+strlen((char*)buffer),":%010u", temp);
	BCD2Ascii(historyCheck->rPublishNumb_H,buffer,8);
// 		if (memcmp(historyCheck->city_code,CARD_CITY_CODE,2)==0)				//本城市CPU
// 		{
// 			memcpy((unsigned char*)&temp, historyCheck->rPublishNumb_L, 4);	
// 			sprintf((char*)buffer+8,"%08u", temp);
// 			
// 		}else{							
	// 		}
	
	display(0,0,(char*)buffer,0);
	dis_time_mini(6, (stcomtime*)historyCheck->rDealTime);
	
	display(0,0,(char*)buffer,0);
	dis_time_mini(6, (stcomtime*)historyCheck->rDealTime);
	return ST_OK;
}


//新的记录格式
void BuildRecorde_CPU_JSB(unsigned char delType, unsigned char *recBuf)
{

	//先统一赋值，再针对建设部记录，做进一步的调整
	Recode_CPU_JSB *rRecordDeal;
	unsigned int dealnum;		//交易流水号
	unsigned int uitemp;


	MSG_LOG("do %s\r\n",__FUNCTION__);
	
	sysferead(BIT_POINT_MESSAGE,POINT_MESSAGE_LEN,(unsigned char*)&POINT_MESSAGE);
	
	rRecordDeal = (Recode_CPU_JSB *)recBuf;

	memset(rRecordDeal,0,sizeof(Recode_CPU_JSB));


	memcpy(rRecordDeal->rPublishNumb_H,cardMessage.PublishBicker,4);
	memcpy(rRecordDeal->rPublishNumb_L,cardMessage.PublishBicker+4,4);

 	getCpuLineNo();
	getCpuDeviceNO(pFistVary.DeviceNo);						//重新取设备号
	Ascii2BCD(pFistVary.DeviceNo, rRecordDeal->rDeviceNo, 8);
	rRecordDeal->rCardType = cardMessage.card_catalog;
	rRecordDeal->rDealType = delType;
//	memcpy(rRecordDeal->city_code, CARD_PUBLIC.MoneyDealSum, 2);//累计交易次数
	memcpy(rRecordDeal->city_code,cardMessage.CityCode,2);

	memcpy(rRecordDeal->rDealTime, (unsigned char*)&SysTime, 7);

	memcpy(rRecordDeal->rLineNo, pFistVary.LineNo, 2);
	Get_SerialNum(rRecordDeal->rST_Serial);				//设备唯一序列号	


	memcpy(rRecordDeal->record_Flag,"\x55\xAA",2);		//新纪录标识


	memcpy(rRecordDeal->rBussinessNo, cardMessage.TradeCode, 2);//商户编号以卡内编号为准，不再取刷卡机内编号
	memcpy(rRecordDeal->rDriverNo, DriveCardNo, 4);
	memcpy(rRecordDeal->rProVer, gBusVerInfo.busProVer, 2);//程序版本

////////////////////////////交易序号//////////////////////////////////////
	
	if((delType&0x7F) == ID_REC_TOLL){
		s_sum1 = get_s_sum1(0);	//V817
		memcpy(rRecordDeal->rDealMoney, (unsigned char *)&s_sum1, 3);
		memcpy(rRecordDeal->rAfterMoney, (unsigned char*)&a_sum1, 4);
		dealnum =POINT_MESSAGE.MoneyDealPointer;
		POINT_MESSAGE.MoneyDealPointer++;
		if(POINT_MESSAGE.MoneyDealPointer > 0x3B9AC9FF)
			POINT_MESSAGE.MoneyDealPointer = 0;
		dealnum = POINT_MESSAGE.MoneyDealPointer;
		CountPoint(0);
	}
	else if((delType&0x7F) == ID_REC_MON){
		uitemp = s_sum1*100;								
		memcpy(rRecordDeal->rDealMoney, (unsigned char *)&uitemp, 3);
		uitemp = dis_sum2*100;								
		memcpy(rRecordDeal->rAfterMoney, (unsigned char*)&uitemp, 4);
		POINT_MESSAGE.MonthDealPointer++;
		if(POINT_MESSAGE.MonthDealPointer > 0x3B9AC9FF)
			POINT_MESSAGE.MonthDealPointer = 0;
		dealnum = POINT_MESSAGE.MonthDealPointer;

		memcpy(jsb_recbuff.deal_serial,&dealnum,4);

		CountPoint(1);
	}
	else{
		POINT_MESSAGE.ControlDealPointer++;
		if(POINT_MESSAGE.ControlDealPointer > 0x3B9AC9FF)
			POINT_MESSAGE.ControlDealPointer = 0;
		dealnum = POINT_MESSAGE.ControlDealPointer;
		CountPoint(2);
	}

//////////////////////////////////////////////////////////////////////////
	memcpy(rRecordDeal->rPSAM_DealNumb, (unsigned char*)&dealnum, 4);//M1卡本机交易流水
	CountRecMac((unsigned char*)&rRecordDeal, 60, rRecordDeal->rTAC);

	

#ifdef _debug_
		debugstring("CPU-------重写-------\r\n");
#endif
		memcpy(rRecordDeal->rDealTime, jsb_recbuff.deal_time, 7);
		memcpy(rRecordDeal->rPSAM_DealNumb,jsb_recbuff.deal_serial,4);
		memcpy(rRecordDeal->rCPUCard_DealNumb,jsb_recbuff.card_deal_serial,2);
		rRecordDeal->rdealtpye_JSB = jsb_recbuff.deal_type;
		rRecordDeal->rKeyEn        = jsb_recbuff.ckeyEn;
		memcpy(rRecordDeal->rPSAM_NO,jsb_recbuff.psam_num,6);	
		memcpy(rRecordDeal->rTAC,jsb_recbuff.TAC,4);


#ifdef _debug_
	debugstring("BLD REC:");
	debugdata(recBuf, 64, 1);
#endif
// 	debugstring("BLD REC:");
// 	debugdata(recBuf, 64, 1);

	
	//#endif  //#ifdef BUS_HANDAN_
	if(((delType&0x7F) == ID_REC_TOLL)||((delType&0x7F) == ID_REC_MON)){
		addStatMoney((delType&0x7F), s_sum1);
#ifdef _Counter_SWcard_times
		addSCtimes(1);//刷卡次数加1 
#endif
	}

}



unsigned char CPUPsamInitialize_JSB(void)
{
	unsigned char ii=0;
	//	unsigned char pos=0;
	unsigned char buffer[256];
	unsigned char uctemp=0;
	
	uctemp = PSAM_JSB;
	
	gPsamErrorFlag |= 1;
	
	display(0,0,"正在检测住建部PSAM卡",0);
	display(2,1,"请等待......",0);
	
	
#ifdef _debug_CPU_
	debugstring("PSAM init JSB cpu \r\n");
#endif
	
	
	ii = SamColdReset(uctemp, 38400, SAM_VOL3, buffer);	//福建一卡通的波特率真
	
	if(ii == 0){
		return ST_ERROR;
	}
	
#ifdef _debug_CPU_
	debugstring("GJPSAM art:");
	debugdata(buffer, ii, 1);//3B69000050534D5F4D4330536B
#endif
	delayxms(5);
	if(Read_Binary(uctemp, 0x16, 0, 6, buffer)==0) 
	{
#ifdef _debug_CPU_
		debugstring("GJRead_Binary error\r\n");
		debugdata(buffer, 8, 1);
#endif
		return ST_ERROR;
	}
#ifdef _debug_CPU_
	debugstring("GJread 16 Binary:");
	debugdata(buffer,6,1);	//364001000748
#endif
	
	memcpy(jsb_recbuff.psam_num,buffer,6);
	
	delayxms(100);
	
#ifdef _debug_CPU_
	debugstring("GJSelect File ");
#endif
	if(Select_File(uctemp, 0x1001)==0) //0x3f01
	{
		debugstring("GJSelect_File error\r\n");
		return ST_ERROR;
	}
	
	memcpy(buffer, "\xBD\xA8\xC9\xE8\xB2\xBF", 6);		//建设部psam卡互联互通应用标识
	if(Select_FileF(uctemp, buffer, 6) == 0){
		debugstring("Select_File error\r\n");
		return ST_ERROR;
	}
	if(Read_Binary(uctemp, 0x17, 0, 25, buffer)==0)		//读0x17文件25字节
		//if(Read_Binary(SAM1, 0x19, 0, 25, buffer)==0) 
	{
		debugstring("GJ_Read_Binary17 error\r\n");
		debugdata(buffer, 8, 1);
		return ST_ERROR;
	}
#ifdef _debug_CPU_
	debugstring("GJ_read 17 Binary:");
	debugdata(buffer,25,1);	//01000000000000000100000000000000012011122220161220
	debugstring("OK\r\n");
#endif
	memcpy((unsigned char*)&SamPubInf_LL, buffer, sizeof(sam_pub));	//第一个字节是密钥索引
	
	gPsamErrorFlag &= (~0x01);
	
#ifdef _debug_CPU_
	debugstring("CPU psaM ok\r\n");
#endif
	
	return ST_OK;
}




/*
unsigned char MonthManage_CPU(void)
{

	
	if(block29==ST_OK)
	{
		if(!buffer_29[dateSum]) 
		{
			return 100;
		}
		for(i=dateSum;i<13;i++)
			SUMb+=buffer_29[i];
		temp10 = 0;
		memcpy((unsigned char*)&temp10,buffer_29+13,2);
		SUMc=temp10 ;			//将来各月点数和
		
		if(block2930==ST_OK)
		{
			for(i=1;i<13;i++)
				SUMb+=buffer_30[i];
			temp10 = 0;
			memcpy((unsigned char*)&temp10 ,buffer_30+13,2);
			SUMc+=temp10;
		}
		SUMa=SUMb-buffer_29[dateSum];	
	}
	else if(block30==ST_OK)
	{
		if(!buffer_30[dateSum]) 
		{
			return 100;
		}
		for(i=dateSum;i<13;i++)
			SUMb+=buffer_30[i];
		
		temp10 = 0;
		memcpy((unsigned char*)&temp10 ,buffer_30+13,2);
		SUMc=temp10 ;
		if(block2930==ST_OK)
		{
			for(i=1;i<13;i++)
				SUMb+=buffer_29[i];
			
			temp10 = 0;
			memcpy((unsigned char*)&temp10 ,buffer_29+13,2);
			SUMc+=temp10 ;
		}
		SUMa=SUMb-buffer_30[dateSum];
	}
	if((SUMb<SUMa)||(SUMb>SUMc)||(SUMa>SUMc)) 
	{
		return 100;	
		}

	s_sum1 = 1;


	if((a_sum1>=1)&&(a_sum1<=3))		//月票卡不足3次
	{
		gucSoudnorm=1;
	}





	memcpy(month[0], revbuf+1, 32);
	
	memcpy(sndbuf,"\x02\x00\xB0\x99\x20\x20",6);//读应用序列号作为分散因子
	deal2or3(sndbuf);
	ret = RfPassthrough(revbuf, 2, sndbuf, 6);
	if(ret == 0)
		return ST_ERROR;

	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return ST_ERROR;
	memcpy(month[0]+0x20, revbuf+1, 32);
#ifdef _debug_CPU_
	debugstring("month[0]:");
	debugdata(month[0], 64, 1);
#endif
	
	memcpy(sndbuf,"\x02\x00\xB0\x9A\x00\x20",6);//读应用序列号作为分散因子
	deal2or3(sndbuf);
	ret = RfPassthrough(revbuf ,2, sndbuf, 6);
	if(ret == 0)
		return ST_ERROR;
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return ST_ERROR;
	memcpy(month[1], revbuf+1, 32);
	
	memcpy(sndbuf,"\x02\x00\xB0\x9A\x20\x20",6);//读应用序列号作为分散因子
	deal2or3(sndbuf);
	ret = RfPassthrough(revbuf, 2, sndbuf, 6);
	if(ret < 2)
		return ST_ERROR;
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return ST_ERROR;
	memcpy(month[1]+0x20, revbuf+1, 32);
	
#ifdef _debug_CPU_
	debugstring("month[1]:");
	debugdata(month[1], 64, 1);
#endif
	if(month[0][0] == SysTime.year_l)
		mothFlag = 0;  //当前年在0块
	else if(month[0][32] == SysTime.year_l)
		mothFlag = 32;	//当前年在1块
	else
		return 3;//月票不正确，去扣钱,没有当前年份的。

	monthstyle = month[0][mothFlag+1];
	if(monthstyle >= 3){//0 是月票  1 是年票  2 是季票
		return 3;//月票不正确，去扣钱，不知道是什么票
	}
	
	memset(TACC, 0, 4);
	for(i=0;i<31;i++){
		TACC[0] ^= month[0][i];
		TACC[1] ^= month[0][32+i];
		TACC[2] ^= month[1][i];
		TACC[3] ^= month[1][32+i];
	}
	if((TACC[0] != month[0][31])||(TACC[0] != month[0][63])){
		errorf |= 8;
	}
	if((TACC[2] != month[1][31])||(TACC[3] != month[1][63])){
		errorf |= 4;
	}	

	ii = ((SysTime.month>>4)&0x0f)*10 + (SysTime.month&0x0f);
	if(monthstyle == 0){//0 是月票  
		a_sum1 = 0;
		memcpy((unsigned char *)&a_sum1, month[0]+(mothFlag+ii*2), 2);//正本

		revLen = 0;
		memcpy((unsigned char *)&revLen, month[1]+(mothFlag+ii*2), 2);//副本
	}
	else if(monthstyle == 1){//1 是年票
		a_sum1 = 0;
		memcpy((unsigned char *)&a_sum1, month[0]+(mothFlag+26), 4);//正本
		
		revLen = 0;
		memcpy((unsigned char *)&revLen, month[1]+(mothFlag+26), 4);//副本
	}
	else if(monthstyle == 2){//1 是季票
		a_sum1 = 0;
		memcpy((unsigned char *)&a_sum1, month[0]+(mothFlag+(ii-1)*6+2), 4);//正本
		
		revLen = 0;
		memcpy((unsigned char *)&revLen, month[1]+(mothFlag+(ii-1)*6+2), 4);//副本
	}
#ifdef _debug_CPU_
	debugstring("钱包\r\n");
	debugdata((unsigned char*)&a_sum1, 4, 1);
	debugdata((unsigned char*)&revLen, 4, 1);
#endif
	if((errorf&8)&&(errorf&4)){
		debugstring("月票正副本校验错\r\n");
		return 3;
	}

	if(errorf&8)
		a_sum1 = revLen;
	else if((errorf&4) == 0){
		if(a_sum1 > revLen)
			a_sum1 = revLen;
	}
	if(a_sum1 == 0)
		return 3;

	s_sum1 = 1;
	a_sum1 -= 1;
	if(cardSound == 0xbb){//月票需要重刷
		if(a_sum1 <= dis_sum2)
			goto cpuMonthEnd;//月票扣款成功
	}
	dis_sum2 = a_sum1;

#ifdef _debug_CPU_
	debugstring("钱包2\r\n");
	debugdata((unsigned char*)&a_sum1, 4, 1);
	debugdata((unsigned char*)&revLen, 4, 1);
#endif
	ii = ((SysTime.month>>4)&0x0f)*10 + (SysTime.month&0x0f);
	if(monthstyle == 0){//0 是月票
		memcpy(month[0]+(mothFlag+ii*2), (unsigned char *)&a_sum1, 2);//正本
	}
	else if(monthstyle == 1){//1 是年票
		memcpy(month[0]+(mothFlag+26), (unsigned char *)&a_sum1, 4);//正本
	}
	else if(monthstyle == 2){//1 是季票
		memcpy(month[0]+(mothFlag+(ii-1)*6+2), (unsigned char *)&a_sum1, 4);//正本
	}
	memset(TACC, 0, 4);
	for(i=0;i<31;i++){
		TACC[0] ^= month[0][i];
		TACC[1] ^= month[0][32+i];
	}
	month[0][31] = TACC[0];
	month[0][63] = TACC[1];
	memcpy(month[1], month[0], 64);

#ifdef _debug_CPU_
	debugstring("month1:");
	debugdata(month[0], 64, 1);
	debugdata(month[1], 64, 1);
#endif
	
	memcpy(sndbuf,"\x02\x00\x84\x00\x00\x04",6);////取随机数，算MAC
	deal2or3(sndbuf);
	ret = RfPassthrough(revbuf ,2, sndbuf, 6);
	if(ret == 0)
		return ST_ERROR;
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return ST_ERROR;
	memset(psRandom, 0, 8);
	memcpy(psRandom, revbuf+1, 4);
	
	if(mothFlag == 32)
		memcpy(sndbuf, "\x02\x04\xD6\x99\x20\x24", 6);
	else
		memcpy(sndbuf, "\x02\x04\xD6\x99\x00\x24", 6);
	memcpy(sndbuf+6, month[0]+mothFlag, 32);
	CountMac("\x36\xDF\xAF\xC7\x9B\x1F\xB5\x5A\xD0\x83\xFC\x78\xE1\x58\x47\x73", psRandom, sndbuf+1, 37, TACC);
#ifdef _debug_CPU_
	debugstring("MAC::");
	debugdata(psRandom, 8, 1);
	debugdata(sndbuf+1, 37, 1);
	debugdata(TACC, 8, 1);
#endif
	
	memcpy(sndbuf+38, TACC, 4);
	deal2or3(sndbuf);
#ifdef _debug_CPU_
	debugstring("write CMD::");
	debugdata(sndbuf, 42, 1);
#endif

	ret = RfPassthrough(revbuf ,2, sndbuf, 42);
	if(ret == 0){
			cardSound = 0xbb;//出错，需要重刷卡
			card_ser = c_serial;
#ifndef BUS_PINGXIANG_
#ifdef AUDIO_SOUND_OLD
			audio_old(Audio_TRY_AGAIN);		//请重刷
#else
			audio(Audio_TRY_AGAIN);		//请重刷
#endif
#endif 
			return 7;
	}
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return 6;
#ifdef _debug_CPU_
	debugstring("month write 1:");
	debugdata(revbuf, ret, 1);
#endif
	
	memcpy(sndbuf,"\x02\x00\x84\x00\x00\x04",6);//取随机数，算MAC
	deal2or3(sndbuf);
	ret = RfPassthrough(revbuf ,2, sndbuf, 6);
	if(ret == 0)
		return ST_ERROR;
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return ST_ERROR;
	memset(psRandom, 0, 8);
	memcpy(psRandom, revbuf+1, 4);

	if(mothFlag == 32)
		memcpy(sndbuf, "\x02\x04\xD6\x9A\x20\x24", 6);
	else
		memcpy(sndbuf, "\x02\x04\xD6\x9A\x00\x24", 6);
	memcpy(sndbuf+6, month[0]+mothFlag, 32);
	CountMac("\x36\xDF\xAF\xC7\x9B\x1F\xB5\x5A\xD0\x83\xFC\x78\xE1\x58\x47\x73", psRandom, sndbuf+1, 37, TACC);
#ifdef _debug_CPU_
	debugstring("MAC 2::");
	debugdata(psRandom, 8, 1);
	debugdata(sndbuf+1, 37, 1);
	debugdata(TACC, 8, 1);
#endif
	
	memcpy(sndbuf+38, TACC, 4);
	deal2or3(sndbuf);
#ifdef _debug_CPU_
	debugstring("write CMD 2::");
	debugdata(sndbuf, 42, 1);
#endif
	
	ret = RfPassthrough(revbuf ,2, sndbuf, 42);
// 	if(ret == 0)
// 		return ST_ERROR;
// 	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
// 		return 6;
#ifdef _debug_CPU_
	debugstring("month write 2:");
	debugdata(revbuf, ret, 1);
#endif

cpuMonthEnd:
	card_ser = 0;
	cardSound = 0;
	money_msg(ID_REC_MON, a_sum1, s_sum1, 2);
	SoundMessage(SOUND_DEAL);

#ifdef _debug_CPU_
	debugstring("-------\r\n");
	debugstring("消费成功:\r\n");
	sprintf((char*)revbuf, "余额:%d次\r\n", a_sum1);
	debugstring((char*)revbuf);
	sprintf((char*)revbuf, "扣款:%d次\r\n", s_sum1);
	debugstring((char*)revbuf);
	debugstring("-------\r\n");
#endif
	
	save_h_month();
	memset(revbuf, 0, 128);

	BuildRecorde(ID_REC_MON, revbuf);
	WriteRecord(revbuf, 0);

	return ST_OK;//消费成功

}
*/










unsigned char  operat_Black(void)
{
	
	unsigned char i;
	unsigned char ret;
	//	unsigned char TACC[4];
	unsigned char revbuf[256];
	unsigned char sndbuf[56];
	//	unsigned int value;
	unsigned char Rader[4];
	unsigned char sam;
	

#ifdef _debug_CPU_
	debugstring("LOCK   Card ~~~~~~\r\n");
#endif

	
	i=0;
	memcpy(sndbuf, "\x80\x50\x01\x02\x0B", 5); i = 5;//11 INITIALIZE FOR PURCHASE命令

	sndbuf[i++] = SamPubInf_LL.kid;

	memset(sndbuf+i, 0, 4); i+=4;
	memcpy(sndbuf+i, jsb_recbuff.psam_num, 6);i += 6;	
	
	
	ret = RfPassthrough(revbuf, 2, sndbuf, i);
	
#ifdef _debug_CPU_
	debugstring("交易初始化:");
	debugdata(sndbuf, i, 1);
	debugdata(revbuf, ret, 1);
#endif
	if(ret == 0)
		return ST_ERROR;
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return ST_ERROR;


	
	
	memcpy(sndbuf, "\x00\x84\x00\x00\x04", 5); i = 5;
	
	ret = RfPassthrough(revbuf, 2, sndbuf, i);
#ifdef _debug_CPU_
	debugstring("取随机数:");
	debugdata(sndbuf,i,1);
	debugstring("返回:::::");
	debugdata(revbuf, ret, 1);
#endif
	if(ret == 0)
		return ST_ERROR;	
	
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return ST_ERROR;
	memcpy(Rader,revbuf+1,4);

	i =8;
	memcpy(sndbuf,cardMessage.PublishBicker,8);
	memcpy(sndbuf+i,cardMessage.CityCode,2);i+=2;
	memcpy(sndbuf+i,"\xFF\x00\x00\x00\x00\x00",6);i+=6;


	sam = PSAM_JSB;		
#ifdef _debug_CPU_
	debugstring("PSAM_MAC初始化指令:");
	debugdata((unsigned char *)&i,4,1);
	debugdata(sndbuf,i,1);
#endif
	if (CpuCardSetMac2(sam,revbuf,sndbuf,i) == ST_ERROR){	//801A450110
		return ST_ERROR;
	}

#ifdef _debug_CPU_
	debugstring("MAC初始化返回:");
	debugdata(revbuf, ret, 1);
#endif

		
	memcpy(sndbuf,Rader,4);i=4;//密钥
	memcpy(sndbuf+i,"\x00\x00\x00\x00\x84\x1E\x00\x00\x04\x80\x00\x00",12);i +=12;

	if(CpuBlackMac(sam,revbuf,sndbuf,i)==ST_ERROR)			//80FA050010
		return ST_ERROR;

	
#ifdef _debug_
	debugstring("MAC2::");
	debugdata(revbuf,4,1);
#endif
	memcpy(jsb_recbuff.TAC,revbuf,4);
	memcpy(sndbuf, "\x84\x1e\x00\x00\x04", 5); i = 5;
	memcpy(sndbuf+i,revbuf,4);i+=4;//MAC

	
	ret = RfPassthrough(revbuf, 2, sndbuf, i);
#ifdef _debug_CPU_
	debugstring("应用锁定:");
	debugdata(sndbuf,i,1);
	debugstring("返回:i:::");
	debugdata(revbuf, ret, 1);
#endif

	if(ret == 0)
		return ST_ERROR;	
	
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return ST_ERROR;


//	memcpy(sndbuf, "\x02\x00\xA4\x00\x00\x02\x3F\x01",8);//有是方文件名不一样，需要处理一下。

// 	memcpy(sndbuf, "\x00\xA4\x00\x00\x02\x3F\x01",7);//有是方文件名不一样，需要处理一下。
// 	ret = RfPassthrough(revbuf, 2, sndbuf, 7);
// #ifdef _debug_CPU_
// 	debugstring("进入3F01:");
// 	debugdata(sndbuf, 7, 1);
// 	debugdata(revbuf, ret, 1);
// #endif
// 	if(ret == 0)
// 		return ST_ERROR;
// 	
// 	if(memcmp(revbuf+ret-2, "\x6A\x81", 2) != 0)	//进入不了这个目录了，说明锁卡成功
// 		return ST_ERROR;
	
	
	
	memset(revbuf, 0, 128);
	s_sum1 = 0;
	a_sum1 = 0;
	
	
	jsb_recbuff.ckeyEn = 0;
	memset(jsb_recbuff.card_deal_serial,0,2);
	memset(jsb_recbuff.deal_serial,0,4);	//
	memcpy(jsb_recbuff.city_code ,cardMessage.CityCode,2);
	memcpy(jsb_recbuff.deal_money,&s_sum1,4);
	jsb_recbuff.deal_type = 0x06;
	memcpy(jsb_recbuff.deal_time,&SysTime.year_h,7);	//8070返回
	
	BuildRecorde_CPU_JSB(ID_REC_BLKLIST, revbuf);
	WriteRecord(revbuf, 0);
	
	
	return ST_OK;

}

unsigned char CPUcardType_JSB(void)
{
	//	unsigned char PSAMDsno[4];
	unsigned int itemp;
	unsigned int start_time,end_time;

	unsigned int ret;
	stJSBF15 *JSBFile15;

	unsigned char sndbuf[100];
	unsigned char revbuf[100];
#ifdef _debug_CPU_
//	char disbuff[30];
#endif



#ifndef BUS_MZT 
	memcpy(sndbuf,"\xe0\x50",2);
	ret = RfPassthrough(revbuf, 2, sndbuf, 2);	
#ifdef _debug_CPU_
	debugstring("CPU start!\r\n");
	debugdata(revbuf, ret, 1);
#endif
// 	debugstring("CPU start!\r\n");
// 	debugdata(revbuf, ret, 1);
	
	delayxms(51);		//适当延时，要不然选择不到文件
#endif

#ifdef _debug_CPU_
		debugstring("选择应用 3F01:开始\r\n");
#endif
		
		memcpy(sndbuf, "\x02\x00\xA4\x00\x00\x02\x3F\x01",8);//有是方文件名不一样，需要处理一下。
		deal2or3(sndbuf);
		ret = RfPassthrough(revbuf, 2, sndbuf, 8);
#ifdef _debug_CPU_
		debugdata(sndbuf, 8, 1);
		debugdata(revbuf, ret, 1);
#endif
		if(ret == 0){
			debugstring("ssssss\r\n");
			return ST_ERROR;
		}

	if(memcmp(revbuf+ret-2, "\x6A\x81", 2) == 0)
		return CARD_STOPWORK;
	if(memcmp(revbuf+ret-2, "\x93\x03", 2) == 0)
		return APP_LOCK;

	

#ifdef _debug_CPU_

		debugstring("建设部选择互联互通应用::\r\n");
#endif


		memcpy(sndbuf, "\x02\x00\xA4\x04\x00\x09\xA0\x00\x00\x00\x03\x86\x98\x07\x01", 15);  // 建设部 选择互联互通应用
												//xA0\x00\x00\x00\x03\x86\x98\x07\x01
		deal2or3(sndbuf);
#ifdef _debug_CPU_
		debugstring("select AID1:");
		debugdata(sndbuf, 15, 1);
#endif
		ret = RfPassthrough(revbuf, 2, sndbuf, 15);
#ifdef _debug_CPU_
		debugdata(revbuf, ret, 1);
#endif
		if(ret == 0)
			return ST_ERROR;
//	}





	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return ST_ERROR;

	memcpy(sndbuf,"\x02\x00\xB0\x95\x00\x1E",6);//   建设部的卡是
	deal2or3(sndbuf);
	ret = RfPassthrough(revbuf, 2, sndbuf, 6);
	if(ret == 0)
		return ST_ERROR;
	
	if (revbuf[1] == 0x6C)		//读取文件长度错误，公司cpu卡
	{
		memcpy(sndbuf,"\x02\x00\xB0\x95\x00\x20",6);//读应用序列号作为分散因子 1e
		deal2or3(sndbuf);
		sndbuf[5] = revbuf[2];			//实际应读取的长度
#ifdef _debug_CPU_
		debugstring("error len=:");
		debugdata(revbuf+2,1,1);

		debugstring("sndbuf:");
		debugdata(sndbuf,6,1);
#endif
		ret = RfPassthrough(revbuf, 2, sndbuf, 6);
		if(ret == 0)
			return ST_ERROR;

	}
//memcpy(Cpufile15, revbuf+1, 30);//tttttttttttttttttttttttttttttttttttt
	//03A0000000030000010301020000000000201100162011052420110830A9389000 
	//03010136201E201211212015010110000000100000000000000000000000009000
	
#ifdef _debug_CPU_
	debugstring("sndbuf:");
	debugdata(sndbuf,6,1);
	debugdata((unsigned char*)&ret, 4,1);
	debugstring("G24 read 95:");
	debugdata(revbuf, ret, 1);
#endif

	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return ST_ERROR;

	JSBFile15 = (stJSBF15 *) (revbuf+1);

	memcpy(cardMessage.TradeCode, JSBFile15->FCcode, 2);//发卡方标识
	memcpy(cardMessage.CityCode, JSBFile15->citycode, 2);//城市代码
// 	memcpy(cardMessage.CardCheckCode,JSBFile15.MAC,CARD_CHECH_CODE_LEN);
	memcpy(cardMessage.PublishBicker, JSBFile15->YYNO, 8);//发卡流水号
	memcpy(jsb_recbuff.card_serial, JSBFile15->YYNO, 8);//卡序列号
	memcpy(jsb_recbuff.city_code, JSBFile15->citycode, 2);//
	
	cardMessage.card_catalog = CARD_NORMAL;			//
	cardMessage.Zicard_catalog = 0;

// 	cardMessage.card_catalog = JSBFile15->card_catalog;			//
// 	cardMessage.Zicard_catalog = JSBFile15->Zicard_catalog;
	

	if(gPsamErrorFlag&2){
		cls();
		if(gchn_eng == 'E'){
			display(4, 0, "PSAM2 error!",0);
		}
		else{
			display(0,0,"错误:",1);
			display(2,0,"CPU卡PSAM错误!",0);
		}
		restore_flag = 3;
		delayxs(1);
		return ST_OK;				
	}
	
		

	memcpy((unsigned char*)&start_time, JSBFile15->StartDate, 4);//启用日期
	over_turn(4, (unsigned char*)&start_time);
	memcpy((unsigned char*)&end_time, JSBFile15->EndDate, 4);//有效日期
	over_turn(4, (unsigned char*)&end_time);



		if (memcmp(cardMessage.CityCode,CARD_CITY_CODE,2) != 0)
		{
#ifdef _debug_CPU_
			debugstring("也不是4420\r\n");
#endif
			cardMessage.card_catalog = CARD_NORMAL;
			cardMessage.Zicard_catalog = 0;	//没有用

			if (IsInWhileList(cardMessage.CityCode) == 0)
			{
	#ifdef _debug_CPU_
				debugstring("不在白名单中\r\n");
	#endif
				return NO_WriteNUM;	//
			}

		}

	
		

	MSG_LOG("JSBFile15->QYflag ======屏蔽***!!!!!!!!!!!!!*\r\n");	
// 	if(JSBFile15->QYflag == 0)//启动标志
// 		return CARD_NO_USE;	


// 	switch(JSBFile15->GS_QY)//启动标志
// 	{
// 	case STARTUP_OK:
// 		break;
// 	case STARTUP_STOP:
// 		return CARD_STOPWORK;
// 	case STARTUP_NO:
// 		return CARD_NO_USE;
// 	case STARTUP_BLACK:
// 		return CARD_BLACK;
// 	default:
// 		return CARD_NO_SYSTEM;
// 	}



#ifdef _debug_CPU_
	debugstring("jsb_recbuff.card_serial::");
	debugdata(jsb_recbuff.card_serial,8,1);

// 	memcpy((unsigned char *)&ret,cardMessage.PublishBicker,8);
// 	memset(disbuff,0,sizeof(disbuff));
// 	sprintf(disbuff,"流水号:%08u\r\n",ret);
// 	debugstring(disbuff);
#endif


#ifdef _debug_CPU_
	debugstring("卡类::");
	debugdata(&cardMessage.card_catalog,1,0);
	debugdata(&cardMessage.Zicard_catalog,1,1);
	debugstring("card_catalog.rate::");
	debugdata(NewPriceTab.rate+cardMessage.card_catalog, 1, 1);
#endif





	memcpy((unsigned char*)&itemp, (unsigned char *)&SysTime, 4);//当前日期
	over_turn(4, (unsigned char*)&itemp);

	
#ifdef _debug_CPU_
	debugstring("cardMessage.card_catalog:");
	debugdata((unsigned char*)&cardMessage.card_catalog,1, 1);
	debugstring(".PublishBicker:::");
	debugdata(cardMessage.PublishBicker, 8, 1);
	debugstring("启用日期::");
	debugdata((unsigned char*)&start_time, 4, 1);
	debugstring("有效期::");
	debugdata((unsigned char*)&end_time, 4, 1);
#endif

#ifndef _debug_
	if((itemp < start_time)||(itemp > end_time)){
		return CARD_NO_TIME;//过期或末到期
	}
#else
	MSG_LOG("不看有效期---\r\n");
#endif

	
	if(IsInBlackList(jsb_recbuff.card_serial) < FLASH_BLK_END){
#ifdef _debug_
		debugstring("lock card:-----\r\n");
#endif
		return CARD_BLACK_PRO;

	}



/*

	if(cardMessage.card_catalog >= 0x20){
		
		return CONTROL_CARD;
	}*/


	if(cardMessage.card_catalog >= 0x40)
		ret = cardMessage.card_catalog-0x40;
	else
		ret = cardMessage.card_catalog;

	
// 	ret = cardMessage.card_catalog;

#ifdef _debug_CPU_
	debugstring("rate:");
	debugdata(&NewPriceTab.rate[ret],1,1);
#endif

	
// 	if((NewPriceTab.rate[ret]==202)||(NewPriceTab.rate[ret]==102))//免费
// 	{
// 		return MONTH_CARD;	
// 	}
// 
// 	if((NewPriceTab.rate[ret]==204)||(NewPriceTab.rate[ret]==104))//禁止
// 	{
// 		return CARD_forbid;	
// 	}
// 	if((NewPriceTab.rate[ret]==203)||(NewPriceTab.rate[ret]==103))//禁止
// 	{
// 		return CARD_MONEY;	
// 	}

	if((NewPriceTab.rate[ret]==204)||(NewPriceTab.rate[ret]==104))//禁止
	{
		return CARD_forbid;	
	}	
	
	
	if((NewPriceTab.rate[ret]==205)||(NewPriceTab.rate[ret] ==105)){//免费消费 且只有一次消费
		
		if(month_decide()) // 第二次去扣钱包(次数不能连刷)
		{
			disp_no_swipe();
			return ST_OK;
			
		}
	
	}
	return CARD_MONEY;

}

//extern int dealCardInfo(unsigned char *file15, unsigned char *outDat); //tttttttttttttttttttttttt
//extern int CoutMAC(unsigned char *inDat, unsigned char *outDat);

extern unsigned char getMiKey(void);

unsigned char CPUDealCard_JSB(unsigned char mode, unsigned char cool)
{

	unsigned char i;
	unsigned char sndbuf[100];
	unsigned char revbuf[128];
	unsigned char TACC[4]={0};
//	unsigned char PSAMDsno[4];
	unsigned int revLen,ret;
	unsigned int value;
	unsigned short ii;
	Recode_CPU_JSB *rp=0;

// 	unsigned char CPUPKEY[16];
// 	unsigned char CPUpInData1[8];
// 	unsigned char CPUinitdat[8];
//	unsigned char snddat[64];

#ifdef _debug_CPU_
	debugstring("CPUDealCard========\r\n");
#endif

	gMCardCand= CARDSTYLE_JSB;


	if(F_FLAG_VARY.stop_flag) 
		return ST_ERROR;



// 	memcpy(sndbuf, "\x02\x00\xA4\x00\x00\x02\x3F\x01",8);//有是方文件名不一样，需要处理一下。
// 	deal2or3(sndbuf);
// 	ret = RfPassthrough(revbuf, 2, sndbuf, 8);
// #ifdef _debug_CPU_
// 	MSG_LOG("重新进去3F01---\r\n");
// 	debugdata(sndbuf, 8, 1);
// 	debugdata(revbuf, ret, 1);
// #endif
// 	if(ret == 0)
// 		return ST_ERROR;
// 	if(memcmp(revbuf+ret-2, "\x6A\x81", 2) == 0)
// 		return CARD_STOPWORK;
// 	if(memcmp(revbuf+ret-2, "\x93\x03", 2) == 0)
// 		return APP_LOCK;
// 
// 	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
// 		return ST_ERROR;


	
	if(Delay_Normal()){
		return ST_ERROR;
	}



// 		if(month_decide()) // 第二次去扣钱包(次数不能连刷)
// 		{
// 			disp_no_swipe();
// 			return ST_OK;
// 		}



	if (CPU_Read_Money((unsigned char*)&value, 3) != 0){
		return ST_ERROR;
	}
	over_turn(4, (unsigned char*)&value);
#ifdef _debug_CPU_
	debugstring("aa余额:");
	debugdata((unsigned char*)&value, 4, 1);
	debugdata((unsigned char*)&a_sumR, 4, 1);
#endif


#ifdef _debug_quan
	time = get_timer0(2);
	sprintf(distime,"333333333:time%d\r\n\x0",time);
	debugstring(distime);
#endif







	if(cardSound == 0xaa){//需要重刷

// 		if(card_ser!=c_serial){
// 			return ST_ERROR;	
// 		}

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
		

		if(value < a_sumR){//上次已经扣成功
			memcpy(sndbuf, "\x02\x80\x5A\x00\x06\x02", 6);//memcpy(sndbuf, "\x02\x80\x5A\x00\x05\x02", 6);
			memcpy((unsigned char*)&ii, stuInitPurchaseRet.cSnq, 2);
			over_turn(2, (unsigned char*)&ii);
			ii += 1;
			over_turn(2, (unsigned char*)&ii);
			memcpy(sndbuf+6, (unsigned char*)&ii, 2);
			sndbuf[9] = 8;
#ifdef _debug_CPU_
			debugstring("防拔:");
			debugdata(stuInitPurchaseRet.cSnq, 2, 1);
			debugdata(sndbuf, 10, 1);
#endif
			deal2or3(sndbuf);
			revLen = RfPassthrough(revbuf, 2, sndbuf, 10);
			if(revLen == 0)
				return ST_ERROR;
#ifdef _debug_CPU_
			debugdata(revbuf, revLen, 1);//02 00 0C 00 00 ED F2 97 9B 6E 94 37 BC 90 00 F2 03
#endif
			if(revLen == 0)//返回超时,错误
				return ST_ERROR;
			if((memcmp(revbuf+revLen-2, "\x90\x00", 2)!=0) &&\
				(memcmp(revbuf+revLen-2, "\x6C\x08", 2)!=0))
				goto CPUMAC2deal;//取不成功02 69 85 
			
			memcpy(TACC, revbuf+revLen-6, 4);//返回的TAC码
			memcpy(sndbuf, revbuf+revLen-10, 4);//用MAC2码

			//如果是其他月票或者M1卡进去了一次，这些变量可能已经变了，为了防止写记录有问题，重新取值
			//其实前端重刷控制好了，这么没有必要要了
			s_sum1 = get_s_sum1(0);		
			a_sum1 = value;		//余额

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


	a_sumR = value;		//扣之前余额


	
#ifdef _debug_CPU_
	debugstring("a_sumR:");
	debugdata((unsigned char*)&a_sumR, 4, 1);
#endif

	memcpy(sndbuf,"\x00\xB2\x01\xC4\x17",5);//取上笔记录 18文件
	ret = RfPassthrough(revbuf, 2, sndbuf, 5);
	if(ret == 0)
		return ST_ERROR;
#ifdef _debug_CPU_
	debugstring("取18记录文件----\r\nsnd:\r\n");
	debugdata(sndbuf, 5, 1);
	debugdata(revbuf, ret, 1);
#endif

	

#ifdef _debug_CPU_
	debugstring("SamPubInf_LL.kid;:::");
	debugdata(&SamPubInf_LL.kid,1,1);
#endif

	i=0;
	
	s_sum1 = get_s_sum1(mode);

	//用户原额
#ifdef _debug_CPU_
	debugstring("票价:::");
	debugdata((unsigned char *)&s_sum1, 4, 1);	
#endif
// 	if((s_sum1 == 0) || (s_sum1 > MAX_money)){
// 		return 22;
// 	}



	
	memcpy(sndbuf, "\x80\xca\x00\x00\x09", 5);
#ifdef _debug_CPU_
	debugstring("取安全认证码:");
	debugdata(sndbuf, 5, 1);
#endif
	ret = RfPassthrough(revbuf, 2, sndbuf, 5);//获取用户卡安全认证识别码
	
	if(ret == 0)
		return ST_ERROR;
	if(memcmp(revbuf+ret-2, "\x90\x00", 2)!=0){
		return ST_ERROR;
	}
#ifdef _debug_CPU_
	debugdata((unsigned char*)&ret, 1, 1);
	debugdata(revbuf, ret, 1);
	debugstring("Safe_Code:");
	debugdata(revbuf+1,9,1);
#endif
	
	
	
	memcpy(sndbuf, revbuf+1, 9);
	
	if(CpuCardCheck(PSAM_JSB,revbuf, sndbuf, 9)==ST_ERROR)//PSAM 卡验证安全认证码别码,新加
		return ST_ERROR;


	
	//8050 0101 0B||密钥标识符 [1]（0x01）||交易金额[4](0x00000001)||终端机编号[6]
	memcpy(sndbuf, "\x02\x80\x50\x01\x02\x0B", 6); i = 6;//11 INITIALIZE FOR PURCHASE命令
	sndbuf[i] = SamPubInf_LL.kid;
	i++;

	value = s_sum1;
	over_turn(4, (unsigned char*)&value);
	memcpy(sndbuf+i, (unsigned char*)&value, 4); i+=4;
	memcpy(sndbuf+i, jsb_recbuff.psam_num, 6);
#ifdef _debug_CPU_
		debugstring("JSB_PSAM编号::");
		debugdata(jsb_recbuff.psam_num,6,1);
#endif
	i += 6;	
	deal2or3(sndbuf);

		




#ifdef _debug_CPU_
	debugstring("消费初始化:");
	debugdata(sndbuf, i, 1);
#endif
	ret = RfPassthrough(revbuf, 2, sndbuf, i);
#ifdef _debug_CPU_

	debugdata((unsigned char*)&ret,4,1);

	debugdata(revbuf, ret, 1);
#endif
	if(ret == 0)
		return ST_ERROR;
	else{
		if(memcmp(revbuf+ret-2, "\x94\x01", 2)==0){
			if(cool == 4)//月票连刷进来，不提示余额不足
				return 4;
			else
				return 3;
		}
		else if(memcmp(revbuf+ret-2, "\x90\x00", 2)!=0)
			return 5;
	}
	memcpy((unsigned char*)&stuInitPurchaseRet, revbuf+1, sizeof(stuInitPurchaseRet));
#ifdef _debug_CPU_
	debugstring("8050指令返回:::\r\n");
	debugdata((unsigned char*)&stuInitPurchaseRet, sizeof(stuInitPurchaseRet), 1);
	debugstring("卡余额::");
	debugdata(stuInitPurchaseRet.cBalance,4,1);
	debugstring("密钥版本和算法标识:");
	debugdata(&stuInitPurchaseRet.cKeyVer,2,1);

#endif
	memcpy(stuSamInitPurchase.cRand, stuInitPurchaseRet.cRand, 4);
	memcpy(stuSamInitPurchase.cSnq, stuInitPurchaseRet.cSnq, 2);
	memcpy(stuSamInitPurchase.cMoney, (unsigned char*)&value, 4);
	
	memcpy((unsigned char*)&a_sum1, stuInitPurchaseRet.cBalance, 4);//余额
	over_turn(4, (unsigned char*)&a_sum1);

#ifdef _debug_CPU_
	debugstring("余额===:");
	debugdata((unsigned char *)&a_sum1, 4, 1);
#endif

// 	if (a_sum1 > MAX_Balance) {
// 		return 21;
// 	}


	gucSoudnorm = 0;

	if((a_sum1>=s_sum1)&&(a_sum1<500)){
		gucSoudnorm = 1;
	}

	stuSamInitPurchase.cType = 0x06;			//消费交易 固定值
	
	memcpy(stuSamInitPurchase.cDateTime, (unsigned char*)&SysTime, 7);

	stuSamInitPurchase.cKeyVer = stuInitPurchaseRet.cKeyVer;//0x05   
	stuSamInitPurchase.cKeyEn = stuInitPurchaseRet.cKeyEn;					//密钥标识

	memcpy(stuSamInitPurchase.cKeyFactor, jsb_recbuff.card_serial, 8);//有28个数据
	memcpy(stuSamInitPurchase.cPublisherSign, jsb_recbuff.city_code, 2);//城市代码
	memcpy(stuSamInitPurchase.cPublisherSign+2,"\xFF\x00\x00\x00\x00\x00", 6);//有36个数据

#ifdef _debug_CPU_
	debugstring("MAC1计算指令:");
	debugdata((unsigned char*)&stuSamInitPurchase,(36), 1);//+12
#endif


	//52B58B730005000000010620110402154217 05000000000090002030
	//4ACF30AF 0005 00000001 06 20110402154422 01000000000090002030 
	memset(revbuf, 0xff, 50);
	memset((unsigned char*)&stuDebitPurchase, 0, sizeof(stuDebitPurchase));

	//--------------------------






#ifdef _debug_CPU_
		debugstring("建设部!!\r\n");
#endif

		if(CpuCardSetMac(PSAM_JSB,revbuf, (unsigned char*)&stuSamInitPurchase, 36) == ST_ERROR)
		{
			//		DeactiveCard(PSAM_YD);
			return 6;
		}


#ifdef _debug_CPU_
	debugstring("MAC OK!:");
	debugstring("8070指令返回:::\r\n");
	debugdata(revbuf, 8, 1);
#endif
	memcpy(stuDebitPurchase.cTermSnq, revbuf, 4);
	memcpy(stuDebitPurchase.cMac, revbuf+4, 4);
	memcpy(stuDebitPurchase.cDateTime, (unsigned char*)&SysTime, 7);

	memcpy(g24PSAMDsno, revbuf, 4);//--4字节的终端脱机交易序号  和 MAC1

#ifdef _debug_CPU_
	debugstring("终端脱机交易序列号::");
	debugdata(stuDebitPurchase.cTermSnq,4,1);
#endif


	memcpy(jsb_recbuff.city_code ,cardMessage.CityCode,2);
	jsb_recbuff.ckeyEn = stuInitPurchaseRet.cKeyEn;
	memcpy(jsb_recbuff.deal_money,&s_sum1,4);
	jsb_recbuff.deal_type = 0x06;
	memcpy(jsb_recbuff.deal_serial,stuDebitPurchase.cTermSnq,4);	//8070返回
	memcpy(jsb_recbuff.deal_time,stuDebitPurchase.cDateTime,7);	//8070返回
	memcpy(jsb_recbuff.card_deal_serial,stuInitPurchaseRet.cSnq,2);


	if (a_sum1 >= s_sum1)
	{
		a_sum1 -= s_sum1;
	}else{
		debugstring("a_sum1 < s_sum1 ERROR!!\r\n");
		return ST_ERROR;
	}



	memset(revbuf, 0, 128);
#ifdef _debug_CPU_
	debugstring("生成灰记录1------\r\n");
#endif
	memset(jsb_recbuff.TAC,0,4);
//	BuildRecorde(ID_REC_TOLL,revbuf);
	BuildRecorde_CPU_JSB(ID_REC_TOLL,revbuf);
	rp = (Recode_CPU_JSB *)revbuf;
	rp->rDealType = ID_REC_HJY_REC;
	if (Unknow_Record(1,revbuf,RECORD_LEN_CPU) == ST_ERROR){	//在操作卡之前先写灰记录 	
		return ST_ERROR;
	}



	memcpy(sndbuf, "\x03\x80\x54\x01\x00\x0F", 6); i = 6;			//8054 消费命令的
	memcpy(sndbuf+i, (unsigned char*)&stuDebitPurchase, 0x0f); i+=0x0f;
	memset(revbuf, 0xff, 50);
	deal2or3(sndbuf);
#ifdef _debug_CPU_
	debugstring("chek Mac1:");
	debugdata(sndbuf, i, 1);
#endif

	ret = RfPassthrough(revbuf, 2, sndbuf, i);

	
	if(ret == 0){
		audio(Audio_TRY_AGAIN);		//请重刷
		cardSound = 0xaa;//出错，需要重刷卡
		card_ser = c_serial;

		return 7;
	}
	
#ifdef _debug_CPU_
	debugstring("chek Mac1 OK:");
	debugdata((unsigned char*)&ret, 4, 1);
	debugdata(revbuf, 20 ,1);
#endif
	if(memcmp(revbuf+ret-2, "\x93\x02", 2)==0)
		return 9;
	if(memcmp(revbuf+ret-2, "\x90\x00", 2)!=0){
//		debugdata(revbuf, 20, 1);
		cardSound = 0xaa;//出错，需要重刷卡
		card_ser = c_serial;
		return 7;
	}//其它错误
	
	memcpy(TACC, revbuf+ret-10, 4);//返回的TAC码 SPI -13
	memcpy(sndbuf, revbuf+ret-6, 4);
#ifdef _debug_CPU_
//	debugdata((unsigned char*)&pmoney, 4, 1);
	debugdata(sndbuf, 4, 1);
	debugstring("消费成功:\r\n");
#endif



#ifdef _debug_CPU_
	debugstring("TAC:");
	debugdata(TACC, 4, 1);
	debugstring("MAC2:");
	debugdata(sndbuf, 4, 1);
	debugdata((unsigned char*)&a_sum1, 4, 1);
	
#endif



CPUMAC2deal0:
#ifdef _debug_CPU_
		debugstring("校验MAC2::\r\n");
#endif
		CPUMAC2(PSAM_JSB,revbuf, sndbuf, 4);
	
CPUMAC2deal:
	memcpy(cardMessage.PbocTac, TACC, 4);	// TAC码
	memcpy(jsb_recbuff.TAC,cardMessage.PbocTac,4);

#ifdef _debug_CPU_
	debugstring("计算MAC2:");
	debugdata(revbuf, 10, 1);
#endif
	

	
	Save_delayCardinfo(mode);

		
	

	
#ifdef _debug_CPU_
	debugstring("城市代码:::::");
	debugdata(jsb_recbuff.city_code,2,1);
	debugstring("应用序列号:::");
	debugdata(jsb_recbuff.card_serial,8,1);	
	debugstring("算法标识:::::");
	debugdata(&jsb_recbuff.ckeyEn,1,1);
	debugstring("交易金额:::::");
	debugdata(jsb_recbuff.deal_money,4,1);
	debugstring("交易标识:::::");
	debugdata(&jsb_recbuff.deal_type,1,1);
	debugstring("终端机编号:");
	debugdata(jsb_recbuff.psam_num,6,1);	
	debugstring("PSAM交易序列号:");
	debugdata(jsb_recbuff.deal_serial,4,1);
	debugstring("CPU卡交易序号:");
	debugdata(jsb_recbuff.card_deal_serial,2,1);
	debugstring("交易时间:");
	debugdata(jsb_recbuff.deal_time,7,1);
	
	debugstring("认证码:");
	debugdata(jsb_recbuff.TAC,4,1);
	
	
	debugstring("all_dat:");
	debugdata((unsigned char *)&jsb_recbuff,sizeof(jsb_recbuff),1);
	
#endif
	


//	read_CPUC_infor(ID_REC_TOLL);			//检测卡号，卡类，和余额是不是对的
	money_msg(ID_REC_TOLL, a_sum1, s_sum1, 2);
#ifdef _debug_CPU_
	debugstring("-------\r\n");
	debugstring("消费成功:\r\n");
	sprintf((char*)revbuf, "原款:%d.%02d元\r\n\0", (a_sumR/100), (a_sumR%100));
	debugstring((char*)revbuf);
	sprintf((char*)revbuf, "扣款:%d.%02d元\r\n\0", (s_sum1/100), (s_sum1%100));
	debugstring((char*)revbuf);
	sprintf((char*)revbuf, "余额:%d.%02d元\r\n\0", (a_sum1/100), (a_sum1%100));
	debugstring((char*)revbuf);
	debugstring("-------\r\n");
#endif
	

	
	memset(revbuf, 0, 128);
	BuildRecorde_CPU_JSB(ID_REC_TOLL, revbuf);	
	WriteRecord(revbuf, 0);


	card_ser = 0;
	cardSound = 0;	
	gMCardCand=0;

	if(cool == 3)//月票连刷进来，不提示优惠的卡型
		audio(Audio_DONG);
	else{
		Sound_cityCMD(0);
	}

#ifdef _debug_quan
	time = get_timer0(2);
	sprintf(distime,"6666666:time%d\r\n\x0",time);
	debugstring(distime);
#endif

	ComSumeFile.SumMoney+=s_sum1; //当前消费总额
	ii = ((unsigned char*)&ComSumeFile.SumMoney)-((unsigned char*)&ComSumeFile);
	sysfewrite(BIT_COMSUME_FILE+ii, 4 ,(unsigned char*)&ComSumeFile.SumMoney);




	return ST_OK;//消费成功
}




void set_line_cpu(void)
{
//	unsigned char i,uctemp;
	unsigned char sndbuf[100];
	unsigned char revbuf[256];
	unsigned char ret;
#ifdef _debug_CPU_
//	char disbuff[64];
#endif	

	
	cls();
	if(gchn_eng == 'E')
		display(4,3,"PLS wait... ",0);
	else
		display(4,3,"请稍等......",0);



	memcpy(sndbuf,"\x00\xB0\x83\x00\x80",5);//   建设部的卡是128长度
#ifdef _debug_CPU_
	debugstring("读03线路文件:\r\n");
	debugdata(sndbuf, 5, 1);
#endif
	ret = RfPassthrough(revbuf, 2, sndbuf, 5);
	
	if(ret == 0)
		return ;
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) !=0){
		return;
	}
#ifdef _debug_CPU_
	debugdata((unsigned char*)&ret, 1, 1);
	debugdata(revbuf, ret, 1);
#endif

	file_15_line(revbuf+1);

	restore_flag=1;
	display(2,3,"线路卡",DIS_CENTER | DIS_Cls);
	display(4,3,"设置完毕!",DIS_CENTER);

#ifdef _debug_
	debugstring("LineNo2:");
	debugdata(pFistVary.LineNo,3,1);
#endif

	a_sum1 = 0;
	s_sum1 = 0;
	beep(1, 100, 100);
	SoundMessage(SOUND_DI_BUS);

	memset(revbuf,0,sizeof(revbuf));

	memcpy(jsb_recbuff.deal_time,&SysTime.year_h,7);
	BuildRecorde_CPU_JSB(ID_REC_LINENO, revbuf);
	#ifdef _debug_
	debugstring("LineNo3:");
	debugdata(pFistVary.LineNo,3,1);
	#endif
	WriteRecord(revbuf, 0);
//#endif	//#ifdef BUS_HANDAN_
#ifdef _debug_
	debugstring("LineNo4:");
	debugdata(pFistVary.LineNo,3,1);
#endif
//	ControlDis(cardMessage.card_catalog);
#ifdef _debug_
debugstring("Set line END!!!\r\n");
#endif

	end_close_card(1);

}










#endif // #ifdef BUS_CPU_



