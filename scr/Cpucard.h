
#ifndef __CPU_CCARD_FILE_HEAD_
#define __CPU_CCARD_FILE_HEAD_

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

extern unsigned int s_sum1;
extern unsigned int a_sum1,dis_sum2;
extern stcomtime SysTime;
extern unsigned char restore_flag;
extern fFlagVary_1 F_FLAG_VARY;

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
	unsigned char cPublisherSign[8];	/* 发卡方标识 */
	unsigned char cAppType;				/* 应用版本号 */
	unsigned char cCitycode[2];			/* 城市代码	*/
	unsigned char cCardType;			/* 卡应用类型 */
	unsigned char cApp_sn[10];			/* 应用序列号 5个00H字节 + 1个卡片类型字节+ 4个流水字节 卡号为：后5个字节 */
	unsigned char cIssueDate[4];		/* 应用启用日期 */
	unsigned char cValidDate[4];		/* 有效日期 */
	unsigned char cSing;				/* 是否记名 */
	unsigned char cYaJin[2];			/* 押金 */
	unsigned char cMinremain[2];		/* 最低余额限制 */
	unsigned char cUseFlah;				/* 使用标记 */
	unsigned char cMaxPur[4];			/* 最高消费金额 */
	unsigned char cLinezf;				/* 联机账户标记 */
	unsigned char cRFU[9];				/* 保留 */
}typea_public_inf_zaozuang;				//枣庄CPU卡信息结构

typedef struct
{
	unsigned char cPublisherSign[2];	/* 发卡方标识 */
	unsigned char cCitycode[2];			/* 城市代码	*/
	unsigned char changyecode[2];		//行业代码
	unsigned char cref[2];				//RFU（预留）
	unsigned char cQiYongflag;			//启用标志
	unsigned char cAppType;				/* 应用版本号 */
	unsigned char ctemp[2];				//保留

	unsigned char cApp_sn[8];			/* 应用序列号 5个00H字节 + 1个卡片类型字节+ 4个流水字节 卡号为：后5个字节 */
	unsigned char cIssueDate[4];		/* 应用启用日期 */
	unsigned char cValidDate[4];		/* 有效日期 */
	unsigned char cMinremain[2];		/* 最低余额限制 */
}typea_public_inf_wuhang;				//武汉CPU卡信息结构

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

extern typea_public_inf stuPubInf;

typedef struct
{
	unsigned char FCcode[2];	//发卡方代码
	unsigned char citycode[2];	//城市代码
	unsigned char tradcoed[2];		//行业代码
	unsigned char money_type;		//应用类型标识 03（电子钱包）
	unsigned char GS_QY;			//公司自己的启用标志
	unsigned char QYflag;			//应用类型标识（启用标志 01启用）1
	unsigned char YYVer;			//应用版本1
	unsigned char FYFlag[2];		//互通标识2
	unsigned char YYNO[8];			//应用序列号8
	unsigned char StartDate[4];		//应用启动日期（YYYYMMDD） 4
	unsigned char EndDate[4];		//应用有效日期（YYYYMMDD） 4
	unsigned char Zicard_catalog;	//子卡类
	unsigned char reserve[1];			//预留2
	unsigned char card_catalog;			//绵州通卡类
//	unsigned char MAC[4];		//卡类型
//	unsigned char reserve2[6];		//预留
}stJSBF15;


typedef  struct 
{
	unsigned char kid;
	unsigned char PublisherSign[8];
	unsigned char ReceiverSign[8];
	unsigned char startDate[4];
	unsigned char endDate[4];
}sam_pub;
//下面是和24G手机卡共用的变量
// #ifdef NO_MOBILE_24G_
// unsigned char G24PsamNo[6];
// unsigned char g24PSAMDsno[4];//终端序列号
// typea_public_inf_zaozuang stuPubInfZaoZuang;
// pboc_debit_capp_purchase stuDebitPurchase;
// pboc_sam_init_purchase stuSamInitPurchase;
// pboc_init_capp_purchase_res stuInitPurchaseRet;
// #else
extern unsigned char G24PsamNo[6];
extern unsigned int g24GDisFlash;//24G卡界面刷新
extern unsigned char g24PSAMDsno[4];//终端序列号
extern typea_public_inf_zaozuang stuPubInfZaoZuang;
extern pboc_debit_capp_purchase stuDebitPurchase;
extern pboc_sam_init_purchase stuSamInitPurchase;
extern pboc_init_capp_purchase_res stuInitPurchaseRet;
// #endif
extern unsigned char gPsamErrorFlag;//PSAM是否出错。
extern unsigned int g24GDisFlash;//24G卡界面刷新
extern pboc_init_capp_purchase_res stuInitPurchaseRet;
extern void end_close_card(unsigned char moded);
#endif

