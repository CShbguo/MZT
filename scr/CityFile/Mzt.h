//绵州通 权鹏飞
////绵州通  现有的M1卡,本地CPU卡和交通部的卡

#ifndef _MZT_HEAD_H__
#define _MZT_HEAD_H__


#define SETION3   3
#define SETION5   5
#define SETION6   6
#define SETION7   7

typedef struct
{
   unsigned char bytes[16];
}ICCard_block12;

typedef struct     //钱包
{
	unsigned char Purse[4];//钱包
	unsigned char InPurse[4];//钱包取反
	unsigned char PurseBak[4];//钱包
	unsigned char Verify[4];//校验
}ICCard_block13;


typedef struct
{
   ICCard_block12 block12;
  ICCard_block13 block13,block14;  //钱包，钱包备份
}ICCard_section3;

typedef struct     //信息区
{
	unsigned char bimonthlyDate[4];//双月日期
	unsigned char singleMonthDate[4];//单月日期
	unsigned char cardType;//卡类
	unsigned char publishBicker[5];//卡号
	unsigned char remain[2];  //保留
}ICCard_block20;

typedef struct      //月票备份
{
	unsigned char monthPurse[4];//钱包
	unsigned char monthInPurse[4];//钱包取反
	unsigned char monthPurseBak[4];//钱包
	unsigned char monthVerify[4];//校验
}ICCard_block21;


typedef struct
{
  ICCard_block20 block20;
  ICCard_block21 block21,block22;
}ICCard_section5;

typedef struct
{
   unsigned char bytes[16];
}ICCard_block24;

typedef struct
{
   unsigned char bytes[16];
}ICCard_block25;

typedef struct
{
   unsigned char remain[8];
   unsigned char evaluationTime[4];     //老人卡，残疾人卡年审时间判断
   unsigned char remain2[4];
}ICCard_block26;

typedef struct
{
   ICCard_block24 block24;
   ICCard_block25 block25;
   ICCard_block26 block26;
}ICCard_section6;

typedef struct
{
   unsigned char remain[4];
   unsigned char saleCardTime[6];
   unsigned char deviceNumber[6];
}ICCard_block28;

typedef struct
{
   unsigned char remain[12];
   unsigned char rechargeAmount[4];
}ICCard_block29;

typedef struct
{
   unsigned char bytes[16];
}ICCard_block30;

typedef struct
{
   ICCard_block28 block28;
   ICCard_block29 block29;
   ICCard_block30 block30;
}ICCard_section7;

typedef struct
{
   unsigned char keya[4][8];			//密码A加密密钥		53
   unsigned char keyb[4][8];			//密码A加密密钥		53
   unsigned char keyc[4][8];			//密码A加密明文		85
   unsigned char keyr[16];			//记录加密密钥		101
   unsigned char keySetion[7][8];
}ICCard_Key;

typedef struct
{
	//卡结构
	ICCard_section3 section3;    //钱包
	ICCard_section5 section5;   //信息区，月票区
	ICCard_section6 section6;
	ICCard_section7 section7;
	ICCard_Key key;
}stICCard;


typedef struct//参数文件开机读取
{

	unsigned char M1StopFlag;       	//参数1   0x31:关闭M1   0x30:M1卡可使用
	unsigned char Monthmun[9];			 //参数预留

}stPARA_ARRAY;
//stPARA_ARRAY MZT_Para;

typedef struct 
{
	unsigned char cardProFlag[2];	//发卡方标示
	unsigned char cityCode[2];	//城市代码
	unsigned char hangYeCode[2];	//行业代码
	unsigned char reserve[2];	//预留
	unsigned char canUseFlag;//应用类型标识
	unsigned char reserve2[5];		//应用版本
	unsigned char deposit[4];
	unsigned char appSerail[10];	//钱包标识0.5版本0.5
	unsigned char blackFlag;	//初始化日期
	unsigned char cardType[2];	//卡应用类型标志
	unsigned char issuingData[4];	//启用标识
	unsigned char issuingMach[6];
	unsigned char appVersion;	//卡类型标识
	unsigned char reserveExt[9];	//预留
}MF_PublicBacinformation1;

//94 文件
typedef struct 
{
	unsigned char saleTime[4];	// 售卡日期
	unsigned char cardExpirationDate[4];	//终止日期
	unsigned long depositAmount;	// 押金
	unsigned char sourceCode[6];//发售点代码
	unsigned char operatorCode[8];		//操作员代码
	unsigned char enable;	//启用标志
	unsigned char blackFlag;	//黑名单标识
	unsigned char cardType[2];	//卡类型
	unsigned char reserve[2];	//预留
}SaleInformation1;


