
/*
	萍乡CPU卡模式

  */
#include "szct.h"
#include "qpboc_8583.h"
#if ((defined BUS_CPU_)||(defined _XMSHDZ_24G_)||(defined ZKXL_24G_) || defined qPBOC_CPU_CARDTYPE_)
#include "Cpucard.h"
#include "E_CPUCard.h"

//下面是和24G手机卡共用的变量
#ifdef NO_MOBILE_24G_
unsigned char G24PsamNo[6];
unsigned char g24PSAMDsno[4];//终端序列号
//typea_public_inf_zaozuang stuPubInfZaoZuang;
pboc_debit_capp_purchase stuDebitPurchase;
pboc_sam_init_purchase stuSamInitPurchase;
pboc_init_capp_purchase_res stuInitPurchaseRet;
#else 
extern unsigned char G24PsamNo[6];
extern unsigned int g24GDisFlash;//24G卡界面刷新
extern unsigned char g24PSAMDsno[4];//终端序列号
extern typea_public_inf_zaozuang stuPubInfZaoZuang;
extern pboc_debit_capp_purchase stuDebitPurchase;
extern pboc_sam_init_purchase stuSamInitPurchase;
extern pboc_init_capp_purchase_res stuInitPurchaseRet;
#endif

#endif

#if defined BUS_CPU_ || defined qPBOC_CPU_CARDTYPE_
#include "E_GJGPRS.h"
#include "Mydes.h"

#ifdef BUS_PRO_JTB
#include "CPUCARJTB.h"
#endif
#ifdef BUS_PRO_JSB
#include "CPU_Union.h"
#endif

#ifdef qPBOC_BUS
#include "qpboc_head.h"
#endif
#define bzero(a,b) memset(a,0,b)

extern void Sound_cityCMD(unsigned char sMode);
extern void SoundMessage(unsigned char cmd);
extern void money_msg(unsigned char dmode, unsigned int remM, unsigned int pucM, unsigned char cMOde);
extern void dis_time_mini(unsigned char x, stcomtime *time);
extern unsigned char WriteRecord(unsigned char *buffer, unsigned char mode);
extern void BlightOpen(void);
extern void BuildRecorde(unsigned char delType, unsigned char *revbuf);
extern void save_normal(void);
extern void save_h_month(void);
extern unsigned char month_decide(void);
extern void CountPoint(unsigned char mode);
extern unsigned int IsInBlackList(unsigned char *c_card_no);
extern unsigned char FreeConsume(void);
extern void disp_no_swipe(void);
extern void error(unsigned char code1,unsigned char Index);
extern void ControlDis(unsigned char type);
extern unsigned char Delay_Normal(void);
extern void SetTcpPacketTTLTime(unsigned char val);
extern void CountMac_CUP(unsigned char *Key,unsigned char *random,unsigned char *Sdat,unsigned int len ,unsigned char *Result);

//jsb_rec jsb_revbuff;
//有些卡需要区分男女声
extern unsigned char nnAudioflag;

sam_pub SamPubInf_LL;
unsigned char mode2or3;
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
extern unsigned char PsamNo[6];

extern unsigned char gPSAMin;//psam卡所在位置
//unsigned char Cpufile15[30];//tttttttttttttttttttttttttttttttttttt
extern unsigned char CPUcardType_JTB(unsigned char mode);
extern unsigned char CPUDealCard_JTB(unsigned char mode, unsigned char cool);


#define SELECT_APPY_1001	"\x00\xa4\x04\x00\x0d\xD1\x56\x00\x00\x15\xB9\xAB\xB9\xB2\xD3\xA6\xD3\xC3"	//18字节 绵阳的卡
#define SELECT_APPY_NEW		"\x00\xA4\x04\x00\x10\xD1\x56\x00\x00\x15\x4D\x59\x4D\x5A\x54\x2E\x41\x44\x46\x30\x31"//绵州通新卡21字节  原来的和成都的一样，为了区分用新的了，先找这个aid
// 0xD156000015 + MZT.DDF01
#define SELECT_APPY_MF	"\x00\xA4\x04\x00\x0E\xD1\x56\x00\x00\x15\x4D\x5A\x54\x2E\x44\x44\x46\x30\x31"//19字节，MF是不一样的，1001不清楚为什么和天府通定义的一样。如果我们终端先选择MF的AID，天府通双应用卡在绵阳也不受影响。
#define NOTENABLED 0x01
#define ENABLED    0x02
#define DISABLED   0x03


//add hb
unsigned char YouHui_Area_other = 0;//控制折扣
unsigned char YouHui_Area = 0;//如果Card_Area在规定的特殊卡类区域，为1，可以实现优惠乘车；
unsigned char YouHui_Flag = 0;//特殊卡类优惠交易:1    其他:0   用于统计优惠乘车记录
unsigned char Card_Area = 0;//城区代码   0x80:北川  0x40:盐亭 0x20:三台  0x10:梓潼  0x08:安县  0x04:平武县  0x02:江油市  0x01:绵阳城区
unsigned char YouHui_Date[4];
unsigned char keyIndex = 0x11	;	// 本地卡索引0x11  异地卡 01
unsigned char dis_overdate=0xff;		//提示过期日期  

void ERR_DIS(char *dis,unsigned char *dat)//显示错误
{
	unsigned short distem;
	char disbuf[20];
	MessageBox(1,dis);
	g24GDisFlash=4;
	restore_flag=0;
	memcpy((unsigned char*)&distem,dat,2);
	over_turn(2,(unsigned char*)&distem);
	sprintf(disbuf,"%04x",distem);
	miniDispstr(6,0,disbuf,0);
}


unsigned char CPUPsamInitialize(void)
{
#ifdef NO_CPU_PSAM_
	memset(G24PsamNo, 0, 6);
	Get_SerialNumF4(G24PsamNo+2);
#elif defined BUS_PRO_JSB
	gPsamErrorFlag &= 0xFD;	//如果是住建部的PSAM已经初始化过了
	return ST_OK;
#else
	unsigned char ii=0;
	//	unsigned char pos=0;
	unsigned char buffer[256];

	gPsamErrorFlag |= 2;
	
	memset(G24PsamNo, 0, 6);
#ifdef _debug_CPU_
	debugstring("CPUPSAM init\r\n");
#endif
	display(0,0,"CPU_PSAM初始化...",0);
#ifdef FUJIAN_EKT_CPU_
	ii = SamColdReset(PSAM_YD, 38400, SAM_VOL3, buffer);	//福建一卡通的波特率真
#else
	ii = SamColdReset(PSAM_YD, 9600, SAM_VOL3, buffer);
#endif
	
	if(ii == 0){
		return ST_ERROR;
	}
	
#ifdef _debug_CPU_
	debugstring("CPUPSAM art:");
	debugdata(buffer, ii, 1);
#endif
	delayxms(5);
	if(Read_Binary(PSAM_YD, 0x16, 0, 6, buffer)==0) 
	{
#ifdef _debug_CPU_
		debugstring("CPURead_Binary error\r\n");
		debugdata(buffer, 8, 1);
#endif
		return ST_ERROR;
	}
#ifdef _debug_CPU_
	debugstring("CPUread 16 Binary:");
	debugdata(buffer,6,1);
#endif
	memcpy(G24PsamNo,buffer,6);
	delayxms(100);
	
	
#ifdef _debug_CPU_
	debugstring("CPUSelect File ");
#endif
	if(Select_File(PSAM_YD, 0x1001)==0) //0x3f01
	{
		debugstring("G24Select_File error\r\n");
		return ST_ERROR;
	}
//	memcpy((unsigned char*)&stuSamPubInf_LL, buffer, sizeof(stuSamPubInf_LL));
	
	gPsamErrorFlag &= 0xFD;
	return ST_OK;
#endif
}

extern unsigned char CPUMAC2(unsigned char PsamID, unsigned char *pMac, unsigned char *pData, unsigned char len);
extern unsigned char CpuCardSetMac(unsigned char PsamID, unsigned char *pMac, unsigned char *pData, unsigned char len);

void deal2or3(unsigned char*sndd)
{
	if(mode2or3 == 2){
		sndd[0] = 2;
		mode2or3 = 3;
	}
	else{
		sndd[0] = 3;
		mode2or3 = 2;
	}
}

unsigned char CPU_Read_Money(unsigned char *remb, unsigned char mode)
{
	unsigned char sndbuf[64];
	unsigned char revbuf[64];
	unsigned char ret;
#ifdef _debug_CPU_
	unsigned int temp;
#endif

	if (mode == 2)	//交通部
	{
		memcpy(sndbuf,"\x02\x80\x5c\x03\x02\x04",6);

	}else
	{
		memcpy(sndbuf,"\x02\x80\x5c\x00\x02\x04",6);
	}

	deal2or3(sndbuf);
	ret = RfPassthrough(revbuf, 2, sndbuf, 6);
	if(ret == 0){
		return 1;
	}
#ifdef _debug_CPU_
	debugstring("---remb:====");
	debugdata(revbuf,ret, 1);
	memcpy((unsigned char*)&temp, revbuf+1, 4);
	over_turn(4, (unsigned char*)&temp);
	sprintf((char *)sndbuf, "余额:%d.%02d\r\n", temp/100, temp%100);
	debugstring((char*)sndbuf);
#endif
	memcpy(remb, revbuf+1, 4);
	
	return 0;
	//	cmdbuff[0] = 0x03;
	//	picc_b_exchange_block_2(cmdbuff, 1, revbuff+10, 4);
}

