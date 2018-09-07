
#include "string.h"
#include "stdio.h"
//#include "Board.h"

#include "..\include\include.h"
//#include "public.h"
#include "Gszct.h"
//#include "timer_isr.h"
#include "Mydes.h"
#include "E_Gmobile.h"

extern unsigned int get_s_sum1(unsigned char mode);

extern stcomtime SysTime;
extern unsigned char restore_flag;
extern fFlagVary_1 F_FLAG_VARY;
extern unsigned int g24GDisFlash;//24G卡界面刷新

unsigned char G24PsamNo[6];
unsigned char gG24CardNo[10];//卡号，钱包卡号

unsigned char gG24Ver;//读头版本标识 0xA1 0xA2 0xA3 0xA4(版本号没取到)
unsigned char gG24VerBuf[21];//读头版本
unsigned char gG24ReaderErr;//读头错误次数
/* 公共基本文件 --- 0015 file */
typedef struct
{
	unsigned char cPublisherSign[8];	/* 发卡方标识 */
	unsigned char cAppType;				/* 应用类型标识 */
	unsigned char cCardType;			/* 卡类别标识 */
	unsigned char cApp_sn[10];			/* 应用序列号(国家标识(1),建设部标识(1),城市代码(2),应用代码(2),卡发行流水号(4) */
	unsigned char cIssueDate[4];		/* 发行日期 */
	unsigned char cValidDate[4];		/* 有效日期 */
	unsigned char cCardVer;				/* 卡版本号 */
	unsigned char cAppVer;				/* 应用版本号 */
	unsigned char cRFU[2];				/* 保留 */
}typea_public_inf;

typedef struct
{
	unsigned char cBalance[4];			/* 电子钱包余额 */
	unsigned char cSnq[2];				/* 电子钱包交易序列号 */
	unsigned char cOverdraftLimit[3];	/* 透支限制 */
	unsigned char cKeyVer;				/* 密钥版本 */
	unsigned char cKeyEn;				/* 密钥标识 */
	unsigned char cRand[4];				/* 随机数 */
}pboc_init_capp_purchase_res;
typedef struct
{
	unsigned char cRand[4];				/* 随机数 */
	unsigned char cSnq[2];				/* 电子钱包交易序列号 */
	unsigned char cMoney[4];			/* 交易金额 */
	unsigned char cType;				/* 交易类型 */
	unsigned char cDateTime[7];			/* 交易日期时间 */
	unsigned char cKeyVer;				/* 密钥版本 */
	unsigned char cKeyEn;				/* 密钥标识 */
	unsigned char cKeyFactor[8];		/* 8密钥分散因子 */
	unsigned char cPublisherSign[8];		/* 8密钥分散因子 */
	unsigned char cReaderMAC[12];//读头MAC,用于PSAM卡校验读头是否合法
}pboc_sam_init_purchase;

typedef struct
{
	unsigned char cTermSnq[4];			/* 终端机交易序号 */
	unsigned char cDateTime[7];			/* 交易日期时间 */
	unsigned char cMac[4];				/* 交易MAC码 */
}pboc_debit_capp_purchase;

pboc_debit_capp_purchase stuDebitPurchase;
pboc_sam_init_purchase stuSamInitPurchase;
pboc_init_capp_purchase_res stuInitPurchaseRet;

struct sam_pub
{
	unsigned char kid;
	unsigned char PublisherSign[8];
	unsigned char ReceiverSign[8];
	unsigned char startDate[4];
	unsigned char endDate[4];
}stuSamPubInf_LL;
typea_public_inf stuPubInf;

unsigned char g24GProFlag;//24G卡询卡指针 0:需要发询卡命令 其它:己发询卡命令,等回应卡信息

typedef struct  {
	unsigned char Sec;
	unsigned char CardSno[8];
} stg24card_Delay;
stg24card_Delay g24PCard_delay;//G24卡

unsigned char g24PurError;//刷卡标志错误，需要请重刷上一张卡
unsigned char g24ErrorCardSno[8];//刷卡时出错的卡号
unsigned char g24PSAMDsno[4];//终端序列号

volatile unsigned char g24ComFlag;//串口接收标志
unsigned char g24ComRBuf[500];
unsigned int g24ComRindex;
unsigned short com1Rlen;//接收数据的长度。