#define MZT_BC
//#define MZT_ST
//#define MZT_JL
//#define MZT_YT

//----无------
#ifdef MZT_BC
#define code_96_57  0x80     //城区代码   0x80:北川  0x40:盐亭 0x20:三台  0x10:梓潼  0x08:安县  0x04:平武县
#define SYS_HEAD_STR	"北川公交"
#define SYS_HEAD_STREn	"  BCGJ  "
#define CLOUD_BUSSINESSNO "\x00\x01"//"\x99\x98"//
#elif defined MZT_ST
#define code_96_57  0x20     //城区代码   0x80:北川  0x40:盐亭 0x20:三台  0x10:梓潼  0x08:安县  0x04:平武县
#define SYS_HEAD_STR	"三台公交"
#define SYS_HEAD_STREn	"  STGJ  "
#define CLOUD_BUSSINESSNO "\x00\x03"//"\x99\x98"//
#elif defined MZT_JL
#define code_96_57  0x03     //城区代码   0x80:北川  0x40:盐亭 0x20:三台  0x10:梓潼  0x08:安县  0x04:平武县
#define SYS_HEAD_STR	"九龙公交"
#define SYS_HEAD_STREn	"  JLGJ  "
#define CLOUD_BUSSINESSNO "\x00\x02"//"\x99\x98"//
#elif defined MZT_YT
#define code_96_57  0x40     //城区代码   0x80:北川  0x40:盐亭 0x20:三台  0x10:梓潼  0x08:安县  0x04:平武县
#define SYS_HEAD_STR	"盐亭公交"
#define SYS_HEAD_STREn	"  YTGJ  "
#define CLOUD_BUSSINESSNO "\x00\x04"//"\x99\x98"//
#endif


#define PCityCode "\x00\x27"

//#define CLOUD_BUSSINESSNO "\x99\x98"
#define SYS_cityName "SL"


//#define GJ_IP 0x0A0F89B6		
///#define GJ_PORT 8095


#define GJ_IP 0x023E03772	 //114.55.224.35 8819
#define GJ_PORT 8819

#define NETAPN "CMNET"
#define SWITCH_EC_CASH		1	// 电子现金
#define card_type_qpboc
//公司平台电信IP:202.96.137.46   联通IP:58.251.128.49
// #define GJ_IP 0x2E8960CA
// #define GJ_PORT 8819
//端口 制卡:8818  消费:8819
//远程：202.96.137.46:2012


//#define JIULONG 

//-----------定义为设备功能配置。-------
	#define _New_Bu_mode_
// 	#define BUS_PRO_JTB
// 	#define  BUS_CPU_
	//SLZR自已定义 
//	#define CDMA_MC323_
//  M50也开启 MG323定义，使用STGPRSM50Lib.lib
//	#define GPRS_MG323_
	#define ALL4G
// 	#define OTHER_GPRS
//	#define WIFI_TLH_
//从用串口转网口，使用的是透传，和其它的DVR相接。打开COM2RJ45_  和 OTHER_GPRS
// 	#define COM2RJ45_
	//外接MODEM，还是要定义上面的无线模块的类型
	//#define Modem_BUS_
	//用公司平台
	//#define IP_OF_SLZR_
	//启用串口测试
	//#define COM_TEST_
	//********移动POSP平台下面只能开一个***************移动POS平台定义
	//使用湖北移动网络

	//#define _MOBILE_IP_HUBEI_
	//使用长沙测试平台
	#define _MOBILE_IP_CHANGSHA_
	#define KEYBOARD
	//使用福建平台
	//#define _MOBILE_IP_FUJIAN_
	//使用西安平台
	//#define _MOBILE_IP_XiAN_
	//使用清海平台（代理商）
	//#define _MOBILE_IP_QINHAI
	//不写死IP地址
	//#define _MOBILE_IP_NOLOCK_
	//*********************************
	//*******************************************网络定义*******
	//使用公网 APN，不使用专用移动网络,除了湖北移动，其它的都要打开此定义
	#define _MOBILE_CMNET_
	//没有公交后台的情况
//	#define NO_GJ_SERVISE
	//没有移动后台//没有GPRS功能，也没有移动GPRS
	#define NO_MOBILE_SERVISE
	//定义不使用2.4G卡
	#define NO_MOBILE_24G_ 
	//*********************************
	//***************************************使用公交PSAM卡的种类***********
	#define NORM_DLY_TIME 0
	//启用分段收费功能，一台车挂两台机。