//月票消费判断
extern unsigned char GetDateTime(void);
extern unsigned int get_Month_s_sum1(void);
unsigned int get_Month(unsigned char *rev)
{
	unsigned char i,ji;
	unsigned char date[3];
	unsigned char dateSum=0;
	unsigned char buffer_29[16],buffer_30[16];
	unsigned int temp10, uii, ui30, uinow;
	unsigned int s_sumTmp;
	unsigned int SUMa=0;
	unsigned int SUMb=0;
	unsigned int SUMc=0;
	unsigned long Pm=0;
	unsigned char block29=ST_ERROR;
	unsigned char block30=ST_ERROR;
	unsigned char block2930=ST_ERROR;
	unsigned int  value;
#ifdef _debug_CPU_
	debugstring("get_Month_purs:");
	debugdata(rev, 64, 1);

#endif
	GetDateTime();
	memcpy(buffer_29, rev, 16);
	memcpy(buffer_30, rev+16, 16);
	memcpy(date,&SysTime.year_l,3);          //年月日
	dateSum=(((date[1]&0xf0)>>4)*10)+(date[1]&0x0f);//当前月份的值

	if(CPU_Read_Money((unsigned char*)&value, 3))
		return ST_ERROR;
		over_turn(4, (unsigned char*)&value);
	//	s_sum1=get_Month_s_sum1();
		s_sum1=1;
		s_sumTmp = s_sum1;//赋值 扣一次
		a_sum1=value;
	if(a_sum1<=0)//去扣钱
		return 100;

	if(cardMessage.Zicard_catalog == 0xA1){//为年票
#ifdef _debug_
	debugstring("Month year!!!\r\n");
	debugdata(date,3,1);
#endif
		uii = 0;
		memcpy((unsigned char*)&uii, buffer_29, 3);//有效期
		over_turn(4, (unsigned char*)&uii);
		ui30 = 0;
		memcpy((unsigned char*)&ui30, buffer_30, 3);//有效期
		over_turn(4, (unsigned char*)&ui30);
		uinow = 0;
		memcpy((unsigned char*)&uinow, (unsigned char*)&SysTime.year_l, 3);
		over_turn(4, (unsigned char*)&uinow);


		if(uii < ui30){	//15 16
			if(uinow < uii){
				block29=ST_OK;
				block2930=ST_OK;//下年还有
			}
			else if(uinow < ui30){
				block30=ST_OK;//没有下一年了
			}
			else
				return 100;
		}
		else{	//17 16
			if(uinow < ui30){
				block30=ST_OK;
					block2930=ST_OK; //下年还有
			}
			else if(uinow < uii){//19
				block29=ST_OK;  //这里由原来的30改成29没有下一年了
			}
			else
				return 100;
		}
		if(block29 == ST_OK){
			temp10 = 0;
			memcpy((unsigned char*)&temp10, buffer_29+13, 2);//当前充的总点数
			SUMb = temp10;
			SUMa = 0;
			if(block2930 == ST_OK){//下一年还有
				temp10 = 0;
				memcpy((unsigned char*)&temp10, buffer_30+13, 2);//当前充的总点数
				SUMb += temp10;
				SUMa = temp10;
			}
			SUMc = SUMb;
		}
		else{
			temp10 = 0;
			memcpy((unsigned char*)&temp10, buffer_30+13, 2);//当前充的总点数
			SUMb = temp10;
			SUMa = 0;
			if(block2930 == ST_OK){//下一年还有
				temp10 = 0;
				memcpy((unsigned char*)&temp10, buffer_29+13, 2);//当前充的总点数
				SUMb += temp10;
				SUMa = temp10;
			}
			SUMc = SUMb;
		}
	}
	else if(cardMessage.Zicard_catalog == 0xA2){//为季票
#ifdef _debug_
		debugstring("Month Quarter!!!\r\n");
#endif
	//	date[0]=(((date[0]&0xf0)>>4)*10)+(date[0]&0x0f);//当前年份份的值
		if(date[0]==buffer_29[0])//
		{
			block29=ST_OK;
			if(date[0]<buffer_30[0])
			{
				block2930=ST_OK;//下年还有
			}
		}
		else if(date[0]==buffer_30[0]) 
		{
			block30=ST_OK;
			if(date[0]<buffer_29[0])
				block2930=ST_OK;//下年还有
		}
		else 
		{
			return 100;	   //本年没有充季票
		}

		if(block29==ST_OK)
		{
			uii = 0;
			ji = ((dateSum-1)/3);//季度 dataSum=0 - 12
			memcpy((unsigned char*)&uii, buffer_29+1+(ji*3), 3);
#ifdef _debug_
			debugstring("季29：");
			debugdata((unsigned char*)&ji, 1, 1);
			debugdata((unsigned char*)&uii, 4, 1);
#endif
			if(uii == 0) //本季没有充次
			{
				return 100;
			}
			for(i=ji;i<4;i++){
				memcpy((unsigned char*)&uii, buffer_29+1+(i*3), 3);
				SUMb+=uii;
#ifdef _debug_
				debugstring("本年总点SUMb：");
				debugdata((unsigned char*)&SUMb, 4, 1);
#endif
			}
			temp10 = 0;
			memcpy((unsigned char*)&temp10,buffer_29+13,2);
			SUMc=temp10 ;			//将来各月点数和
			
			if(block2930==ST_OK)
			{
				for(i=0;i<4;i++){
					memcpy((unsigned char*)&uii, buffer_30+1+(i*3), 3);
					SUMb+=uii;
#ifdef _debug_
					debugstring("本年总点sumb：");
					debugdata((unsigned char*)&SUMb, 4, 1);
#endif
				}
				temp10 = 0;
				memcpy((unsigned char*)&temp10 ,buffer_30+13,2);
				SUMc+=temp10;
			}
			memcpy((unsigned char*)&uii, buffer_29+1+(ji*3), 3);
			SUMa=SUMb-uii;
		}
		else if(block30==ST_OK)
		{
			uii = 0;
			ji = ((dateSum-1)/3);//季度 dataSum=0 - 12
			memcpy((unsigned char*)&uii, buffer_30+1+(ji*3), 3);
#ifdef _debug_
			debugstring("季30：");
			debugdata((unsigned char*)&ji, 1, 1);
			debugdata((unsigned char*)&uii, 4, 1);
#endif
			if(uii == 0) //本季没有充次
			{
				return 100;
			}
			for(i=ji;i<4;i++){
				memcpy((unsigned char*)&uii, buffer_30+1+(i*3), 3);
				SUMb+=uii;
#ifdef _debug_
				debugstring("本年总点3：");
				debugdata((unsigned char*)&SUMb, 4, 1);
#endif
			}
			temp10 = 0;
			memcpy((unsigned char*)&temp10,buffer_30+13,2);
			SUMc=temp10 ;			//将来各月点数和
			
			if(block2930==ST_OK)
			{
				for(i=0;i<4;i++){
					memcpy((unsigned char*)&uii, buffer_29+1+(i*3), 3);
					SUMb+=uii;
#ifdef _debug_
					debugstring("本年总点4：");
					debugdata((unsigned char*)&SUMb, 4, 1);
#endif
				}
				temp10 = 0;
				memcpy((unsigned char*)&temp10 ,buffer_29+13,2);
				SUMc+=temp10;
			}
			memcpy((unsigned char*)&uii, buffer_30+1+(ji*3), 3);
			SUMa=SUMb-uii;
		}
		if((SUMb<SUMa)||(SUMb>SUMc)||(SUMa>SUMc)) 
		{
		//	debugstring("次数有问题\r\n");
			return 100;	
		}

	}
	else{//是月票

#ifdef _debug_
		debugstring("Month Month!!!\r\n");
#endif
	//	date[0]=(((date[0]&0xf0)>>4)*10)+(date[0]&0x0f);//当前年份份的值
		if(date[0]==buffer_29[0]) //当前年在29
		{
			block29=ST_OK;
			if(date[0]<buffer_30[0])//下年还有次数
			{
				block2930=ST_OK;
			}
		}
		else if(date[0]==buffer_30[0]) //当前年在30
		{
			block30=ST_OK;
			if(date[0]<buffer_29[0])//下年还有次数
				block2930=ST_OK;
		}
		else 
		{
			return 100;	   //月票块坏了，去扣钱								 
		}

		if(block29==ST_OK)
		{
			if(!buffer_29[dateSum]) //当前月份没有充值
			{
				return 100;
			}
			for(i=dateSum;i<13;i++)
				SUMb+=buffer_29[i];//当前月以后 SUMb
			temp10 = 0;
			memcpy((unsigned char*)&temp10,buffer_29+13,2);
			SUMc=temp10 ;			//将来各月点数和
			if(block2930==ST_OK)
			{
				for(i=1;i<13;i++)
					SUMb+=buffer_30[i];//两年总和
				temp10 = 0;
				memcpy((unsigned char*)&temp10 ,buffer_30+13,2);
				SUMc+=temp10;
			}
				SUMa=SUMb-buffer_29[dateSum];	//除本月的次数
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
			MSG_LOG("用完了\r\n");
			return 100;	
		}
	}
	Pm=a_sum1;
#ifdef _debug_
	debugstring("下一年SUMa:");
	debugdata((unsigned char*)&SUMa, 4, 1);//除本月以后的点数
	debugstring("总SUMb:");
	debugdata((unsigned char*)&SUMb, 4, 1);//本月及以后点数
	debugstring("SUMc:");
	debugdata((unsigned char*)&SUMc, 4, 1);
#endif

#ifdef _debug_
	debugstring("Month MMM 6 !!!\r\n");
	debugdata((unsigned char*)&a_sum1, 4, 1);
#endif

	if(Pm<=SUMa)//此时的已经用完
	{
		MSG_LOG("用完了\r\n");
		return 100;
	}
	if(s_sum1 > 3){
		MSG_LOG(" 次数不对 \r\n");
		return ST_ERROR;
	}
//当前余额大于除本月后的点数，并且小鱼所有点数和
    if((Pm>=SUMa)&&(Pm<=SUMb))
	{	
		MSG_LOG("正常扣费\r\n");
		if(a_sum1>=s_sum1)
		{
			a_sum1-=s_sum1;
			dis_sum2=a_sum1-SUMa;
		}
		else 
		{
			MSG_LOG("用完了1\r\n");
			return 100;
		}
	}
	else if(Pm>SUMb)//余额大于 本月及以后点数，需减掉以前的。
	{
			MSG_LOG("需减掉以前的扣费\r\n");
		s_sum1=Pm-SUMb+s_sumTmp;
		if(a_sum1>=s_sum1)
		{
			a_sum1-=s_sum1;
			if(a_sum1 > SUMa)
				dis_sum2=a_sum1-SUMa;
			else
				dis_sum2=a_sum1;
			MSG_LOG("s_sum1:%d\r\n",s_sum1);
		}
		else
		{
			return 100;
		}
	}
	else 
		return ST_ERROR;
	
	return ST_OK;

}
unsigned char CPUDealCard(unsigned char mode, unsigned char cool);
unsigned char CPU_month_deal(void)
{
//	unsigned char errorf=0,i=0;				  
	unsigned char sndbuf[100];
	unsigned char revbuf[100];
//	unsigned char TACC[8];
//	unsigned char psRandom[10];
	unsigned int ret;
//	unsigned short ii;
//	unsigned char month[64];
//	unsigned char mothFlag;
//	unsigned char monthstyle;//月票类型 0是月票 1 是机票 2 是年票 3 是钱包

//	unsigned int value;
	
#ifdef _debug_CPU_
	debugstring("CPU _MONTH _DEAL::\r\n");
#endif
	if(cardSound == 0xaa){//钱包需要重刷，去钱包扣钱
		if(card_ser!=c_serial){
			card_ser = 0;
			cardSound = 0;
			//return ST_ERROR;
		}
		else
			return 3;
	}	
	if(cardSound == 0xbb){//月票需要重刷
		if(card_ser!=c_serial){
			card_ser = 0;
			cardSound = 0;
			//return ST_ERROR;
		}
	}


//**************************************

	memcpy(sndbuf, "\x02\x00\xA4\x00\x00\x02\x3F\x02",8);//有此卡需要用文件名，所以取不到时需要处理一下。
	deal2or3(sndbuf);
	ret = RfPassthrough(revbuf, 2, sndbuf, 8);
#ifdef _debug_CPU_
	debugdata(sndbuf, 15, 1);
	debugdata(revbuf, ret, 1);
#endif
	if(ret == 0)
		return ST_ERROR;
	else if(memcmp(revbuf+ret-2, "\x90\x00", 2)!=0)//选文件出错
	 return ST_ERROR;

//0003文件
	//0~15
	//16~31
	//32_48
	memcpy(sndbuf,"\x02\x00\xB0\x83\x00\x20",6);//读月票32个字节
	deal2or3(sndbuf);
	ret = RfPassthrough(revbuf, 2, sndbuf, 6);
#ifdef _debug_CPU_
	debugstring("0003文件:");
	debugdata(revbuf, ret, 1);
#endif
	if(ret == 0)
		return ST_ERROR;
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return 3;	//如果没有此文件，去扣钱
//*********************************
		ret=get_Month(revbuf+1);
		if(ret==ST_ERROR)
			return ST_ERROR;
		if(ret==100)
			return 3;//去扣钱
		MSG_LOG("get_Month:%d",ret);
		ret = CPUDealCard(MONTH_CARD, 0);
		return ret;
}

