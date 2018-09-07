
#include <stdlib.h>
#include "string.h"
#include "stdio.h"
#include "..\include\include.h"
#include "szct.h"
#include "Mydes.h"
#include "SL8583.h"
#include "qpboc_8583.h"
#include "Gszct.h"
#include "ProcCharacter.h"

#include "Fatfiles.h"
#include "Font.h"

#include "qpboc_head.h"
#include "qPBOC.h"
#include "MD5.h"
//#include "libsm.h"
#include "..\include\iso8583.h"
unsigned char g_supportQR = qr_aliPay | qr_weiXin | qr_unPay;

#if 0	// use for test valid ram

unsigned char ACK_flag = 0xFF;	//后台应答39域的值  bcd
unsigned int POS_8583_COUNT = 0;	//受卡方系统跟踪号 在8583 11域
unsigned char temp[1000];

unsigned char getMobileParameter(unsigned char mode, unsigned char *obuf)
{
	temp[0] = 0;
	return 0;
}
void dis_qboc_ack_code(unsigned char ack)
{
}
int SQDataFromSVT(unsigned char SQmode, int msecends)
{
	return 0;
}

#else

// 1.2 通讯命令解释
typedef enum {
	mpc_stx = 0,
	mpc_ln = 1,
	mpc_cmd = 2,
	mpc_st = mpc_cmd,
	mpc_data = 3,
	mpc_bcc,	// 无效
	mpc_etx,
}emMPosCom;

typedef enum {
	mpsc_ok = 0x04,
	mpsc_param = 0x15,
	mpsc_timeout = 0x80,
	mpsc_invalid = 0xE0,
}emMPosStCode;

typedef struct {
	unsigned int odaRecordBegin;
	unsigned int odaRecordEnd;
	unsigned int odaRecordWrite;
	unsigned int odaRecordRead;
	unsigned int odaRecordEarase;
}stOdaRecordAddr;

#define RECORD_FALG	"\x55\xAA"

#if PBOC_HTTPS
extern void ssl_read_cmd(void);
#endif
void build_YZFQRC_Record(unsigned char delType, unsigned char *orecBuf, unsigned char *iQRCdat, int qrLen)
{
;
}

//组织二维码记录（公司以外）
extern void build_YZFQRC_Record(unsigned char delType, unsigned char *orecBuf, unsigned char *iQRCdat, int qrLen);

stMobilStyle Sign_Infor;
unsigned char qr_pboc_AccountNo[32] = { 0 };//二维码 的账户 ascii
volatile unsigned char ACK_flag = 0xFF;	//后台应答39域的值  bcd
unsigned char  ACK_flag_oda = 0xFF;
unsigned int POS_8583_COUNT = 0;	//受卡方系统跟踪号 在8583 11域

data_element_flag msgf[129];	//128个域的分解情况.

unsigned char pakege_8583[1024];	//这个先存储起来，配合msgf 用
unsigned char repurse_infor[512];		//冲正时候用
unsigned char pboc_free_cnt = 200;		//刷二维码或者银行卡，置为一个数，再自减为0 时候，自己关闭银行的连接，怕有时候银联关闭后，再open出错，所以自己关闭

//unsigned char *pboc8583bitmapaddr=NULL;//8583位图到地址指针

unsigned char *gpboc_8583bitmapaddr;//8583位图到地址指针

extern unsigned char gTcpIpbusydly;
extern stPricetable NewPriceTab;
extern unsigned char PsamNo[6];
extern unsigned char restore_flag;
extern unsigned char gmissflag;//任务标志 0x00 登陆移动,签到.
extern stcomtime SysTime;
extern pFistVary_1 pFistVary;
extern stBusVerIfo gBusVerInfo;
extern unsigned char GprsCardList[16];
extern stBusTicketV gBusTicketV;
extern unsigned char GPRSLinkProcess;
extern unsigned int gSendGLogin;//登陆命令
extern unsigned short gErrorFlag;//错误标识,高字节表示错误编号，低字节表示错次数据 详见"*错误编码标识定义"
extern unsigned int gErrortimes[MAX_RCV_PACKET_NUM];//错误次数据,主要是用于GPRS连接
extern Parameter2 POINT_MESSAGE;
extern unsigned int a_sum1, s_sum1, a_sumR;
extern unsigned short gSendOverTime;//发送后收应答超时计数器
extern unsigned char gMCardCand;
#define GPRSCOMMAX 1600
extern unsigned char gprsRevBuf[GPRSCOMMAX];//GPRS接收缓冲
extern volatile unsigned short gprsIndex;
extern unsigned char gprsErrorTimes;//出现ＥＲＲＯＲ返回错误次数
extern PACKET_RCV gPacketRcvBuf[MAX_RCV_PACKET_NUM];	//接收帧缓冲
extern volatile stGPrsDataStyle gGPRS_data_style;
extern unsigned char gTcpIpbusy;//模块是正正在发数据，模块工作是发命令，然后回应后再发数据。在等回应时不能发其它数据。
extern unsigned int c_serial;
extern unsigned char isNetOK[MAX_RCV_PACKET_NUM];//两路链接是否成功;
extern Parameter5 cardMessage;



extern unsigned char pakege[1024];
extern unsigned int Build_qpboc_8583_55(unsigned char *dat, unsigned char isSetbitMap);
extern void SetUSART1mode(unsigned char mode);
//从接收缓冲中获得一个有效的数据帧 OK
extern unsigned char PeekFrame(void);
extern char *ASC2BCD(char *strASC, int lenASC);
extern void delallBlacInbus(void);
extern unsigned char GetDateTime(void);
extern void getbusticketVer(void);
extern void getProVer(void);
extern unsigned char SYSgetbussinessNO(unsigned char *dat);
extern void sysSaveDevice(unsigned char *dbuf);
extern void getCpuInfo(stCpuInfo *inP);
extern void saveCpuInfo(unsigned char mode, unsigned char *dat);
extern void savesysbupara(void);
extern void gprs_send_data(unsigned char linkNum, unsigned int len, unsigned char *dat);
extern void SoundMessage(unsigned char cmd);
extern void money_msg(unsigned char dmode, unsigned int remM, unsigned int pucM, unsigned char cMOde);
extern unsigned char SetupClientIP(unsigned char mode);
extern void dis_messgebox(char *istr, int code, unsigned char row);

extern void save_normal(void);
extern void save_h_month(void);
extern unsigned char month_decide(void);
extern void Save_delayCardinfo(unsigned char Cmode);
extern unsigned char getMiKey(void);
extern unsigned char Delay_Normal(void);
extern void disp_no_swipe(void);
extern void SetTcpPacketTTLTime(unsigned char val);
extern unsigned char WriteRecord(unsigned char *buffer, unsigned char mode);
extern unsigned int get_s_sum1(unsigned char mode);
extern void tcpipClose(unsigned char mode);

unsigned char s_isAuthOk = 1;
unsigned char s_isDownOdaBlkList = 0;
QPBOC_TYPE_63 TYPE_63;
unsigned char  CountPointRead(void);
void CountPoint(unsigned char mode);
void BLK_63_int_first(void)
{
	memset((unsigned char *)&TYPE_63, 0x30, sizeof(QPBOC_TYPE_63));
	memcpy((unsigned char *)TYPE_63.Organization_, "0000FSGJ", 8);
	fewrite(BIS_BLK_63, sizeof(QPBOC_TYPE_63), (unsigned char *)&TYPE_63);
	MSG_LOG("63初始化=1\r\n");
	BCD_LOG((unsigned char *)&TYPE_63, sizeof(QPBOC_TYPE_63), 1);
};
void BLK_63_int(void)
{
	feread(BIS_BLK_63, sizeof(QPBOC_TYPE_63), (unsigned char *)&TYPE_63);
	MSG_LOG("63初始化=22\r\n");
	BCD_LOG((unsigned char *)&TYPE_63, sizeof(QPBOC_TYPE_63), 1);
}


void mzt_qpoc_init(void)
{
	switch_both=0;
	shuangmian=1;
}
//超过10,不再登录
typedef struct
{
	unsigned char POBC_time;		//
	unsigned char POBC_time_flag;		//
	unsigned char res[10];		//
}QPOC_FLGE;
QPOC_FLGE qpoc_flag={0};
void qpoc_init_singe(void)
{
	MSG_LOG("清次数-\r\n");
	memset((unsigned char *)&qpoc_flag,0,sizeof(QPOC_FLGE));
}
void add_qpoc_flag(void)
{
	MSG_LOG("次数加-\r\n");
	qpoc_flag.POBC_time++;
		restore_flag=3;
}
unsigned char qpoc_flag_or(void)
{
	if(qpoc_flag.POBC_time>=10) //登录超过10次，就不再登录银联
	{

		if ((gmissflag&MISS_PBOC_LOGIN)==MISS_PBOC_LOGIN)  //银联超时重新签到
		{
				restore_flag=3;
				gmissflag=0x4f;
				debugstring("超过10次\r\n");
				GPRSLinkProcess= TCPSTARTSTAT;
				
		}
		Sign_Infor.ISOK =0;
		return 0xad;
	}

return ST_OK;
	
}

void show_err(void)
{
	if(qpoc_flag.POBC_time>=20)
	{
	//	cls();
	//	display(3,3,"请检查银联参数!",DIS_CENTER);
		MessageBox(1, "请检查银联参数!");
		beep(3,100,100);
	//	delayxms(3);
	//			restore_flag=3;
	}

}

//////
//4G
// extern void ALL4G_SISREAD(unsigned char link);

void cls_repurse(void)
{
	unsigned char i;
	unsigned char buff[64];

	
	cls();
	display(0,0,"清除冲正",DIS_CENTER|DIS_CONVERT);

	display(6,0,Card_HitSure, 0);
	while(1){
		i = getkey(1);
		if(i == KEY_ENT)
			break;
		else if(i == KEY_ESC)
			return;
		delayxms(50);
	}
	save_repurse_infor(FeRC_Dlelt, NULL);
	cls();
	display(3,3,"清除冲正成功!",DIS_CENTER);
	delayxms(1500);
}
//广佛通322上面移植过来的
extern int calFileCRC(unsigned int fileName, unsigned int fileLen);




//前补0
extern void INT2BCD(unsigned int ii, unsigned char *bcdbuf, unsigned char bcdbuflen);


//根据输入的域位设置位图
extern void setBitmapBits(unsigned char fieldID, unsigned char *sl8583bitmap);
//查看对应的域是否存在,0存在
//extern unsigned char getBitmapBits(unsigned char fieldID, unsigned char *sl8583bitmap);

extern unsigned char StufferNO[4];
extern unsigned char DriveCardNo[4];

/* 贵州银联提供
无线POS：
移动VPN： yl.gz
联通vpn： gzyl.gz.apn
IP：9.234.21.113
9.234.21.114
端口：1025


电话线拨号：
联通：0851-8545254
4006471002
电信：0851-5377144
4008961001


tpdu:6000050000(多渠道)
	 6000070000（收单平台）
*/



//#define		OPERATOR_ID "10 "	//操作员代码，后面有个空格？ 
// int Build8583_64(unsigned char *iDat, unsigned int iDatLen, unsigned char *oDat)
// {
// 	unsigned int cc;
// 	unsigned char radom[8];
// 	unsigned char omac[8];
// 
// 	setBitmapBits(64, gpboc_8583bitmapaddr);	//因为MAC计算加上了位元表，所以要先把第64域的位加上，再计算MAC
// 
// 	cc = __cpuCrc32(gsl8583Style.MAC_KEY, 16);
// 	if(cc == gsl8583Style.crc_mackey){//有MAC KEY，签到时下发。所每次开机都会签到，所以不需要保存在铁电中 
// 		memset(radom, 0, 8);
// 		//	CountMac1(gMobileParameter.KEK+(gMobileParameter.KEK_SN*16), dat, len, omac);
// 		CountMac1(gsl8583Style.MAC_KEY, radom, iDat, iDatLen, omac);
// 		memcpy(oDat, omac, 4);	//取前4个字节
// 	}
// 	else{
// 		memset(oDat, 0xFF, 4);
// 	}
// 	return 4;
// }


//根据输入的域位设置位图
extern void setBitmapBits(unsigned char fieldID, unsigned char *sl8583bitmap);
//查看对应的域是否存在,0存在
extern unsigned char getBitmapBits(unsigned char fieldID, unsigned char *sl8583bitmap);

/*
//根据输入的域位设置位图
void setBitmapBits(unsigned char fieldID, unsigned char *sl8583bitmap)
{
	unsigned char fbyte,fbit;

	if(fieldID == 0)
		return;
	fbyte = ((fieldID-1)/8);
	fbit = ((fieldID-1)%8);

	sl8583bitmap[fbyte] |= (0x80>>fbit);

	// 	MSG_LOG("bitmap_%02d:", fieldID);
	// 	BCD_LOG(sl8583bitmap, 16, 1);
}
//查看对应的域是否存在,0存在
unsigned char getBitmapBits(unsigned char fieldID, unsigned char *sl8583bitmap)
{
	unsigned char fbyte,fbit;

	if(fieldID <= 1)
		return ST_OK;
	fbyte = ((fieldID-1)/8);
	fbit = ((fieldID-1)%8);

	// 	cv = (0x80>>fbit);
	// 	MSG_LOG("ID:%d,fbyte:%02d, fbit:%d, value:%02X\r\n", fieldID, fbyte, fbit, sl8583bitmap[fbyte]);
	// 	MSG_LOG("cv:%02X\r\n", cv);
	// 	BCD_LOG(sl8583bitmap, 16, 1);

	if((sl8583bitmap[fbyte] & ((0x80>>fbit))) != 0){
		return ST_OK;
	}
	else{
		return ST_ERROR;
	}
}

*/

void InitQpboc8583(void) {
	getMobileParameter(8, &g_supportQR);
}


//填入报文头
int Build_qpboc_8583_HD(unsigned char *Odat, int headType)
{
	unsigned int pos = 0;

	Ascii2BCD(TPDU, Odat, sizeof(TPDU));
	pos += sizeof(TPDU) / 2;		//60 0632 0000  TPDU

	Ascii2BCD(HEAD, Odat + pos, sizeof(HEAD));
	pos += sizeof(HEAD) / 2;

	MSG_LOG("%s:len:%d dat:", __FUNCTION__, pos);
	BCD_LOG(Odat, pos, 1);

	return pos;
}

//2  主账号  n..19  LLVAR  BCD  C  
int Build_qpboc_8583_02(unsigned char *dat)
{
	unsigned int pos = 0;
	BER_TVL TempTVL;
	// 	unsigned char rcardMainNO[10];
	unsigned char temp[50];
	char *p = NULL;
	unsigned char cardNolen = 0;

	pos = 1;//前1个字节是BCD码的LL



	if (gMCardCand == CARDSTYLE_QR_PBOC)
	{
		cardNolen = strlen((const char *)qr_pboc_AccountNo);
		//MSG_LOG("qr卡帐号:%s\r\n",qr_pboc_AccountNo);
		memset(temp, 0, sizeof(temp));
		strcpy((char *)temp, (const char *)qr_pboc_AccountNo);

	}
	else {
		get_BER_TVL(QpbocTVLFCI.t_57.SizeOff, &TempTVL);
		// 	memset(rcardMainNO, 0xFF, 10);
		// 
		MSG_LOG("卡帐号:");
		BCD_LOG(TempTVL.V, TempTVL.L, 1);
		memset(temp, 0, sizeof(temp));
		BCD2Ascii(TempTVL.V, temp, TempTVL.L);
		MSG_LOG("temp:%s\r\n", temp);

		p = strstr((const char *)temp, "D");

		if (p != NULL)
		{
			cardNolen = (unsigned char)(p - (char *)temp);
		}
		else {
			cardNolen = sizeof(temp);
		}



	}

	// 	MSG_LOG("cardNolen:%d :%s\r\n",cardNolen,temp);
	INT2BCD(cardNolen, dat, 1);//1字节长度的长度

	if (cardNolen % 2)
	{
		temp[cardNolen] = '0';		//后面填充0 6223670100004522768 --> 62236701000045227680
		cardNolen++;

	}

	temp[cardNolen] = 0;
	Ascii2BCD(temp, dat + pos, cardNolen);





	setBitmapBits(2, gpboc_8583bitmapaddr);
	// 	pos = (pos+1)/2;
	pos = (cardNolen / 2) + 1;



	MSG_LOG("%s:len:%d dat:", __FUNCTION__, pos);
	BCD_LOG(dat, pos, 1);


	return pos;

}


void deleteAllChar(char str[], char c)
{
	int i, j;
	for (i = j = 0; str[i] != '\0'; i++)
	{
		if (str[i] != c)/**如果找到符合的则后续的循环把其后的字符前移*/
			str[j++] = str[i];
	}
	str[j] = '\0';
}

void replace_AllChar(char str[], char src_c, char des_c)
{
	int i;//,j;  
	for (i = 0; str[i] != '\0'; i++)
	{
		if (str[i] == src_c)/**如果找到符合的替换*/
		{
			str[i] = des_c;
		}
	}
	//	str[j]='\0';  
}


//二磁必须出现。  llv n数字
unsigned int Build_qpboc_8583_35(unsigned char *dat)
{
	unsigned int pos;
	//	stMobileParameter smpPara;
	//	unsigned char tempbuf2[50]="\0";
	BER_TVL TempTVL;
	//char *p=NULL;
	unsigned char cardNolen = 0;
	unsigned char temp[50];
	unsigned char bcdlen = 0;
	//	unsigned char error_card=0;

	pos = 1;//前1个字节是BCD码的LL




	get_BER_TVL(QpbocTVLFCI.t_57.SizeOff, &TempTVL);

	MSG_LOG("2磁道数据:");
	BCD_LOG(TempTVL.V, TempTVL.L, 1);


	memset(temp, 0, 50);
	BCD2Ascii(TempTVL.V, temp, TempTVL.L);
	MSG_LOG("temp:%s\r\n", temp);

	deleteAllChar((char *)temp, 'F');//去掉F
	MSG_LOG("temp:%s\r\n", temp);

	cardNolen = strlen((char *)temp);
	bcdlen = strlen((const char *)temp);

	if (cardNolen > 37)
	{
		MSG_LOG("二磁道不能>37");
		//error_card = 1;
		bcdlen = 37;			//强制去掉1位，华夏银行的卡这个有问题

	}


	MSG_LOG("cardNolen:%d\r\n", cardNolen);

	if (cardNolen % 2)
	{
		MSG_LOG("补0\r\n");
		temp[cardNolen] = '0';
		cardNolen++;
	}



	Ascii2BCD(temp, dat + pos, cardNolen);




	pos += cardNolen / 2;



	// 	replace_AllChar((char *)temp,'D','=');	//替换D 为=
	// 	MSG_LOG("temp:%s\r\n",temp);








	//	MSG_LOG("bcdlen:%d,:%s\r\n",bcdlen,temp);





	pos -= 1;	//数据部分的长度。

	INT2BCD(bcdlen, dat, 1);//1字节长度的长度

	pos += 1;//刚减掉了LL，现在加上长度。

	setBitmapBits(35, gpboc_8583bitmapaddr);
	MSG_LOG("%s:len:%d dat:", __FUNCTION__, pos);
	BCD_LOG(dat, pos, 1);
	return pos;
}


//3域：交易处理码000000 默认
unsigned int Build_qpboc_8583_03(unsigned char *dat)
{
	memcpy(dat, "\x00\x00\x00", 3);
	setBitmapBits(3, gpboc_8583bitmapaddr);
	MSG_LOG("%s:len:%d dat:", __FUNCTION__, 3);
	BCD_LOG(dat, 3, 1);
	return 3;

}
//4  交易金额  N12    BCD  M  M  终端可输入最大金额 99999999.99
unsigned int Build_qpboc_8583_04(unsigned char *dat)
{
	unsigned char Buffer[16];//,Buff[16];
	BER_TVL TempTVL;
	int pos = 0;

	//	memset(Buff,0,20);

	if (gMCardCand != CARDSTYLE_QR_PBOC)
	{

		get_BER_TVL(QpbocTVLFCI.t_9F02.SizeOff, &TempTVL);
		// 		MSG_LOG("%04X:授权金额:",QpbocTVLFCI.t_9F02.TagValue);
		// 		BCD_LOG(TempTVL.V,TempTVL.L,1);

		memcpy(dat + pos, TempTVL.V, TempTVL.L);
		pos += TempTVL.L;
	}

	else
	{

		memset(Buffer, 0, sizeof(Buffer));
		sprintf((char *)Buffer, "%012d", s_sum1);
		Ascii2BCD(Buffer, dat + pos, 12);

		pos += 6;

	}



	setBitmapBits(4, gpboc_8583bitmapaddr);
	MSG_LOG("%s:len:%d dat:", __FUNCTION__, pos);
	BCD_LOG(dat, pos, 1);
	return pos;

}

unsigned int get_deal_count(unsigned int addr)
{

	//	char buffer[32];
	unsigned int uiTemp = 0;
	//	stTradeBatchinfo oinfo;
	// 	CountPointRead();
	// 	
	// 	if ((POINT_MESSAGE.MoneyDealPointer==0)|| POINT_MESSAGE.MoneyDealPointer>99999)
	// 	{
	// 		POINT_MESSAGE.MoneyDealPointer = 1;
	// 		CountPoint(0);
	// 	}
	//银联用的11域交易流水4字节 BIT_PBOC_NS 这个有时候不对，会被覆盖，或者就是自增没有存储上，
//	feread(BIT_PBOC_NS, 4, (unsigned char*)&uiTemp);
	feread(addr, 4, (unsigned char*)&uiTemp);
	MSG_LOG("get_deal_count%d:%d\n", addr, uiTemp);
	if ((uiTemp < 300000 || uiTemp > 999999) && addr != BIT_PBOC_NS_BIG) {
		//MSG_LOG("get_deal_count%d:%d\n", addr, uiTemp);
		uiTemp = 300000 + SysTime.seconds + SysTime.minutes * 60;
	}

	//MSG_LOG("%s:addr:%d,%d\r\n", __FUNCTION__, addr, uiTemp);

	if (addr == BIT_PBOC_NS_8583) {
		MSG_LOG("取流水号:%d\n", uiTemp);
	}
	else if (addr == BIT_PBOC_NS_BIG) {
		MSG_LOG("取批次号:%d\n", uiTemp);
	}
	else {
		MSG_LOG("取未知号:%d\n", uiTemp);
	}

	return uiTemp;

}

unsigned int save_deal_count_add(unsigned int addr)
{

	//	char buffer[32];
	unsigned int uiTemp = 0;

	uiTemp = get_deal_count(addr);
	uiTemp++;
	MSG_LOG("save_deal_count_add%d:%d\n", addr, uiTemp);
	if (uiTemp > 999999)
	{
		uiTemp = 300000 + SysTime.seconds + SysTime.minutes * 60;
	}
	//银联用的11域交易流水4字节 BIT_PBOC_NS 这个有时候不对，会被覆盖，或者就是自增没有存储上，
//	sysfewrite(BIT_PBOC_NS, 4, (unsigned char*)&uiTemp);
	sysfewrite(addr, 4, (unsigned char*)&uiTemp);
	//MSG_LOG("%s:addr:%d,%d\r\n", __FUNCTION__, BIT_PBOC_NS, uiTemp);

	if (addr == BIT_PBOC_NS_8583) {
		MSG_LOG("增加流水号:%d\n", uiTemp);
	}
	else if (addr == BIT_PBOC_NS_BIG) {
		MSG_LOG("增加批次号:%d\n", uiTemp);
	}
	else {
		MSG_LOG("增加未知号:%d\n", uiTemp);
	}

	return ST_OK;

}

//11  受卡方系统跟踪号  n6    BCD  M  M  POS 终端交易流水
unsigned int Build_qpboc_8583_11(unsigned char *dat)
{
	//	unsigned char temp[4];
	char buffer[32];
	unsigned int uiTemp = 0;
	//	stTradeBatchinfo oinfo;
	// 	CountPointRead();
	// 	
	// 	if ((POINT_MESSAGE.MoneyDealPointer==0)|| POINT_MESSAGE.MoneyDealPointer>99999)
	// 	{
	// 		POINT_MESSAGE.MoneyDealPointer = 1;
	// 		CountPoint(0);
	// 	}

	uiTemp = get_deal_count(BIT_PBOC_NS_8583);
	POS_8583_COUNT = uiTemp;

	sprintf((char*)buffer, "%06ui", uiTemp);

	memcpy(dat, ASC2BCD((char*)buffer, 6), 3);


	setBitmapBits(11, gpboc_8583bitmapaddr);
	MSG_LOG("%s:len:%d dat:", __FUNCTION__, 3);
	BCD_LOG(dat, 3, 1);
	return 3;

}
//9.1.3.6.	BM #12收单方终端所在地时间
extern unsigned int Build8583_12(unsigned char *dat);
//9.1.3.7.	BM #13收单方终端所在地日期MMDD
unsigned int Build_qpboc_8583_13(unsigned char *dat)
{
	dat[0] = SysTime.month;
	dat[1] = SysTime.year_l;

	setBitmapBits(13, gpboc_8583bitmapaddr);
	MSG_LOG("%s:len:%d dat:", __FUNCTION__, 2);
	BCD_LOG(dat, 2, 1);
	return 2;

}


//二维码的话为qrcardd==1
unsigned int Build_qpboc_8583_22(unsigned char *dat)
{
#if 1
	if (gMCardCand == CARDSTYLE_QR_PBOC)
	{
		memcpy(dat, "\x03\x20", 2);	//POS终端不输入密码，扫码付22域取值为032。
	}
	else
#endif
	{
		memcpy(dat, "\x07\x20", 2);		//快速 PBOC借/贷记IC卡读入（非接触式）交易中不包含PIN

	}

	MSG_LOG("%s:len:%d dat:", __FUNCTION__, 2);
	BCD_LOG(dat, 2, 1);
	setBitmapBits(22, gpboc_8583bitmapaddr);
	return 2;

}
//23	卡片序列号	n3		BCD	C	C	C：当POS能够获得该值时存在；
unsigned char Build_qpboc_8583_23(unsigned char *dat)
{

	BER_TVL TempTVL;
	unsigned char rcardseial[2];


	get_BER_TVL(QpbocTVLFCI.t_5F34.SizeOff, &TempTVL);
	 	MSG_LOG("55.3:%04X:PIN序号:",QpbocTVLFCI.t_5F34.TagValue);
		BCD_LOG(TempTVL.V,TempTVL.L,1);

	memset(rcardseial, 0, 2);
	if (TempTVL.L == 1)
		rcardseial[1] = TempTVL.V[0];//卡片序列号
	else
		memcpy(rcardseial, TempTVL.V, 2);//卡片序列号

	memcpy(dat, rcardseial, 2);		//


	MSG_LOG("%s:len:%d dat:", __FUNCTION__, 2);
	BCD_LOG(dat, 2, 1);
	setBitmapBits(23, gpboc_8583bitmapaddr);
	return 2;

}


//25  服务点条件码  N2    BCD  M  M
unsigned int Build_qpboc_8583_25(unsigned char *dat)
{
	// 	if (gMCardCand == CARDSTYLE_QR_PBOC)
	// 	{
	// 		memcpy(dat, "\x03", 1);		//03  二维码
	// 
	// 	}else{

	memcpy(dat, "\x00", 1);		//00  正常提交
//	}

	MSG_LOG("%s:len:%d dat:", __FUNCTION__, 1);
	BCD_LOG(dat, 1, 1);
	setBitmapBits(25, gpboc_8583bitmapaddr);
	return 1;

}

//26 服务点 PIN 获取码 n2 BCD C 06
unsigned int Build_qpboc_8583_26(unsigned char *dat)
{
	// 	if (gMCardCand == CARDSTYLE_QR_PBOC)
	return 1;
}