//	#define FengDuan_BU_
		//前门带GPS功能的机器。刷卡时写入刷卡的地点，刷卡次数，不扣钱
		//只有一台车，又刷卡又扣钱
//	#define FengDuan_only_1
	//启用月票类型，主要是有以前的卡这个位置的值不确定，所以不能有这功能，以后的程序都应带此定义
	//定义可通过键盘输入当前站点、方向。键盘输入有效，GPS正常时也是有效的。[用于分段收费-相关说明：]
//	#define _GPS_Station_ExKey_enable_
//	#define New_Times_Card__
	//定义为建设部的卡M1
	#define BUS_PRO_JSB
	//Cloud
// 	#define BUS_Cloud_ 
// 	#define _M1KEY_CLOUD_
	//使用9996商户,此商户是汉威那里演示用。
	//#define CLOUD_9996_
	//定义用线路卡内设置的语音
	//#define AutoAudio_line
	//定义为CPU卡,应该不用2.4G的

//SLZR自已定义 
	#define BUS_CPU_

//	#define CPU_JSB		//住建部互联互通
	#define BUS_PRO_JTB	//交通部互联互通

	//福建一卡通
	//		#define FUJIAN_EKT_CPU_
	//中控CPU卡--没有PSAM卡,要打开 NO_CPU_PSAM_ - D:\CUR WORK\DOC\通讯协议需求\中控_大庆
	//#define ZongKong_CPU_
		//不用专门的CPU 卡的 PSAM卡，密钥通过M1卡的PSAM算出，MAC1用软件计算。(各2.4G读头中也有效)
//	#define NO_CPU_PSAM_
		//九江CPU卡需要选择PSE 中钞的PSE："\x02\x00\xA4\x04\x00\x0E\x31\x50\x41\x59\x2E\x53\x59\x53\x2E\x44\x44\x46\x30\x33"
//	#define CPU_PSE "\x02\x00\xA4\x04\x00\x0E\x31\x50\x41\x59\x2E\x53\x59\x53\x2E\x44\x44\x46\x30\x33"
				//胜华电子的移动手机卡，13.56M 厦门盛华13.56MHZ手机卡测试密钥
//			#define CPU_PSE "\x02\x00\xA4\x04\x00\x10\xD1\x56\x00\x00\x40\x10\x00\x30\x00\x00\x00\x01\x00\x00\x00\x00"
	//移动2.4G手机CPU卡，不是移动区，公交专用区
	#define _CPU_24G_		//国民技术读头
//新ESF3000E  COM6
 	#define G24Com COM6
	//厦门盛华电子2.4G读头，使用M1卡的功能，只有发行区、钱包区和公共信息区,密钥使用租赁平台的方法。
				//CPU卡使用的公司的PSAM卡，计算的密钥
//	#define _XMSHDZ_24G_
//	#define G24Com COM5
	//定义为使用3DES计算密钥
	//#define _M1KEY_3DES_
	//定义为中科讯联2.4G读头
//	#define ZKXL_24G_
//在公司CPU卡区取卡类编号
//	#define qPBOC_CPU_CARDTYPE_
	//定义为银行卡
	#define qPBOC_BUS
//	==1 安顺句柄 
//	#define qPBOC_HAND 1
//	== 3没有任何句柄限制
//	#define qPBOC_HAND 3
//	==2 钟国营，需要读出公交卡类信息。
//	#define qPBOC_HAND 2
	//定义可以询TYPE B的卡
//	#define Card_typeB_enable_
	//*********************************
	//使用外接键盘，现在用COM1
//	#define KEYBOARD
	//老的扣款方式（电信的卡用）
	#define _PURSE_OLD_STYLE
	//都外于收费状态
	#define _BU_NO_STOP_
	//开启刷卡计数功能
//	#define _Counter_SWcard_times
//设置用户卡卡类型从0开始。普通卡为0
//	#define Card_typeM_Min_is0_
//最开始的取PSAM密钥的方法，只有公司的PSAM卡有效
//	#define _old_PSAM_GET_KEY_
	//********************************
	//只扣一分钱
//	#define _Money_purchase_test
	//#define _Money_zero_
	//************************************************GPS 相关定义**********
	//加入GPS功能,自动报站  
//	#define _GPS_ENABLE_
	//定义使用语音功能 使用M50及65定义和以前的模块一样，用的库和C文件不一样。
//	#define _TTSSound_enable_
	//有调度功能
	//#define _GPS_DIAODU_