#ifdef ZongKong_CPU_ //中控的CPU单独处理
unsigned char CpuZongKongdDeal(void)
{
	unsigned int itemp,startDate, endDate;
	unsigned int ret, i;
	
	unsigned char userID[18];
	unsigned char RZkey[16];
	unsigned char Radommm[8];
	unsigned char sndbuf[100];
	unsigned char revbuf[100];
	
	for (i=0; i<3; i++)
	{
//	set_timer0(1000, 2); 用于测消费时间
#ifdef _debug_CPU_
	debugstring("取随机数据1:");
#endif
	memcpy(sndbuf, "\x02\x00\x84\x00\x00\x08",6);
	deal2or3(sndbuf);
	ret = RfPassthrough(revbuf, 2, sndbuf, 6);
	if(ret == 0)
		return ST_ERROR;
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return ST_ERROR;
	memcpy(Radommm, revbuf+1, 8);
#ifdef _debug_CPU_
	debugdata(Radommm, 8, 1);
#endif
	
#ifdef _debug_CPU_
	debugstring("3des加密1:");
#endif
	memcpy(RZkey, "DQLHMkey21132379", 16);//主文件外部认证密钥
	DES3_encrypt(Radommm, RZkey, revbuf);
#ifdef _debug_CPU_
	debugdata(revbuf, 8, 1);
#endif
	
#ifdef _debug_CPU_
	debugstring("外部认证1:");
#endif
	memcpy(sndbuf, "\x02\x00\x82\x00\x00\x08",6);
	memcpy(sndbuf+6, revbuf, 8);
	deal2or3(sndbuf);
	ret = RfPassthrough(revbuf, 2, sndbuf, 14);
#ifdef _debug_CPU_
	debugdata((unsigned char*)&ret, 1, 1);
	debugdata(revbuf, ret, 1);
#endif
	if(ret != 3)
		continue;
	else
		break;
	}
	if(i >= 3)
		return ST_ERROR;
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return 8; //非法卡
	
#ifdef _debug_CPU_
	debugstring("选择公共信息文件:");
#endif
	memcpy(sndbuf, "\x02\x00\xA4\x00\x00\x02\x00\x01",8);
	deal2or3(sndbuf);
	ret = RfPassthrough(revbuf, 2, sndbuf, 8);
#ifdef _debug_CPU_
	debugdata(revbuf, ret, 1);
#endif
	if(ret == 0)
		return ST_ERROR;
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return ST_ERROR;
	
#ifdef _debug_CPU_
	debugstring("读公共信息文件:");
#endif
	memcpy(sndbuf, "\x02\x00\xB0\x81\x00\x3A",6);
	deal2or3(sndbuf);
	ret = RfPassthrough(revbuf, 2, sndbuf, 6);
#ifdef _debug_CPU_
	debugdata(revbuf, ret, 1);//0331303030303030303032 313233343536373839303132333435363743 D6D0BFD8BFC6BCBCFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF9000
#endif
	if(ret == 0)
		return ST_ERROR;
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return ST_ERROR;
	memcpy(userID, revbuf+11, 18);
//---------------------下面是车载机应用-------------------------------
	
#ifdef _debug_CPU_
	debugstring("选择车载应用:");
#endif
	memcpy(sndbuf, "\x02\x00\xA4\x00\x00\x02\x3F\x02",8);
	deal2or3(sndbuf);
	ret = RfPassthrough(revbuf, 2, sndbuf, 8);
#ifdef _debug_CPU_
	debugdata(revbuf, ret, 1);
#endif
	if(ret == 0)
		return ST_ERROR;
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return ST_ERROR;
	
#ifdef _debug_CPU_
	debugstring("取随机数据2:");
#endif
	memcpy(sndbuf, "\x02\x00\x84\x00\x00\x08",6);
	deal2or3(sndbuf);
	ret = RfPassthrough(revbuf, 2, sndbuf, 6);
	if(ret == 0)
		return ST_ERROR;
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return ST_ERROR;
	memcpy(Radommm, revbuf+1, 8);
#ifdef _debug_CPU_
	debugdata(Radommm, 8, 1);
#endif
	
#ifdef _debug_CPU_
	debugstring("3des加密2:");
#endif
	memcpy(RZkey, "DQLHVkey13018186", 16);//主文件外部认证密钥
	DES3_encrypt(Radommm, RZkey, revbuf);
#ifdef _debug_CPU_
	debugdata(revbuf, 8, 1);
#endif
	 
#ifdef _debug_CPU_
	debugstring("外部认证2:");
#endif
	memcpy(sndbuf, "\x02\x00\x82\x00\x00\x08",6);
	memcpy(sndbuf+6, revbuf, 8);
	deal2or3(sndbuf);
	ret = RfPassthrough(revbuf, 2, sndbuf, 14);
#ifdef _debug_CPU_
	debugdata((unsigned char*)&ret, 1, 1);
	debugdata(revbuf, ret, 1);
#endif
	if(ret != 3)
		return ST_ERROR;
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return 8; //非法卡
	
#ifdef _debug_CPU_
	debugstring("选择公共信息文件:");
#endif
	memcpy(sndbuf, "\x02\x00\xA4\x00\x00\x02\x00\x06",8);
	deal2or3(sndbuf);
	ret = RfPassthrough(revbuf, 2, sndbuf, 8);
#ifdef _debug_CPU_
	debugdata(revbuf, ret, 1);
#endif
	if(ret == 0)
		return ST_ERROR;
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return ST_ERROR;
	
#ifdef _debug_CPU_
	debugstring("读公共信息文件:");
#endif
	memcpy(sndbuf, "\x02\x00\xB0\x86\x00\x14",6);
	deal2or3(sndbuf);
	ret = RfPassthrough(revbuf, 2, sndbuf, 6);
#ifdef _debug_CPU_
	debugdata(revbuf, ret, 1);//02 7C000000 23063301 DA2A3301 40E20100 02  0000FF9000
								// 卡号    StartDate EndDate   UserPwd CardType
#endif
	if(ret == 0)
		return ST_ERROR;
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return ST_ERROR;
	
	memcpy(cardMessage.PublishBicker, revbuf+1, 4); //卡号
	cardMessage.card_catalog = revbuf[17];
	memcpy((unsigned char*)&itemp, revbuf+5, 4);
	sprintf((char*)sndbuf, "%08u", itemp);
	Ascii2BCD(sndbuf, (unsigned char*)&startDate, 8);
	memcpy((unsigned char*)&itemp, revbuf+9, 4);
	sprintf((char*)sndbuf, "%08u", itemp);
	Ascii2BCD(sndbuf, (unsigned char*)&endDate, 8);
#ifdef _debug_CPU_
	debugdata((unsigned char*)&startDate, 4, 1);
	debugdata((unsigned char*)&endDate, 4, 1);
#endif
	
	if(IsInBlackList(cardMessage.PublishBicker) < FLASH_BLK_END){
		return CARD_BLACK_PRO;
	}

	memcpy((unsigned char *)&itemp, (unsigned char*)&SysTime, 4);
	over_turn(4, (unsigned char *)&itemp);
	over_turn(4, (unsigned char *)&startDate);
	over_turn(4, (unsigned char *)&endDate);
	if((itemp < startDate)||(itemp > endDate)){
		return CARD_NO_TIME;
	}
	
	memcpy((unsigned char*)&c_serial, cardMessage.PublishBicker, 4);	//这里以卡号代替序列号判断
	if(month_decide()) // 不能连刷
	{
		disp_no_swipe();
		return ST_OK;
	}
/* 	
#ifdef _debug_CPU_
	debugstring("选择公共信息文件:");
#endif
	memcpy(sndbuf, "\x02\x00\xA4\x00\x00\x02\x00\x07",8);
	deal2or3(sndbuf);
	ret = RfPassthrough(revbuf, 2, sndbuf, 8);
#ifdef _debug_CPU_
	debugdata(revbuf, ret, 1);
#endif
	if(ret == 0)
		return ST_ERROR;
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return ST_ERROR;
现在还没用到消费
#ifdef _debug_CPU_
	debugstring("读公消费文件:");
#endif
	memcpy(sndbuf, "\x02\x00\xB0\x87\x00\x14",6);
	deal2or3(sndbuf);
	ret = RfPassthrough(revbuf, 2, sndbuf, 6);
#ifdef _debug_CPU_
	debugdata(revbuf, ret, 1);//02 7C000000 23063301 DA2A3301 40E20100 02  0000FF9000
	// 卡号    StartDate EndDate   UserPwd CardType
#endif
	if(ret == 0)
		return ST_ERROR;
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return ST_ERROR;

#ifdef _debug_CPU_
	debugstring("写公消费文件:");
#endif
	memcpy(sndbuf, "\x02\x00\xD6\x00\x00\x14",6);
	memcpy(sndbuf+6, revbuf+1, 0x14);
	deal2or3(sndbuf);
	ret = RfPassthrough(revbuf, 2, sndbuf, (0x14+6));
#ifdef _debug_CPU_
	debugdata(revbuf, ret, 1);//02 7C000000 23063301 DA2A3301 40E20100 02  0000FF9000
	// 卡号    StartDate EndDate   UserPwd CardType
#endif
	if(ret == 0)
		return ST_ERROR;
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return ST_ERROR;
*/

// 	itemp = get_timer0(2);用于测消费时间
//	itemp = (1000-itemp);
// 	debugdata((unsigned char*)&itemp, 4, 1);
	cls();
	memcpy((unsigned char*)&itemp, cardMessage.PublishBicker, 4);
	sprintf((char*)sndbuf, "卡号:%u", itemp);
	display(0, 0, (char*)sndbuf, 0);
	sprintf((char*)sndbuf, "卡类:%u", cardMessage.card_catalog);
	display(2, 0, (char*)sndbuf, 0);
	memcpy(Radommm, (unsigned char*)&endDate, 4);
	over_turn(4, Radommm);
	sprintf((char*)sndbuf, "有效:%02X%02X-%02X-%02X", Radommm[0], Radommm[1], Radommm[2], Radommm[3]);
	display(4, 0, (char*)sndbuf, 0);
	audio(Audio_SuaKaOK);
	save_h_month();
	//组织记录
	a_sum1 = 0;
	s_sum1 = 0;
	memcpy(irda_rxBuf, userID, 18);
	BuildRecorde(ID_REC_TOLL, sndbuf);
	WriteRecord(sndbuf, 0);
	delayxms(500);
	return ST_OK;
}
#endif	//#ifdef ZongKong_CPU_ //中控的CPU单独处理