// 32  受理方标识码  n..11  LLVAR  BCD    M 
unsigned int Build_qpboc_8583_32(unsigned char *dat)
{
	unsigned int pos;
	//	int i=0;

	pos = 1;//前1个字节是BCD码的LL



	pos -= 1;	//数据部分的长度。

	INT2BCD(pos, dat, 1);//1字节长度的长度

	pos += 1;//刚减掉了LL，现在加上长度。

	// 	MSG_LOG("bit50:%d\r\n",pos);
	// 	BCD_LOG(dat, pos, 1);
	setBitmapBits(32, gpboc_8583bitmapaddr);
	MSG_LOG("%s:len:%d dat:", __FUNCTION__, pos);
	BCD_LOG(dat, pos, 1);
	return pos;
}


unsigned int Build_qpboc_8583_39(unsigned char *dat)
{
	memcpy(dat, "00", 2);
	setBitmapBits(39, gpboc_8583bitmapaddr);
	dat[2] = 0;
	MSG_LOG("%s:len:%d dat:%s(ASCII)\r\n", __FUNCTION__, 2, dat);
	return 2;
}


// 41  受卡机终端标识码  ans8    ASCII  M  M  终端代码 
unsigned int Build_qpboc_8583_41(unsigned char *dat)
{

	getMobileParameter(6, dat);
	setBitmapBits(41, gpboc_8583bitmapaddr);
	dat[8] = 0;
	MSG_LOG("%s:len:%d dat:%s(ASCII)\r\n", __FUNCTION__, 8, dat);
	//	BCD_LOG(dat,8,1);
	return 8;
}
//42  受卡方标识码  ans15    ASCII  M  M  商户代码
unsigned int Build_qpboc_8583_42(unsigned char *dat)
{
	//商户号	终端号	秘钥明文
	//898520154110004	52010001	1FF8BC2F0E3DE3DC547F49A18F49D57A

//	stMobileParameter smpPara;

	//memcpy(dat, BUSS_ID, sizeof(BUSS_ID));
	getMobileParameter(1, dat);


	setBitmapBits(42, gpboc_8583bitmapaddr);
	dat[15] = 0;
	MSG_LOG("%s:len:%d dat:%s(ASCII)\r\n", __FUNCTION__, 15, dat);
	return 15;

}
//48	行业特定信息	ans...300	LLLVAR	ASCII	M	M	详见48域用法说明
/*用法标识	Ans	2	M	“PA”
产品代码（大类+细类）	Ans	4	M	“5709”
所属地区代码	Ans	4	M	“0000”
属地附加代码	Ans	4	M	“0001”*/
unsigned int Build_qpboc_8583_48(unsigned char *dat)
{
	int len = 0;
	unsigned char tmpChs[20];
	int tmpI = 0;


	len = 2;
	memcpy(dat + len, "PA", 2);
	len += 2;
	memcpy(dat + len, "5709", 4);
	len += 4;
	memcpy(dat + len, "0000", 4);
	len += 4;
	memcpy(dat + len, "0001", 4);
	len += 4;

	memcpy(dat + len, "\x1F\x51\x02", 3);
	len += 3;
	memcpy(dat + len, "24", 2);
	len += 2;

	memcpy(dat + len, "\x1F\x52\x02", 3);
	len += 3;
	if (gMCardCand == CARDSTYLE_QPBOC) {
		memcpy(dat + len, "03", 2);
		len += 2;
	}
	else if (gMCardCand == CARDSTYLE_QR_PBOC) {
		memcpy(dat + len, "02", 2);
		len += 2;
	}
	else {	// ODA
		memcpy(dat + len, "01", 2);
		len += 2;
	}
	if (gMCardCand == CARDSTYLE_UNPAY_ODA) {
		memcpy(dat + len, "\xFF\x56", 2);
		len += 2;

		tmpI = Build_qpboc_8583_55(dat + len, 0);

		dat[len] = 0x81;
		dat[len + 1] = tmpI - 2;
		//dat[len - 1] = tmpI;
		len += tmpI;
	}


	memcpy(dat + len, "\xFF\x57\x01\x31", 4);
	len += 4;
#if 1

	memcpy(dat + len, "\xFF\x58\x0E", 3);
	len += 3;
	BCD2Ascii((unsigned char *)&SysTime, dat + len, 7);
	len += 14;
#if 0
	memcpy(dat + len, "\xFF\x59\x01\x31", 4);
	len += 4;
	memcpy(dat + len, "\xFF\x60\x0E", 3);
	len += 3;
	BCD2Ascii((unsigned char *)&SysTime, dat + len, 7);
	len += 14;
#endif
	memcpy(dat + len, "\xFF\x61\x07", 3);
	len += 3;
	memcpy(dat + len, "SLZR-CS", 7);
	len += 7;
	memcpy(dat + len, "\xFF\x62\x06", 3);
	len += 3;
	BCD2Ascii(pFistVary.LineNo, dat + len, 3);
	len += 6;
	memcpy(dat + len, "\xFF\x63\x08", 3);
	len += 3;
	sysferead(BIT_car_num, BIT_car_num_LEN, tmpChs);
	BCD2Ascii(tmpChs, dat + len, 4);
	len += 8;
	memcpy(dat + len, "\xFF\x64\x08", 3);
	len += 3;
	BCD2Ascii(DriveCardNo, dat + len, 4);
	len += 8;
#endif
	if (gMCardCand == CARDSTYLE_UNPAY_ODA) {
		memcpy(dat + len, "\xFF\x43\x08", 3);
		len += 3;
		memcpy(dat + len, "0000FSGJ", 8);
		len += 8;
	}
	dat[len] = '#';
	//memcpy(dat + len, "#", 1);
	++len;

	INT2BCD(len - 2, dat, 2);//2字节长度的长度  
	dat[len] = 0;

	setBitmapBits(48, gpboc_8583bitmapaddr);
	MSG_LOG("%s:len:%d dat:%s(ASCII)\r\n", __FUNCTION__, len, dat + 2);
	MSG_LOG("dat:");
	BCD_LOG(dat, len, 1);
	return len;

}
//49 交易货币代码  an3    ASCII  M  M 
unsigned int Build_qpboc_8583_49(unsigned char *dat)
{
	memcpy(dat, "156", 3);

	setBitmapBits(49, gpboc_8583bitmapaddr);
	dat[3] = 0;
	MSG_LOG("%s:len:%d dat:%s(ASCII)\r\n", __FUNCTION__, 3, dat);
	return 3;

}
//n16，16个字节的定长数字字符域
unsigned int Build_qpboc_8583_53(unsigned char *dat)
{
	//PIN加密方法   0：PIN不出现，未知
// 	1：ANSI X9.8 Format（不带主账号信息）
// 		2：ANSI X9.8 Format（带主账号信息）
// 		加密算法标志   0：单倍长密钥算法
// 		6：双倍长密钥算法
// 		磁道加密标志   0：不加密
// 		1：加密

	Ascii2BCD("0600000000000000", dat, 8);

	setBitmapBits(53, gpboc_8583bitmapaddr);
	dat[8] = 0;
	MSG_LOG("%s:len:%d dat:\r\n", __FUNCTION__, 8);
	BCD_LOG(dat, 8, 1);

	return 8;

}

/*
S_DE55 POS_CON_de55[25] =
{
	"9F33", "95", "9F1E", "9F10", "9F26", "9F36",
		"82", "9C", "9F1A", "9A", "9F02", "5F2A", "9F03", "9F35", "9F34","9F37",
		"9F27","9F41","9F63",""
};*/