//发送到站记录，GPS信息10发送一次
//	#define _GPS_NEED_REC_
	//用于采点机，和PC机相连
	//#define _GPS_CAIDIAN_
	//记录车载机的GPS轨迹，每秒写一次GPS记录
	//#define _GPS_TRA_REC_
	//用于串口输入GPS信息。
//	#define _GPS_TEST_
	//使用外接控制屏用COM4
	//#define KEY_PAD
	//具有GPRS模块语音功能
	//#define GPRS_SOUD_ENABLE
	//定义使用以前16段语音
	//#define AUDIO_SOUND_OLD
	//没有附加费功能
	#define NO_PlusSta_

	//增加二维码功能,同时指示COM口位置
	#define QR_CODE_COM COM_QR
// 	//蓝牙功能测试 同时指示COM口
// 	#define _BlueTooth_COM COM6
	//是否使用FLASH文件系统
	#define _FASH_FATs_SYS_
	#define Factory_NUM "\x04\x22\x65\x90"

//-----------定义为设备功能配置。-------

// #ifndef KEYB_COM
// #define KEYB_COM 4
// #endif





/*
M1			
M1	普通卡	无	41
M1	优惠卡	无	42
M1	A员工卡	无	43
M1	B老人卡	无	44
M1	C卡残疾人卡	无	45
M1	D卡	无	46
M1	E卡	无	47
M1	学生卡	无	48
M1	子女卡	无	49


绵州通	普通卡		88	81
绵州通	员工卡		88	83
绵州通	老年卡		88	84
绵州通	残疾卡		88	85
绵州通	学生卡		88	88
绵州通	员工子女卡	88	89
手机2.4G、NFC卡		89	绵州通子卡类折扣	按绵州对应卡类走

*/


///////////////////////////////各种卡的定义/////////////////////////////////
#define CARD_DATA_GATHER	1				//数据采集卡
#define CARD_DEV_SET		2				//设备设置卡
#define CARD_LINE_SET		3				//线路设置卡
#define CARD_NUMBER_SET		4				//数码设置卡
#define CARD_DELAY_SET		5				//延时卡
#define CARD_TOLL_START		6				//收费启动卡
#define CARD_TIME_SET		7				//时间设置卡
#define CARD_LEV_BUS		8				//发车卡
#define CARD_ARV_BUS		9				//到站卡
#define CARD_DRIVER_BUS		10				//司机卡
#define CARD_CLERK_BUS		11				//站务员卡
#define CARD_AIR_BUS		12				//空调卡
#define CARD_MONTH_STA		13				//月票启动卡
#define CARD_Business		49				//商户设置卡
#define CARD_WHITE_BUS		50				//白卡	
#define CARD_KEY_BUS		60				//密钥卡	
///////////////////////////////乘客卡32种,范围40H~5FH//////////////////
#define CARD_NORMAL_BUS		0x41			//1普通卡65
#define CARD_YOUHUI_BUS		0x42			//优惠卡73  ..
#define CARD_STUFF_BUS		0x43			//9优待卡67  .. 绵州通员工卡
#define CARD_OLDMAN_BUS		0x44			//B老人卡
#define CARD_Love_BUS		0x45			//轻度残疾人卡
#define CARD_Love_BUS_2		0x46			//重度残疾人卡
#define CARD_STUDENT_BUS	0x48			//学生卡
#define CARD_ZILU_BUS		0x49			//子女卡 
#define CARD_UNION_BUS		0x4A			//交通联合卡
#define CARD_QR_BUS			0x4B			//二维码卡
#define CARD_PBOC_CASH		0x4C			//电子现金 76	
#define CARD_QPBOC_BUS		0x4D			//银联卡   77
#define CARD_QR_PBOC		0x4E            //银联二维码 78

#define CARD_NORMAL				CARD_NORMAL_BUS				//普通A卡
#define CARD_STUDENT			CARD_STUDENT_BUS				//学生A卡

#define CARD_OLDMAN				CARD_OLDMAN_BUS				//老年卡
#define CARD_STUFF				CARD_STUFF_BUS				//员工卡
#define CARD_COUPON				CARD_YOUHUI_BUS				//优惠卡
#define CARD_NORMAL_B			CARD_ZILU_BUS				//普通B卡


#define  CARD_FENDUAN_Line	33		//分段线路票价卡







































extern unsigned char second_money_flag;

unsigned char Card_typeProcess_MZT(unsigned char mode);


int set_line_mzt(char *infor);

//U盘取线路信息，返回0：返回<0:失败，返回>0成功
int usb_loadlineinfor(void);


#endif 