extern unsigned char check_time(void);
extern unsigned char StufferNO[4];
extern unsigned int startTime,endTime,nowTime,endTimeEn;
		unsigned char CARD_TIME(unsigned char *YouHui_Date)
		{
			unsigned char sndbuf[32];
			unsigned int end_time=0,itemp=0,ret=0;
			memset(sndbuf,0,sizeof(sndbuf));
#ifdef _debug_CPU_
			debugstring("看过期:\r\n");
			debugdata(YouHui_Date,4,1);
#endif
			
			
			memcpy(sndbuf,YouHui_Date,4);
			memcpy(sndbuf+4,"\x11\x59\x59",3);
			
			if (time_invalid(sndbuf) == 1)
			{
				memcpy((unsigned char*)&end_time, YouHui_Date, 4);//有效日期
				over_turn(4, (unsigned char*)&end_time);
				
				
				
				memcpy((unsigned char*)&itemp, (unsigned char *)&SysTime, 4);//当前日期
				over_turn(4, (unsigned char*)&itemp);
				
				if(end_time<itemp)
					return CARD_NO_TIME;	//过期卡
				
				nowTime = BCDTime2Long((unsigned char *)&SysTime);
				
				itemp = BCDTime2Long((unsigned char *)sndbuf);
				
				if (nowTime>itemp)
				{
					ret = nowTime-itemp;
				}else
				{
					ret = itemp-nowTime;
				}
				
				MSG_LOG("nowTime:%u\r\n",nowTime);
				MSG_LOG("itemp  :%u\r\n",itemp);
				
				dis_overdate = ret/86400;  //1天(d)=86400秒(s)
				
				
				MSG_LOG("dis_overdate:%d\r\n",dis_overdate);
			}else{
				return CARD_NO_TIME;	//过期卡
			}
			return ST_OK;
			
		}