/*
9F33   - Terminal Capabilities
	   Length = 03
	   Value  = E0 E1 C8
	   Byte 1 Bit 8 : Manual Key Entry
	   Byte 1 Bit 7 : Magnetic Stripe
	   Byte 1 Bit 6 : IC With Contacts
	   Byte 2 Bit 8 : Plaintext PIN for Offline ICC Verification
	   Byte 2 Bit 7 : Enciphered PIN for Online Verification
	   Byte 2 Bit 6 : Signature (paper)
	   Byte 2 Bit 1 : Cardholder Certificate Verification - PBOC
	   Byte 3 Bit 8 : Offline Static Data Authentication(SDA)
	   Byte 3 Bit 7 : Offline Dynamic Data Authentication(DDA)
	   Byte 3 Bit 4 : Combined DDA/AC Generation(CDA)
95     - Terminal Verification Results (TVR)
	   Length = 05
	   Value  = 00 00 00 00 00
9F1E   - Interface Device (IFD) Serial Number
	   Length = 08
	   Value  = 35 34 30 30 34 33 31 38
	   Inter  = 54004318
9F10   - Issuer Application Data (IAD)
	   Length = 13
	   Value  = 07 01 17 03 A0 00 00 01 0A 01 00 00 04 99 93 86 5E F2 20
	   ----Below is based on PBOC 3.0 Book 5 P82 & Book 12 Appendix D----
	   Length of Cryptogram Info & CVR : 07
	   Derivation Key Index(DKI) : 01
	   Cryptogram Version Number : 17
	   --------------------------------------
	   Byte 1 : 03 - Length of Card Verification Results(CVR)
	   Byte 2 Bit 8-7 : 10 - Not Requested : Second Cryptogram Type
	   Byte 2 Bit 6-5 : 10 - ARQC : First Cryptogram Type
	   Byte 4 Bit 8-5 : 0000 - Number of Issuer Script Commands Processed on Last Oline Transaction
	   --------------------------------------
	   Algorithm Identifier : 01 - RSA
	   Issuer Defined Data (IDD)
	   0A 01 00 00 04 99 93 86 5E F2 20
	   Length : 0A
	   ID : 01
	   EC Balance : 00 00 04 99 93
	   MAC : 86 5E F2 20
	   ----Below is based on VSDC Field 134 - Visa Discretionary Data----
	   Length of Cryptogram Info & CVR : 07
	   Derivation Key Index(DKI) : 01
	   Cryptogram Version Number : 17
	   --------------------------------------
	   Byte 1 : 03 - Length of Card Verification Results(CVR)
	   Byte 2 Bit 8-7 : 10 - Not Requested : Second Cryptogram Type
	   Byte 2 Bit 6-5 : 10 - ARQC : First Cryptogram Type
	   Byte 4 Bit 8-5 : 0000 - Number of Issuer Script Commands Processed on Last Oline Transaction
	   --------------------------------------
	   RFU Bytes : 01 0A 01 00 00 04 99 93 86 5E F2 20
9F26   - Application Cryptogram (AC)-(AAC/ARQC/TC)
	   Length = 08
	   Value  = F2 07 A5 8A 91 AB 83 83
9F36   - Application Transaction Counter (ATC)
	   Length = 02
	   Value  = 00 67
82     - Application Interchange Profile (AIP)
	   Length = 02
	   Value  = 7C 00
	   Byte 1 Bit 7 : SDA supported
	   Byte 1 Bit 6 : DDA supported
	   Byte 1 Bit 5 : Cardholder verification is supported
	   Byte 1 Bit 4 : Terminal risk management is to be performed
	   Byte 1 Bit 3 : Issuer authentication is supported
9C     - Transaction Type
	   Length = 01
	   Value  = 00
	   Inter  = Goods/Service
9F1A   - Terminal Country Code
	   Length = 02
	   Value  = 01 56
9A     - Transaction Date
	   Length = 03
	   Value  = 17 09 22
9F02   - Amount, Authorised (Numeric)
	   Length = 06
	   Value  = 00 00 00 00 00 01
5F2A   - Transaction Currency Code
	   Length = 02
	   Value  = 01 56
9F03   - Amount, Other (Numeric)
	   Length = 06
	   Value  = 00 00 00 00 00 00
9F35   - Terminal Type
	   Length = 01
	   Value  = 22
	   Inter  = Attended, offline with online capability, operated by merchant
9F37   - Unpredictable Number
	   Length = 04
	   Value  = 63 A5 E8 1D
9F27   - Cryptogram Information Data (CID)
	   Length = 01
	   Value  = 80
	   Bit 8-7 : 10 - ARQC
	   Bit 6-5 : 00 - Payment System-specific cryptogram
	   Bit 4   : 0 - No advice required
	   Bit 3-1 : 000 - No information given
9F41   - Transaction Sequence Counter
	   Length = 04
	   Value  = 00 00 00 89
*/
//IC 卡数据域 最大 255 字节数据LLLVAR包含多个子域
unsigned int Build_qpboc_8583_55(unsigned char *dat, unsigned char isSetbitMap)
{
	unsigned int pos;
	BER_TVL TempTVL;
	//int i=0;
//	char disbuff[16];


	if (gMCardCand == CARDSTYLE_QR_PBOC)
	{
		//		MSG_LOG("二维码没有55域\r\n");
		return 0;
	}

#if 0
	//百富的一个报文
	Ascii2BCD("01249F3303E0E1C8950500000000009F1E0835343030343331389F101307011703A00000010A010000049993865EF2209F2608F207A58A91AB83839F3602006782027C009C01009F1A0201569A031709229F02060000000000015F2A0201569F03060000000000009F3501229F370463A5E81D9F2701809F410400000089"\
		, dat, (124 + 2) * 2);
	pos = 124 + 2;
#endif

#if 1

	pos = 2;//前2个字节是BCD码的LLL

	//  9F35 01 22
	// 	9F27 01 80
	//  9F41 04 00000086


	MSG_LOG("55.14:9F33:终端性能:");
	memcpy(dat + pos, "\x9F\x33\x03", 3);
	pos += 3;
	memcpy(dat + pos, INFO_9F33, 3);
	BCD_LOG(dat + pos, 3, 1);
	pos += 3;

	MSG_LOG("55.6:95 05 0000000000:验证结果\r\n");
	memcpy(dat + pos, "\x95\x05\x00\x00\x00\x00\x00", 7);
	pos += 7;


	MSG_LOG("55.15:9F1E:设备序列号:");
	memcpy(dat + pos, "\x9F\x1E\x08", 3);
	pos += 3;
	//	memcpy(dat+pos, pFistVary.DeviceNo,8);
	memcpy(dat + pos, INFO_9F1E, 8);
	BCD_LOG(dat + pos, 8, 1);
	pos += 8;


	//9F10（tag）	发卡行应用数据	b…256	VAR	BINARY	M
	get_BER_TVL(QpbocTVLFCI.t_9F10.SizeOff, &TempTVL);
	MSG_LOG("55.3:%04X:发卡行应用数据:", QpbocTVLFCI.t_9F10.TagValue);
	BCD_LOG(TempTVL.V, TempTVL.L, 1);
	memcpy(dat + pos, &QpbocTVLFCI.t_9F10.TagValue, 2);
	over_turn(2, dat + pos);
	pos += 2;
	dat[pos++] = TempTVL.L;
	memcpy(dat + pos, TempTVL.V, TempTVL.L);
	pos += TempTVL.L;



	//应用密文
	get_BER_TVL(QpbocTVLFCI.t_9F26.SizeOff, &TempTVL);
	MSG_LOG("55.1:%04X:密文:", QpbocTVLFCI.t_9F26.TagValue);
	BCD_LOG(TempTVL.V, TempTVL.L, 1);
	memcpy(dat + pos, &QpbocTVLFCI.t_9F26.TagValue, 2);
	over_turn(2, dat + pos);
	pos += 2;
	dat[pos++] = TempTVL.L;
	memcpy(dat + pos, TempTVL.V, TempTVL.L);
	pos += TempTVL.L;

	get_BER_TVL(QpbocTVLFCI.t_9F36.SizeOff, &TempTVL);
	MSG_LOG("55.5:%04X:交易计数器:", QpbocTVLFCI.t_9F36.TagValue);
	BCD_LOG(TempTVL.V, TempTVL.L, 1);
	memcpy(dat + pos, &QpbocTVLFCI.t_9F36.TagValue, 2);
	over_turn(2, dat + pos);
	pos += 2;
	dat[pos++] = TempTVL.L;
	memcpy(dat + pos, TempTVL.V, TempTVL.L);
	pos += TempTVL.L;


	get_BER_TVL(QpbocTVLFCI.t_82.SizeOff, &TempTVL);
	MSG_LOG("55.11:%02X:应用交互特征:", QpbocTVLFCI.t_82.TagValue);
	BCD_LOG(TempTVL.V, TempTVL.L, 1);
	memcpy(dat + pos, &QpbocTVLFCI.t_82.TagValue, 1);
	pos++;
	dat[pos++] = TempTVL.L;
	memcpy(dat + pos, TempTVL.V, TempTVL.L);
	pos += TempTVL.L;


	if (QpbocTVLFCI.t_9C.TagValue != 0)
	{
		get_BER_TVL(QpbocTVLFCI.t_9C.SizeOff, &TempTVL);
		MSG_LOG("55.8:%02X:类型:", QpbocTVLFCI.t_9C.TagValue);
		BCD_LOG(TempTVL.V, TempTVL.L, 1);
		memcpy(dat + pos, &QpbocTVLFCI.t_9C.TagValue, 1);
		pos++;
		dat[pos++] = TempTVL.L;
		memcpy(dat + pos, TempTVL.V, TempTVL.L);
		pos += TempTVL.L;
	}
	else
	{
		memcpy(dat + pos, "\x9C\x01\x00", 3);
		pos += 3;
		MSG_LOG("类型:");
		BCD_LOG(dat + pos - 3, 3, 1);


	}



	MSG_LOG("55.12:9F1A 02 0156:终端国家代码\r\n");
	memcpy(dat + pos, "\x9F\x1A\x02\x01\x56", 5);
	pos += 5;


	// 	if (QpbocTVLFCI.t_9A.TagValue !=0)
	// 	{
	// 
	// 		get_BER_TVL(QpbocTVLFCI.t_9A.SizeOff,&TempTVL);
	// 		MSG_LOG("55.7:%02X:日期:",QpbocTVLFCI.t_9A.TagValue);
	// 		BCD_LOG(TempTVL.V,TempTVL.L,1);
	// 		memcpy(dat+pos,&QpbocTVLFCI.t_9A.TagValue,1);		
	// 		pos ++;
	// 		dat[pos++] = TempTVL.L;
	// 		memcpy(dat+pos,TempTVL.V,TempTVL.L);
	// 		pos += TempTVL.L;
	// 	}else
	// 	{
	memcpy(dat + pos, "\x9A\x03", 2);
	pos += 2;
	memcpy(dat + pos, &SysTime.year_l, 3);
	pos += 3;
	MSG_LOG("EE日期:");
	BCD_LOG(dat + pos - 5, 5, 1);
	// 	}

	get_BER_TVL(QpbocTVLFCI.t_9F02.SizeOff, &TempTVL);
	MSG_LOG("55.9:%04X:授权金额:", QpbocTVLFCI.t_9F02.TagValue);
	BCD_LOG(TempTVL.V, TempTVL.L, 1);
	memcpy(dat + pos, &QpbocTVLFCI.t_9F02.TagValue, 2);
	over_turn(2, dat + pos);
	pos += 2;
	dat[pos++] = TempTVL.L;
	memcpy(dat + pos, TempTVL.V, TempTVL.L);
	pos += TempTVL.L;

	get_BER_TVL(QpbocTVLFCI.t_5F2A.SizeOff, &TempTVL);
	MSG_LOG("55.10:%04X:货币代码:", QpbocTVLFCI.t_5F2A.TagValue);
	BCD_LOG(TempTVL.V, TempTVL.L, 1);
	memcpy(dat + pos, &QpbocTVLFCI.t_5F2A.TagValue, 2);
	over_turn(2, dat + pos);
	pos += 2;
	dat[pos++] = TempTVL.L;
	memcpy(dat + pos, TempTVL.V, TempTVL.L);
	pos += TempTVL.L;


	MSG_LOG("55.10:9F03 06 00000000000000:其他金额\r\n");
	memcpy(dat + pos, "\x9F\x03\x06\x00\x00\x00\x00\x00\x00", 9);
	pos += 9;

	//  	MSG_LOG("写死终端类型9F35:?\r\n");//终端类型	n2		BCD	O1	
	memcpy(dat + pos, "\x9F\x35\x01\x22", 4);
	pos += 4;



	get_BER_TVL(QpbocTVLFCI.t_9F37.SizeOff, &TempTVL);
	MSG_LOG("55.4:%04X:随机数:", QpbocTVLFCI.t_9F37.TagValue);
	BCD_LOG(TempTVL.V, TempTVL.L, 1);
	memcpy(dat + pos, &QpbocTVLFCI.t_9F37.TagValue, 2);
	over_turn(2, dat + pos);
	pos += 2;
	dat[pos++] = TempTVL.L;
	memcpy(dat + pos, TempTVL.V, TempTVL.L);
	pos += TempTVL.L;

	/*
		对于联机消费，卡片一共吐出两次应用密文，第一次是ARQC（授权请求密文，表示请求联机），第二次是TC（交易批准）或者AAC（交易拒绝）
	对于脱机消费，卡片只吐出一次应用密文，就是TC（交易批准）或者AAC（交易拒绝）

	  1、对于联机消费
	  请求阶段：F55的9F26存放的是ARQC；
	  应答阶段：卡片承兑，吐出TC，终端拿到TC就打单。
	*/

	//	MSG_LOG("写死:密文信息数据??\r\n");//应用信息数据	b8		BINARY	M1	
	memcpy(dat + pos, "\x9F\x27\x01\x80", 4);	//Cryptogram Information Data (CID)	 
	MSG_LOG("9F27 01 80\r\n");
	pos += 4;

#if 0
	memcpy(dat + pos, "\xDF\x32\x04", 3);	//Cryptogram Information Data (CID)	 
	MSG_LOG("DF32 04\r\n");
	pos += 3;
	SET_INT32(dat + pos, c_serial);
	pos += 4;


	memcpy(dat + pos, "\xDF\x33\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 0x13);	//Cryptogram Information Data (CID)	 
	MSG_LOG("DF33 10\r\n");
	pos += 0x13;

	memcpy(dat + pos, "\xDF\x34\x0E\x12\x00\x00\x34\x00\x00\x00\x56\x00\x00\x00\x00\x00\x78", 0x11);	//Cryptogram Information Data (CID)	 
	MSG_LOG("DF34 0E\r\n");
	pos += 0x11;
#endif

	memcpy(dat + pos, "\x9F\x34\x03\x00\x00\x00", 6);	//Cryptogram Information Data (CID)	 
	MSG_LOG("9F34 03 000000\r\n");
	pos += 6;
	// 	MSG_LOG("写死:?\r\n");	//交易序列计数器	n…4	VAR	BCD	O1	


	/*
		POINT_MESSAGE.MoneyDealPointer;
		POINT_MESSAGE.MoneyDealPointer ++;
		if(POINT_MESSAGE.MoneyDealPointer > 999999)
			POINT_MESSAGE.MoneyDealPointer = 0;
		CountPoint(0);
		memcpy(dat+pos,"\x9F\x41\x04",3);
		pos +=3;

		sprintf(disbuff,"%08d",POINT_MESSAGE.MoneyDealPointer);
		Ascii2BCD((unsigned char *)disbuff,dat+pos,8);
	//	memcpy(dat+pos,"\x00\x00\x00\x04",4);
		pos += 4;

		MSG_LOG("9F41 04:");
		BCD_LOG(dat+pos-7,7,1);

	*/


	pos -= 2;	//数据部分的长度。


	INT2BCD(pos, dat, 2);//2字节长度的长度

	pos += 2;//刚减掉了LLL，现在加上长度。* /

#endif

	if (isSetbitMap) {
		setBitmapBits(55, gpboc_8583bitmapaddr);
	}

	MSG_LOG("%s:len:%d dat:", __FUNCTION__, pos);
	BCD_LOG(dat, pos, 1);
	return pos;

}

unsigned int Build_qpboc_8583_57(unsigned char *dat) {
	int pos = 0;
	int tmpI = 0;
	int addfieldOff = 0;
	int bcdLen = 0;
	char buffer[10];

	pos = 2;
	memcpy(dat + pos, "PB", 2);//
	pos += 2;
	memcpy(dat + pos, "51A2", 4);//
	pos += 4;
	memcpy(dat + pos, "0000", 4);//
	pos += 4;
	memcpy(dat + pos, "0002", 4);//
	pos += 4;
	tmpI = 0;// strlen((char *)qr_pboc_AccountNo);
	memcpy(dat + pos, qr_pboc_AccountNo, tmpI);//
	pos += tmpI;
	memset(dat + pos, ' ', 50 - tmpI);//
	pos += 50 - tmpI;
	memcpy(dat + pos, "000000", 6);//
	pos += 6;

	addfieldOff = pos;
	pos += 3;

	bcdLen = 0;
#if 1
	memcpy(dat + pos, "\x1F\x51\x03", 3);//
	pos += 3;
	bcdLen += 3;
	memcpy(dat + pos, "ODA", 3);//
	pos += 3;
	bcdLen += 3;
#endif

	memcpy(dat + pos, "\xFF\x55", 2);//
	pos += 2;
	bcdLen += 2;
	tmpI = strlen((char *)qr_pboc_AccountNo);
	dat[pos] = tmpI;
	pos += 1;
	bcdLen += 1;
	memcpy(dat + pos, qr_pboc_AccountNo, tmpI);//
	pos += tmpI;
	bcdLen += tmpI;

	memcpy(dat + pos, "\xFF\x57\x10", 3);//
	pos += 3;
	bcdLen += 3;
	getMobileParameter(1, dat + pos);
	pos += 16;
	bcdLen += 16;

	memcpy(dat + pos, "\xFF\x58\x08", 3);//
	pos += 3;
	bcdLen += 3;
	getMobileParameter(6, dat + pos);
	pos += 8;
	bcdLen += 8;

	memcpy(dat + pos, "\xFF\x61\x01\x03", 4);//
	pos += 4;
	bcdLen += 4;

	memcpy(dat + pos, "\xFF\x42\x04", 3);//
	pos += 3;
	bcdLen += 3;
	memcpy(dat + pos, "FFFF", 4);//
	pos += 4;
	bcdLen += 4;

#if 1
	memcpy(dat + pos, "\xBF\x12\x1A", 3);//
	pos += 3;
	bcdLen += 3;
	memcpy(dat + pos, "11111111111111111111111111", 26);//
	pos += 26;
	bcdLen += 26;
#endif

	sprintf(buffer, "%03u", bcdLen);	//3个字节的长度 用ASC码表示
	memcpy(dat + addfieldOff, buffer, 3);

	dat[pos] = '#';
	++pos;

	pos -= 2;//前2个字节是BCD码的LLL


	INT2BCD(pos, dat, 2);//2字节长度的长度   

	pos += 2;//前2个字节是BCD码的LLL

	setBitmapBits(57, gpboc_8583bitmapaddr);
	MSG_LOG("%s:len:%d dat:ASCII:%s\r\n", __FUNCTION__, pos, dat + 2);
	MSG_LOG("dat:");
	BCD_LOG(dat, pos, 1);
	return pos;

}

//Bit59=(70)A2 065 01 002 02 02 016 0000020254004318 03 006 000052 04 008 EC7CD424 05 008 321702 
unsigned int tag_A2(unsigned char *dat)
{

	unsigned int pos = 0;
	char buffer[32];
	//	unsigned int uiTemp=0;
	unsigned char bcdlen = 0;
	unsigned char A2_pos = 0;
	unsigned short ii = 0;
	//Bit59=(70)A2 065 01 002 02 02 016 0000020254004318 03 006 000052 04 008 EC7CD424 05 008 321702 


	BER_TVL TempTVL;
	// 	unsigned char rcardMainNO[10];
	unsigned char temp[50];
	char *p = NULL;
	unsigned char cardNolen = 0;



	memcpy(dat + pos, "A2", 2);//TagA3	扫码付信息数据
	pos += 2;
	pos += 3;


	A2_pos = 0;
	memcpy(dat + pos, "0100202", 7);//设备类型	01	2字节	AN2  02：传统POS
	A2_pos += 7;
	pos += 7;


#ifdef PBOC_TEST1
	memcpy(dat + pos, "02016", 5);//终端硬件序列号	02	最长50字节	ANS…50
	A2_pos += 5;
	pos += 5;
	memcpy(dat + pos, "LF28517121111111", 16);
	A2_pos += 16;
	pos += 16;
#else
	memcpy(dat + pos, "02016", 5);//终端硬件序列号	02	最长50字节	ANS…50
	A2_pos += 5;
	pos += 5;
	memcpy(dat + pos, "EFS3000L", 8);
	A2_pos += 8;
	pos += 8;

	//Get_SerialNumF4((unsigned char *)buffer);//Get_SerialNumF4_Self(buff);//
	//BCD2Ascii((unsigned char *)buffer, dat + pos, 4);
	memcpy(dat + pos, pFistVary.DeviceNo, 8);
	A2_pos += 8;
	pos += 8;
#endif

	memcpy(dat + pos, "03006", 5);//加密随机因子取值说明：银行卡交易采用2域卡号后6位；扫码付交易采用C2B码后6位
	A2_pos += 5;
	pos += 5;

	if (gMCardCand == CARDSTYLE_QR_PBOC)
	{

		bcdlen = strlen((char *)qr_pboc_AccountNo);
		//		MSG_LOG("qr卡帐号:len:%d,%s\r\n",bcdlen,qr_pboc_AccountNo);
		memcpy(dat + pos, &qr_pboc_AccountNo[bcdlen - 6], 6);
	}
	else
	{


		get_BER_TVL(QpbocTVLFCI.t_57.SizeOff, &TempTVL);
		// 	memset(rcardMainNO, 0xFF, 10);
		// 
// 		MSG_LOG("卡帐号:");
// 		BCD_LOG(TempTVL.V,TempTVL.L,1);
		memset(temp, 0, sizeof(temp));
		BCD2Ascii(TempTVL.V, temp, TempTVL.L);
		//		MSG_LOG("kh:%s\r\n",temp);

		p = strstr((char *)temp, "D");

		if (p != NULL)
		{
			cardNolen = (unsigned char)(p - (char *)temp);
		}
		else {
			cardNolen = sizeof(temp);
		}

		memcpy(dat + pos, &temp[cardNolen - 6], 6);//银行卡交易采用2域卡号后6位；扫码付交易采用C2B码后6位

	}

	A2_pos += 6;
	pos += 6;


	memcpy(dat + pos, "04008", 5);//加密随机因子取值说明：银行卡交易采用2域卡号后6位；扫码付交易采用C2B码后6位
	A2_pos += 5;
	pos += 5;
	memcpy(dat + pos, "EC7CD424", 8);		//SAM4 算法    加密数据内容为终端硬件序列号+加密随机因子
	A2_pos += 8;
	pos += 8;


	memcpy(dat + pos, "05008", 5);//  应用程序变更应保证版本号不重复，当长度不足时，右补空格
	A2_pos += 5;
	pos += 5;
	memcpy((unsigned char*)&ii, gBusVerInfo.busProVer, 2);
	sprintf((char*)buffer, "%04X", ii);		//PFQUAN  X新的通讯平台

	memcpy(dat + pos, buffer, 4);
	memset(dat + pos + 4, ' ', 4);

	A2_pos += 8;
	pos += 8;



	MSG_LOG("a2len:%d\r\n", A2_pos);


	sprintf(buffer, "%03u", A2_pos);	//3个字节的长度 用ASC码表示
	memcpy(dat + 2, buffer, 3);		//A2 的长度


	MSG_LOG("%s:len:%d dat:ASCII:%s\r\nBCD:", __FUNCTION__, pos, dat);
	BCD_LOG(dat, pos, 1);

	return pos;



}

//TagA2	终端硬件序列号及密文数据	ans…128	VAR	ASCII	C		消费、预授权交易上送终端唯一安全编号相关信息。TagA2包含第二层TLV，详细定义参见3.1.1

//Bit59=(70)A2 065 01 002 02 02 016 0000020254004318 03 006 000052 04 008 EC7CD424 05 008 321702 
unsigned int Build_qpboc_8583_59(unsigned char *dat)
{
	unsigned int pos;
	//	stMobileParameter smpPara;
	//	unsigned char tempbuf2[50]="\0";
	//	stTradeBatchinfo oinfo;
	char buffer[32];
	unsigned int uiTemp = 0;
	unsigned char bcdlen = 0;
	//unsigned char A2_pos=0,A3_pos = 0;

	pos = 2;//前2个字节是BCD码的LLL

// 	uiTemp = tag_A2(dat + pos);
// 	pos += uiTemp;

	if (gMCardCand == CARDSTYLE_QR_PBOC) {
		memcpy(dat + pos, "A3", 2);//TagA3	扫码付信息数据
		pos += 2;
		bcdlen = strlen((const char *)qr_pboc_AccountNo);

		sprintf(buffer, "%03u", bcdlen);	//3个字节的长度 用ASC码表示
		memcpy(dat + pos, buffer, 3);
		pos += 3;
		memcpy(dat + pos, qr_pboc_AccountNo, bcdlen);

		pos += bcdlen;
		//	MSG_LOG("bcdlen:%d\r\n",bcdlen);
	}

	pos -= 2;//前2个字节是BCD码的LLL


	INT2BCD(pos, dat, 2);//2字节长度的长度   

	pos += 2;//前2个字节是BCD码的LLL

	setBitmapBits(59, gpboc_8583bitmapaddr);
	MSG_LOG("%s:len:%d dat:ASCII:%s\r\n", __FUNCTION__, pos, dat + 2);
	MSG_LOG("dat:");
	BCD_LOG(dat, pos, 1);
	return pos;
}

//60  自定义域  n..017  LLLVAR  BCD  M  M   感觉是个ASCII 发送的时候却是个bcd 晕 ！
//银联定义的变成 BCD域都是这样的，有点怪
//如0011 00000001003
//按理要写0011 的话 后面有001122334455667788990011 个字节，但是实际是如0011 00000001003
unsigned int Build_qpboc_8583_60_login(unsigned char *dat)
{
	unsigned int pos;
	//	stMobileParameter smpPara;
	//	unsigned char tempbuf2[50]="\0";
	//	stTradeBatchinfo oinfo;
	char buffer[32];
	unsigned int uiTemp = 0;
	unsigned char bcdlen = 0;

	pos = 2;//前2个字节是BCD码的LLL

	dat[pos++] = 0;	//交易类型 00
	bcdlen = 2;
	//	getMobileParameter((unsigned char * )&smpPara);

	uiTemp = get_deal_count(BIT_PBOC_NS_BIG);

	//feread(BIT_PBOC_NS_BIG, 4, (unsigned char*)&uiTemp);

	MSG_LOG("read:%d\r\n", uiTemp);

	memset(buffer, 0, sizeof(buffer));
	sprintf((char*)buffer, "%06u", uiTemp);
	uiTemp++;
	MSG_LOG("批次:%s\r\n", buffer);
	memcpy(dat + pos, ASC2BCD((char*)buffer, 6), 3);
	pos += 3;
	bcdlen += 6;


	memcpy(dat + pos, "\x00\x30", 2);// 3个字符双倍长密钥算法（含磁道密钥）终端用 004
	pos += 2;

	bcdlen += 3;




	//	MSG_LOG("bcdlen:%d\r\n",bcdlen);

	INT2BCD(bcdlen, dat, 2);//2字节长度的长度    感觉是个ASCII 发送的时候却是个bcd 晕 


	setBitmapBits(60, gpboc_8583bitmapaddr);
	MSG_LOG("%s:len:%d dat:", __FUNCTION__, pos);
	BCD_LOG(dat, pos, 1);
	return pos;
}




unsigned int Build_qpboc_8583_60_purse(unsigned char *dat)
{
	char buffer[32];
	unsigned int uiTemp = 0;
	int len;
	int startPos = 1;

	len = startPos;
	strcpy(buffer + len, "22");
	len += 2;

	uiTemp = get_deal_count(BIT_PBOC_NS_BIG);

	memset(buffer + len, 0, 7);
	sprintf((char*)buffer + len, "%06u", uiTemp);
	uiTemp++;
	MSG_LOG("批次:%s\r\n", buffer + len);
	len += 6;

	memcpy(buffer + len, "000601", 6);
	len += 6;

	len -= startPos;
	INT2BCD(len, dat, 2);//2字节长度的长度  

	Ascii2BCD((unsigned char *)buffer + startPos, dat + 2, len);
	//memcpy(dat + 2, ASC2BCD((char*)buffer + startPos, len), len >> 1);
	len >>= 1;
	setBitmapBits(60, gpboc_8583bitmapaddr);
	MSG_LOG("%s:len:%d dat:", __FUNCTION__, len);
	BCD_LOG(dat + 2, len, 1);
	return len + 2;
}

//60  自定义域  n..017  LLLVAR  BCD  M  M 
unsigned int Build_qpboc_8583_63(unsigned char *dat)
{
	unsigned int pos;
	//	stMobileParameter smpPara;
	//	unsigned char tempbuf2[50]="\0";
	//	stTradeBatchinfo *oinfo;

	pos = 2;//前2个字节是BCD码的LLL


	memcpy(dat + pos, "001", 3);//操作员代码
	pos += 3;


	pos -= 2;	//数据部分的长度。

	INT2BCD(pos, dat, 2);//2字节长度的长度

	pos += 2;//刚减掉了LLL，现在加上长度。

	setBitmapBits(63, gpboc_8583bitmapaddr);

	MSG_LOG("%s:len:%d dat:", __FUNCTION__, pos);
	BCD_LOG(dat, pos, 1);
	return pos;
}

//60  自定义域  n..017  LLLVAR  BCD  M  M 
//0003 	435550
unsigned int Build_qpboc_8583_63_purse(unsigned char *dat)
{
	unsigned int pos;
	//	stMobileParameter smpPara;
	//	unsigned char tempbuf2[50]="\0";
	//	stTradeBatchinfo *oinfo;

	pos = 2;//前2个字节是BCD码的LLL


	memcpy(dat + pos, "\x43\x55\x50", 3);//63.1  国际信用卡公司代码 "CUP" :435550
	pos += 3;

	//memset(dat + pos, ' ', 120);		//63.2  域天填充120个了空格
	//pos += 120;

	pos -= 2;	//数据部分的长度。

	INT2BCD(pos, dat, 2);//2字节长度的长度

	pos += 2;//刚减掉了LLL，现在加上长度。

	setBitmapBits(63, gpboc_8583bitmapaddr);

	MSG_LOG("%s:len:%d dat:", __FUNCTION__, pos);
	BCD_LOG(dat, pos, 1);
	return pos;
}



void ProcessAlgorithmCreatMacEcb_ChangSha(unsigned char *key, unsigned char *data, int len, unsigned char *mac)
{
	int i, j;// , n, m;

	unsigned char tmp[9] = "\0";
	unsigned char sTmp[17] = "\0";



	MSG_LOG("MAKLEY:");
	BCD_LOG(key, 16, 1);
	MSG_LOG("dat:len:%d\r\n", len);
	BCD_LOG(data, len, 1);

	memset(sTmp, 0x00, 8);
	memset(tmp, 0x00, 8);
	for (i = 0; i < len; i += 8) {
		if (i + 8 >= len) {
			break;
		}
		//memcpy(sTmp, data + i, 8);
		for (j = 0; j < 8; ++j)
		{
			tmp[j] ^= data[i + j];
		}
		//DES_encrypt(tmp, key, sTmp);
		//memcpy(tmp, sTmp, 8);
	}
	if (i < len) {	// 有不足8字节的块
		memset(sTmp, 0x00, 8);
		memcpy(sTmp, data + i, len - i);
		for (j = 0; j < 8; ++j)
		{
			tmp[j] ^= sTmp[j];
		}
		//DES_encrypt(tmp, key, mac);
	}
	// c) 将异或运算后的最后8个字节（RESULT BLOCK）转换成16 个HEXDECIMAL：
	BCD2Ascii(tmp, sTmp, 8);
	// d)  取前8 个字节用MAK加密：
	DES_encrypt(sTmp, key, tmp);
	// e)  将加密后的结果与后8 个字节异或：
	for (j = 0; j < 8; ++j)
	{
		sTmp[j + 8] ^= tmp[j];
	}
	// f)  用异或的结果TEMP BLOCK 再进行一次单倍长密钥算法运算。
	DES_encrypt(sTmp + 8, key, tmp);
	// g)  将运算后的结果（ENC BLOCK2）转换成16 个HEXDECIMAL：
	BCD2Ascii(tmp, sTmp, 8);

	memcpy(mac, sTmp, 8);

	return;
}


void ProcessAlgorithmCreatMacEcb(unsigned char *key, unsigned char *data, int len, unsigned char *mac)
{
	int i, j;// , n, m;

	unsigned char tmp[9] = "\0";
	unsigned char sTmp[8] = "\0";



	MSG_LOG("MAKLEY:");
	BCD_LOG(key, 16, 1);
	MSG_LOG("dat:len:%d\r\n", len);
	BCD_LOG(data, len, 1);

	memset(sTmp, 0x00, 8);
	memset(tmp, 0x00, 8);
	for (i = 0; i < len; i += 8) {
		if (i + 8 >= len) {
			break;
		}
		//memcpy(sTmp, data + i, 8);
		for (j = 0; j < 8; ++j)
		{
			tmp[j] ^= data[i + j];
		}
		DES_encrypt(tmp, key, sTmp);
		memcpy(tmp, sTmp, 8);
	}
	if (i < len) {	// 有不足8字节的块
		memset(sTmp, 0x00, 8);
		memcpy(sTmp, data + i, len - i);
		for (j = 0; j < 8; ++j)
		{
			tmp[j] ^= sTmp[j];
		}
		DES_encrypt(tmp, key, mac);
	}

	return;
}



int Build_qpboc_8583_64(unsigned char *iDat, unsigned int iDatLen, unsigned char *oDat)
{
	//	unsigned int cc;
	// 	unsigned char radom[8];
	unsigned char omac[8];
	// 	unsigned char buffer[60];
		//stMobilStyle



	setBitmapBits(64, gpboc_8583bitmapaddr);	//因为MAC计算加上了位元表，所以要先把第64域的位加上，再计算MAC

	ProcessAlgorithmCreatMacEcb_ChangSha(Sign_Infor.MAC_KEY, iDat, iDatLen, omac);

	memcpy(oDat, omac, 8);	//取前8个字节

	MSG_LOG("%s:len:%d dat:", __FUNCTION__, 8);
	BCD_LOG(oDat, 8, 1);

	return 8;
}


#if 1
//冲正报文
int build8583_qpboc_re_Purse_0400(unsigned char *oDat)
{
#define BUILD8583_FORM_RECORD(pTlv, tlv8583, fieldId, ilen, iret) \
	pTlv = tlv8583 + fieldId - 1;\
	if (pTlv->tag == fieldId){\
		iret = pTlv->len; \
		MSG_LOG("build%d--%d:", fieldId, iret); \
		BCD_LOG(pTlv->pValue, iret, 1); \
		memcpy(oDat + ilen, pTlv->pValue, iret); \
		ilen += iret; \
		setBitmapBits(fieldId, gpboc_8583bitmapaddr);\
	}

	DEF8583MSG_BIP *msg6001A;
	//	int curp, ltemp, headp;
	int iret, ilen;
	unsigned char infor_temp[1000];
	int inforLen = 1000;
	stTlv tlv8583[65] = { 0 };
	stTlv *pTlv = NULL;
	//	int tlvCount = 64;
	unsigned cardStyle = 0;
	int fieldId = 0;
	int i = 0;

	MSG_LOG("build8583_qpboc_re_Purse_0400:\n");
	if (gmissflag == MISS_PBOC_RE_PURSE) {
		iret = read_re_infor(infor_temp, &inforLen);
		if (iret != Ret_OK)
		{
			fieldId = fieldId;
			MSG_LOG("read_re_infor(冲正记录突然被删除了):%d\n", iret);
			return 0;
		}
		MSG_LOG("infor_temp2:");
		BCD_LOG(infor_temp, inforLen, 1);

		ilen = 0;
		msg6001A = (DEF8583MSG_BIP*)oDat;


		iret = PareseT1L2V_8583(infor_temp, inforLen, tlv8583);
		if (iret != Ret_OK) {
			MSG_LOG("冲正记录错误:%d\n", iret);
			return 0;
		}
		if (tlv8583[64].tag == 0 || tlv8583[64].len != 1) {
			MSG_LOG("冲正记录无卡交易类型:%d,%d\n", tlv8583[64].tag, tlv8583[64].len);
			return 0;
		}
	}
	else {
		iret = save_ODA_infor(ODA_FeRC_READ, infor_temp);
		if (iret != Ret_OK)
		{
			fieldId = fieldId;
			MSG_LOG("read_re_infor(ODA 冲正记录突然被删除了):%d\n", iret);
			save_ODA_infor(ODA_FeRC_Dlelt, NULL);
			return 0;
		}
		inforLen = GET_INT16(infor_temp + 2);
		MSG_LOG("infor_temp1:");
		BCD_LOG(infor_temp, inforLen + 4, 1);

		ilen = 0;
		msg6001A = (DEF8583MSG_BIP*)oDat;

		iret = PareseT1L2V_8583(infor_temp + 4, inforLen, tlv8583);
		if (iret != Ret_OK) {
			MSG_LOG("冲正记录错误1:%d\n", iret);
			return 0;
		}
		if (tlv8583[64].tag == 0 || tlv8583[64].len != 1) {
			MSG_LOG("冲正记录无卡交易类型1:%d,%d\n", tlv8583[64].tag, tlv8583[64].len);
			return 0;
		}
	}
	cardStyle = tlv8583[64].pValue[0];
	if (cardStyle == CARDSTYLE_UNPAY_ODA) {
		memcpy(oDat + ilen, "\x02\x00", 2); ilen += 2;		//消息类型
		gpboc_8583bitmapaddr = msg6001A->bitMap;
		memset(gpboc_8583bitmapaddr, 0, 8);
		ilen += 8;	//这里是位元表，跳过		
		for (i = 0; i < 64; i++)
		{
			if (tlv8583[i].tag != 0) {
				BUILD8583_FORM_RECORD(pTlv, tlv8583, tlv8583[i].tag, ilen, iret);
			}
#if 0
			if (tlv8583[i].tag == 11) {
				iret = Build_qpboc_8583_11(oDat + ilen);
				//SetTLV(tlv8583 + tlvCount, 11, iret, oDat + ilen);
				//++tlvCount;
				//BCD2Ascii(oDat + ilen, originData + 8, 3);
				ilen += iret;
				save_deal_count_add(BIT_PBOC_NS_8583);
			}
			else if (tlv8583[i].tag != 0) {
				BUILD8583_FORM_RECORD(pTlv, tlv8583, tlv8583[i].tag, ilen, iret);
			}
#endif
		}
		goto build8583_qpboc_re_Purse_0400_OVER;
	}
	else {
		memcpy(oDat + ilen, "\x04\x00", 2); ilen += 2;		//消息类型
	}
	gpboc_8583bitmapaddr = msg6001A->bitMap;
	memset(gpboc_8583bitmapaddr, 0, 8);
	ilen += 8;	//这里是位元表，跳过	

	BUILD8583_FORM_RECORD(pTlv, tlv8583, 3, ilen, iret);

	BUILD8583_FORM_RECORD(pTlv, tlv8583, 4, ilen, iret);

	BUILD8583_FORM_RECORD(pTlv, tlv8583, 11, ilen, iret);

	BUILD8583_FORM_RECORD(pTlv, tlv8583, 22, ilen, iret);

	BUILD8583_FORM_RECORD(pTlv, tlv8583, 25, ilen, iret);

	BUILD8583_FORM_RECORD(pTlv, tlv8583, 35, ilen, iret);

	iret = 2;
	MSG_LOG("build39--:3938\r\n");

	memcpy(oDat + ilen, "98", 2); ilen += iret;	//POS终端在时限内未能收到POS中心的应答消息而引发，冲正原因码填“98”。
	setBitmapBits(39, gpboc_8583bitmapaddr);

	BUILD8583_FORM_RECORD(pTlv, tlv8583, 41, ilen, iret);

	BUILD8583_FORM_RECORD(pTlv, tlv8583, 42, ilen, iret);

	BUILD8583_FORM_RECORD(pTlv, tlv8583, 48, ilen, iret);

	BUILD8583_FORM_RECORD(pTlv, tlv8583, 49, ilen, iret);

	BUILD8583_FORM_RECORD(pTlv, tlv8583, 55, ilen, iret);

	BUILD8583_FORM_RECORD(pTlv, tlv8583, 57, ilen, iret);
	BUILD8583_FORM_RECORD(pTlv, tlv8583, 59, ilen, iret);

	BUILD8583_FORM_RECORD(pTlv, tlv8583, 60, ilen, iret);

build8583_qpboc_re_Purse_0400_OVER:
	iret = Build_qpboc_8583_64(oDat, ilen, oDat + ilen);
	ilen += iret;

	MSG_LOG("Build_qpboc_8583MAP:len:8 dat:");
	BCD_LOG(gpboc_8583bitmapaddr, 8, 1);

	return ilen;

#undef BUILD8583_FORM_RECORD
}
#endif

int build8583_qpboc_Purse_0200(unsigned char *oDat)
{
	DEF8583MSG_BIP *msg6001A;
	//	int curp, ltemp, headp;
	int iret, ilen;
	stTlv tlv8583[50];
	int tlvCount = 0;
	int tmpI;
	unsigned char originData[10] = { 0x00,0x16 };


	ilen = 0;
	msg6001A = (DEF8583MSG_BIP*)oDat;
	memcpy(oDat + ilen, "\x02\x00", 2); ilen += 2;		//消息类型

	MSG_LOG("Build_qpboc_8583_00:len:2 dat:");
	BCD_LOG(oDat, ilen, 1);

	gpboc_8583bitmapaddr = msg6001A->bitMap;
	memset(gpboc_8583bitmapaddr, 0, 8);
	ilen += 8;	//这里是位元表，跳过

	SetTLV(tlv8583 + tlvCount, 65, 1, &gMCardCand);
	++tlvCount;
MSG_LOG("gMCardCand====:%d\n", gMCardCand);
	if (gMCardCand != CARDSTYLE_QR_PBOC)
	{
		iret = Build_qpboc_8583_02(oDat + ilen); 	//主帐号

		SetTLV(tlv8583 + tlvCount, 2, iret, oDat + ilen);
		++tlvCount;

		ilen += iret;
	}

	iret = Build_qpboc_8583_03(oDat + ilen); 		//处理交易码
	SetTLV(tlv8583 + tlvCount, 3, iret, oDat + ilen);
	++tlvCount;
	ilen += iret;


	iret = Build_qpboc_8583_04(oDat + ilen); 		//交易金额
	SetTLV(tlv8583 + tlvCount, 4, iret, oDat + ilen);
	++tlvCount;
	ilen += iret;

	iret = Build_qpboc_8583_11(oDat + ilen);
	SetTLV(tlv8583 + tlvCount, 11, iret, oDat + ilen);
	++tlvCount;
	memcpy(originData + 5, oDat + ilen, 3);
	ilen += iret;
	save_deal_count_add(BIT_PBOC_NS_8583);

//	uiTemp = get_deal_count(BIT_PBOC_NS_8583);
	//iret = Build_qpboc_8583_13(oDat + ilen);
	//dat[0] = SysTime.month;
	//dat[1] = SysTime.year_l;
	//SetTLV(tlv8583 + tlvCount, 13, iret, oDat + ilen);
	//++tlvCount;
	memcpy(originData + 8, oDat + ilen, 2);
	//ilen += iret;

	iret = Build_qpboc_8583_22(oDat + ilen);		//07  快速  PBOC 借/贷记 IC 卡读入 2（非接触式）交易中不包含 PIN
	SetTLV(tlv8583 + tlvCount, 22, iret, oDat + ilen);
	++tlvCount;
	ilen += iret;

	if (gMCardCand != CARDSTYLE_QR_PBOC) 
	{	// 这个是可选的
		iret = Build_qpboc_8583_23(oDat + ilen);
		SetTLV(tlv8583 + tlvCount, 23, iret, oDat + ilen);
		++tlvCount;
		ilen += iret;
	}

	iret = Build_qpboc_8583_25(oDat + ilen);
	SetTLV(tlv8583 + tlvCount, 25, iret, oDat + ilen);
	++tlvCount;
	ilen += iret;

	if (gMCardCand != CARDSTYLE_QR_PBOC)
	{
		iret = Build_qpboc_8583_35(oDat + ilen);//二磁道数据
		SetTLV(tlv8583 + tlvCount, 35, iret, oDat + ilen);
		++tlvCount;
		ilen += iret;
	}

	iret = Build_qpboc_8583_41(oDat + ilen);
	SetTLV(tlv8583 + tlvCount, 41, iret, oDat + ilen);
	++tlvCount;
	ilen += iret;

	iret = Build_qpboc_8583_42(oDat + ilen);
	SetTLV(tlv8583 + tlvCount, 42, iret, oDat + ilen);
	++tlvCount;
	ilen += iret;
#if 0
	if (gMCardCand != CARDSTYLE_QPBOC) {
		iret = Build_qpboc_8583_48(oDat + ilen);
		SetTLV(tlv8583 + tlvCount, 48, iret, oDat + ilen);
		++tlvCount;
		ilen += iret;
	}
#endif

	iret = Build_qpboc_8583_49(oDat + ilen);
	SetTLV(tlv8583 + tlvCount, 49, iret, oDat + ilen);
	++tlvCount;
	ilen += iret;

	if (gMCardCand == CARDSTYLE_QPBOC)
	{
#if 0
		iret = Build_qpboc_8583_53(oDat + ilen);
		SetTLV(tlv8583 + tlvCount, 53, iret, oDat + ilen);
		++tlvCount;
		ilen += iret;
#endif
		//		iret = Build_qpboc_8583_53(oDat+ilen);	ilen+=iret;		//有二磁道信息时候必须出现
		iret = Build_qpboc_8583_55(oDat + ilen, 1);
		SetTLV(tlv8583 + tlvCount, 55, iret, oDat + ilen);
		++tlvCount;
		ilen += iret;
	}

	if (gMCardCand == CARDSTYLE_QR_PBOC)
	{
#if 0
		iret = Build_qpboc_8583_57(oDat + ilen);
		//SetTLV(tlv8583 + tlvCount, 57, iret, oDat + ilen);
		//++tlvCount;
		ilen += iret;
#endif

#if 1
	iret = Build_qpboc_8583_59(oDat + ilen);
	SetTLV(tlv8583 + tlvCount, 59, iret, oDat + ilen);
	++tlvCount;
	ilen += iret;

#endif
		}
	iret = Build_qpboc_8583_60_purse(oDat + ilen);
	memcpy(originData + 2, oDat + ilen + 1, 3);
	SetTLV(tlv8583 + tlvCount, 60, iret, oDat + ilen);
	++tlvCount;
	ilen += iret;

#if 1
	SetTLV(tlv8583 + tlvCount, 61, sizeof(originData), originData);
	++tlvCount;
#endif
#if 0
	if (gMCardCand == CARDSTYLE_UNPAY_ODA) {
		iret = Build_qpboc_8583_63_purse(oDat + ilen);
		SetTLV(tlv8583 + tlvCount, 63, iret, oDat + ilen);
		++tlvCount;
		ilen += iret;
	}
#endif

	iret = Build_qpboc_8583_64(oDat, ilen, oDat + ilen);
	ilen += iret;

	MSG_LOG("Build_qpboc_8583MAP:len:8 dat:");
	BCD_LOG(gpboc_8583bitmapaddr, 8, 1);


	tmpI = sizeof(repurse_infor) - 4;
	iret = BuildT1L2V(tlv8583, tlvCount, repurse_infor + 4, &tmpI);
	if (iret != Ret_OK) {
		MSG_LOG("冲正数据组织失败\n");
	}
	memcpy(repurse_infor, RECORD_FALG, 2);
	SET_INT16(repurse_infor + 2, tmpI);
	MSG_LOG("打好冲正总的(%d):\n", tmpI);
	BCD_LOG(repurse_infor, tmpI + 4, 1);

	return ilen;
}
/*									认证定长
整包固定长度		厂商机型	硬件序列号	认证密文	随机数	校验位
	 99			ANS20	ANS38		ANS24	N16	   ANS1
*/
int build8583_qpboc_Auth(unsigned char *oDat, unsigned inout) {
	//int pos = 0;
	int ilen;
	//unsigned char rands = "\x1e\x45\x7e\xa9\x02\x12\xef\xa3\x76\x9b\x"
	unsigned char rands[16];// = "\x11\x22\x33\x44\x55\x66\x77\x88\x99\x00\x11\x22\x33\x44\x55\x66";
	unsigned char keyMain[16];
	unsigned char keySession[16];
	int i;
	MD5_CTX md5;

	ilen = 0;
	// ANS20
	memcpy(oDat + ilen, "EFS3000L", 8);
	ilen += 8;
	memset(oDat + ilen, ' ', 16);
	ilen += 12;
	// 计算认证密文
	// 随机数SysTime
	GetDateTime();

	MD5Init(&md5);
	MD5Update(&md5, (unsigned char *)&SysTime, sizeof(stcomtime));
	MD5Final(&md5, rands);

	// session key
	getMobileParameter(7, keyMain);
	for (i = 0; i < 16; i += 8) {
		DES3_encrypt(rands + i, keyMain, keySession + i);
	}
	for (i = 0; i < 24; i += 8) {
		DES3_encrypt(oDat + i, keySession, oDat + 58 + i);
	}
	DES3_encrypt(oDat, keySession, oDat + 58);
	// ANS38
#ifdef PBOC_TEST1
	memcpy(oDat + ilen, "LF28517121111111", 16);
	ilen += 16;
#else
	memcpy(oDat + ilen, "EFS3000L", 8);
	ilen += 8;
	//Get_SerialNumF4((unsigned char *)oDat + ilen + 8);//Get_SerialNumF4_Self(buff);//
	//BCD2Ascii(oDat + ilen + 8, oDat + ilen, 4);

	memcpy(oDat + ilen, pFistVary.DeviceNo, 8);
	ilen += 8;
#endif
	memset(oDat + ilen, ' ', 22);
	ilen += 22;
	// ANS24
	ilen += 24;
	// N16
	memcpy(oDat + ilen, rands, 16);
	ilen += 16;
	for (i = 0; i < 16; ++i)
	{
		oDat[ilen] ^= rands[i];
	}
	++ilen;
	MSG_LOG("build8583_qpboc_Auth:len:%d dat:", ilen);
	BCD_LOG(oDat, ilen, 1);

	return ilen;
}
//0 签到，1签退
int build8583_qpboc_SING(unsigned char *oDat, unsigned inout)
{
	DEF8583MSG_BIP *msg6001A;
	//	int curp, ltemp, headp;
	int iret, ilen;

	ilen = 0;
	msg6001A = (DEF8583MSG_BIP*)oDat;
	if (inout == 0)
	{
		memcpy(oDat + ilen, "\x08\x00", 2); ilen += 2;		//消息类型
	}
	else {
		memcpy(oDat + ilen, "\x08\x20", 2); ilen += 2;		//消息类型  签退

	}

	MSG_LOG("Build_qpboc_8583_00:len:2 %s dat:", inout == 0 ? "签到" : "签退");
	BCD_LOG(oDat, ilen, 1);

	gpboc_8583bitmapaddr = msg6001A->bitMap;
	memset(gpboc_8583bitmapaddr, 0, 8);
	ilen += 8;	//这里是位元表，跳过

	iret = Build_qpboc_8583_11(oDat + ilen); ilen += iret;
	save_deal_count_add(BIT_PBOC_NS_8583);
	//  	iret = Build8583_12(oDat+ilen); ilen+=iret;			//hhmmss
	//  	iret = Build_qpboc_8583_13(oDat+ilen); ilen+=iret;	//2B  MMDD
	// 	iret = Build_qpboc_8583_32(oDat+ilen);	ilen+=iret;
	iret = Build_qpboc_8583_41(oDat + ilen);	ilen += iret;
	iret = Build_qpboc_8583_42(oDat + ilen);	ilen += iret;
	iret = Build_qpboc_8583_60_login(oDat + ilen);	ilen += iret;

	if (inout == 0)
	{
		iret = Build_qpboc_8583_63(oDat + ilen);	ilen += iret;//操作员代码 
	}

	MSG_LOG("Build_qpboc_8583MAP:len:8 dat:");
	BCD_LOG(gpboc_8583bitmapaddr, 8, 1);

	return ilen;
}
unsigned int Build_qpboc_8583_63_BLK(unsigned char *dat)
{
	unsigned int pos;
	//	stMobileParameter smpPara;
	//	unsigned char tempbuf2[50]="\0";
	//	stTradeBatchinfo *oinfo;

	pos = 2;//前2个字节是BCD码的LLL

	memcpy(dat + pos, (unsigned char *)&TYPE_63, sizeof(QPBOC_TYPE_63));//操作员代码
	pos += sizeof(QPBOC_TYPE_63);


	pos -= 2;	//数据部分的长度。

	INT2BCD(pos, dat, 2);//2字节长度的长度

	pos += 2;//刚减掉了LLL，现在加上长度。

	setBitmapBits(63, gpboc_8583bitmapaddr);

	MSG_LOG("%s:len:%d dat:", __FUNCTION__, pos);
	BCD_LOG(dat, pos, 1);
	return pos;
}

unsigned int Build_qpboc_8583_60_BLK(unsigned char *dat)
{
	unsigned int pos;
	//	stMobileParameter smpPara;
	//	unsigned char tempbuf2[50]="\0";
	//	stTradeBatchinfo oinfo;
	char buffer[32];
	unsigned int uiTemp = 0;
	unsigned char bcdlen = 0;

	pos = 2;//前2个字节是BCD码的LLL

	dat[pos++] = 0;	//交易类型 00
	bcdlen = 2;
	//	getMobileParameter((unsigned char * )&smpPara);

	uiTemp = get_deal_count(BIT_PBOC_NS_BIG);

	//feread(BIT_PBOC_NS_BIG, 4, (unsigned char*)&uiTemp);

	MSG_LOG("read:%d\r\n", uiTemp);

	memset(buffer, 0, sizeof(buffer));
	sprintf((char*)buffer, "%06u", uiTemp);
	uiTemp++;
	MSG_LOG("批次:%s\r\n", buffer);
	memcpy(dat + pos, ASC2BCD((char*)buffer, 6), 3);
	pos += 3;
	bcdlen += 6;


	memcpy(dat + pos, "\x57\x00", 2);// 3个字符双倍长密钥算法（含磁道密钥）终端用 004
	pos += 2;

	bcdlen += 3;




	//	MSG_LOG("bcdlen:%d\r\n",bcdlen);

	INT2BCD(bcdlen, dat, 2);//2字节长度的长度    感觉是个ASCII 发送的时候却是个bcd 晕 


	setBitmapBits(60, gpboc_8583bitmapaddr);
	MSG_LOG("%s:len:%d dat:", __FUNCTION__, pos);
	BCD_LOG(dat, pos, 1);
	return pos;
}
int build8583_qpboc_0800(unsigned char *oDat)
{
	DEF8583MSG_BIP *msg6001A;
	//	int curp, ltemp, headp;
	int iret, ilen;

	ilen = 0;
	msg6001A = (DEF8583MSG_BIP*)oDat;
	memcpy(oDat + ilen, "\x08\x00", 2); ilen += 2;		//消息类型

	BCD_LOG(oDat, ilen, 1);

	gpboc_8583bitmapaddr = msg6001A->bitMap;
	memset(gpboc_8583bitmapaddr, 0, 8);
	ilen += 8;	//这里是位元表，跳过

//	iret = Build_qpboc_8583_11(oDat + ilen); ilen += iret;
//	save_deal_count_add(BIT_PBOC_NS_8583);
	//  	iret = Build8583_12(oDat+ilen); ilen+=iret;			//hhmmss
	//  	iret = Build_qpboc_8583_13(oDat+ilen); ilen+=iret;	//2B  MMDD
	// 	iret = Build_qpboc_8583_32(oDat+ilen);	ilen+=iret;
	iret = Build_qpboc_8583_41(oDat + ilen);	ilen += iret;
	iret = Build_qpboc_8583_42(oDat + ilen);	ilen += iret;
	iret = Build_qpboc_8583_60_BLK(oDat + ilen);	ilen += iret;

	iret = Build_qpboc_8583_63_BLK(oDat + ilen);	ilen += iret;//


	MSG_LOG("Build_qpboc_8583MAP:len:8 dat:");
	BCD_LOG(gpboc_8583bitmapaddr, 8, 1);

	return ilen;
}
//
unsigned int Build_qpboc_8583_63_para(unsigned char *dat, unsigned char *indata, unsigned int len)
{
	unsigned int pos;
	//	stMobileParameter smpPara;
	//	unsigned char tempbuf2[50]="\0";
	//	stTradeBatchinfo *oinfo;
	if (len == 0)
	{
		MSG_LOG("没有63\r\n");
		return 0;

	}
	pos = 2;//前2个字节是BCD码的LLL

	memcpy(dat + pos, indata, len);//操作员代码
	pos += len;


	pos -= 2;	//数据部分的长度。

	INT2BCD(pos, dat, 2);//2字节长度的长度

	pos += 2;//刚减掉了LLL，现在加上长度。

	setBitmapBits(63, gpboc_8583bitmapaddr);

	MSG_LOG("%s:len:%d dat:", __FUNCTION__, pos);
	BCD_LOG(dat, pos, 1);
	return pos;
}

unsigned int Build_qpboc_8583_60_para(unsigned char *dat, unsigned char *mode)
{
	unsigned int pos;
	//	stMobileParameter smpPara;
	//	unsigned char tempbuf2[50]="\0";
	//	stTradeBatchinfo oinfo;
	char buffer[32];
	unsigned int uiTemp = 0;
	unsigned char bcdlen = 0;

	pos = 2;//前2个字节是BCD码的LLL

	dat[pos++] = 0;	//交易类型 00
	bcdlen = 2;
	//	getMobileParameter((unsigned char * )&smpPara);

	uiTemp = get_deal_count(BIT_PBOC_NS_BIG);

	//feread(BIT_PBOC_NS_BIG, 4, (unsigned char*)&uiTemp);

	MSG_LOG("read:%d\r\n", uiTemp);

	memset(buffer, 0, sizeof(buffer));
	sprintf((char*)buffer, "%06u", uiTemp);
	uiTemp++;
	MSG_LOG("批次:%s\r\n", buffer);
	memcpy(dat + pos, ASC2BCD((char*)buffer, 6), 3);
	pos += 3;
	bcdlen += 6;
	memcpy(dat + pos, mode, 2);// 3个字符双倍长密钥算法（含磁道密钥）终端用 004
	pos += 2;

	bcdlen += 3;
	//	MSG_LOG("bcdlen:%d\r\n",bcdlen);
	INT2BCD(bcdlen, dat, 2);//2字节长度的长度    感觉是个ASCII 发送的时候却是个bcd 晕 
	setBitmapBits(60, gpboc_8583bitmapaddr);
	MSG_LOG("%s:len:%d dat:", __FUNCTION__, pos);
	BCD_LOG(dat, pos, 1);
	return pos;
}
int build8583_qpboc_0820(unsigned char *oDat, unsigned char *mode, unsigned char *mode_63, unsigned int len, unsigned char type)
{
	DEF8583MSG_BIP *msg6001A;
	//	int curp, ltemp, headp;
	int iret, ilen;

	ilen = 0;
	msg6001A = (DEF8583MSG_BIP*)oDat;
	if (type == 2)
	{
		memcpy(oDat + ilen, "\x08\x20", 2); ilen += 2;		//消息类型
	}
	else
	{
		memcpy(oDat + ilen, "\x08\x00", 2); ilen += 2;		//消息类型
	}
	BCD_LOG(oDat, ilen, 1);

	gpboc_8583bitmapaddr = msg6001A->bitMap;
	memset(gpboc_8583bitmapaddr, 0, 8);
	ilen += 8;	//这里是位元表，跳过

	//	iret = Build_qpboc_8583_11(oDat + ilen); ilen += iret;
	//	save_deal_count_add(BIT_PBOC_NS_8583);
	//  	iret = Build8583_12(oDat+ilen); ilen+=iret;			//hhmmss
	//  	iret = Build_qpboc_8583_13(oDat+ilen); ilen+=iret;	//2B  MMDD
	// 	iret = Build_qpboc_8583_32(oDat+ilen);	ilen+=iret;
	iret = Build_qpboc_8583_41(oDat + ilen);	ilen += iret;
	iret = Build_qpboc_8583_42(oDat + ilen);	ilen += iret;
	iret = Build_qpboc_8583_60_para(oDat + ilen, mode);	ilen += iret;

	iret = Build_qpboc_8583_63_para(oDat + ilen, mode_63, len);	ilen += iret;//操作员写死

	MSG_LOG("Build_qpboc_8583MAP:len:8 dat:");
	BCD_LOG(gpboc_8583bitmapaddr, 8, 1);

	return ilen;
}


// int build8583_qpboc_heart_0820(unsigned char *oDat)
// {
// 	DEF8583MSG_BIP *msg6001A;
// 	//	int curp, ltemp, headp;
// 	int iret, ilen;
// 	
// 	ilen = 0;
// 	msg6001A = (DEF8583MSG_BIP*)oDat;
// 	memcpy(oDat+ilen, "\x08\x20", 2); ilen+=2;		//消息类型
// 	
// 	MSG_LOG("Build_qpboc_8583_00:len:2 dat:");
// 	BCD_LOG(oDat,ilen,1);
// 	
// 	gpboc_8583bitmapaddr = msg6001A->bitMap;
// 	memset(gpboc_8583bitmapaddr, 0, 8);
// 	ilen += 8;	//这里是位元表，跳过
// 	
// 	/iret = Build8583_12(oDat+ilen); ilen+=iret;			//hhmmss
// 	iret = Build_qpboc_8583_13(oDat+ilen); ilen+=iret;	//2B  MMDD
// 	// 	iret = Build_qpboc_8583_32(oDat+ilen);	ilen+=iret;
// 	iret = Build_qpboc_8583_39(oDat+ilen); ilen+=iret;	//2B  "00"
// 	iret = Build_qpboc_8583_41(oDat+ilen);	ilen+=iret;
// 	iret = Build_qpboc_8583_42(oDat+ilen);	ilen+=iret;
// 	iret = Build_qpboc_8583_60(oDat+ilen);	ilen+=iret;
// 	iret = Build_qpboc_8583_63(oDat+ilen);	ilen+=iret;//操作员代码 
// 	
// 	
// 	MSG_LOG("Build_qpboc_8583MAP:len:8 dat:");
// 	BCD_LOG(gpboc_8583bitmapaddr,8,1);
// 	
// 	return ilen;
// }

unsigned int Build_http_pack(char *outhttppack, unsigned int ip, unsigned int port, unsigned int Senddatlen)
{
	char IPbuffer[20] = { 0 };
	unsigned char temp[4] = { 0 };
	unsigned int lenth = 0;

	memcpy(temp, &ip, 4);

	memset(IPbuffer, 0, sizeof(IPbuffer));
	memset(IPbuffer, 0, sizeof(IPbuffer));

	sprintf(IPbuffer, "%03d.%03d.%03d.%03d", temp[0], temp[1], temp[2], temp[3]);

	//lenth = sprintf(outhttppack, HTTP_POST, IPbuffer, port, Senddatlen);
	//lenth = sprintf(outhttppack, HTTP_POST, Senddatlen);

	return lenth;

}


unsigned char mode_para[10];
unsigned char mode_63[128];
unsigned int mode_len;
unsigned char mode_type;
unsigned int Build_qpboc_8583Packge(unsigned char *revBuf, unsigned char mode)
{
	unsigned int blen = 0, len = 0;

	revBuf[len++] = 0;	//前面是前导长度。
	revBuf[len++] = 0;	//前面是前导长度。

	blen = Build_qpboc_8583_HD(revBuf + len, mode != MISS_PBOC_LOGIN);
	len += blen;

	if (s_isAuthOk == 0)
	{
		len = 2;
		blen = build8583_qpboc_Auth(revBuf + len, 0);
		if (blen == 0)
			return 0;
		len += blen;
		revBuf[0] = (len - 2) / 256;
		revBuf[1] = (len - 2) % 256;//最前面添加总长度

		return len;
	}
	switch (mode)
	{
	case MISS_PBOC_LOGIN://签到
		blen = build8583_qpboc_SING(revBuf + len, 0);
		if (blen == 0)
			return 0;
		len += blen;
		revBuf[0] = (len - 2) / 256;
		revBuf[1] = (len - 2) % 256;//最前面添加总长度
		break;
	case MISS_PBOC_UPREC_ODA_first:
	case MISS_PBOC_PURSE:	//1.4.1　消费记录上传-7001
		//blen = build8583UPREC7001(revBuf+len);

		if (get_repurse_num() == ST_OK )
		{
			MSG_LOG("有冲正不给发\r\n");
			len = 0;
			break;
		}
		blen = build8583_qpboc_Purse_0200(revBuf + len);
		//		MSG_LOG("blen:%d\r\n",blen);
		if (blen == 0)
			return 0;
		len += blen;
		revBuf[0] = (len - 2) / 256;
		revBuf[1] = (len - 2) % 256;//最前面添加总长度
		break;
	case MISS_PBOC_UPREC_ODA:
	case MISS_PBOC_RE_PURSE:	//1.4.1　消费记录上传-7001
		//blen = build8583UPREC7001(revBuf+len);
		blen = build8583_qpboc_re_Purse_0400(revBuf + len);
		//		MSG_LOG("blen:%d\r\n",blen);
		if (blen <= 0)
			return 0;
		len += blen;
		revBuf[0] = (len - 2) / 256;
		revBuf[1] = (len - 2) % 256;//最前面添加总长度
		break;
	case MISS_PBOC_DOWN_ODA_BLK:
		blen = build8583_qpboc_0800(revBuf + len);
		//		MSG_LOG("blen:%d\r\n",blen);
		if (blen <= 0)
			return 0;
		len += blen;
		revBuf[0] = (len - 2) / 256;
		revBuf[1] = (len - 2) % 256;//最前面添加总长度
		break;
	case MISS_PBOC_UPREC_ca:
		blen = build8583_qpboc_0820(revBuf + len, mode_para, mode_63, mode_len, mode_type);
		//		MSG_LOG("blen:%d\r\n",blen);
		if (blen <= 0)
			return 0;
		len += blen;
		revBuf[0] = (len - 2) / 256;
		revBuf[1] = (len - 2) % 256;//最前面添加总长度
		break;
	default:
		len = 0;
		break;
	}
	return len;
}



/************************************************************************/
/*    贵州银联报文格式
位  域名定义  属性  格式  类型  请求  响应  备    注
消息类型  n4    BCD  0800  0810  MSG-TYPE-ID
位元表  b64    BINARY  M  M  BIT MAP
2  主账号  n..19  LLVAR  BCD  C  M  当 22 域指明非磁条输入且可判卡号
4  交易金额  N12    BCD  M  M
11  受卡方系统跟踪号  n6    BCD  M  M  POS 终端交易流水
12  受卡方所在地时间  n6  hhmmss  BCD    M
13  受卡方所在地日期  n4  MMDD  BCD    M
14  卡有效期  N4  YYMM  BCD  C1  C2
22  服务点输入方式码  N3    BCD  M
32  受理方标识码  n..11  LLVAR  BCD    M
37  检索参考号  an12    ASCII    M
39  应答码  an2    ASCII    M
41  受卡机终端标识码  ans8    ASCII  M  M  终端代码
42  受卡方标识码  ans15    ASCII  M  M  商户代码
49  交易货币代码  an3    ASCII  M  M
60  自定义域  n?017  LLLVAR  BCD  M  M
60.1  交易类型码  n2    BCD  M  M  填“00”
60.2  批次号  n6    BCD  M  M
60.3  网络管理信息码  n3    BCD  M  M
单倍长密钥算法终端用 001/
双倍长密钥算法终端用 003/
双倍长密钥算法（含磁道密钥）终
端用 004
62 终端信息/终端密钥 b...084  LLLVAR  BINARY  C  C终端可获取设备信息且在请求报文中出现，使用用法十九；应答报文按照下列要求填写：
当 39 域为“00”时必选，
当 60.3 域填写 001 时包含 PIK、MAK，共 24 字节；
当 60.3 域填写 003 时包含 PIK、MAK，共 40 字节；
当 60.3 域填写 004 时包含 PIK、MAK 和 TDK，共 60 字节

  63  自定义域  ans...003  LLLVAR  ASCII  M
63.1  操作员代码  an3    ASCII  M
															 */
															 /************************************************************************/

//add hb
unsigned char deal_qpoc_blk(unsigned char * data, unsigned int len)
{
	MSG_LOG("63域数据\r\n");
	BCD_LOG(data, len, 1);

	if (data[0] == '0' || data[0] == '1')
	{
		return 1;//
	}
	memcpy(TYPE_63.card_no, data + 1, 10);
	BCD2Ascii((unsigned char *)&SysTime, TYPE_63.up_time, 4);
	fewrite(BIS_BLK_63, sizeof(QPBOC_TYPE_63), (unsigned char *)&TYPE_63);
	BCD_LOG((unsigned char *)&TYPE_63, sizeof(QPBOC_TYPE_63), 1);
	//存黑名单
	return 2;
}
unsigned char QPBOC_DataDeal(unsigned char *pakege, int packLen)
{
	struct ISO_8583 iso8583[128] =
	{
		{  8, L_ASC, FIX_LEN},  //1
		{ 19, L_BCD, LLVAR_LEN}, //2 账号
		{  6, L_BCD, FIX_LEN},	//3
		{  12, L_BCD, FIX_LEN},	//4
		{  8, L_BCD, FIX_LEN},	//5
		{  8, L_BCD, FIX_LEN},	//6
		{ 99, R_BCD, LLVAR_LEN},//7
		{  8, R_BCD, FIX_LEN},	//8
		{  8, R_BCD, FIX_LEN}, //9
		{  6, R_BCD, FIX_LEN}, //10
		{  6, R_BCD, FIX_LEN},	//11  跟踪号
		{  6, R_BCD, FIX_LEN},	//12  时间hhmmss
		{  4, R_BCD, FIX_LEN},	//13	时间MMDD
		{  4, R_BCD, FIX_LEN},	//14  卡有效期
		{  4, R_BCD, FIX_LEN},	//15
		{  4, R_BCD, FIX_LEN},	//16
		{  4, R_BCD, FIX_LEN},	//17
		{  4, R_BCD, FIX_LEN},	//18
		{  4, R_BCD, FIX_LEN},	//19
		{  4, R_BCD, FIX_LEN},	//20
		{  4, R_BCD, FIX_LEN},	//21
		{  4, R_BCD, FIX_LEN}, 	//22
		{  4, R_BCD, FIX_LEN},	//23
		{  4, R_BCD, FIX_LEN},	//24
		{  2, R_BCD, FIX_LEN},	//25  服务条件码
		{  2, R_BCD, FIX_LEN},	//26
		{  99, L_BCD, LLVAR_LEN},	//27
		{  8, L_BCD, FIX_LEN},	//28
		{  99, L_BCD, LLVAR_LEN},	//29
		{  99, L_BCD, LLVAR_LEN},	//30   
		{  99, L_BCD, LLVAR_LEN},	//31
		{  99, L_BCD, LLVAR_LEN},	//32  受理方标识码  n..11  LLVAR  BCD    M
		{  99, L_BCD, LLVAR_LEN},	//33
		{ 999, L_BCD, LLLVAR_LEN},	//34
		{ 37,  L_BCD, LLVAR_LEN},	//35
		{104,  L_BCD, LLLVAR_LEN},	//36
		{ 12,  L_ASC, FIX_LEN},		//37	检索参考号	
		{  6,  L_ASC, FIX_LEN},//38
		{  2,  L_ASC, FIX_LEN},		//39  应答码
		{ 99,  L_ASC, LLVAR_LEN},	//40
		{  8,  L_ASC, FIX_LEN},		//41   受卡机终端标识码  ans8    ASCII  M  M  终端代码
		{ 15,  L_ASC, FIX_LEN},		//42   受卡方标识码  ans15    ASCII  M  M  商户代
		{ 8,  L_ASC, FIX_LEN},	//43
		{ 25,  L_ASC, LLVAR_LEN},	//44		收单机构
		{ 999,  L_BCD, LLLVAR_LEN},	//45
		{999, R_BCD, LLLVAR_LEN},	//46
		{13, L_ASC, FIX_LEN},	//47  
		{999, L_ASC, LLLVAR_LEN},	//48
		{  3, R_ASC, FIX_LEN},		//49		交易货币代码 “156”
		{ 99,  L_BCD, LLVAR_LEN},	//50	
		{  999, L_BCD, LLLVAR_LEN},	//51	
		{999, L_ASC, LLLVAR_LEN},	//52
		{ 16, R_ASC, FIX_LEN},		//53
		{999, R_ASC, LLLVAR_LEN},	//54
		{ 255, BINARY, LLLVAR_LEN},	//55		
		{999, R_BCD, LLLVAR_LEN},	//56
		{999, R_BCD, LLLVAR_LEN},	//57
		{999, R_BCD, LLLVAR_LEN},	//58
		{999, L_ASC, LLLVAR_LEN},	//59
		{999, R_BCD, LLLVAR_LEN},	//60自定义域  n?017  LLLVAR  BCD  M  M  
		{999, L_ASC, LLLVAR_LEN},	//61
		{999, L_ASC, LLLVAR_LEN},	//62
		{999, R_ASC, LLLVAR_LEN},	//63
		{ 8, BINARY, FIX_LEN},		//64
		{  8, R_BCD, FIX_LEN},
		{  1, R_ASC, FIX_LEN},
		{  2, R_ASC, FIX_LEN},
		{  3, R_ASC, FIX_LEN},
		{  3, R_ASC, FIX_LEN},
		{  3, R_ASC, FIX_LEN},
		{  4, R_ASC, FIX_LEN},
		{  4, R_ASC, FIX_LEN},
		{  6, R_ASC, FIX_LEN},
		{ 10, R_ASC, FIX_LEN},
		{ 10, R_ASC, FIX_LEN},
		{ 10, R_ASC, FIX_LEN},
		{ 10, R_ASC, FIX_LEN},
		{ 10, R_ASC, FIX_LEN},
		{ 10, R_ASC, FIX_LEN},
		{ 10, R_ASC, FIX_LEN},
		{ 10, R_ASC, FIX_LEN},
		{ 12, R_ASC, FIX_LEN},
		{ 12, R_ASC, FIX_LEN},
		{ 12, R_ASC, FIX_LEN},
		{ 12, R_ASC, FIX_LEN},
		{ 16, R_ASC, FIX_LEN},
		{ 16, R_ASC, FIX_LEN},
		{ 16, R_ASC, FIX_LEN},
		{ 16, R_ASC, FIX_LEN},
		{ 42, R_ASC, FIX_LEN},
		{  1, R_BCD, FIX_LEN},
		{  2, R_BCD, FIX_LEN},
		{  5, R_BCD, FIX_LEN},
		{  7, R_BCD, FIX_LEN},
		{ 42, R_BCD, FIX_LEN},
		{  8, R_BCD, FIX_LEN},
		{ 16, R_BCD, FIX_LEN},
		{ 25, R_BCD, FIX_LEN},
		{ 11, R_ASC, LLVAR_LEN},
		{ 11, R_ASC, LLVAR_LEN},
		{ 17, R_BCD, LLVAR_LEN},
		{ 28, R_BCD, LLVAR_LEN},
		{ 28, R_BCD, LLVAR_LEN},
		{10,  R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{999, R_BCD, LLLVAR_LEN},
		{  8, R_BCD, FIX_LEN}
	};

	unsigned short stemp;
	unsigned short sAlen = 0, Alen;//收到的总的包的大小
	unsigned short msgCmd = 0;//消息类型
//	unsigned short dealCode=0;//交易处理码
	unsigned short sfi;
	unsigned int lc;
	unsigned char getbuff[300];
	int tmpI = 0;

	unsigned char *msgpoint;
	int ret = 0, i = 0;//,len=0;
//	unsigned char rdt[7];
	DEF_qpboc_8583Head *sl8583head;

	unsigned short nestpLen;//其它包的长度
	unsigned char *nestpP;//是否有重包，如果有就指向下一包。
#ifdef HTTP_HEAD
	char * start_p = NULL;
	char *ret_p = NULL;
#endif
	//	unsigned char MSGerrflag=0;

	// 	stsl8583msg_ack  msg_ack;		//消息确认指令 V2.0加
	// 	stsl8583msg_down *msg_down;
	//	unsigned short msg_type=0;
	//	unsigned int templen=0;
	//	unsigned int temp=0;

	//	stFAT_hisRec *hismsg;		//

	unsigned char KEK[16];//秘钥明文  由母pos来 
	unsigned char pinkeytemp[17] = "\0", pinkeycheck[4] = "\0";
	unsigned char mackeytemp[17] = "\0", mackeycheck[4] = "\0";
	unsigned char tdkeytemp[17] = "\0", tdkeycheck[4] = "\0";
	unsigned char keytemp[17] = "\0";
	nestpP = 0;
	nestpLen = 0;

	clr_wdt();

	MSG_LOG("QPBOC_DataDeal:%d\n", packLen);
	gSendOverTime = 0;
	memcpy((unsigned char*)&Alen, pakege + 1, 2);//数据域总长度
	//	unsigned char msgtype;
	memcpy((unsigned char*)&sAlen, pakege + 3, 2);//8583包从第3字节开始，帧总长度，//前三个字节是 帧类型和帧长度，和数据无关
	over_turn(2, (unsigned char*)&sAlen);
#if 0
#ifdef _debug_
	debugstring("QPBOC_DataDealdeal sl8583 CMD!\r\n");
	debugstring("Alen:");
	debugdata((unsigned char *)&Alen, 2, 1);
	debugstring("sAlen:");
	debugdata((unsigned char *)&sAlen, 2, 1);
	debugdata(pakege + 3, Alen, 1);
#endif
#endif


#ifdef HTTP_HEAD
	/*
	HTTP/1.1 500 Internal Server Error
	Allow: POST, PUT
	Content-Type: x-ISO-TPDU/x-auth
	Content-Length: 0
	Server:                       Access-Guard-1000-Software/1.0
	Connection: close

	xxxxxxxxx
	*/


	start_p = NULL;
	memset(getbuff, 0, sizeof(getbuff));

	memcpy(getbuff, pakege, sizeof(getbuff));

	start_p = NULL;
	start_p = strstr((char *)getbuff, "Content-Length:");
	if (start_p == NULL)
	{
		MSG_LOG("找不到http数据\r\n");
		return ST_OK;
	}

	ret_p = strtok(start_p, ":");
	ret_p = strtok(NULL, "\r\n");
	MSG_LOG("8583:len:%d,%s\r\n", strlen(ret_p), ret_p);


	Alen = __atol(ret_p);

	if (Alen == 0)
	{
		//后台给的头，但是没有数据，要退出 这个时候可能网络已经掉了
		MSG_LOG("找不到8583数据\r\n");
		return ST_OK;

	}


	////////////////////////////////////////////////////////////////




	start_p = NULL;
	start_p = strstr(pakege, "\r\n\r\n");		//之后就是数据了

	start_p += 4;



	memmove(pakege, start_p, Alen);
	MSG_LOG("8583数据\r\n");
	BCD_LOG((unsigned char *)pakege, Alen, 1);

	memcpy(pakege_8583, pakege, Alen);
	MSG_LOG("拷贝8583数据\r\n");
	BCD_LOG((unsigned char *)pakege_8583, Alen, 1);

#else
	pakege += 1;
	if (packLen == 0) {

		debugstring("GPRS error msg:");
		BCD_LOG((char *)pakege, 2, 1);
		ACK_flag = 0xAA;
		GPRSLinkProcess = GPRS_FREE;
		gmissflag = MISS_G_FREE;
		//return 1;
		return 1;
	}
	pakege += 2;
#endif

	if (s_isAuthOk == 0) {
		i = 0;
		GPRSLinkProcess = GPRS_FREE;
		if (memcmp(pakege + i, "\x00\x22", 2) != 0) {
			debugdata(pakege + 3, Alen, 1);
			return 1;
		}
		i += 2;
		// 应答成功：返回码固定00，应答描述：认证成功
		if (memcmp(pakege + i, "\x30\x30", 2) != 0) {
			debugstring("auth error msg:");
			debugstring((char *)pakege + i + 2);
			return 1;
		}

		//debugstring("union pay auth ok!!!\n");
		s_isAuthOk = 1;

		MSG_LOG("银联认证成功，继续签到---\r\n");
		ACK_flag = 0xff;
		//Sign_Infor.ISOK = 0;
		return ST_OK;
	}

	for (i = 0; i < 129; i++) {	//初始化
		msgf[i].bitf = 0;
		msgf[i].len = 0;
	}

	sl8583head = (DEF_qpboc_8583Head*)(pakege);//指向消息报文 TPDU 5字节

	MSG_LOG("sl8583head:");
	BCD_LOG(&sl8583head->ID, 11, 1);


	if (sl8583head->ID != 0x60) {
		MSG_LOG("TPDU ID error:%02X\r\n", sl8583head->ID);
		return 0;
	}

	memcpy((unsigned char*)&msgCmd, sl8583head->MSGtype, 2);//此包消息类型
	over_turn(2, (unsigned char *)&msgCmd);

	MSG_LOG("bitmap:");
	BCD_LOG(sl8583head->MSGbitmap, 8, 1);
	MSG_LOG("msgCmd:%04X\r\n", msgCmd);


	msgpoint = sl8583head->MSGbitmap;	//应用数据指针，从交易处理码
	msgpoint += 8;	//

//**********分析8583域******************************************************
//把数据清分到msgf中。
	for (sfi = 2; sfi <= 64; sfi++) {//从第4域开始（0~63）

		if (getBitmapBits(sfi, sl8583head->MSGbitmap) != ST_OK) {
			continue;//此位不存在，继续下一位
		}

		lc = iso8583[sfi - 1].len;

		if (iso8583[sfi - 1].flag == FIX_LEN)/*固定长度*/
		{
			;
		}
		else if (iso8583[sfi - 1].flag == LLVAR_LEN)/*两位变长 1Byte*/
		{
			lc = 0;
			lc = (((msgpoint[0] >> 4) * 10) + (msgpoint[0] & 0x0f));
			//			memcpy((unsigned char *)&lc, pakege+index, 2);//
			//			over_turn(2, (unsigned char*)&lc);
			msgpoint++;

		}
		else/*三位变长*/
		{
			lc = 0;
			lc = (((msgpoint[0] >> 4) * 10) + (msgpoint[0] & 0x0f));
			stemp = (((msgpoint[1] >> 4) * 10) + (msgpoint[1] & 0x0f));
			lc *= 100;
			lc += stemp;
			msgpoint += 2;
		}

		if (iso8583[sfi - 1].type == L_BCD)	/*左对齐BCD码*/
		{
			lc = (lc + 1) / 2;
		}
		else if (iso8583[sfi - 1].type == L_ASC)  /*左对齐ASC*/
		{
			lc = lc;
		}
		else if (iso8583[sfi - 1].type == R_BCD)	/*右对齐BCD码*/
		{
			lc = (lc + 1) / 2;
		}
		else if (iso8583[sfi - 1].type == R_ASC)	/*右对齐ASC码*/
		{
			lc = lc;
		}
		else if (iso8583[sfi - 1].type == BINARY)	/*右对齐ASC码*/
		{
			lc = lc;
		}


		//MSG_LOG("里面有%d域\r\n", sfi);

		if (lc >= 1000) {//长度错误
			MSG_LOG("--Fild:%d,len:%d,LEN ERROR!!!\r\n", sfi, lc);
			ACK_flag = 0xAA;
			goto pboc_8583dataDealEnd;
		}


		//		memcpy(getbuff, msgpoint, lc);
		msgf[sfi].bitf = 1;
		msgf[sfi].len = lc;
		msgf[sfi].dbuf_addr = msgpoint - pakege;

#ifdef debug_deal_8583_data
		if ((iso8583[sfi - 1].type == R_BCD) || (iso8583[sfi - 1].type == L_BCD))
		{
			MSG_LOG("Fild:%d,BCD:len:%d--\r\n", sfi, iso8583[sfi - 1].len);
		}
		else {
			MSG_LOG("Fild:%d,ASC:len:%d--\r\n", sfi, iso8583[sfi - 1].len);

		}
		MSG_LOG("--Fild:%d,len:%d,dat:", sfi, lc);
		BCD_LOG(pakege + msgf[sfi].dbuf_addr, lc, 1);//打印的是此域的数据
#endif

		msgpoint += lc;


		if (msgf[sfi].dbuf_addr > packLen) {//长度错误
			MSG_LOG("--Fild:%d,total len:%d,LEN too short!!!\r\n", sfi, packLen);
			goto pboc_8583dataDealEnd;
		}

#ifdef debug_deal_8583_data
		//	BCD_LOG(msgpoint, 16, 1);//打印的是此域的数据
#endif

	}

	MSG_LOG("\r\n");
	MSG_LOG("\r\n");



	if ((msgf[field_date].bitf) && (msgf[field_time].bitf)) {//有时间日期
		memcpy(getbuff, &SysTime.year_h, 2);

		memcpy(getbuff + 2, pakege + msgf[field_date].dbuf_addr, 4);
		memcpy(getbuff + 4, pakege + msgf[field_time].dbuf_addr, 3);
		MSG_LOG("后台时间:");
		BCD_LOG(getbuff, 7, 1);
		if (time_invalid(getbuff) == 1) {//时间有效，校时
			GetDateTime();
			//if(memcmp((unsigned char*)&SysTime.year_h, getbuff, 5) != 0){//分钟不一致，校时
			ret = BCDTime2Long(getbuff);
			i = BCDTime2Long(&SysTime.year_h);
			ret -= i;
			if (ret > 300 || ret < -300)
			{
				MSG_LOG("校时---\r\n");
				timewrite(getbuff);
// #ifdef _FASH_FATs_SYS_
// 				//如果时间不是今天，这个要重新打开或者新建一个文件，这里都打开一次
// 				close_rfile_Wrecord();
// 				open_rfile_Wrecord();
// #endif
			}
			// 			if (ret >86400 || ret <-86400)		//跨天了
			// 			{
			// 				MSG_LOG("校时过1天\r\n");
			// 				ReStartCPU();
			// 			}

						//}
		}
	}


	//**************************************************************************************
	if (msgf[field_ack].bitf == 0) {	//没有应答码，此包不处理。
		MSG_LOG("没有应答码。\r\n");
		goto pboc_8583dataDealEnd;	//没有应答码
	}


	Ascii2BCD(pakege + msgf[field_ack].dbuf_addr, getbuff, 2);


	MSG_LOG("8583p:bit2:");
	BCD_LOG(pakege_8583 + msgf[2].dbuf_addr, msgf[2].len, 1);
	BCD_LOG(pakege + msgf[2].dbuf_addr, msgf[2].len, 1);

#ifdef debug_deal_8583_data
	MSG_LOG("8583p:bit41:");
	BCD_LOG(pakege_8583 + msgf[41].dbuf_addr, msgf[41].len, 1);

#endif


	ACK_flag = getbuff[0];

	MSG_LOG("bit:%d,ACK_flag:%02X\r\n", msgf[field_ack].bitf, ACK_flag);


	switch (getbuff[0])
	{
	case 0x00:
		//	ACK_flag = 0;  //hbg
		MSG_LOG("应答码正确\n");
		break;
	case 0xA0:
		MSG_LOG("mac不对，重新签到---\r\n");
		ACK_flag = 0xAA;
		Sign_Infor.ISOK = 0;
		break;
	case 0x94:
		MSG_LOG("重复交易\r\n");

		ACK_flag_oda = 0;
		ACK_flag = 0;	//已经送上去一个了，不要再送了，直接显示扣款成功
		break;
	case 0x14:
		MSG_LOG("无效帐号交易14=>00\r\n");
		//银联二维码发现有这个错误提示，但是客户的账户被扣钱了，银联意思先放开
		ACK_flag = 0xAA;	//已经送上去一个了，不要再送了，直接显示扣款成功
		break;
	default:
		ACK_flag = 0xAA;
		// 		memset(getbuff,0,sizeof(getbuff));
		// 		sprintf((char *)getbuff,"返回错误码:%02X",ACK_flag);
		// 		MessageBox(1,(char *)getbuff);
		// 		delayxms(500);
		// 		restore_flag = 3;
		//		goto pboc_8583dataDealEnd;		
		break;
	}

	MSG_LOG("bit:%d,应答码:%c%c\r\n", msgf[field_ack].bitf, pakege[msgf[field_ack].dbuf_addr], pakege[msgf[field_ack].dbuf_addr + 1]);
	memcpy(getbuff, pakege + msgf[field_cmdcode].dbuf_addr, msgf[field_cmdcode].len);
	getbuff[msgf[field_cmdcode].len] = '\0';
	MSG_LOG("bit:%d,应答码:%s\r\n", msgf[field_cmdcode].bitf, getbuff);
//	MessageBox(1, (char*)getbuff);

	if(ACK_flag!=0)
	{
		MessageBox(1, (char*)getbuff);
		delayxms(1500);
	}
	MSG_LOG(" gj1.. ");
	clr_wdt();
	//后面都是成功的.


//	gSendGLogin = 0;//有应答，不发心跳包

	switch (msgCmd) {
	case 0x0810://签到的回应

		MSG_LOG("签到---\r\n");

		if (msgf[60].bitf)
		{
			memset(getbuff, 0, sizeof(getbuff));

			memcpy(getbuff, pakege + msgf[60].dbuf_addr + 1, 3);
			MSG_LOG("60域\r\n");
			BCD_LOG(pakege + msgf[60].dbuf_addr, msgf[60].len, 1);
			tmpI = GET_INT16(pakege + msgf[60].dbuf_addr + 1 + 3) & 0x0FFF0;
			//  ((buf)[0]<<8)|((buf)[1]&0x0FF)  57 00
			if (tmpI == 0x5700) {	// ODA黑名单下载 “570”//add hb
				if (ACK_flag == 0) {	//成功
					MSG_LOG("ODA黑名单下载\r\n");
					s_isDownOdaBlkList = 1;
					if (deal_qpoc_blk(pakege + msgf[63].dbuf_addr, msgf[63].len) == 1)
					{
						MSG_LOG("无黑名单下载\r\n");
						s_isDownOdaBlkList = 0;
					}
					else
						s_isDownOdaBlkList = 1;

				}
			}
			else if (tmpI == 0x3700)
			{
				//公钥下载

			}
			else {
				MSG_LOG("批次号:");
				BCD_LOG(getbuff, 3, 1);
				//saveTradeBatchinfo(getbuff, 0, 0);
				BCD2Ascii(getbuff, getbuff + 50, 3);
				lc = __atol((char *)(getbuff + 50));
				MSG_LOG("存储批次号:%d", lc);
				sysfewrite(BIT_PBOC_NS_BIG, 4, (unsigned char*)&lc);
			}
		}

		if (msgf[62].bitf)
		{
			getMobileParameter(2, KEK);
			MSG_LOG("(char*)smpPara.KEK:\n");
			BCD_LOG(KEK, 16, 1);

			memset(getbuff, 0, sizeof(getbuff));
			memcpy(pinkeytemp, pakege + msgf[62].dbuf_addr, 16);
			memcpy(pinkeycheck, pakege + msgf[62].dbuf_addr + 16, 4);

			memcpy(mackeytemp, pakege + msgf[62].dbuf_addr + 20, 8);
			//memcpy(mackeycheck, pakege + msgf[62].dbuf_addr + 28, 4); // 湖南银联取值与上海银联总部不一样
			memcpy(mackeycheck, pakege + msgf[62].dbuf_addr + 20 + 16, 4); // 湖南银联取值与上海银联总部不一样

			//memcpy(tdkeytemp, pakege + msgf[62].dbuf_addr + 40, 16);
			//memcpy(tdkeycheck, pakege + msgf[62].dbuf_addr + 56, 4);


			MSG_LOG("PIN 密文:");
			BCD_LOG(pinkeytemp, 16, 1);

			DES3_decrypt(pinkeytemp, KEK, keytemp);
			DES3_decrypt(pinkeytemp + 8, KEK, keytemp + 8);

			MSG_LOG("解密:scr:");
			BCD_LOG(keytemp, 16, 1);
#if 1
			memset(getbuff, 0, sizeof(getbuff));
			DES3_encrypt(getbuff, keytemp, getbuff);

			MSG_LOG("用明文加密8个0算校验:scr:");
			BCD_LOG(getbuff, 4, 1);
			BCD_LOG(pinkeycheck, 4, 1);

			if (memcmp(getbuff, pinkeycheck, 4) != 0)
			{
					MessageBox(1, "银联密钥验证不过-1,请检查设备号");
					delayxms(1000);
				MSG_LOG("PIN mac error!\r\n");
					add_qpoc_flag();
				break;
			}
#endif
			memcpy(pinkeytemp, keytemp, 16);//拷贝为明文


			MSG_LOG("MAK 密文:");
			BCD_LOG(mackeytemp, 16, 1);

			DES3_decrypt(mackeytemp, KEK, keytemp);
			// DES3_decrypt(mackeytemp+8 ,KEK,keytemp+8);

			MSG_LOG("解密mak只用前8:scr:");
			BCD_LOG(keytemp, 16, 1);
#if 1
			memset(getbuff, 0, sizeof(getbuff));
			DES_encrypt(getbuff, keytemp, getbuff);

			MSG_LOG("用明文加密8个0算校验:scr:");
			BCD_LOG(getbuff, 4, 1);
			BCD_LOG(mackeycheck, 4, 1);

			if (memcmp(getbuff, mackeycheck, 4) != 0)
			{
				delayxms(500);
					MessageBox(1, "银联密钥验证不过-2,请检查设备号!");
				MSG_LOG("mak mac error!!!!!!!\r\n");
					add_qpoc_flag();
				break;
			}
#endif
			memcpy(mackeytemp, keytemp, 16);//拷贝为明文

#if 0
			MSG_LOG("TDK 密文:");
			BCD_LOG(tdkeytemp, 16, 1);

			DES3_decrypt(tdkeytemp, KEK, keytemp);
			DES3_decrypt(tdkeytemp + 8, KEK, keytemp + 8);

			MSG_LOG("解密:scr:");
			BCD_LOG(keytemp, 16, 1);

			memset(getbuff, 0, sizeof(getbuff));
			DES3_encrypt(getbuff, keytemp, getbuff);

			MSG_LOG("用明文加密8个0算校验:scr:");
			BCD_LOG(getbuff, 4, 1);
			BCD_LOG(tdkeycheck, 4, 1);

			if (memcmp(getbuff, tdkeycheck, 4) != 0)
			{
				MSG_LOG("TDK mac error!\r\n");
				break;
			}
			memcpy(tdkeytemp, keytemp, 16);//拷贝为明文

#endif

			MSG_LOG("PIN MAK TDK 下载完成--\r\n");



			memcpy(Sign_Infor.SingInDate, &SysTime, 7);
			memcpy(Sign_Infor.PIN_KEY, pinkeytemp, 16);
			memcpy(Sign_Infor.PIN_KEY_CHECKVALUE, pinkeycheck, 4);
			memcpy(Sign_Infor.MAC_KEY, mackeytemp, 16);
			memcpy(Sign_Infor.MAC_KEY_CHECKVALUE, mackeycheck, 4);
			memcpy(Sign_Infor.TDK_KEY, tdkeytemp, 16);
			memcpy(Sign_Infor.MAC_KEY_CHECKVALUE, tdkeycheck, 4);

			saveSingInInfo();
			// 上海银联只要求到60域就签到成功了
			Sign_Infor.ISOK = 'O';	//置签到成功标识
			gmissflag = MISS_G_FREE;
			restore_flag = 3;

			// 下载黑名单
			if (s_isDownOdaBlkList == 1) {
				gmissflag = MISS_PBOC_DOWN_ODA_BLK;
			}
		}

		// 下载黑名单   hbg
		restore_flag = 3;
		if (s_isDownOdaBlkList == 1) {
			gmissflag = MISS_PBOC_DOWN_ODA_BLK;
		}
		else
			gmissflag = MISS_G_FREE;

		if (GPRSLinkProcess == GPRS_SENDING_CMD)
			GPRSLinkProcess = TCPSTARTSTAT;
		break;

		// 	case 0x0830://终端签退回应	
		// 		MSG_LOG("签退---\r\n");	
		// 
		// 		gmissflag = MISS_G_FREE;
		// 		if(GPRSLinkProcess == GPRS_SENDING_CMD)
		// 			GPRSLinkProcess = TCPSTARTSTAT;
		// 		break;

	case 0x0210:
		MSG_LOG("交易返回=0x0210=\r\n");
		MSG_LOG("gmissflag==%x", gmissflag);
		MSG_LOG("ACK_flag==%x", ACK_flag);


		if (gmissflag == MISS_PBOC_UPREC_ODA && (ACK_flag == 0)) {
			if (msgf[14].bitf == 1 && ACK_flag_oda != 0)//ODA加个判断保险
			{
				MSG_LOG("ODA加个判断保险\r\n");
				gmissflag = MISS_G_FREE;
				ACK_flag_oda = 0xff;
				restore_flag = 3;

				if (GPRSLinkProcess == GPRS_SENDING_CMD)
					GPRSLinkProcess = TCPSTARTSTAT;
				break;
			}
			PAY_MODE_SWITCH(shuangmian, switch_both);//双通道切换
			MSG_LOG("上传ODA交易记录返离开的\r\n");
			gmissflag = MISS_G_FREE;

			save_ODA_infor(FeRC_Dlelt, NULL);
			restore_flag = 3;

			if (GPRSLinkProcess == GPRS_SENDING_CMD)
				GPRSLinkProcess = TCPSTARTSTAT;
		}
		//add hbg
		else if (ACK_flag == 0)
		{
			gmissflag = MISS_G_FREE;

			restore_flag = 3;

			if (GPRSLinkProcess == GPRS_SENDING_CMD)
				GPRSLinkProcess = TCPSTARTSTAT;
		}
		if (GPRSLinkProcess == GPRS_SENDING_CMD)
			GPRSLinkProcess = TCPSTARTSTAT;
		MSG_LOG("gmissflag 2==%x", gmissflag);
		break;

	case 0x0410:	//冲正返回

		MSG_LOG("冲正返\r\n");
		gmissflag = MISS_G_FREE;
		// 		del_repurse(pakege+ msgf[11].dbuf_addr);

		//memset(repurse_infor, 0, sizeof(repurse_infor));
		save_repurse_infor(FeRC_Dlelt, NULL);

		restore_flag = 3;

		if (GPRSLinkProcess == GPRS_SENDING_CMD)
			GPRSLinkProcess = TCPSTARTSTAT;
		break;


	case 0x0830:	//返回

		MSG_LOG("查询\r\n");
		gmissflag = MISS_G_FREE;

		if (msgf[63].bitf)
		{

			MSG_LOG("63域\r\n");
			BCD_LOG(pakege + msgf[63].dbuf_addr, msgf[63].len, 1);
		}
		restore_flag = 3;

		if (GPRSLinkProcess == GPRS_SENDING_CMD)
			GPRSLinkProcess = TCPSTARTSTAT;
		break;
	default:



		break;


	}

	clr_wdt();
pboc_8583dataDealEnd:
	MSG_LOG(" gjend..%d ", nestpLen);
	if ((nestpP != 0) && (nestpLen > 0))//有重包，需要移到前面
	{
		pakege[0] = 0x01;//还有数据
		memcpy(pakege + 1, (unsigned char*)&nestpLen, 2);//剩余的长度
		memmove(pakege + 3, nestpP, nestpLen);
		return 1;
	}
	else
		return 0;
}



extern unsigned char PeekFrame(void);

void find_qpboc_new_mission(void)//此任务一秒进一次
{
	unsigned char frameIndex = 0xFF;
	//	unsigned int iver;
		//	unsigned short usTemp=0;
	//	unsigned char i;//, flag;
	//  	unsigned char btf[20];
	//	static unsigned char link_check=0;

	//	MSG_LOG("do %s\r\n",__FUNCTION__);

	//MSG_LOG("%04X", gErrorFlag);

	if (gErrorFlag&ERROR_BUS_CONNECT) {
#ifdef _debug_
		// 		sprintf((char*)btf, "公:%u", gErrortimes[1]);
		// 		debugstring((char*)btf);
#endif
		MSG_LOG("E");

		if (gErrortimes[1] > 0) {
			if ((gmissflag & MISS_GJ) != 0)
				gmissflag = 0;
			return;//上次连接错误,时间没到不给任务.
		}
	}

	gErrorFlag &= (~ERROR_BUS_CONNECT);// 清除错误标识


	frameIndex = PeekFrame();
	if (frameIndex != 0xff) {	// 先处理GPRS数据
		MSG_LOG("GPRS有数据银联后台,先处理数据:%d\n", frameIndex);
		return;
	}
	if ((gmissflag) != MISS_G_FREE)
	{
		MSG_LOG("公交有任务\r\n");
		return;
	}
	// 	if ((isNetOK[LINK_POBC] !=3) &&(GPRSLinkProcess == TCPSTARTSTAT))
	// 	{
	// 		MSG_LOG("去连接银联\r\n");
	// 		GPRSLinkProcess = 23;		//
	// 		return ;
	// 	}
if(get_qpoc_first()!=0xad)
{
		MSG_LOG("没有下载秘钥\r\n");
	return;
}
	if (Sign_Infor.ISOK == 0) {//还没有签过到
		if (gmissflag == MISS_PBOC_LOGIN)
			return;	//已经是签到状态了。
#ifdef debug_deal_8583_data
		debugstring("银联没有签到！:");
#endif
		s_isAuthOk = 1;
		gmissflag = MISS_PBOC_LOGIN;
		if (GPRSLinkProcess == GPRS_SENDING_CMD)
			GPRSLinkProcess = TCPSTARTSTAT;
		return;
	}

	if (get_repurse_num() == ST_OK)
	{
		MSG_LOG("要冲正---\r\n");
		gmissflag = MISS_PBOC_RE_PURSE;
		return;//add hbg
	}
// 	if (save_ODA_infor(ODA_FeRC_Check, NULL) != Ret_NO) {
// 		MSG_LOG("要上传ODA交易---\r\n");
// 		gmissflag = MISS_PBOC_UPREC_ODA;
// 	}
	// 	gSendGLogin++;
	// 	if(gSendGLogin > SEND_PIN_COUNT_){
	// 		gSendGLogin = 0;
	// 		//			gsl8583Style.ISOK = 0;	//重签到
	// 		gmissflag = MISS_G_TOO;
	// 		GPRSLinkProcess =TCPSTARTSTAT;
	// 	}
	// 	
}



#if SWITCH_PBOC
extern unsigned char shuangmian;
#endif
extern unsigned char write_linux_re_build(void);
//SQmode:申请类型 1=申请CPU卡金额，2申请卡信息, 3挂失，4解挂
int SQDataFromSVT(unsigned char SQmode, int msecends)
{
	//	unsigned short us1;
	unsigned char flag, ret;
	unsigned int outdly = 0;
	//	unsigned int i=0;
		//unsigned long long read_dat=0;
		//unsigned int count=0;
	unsigned char disbuff[20];
	//	BER_TVL TempTVL;

#if 1
	msecends = 7500;
#endif

	if (msecends > 0) {
		flag = 0;
		if (gMCardCand != CARDSTYLE_UNPAY_ODA)
		{
			cls();
			display(6, 0, "通讯中...", DIS_ClsLine | DIS_CENTER);
			display(8, 0, "请稍等", DIS_ClsLine | DIS_CENTER);
		}



		MSG_LOG("do %s:0x%02X\r\n", __FUNCTION__, SQmode);
		MSG_LOG("set_timer0(10000,2)--\r\n");

		set_timer0(msecends, 3);
	}
	memset(irda_rxBuf, 0, 22);

	memset(pakege_8583, 0, sizeof(pakege_8583));


	if (Sign_Infor.ISOK == 0)
	{
		MSG_LOG("没签到--return--\r\n");
		gmissflag = MISS_PBOC_LOGIN;
		ACK_flag = 0;
		MessageBox(1, "设备未签到,请稍后");
		delayxms(500);
		set_timer0(0, 3);
		return -3;

	}

	find_qpboc_new_mission();
	MSG_LOG("找任务-\r\n");
	ACK_flag = 0xFF;

	//	p = NULL;


	while (1) {//开始申请
		clr_wdt();
#ifdef WIFI_TLH_
		wifiTlh_main();
#else
		main_GpsRs();
#endif


		outdly = get_timer0(3);
		if (outdly == 0)
		{
			return -2;	// 超时重刷
		}
		ret = getkey(1);
		if (ret == KEY_ESC) {
			if (GPRSLinkProcess == 0xA0)
				GPRSLinkProcess = TCPSTARTSTAT;
			gmissflag = 0;
			return -1;
		}
		if (isNetOK[LINK_PBOC] == 3)
		{
			GPRSLinkProcess = TCPSTARTSTAT;
		}
		if (GPRSLinkProcess < 21) {//已经连接
			if (flag != 1) {
				flag = 1;
				display(6, 0, "正在拔号", DIS_ClsLine | DIS_CENTER);
			}
			//set_timer0(5000,2);	

			continue;
		}
		else if (GPRSLinkProcess == 0x2F) {
			if (flag != 1) {
				flag = 1;
				display(6, 0, "正在重新连接", DIS_ClsLine | DIS_CENTER);
			}
			//set_timer0(5000,2);	

			continue;
		}

		// 		memcpy((unsigned char*)&us1, gBusVerInfo.newbusBLKVer, 2);
		// 		if((us1 ==  0)||(us1 ==  0xffff)){//服务器的版本号.还没有登陆
		// 			if(flag != 2){
		// 				flag = 2;
		// 				display(4, 0, "正在登陆", DIS_ClsLine|DIS_CENTER);
		// 			}
		// 			continue;
		// 		}

		if (flag != 3) {
			flag = 3;
			if (SQmode == MISS_PBOC_PURSE)
			{
				if (gMCardCand == CARDSTYLE_UNPAY_ODA) {
					;//	display(6, 0, "正在发送ODA消费记录", DIS_ClsLine | DIS_CENTER);
				}
				else
					display(4, 0, "正在发送...", DIS_ClsLine | DIS_CENTER);
					display(6, 0, "乘客可以拿开手机或卡片", DIS_ClsLine | DIS_CENTER);

			}
			else if (SQmode == MISS_PBOC_RE_PURSE)
			{
				if (gMCardCand == CARDSTYLE_UNPAY_ODA) {
					display(6, 0, "正在发送ODA消费记录", DIS_ClsLine | DIS_CENTER);
				}
				else {
					display(6, 0, "正在发送冲正申请", DIS_ClsLine | DIS_CENTER);
				}
			}
		}

		if (get_repurse_num() == ST_OK &&shuangmian == 1)
		{
			MSG_LOG("要冲正-22222-\r\n");
			//gmissflag = MISS_PBOC_RE_PURSE;
			ACK_flag = 0xFE;
			return -2;
		}
		gmissflag = SQmode;


#ifdef _debug_
		MSG_LOG("赶紧发--overtime=0--:0x%02X-\r\n", gmissflag);
#endif

		GPRSLinkProcess = TCPSTARTSTAT;
		flag = 0;
		gSendOverTime = 0;
		gGPRS_data_style.ucReadStyle = 0;
		gGPRS_data_style.ucSendstyle = 0;
		gTcpIpbusy = TcpIPFree;
		//ADD hbg
		ACK_flag = 0xFF;
		MSG_LOG("gMCardCand:%d\n", gMCardCand);
		while (1) {
			clr_wdt();
			//			i = get_timer0(1);
			outdly = get_timer0(3);






			//

			// 			memset(disbuff,0,sizeof(disbuff));
			// 			sprintf((char *)disbuff,"T1:%04d",i);
			// 			miniDispstr(6,30,(const char *)disbuff,0);

			if (gMCardCand != CARDSTYLE_UNPAY_ODA) {
				memset(disbuff, 0, sizeof(disbuff));
				sprintf((char *)disbuff, "%dS", outdly / 1000);
				display(8, 16, (const char *)disbuff, 0);
			}

			/*

						if(GPRSLinkProcess == GPRS_SENDING_CMD){

						//	if ((read_dat++ > 65530*3) && (gGPRS_data_style.ucReadStyle == GPRS_SISR))
							if (read_dat++ > 65530*3)
							{

								read_dat = 0;
								if (count ++ >5)
								{
									MSG_LOG("再发试试\r\n");
									count = 0;
									gmissflag = 0;
									find_qpboc_new_mission();
								}
								ALL4G_SISREAD(gGPRS_data_style.link);

							}
						}
			*/



			if (gmissflag != SQmode) {
				gmissflag = SQmode;
			}

#ifdef WIFI_TLH_
			wifiTlh_main();
#else
			main_GpsRs();
#endif
// 			//#if SWITCH_PBOC
// 			if (switch_both)
// 			{
// 
// 				if (outdly < 900) {	// 考虑耗时4s后下笔进入ODA交易
// 					shuangmian = 0;
// 				}
// 				else {	// 
// 					shuangmian = 1;
// 				}
// 			}

			//#endif
			if (outdly == 0)
			{
				MSG_LOG("time out-3-\r\n");
			//	tcpipClose(LINK_PBOC);
				gmissflag = 0x4f;
				if (GPRSLinkProcess == 0xA0)
					GPRSLinkProcess = TCPSTARTSTAT;
			//	gmissflag = 0;
				write_linux_re_build();
				return -2;
			}




			if (ACK_flag != 0xFF)
			{
				// 接受交易
				set_timer0(0, 3);

				//MSG_LOG("444bit:%d,ACK_flag:%02x   ",msgf[field_ack].bitf,ACK_flag);
				if ((ACK_flag == 0x00) && (msgf[field_ack].bitf)) {
					flag = 0xA5;
					break;
				}
				else {
					MSG_LOG("%s,应答:%02X\r\n", __FUNCTION__, ACK_flag);
					MSG_LOG("444bit:%d,ACK_flag:%02x   ", msgf[field_ack].bitf, ACK_flag);
					if (GPRSLinkProcess == 0xA0)
						GPRSLinkProcess = TCPSTARTSTAT;
					gmissflag = 0x4f;
					gSendOverTime = 0;
					write_linux_re_build();
					return -1;	//
				}

				// 				if (gMCardCand == CARDSTYLE_UNPAY_ODA&&ACK_flag_oda !==0) {
				// 					MSG_LOG("删ODA858记录--:%d\r\n", ACK_flag);
				// 					save_ODA_infor(ODA_FeRC_Dlelt, NULL);
				// 					
				// 					MSG_LOG("%s,应答:%02X\r\n", __FUNCTION__, ACK_flag);
				// 					if (GPRSLinkProcess == 0xA0)
				// 						GPRSLinkProcess = TCPSTARTSTAT;
				// 				//	gmissflag = 0;
				// 					gSendOverTime = 0;
				// 					
				// 					return 0;	//
				// 				}
				// 			

			}
			else {
				ret = getkey(1);
				if (ret == KEY_ESC) {
					gmissflag = 0;

					return -1;
				}
			}
		}

		// 		if(outdly++ > 200)
		// 			return;
		// 		delayxms(50);
		// 		clr_wdt();

		if (flag == 0xA5 && gMCardCand != CARDSTYLE_UNPAY_ODA) {	// 收到正确数据
			if (GPRSLinkProcess == 0xA0)
				GPRSLinkProcess = TCPSTARTSTAT;
			gmissflag = 0x4f;
			gSendOverTime = 0;

			MSG_LOG("删冲正--\r\n");
			//memset(repurse_infor, 0, sizeof(repurse_infor));
			save_repurse_infor(FeRC_Dlelt, NULL);
       
			pboc_free_cnt = PBOC_FREE_CNT;

			MSG_LOG("返回正常:%02x\r\n", flag);

			return 0;
		}
	}
	return 0;
}

//BASE64加密，输出在base64
extern char *base64_encode(const unsigned char * bindata, char * base64, int binlength);
//BASE64解密，输入base64为字符串\0结束,返回输出的长度
extern int base64_decode(const char * base64, unsigned char * bindata);
//银联钱包二维码
int qpboc_qr_main(char *QRCdat, unsigned char *Rdata)
{
	unsigned char cardlen = 0;
	unsigned char buffer[32];
	//unsigned char recordbuff[RECORD_LEN];
	unsigned char qr_infor[128], qrinfor_len = 0;	//
	int ret = -1;


	memset(qr_pboc_AccountNo, 0, sizeof(qr_pboc_AccountNo));
	strcpy((char *)qr_pboc_AccountNo, QRCdat);

	cls();
	display(0, 0, "交易码:", 0);

	gMCardCand = CARDSTYLE_QR_PBOC;
	
#ifdef card_type_qpboc
	cardMessage.card_catalog = CARD_QR_PBOC;
#else
	cardMessage.card_catalog = CARD_QPBOC_BUS;
#endif

	s_sum1 = get_s_sum1(0);
// #ifdef _debug_
// s_sum1=1;
// #endif
#ifndef _debug_
	if((s_sum1 == 0) || (s_sum1 > 2000))
	{
//		if(gchn_eng == 'E'){
//			display(0,0,"hint:", DIS_Cls);
//			display(2,0,"price is error",DIS_CENTER);
//		}
//		else{
			display(0,0,"提示:", DIS_Cls);
			display(2,0,"票价>20元",DIS_CENTER);
//		}
		delayxms(1500);
	//	g24GDisFlash = 1;
		restore_flag = 3;
	
	}

#endif

	
	if ((NewPriceTab.rate[cardMessage.card_catalog - 0x40] == 104) || (NewPriceTab.rate[cardMessage.card_catalog - 0x40] == 204)) {

		audio(Audio_FLING_MONEY);	//请投币
		MessageBox(1, "此卡禁止刷卡!");
		led_on(LED_RED);
		restore_flag = 3;
		end_card();

		return ST_OK;
	}
	cardlen = strlen((const char *)qr_pboc_AccountNo);
	memcpy(buffer, &qr_pboc_AccountNo[cardlen - 8], 8);
	Ascii2BCD(buffer, (unsigned char *)&c_serial, 8);
#ifndef _debug_
	if (month_decide()) // 第二次去扣钱包(次数不能连刷)
	{
		disp_no_swipe();
		return ST_OK;
	}
#endif
	MSG_LOG("交易码:%s\r\n", qr_pboc_AccountNo);

	//miniDispstr(0,0,(char *)qrcodeRevBuf+2,0);
	display(2, 0, (char *)qr_pboc_AccountNo, 0);
emv_set_pay_channel(PAY_Online);
	ret = SQDataFromSVT(MISS_PBOC_PURSE, 6000);

	MSG_LOG("SQDataFromSVT:ret=%d\r\n", ret);
	if (ret == 0)
	{

		money_msg(ID_REC_TOLL, -1, s_sum1, 0);
		SoundMessage(SOUND_DEAL);

		//memset(recordbuff, 0, sizeof(recordbuff));
		memset(qr_infor, 0, sizeof(qr_infor));
		qrinfor_len = 0;

		cardlen = strlen((const char *)qr_pboc_AccountNo);
		qr_infor[qrinfor_len++] = cardlen;
		memcpy(qr_infor + qrinfor_len, qr_pboc_AccountNo, cardlen);
		qrinfor_len += cardlen;

		getMobileParameter(6, (unsigned char *)qr_infor + qrinfor_len);	//银联设备号
		qrinfor_len += 8;

		getMobileParameter(1, qr_infor + qrinfor_len);	//银联商户号
		qrinfor_len += 15;

		memset(buffer, 0, sizeof(buffer));
		sprintf((char*)buffer, "%06ui", POS_8583_COUNT);
		MSG_LOG("pos流水:%s\r\n", buffer);

		memcpy(qr_infor + qrinfor_len, ASC2BCD((char*)buffer, 6), 3);	//系统跟踪号
		qrinfor_len += 3;
		SYSgetbussinessNO(qr_infor + qrinfor_len);//两字节商户编号
		qrinfor_len += 2;
		//, unsigned char *Rdata
	//	build_YZFQRC_Record(ID_REC_QRC_PBOC, Rdata, qr_infor, qrinfor_len);
		//QTXQRCWriteRecord(recordbuff);
		BuildQRCRecorde(ID_REC_QRC_PBOC, Rdata, qr_infor, qrinfor_len);
		save_h_month();
		SetTcpPacketTTLTime(RECORD_TTL);
		set_delay_send(set_delay_send_val); //延时找任务
		MSG_LOG("qpboc_qr_main:%d\n", 256);
		return 256;

	}
	else
	{
		if (ACK_flag != 0)
		{
			//SoundMessage(SOUND_FLING_MONEY);
			dis_qboc_ack_code(ACK_flag);
		}


		delayxms(200);
		restore_flag = 3;


	}

	return -8;
}



//存入
//PiCiNo:当前批次号3 + 流水号3
//addRec:当前需要增加的条数
//addMoney: 当前批次需要增加的金额
//函数返回
//-1 : 批次号不正确
// 0 : 成功
int saveTradeBatchinfo(unsigned char *PiCiNoLS, unsigned int addRec, unsigned int addMoney)
{
	stTradeBatchinfo stbInfo;
	unsigned int itt;
	MSG_LOG("PiCiNo:%02x %02x %02x \n", PiCiNoLS[0], PiCiNoLS[1], PiCiNoLS[2]);
	MSG_LOG("LSNo:%02x %02x %02x \n", PiCiNoLS[3], PiCiNoLS[4], PiCiNoLS[5]);
	sysferead(BIT_TradeBatchinfo, sizeof(stTradeBatchinfo), (unsigned char*)&stbInfo);
	itt = __cpuCrc32((unsigned char*)&stbInfo, sizeof(stTradeBatchinfo) - 4);
	if (itt != stbInfo.CrcCheck) {//第一次上传
		memcpy(stbInfo.UpDataTime, (unsigned char*)&SysTime, 7);
		memset(stbInfo.DLushuiNo, 0, 3);
		memcpy(stbInfo.posPiCiNo, PiCiNoLS, 3);
		stbInfo.MoneySum = 0;
		stbInfo.RecordSum = 0;
		//MSG_LOG("save tbi CRC ERROR!\r\n");
	}

	if (addRec != 0) {
		// 		if(memcmp(stbInfo.posPiCiNo, PiCiNoLS, 3) != 0){ //批次号不一致，需要处理.
		// 			return -1;
		// 		}
	}

	if (memcmp(stbInfo.DLushuiNo, PiCiNoLS, 3) == 0) {//流水相同
		return 1;
	}

	stbInfo.MoneySum += addMoney;
	stbInfo.RecordSum += addRec;

	stbInfo.CrcCheck = __cpuCrc32((unsigned char*)&stbInfo, sizeof(stTradeBatchinfo) - 4);
	//	MSG_LOG("END:\n");

	sysfewrite(BIT_TradeBatchinfo, sizeof(stTradeBatchinfo), (unsigned char*)&stbInfo);
	return 0;
}

//读批次保存信息
void getTradeBatchinfo(unsigned char *oVal)
{
	stTradeBatchinfo oinfo;
	unsigned int itt;

	sysferead(BIT_TradeBatchinfo, sizeof(stTradeBatchinfo), (unsigned char*)&oinfo);
	itt = __cpuCrc32((unsigned char*)&oinfo, sizeof(stTradeBatchinfo) - 4);
	// 	debugstring("getTinfo:");
	// 	debugdata((unsigned char*)&oinfo,sizeof(stTradeBatchinfo), 1); 
	if (itt != oinfo.CrcCheck) {
		memset((unsigned char*)&oinfo, 0, sizeof(stTradeBatchinfo));
	}
	memcpy(oVal, (unsigned char*)&oinfo, sizeof(stTradeBatchinfo));
	return;
}
/*
mode=1: shopNo[15];//商户编号
mode=2: KEK[48];//KEK 16字节
*/

unsigned char getMobileParameter(unsigned char mode, unsigned char *obuf)
{

	stMobileParameter smpPara;
	unsigned int itemp;
	
	if (mode == 1) {	// 固定死
		memcpy(obuf, UNPAY_MERCHANTID, 15);
		return ST_OK;
	}
	else if(mode == 3)
	{
		itemp=0xDA0183B6;
		memcpy(obuf, (unsigned char *)&itemp, 4);
		
		return ST_OK;
	}
	else if(mode == 4)
	{
		itemp=7788;
		memcpy(obuf, (unsigned char *)&itemp, 4);
		return ST_OK;
	}
	sysferead(BIT_qpbpc_para, sizeof(stMobileParameter), (unsigned char *)&smpPara);
	//BCD_LOG((unsigned char *)&smpPara, sizeof(stMobileParameter), 1);
#if 0
	{
		char buffer[80];
		sysferead(BIT_qpbpc_para, 80, buffer);
		BCD_LOG(buffer, 80, 1);
	}
#endif

	itemp = __cpuCrc32((unsigned char*)&smpPara, (sizeof(stMobileParameter) - 4));
	if (memcmp((unsigned char*)&itemp, smpPara.CrcCheck, 4) != 0) {//校验错
		clr_wdt();
		MSG_LOG("getMobileParameter R:-1:%08X\n", itemp);
		flashread(FLASH_PBOC_PARA, (unsigned char *)&smpPara, sizeof(stMobileParameter));
		itemp = __cpuCrc32((unsigned char*)&smpPara, (sizeof(stMobileParameter) - 4));
		if (memcmp((unsigned char*)&itemp, smpPara.CrcCheck, 4) != 0) {//校验错
			return ST_ERROR;
		}
		MSG_LOG("覆盖flash 2 fe\r\n");
		sysfewrite(BIT_qpbpc_para, sizeof(stMobileParameter), (unsigned char *)&smpPara);

	}

	switch (mode) {
	case 1:
		memcpy(obuf, smpPara.shopNo, 15);
		MSG_LOG("商户号=%s\r\n",obuf);
		BCD_LOG(obuf, 156, 1);
		break;
	case 2:
		memcpy(obuf, smpPara.KEK, 16);

		break;
	case 3:
		memcpy(obuf, smpPara.ip, 4);
		break;
	case 4:
		memcpy(obuf, smpPara.port, 4);
		break;
	case 5:
		memcpy(obuf, smpPara.tpdu, 5);
		break;
	case 6:
		memcpy(obuf, smpPara.device, 8);
		break;

	case 7:
		memcpy(obuf, smpPara.AUTHKEY, 16);
		break;

	case 8:
		obuf[0] = smpPara.functionQr;
		//memcpy(obuf, smpPara.functionQr, 1);
		break;
	default:
		MSG_LOG("参数不对\r\n");
		return ST_ERROR;
	}
	return ST_OK;
}

//-------参数 stMobileParameter--------------
/*
mode=1: shopNo[15];//商户编号
mode=2: KEK[48];//KEK 16字节
*/
void saveMobileParameter(unsigned char mode, unsigned char *buf)
{
	stMobileParameter smpPara;
	unsigned int itemp;

	MSG_LOG("%s,%d\r\n", __FUNCTION__, mode);
	sysferead(BIT_qpbpc_para, sizeof(stMobileParameter), (unsigned char *)&smpPara);

	itemp = __cpuCrc32((unsigned char*)&smpPara, (sizeof(stMobileParameter) - 4));
	if (memcmp((unsigned char*)&itemp, smpPara.CrcCheck, 4) != 0) {//校验错
		memset((unsigned char*)&smpPara, 0, sizeof(stMobileParameter));
	}

	switch (mode) {
	case 1:
				MSG_LOG("商户号=%s\r\n",buf);
		BCD_LOG(buf, 156, 1);
		memcpy(smpPara.shopNo, buf, 15);
		break;
	case 2:
		MSG_LOG("明文KEY:");
		BCD_LOG(buf, 16, 1);
		memcpy(smpPara.KEK, buf, 16);
		break;
	case 3:
		memcpy(smpPara.ip, buf, 4);
		break;
	case 4:
		memcpy(smpPara.port, buf, 4);
		break;
	case 5:
		memcpy(smpPara.tpdu, buf, 5);
		break;
	case 6:
		memcpy(smpPara.device, buf, 8);
		break;
	case 7:
		memcpy(smpPara.AUTHKEY, buf, 16);
		break;
	case 8:
		smpPara.functionQr = buf[0];
		//memcpy(smpPara.functionQr, buf, 1);
		break;
	default:
		return;
	}

	itemp = __cpuCrc32((unsigned char*)&smpPara, (sizeof(stMobileParameter) - 4));
	memcpy(smpPara.CrcCheck, (unsigned char*)&itemp, 4);
	sysfewrite(BIT_qpbpc_para, sizeof(stMobileParameter), (unsigned char *)&smpPara);

	flasherase(ERASE4KBYTESECTOR, FLASH_PBOC_PARA);
	flashwrite(FLASH_PBOC_PARA, (unsigned char *)&smpPara, sizeof(stMobileParameter));

}

void load_pboc_8583para(void)
{
	stMobileParameter smpPara, smpPara_flash;
	unsigned int itemp;
	unsigned feerror = 0, flasherror = 0;
	unsigned char buff[4096];
	const char carcert[] = "CERTIFICATE";
	MSG_LOG("do %s\r\n", __FUNCTION__);
	// 	pbpc_para_error = 0;
	sysferead(BIT_qpbpc_para, sizeof(stMobileParameter), (unsigned char *)&smpPara);

	itemp = __cpuCrc32((unsigned char*)&smpPara, (sizeof(stMobileParameter) - 4));
	if (memcmp((unsigned char*)&itemp, smpPara.CrcCheck, 4) != 0) {//校验错
		//printf("getMobileParameter R:-1\n");
		feerror = 1;

	}

	MSG_LOG("FLASH_PBOC_PARA:%04X\r\n", FLASH_PBOC_PARA);

	flashread(FLASH_PBOC_PARA, (unsigned char *)&smpPara_flash, sizeof(stMobileParameter));
	itemp = __cpuCrc32((unsigned char*)&smpPara_flash, (sizeof(stMobileParameter) - 4));
	if (memcmp((unsigned char*)&itemp, smpPara_flash.CrcCheck, 4) != 0) {//校验错
		flasherror = 1;
	}


	if ((flasherror == 1) && (feerror == 0))
	{
		MSG_LOG("覆盖fe 2 flash \r\n");
		flasherase(ERASE4KBYTESECTOR, FLASH_PBOC_PARA);
		flashwrite(FLASH_PBOC_PARA, (unsigned char *)&smpPara, sizeof(stMobileParameter));
	}

	if ((flasherror == 0) && (feerror == 1))
	{
		MSG_LOG("覆盖flash 2 fe\r\n");
		sysfewrite(BIT_qpbpc_para, sizeof(stMobileParameter), (unsigned char *)&smpPara_flash);
	}

	MSG_LOG("FLASH_CACERT_BACKUP:%04X\r\n", FLASH_CACERT_BACKUP);
	MSG_LOG("FLASH_CACERT:%04X\r\n", FLASH_CACERT);
	memset(buff, 0, sizeof(buff));
	//-----BEGIN CERTIFICATE-----
	flashread(FLASH_CACERT + 4, buff, 32);
	if (strstr((const char *)buff, carcert) == NULL)
	{
		flashread(FLASH_CACERT_BACKUP + 4, buff, 32);
		if (strstr((const char *)buff, carcert) == NULL)
		{
			//都错了
			flasherase(ERASE4KBYTESECTOR, FLASH_CACERT_BACKUP);
			flasherase(ERASE4KBYTESECTOR, FLASH_CACERT);
		}
		else
		{
			MSG_LOG("cacert bp 2 \r\n");
			flashread(FLASH_CACERT_BACKUP, buff, MASK4K);
			flasherase(ERASE4KBYTESECTOR, FLASH_CACERT);
			flashwrite(FLASH_CACERT, buff, MASK4K);

		}

	}
	else
	{
		flashread(FLASH_CACERT_BACKUP + 4, buff, 32);
		//备份错了 
		if (strstr((const char *)buff, carcert) == NULL)
		{
			MSG_LOG("cacert 2 bp \r\n");
			flashread(FLASH_CACERT, buff, MASK4K);
			flasherase(ERASE4KBYTESECTOR, FLASH_CACERT_BACKUP);
			flashwrite(FLASH_CACERT_BACKUP, buff, MASK4K);

		}

	}




}
void getbussinessNO_qpboc(unsigned char *buffer)
{

	getMobileParameter(1, buffer);

	return;

}


//----签到信息---stMobilStyle--------------------
//Invalue 需要传入 ISOK[成功为0xA5,其它不成功 不成功后面台数无意义]   PIN_KEY  PIN_KEY_CHECKVALUE  MAC_KEY   MAC_KEY_CHECKVALUE
int saveSingInInfo(void)
{

	MSG_LOG("sizeof(stMobilStyle):%d:do %s\r\n", sizeof(stMobilStyle), __FUNCTION__);


	sysfewrite(BIT_SingInStyle, sizeof(stMobilStyle), (unsigned char*)&Sign_Infor);
	return 0;
}



volatile unsigned char rev_finish = 0;		//外接pos机串口接收完成标志

volatile unsigned char ComTest_index = 0;

void PBOC_hand(unsigned char value)
{
	unsigned char ii;
	//	unsigned char i;
	unsigned char xorCh = 0;
	//	unsigned char buff[10];

	ii = value;

	//if (value != 0x0D && value != 0x0A && value != 0x00) {
	MSG_LOG("%02X", value);
	//}

	if (rev_finish != 0xee)			//空则接收数据
	{
		if (rev_finish == 0xaa) {
			if (ii != 0x03) {
				ComTest_index = 0;
				rev_finish = 0;
				goto PBOC_hand_OVER;
			}
			irda_rxBuf[ComTest_index++] = ii;
			rev_finish = 0xee;
			MSG_LOG("\n");
			goto PBOC_hand_OVER;
		}

		switch (ComTest_index)
		{
		case 0://STX 	02h 	报文起始
			if (ii != 0x02) {
				ComTest_index = 0;
			}
			else {
				irda_rxBuf[ComTest_index++] = ii;
			}
			break;
		case 1:		//ln
			irda_rxBuf[ComTest_index++] = ii;
			break;
		case 2:		//cmd
			irda_rxBuf[ComTest_index++] = ii;
			break;
		default:
			irda_rxBuf[ComTest_index++] = ii;

			if (ComTest_index < (irda_rxBuf[mpc_ln] + 3)) {
				break;
			}


			// BCC——从 Ln 到 DATA 的字节异或校验和（hex）。
			xorCh = getSignXor(irda_rxBuf + mpc_ln, irda_rxBuf[mpc_ln] + mpc_ln);
			MSG_LOG("\ngetSignXor:%02X\n", xorCh);
			if (xorCh != ii)
			{
				memset(irda_rxBuf, 0, MaxPackLen);
				ComTest_index = 0;
				rev_finish = 0;
				break;
			}
			rev_finish = 0xaa;
			break;
		}
	}
	else {
		ComTest_index = 0;
	}
PBOC_hand_OVER:
	//MSG_LOG("%02X ", rev_finish);
	return;
}



void down_kek(void)
{
	unsigned char ret = 0;
	//	unsigned char i=0;
	unsigned int outdly = 0;
	unsigned char buff[100];
	//	unsigned char len=0;
	//	unsigned char XOR;
	unsigned char kekMM[30];
	//	unsigned char resbuff[16];
	unsigned char rands[16];
	unsigned char stCh = mpsc_ok;
	MD5_CTX md5;
	int len = 0;
	unsigned char backComDbg = 0;
	int pos = 0;


	memset(buff, 0, 100);

	//	cls();
	//	display(0,0,"等待接收母POS",DIS_CENTER);
	outdly = 0;
	rev_finish = 0;

	//	printf("qing%d\r\n",i);

	SetUSART1mode(0xFE);
	backComDbg = DEBUG_COM;
	DEBUG_COM = COM4;

	com_init(COM1, 9600);
	COM_it_enable(COM1);

	ComTest_index = 0;
	while (1) {

		if (outdly % 200 == 0) {
			cls();
			display(0, 0, "请连接机器COM1口9600", 0);
			display(2, 0, "等待接收母POS数据", 0);

			memset(buff, 0, sizeof(buff));
			getMobileParameter(1, buff);
			memset(kekMM, 0, sizeof(kekMM));
			sprintf((char *)kekMM, "银联商户号:%s", buff);
			display(4, 0, (char *)kekMM, 0);

			memset(buff, 0, sizeof(buff));
			getMobileParameter(6, buff);
			memset(kekMM, 0, sizeof(kekMM));
			sprintf((char *)kekMM, "银联设备号:%s", buff);
			display(6, 0, (char *)kekMM, 0);

			//com_snd(COM4,len,buff);
		}
		if (rev_finish == 0xee) {
			stCh = mpsc_ok;
			len = 0;

			switch (irda_rxBuf[mpc_cmd])
			{
			case 0x31:
				break;
			case 0x34:
				break;
			case 0x33:
				if (irda_rxBuf[mpc_ln] != 0x01) {
					stCh = mpsc_param;
					break;
				}
				// 随机数SysTime
				GetDateTime();

				MD5Init(&md5);
				MD5Update(&md5, (unsigned char *)&SysTime, sizeof(stcomtime));
				MD5Final(&md5, rands);

				len = 0x11;
				buff[mpc_stx] = 0x02;
				buff[mpc_ln] = len;
				buff[mpc_st] = stCh;
				memcpy(buff + mpc_data, rands, 16);
				buff[mpc_st + len] = getSignXor(buff + mpc_ln, len + mpc_ln);
				len += mpc_data;
				buff[len] = 0x03;
				++len;
				break;
			case 0x32:
				if (irda_rxBuf[mpc_ln] != 0x12) {
					stCh = mpsc_param;
					break;
				}
				// 主密钥号 M 为 1 字节
				DES3_decrypt(irda_rxBuf + mpc_data, rands, buff);
				DES3_decrypt(irda_rxBuf + mpc_data + 8, rands, buff + 8);
				saveMobileParameter(2, buff);		//kek明文

				display(7, 0, "KEK下载成功", DIS_CONVERT | DIS_CENTER);
				break;
			case 0x35:
				if (irda_rxBuf[mpc_ln] != 0x18) {
					stCh = mpsc_param;
					break;
				}
				saveMobileParameter(1, irda_rxBuf + mpc_data);		//
				saveMobileParameter(6, irda_rxBuf + mpc_data + 15);		//


				gmissflag = MISS_G_FREE;
				Sign_Infor.ISOK = 0;
				display(8, 0, "商户号和设备号下载成功", DIS_CONVERT | DIS_CENTER);
				beep(1, 200, 50);
				delayxms(1000);
				break;
			case 0x36:
				if (irda_rxBuf[mpc_ln] != 0x01) {
					stCh = mpsc_param;
					break;
				}
				len = 0x3B;
				buff[mpc_stx] = 0x02;
				buff[mpc_ln] = len;
				buff[mpc_st] = stCh;
				pos = mpc_data;
				memcpy(buff + pos, "EFS3000L", 8);
				pos += 8;
				memset(buff + pos, ' ', 12);
				pos += 12;
#ifdef PBOC_TEST1
				memcpy(buff + pos, "LF28517121111111", 16);
				pos += 16;
#else
				memcpy(buff + pos, "EFS3000L", 8);
				pos += 8;
				//Get_SerialNumF4((unsigned char *)buff + pos + 8);//Get_SerialNumF4_Self(buff);//
				//BCD2Ascii(buff + pos + 8, buff + pos, 4);
				memcpy(buff + pos, pFistVary.DeviceNo, 8);
				pos += 8;
#endif
				memset(buff + pos, ' ', 12);
				pos += 12;

				buff[mpc_st + len] = getSignXor(buff + mpc_ln, len + mpc_ln);
				len += mpc_data;
				buff[len] = 0x03;
				++len;
				break;
			default:
				break;
			}
			if (len == 0) {
				len = 0x01;
				buff[mpc_stx] = 0x02;
				buff[mpc_ln] = len;
				buff[mpc_st] = stCh;
				buff[mpc_st + len] = getSignXor(buff + mpc_ln, len + mpc_ln);
				len += mpc_data;
				buff[len] = 0x03;
				++len;
			}
			if (len > 0) {
				MSG_LOG("回复POS响应(%d):", len);
				BCD_LOG(buff, len, 1);
				com_snd(COM1, len, buff);
			}

			memset(irda_rxBuf, 0, MaxPackLen);
			ComTest_index = 0;
			rev_finish = 0;
			//break;
		}
		delayxms(10);

		ret = getkey(1);
		if (ret == KEY_ESC) {
			break;
		}
		if (outdly++ > 15000) {

			display(2, 0, "接收超时", DIS_CONVERT | DIS_CENTER);
			beep(3, 100, 30);
			delayxms(1000);
			break;
		}

	}
	//down_kek_over:
	SetUSART1mode(0xFD);

	com_init(COM1, 256000);
	COM_it_enable(COM1);

	DEBUG_COM = backComDbg;
}
#if 0
static int inline ReadM1Block(int block, unsigned char key[6], unsigned char serialId[4], unsigned char output[16]) {


	MifareAuthKey(PICC_AUTHENT1A, (unsigned char *)&c_serial, output, block);

	if (MifareRead(block, buffer) == 0)
	{
		debugstring("read 1 error\r\n ");
		//			profile(0);
	}
	else {
		MSG_LOG(buffer, blockLen, 1);
	}
}
#endif

void down_kek_ic(void)
{
	unsigned char ret = 0;
	int outdly = 200;
	unsigned char kekMM[30];
	unsigned char resPonse = 0;
	unsigned char cNo[8];
	unsigned char buffer[100];
	unsigned char downKey[] = "\x6E\xE1\x25\x9B\x41\x60\x27\x24\xAA\xD1\x61\x7E\xC7\xC2\x1C\x08";
	unsigned char m1Key[] = "\xFF\xFF\xFF\xFF\xFF\xFF";
	int block = 0;
	int blockLen = 16;


	cls();
	display(0, 0, "请将密钥卡放入刷卡区", 0);

	while (1) {
		sprintf((char *)buffer, "等待倒计时:%dS ", outdly >> 1);
		display(2, 0, (char *)buffer, 0);

		resPonse = profile(0);

		//MSG_LOG("resPonse:%d\n", resPonse);

		while (resPonse == 0) {	// M1卡

			display(2, 0, "检测到密钥卡,正在处理中...", 0);

			//	unsigned char ptime;
			memcpy(cNo, (unsigned char *)&c_serial, 4);

			MSG_LOG("M1 card type Process:\n");
			// 7扇区0块：银行卡终端号：HNBANK00XXXXXXXX
			MSG_LOG("7扇区0块：银行卡终端号：HNBANK00XXXXXXXX\n");
			block = 7 * 4 + 1;	// 
			if (MifareAuthKey(PICC_AUTHENT1A, (unsigned char *)&c_serial, m1Key, block) == 0) {
				debugstring("fail to MifareAuthKey\r\n ");
				break;
			}

			if (MifareRead(block, buffer) == 0)
			{
				debugstring("read 1 error\r\n ");
				break;
			}
			else {
				buffer[blockLen] = '\0';
				MSG_LOG("POS终端号:%s", buffer);
				sprintf((char *)buffer + 50, "POS终端号:%s", buffer);
				display(4, 0, (char *)buffer + 50, 0);
				getMobileParameter(6, buffer + blockLen);
				(buffer + blockLen)[8] = '\0';
				if (memcmp(buffer + 8, buffer + blockLen, 8) != 0) {
					sprintf((char *)buffer + 50, "!!BUS终端号不一致:%s", buffer + blockLen);
					display(6, 0, (char *)buffer + 50, 0);
					delayxms(5000);
					break;
				}
			}
			MSG_LOG("7扇区1块：银行卡商户号：15个字节\n");
			block++;
			if (MifareRead(block, buffer) == 0)
			{
				debugstring("read 1 error\r\n ");
				break;
			}
			else {
				buffer[blockLen] = 0;
				MSG_LOG("商户号:%s", buffer);
				//saveMobileParameter(1, buffer);

				//BCD2Ascii(buffer, buffer + blockLen, blockLen);
				sprintf((char *)buffer + 50, "商户号:%s", buffer);
				display(6, 0, (char *)buffer + 50, 0);

				buffer[blockLen] = '\0';
				MSG_LOG("POS商户号:%s", buffer);
				sprintf((char *)buffer + 50, "POS商户号:%s", buffer);
				display(6, 0, (char *)buffer + 50, 0);
				getMobileParameter(1, buffer + blockLen);
				(buffer + blockLen)[15] = '\0';
				if (memcmp(buffer, buffer + blockLen, 15) != 0) {
					sprintf((char *)buffer + 50, "BUS商户号:%s", buffer + blockLen);
					display(8, 0, (char *)buffer + 50, 0);
					break;
				}
			}


			MSG_LOG("8扇区0块：银行卡密钥加密后前16字节\n");
			block = 8 * 4 + 1;	// 
			if (MifareAuthKey(PICC_AUTHENT1A, (unsigned char *)&c_serial, m1Key, block) == 0) {
				debugstring("fail to MifareAuthKey\r\n ");
				break;
			}

			if (MifareRead(block, buffer) == 0)
			{
				debugstring("read 2 error\r\n ");
				break;
			}
			else {
				BCD_LOG(buffer, blockLen, 1);
				DES3_decrypt(buffer, downKey, kekMM);
				DES3_decrypt(buffer + 8, downKey, kekMM + 8);
				BCD_LOG((unsigned char *)kekMM, blockLen, 1);
				saveMobileParameter(2, kekMM);
			}
			MSG_LOG("8扇区1块：银行卡密钥加密后后16字节\n");
			block++;
			if (MifareRead(block, buffer) == 0)
			{
				debugstring("read 3 error\r\n ");
				break;
			}
			else {
				BCD_LOG(buffer, blockLen, 1);
				DES3_decrypt(buffer, downKey, kekMM);
				DES3_decrypt(buffer + 8, downKey, kekMM + 8);
				BCD_LOG((unsigned char *)kekMM, blockLen, 1);
			}

			gmissflag = MISS_G_FREE;
			Sign_Infor.ISOK = 0;
			display(8, 0, "岳阳银联密钥下载成功", DIS_CONVERT | DIS_CENTER);
			beep(1, 200, 50);
			delayxms(5000);
			goto down_kek_ic_over;
		}
		if (resPonse == 0) {
			cls();
			display(0, 0, "请将密钥卡放入刷卡区", 0);
		}

		delayxms(500);

		ret = getkey(1);
		if (ret == KEY_ESC) {
			break;
		}



		if (outdly-- <= 0) {

			display(2, 0, "银联密钥注入超时", DIS_CONVERT | DIS_CENTER);
			beep(3, 100, 30);
			delayxms(1000);
			break;
		}

	}
down_kek_ic_over:

	return;
}





//设置商户号15位ASC、终端号8位ASC、电话号16位BCD
////mode 1 设置tpdu  2,设备号
void set_bussid(unsigned char mode)
{

	unsigned char i;
	unsigned char buffer[20];
	unsigned char ret;
	//	unsigned int indd;
	unsigned char shopNo[16];
	unsigned char pralenth = 0;



	cls();
	memset(shopNo, 0, sizeof(shopNo));

	if (mode == 1)
	{
		pralenth = 10;
		getMobileParameter(5, buffer);
		BCD2Ascii(buffer, shopNo, 5);
	}
	else if (mode == 2)
	{
		pralenth = 8;
		getMobileParameter(6, shopNo);

	}
	else
	{
		pralenth = 15;

		getbussinessNO_qpboc(shopNo);
	}



	//bussStartt:
	i = 0;
	memset(buffer, 0, 20);

	if (ValidDevNo(shopNo) == 1) {
		memset(shopNo, 0, sizeof(shopNo));

		memset(shopNo, '0', pralenth);
	}

	memcpy(buffer, shopNo, pralenth);

	if (mode == 1)
	{
		display(0, 2, "设置TPDU", DIS_CENTER);
	}
	else if (mode == 2)
	{
		display(0, 2, "设置银联设备号", DIS_CENTER);

	}
	else
	{
		display(0, 2, "设置商户号", DIS_CENTER);

	}
	display(2, 4, (char*)buffer, 0);
	while (1)
	{
		delayxms(10);
		memset(buffer, 0, 10);
		buffer[0] = shopNo[i];
		display(2, 4 + i, (char*)buffer, DIS_CONVERT);
		ret = getkey(1);
		switch (ret)
		{

		case UP:
			if ((shopNo[i] < '9') && (shopNo[i] >= '0'))
				shopNo[i]++;
			else if (shopNo[i] == '9') {
				shopNo[i] = '0';
			}
			else if ((shopNo[i] >= 'A') && (shopNo[i] < 'Z'))
				shopNo[i]++;
			else if (shopNo[i] == 'Z') {
				shopNo[i] = '0';
			}
			else
				shopNo[i] = '0';
			break;
		case ESC:
			goto Setupbuss_exit;
		case ENTER:
			i++;
			memset(buffer, 0, 16);
			memcpy(buffer, shopNo, pralenth);
			display(2, 4, (char*)buffer, 0);


			if (i >= pralenth) {

				if (mode == 1)
				{
					Ascii2BCD(shopNo, buffer, pralenth);
					saveMobileParameter(5, buffer);
				}
				else if (mode == 2)
				{
					saveMobileParameter(6, shopNo);
				}
				else {
					saveMobileParameter(1, shopNo);

				}

				display(0, 0, "提示:", 1);
				display(5, 0, "设置完成", DIS_CENTER);
				delayxs(1);




#ifdef OTHER_GPRS
				gprs_other_sndbuinfo();
#endif
				goto Setupbuss_exit;
				}
			break;
		default:
			break;

			}
		}
Setupbuss_exit:
	;

	}

void set_bussid_pboc(void)
{
	set_bussid(0);
}
void set_tpdu(void)
{
	set_bussid(1);
}
void set_pboc_devide(void)
{
	set_bussid(2);
}


//用于USB优盘更新参数文件 by pfuqan
//>0 成功，<0 失败  =0 其他参数不对
int set_PBOC_KEY(char *infor)
{
	char *lineinfor = NULL;
	unsigned char dat[16];
	unsigned char key[16];
	char buff[257];
#ifdef _debug_
	//	unsigned char *p=NULL;
#endif
//	char ctempbuff[16];
	//	unsigned char ucbuff[16];
	unsigned int i = 0;//,count=0;


	MSG_LOG("do %s\r\n", __FUNCTION__);

	memset(buff, 0, sizeof(buff));
	memcpy(buff, infor, sizeof(buff));
	buff[256] = 0;
	//	MSG_LOG("BUFF:%s\r\n",buff);
	MSG_LOG("buff:");
	for (i = 0; i < 58; i++)
	{
		MSG_LOG("%c", buff[i]);
	}
	MSG_LOG("\r\n");

	lineinfor = NULL;
	lineinfor = strtok(buff, ",");	//找到商户后面的, 
	if (lineinfor == NULL)
	{
		return -1;
	}


	MSG_LOG("find 1商户:%s\r\n", lineinfor);

	getMobileParameter(1, dat);


	if (memcmp(dat, lineinfor, 15) != 0)
	{
		return 0;
	}

	lineinfor = NULL;
	lineinfor = strtok(NULL, ",");
	if (lineinfor == NULL)
	{
		return -2;
	}
	MSG_LOG("find 2设备号:%s\r\n", lineinfor);	//

	memset(dat, 0, sizeof(dat));
	getMobileParameter(6, dat);
	MSG_LOG("yinl:%s\r\n", dat);


	if (memcmp(dat, lineinfor, 8) != 0)
	{
		return 0;
	}

	lineinfor = NULL;
	lineinfor = strtok(NULL, ",");
	if (lineinfor == NULL)
	{
		return -3;
	}
	MSG_LOG("find 3密钥:%s\r\n", lineinfor);	//



	Ascii2BCD((unsigned char *)lineinfor, key, 32);

	MSG_LOG("save:key:");
	BCD_LOG(key, 16, 1);
	saveMobileParameter(2, key);		//kek明文


	return 1;



}
//U盘取密钥信息，返回0：返回<0:失败，返回>0成功
int usb_load_pboc_infor(void)
{
	FRESULT res;
	unsigned char buff[4096];
	// 	unsigned char buf1[16];
	char *lineinfor = NULL;
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

	MSG_LOG("do %s\r\n", __FUNCTION__);

	memset(buff, 0, sizeof(buff));




	cls();
	display(2, 0, "更新主密钥参数", DIS_CENTER);

	res = f_open(&rf1, "0:/pra/GZPBOCINFOR.txt", FA_READ);
	if (res != FR_OK)//if(access("0:/current/Rindex.txt",0)!=0)
	{
		dis_messgebox("密钥文件错", res, 12);
		return 0;
	}

	res = f_stat("0:/pra/GZPBOCINFOR.txt", &finf);
	if (res != FR_OK) {
		dis_messgebox("密钥文件信息错", res, 12);
		return -1;
	}

	memset(buff, 0, sizeof(buff));
	ret = _Fread(buff, sizeof(buff), &rf1);
	//	ret = fread(buffer,1,1024,fp);	//size_t fread ( void *buffer, size_t size, size_t count, FILE *stream) ;

	MSG_LOG("ret:%d\r\n", ret);

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
	lineinfor = strstr((char *)buff, "\r\n");

	if (lineinfor == NULL)
	{
		return -2;
	}

	lineinfor += 2;	//跳\r\n  这个是头注释

	MSG_LOG("head  +2:");


	while (1)
	{
		ret = set_PBOC_KEY(lineinfor);

		if (ret > 0)
		{
			return 2;	//成功
		}
		clr_wdt();
		lineinfor = strstr((char *)lineinfor, "\r\n");
		if (lineinfor == NULL)
		{
			break;
		}
		lineinfor += 2;	//跳\r\n

	}






	return 0;



}





//用于USB优盘更新参数文件 by pfuqan
//>0 成功，<0 失败  =0 其他参数不对
// int set_line_mzt(char *infor)
// {
// 	char *lineinfor = NULL;
// 	char buff[257];
// #ifdef _debug_
// 	//	unsigned char *p = NULL;
// #endif
// 	char ctempbuff[16];
// 	//	unsigned char ucbuff[16];
// 	unsigned int i = 0, count = 0;
// 
// 	unsigned char lineNO[3] = { 0 };
// 	unsigned int price = 0, plusprice = 0;
// 	unsigned char rate[32] = { 0 }, rateplus[32] = { 0 };
// 
// 	MSG_LOG("do %s\r\n", __FUNCTION__);
// 
// 	memset(buff, 0, sizeof(buff));
// 	memcpy(buff, infor, sizeof(buff));
// 	buff[256] = 0;
// 
// 	MSG_LOG("Line infor:%s\r\n", buff);
// 
// 
// 	memset(rate, 204, 32);
// 	memset(rateplus, 100, 32);
// 
// 
// 
// 	//线路 票价，附加票价，折扣，附加折扣，?个 0~31 折扣附加折扣...
// 	//LINO,000100,PRICE,100,0,RATE,10,100,100,90,100,80,100,70,100,60,100,50,100,40,100,30,100,20,100,10,100
// 	lineinfor = strtok(buff, ",");	//找到LINE后面的, 
// 	if (lineinfor == NULL)
// 	{
// 		return -1;
// 	}
// 
// 	MSG_LOG("find KEY:%s\r\n", lineinfor);
// 
// 	lineinfor = strtok(NULL, ",");
// 	if (lineinfor == NULL)
// 	{
// 		return -2;
// 	}
// 
// 	MSG_LOG("find LINO:%s\r\n", lineinfor);
// 
// 	count = __atol(lineinfor);
// 	memset(ctempbuff, 0, sizeof(ctempbuff));
// 	sprintf(ctempbuff, "%06d", count);
// 	Ascii2BCD((unsigned char *)ctempbuff, lineNO, 6);
// 
// 	if (memcmp(pFistVary.LineNo, lineNO, 2) != 0)
// 	{
// 		MSG_LOG("线路不一致,退出\r\n");
// 		BCD_LOG(pFistVary.LineNo, 3, 1);
// 		BCD_LOG(lineNO, 3, 1);
// 		return 0;
// 	}
// 
// 
// 
// 
// 	lineinfor = strtok(NULL, ",");
// 	if (lineinfor == NULL)
// 	{
// 		return -3;
// 	}
// 	MSG_LOG("find KEY:%s\r\n", lineinfor);	//PRICE
// 
// 	lineinfor = strtok(NULL, ",");
// 	if (lineinfor == NULL)
// 	{
// 		return -4;
// 	}
// 	price = __atol(lineinfor);
// 
// 	MSG_LOG("find price:%s:%d\r\n", lineinfor, price);
// 
// 	lineinfor = strtok(NULL, ",");
// 	if (lineinfor == NULL)
// 	{
// 		return -5;
// 	}
// 	plusprice = __atol(lineinfor);
// 
// 	MSG_LOG("find pluseprice:%s:%d\r\n", lineinfor, plusprice);
// 
// 
// 	lineinfor = strtok(NULL, ",");
// 	if (lineinfor == NULL)
// 	{
// 		return -6;
// 	}
// 	MSG_LOG("find KEY:%s\r\n", lineinfor);	//RATE
// 
// 	lineinfor = strtok(NULL, ",");
// 	if (lineinfor == NULL)
// 	{
// 		return -7;
// 
// 	}
// 	count = __atol(lineinfor);
// 	MSG_LOG("find rateNUM:%s,count=%d\r\n", lineinfor, count);
// 
// 	for (i = 0; i < count; i++)
// 	{
// 		lineinfor = strtok(NULL, ",");
// 		if (lineinfor == NULL)
// 		{
// 			return -8;
// 		}
// 		rate[i] = __atol(lineinfor);
// 		MSG_LOG("find card:%d rate:%s ", i, lineinfor);
// 
// 		lineinfor = strtok(NULL, ",");
// 		if (lineinfor == NULL)
// 		{
// 			return -9;
// 		}
// 		rateplus[i] = __atol(lineinfor);
// 		MSG_LOG(" plusrate:%s\r\n", lineinfor);
// 
// 	}
// 
// 
// 	MSG_LOG("price:%d,plus:%d\r\n", price, plusprice);
// 	MSG_LOG("line:");
// 	BCD_LOG(lineNO, 3, 1);
// 
// 	MSG_LOG("Rate:");
// 	BCD_LOG(rate, 32, 1);
// 	MSG_LOG("pluseRate:");
// 	BCD_LOG(rateplus, 32, 1);
// 
// 
// 	memcpy(pFistVary.LineNo, lineNO, 3);
// 	memcpy(NewPriceTab.busPrice, &price, 4);
// 	memcpy(NewPriceTab.plusPrice, &plusprice, 4);
// 
// 	memcpy(&NewPriceTab.rate[1], rate, 31);
// 	NewPriceTab.rate[0] = rate[31];
// 	memcpy(&NewPriceTab.plusrate[1], rateplus, 31);
// 	NewPriceTab.plusrate[0] = rateplus[31];
// 
// 
// 	memset(NewPriceTab.lineVer, 0, 2);
// 
// 	sysfewrite(BIT_LINENO, 3, pFistVary.LineNo);
// 	sysfewrite(BIT_PRICETAB, sizeof(stPricetable), (unsigned char*)&NewPriceTab);
// 	sysfewrite(BIT_GPSChildLine, 2, pFistVary.LineNo);//刚开始时，子线路和主线路一致
// 
// 	savesysbupara();
// 	saveCpuInfo(1, (unsigned char *)ctempbuff);
// 
// 
// 
// 
// 
// 
// 
// 	return 1;
// 
// 
// }

//U盘取线路信息，返回0：返回<0:失败，返回>0成功
// int usb_loadlineinfor(void)
// {
// 	FRESULT res;
// 	unsigned char buff[4096];
// 	// 	unsigned char buf1[16];
// 	char *lineinfor = NULL;
// 	FILINFO finf;
// 	FIL rf1;
// 	int ret = 0;
// 	//unsigned int count=0;
// 
// 
// 
// // 	char ctempbuff[16];
// // 	//	unsigned char ucbuff[16];
// // 	
// // 	char *lineinfor_temp=NULL;
// // 	unsigned char lineNO[3]={0};
// // 	unsigned int price=0,plusprice=0;
// // 	unsigned char rate[32]={0},rateplus[32]={0};
// 
// 	MSG_LOG("do %s\r\n", __FUNCTION__);
// 
// 	memset(buff, 0, sizeof(buff));
// 
// 
// 
// 
// 	cls();
// 	display(2, 0, "更新线路参数", DIS_CENTER);
// 
// 	res = f_open(&rf1, "0:/pra/LINEINFOR.txt", FA_READ);
// 	if (res != FR_OK)//if(access("0:/current/Rindex.txt",0)!=0)
// 	{
// 		dis_messgebox("线路文件错", res, 12);
// 		return 0;
// 	}
// 
// 	res = f_stat("0:/pra/LINEINFOR.txt", &finf);
// 	if (res != FR_OK) {
// 		dis_messgebox("线路文件信息错", res, 12);
// 		return -1;
// 	}
// 
// 	memset(buff, 0, sizeof(buff));
// 	ret = _Fread(buff, sizeof(buff), &rf1);
// 	//	ret = fread(buffer,1,1024,fp);	//size_t fread ( void *buffer, size_t size, size_t count, FILE *stream) ;
// 
// 	MSG_LOG("ret:%d\r\n", ret);
// 
// 	//	MSG_LOG(buff);
// 
// 	//	MSG_LOG("find line\r\n");
// 
// 
// 	// 	count = 0;
// 	// 	
// 	// 	lineinfor = strtok((char *)buff, "\r\n");  
// 	//     if (lineinfor != NULL)  
// 	// 	{
// 	// 		
// 	// 		while (lineinfor !=NULL)
// 	// 		{
// 	// 			MSG_LOG("\r\n**************Line:%d  :%s***********\r\n",count,lineinfor);
// 	// 			lineinfor = strtok(NULL, "\r\n"); 
// 	// 
// 	// 			count ++;
// 	// 		}
// 	// 	}
// 	// 
// 	// 
// 	// 	MSG_LOG("有%d个\r\n",count);
// 
// 
// 	lineinfor = NULL;
// 	lineinfor = strstr((char *)buff, "\r\n");
// 
// 	if (lineinfor == NULL)
// 	{
// 		return -2;
// 	}
// 
// 	lineinfor += 2;	//跳\r\n  这个是头注释
// 
// 	while (1)
// 	{
// 		ret = set_line_mzt(lineinfor);
// 		if (ret > 0)
// 		{
// 			return 2;	//成功
// 		}
// 		clr_wdt();
// 		lineinfor = strstr((char *)lineinfor, "\r\n");
// 		if (lineinfor == NULL)
// 		{
// 			break;
// 		}
// 		lineinfor += 2;	//跳\r\n
// 
// 
// 	}
// 
// 
// 
// 
// 
// 
// 	return 0;
// 
// 
// 
// }

//返回<0:失败，返回>0成功
int cp_cacert(void)
{

	FRESULT res;
	unsigned char buff[4096];

	// 	unsigned char buf1[16];
//	char *lineinfor=NULL;
	FILINFO finf;
	FIL rf1;
	int ret = 0;
	unsigned int count = 0;
	unsigned int addr = 0;

#ifdef _debug_
	unsigned int i = 0, uitemp = 0;
	unsigned char disbuff[257];
#endif


	// 	char ctempbuff[16];
	// 	//	unsigned char ucbuff[16];
	// 	
	// 	char *lineinfor_temp=NULL;
	// 	unsigned char lineNO[3]={0};
	// 	unsigned int price=0,plusprice=0;
	// 	unsigned char rate[32]={0},rateplus[32]={0};

	MSG_LOG("do %s\r\n", __FUNCTION__);

	memset(buff, 0, sizeof(buff));




	cls();
	display(2, 0, "更新证书", DIS_CENTER);

	res = f_open(&rf1, "0:/pra/cacert.pem", FA_READ);
	if (res != FR_OK)//if(access("0:/current/Rindex.txt",0)!=0)
	{
		dis_messgebox("证书文件错", res, 12);
		return 0;
	}

	res = f_stat("0:/pra/cacert.pem", &finf);
	if (res != FR_OK) {
		dis_messgebox("线路文件信息错", res, 12);
		return -1;
	}

	memset(buff, 0, sizeof(buff));
	ret = _Fread(buff, sizeof(buff), &rf1);
	//	ret = fread(buffer,1,1024,fp);	//size_t fread ( void *buffer, size_t size, size_t count, FILE *stream) ;

	MSG_LOG("ret:%d\r\n", ret);

	addr = FLASH_CACERT;
	flasherase(ERASE4KBYTESECTOR, addr);
	addr = FLASH_CACERT_BACKUP;
	flasherase(ERASE4KBYTESECTOR, addr);

	addr = FLASH_CACERT;
	count = ret;
	flashwrite(addr, (unsigned char *)&count, 4);
	flashwrite(addr + 4, buff, ret);

	addr = FLASH_CACERT_BACKUP;
	count = ret;
	flashwrite(addr, (unsigned char *)&count, 4);
	flashwrite(addr + 4, buff, ret);



	memset(buff, 0, sizeof(buff));

#ifdef _debug_
	count = 0;
	memset(buff, 0, sizeof(buff));
	flashread(addr, (unsigned char *)&count, 4);
	MSG_LOG("len:%d\r\n", count);
	if (count > 4096)
	{
		count = 4096 - 4;
	}
	flashread(addr + 4, buff, count);


	i = 0;
	while (i < count)
	{
		clr_wdt();
		memset(disbuff, 0, sizeof(disbuff));
		uitemp = count - i;
		if (uitemp > 256)
		{
			memcpy(disbuff, buff + i, 256);
			i += 256;
		}
		else {
			memcpy(disbuff, buff + i, uitemp);
			i = count;
		}
		MSG_LOG("%s", disbuff);

	}

#endif



	return 1;



}

void SetSevice_pboc(void)
{
	SetupClientIP(1);
}

void EnableWeiXin(void) {
	g_supportQR |= qr_weiXin;
	saveMobileParameter(8, &g_supportQR);
	cls();
	display(6, 0, "启用微信付款码成功!", DIS_CENTER);
	delayxms(500);
}

void DisableWeiXin(void) {
	g_supportQR &= ~qr_weiXin;
	saveMobileParameter(8, &g_supportQR);
	cls();
	display(6, 0, "禁用微信付款码成功!", DIS_CENTER);
	delayxms(500);
}

void EnableAliPay(void) {
	g_supportQR |= qr_aliPay;
	saveMobileParameter(8, &g_supportQR);
	cls();
	display(6, 0, "启用支付宝付款码成功!", DIS_CENTER);
	delayxms(500);
}

void DisableAliPay(void) {
	g_supportQR &= ~qr_aliPay;
	saveMobileParameter(8, &g_supportQR);
	cls();
	display(6, 0, "禁用支付宝付款码成功!", DIS_CENTER);
	delayxms(500);
}

void showSetOtherPayCode() {

	unsigned char ucMENU_NUM = 0;
	char menu[5][20];
	void(*function[5])(void);

	unsigned int i = 0;

	for (i = 0; i < 5; i++)
	{
		memset(menu[i], 0, sizeof(menu[i]));
	}


	ucMENU_NUM = 0;
	strcpy(menu[ucMENU_NUM], "启用微信付款码  ");
	function[ucMENU_NUM++] = EnableWeiXin;
	strcpy(menu[ucMENU_NUM], "禁用微信付款码  ");
	function[ucMENU_NUM++] = DisableWeiXin;
	strcpy(menu[ucMENU_NUM], "启用支付宝付款码  ");
	function[ucMENU_NUM++] = EnableAliPay;
	strcpy(menu[ucMENU_NUM], "禁用支付宝付款码  ");
	function[ucMENU_NUM++] = DisableAliPay;

	MenuFrame(menu, function, "  --配置其他付款码菜单--  ", ucMENU_NUM, (100));
}
unsigned char switch_both = 0;
void PAY_MODE_init(void)  //1字节存标志，1个字节存开关
{
	unsigned  char buff[4];
	memset(buff, 0, 4);
	sysferead(BIS_PAY_MODE, 4, buff);
	MSG_LOG("开关\r\n");
	BCD_LOG(buff, 4, 1);
	shuangmian = buff[0];
	switch_both = buff[1];

	shuangmian = 1;
	switch_both = 0;

}
void PAY_MODE_init_first(void)  //1字节存标志，1个字节存开关
{
	unsigned char buff[4];
	unsigned char buffer[33];
	memset(buff, 0, 4);
	shuangmian = 1;
	switch_both = 0;

	buff[0] = shuangmian;
	buff[1] = switch_both;
	MSG_LOG("设置开关\r\n");
	BCD_LOG(buff, 4, 1);

	sysfewrite(BIS_PAY_MODE, 4, buff);

	Ascii2BCD("98765432101234567890123456789012", buffer, 32);
	saveMobileParameter(7, buffer);		//认证密钥明文
}
void PAY_MODE_SWITCH(unsigned char shuangmian_T, unsigned char switch_both_T)  //1字节存标志，1个字节存开关
{
	unsigned  char buff[4];
	memset(buff, 0, 4);
	sysferead(BIS_PAY_MODE, 4, buff);
	MSG_LOG("开关\r\n");
	BCD_LOG(buff, 4, 1);
	if (switch_both_T == 1)//开启了双通道
	{
		if (shuangmian == 0) //		切回在线
		{
			shuangmian = 1;
		}
		return;
	}
	else
		return;


}
void online_1(void) {
	unsigned char buff[4];

	memset(buff, 0, 4);

	shuangmian = 1;
	switch_both = 0;

	buff[0] = shuangmian;
	buff[1] = switch_both;
	MSG_LOG("设置开关\r\n");
	BCD_LOG(buff, 4, 1);

	sysfewrite(BIS_PAY_MODE, 4, buff);
	restore_flag = 3;
	cls();
	display(6, 0, "设置成功!", DIS_CENTER);
	delayxms(500);
}

void online_2(void) {
	unsigned char buff[4];

	memset(buff, 0, 4);

	//oda
	shuangmian = 0;
	switch_both = 0;

	buff[0] = shuangmian;
	buff[1] = switch_both;
	MSG_LOG("设置开关\r\n");
	BCD_LOG(buff, 4, 1);
	sysfewrite(BIS_PAY_MODE, 4, buff);
	restore_flag = 3;
	cls();
	display(6, 0, "设置成功!", DIS_CENTER);
	delayxms(500);
}

void online_3(void) {
	unsigned char buff[4];

	memset(buff, 0, 4);

	//双
	shuangmian = 1;
	switch_both = 1;

	buff[0] = shuangmian;
	buff[1] = switch_both;
	MSG_LOG("设置开关\r\n");
	BCD_LOG(buff, 4, 1);
	sysfewrite(BIS_PAY_MODE, 4, buff);
	memset(buff, 0, 4);
	sysferead(BIS_PAY_MODE, 4, buff);
	MSG_LOG("设置开关==2\r\n");
	BCD_LOG(buff, 4, 1);
	restore_flag = 3;
	cls();
	display(6, 0, "设置成功!", DIS_CENTER);
	delayxms(500);
}
void showSetOtherPaymode(void)
{

	unsigned char ucMENU_NUM = 0;
	char menu[3][20];
	void(*function[3])(void);

	unsigned int i = 0;

	for (i = 0; i < 3; i++)
	{
		memset(menu[i], 0, sizeof(menu[i]));
	}


	ucMENU_NUM = 0;
	strcpy(menu[ucMENU_NUM], "只支持在线  ");
	function[ucMENU_NUM++] = online_1;

	strcpy(menu[ucMENU_NUM], "ODA  ");
	function[ucMENU_NUM++] = online_2;
	strcpy(menu[ucMENU_NUM], "双通道  ");
	function[ucMENU_NUM++] = online_3;


	MenuFrame(menu, function, "  --配置模式菜单--  ", ucMENU_NUM, (100));
}



void set_pboc_menu(void)

{
	unsigned char ucMENU_NUM = 0;
	char menu[10][20];
	void(*function[10])(void);

	unsigned int i = 0;

	for (i = 0; i < 10; i++)
	{
		memset(menu[i], 0, sizeof(menu[i]));
	}


	ucMENU_NUM = 0;
	strcpy(menu[ucMENU_NUM], "下载密钥         ");
	function[ucMENU_NUM++] = down_kek_ic;
	strcpy(menu[ucMENU_NUM], "设置银联设备号   ");
	function[ucMENU_NUM++] = set_pboc_devide;
	strcpy(menu[ucMENU_NUM], "设置银联服务器   ");
	function[ucMENU_NUM++] = SetSevice_pboc;
	//strcpy(menu[ucMENU_NUM], "设置银联商户号   ");
	//function[ucMENU_NUM++] = set_bussid_pboc;
	strcpy(menu[ucMENU_NUM], "设置银联TPDU     ");
	function[ucMENU_NUM++] = set_tpdu;
	
	strcpy(menu[ucMENU_NUM], "清空银联参数  ");
	function[ucMENU_NUM++] = cls_qpoc;
		strcpy(menu[ucMENU_NUM], "清空银联冲正  ");
	function[ucMENU_NUM++] = cls_repurse;
	
	strcpy(menu[ucMENU_NUM], "配置其他付款码  ");
	function[ucMENU_NUM++] = showSetOtherPayCode;
#if 0
	strcpy(menu[ucMENU_NUM], "配置扣费模式    ");
	function[ucMENU_NUM++] = showSetOtherPaymode;
#endif

	MenuFrame(menu, function, "  --设置银联菜单--  ", ucMENU_NUM, (100));

	}

void Show_pboc_minu(void)
{
	char disbuf[50][30];
	unsigned char buff[32];
	//	unsigned short kk;
	unsigned int i, index;
	//	unsigned char cstart, uct;

	index = 0;
	for (i = 0; i < 50; i++)
		memset(disbuf[i], 0, 30);


	memset(buff, 0, sizeof(buff));
	getMobileParameter(3, buff);
	getMobileParameter(4, (unsigned char *)&i);
	sprintf(disbuf[index], "IP:%d.%d.%d.%d:%u", buff[0], buff[1], buff[2], buff[3], i);
	index++;

	memset(buff, 0, sizeof(buff));
	getMobileParameter(6, buff);
	sprintf(disbuf[index++], "银联设备号:%s", buff);

	memset(buff, 0, sizeof(buff));
	getMobileParameter(1, buff);
	sprintf(disbuf[index++], "银联商户号:%s", buff);
	getMobileParameter(5, buff);
	strcpy(disbuf[index], "银联TPDU  :");
	BCD2Ascii(buff, (unsigned char *)(disbuf[index++] + 11), 5);

	dispageInfo_L(index, disbuf);



}

void dis_qboc_ack_code(unsigned char ack)
{
	unsigned char Buffer[32];
	cls();
	memset(Buffer, 0, sizeof(Buffer));
	sprintf((char *)Buffer, "拒绝交易,错误码:%02X", ACK_flag);
	display(0, 0, (const char *)Buffer, 0);

	switch (ACK_flag)
	{
	case 0x51:
		display_3232(2, 0, "帐号余额不足", 0);
		break;
	case 0x14:
	case 0x05:
		display_3232(2, 0, "无效卡号", 0);
		break;
	case 0x55:

		display_3232(2, 0, "发卡行未开通免密免签功能", 0);
		break;
	case 0x40:

		display_3232(2, 0, "发卡行不支持的交易类型", 0);
		break;
	case 0x65:
		display_3232(2, 0, "超出消费次数限制", 0);
		break;
	case 0xFF:
		display_3232(2, 0, "网络超时", 0);
		display_3232(6, 0, "请稍后再试", 0);

		break;

	default:
		break;
	}

}

unsigned char read_re_infor(unsigned char *out_infor, int *pOlen)
{
	//st_qpoc_repurse re_infor;
	int len = 0;
	int lenLimit = 0;

	if (out_infor == NULL || pOlen == NULL) {
		return ST_ERROR;
	}
	lenLimit = *pOlen;
	*pOlen = 0;

	sysferead(BIT_repurse_infor, 4, out_infor);
	len = GET_INT16(out_infor + 2);
	if (len > lenLimit) {
		return ST_ERROR;
	}
	if (memcmp(out_infor, RECORD_FALG, 2) != 0)
	{
		return ST_ERROR;
	}
	sysferead(BIT_repurse_infor + 4, len, out_infor);
	MSG_LOG("铁电有冲正记录需要处理\n");
	*pOlen = len;

	return ST_OK;

}
//ST_OK 有冲正记录要用
unsigned char get_repurse_num(void)
{
	//st_qpoc_repurse re_infor;
	unsigned char re_infor[2];

// 	if (s_isAuthOk == 0) {
// 		return ST_ERROR;
// 	}

	sysferead(BIT_repurse_infor, 2, re_infor);
	if (memcmp(re_infor, RECORD_FALG, 2) == 0)
	{
		return ST_OK;
	}

	return ST_ERROR;

}
//开机加载
//mode==1  写进铁电，mode==2删掉铁电记录  3 检查是否有记录需要转正      解决消费未完成  断电没记录的情况
void save_repurse_infor(unsigned char mode, unsigned char *re_infor) {

	unsigned int addr = 0;
	int re_inforLen = 0;
	unsigned char tmpChs[4] = { 0,0,0,0 };


	MSG_LOG("save_repurse_infor(%02X):%d\n", mode, re_inforLen);

	addr = BIT_repurse_infor;

	switch (mode)
	{
	case FeRC_Write://写记录
		clr_wdt();
		//memcpy((unsigned char*)&re_infor, data, sizeof(re_infor));
		re_inforLen = GET_INT16(re_infor + 2) + 4;
		memcpy(re_infor, RECORD_FALG, 2);
		sysfewrite(addr, re_inforLen, re_infor);
		MSG_LOG("写铁电冲正\r\n ");
		BCD_LOG(re_infor, re_inforLen, 1);
		break;
	case FeRC_Dlelt://删掉
		MSG_LOG("删冲正记录\r\n ");
		//memset((unsigned char*)&re_infor, 0, sizeof(re_infor));
		sysfewrite(addr, sizeof(tmpChs), tmpChs);
		break;
	case FeRC_Check://查看是否有记录
		sysferead(addr, sizeof(tmpChs), tmpChs);
		if (memcmp(tmpChs, RECORD_FALG, 2) != 0)
		{
			*re_infor = 1;
			return;
		}
		//		save_repurse_flash(&re_infor);
		MSG_LOG("没有记录\r\n");
		break;
	default:
		break;
	}
}
#if 0
//开机加载
//mode==1  写进铁电，mode==2删掉铁电记录  3 检查是否有记录需要转正      解决消费未完成  断电没记录的情况
int save_ODA_infor(unsigned char mode, unsigned char *re_infor) {

	//	unsigned int CRC;
	//	unsigned char i;
	unsigned char buff[512];
	unsigned int addr = 0;
	stOdaRecordAddr odaRecord;
	int re_inforLen = 0;
	//	int tmpI = 0;
	//	stTlv tlvCardStyle;
	unsigned char tmpChs[4] = { 0,0,0,0 };
	int retCode = Ret_OK;


	if (mode != ODA_FeRC_Check) {
		MSG_LOG("save_repurse_infor(%02X):%d\n", mode, re_inforLen);
	}

	addr = FLASH_M_REC_START;

	if (mode != ODA_FeRC_Init) {	// 检查目录区
		sysferead(BIT_UNPAY_ODA_RECORD, sizeof(stOdaRecordAddr), (unsigned char *)&odaRecord);
		if (mode != ODA_FeRC_Check)
		{

			MSG_LOG("odaRecord.odaRecordBegin==%x\r\n", odaRecord.odaRecordBegin);
			MSG_LOG("odaRecord.odaRecordEnd==%x\r\n", odaRecord.odaRecordEnd);
			MSG_LOG("odaRecord.odaRecordWrite==%x\r\n", odaRecord.odaRecordWrite);
			MSG_LOG("odaRecord.odaRecordRead==%x\r\n", odaRecord.odaRecordRead);
			MSG_LOG("odaRecord.odaRecordEarase==%x\r\n", odaRecord.odaRecordEarase);
		}
		if (odaRecord.odaRecordBegin != FLASH_M_REC_START) {
			addr = FLASH_M_REC_START;
			flasherase(ERASE4KBYTESECTOR, addr);
			odaRecord.odaRecordEarase = addr + 0x1000;	// 擦写4K
			odaRecord.odaRecordWrite = addr;
			odaRecord.odaRecordRead = addr;
			odaRecord.odaRecordBegin = addr;
			odaRecord.odaRecordEnd = FLASH_M_REC_START + LEN_M_REC;
		}
	}
	switch (mode)
	{
	case ODA_FeRC_Init://初始化
		MSG_LOG("初始化ODA目录结构\n ");
		addr = FLASH_M_REC_START;
		flasherase(ERASE4KBYTESECTOR, addr + 0);
		//flasherase(ERASE64KBYTESECTOR, addr + 0x10000);
		//flasherase(ERASE64KBYTESECTOR, addr + 0x20000);
		//flasherase(ERASE64KBYTESECTOR, addr + 0x30000);
		odaRecord.odaRecordEarase = addr + 0x1000;	// 擦写32K
		odaRecord.odaRecordWrite = addr;
		odaRecord.odaRecordRead = addr;
		odaRecord.odaRecordBegin = addr;
		odaRecord.odaRecordEnd = FLASH_M_REC_START + LEN_M_REC;
		sysfewrite(BIT_UNPAY_ODA_RECORD, sizeof(stOdaRecordAddr), (unsigned char *)&odaRecord);

		break;
	case ODA_FeRC_Write://写记录
		re_inforLen = GET_INT16(re_infor + 2) + 4;
		if (re_inforLen > 0x1000) {
			MSG_LOG("ODA记录无法写入走入4K的记录:%d\n", re_inforLen);
			retCode = Ret_Err_Overflow;
			break;
		}

		// 4K对齐
		addr = odaRecord.odaRecordWrite;
		MSG_LOG("re_inforLen==%d\r\n", re_inforLen);
		MSG_LOG("odaRecord.odaRecordWrite==%x\r\n", odaRecord.odaRecordWrite);
		MSG_LOG("ALIGN_4K_CUR(addr)==%x\r\n", ALIGN_4K_CUR(addr));

		MSG_LOG("ALIGN_4K_CUR(addr + re_inforLen)==%x\r\n", ALIGN_4K_CUR(addr + re_inforLen));
		if (ALIGN_4K_CUR(addr) != ALIGN_4K_CUR(addr + re_inforLen)) {
			//flashwrite(addr, "\xFF\xFF\xFF\xFF", 4);
			addr = ALIGN_4K_NEXT(odaRecord.odaRecordWrite + re_inforLen);
			MSG_LOG("ODA交易要写新的4K, 直接先擦除4K\n");
			flasherase(ERASE4KBYTESECTOR, addr);
			odaRecord.odaRecordEarase = addr + 0x1000;	// 擦写4K
			if (odaRecord.odaRecordEarase >= odaRecord.odaRecordEnd) {
				odaRecord.odaRecordEarase = odaRecord.odaRecordBegin;
			}
			if (odaRecord.odaRecordRead > addr && odaRecord.odaRecordRead < odaRecord.odaRecordEarase) {
				MSG_LOG("ODA交易有记录被擦除了:%d,%d\n", addr, odaRecord.odaRecordRead);
				odaRecord.odaRecordRead = ALIGN_4K_NEXT(odaRecord.odaRecordRead);
			}
		}
		// 是否指针到结束了		
		if (addr > odaRecord.odaRecordEnd) {
			MSG_LOG("ODA记录的写指针异常:%08X,%08X\n", addr, odaRecord.odaRecordEnd);
			addr = odaRecord.odaRecordBegin;
		}
		else if (addr == odaRecord.odaRecordEnd) {
			MSG_LOG("ODA记录的写指针到尾了:%08X,%08X\n", addr, odaRecord.odaRecordEnd);
			addr = odaRecord.odaRecordBegin;
			//flasherase(ERASE4KBYTESECTOR, addr);
		}
		// 写记录
		memcpy(re_infor, RECORD_FALG, 2);
		if ((re_inforLen & 0x01) != 0) {
			re_infor[re_inforLen] = 0xFF;
			++re_inforLen;
		}
		memset(buff, 0, re_inforLen);
		flashwrite(addr, re_infor, re_inforLen);
		delayxms(1);
		flashread(addr, buff, re_inforLen);
		MSG_LOG("读出来数据\r\n");
		BCD_LOG(buff, re_inforLen, 1);
		if (memcmp(buff, re_infor, re_inforLen) != 0)
		{
			MSG_LOG("写失败\r\n");
		}

		odaRecord.odaRecordWrite = addr + re_inforLen;

		sysfewrite(BIT_UNPAY_ODA_RECORD, sizeof(stOdaRecordAddr), (unsigned char *)&odaRecord);

		MSG_LOG("写FLAHS ODA8583记录:");
		BCD_LOG(re_infor, re_inforLen, 1);

		break;
	case ODA_FeRC_Dlelt://删掉
		MSG_LOG("删ODA8583记录\n ");
		addr = odaRecord.odaRecordRead;
		if (addr == odaRecord.odaRecordWrite) {
			MSG_LOG("指针异常%08X\n", addr);
			retCode = Ret_Err_Format;
			break;
		}
		flashread(addr, tmpChs, 4);
		if (memcmp(tmpChs, RECORD_FALG, 2) != 0) {
			MSG_LOG("ODA 8583记录异常%08X\n", addr);
			retCode = Ret_Err_Format;
			break;
		}
		re_inforLen = GET_INT16(tmpChs + 2) + 4;
		addr += re_inforLen;
		if (addr == odaRecord.odaRecordWrite) {

		}
		else {
			flashread(addr, tmpChs, 4);
			if (tmpChs[0] == 0xFF) {
				++addr;
				if (addr == odaRecord.odaRecordWrite) {

				}
				else if (memcmp(tmpChs + 1, RECORD_FALG, 2) != 0) {
					addr = ALIGN_4K_NEXT(addr);
				}
			}
			else if (memcmp(tmpChs, RECORD_FALG, 2) != 0) {
				addr = ALIGN_4K_NEXT(addr);
			}
		}
		if (addr >= odaRecord.odaRecordEnd) {
			addr = odaRecord.odaRecordBegin;
		}
		odaRecord.odaRecordRead = addr;
		MSG_LOG("odaRecord.odaRecordRead dele==%x\r\n", odaRecord.odaRecordRead);
		//memset((unsigned char*)&re_infor, 0, sizeof(re_infor));
		sysfewrite(BIT_UNPAY_ODA_RECORD, sizeof(stOdaRecordAddr), (unsigned char *)&odaRecord);
		break;
	case ODA_FeRC_Check://查看是否有记录
		//MSG_LOG("C");
		addr = odaRecord.odaRecordRead;
		if (addr == odaRecord.odaRecordWrite) {
			retCode = Ret_NO;
			break;
		}

		flashread(addr, tmpChs, 4);
		MSG_LOG("前面4个字节格式:");
		BCD_LOG(tmpChs, 4, 1);
		MSG_LOG("指针状态:");
		if (memcmp(tmpChs, RECORD_FALG, 2) != 0) {
			retCode = Ret_NO;
			break;
		}
		MSG_LOG("前面4个字节格式:");
		BCD_LOG(tmpChs, 4, 1);
		MSG_LOG("指针状态:");
		BCD_LOG((unsigned char *)&odaRecord, sizeof(stOdaRecordAddr), 1);
		retCode = Ret_YES;
		break;
	case ODA_FeRC_READ://查看是否有记录
		addr = odaRecord.odaRecordRead;
		if (odaRecord.odaRecordRead == odaRecord.odaRecordWrite) {
			retCode = Ret_Err_N_Exist;
			break;
		}
		MSG_LOG("odaRecord.odaRecordRead==%x\r\n", odaRecord.odaRecordRead);
		MSG_LOG("addr==%x\r\n", addr);
		flashread(addr, tmpChs, 4);
		if (memcmp(tmpChs, RECORD_FALG, 2) != 0) {
			retCode = Ret_Err_Format;
			break;
		}
		re_inforLen = GET_INT16(tmpChs + 2) + 4;
		if (re_inforLen > 4) {
			re_inforLen = ALIGN_2_NEXT(re_inforLen);
			flashread(addr, re_infor, re_inforLen);
		}
		else {	// 丢弃不要
			odaRecord.odaRecordRead += re_inforLen;
			sysfewrite(BIT_UNPAY_ODA_RECORD, sizeof(stOdaRecordAddr), (unsigned char *)&odaRecord);
			retCode = Ret_Err_Format;
	}
		break;
	default:
		break;
}
	if (mode != ODA_FeRC_Check) {
		MSG_LOG("前面4个字节格式:");
		BCD_LOG(tmpChs, 4, 1);
		MSG_LOG("指针状态:");
		BCD_LOG((unsigned char *)&odaRecord, sizeof(stOdaRecordAddr), 1);

	}
	return retCode;
}

#endif
#endif


/*==========================================================================*
 * 函数	 :	DELAY_SEND
 * 描述   :  延时找任务，发送
 * 建立   :
 *==========================================================================*/
typedef struct
{
	unsigned int time;			//
	//	unsigned char RES[20];
	
}DELAY_SEND_STRUCT;
DELAY_SEND_STRUCT delay_send;
void init_delay_send(void)
{
	memset((unsigned char *)&delay_send,0,sizeof(DELAY_SEND_STRUCT));
}
void set_delay_send(unsigned int val)
{
	delay_send.time=val;
}
unsigned int get_delay_send(void)
{
	
	return delay_send.time;
}
void reduce_delay_send(void)
{
	if(delay_send.time>0)
	{
		//	MSG_LOG("延时找任务减1==%d==\r\n",delay_send.time);
		delay_send.time--;
	}
}
unsigned char  DELAY_SEND(void)
{
	if(delay_send.time==0)
	{
		return ST_OK;
	}
	else
	{
		//		MSG_LOG("刚刷完卡\r\n");
		return ST_ERROR;
	}
}