unsigned char CPUcardType(void)
{
	//	unsigned char PSAMDsno[4];
//	unsigned int itemp;//,value;
	unsigned int ret=0;
	unsigned int start_time,end_time;
	unsigned int itemp=0;
	unsigned char sndbuf[100];
	unsigned char revbuf[100];
	unsigned char sw[2];
// 	MF_PublicBacinformation1 *Root_File15;
	stJSBF15 *JSBFile15;
	SaleInformation1 *file14;

	unsigned char CITYNO[2];
	Card_Area = 0;


	if(check_time())
		return ST_ERROR;



	if(F_FLAG_VARY.stop_flag)
		return ST_ERROR;
// 	memcpy(sndbuf,"\xe0\x50",2);
// 		ret = RfPassthrough(revbuf, 2, sndbuf, 2);
// 	 	mode2or3 = 2;
// 		delayxms(2);
	
		PcdRfReset(5);
		delayxms(3);
		profile(0);
		delayxms(1);
		for (itemp=0;itemp <4;itemp++)//有的卡复位过不去多试几次
		{
			MSG_LOG("复位e050::%d次\r\n",itemp+1);
			ret = RfPassthrough(revbuf, 2, "\xe0\x50", 2);
			
			if (ret <= 4 )		//都认为有问题
			{
				mode2or3 = 2;		
				PcdRfReset(3);
				delayxms(3);
				profile(0);
				continue;
			}
			debugstring("复位!\r\n");
			debugdata(revbuf, ret, 1);
			mode2or3 = 2;
			break;
		}
		if (itemp >=4)
		{
			return ST_ERROR;
		}





#ifdef _debug_CPU_
	debugstring("CPU start!\r\n");
	debugdata(revbuf, ret, 1);
#endif



	if (cardSound != 0)
	{
		if (c_serial != card_ser)
		{
#if defined _debug_ || defined _debug_trans
			printf("now:%08X,befor:%08X\r\n",c_serial,card_ser);
#endif
			return ST_ERROR ;
		}
	}



	//2、读根目录15file

/*
	memcpy(sndbuf,"\x00\xB0\x95\x00\x33",5);//  里面有卡类
	ret = RfPassthrough(revbuf, 2, sndbuf, 5);
	if(ret == 0)
		return ST_ERROR;

	memcpy(sw,revbuf+ret-2,2);
	MSG_LOG("sw:");
	BCD_LOG(sw,2,1);
	if((memcmp(sw,"\x62\x83",2)==0)||(memcmp(sw,"\x6A\x81",2)==0)||(memcmp(sw,"\x6A\x86",2)==0)||(memcmp(sw,"\x6A\x82",2)==0)||(memcmp(sw,"\x6D\x00",2)==0))
	{
		//没有选择到绵州通钱包，走交通部钱包
		printf("select MZT wrong!\n");
		return NO_CITY_UNION;
	}
	else if(memcmp(sw,"\x90\x00",2)!=0)
        return ST_ERROR;

	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return ST_ERROR;
	
	
#ifdef _debug_CPU_
	debugstring("sndbuf:");
	debugdata(sndbuf,5,1);
	debugdata((unsigned char*)&ret, 4,1);
	debugstring("Read 15:");
	debugdata(revbuf, ret, 1);
#endif
	
	Root_File15 = (MF_PublicBacinformation1 *)(revbuf+1);

	memcpy(CITYNO,Root_File15->cityCode,2);
	cardMessage.card_catalog = Root_File15->cardType[1];
	cardMessage.Zicard_catalog = Root_File15->cardType[0];
	if ((cardMessage.card_catalog <0x81) || (cardMessage.card_catalog >0xA0))		//32 种折扣，普通M1 0x41 开始，CPU卡 0x81 开始
	{
		MSG_LOG("卡类超范围:%02X\r\n",cardMessage.card_catalog);
		cardMessage.card_catalog = CARD_NORMAL_BUS;
		cardMessage.Zicard_catalog = 0;
	}
*/




		//4、选电子钱包应用
	bzero(sndbuf, sizeof(sndbuf));
	bzero(revbuf, sizeof(revbuf));
	memcpy(sndbuf,SELECT_APPY_MF, 19);//为了区分成都卡，因为成都的下面1001 和绵阳一样的

	MSG_LOG("SecAPP_MF:\r\n");
	BCD_LOG(sndbuf,19,1);
	ret = RfPassthrough(revbuf, 2, sndbuf, 19);
	if(ret == 0)
        return ST_ERROR;
	MSG_LOG("REC:\r\n");
	BCD_LOG(revbuf,ret,1);
	

	memcpy(sw,revbuf+ret-2,2);
	if((memcmp(sw,"\x62\x83",2)==0)||
		(memcmp(sw,"\x6A\x81",2)==0)||
		(memcmp(sw,"\x6A\x82",2)==0)
		
				)
	{
// 		beep_err(BEEP_DELAY_ERR);
// 		Audio(SOUND_INVALID);
		return NO_CITY_UNION;
	}
	else if(memcmp(sw,"\x90\x00",2)!=0)
        return ST_ERROR;



	
	if(gPsamErrorFlag&1){
		cls();
		if(gchn_eng == 'E'){
			display(4, 0, "PSAM2 error!",0);
		}
		else{
			display(0,0,"错误:",1);
			display(3,0,"CPUPSAM错误!",0);
		}
		restore_flag = 3;
		delayxs(1);
		return ST_ERROR;
	}



	//4、选电子钱包应用
	bzero(sndbuf, sizeof(sndbuf));
	bzero(revbuf, sizeof(revbuf));
	memcpy(sndbuf,SELECT_APPY_NEW, 21);		//绵阳新
// 	sendlen = 21;
	
	MSG_LOG("先选新的 \"MYMZT.ADF01 \":\r\n");
	BCD_LOG(sndbuf,21,1);
	ret = RfPassthrough(revbuf, 2, sndbuf, 21);
	if(ret == 0)
        return ST_ERROR;
	
	MSG_LOG("REC:\r\n");
	BCD_LOG(revbuf,ret,1);

	memcpy(sw,revbuf+ret-2,2);
	if(memcmp(sw,"\x90\x00",2)!=0)
	{
		MSG_LOG("选不到选旧的--\r\n");
			////////////////////////////////
			//4、选电子钱包应用
			bzero(sndbuf, sizeof(sndbuf));
			bzero(revbuf, sizeof(revbuf));
			memcpy(sndbuf,SELECT_APPY_1001, 18);		//绵阳旧的
			ret = RfPassthrough(revbuf, 2, sndbuf, 18);
			if(ret == 0)
				return ST_ERROR;

#ifdef _debug_CPU_
			printf("SELECT_APPY ret=%d\n", ret);
			debugdata(revbuf,ret,1);
			printf("\n");
#endif
			memcpy(sw,revbuf+ret-2,2);
			if((memcmp(sw,"\x62\x83",2)==0)||
			   (memcmp(sw,"\x6A\x81",2)==0)||
			   (memcmp(sw,"\x6A\x82",2)==0)

				)
			{
				// 		beep_err(BEEP_DELAY_ERR);
				// 		Audio(SOUND_INVALID);
				return NO_CITY_UNION;
			}
			else if(memcmp(sw,"\x90\x00",2)!=0)
				return ST_ERROR;

		

	}
	

	


	
	//5、读卡状态信息文件0x15

	MSG_LOG("读15文件\r\n");
	BCD_LOG(sndbuf,5,1);

	memcpy(sndbuf,"\x00\xb0\x95\x00\x1f",5);
	ret = RfPassthrough(revbuf, 2, sndbuf, 5);
	if(ret == 0)
		return ST_ERROR;
	if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
		return ST_ERROR;

	MSG_LOG("REC:\r\n");
	BCD_LOG(revbuf,ret,1);




	JSBFile15 = (stJSBF15 *)(revbuf+1);
	
	memcpy(cardMessage.TradeCode, JSBFile15->FCcode, 2);//发卡方标识
	memcpy(cardMessage.CityCode, JSBFile15->citycode, 2);//城市代码
	// 	memcpy(cardMessage.CardCheckCode,JSBFile15.MAC,CARD_CHECH_CODE_LEN);
	memcpy(cardMessage.PublishBicker, JSBFile15->FYFlag,10);//发卡流水号

	

	memcpy(CITYNO,JSBFile15->citycode,2);
	cardMessage.card_catalog = JSBFile15->card_catalog;
	cardMessage.Zicard_catalog = JSBFile15->Zicard_catalog;
	if ((cardMessage.card_catalog <0x81) || (cardMessage.card_catalog >0xA0))		//32 种折扣，普通M1 0x41 开始，CPU卡 0x81 开始
	{
		MSG_LOG("卡类超范围:%02X\r\n",cardMessage.card_catalog);
		cardMessage.card_catalog = CARD_NORMAL_BUS;
		cardMessage.Zicard_catalog = 0;
	}

	cardMessage.card_catalog_bak=cardMessage.card_catalog; //备份
	
	
// 	memcpy((unsigned char*)&stuPubInfZaoZuang.cPublisherSign, JSBFile15->YYNO, 8);//卡序列号  卡应用序列号，完整的，备用
// 	memcpy(stuPubInfZaoZuang.cApp_sn, JSBFile15->FYFlag, 10);	//卡应用序列号，完整的，备用
// 	memcpy(stuPubInfZaoZuang.cCitycode,JSBFile15->citycode, 2);	//
	
	
	
#ifdef _debug_CPU_
	debugstring("cardMessage.card_catalog:0x");
	debugdata((unsigned char*)&cardMessage.card_catalog,1, 1);
	debugstring("zcard-talog:0x");
	debugdata((unsigned char*)&cardMessage.Zicard_catalog,1, 1);
	debugstring(".PublishBicker:::");
	debugdata(cardMessage.PublishBicker, 10, 1);
	debugstring("启用日期::");
	debugdata((unsigned char*)&start_time, 4, 1);
	debugstring("有效期::");
	debugdata((unsigned char*)&end_time, 4, 1);
#endif
	
	
// 	if((itemp < start_time)||(itemp > end_time)){
// 		return CARD_NO_TIME;//过期或末到期
// 	}


	MSG_LOG("CITYNO:");
	BCD_LOG(CITYNO,2,1);
	BCD_LOG(PsamNo,6,1);


	dis_overdate = 0xff;

	if(memcmp(PsamNo,CITYNO,2)==0)    //绵阳本地卡,
	{


		MSG_LOG("本地卡\r\n");
		keyIndex = 0x11;
		//6、读公共应用基本信息文件0x15
		bzero(sndbuf, sizeof(sndbuf));
		bzero(revbuf, sizeof(revbuf));
		memcpy(sndbuf,"\x00\xb0\x94\x00\x20",5);
		ret = RfPassthrough(revbuf, 2, sndbuf, 5);
		if(ret == 0)
			return ST_ERROR;
		if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
			return ST_ERROR;

#ifdef _debug_CPU_
			printf("read 94 ret=%d\n", ret);
			debugdata(revbuf,ret,1);
			printf("\n");
#endif
		

			
		file14 = (SaleInformation1 *)(revbuf+1);


		switch(file14->enable)
		{
		case NOTENABLED:
			return CARD_NO_USE;
		case DISABLED:
			return CARD_NO_TIME;	//过期卡		
		case ENABLED:
			break;
		default:
			return CARD_NO_SYSTEM;
		}



/*
		//老人卡，残疾卡，子女卡需要进行年审判断20150514
		if ((ret==CARD_OLDMAN_BUS)||(ret==CARD_Love_BUS)||(ret==CARD_ZILU_BUS))
		{


			memcpy((unsigned char*)&end_time, file14->cardExpirationDate, 4);//有效日期
			over_turn(4, (unsigned char*)&end_time);
			
			
			
			memcpy((unsigned char*)&itemp, (unsigned char *)&SysTime, 4);//当前日期
			over_turn(4, (unsigned char*)&itemp);

			if(end_time<itemp)
				return CARD_NO_TIME;	//过期卡
		}
		*/

		
		//(3)读0x16公共应用基本数据文件
		bzero(sndbuf, sizeof(sndbuf));
		bzero(revbuf, sizeof(revbuf));
		memcpy(sndbuf,"\x00\xb0\x96\x00\x50",5);
		ret = RfPassthrough(revbuf, 2, sndbuf, 5);
		if(ret == 0)
			return ST_ERROR;
		if(memcmp(revbuf+ret-2, "\x90\x00", 2) != 0)
			return ST_ERROR;
		
#ifdef _debug_CPU_
		printf("read 96 ret=%d\n", ret);
		debugdata(revbuf,ret,1);
		printf("\n");
#endif
		MSG_LOG("YouHui_Date:");
		BCD_LOG(revbuf+58,4,1);

		
        Card_Area = revbuf[57];//县区号

		memcpy(YouHui_Date,revbuf+58,4);
		
		//如果卡类不在特殊卡类定义的区域内，就按照普通卡折扣进行交易
		MSG_LOG("Card_Area = %02X\n",Card_Area);
		
	#ifndef JIULONG 

//判断是否是本郊县
#ifdef MZT_JL
if(Card_Area == code_96_57 ||(Card_Area==1&&(cardMessage.card_catalog -0x40==CARD_OLDMAN_BUS ||cardMessage.card_catalog -0x40==CARD_Love_BUS ||cardMessage.card_catalog -0x40==CARD_Love_BUS_2)))  //九龙比较特殊,现在是01的老人卡,残疾人可以刷，其他01都当普通卡

#else
	if(Card_Area == code_96_57)  //本郊县
#endif
	{
		YouHui_Area_other=0xae;
		YouHui_Area = 1;//可以优惠区域
		
		memcpy(YouHui_Date,revbuf+58,4);
		MSG_LOG("YouHui_Date:");
		BCD_LOG(revbuf,58,4);
		
			//判断有效期
//城区代码   0x80:北川  0x40:盐亭 0x20:三台  0x10:梓潼  0x08:安县  0x04:平武县
	ret = cardMessage.card_catalog -0x40;		//CPU 卡81 开始的 M1 卡41开始
		 memcpy(YouHui_Date,revbuf+58,4);
		//看有效期
	switch(Card_Area)
	{
	case 0x80:
		if(CARD_TIME(YouHui_Date)==CARD_NO_TIME &&((ret==CARD_STUDENT_BUS)||(ret==CARD_Love_BUS)||(ret==CARD_Love_BUS_2)||(ret == CARD_STUFF_BUS)||(ret==CARD_ZILU_BUS)))
		{
			#ifdef _debug_CPU_
			debugstring("过期:\r\n");
			debugdata(YouHui_Date,4,1);
			#endif
			return CARD_NO_TIME;
		}
		break;
	case 0x20:
		if(CARD_TIME(YouHui_Date)==CARD_NO_TIME &&((ret==CARD_Love_BUS)||(ret==CARD_Love_BUS_2)||(ret == CARD_STUFF_BUS)||(ret==CARD_ZILU_BUS)))
		{
			#ifdef _debug_CPU_
			debugstring("过期:\r\n");
			debugdata(YouHui_Date,4,1);
			#endif
			return CARD_NO_TIME;
		}
		break;
	case 0x40:
		if(CARD_TIME(YouHui_Date)==CARD_NO_TIME &&((ret==CARD_OLDMAN_BUS)||(ret==CARD_STUDENT_BUS)||(ret==CARD_Love_BUS)||(ret==CARD_Love_BUS_2)||(ret == CARD_STUFF_BUS)||(ret==CARD_ZILU_BUS)))
		{
			#ifdef _debug_CPU_
			debugstring("过期:\r\n");
			debugdata(YouHui_Date,4,1);
			#endif
			return CARD_NO_TIME;
		}
		break;
	case 0x01:
	case 0x03:
		if(CARD_TIME(YouHui_Date)==CARD_NO_TIME &&((ret==CARD_OLDMAN_BUS)||(ret==CARD_STUDENT_BUS)||(ret==CARD_Love_BUS)||(ret==CARD_Love_BUS_2)||(ret == CARD_STUFF_BUS)||(ret==CARD_ZILU_BUS)))
		{
#ifdef _debug_CPU_
			debugstring("过期:\r\n");
			debugdata(YouHui_Date,4,1);
#endif
			return CARD_NO_TIME;
		}
		break;
	default:
		if(CARD_TIME(YouHui_Date)==CARD_NO_TIME &&((ret==CARD_OLDMAN_BUS)||(ret==CARD_STUDENT_BUS)||(ret==CARD_Love_BUS)||(ret==CARD_Love_BUS_2)||(ret == CARD_STUFF_BUS)||(ret==CARD_ZILU_BUS)))
		{
			#ifdef _debug_CPU_
			debugstring("过期:\r\n");
			debugdata(YouHui_Date,4,1);
			#endif
			return CARD_NO_TIME;
		}
		break;
}



	}
	else
	{	
		YouHui_Area_other=0xad;
		MSG_LOG("其他郊县当普通卡处理\r\n");
			ret = cardMessage.card_catalog -0x40;
		if(ret!=CARD_NORMAL_BUS)
		{	cardMessage.card_catalog=CARD_NORMAL_BUS+0x40;
		//	cardMessage.Zicard_catalog = 0;
		}
			YouHui_Area = 0;
	
	}
	
		
	


#else  //九龙公交
		//只有绵阳城区的卡可以实现优惠
		if(Card_Area == 0x01)
		{
			YouHui_Area = 1;//可以优惠区域
            memcpy(YouHui_Date,revbuf+58,4);
			
			MSG_LOG("YouHui_Date:");
			BCD_LOG(revbuf,58,4);
			
       	}
		else
			YouHui_Area = 0;

		ret = cardMessage.card_catalog -0x40;		//CPU 卡81 开始的 M1 卡41开始
		
		//老 残 员工卡要看有效期
		if((Card_Area == 0x03) &&((ret==CARD_OLDMAN_BUS)||(ret==CARD_Love_BUS)||(ret == CARD_STUFF_BUS)))
		{
#ifdef _debug_CPU_
			debugstring("看过期:\r\n");
			debugdata(YouHui_Date,4,1);
#endif
			
			
			memcpy(sndbuf,YouHui_Date,4);
			memcpy(sndbuf+4,"\x11\x59\x59",3);
			
			if (time_invalid(sndbuf) == 1)
			{
				memcpy((unsigned char*)&end_time, YouHui_Date, 4);//有效日期
				over_turn(4, (unsigned char*)&end_time);
				
				
				
				memcpy((unsigned char*)&itemp, (unsigned char *)&SysTime, 4);//当前日期
				over_turn(4, (unsigned char*)&itemp);
				
				if(end_time<itemp)
					return CARD_NO_TIME;	//过期卡
				
				nowTime = BCDTime2Long((unsigned char *)&SysTime);
				
				itemp = BCDTime2Long((unsigned char *)sndbuf);
				
				if (nowTime>itemp)
				{
					ret = nowTime-itemp;
				}else
				{
					ret = itemp-nowTime;
				}
				
				MSG_LOG("nowTime:%u\r\n",nowTime);
				MSG_LOG("itemp  :%u\r\n",itemp);
				
				dis_overdate = ret/86400;  //1天(d)=86400秒(s)
				
				
				MSG_LOG("dis_overdate:%d\r\n",dis_overdate);
			}else{
				return CARD_NO_TIME;	//过期卡
			}
			
			
			
			
		}
#endif


		if(IsInBlackList(cardMessage.PublishBicker+2) < FLASH_BLK_END){
			return CARD_BLACK_PRO;
		}



		

	}else{

		keyIndex = 0x01;    //异地卡，使用01密钥索引

	}



	ret=cardMessage.card_catalog;

	if(cardMessage.card_catalog >= 0x80){
		ret = cardMessage.card_catalog - 0x80;
	}
	else if(cardMessage.card_catalog >= 0x40)
	{
		ret = cardMessage.card_catalog-0x40;
	}else{
		
		ret = cardMessage.card_catalog;
		
	}

	
	MSG_LOG("ret:0x%02X,%d\r\n",ret,NewPriceTab.rate[ret]);




	
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
	if(month_decide()) // 第二次去扣钱包(次数不能连刷)
	{
		return CARD_MONEY;
		
	}	
	if((NewPriceTab.rate[ret]==202)||(NewPriceTab.rate[ret] ==102) )
	{
		
		return CARD_FREE;	
	
	}
	return CARD_MONEY;


		
}
//extern int dealCardInfo(unsigned char *file15, unsigned char *outDat); //tttttttttttttttttttttttt
//extern int CoutMAC(unsigned char *inDat, unsigned char *outDat);

extern void Save_delayCardinfo(unsigned char Cmode);
extern unsigned char getMiKey(void);
unsigned char CPUDealCard(unsigned char mode, unsigned char cool)
{
	unsigned char i;
	unsigned char sndbuf[64];
	unsigned char revbuf[RECORD_LEN+32];
	unsigned char TACC[4];
//	unsigned char PSAMDsno[4];
	unsigned int revLen,ret;
	unsigned int value;
	unsigned short ii;
#ifdef NO_CPU_PSAM_
	unsigned char CPUPKEY[16];
	unsigned char CPUpInData1[8];
	unsigned char CPUinitdat[8];
	unsigned char snddat[64];
#endif

	gMCardCand= CARDSTYLE_CPU;
	
	if(F_FLAG_VARY.stop_flag) 
		return ST_ERROR;
	//选择应用目录 3F01   03 6F 0C 84 06 5A 42 47 47 51 42 A5 04 9F 08 90 00

// 		if(mode==MONEY_CARD){//月票进来需要切换
// #ifdef _debug_CPU_
// 			debugstring("选择应用 3F01:开始\r\n");
// 			MSG_LOG("mode %d\r\n",mode);
// #endif
// 			memcpy(sndbuf, "\x02\x00\xA4\x00\x00\x02\x3F\x01",8);//有此卡需要用文件名，所以取不到时需要处理一下。
// 			deal2or3(sndbuf);
// 			ret = RfPassthrough(revbuf, 2, sndbuf, 8);
// #ifdef _debug_CPU_
// 			
// 		debugdata(sndbuf, 15, 1);
// 		debugdata(revbuf, ret, 1);
// #endif
// 			if(ret == 0)
// 				return ST_ERROR;
// 			else if(memcmp(revbuf+ret-2, "\x90\x00", 2)!=0)
// 				return 5;
// 		}


// 	if(Delay_Normal()){
// 		return ST_ERROR;
// 	}
	memset(TACC,0,sizeof(TACC));


// 	if(month_decide()) // 第二次去扣钱包(次数不能连刷)
// 	{
// 		disp_no_swipe();
// 		return ST_OK;	
// 	}

	if(CPU_Read_Money((unsigned char*)&value, 3))
		return ST_ERROR;
	over_turn(4, (unsigned char*)&value);



/*
	memcpy(sndbuf,"\x00\xB2\x01\xC4\x17",5);//取上笔记录 18文件
	ret = RfPassthrough(revbuf, 2, sndbuf, 5);
	if(ret == 0)
		return ST_ERROR;
#ifdef _debug_CPU_
	debugstring("取MZT:18记录文件----\r\nsnd:\r\n");
	debugdata(sndbuf, 5, 1);
	debugdata(revbuf, ret, 1);
#endif
	
	
	memcpy(sndbuf, "\x02\x80\x5A\x00\x06\x02", 6);//memcpy(sndbuf, "\x02\x80\x5A\x00\x05\x02", 6);
	memcpy((unsigned char*)&ii, stuInitPurchaseRet.cSnq, 2);
	over_turn(2, (unsigned char*)&ii);
	ii += 1;
	over_turn(2, (unsigned char*)&ii);
	memcpy(sndbuf+6, (unsigned char*)&ii, 2);
	sndbuf[8] = 8;
#ifdef _debug_CPU_
	display(0,0,"放拔",0);
	delayxms(500);
	beep(3,50,50);
	debugstring("防拔:");
	debugdata(stuInitPurchaseRet.cSnq, 2, 1);
	debugdata(sndbuf, 9, 1);
#endif
	deal2or3(sndbuf);
	revLen = RfPassthrough(revbuf, 2, sndbuf, 9);
	// 	if(revLen == 0)
	// 		return ST_ERROR;
#ifdef _debug_CPU_
	debugdata(revbuf, revLen, 1);//02 00 0C 00 00 ED F2 97 9B 6E 94 37 BC 90 00 F2 03
#endif
*/



	if(cardSound == 0xaa||cardSound==0xbb){//需要重刷	
#ifdef _debug_CPU_
		debugstring("aa余额:");
		debugdata((unsigned char*)&value, 4, 1);
		debugdata((unsigned char*)&a_sumR, 4, 1);
#endif
		if(value < a_sumR){//上次已经扣成功
			memcpy(sndbuf, "\x02\x80\x5A\x00\x06\x02", 6);//memcpy(sndbuf, "\x02\x80\x5A\x00\x05\x02", 6);
			memcpy((unsigned char*)&ii, stuInitPurchaseRet.cSnq, 2);
			over_turn(2, (unsigned char*)&ii);
			ii += 1;
			over_turn(2, (unsigned char*)&ii);
			memcpy(sndbuf+6, (unsigned char*)&ii, 2);
			sndbuf[8] = 8;
#ifdef _debug_CPU_
			display(0,0,"放拔",0);
			delayxms(500);
			beep(3,50,50);
			debugstring("防拔:");
			debugdata(stuInitPurchaseRet.cSnq, 2, 1);
			debugdata(sndbuf, 9, 1);
#endif
			deal2or3(sndbuf);
			revLen = RfPassthrough(revbuf, 2, sndbuf, 9);
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
			//			return ST_ERROR;
			goto CPUMAC2deal0;
		}

	}
	a_sumR = value;	
#ifdef _debug_CPU_
	debugstring("a_sumR:");
	debugdata((unsigned char*)&a_sumR, 4, 1);
#endif
	
#ifdef NO_CPU_PSAM_
	memset(G24PsamNo, 0, 6);
	Get_SerialNumF4(G24PsamNo+2);
#ifdef _debug_CPU_
	debugstring("G24 PsamNo:");
	debugdata(G24PsamNo, 6, 1);
#endif
#endif


#ifndef BUS_MZT
	memcpy(sndbuf, "\x80\xca\x00\x00\x09", 5);
#ifdef _debug_CPU_
	debugstring("取安全认证码:");
	debugdata(sndbuf, 5, 1);
#endif
	ret = RfPassthrough(revbuf, 2, sndbuf, 5);//获取用户卡安全认证识别码
	
	if(ret == 0)
		return ST_ERROR;
	if(memcmp(revbuf+ret-2, "\x90\x00", 2)==0){
		//		memcpy(Safe_Code,revbuf+1,9);
	}
#ifdef _debug_CPU_
	debugdata((unsigned char*)&ret, 1, 1);
	debugdata(revbuf, ret, 1);
	//	debugstring("Safe_Code:");
	// 	debugdata(Safe_Code,9,1);
#endif
	
	memcpy(sndbuf, revbuf+1, 9);
	if(CpuCardCheck(PSAM_YD, revbuf, sndbuf, 9)==ST_ERROR)//PSAM 卡验证安全认证码别码,新加
		return ST_ERROR;
#endif	//#ifdef BUS_MZT







//8050 0101 0B||密钥标识符 [1]（0x01）||交易金额[4](0x00000001)||终端机编号[6]
	memcpy(sndbuf, "\x02\x80\x50\x01\x02\x0B", 6); i = 6;//11 INITIALIZE FOR PURCHASE命令
	sndbuf[i++] = keyIndex;//0x01;
	
//	cardMessage.card_catalog = CARD_NORMAL; CPU卡中有卡类
	if(mode== MONEY_CARD){//
		s_sum1 = get_s_sum1(0);//取得消费金额
		if((s_sum1 == 0) || (s_sum1 > 9900))
			return 22;

	}else if (mode == FREE_CARD)
	{
		s_sum1 = 0;
	}else
	{
		s_sum1=1;
	}

// #ifdef _debug_
// s_sum1 = 0;
// #endif
	value = s_sum1;
	over_turn(4, (unsigned char*)&value);
	memcpy(sndbuf+i, (unsigned char*)&value, 4); i+=4;
	memcpy(sndbuf+i, PsamNo, 6); i += 6;

//	sndbuf[i++] = 0x0f;
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
	debugdata((unsigned char*)&stuInitPurchaseRet, sizeof(stuInitPurchaseRet), 1);
//	debugstring("MAC1计算指令:");
#endif
	memcpy(stuSamInitPurchase.cRand, stuInitPurchaseRet.cRand, 4);
	memcpy(stuSamInitPurchase.cSnq, stuInitPurchaseRet.cSnq, 2);
	memcpy(stuSamInitPurchase.cMoney, (unsigned char*)&value, 4);
	
	memcpy((unsigned char*)&a_sum1, stuInitPurchaseRet.cBalance, 4);//余额
	over_turn(4, (unsigned char*)&a_sum1);
	if (a_sum1 >= 5000000)
	{
		return 21;
	}
	gucSoudnorm = 0;

	if((a_sum1>=s_sum1)&&(a_sum1<500)){
		gucSoudnorm = 1;
	}
	stuSamInitPurchase.cType = 0x06;
	
	memcpy(stuSamInitPurchase.cDateTime, (unsigned char*)&SysTime, 7);


#ifdef _debug_CPU_
	debugstring("MAC1计算指令:");
	debugdata((unsigned char*)&stuSamInitPurchase,(28), 1);//+12
#endif

	stuSamInitPurchase.cKeyVer = stuInitPurchaseRet.cKeyVer;//0x05
	stuSamInitPurchase.cKeyEn = stuInitPurchaseRet.cKeyEn;


	memcpy(stuSamInitPurchase.cKeyFactor, cardMessage.PublishBicker+2, 8);//有28个数据
//	memcpy(stuSamInitPurchase.cPublisherSign, cardMessage.CityCode, 2);//城市代码
//	memcpy(stuSamInitPurchase.cPublisherSign+2,"\xFF\x00\x00\x00\x00\x00", 6);//有36个数据
//	memcpy(stuSamInitPurchase.cPublisherSign, stuPubInfZaoZuang.cPublisherSign, 8);//这个...

	
	



	//52B58B730005000000010620110402154217 05000000000090002030
	//4ACF30AF 0005 00000001 06 20110402154422 01000000000090002030 
	memset(revbuf, 0xff, 50);
	memset((unsigned char*)&stuDebitPurchase, 0, sizeof(stuDebitPurchase));



	if(CpuCardSetMac(PSAM_GJ, revbuf, (unsigned char*)&stuSamInitPurchase, 28) == ST_ERROR)
	{
		//		DeactiveCard(PSAM_YD);
		return 6;
	}
	
#ifdef _debug_CPU_
	debugstring("MAC OK!:");
	debugdata(revbuf, 8, 1);
#endif
	memcpy(stuDebitPurchase.cTermSnq, revbuf, 4);
	memcpy(stuDebitPurchase.cMac, revbuf+4, 4);
	memcpy(stuDebitPurchase.cDateTime, (unsigned char*)&SysTime, 7);
	memcpy(g24PSAMDsno, revbuf, 4);//--4字节的终端脱机交易序号  和 MAC1

		memcpy(sndbuf, "\x03\x80\x54\x01\x00\x0F", 6); i = 6;

	memcpy(sndbuf+i, (unsigned char*)&stuDebitPurchase, 0x0f); i+=0x0f;
	memset(revbuf, 0xff, 50);
	deal2or3(sndbuf);
#ifdef _debug_CPU_
	debugstring("chek Mac1:");
	debugdata(sndbuf, i, 1);
#endif
//	delayxms(2);
	ret = RfPassthrough(revbuf, 2, sndbuf, i);
	if(ret == 0){
		
			debugstring("没有返回\r\n");
//		debugdata(revbuf, 20, 1);
//		debugdata(revbuf, 20, 1);
// #ifndef BUS_PINGXIANG_
// #ifdef AUDIO_SOUND_OLD
// 		audio_old(Audio_TRY_AGAIN);		//请重刷
// #else
// 		audio(Audio_TRY_AGAIN);		//请重刷
// #endif
// #endif 
		if(mode == MONTH_CARD)
			cardSound = 0xbb;//出错，需要重刷卡
		else
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
		if(mode == MONTH_CARD)
			cardSound = 0xbb;//出错，需要重刷卡
		else
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
	a_sum1 -= s_sum1;//应该放在这里
CPUMAC2deal0:

#ifndef NO_CPU_PSAM_
	CPUMAC2(PSAM_YD, revbuf, sndbuf, 4);
#endif	
CPUMAC2deal:
	memcpy(cardMessage.PbocTac, TACC, 4);	// TAC码
#ifdef _debug_CPU_
	debugstring("计算MAC2:");
	debugdata(revbuf, 10, 1);
#endif

	memset(revbuf, 0, 70);
	memcpy((unsigned char*)&dis_sum2,(unsigned char*)&a_sum1,4);
// 	if(mode==MONEY_CARD){
// 		BuildRecorde(ID_REC_TOLL, revbuf);
// 		money_msg(ID_REC_TOLL, a_sum1, s_sum1, 2);
// 	}
// 	else{
// 		BuildRecorde(ID_REC_MON, revbuf);
// 		money_msg(ID_REC_MON, a_sum1, s_sum1, 2);//月票
// 	}

	BuildRecorde(ID_REC_TOLL, revbuf);
	money_msg(ID_REC_TOLL, a_sum1, s_sum1, 2);

	Save_delayCardinfo(0);	

	WriteRecord(revbuf, 0);
	Sound_cityCMD(0);

	card_ser = 0;
	cardSound = 0;	
	gMCardCand=0;
		
#ifdef _debug_CPU_
	debugstring("-------\r\n");
	debugstring("消费成功:\r\n");
	if(mode==MONTH_CARD)
		sprintf((char*)revbuf, "余次:%d次\r\n", (a_sum1));
	else
		sprintf((char*)revbuf, "余额:%d.%02d元\r\n", (a_sum1/100), (a_sum1%100));

	debugstring((char*)revbuf);
	if(mode==MONTH_CARD)
		sprintf((char*)revbuf, "扣次:%d次\r\n", s_sum1);
	else
		sprintf((char*)revbuf, "扣款:%d.%02d元\r\n", (s_sum1/100), (s_sum1%100));
	debugstring((char*)revbuf);
	debugstring("-------\r\n");
#endif
	return ST_OK;//消费成功
}


void end_close_card(unsigned char moded)
{
	unsigned int i;
	unsigned int sno1;
	unsigned char buffer[10];
//	unsigned char key;
	
	sno1 = c_serial;
	i = 0;
	do {
		if(moded == 1){
			PcdRfReset(50);
			delayxms(5);
		}
		i++;
		if (GetTypeAUID(buffer)!=0) 
		{
			if (memcmp((unsigned char *)&sno1,buffer,4) ==0) 
			{
				i=0;
			}
		}
// 		key = getkey(0);
// 		if((key == KEY_ESC)||(key == KEYP_ESC)||(key == 0x0b)){
// 			break;
// 		}
	} while(i<5);
	MifareHalt();
	return;
}

unsigned char CpuBlackMac(unsigned char PsamID, unsigned char *pMac, unsigned char *pData, unsigned char len)
{
	ISO7816_ADPU_SEND Scmd;
	ISO7816_ADPU_RESPONSE Rcmd;
	
	Scmd.CLA = 0x80;
	Scmd.INS = 0xFA;
	
	Scmd.P1 = 0x05;//0x01;//
	
	Scmd.P2 = 0x00;
	Scmd.LC = len;
	
	//memset(Scmd.DATA, 0, 0x24);
	//Scmd.DATA[10] = 0x05;
	//Scmd.DATA[18] = 0x01;
	
	memcpy(Scmd.DATA, pData, len);
	
	Scmd.LE = 0x00;
#ifdef _debug_
	debugstring("CpuCardSetMac scmd:");
	debugdata(pData, len, 1);
#endif
	IccCommand(PsamID, &Scmd, &Rcmd);
#ifdef _debug_
	debugstring("CpuCardSetMac rcmd:");
	debugdata((unsigned char*)&Rcmd, 10, 1);
	debugdata((unsigned char*)&Rcmd.SW1, 2, 1);
#endif
	if(memcmp((unsigned char*)&Rcmd.SW1, "\x90\x00", 2)!=0)// *(unsigned int*)&Rcmd.SW1 != 0x9000 )
	{
		memcpy(pMac, &Rcmd.SW1, 2);
		return ST_ERROR;
	}
	memcpy(pMac, Rcmd.DATA, Rcmd.LE);
	return ST_OK;
}
unsigned char CpuCardSetMac2(unsigned char PsamID, unsigned char *pMac, unsigned char *pData, unsigned char len)
{
	ISO7816_ADPU_SEND Scmd;
	ISO7816_ADPU_RESPONSE Rcmd;
	
	Scmd.CLA = 0x80;
	Scmd.INS = 0x1A;
	
//	Scmd.P1 = 0x28;//0x01;//
//	Scmd.P2 = 0x0C;
	Scmd.P1 = 0x25;		//绵州通
	Scmd.P2 = 0x01;		//绵州通
	Scmd.LC = len;
	
	//memset(Scmd.DATA, 0, 0x24);
	//Scmd.DATA[10] = 0x05;
	//Scmd.DATA[18] = 0x01;
	
	memcpy(Scmd.DATA, pData, len);
	
	Scmd.LE = 0x00;
#ifdef _debug_
	debugstring("MAC=scmd:");
	debugdata(&Scmd.CLA, len+5, 1);
#endif
	IccCommand(PsamID, &Scmd, &Rcmd);
#ifdef _debug_
	debugstring("CpuCardSetMac rcmd:");
	debugdata((unsigned char*)&Rcmd, 10, 1);
	debugdata((unsigned char*)&Rcmd.SW1, 2, 1);
#endif
	if(memcmp((unsigned char*)&Rcmd.SW1, "\x90\x00", 2)!=0)// *(unsigned int*)&Rcmd.SW1 != 0x9000 )
	{
		memcpy(pMac, &Rcmd.SW1, 2);
		return ST_ERROR;
	}
	memcpy(pMac, Rcmd.DATA, Rcmd.LE);
	return ST_OK;
}


//extern void Mac(unsigned char *Key,unsigned char *pInData1,unsigned char *pInData2,int data2len,unsigned char *initData, unsigned char *mac);
unsigned char  operat_Black_MZT(void)
{
	
	unsigned char i;
	unsigned char ret;
	unsigned char revbuf[256];
	unsigned char sndbuf[56];
	unsigned char Rader[10];
#ifdef NO_CPU_PSAM_
	unsigned char CPUPKEY[16];
	unsigned char snddat[64];
#endif

//	unsigned char CPUinitdat[8];


// 	memcpy(sndbuf, "\x02\x00\xA4\x00\x00\x02\x3F\x01",8);//有此卡需要用文件名，所以取不到时需要处理一下。
// 	deal2or3(sndbuf);
// 	ret = RfPassthrough(revbuf, 2, sndbuf, 8);
// 	if(ret == 0)
// 		return ST_ERROR;

	memcpy(sndbuf, "\02\x00\x84\x00\x00\x04", 6); i = 6;
	deal2or3(sndbuf);	
	ret = RfPassthrough(revbuf, 2, sndbuf, i);
#ifdef _debug_
	debugstring("随机数:");
	debugdata(revbuf, ret, 1);
#endif
	if(ret == 0)
		return 4;
	else{
		if(memcmp(revbuf+1, "\x94\x01", 2)==0)
			return 3;
		else if(memcmp(revbuf+ret-2, "\x90\x00", 2)!=0)
			return 5;
	}
	memset(Rader,0,10);
	memcpy(Rader,revbuf+1,4);
#ifdef NO_CPU_PSAM_
	if(getMiKey() != ST_OK)	//验证卡的认证码
		return 8;
	//----------计算KEY----------
	memcpy(CPUPKEY, cardMessage.MoneyKEYA, 6);
	memcpy(CPUPKEY+6, cardMessage.RecodeKEYA, 6);
	memcpy(CPUPKEY+12, cardMessage.MonthKEYA, 4);

	memcpy(snddat,"\x84\x1E\x00\x00\x04",5);
#ifdef _debug_
	debugstring("sndat1:\r\n");
	debugdata(sndbuf,20,1);
	debugstring("sndat2:\r\n");
	debugdata(snddat,16,1);
	debugstring("key:\r\n");
	debugdata(CPUPKEY,16,1);
	debugstring("CPUinitdat:\r\n");
//	debugdata(CPUinitdat,8,1);
#endif
	memset(revbuf,0,20);
//	Mac(CPUPKEY, Rader, snddat, 5, CPUinitdat, revbuf);//1BE5F163F50843699E120D74FAF08E76 消费
	//CountMac(CPUPKEY, Rader, snddat, 5,revbuf);
#else//$ NO_CPU_PSAM_  修水用
	i =8;
	memcpy(sndbuf, cardMessage.PublishBicker+2,8);
	//memcpy(sndbuf+i,cardMessage.CityCode,2);i+=2;
	//memcpy(sndbuf+i,"\xFF\x00\x00\x00\x00\x00",6);i+=6;
		
#ifdef _debug_CPU_
	debugstring("PSAM_MAC初始化指令:");
	debugdata((unsigned char *)&i,4,1);
	debugdata(sndbuf,i,1);
#endif
	if (CpuCardSetMac2(PSAM_GJ,revbuf,sndbuf,i) == ST_ERROR){	//801A450110
		return ST_ERROR;
	}

	memcpy(sndbuf,Rader,4);i=4;//密钥
	memcpy(sndbuf+i,"\x00\x00\x00\x00\x84\x1E\x00\x00\x04\x80\x00\x00",12);i +=12;
	
	if(CpuBlackMac(PSAM_GJ,revbuf,sndbuf,i)==ST_ERROR)			//80FA050010
		return ST_ERROR;
#endif
#ifdef _debug_
	debugstring("MAC1:");
	debugdata(revbuf,10,1);
#endif	
	//if(CpuBlackMac(gPSAMin,revbuf,sndbuf,i)==ST_ERROR)
	//	return ST_ERROR;
	//CountMAC();

	memcpy(sndbuf, "\02\x84\x1e\x00\x00\x04", 6); i = 6;
	memcpy(sndbuf+i,revbuf,4);i+=4;//MAC
	deal2or3(sndbuf);
	
	ret = RfPassthrough(revbuf, 2, sndbuf, i);
#ifdef _debug_
	debugstring("锁定:");
	debugdata(&ret,1,1);
	debugdata(revbuf, ret, 1);
#endif
	if(ret == 0)
		return 4;
	else{
		if(memcmp(revbuf+1, "\x94\x01", 2)==0)
			return 3;
		else if(memcmp(revbuf+ret-2, "\x90\x00", 2)!=0)
			return 5;
	}


// 	memcpy(sndbuf, "\x02\x00\xA4\x04\x00\x09\xA0\x00\x00\x00\x03\x86\x98\x07\x01", 15); // 选择3F01
// 	deal2or3(sndbuf);
// #ifdef _debug_CPU_
// 	debugstring("select AID1:");
// 	debugdata(sndbuf, 15, 1);
// #endif
// 	ret = RfPassthrough(revbuf, 2, sndbuf, 15);
// #ifdef _debug_CPU_
// 	debugdata(revbuf, ret, 1);
// #endif
// 	if(ret == 0)
// 		return ST_ERROR;
	
	BuildRecorde(0x50, revbuf);
	WriteRecord(revbuf, 0);
	return ST_OK;
}
extern void ControlMagane(void);
extern unsigned char  operat_Black(void);
extern void DisRetry(void);
void CPUMainCard(void)
{
	unsigned char ret=0,ret_mzt=0;//,ret_JTB=0;
	unsigned char ret_pboc=0;
	char disbuf[20];
	
	gMCardCand= CARDSTYLE_QPBOC;


#ifdef BUS_MZT
	second_money_flag = 0;
#endif


	
	BlightOpen();

#ifdef _debug_CPU_
	debugstring("询卡成功:CSNO:");
	debugdata((unsigned char*)&c_serial, 4, 1);
	debugdata((unsigned char*)&gPsamErrorFlag,1,1);
#endif


	ret = CPUcardType();
	ret_mzt = ret;


	switch(ret )
	{
	case APP_LOCK:
	case NO_CITY_UNION:
		ret=CPUcardType_JTB(0);
		//ret_JTB = ret;
		break;
	default:
		break;

	}
	

#ifdef _debug_CPU_
	debugstring("ret:");
	debugdata((unsigned char*)&ret, 1, 1);
#endif

	if(gchn_eng == 'E'){
		sprintf(disbuf, "code:%02d ", ret);
	}
	else{
		sprintf(disbuf, "错误:%02d ", ret);
	}
	switch(ret)
	{
	case ST_OK:
		g24GDisFlash = 3;
		restore_flag = 0;
		SetTcpPacketTTLTime(RECORD_TTL);
		set_delay_send(set_delay_send_val); //延时找任务
		end_close_card(1);//这个不起作用。
		return;

	case CARD_STOPWORK:		
		error(ERR_CARD_STOP_WORK,0);
		break;

	case APP_LOCK:	
	case NO_JTB_UNION:

		MSG_LOG("走银联流程试试\r\n");
//		ret_pboc = 
	cpuPBOCmain();
//		if ((ret_pboc == ST_ERROR)&&(ret_mzt == NO_CITY_UNION))
//		{
//			ret = APP_LOCK;	//
//			break;
//		}

		break;
		
	case CARD_NO_SYSTEM:	
		error(ERR_CARD_NO_SYSTEM,0);
		break;
	case CARD_WHITE_BUS:				//白卡
		error(ERR_CARD_WHITE_BUS,0);
		break;
	case CARD_NO_USE:
		error(ERR_CARD_NO_USE,0);
		break;

	case MONTH_CARD:
// 		ret = CPU_month_deal();//公司月票
// #ifdef _debug_CPU_
// 		debugstring("ret:");
// 		debugdata((unsigned char*)&ret, 1, 1);
// #endif
// 		if((ret == 3)||(ret == 4))//余额不足  
// 			ret = CPUDealCard(MONEY_CARD, 3);//扣钱

		break;
	case FREE_CARD:
	case MONEY_CARD:

		if(gMCardCand== CARDSTYLE_JTB)
			ret=CPUDealCard_JTB(ret,0);
		else
			ret = CPUDealCard(ret, 0);

		break;
		
//		goto CPUMEnd;

	case NO_WriteNUM:
		MessageBox(1, "城市未在白名单!");
		led_on(LED_RED);
		restore_flag=3;
		goto CPUMEnd;

	case CARD_forbid:
		led_off(ALL_LED);
		delayxms(5);
		MessageBox(1, "此卡禁止刷卡!");
		led_on(LED_RED);
		SoundMessage(SOUND_FLING_MONEY);//请投币
		restore_flag=3;
		goto CPUMEnd;
// 	case CARD_STOPWORK:
// 		led_off(ALL_LED);
// 		delayxms(5);
// 		led_on(LED_RED);
// 		restore_flag=3;
// 		goto CPUMEnd;
	case 9:
	case 8:
		cls();
		display(0, 0, disbuf, 0);
		if(gchn_eng == 'E'){
			display(3,0,"illegal card",DIS_CENTER);
		}
		else
			display(3, 0, "非法卡", DIS_CENTER);
		SoundMessage(INVALID_CARD);
		goto CPUMEnd;
	case CARD_NO_TIME:
		cls();
		display(0, 0, disbuf, 0);
		if(gchn_eng == 'E'){
			display(3,0,"out date card",DIS_CENTER);
		}
		else{
			display(3, 0, "卡过期或没启用", DIS_CENTER);
		}
		memset(disbuf,0,sizeof(disbuf));
		strcpy(disbuf,"过期日:");
		BCD2Ascii(YouHui_Date,disbuf+7,4);
		display(5, 0, disbuf, DIS_CENTER);
		restore_flag = 3;

		SoundMessage(SOUND_FLING_MONEY);//请投币

		goto CPUMEnd;
	case CARD_BLACK_PRO:
		cls();
		display(0,0,"警告：黑名单卡!",0);
		display(2,4,"请投币!!",0);
		SoundMessage(INVALID_CARD);
		restore_flag=3;
		if (gMCardCand== CARDSTYLE_CPU)
		{
			operat_Black_MZT();
		}else 
		{
			operat_Black();
		}


		goto CPUMEnd;

	
	case CONTROL_CARD:
		debugstring("contro");
		ControlMagane();
		goto CPUMEnd;
	case ST_ERROR:
		return;
	default:
		return;
	}


	

	

	if(gchn_eng == 'E'){
		sprintf(disbuf, "code:%02d ", ret);
	}
	else{
		sprintf(disbuf, "错误:%02d ", ret);
	}
	switch (ret)
	{
	case ST_ERROR:
		return;

	case CARD_STOPWORK:		
		error(ERR_CARD_STOP_WORK,0);
		break;
	case APP_LOCK:		
		cls();
		display(0,0,"警告：应用锁定!",0);
		display(2,4,"请投币!!",0);
		SoundMessage(INVALID_CARD);
		restore_flag=3;
		break;


	case CARD_BLACK_PRO:
		cls();
		display(0,0,"警告：黑名单卡!",0);
		display(2,4,"请投币!!",0);
		SoundMessage(INVALID_CARD);
		restore_flag=3;
		end_close_card(1);//这个不起作用。
		break;
	case 21:
	case RET_MONEY_SN_EROR:																//两个钱包都错误
		error(ERR_MONEY_SN, 0);
		end_close_card(1);//这个不起作用。
		break;
	case 22:
		if(gchn_eng == 'E'){
			display(0,0,"hint:", DIS_Cls);
			display(2,0,"pos price error",DIS_CENTER);
		}
		else{
			display(0,0,"提示:", DIS_Cls);
			display(2,0,"票价信息不正确",DIS_CENTER);
		}
		end_close_card(1);//这个不起作用。
		restore_flag = 3;
		break;
	case CARD_NO_TIME:
		cls();
		display(0, 0, disbuf, 0);
		if(gchn_eng == 'E'){
			display(3,0,"out date card",DIS_CENTER);
		}
		else{
			display(3, 0, "卡过期或没启用", DIS_CENTER);
		}
		end_close_card(1);//这个不起作用。
		break;
	case 3:
		cls();
		display(0, 0, disbuf, 0);
		if(gchn_eng == 'E'){
			display(3,0,"NO SAVE ACCOUNT",DIS_CENTER);
		}
		else{
			display(3, 0, "余额不足", DIS_CENTER);
		}
		SoundMessage(SOUND_FLING_MONEY);
		end_close_card(1);//这个不起作用。
		break;
	case 4:
		cls();
		display(0, 0, disbuf, 0);
		display(3, 0, "扣次不能连刷", DIS_CENTER);
		SoundMessage(SOUND_FLING_MONEY);
		end_close_card(1);//这个不起作用。
		break;
	case 6:
	case 7:
		cls();
		display(0, 0, disbuf, 0);
		if(gchn_eng == 'E'){
			display(3,0,"MAC1 eror",DIS_CENTER);
		}
		else{
			//display(3, 0, "MAC1计算错误", DIS_CENTER);
			ERR_DIS("EP ERRO A0",&ret);
		}
//		CPUPsamInitialize();

		break;
	case 9:
	case 8:
		cls();
		display(0, 0, disbuf, 0);
		if(gchn_eng == 'E'){
			display(3,0,"illegal card",DIS_CENTER);
		}
		else
			display(3, 0, "非法卡", DIS_CENTER);
		SoundMessage(INVALID_CARD);
		end_close_card(1);//这个不起作用。
		break;
	case ST_OK://成功
		restore_flag = 3;
		SetTcpPacketTTLTime(RECORD_TTL);
		break;
	default:
// 		cls();
// 		display(0, 0, disbuf, 0);
// 		display(3, 0, "末定义错误", DIS_CENTER);
		break;
	}
CPUMEnd:
	end_close_card(1);
	g24GDisFlash = 2;
//	delayxs(3);
	restore_flag = 0;
}
#endif // #ifdef BUS_CPU_
