
#ifndef _FUZHOU_HEAD_
#define _FUZHOU_HEAD_



//************抚州****************/
#define Ver_city "FuZhou"
extern int getCardtypecommon_FuZhou(char *cardD, unsigned char type);
extern void getFixPrice_FuZhou(void);//抚州的无线路卡，写死在程序里面
extern void setAudio_FuZhou(void);
//>>>>>>>>>>>>抚州>>>>>>>>>>>>>>>>

//----无------
#define HandListlen 1
extern const unsigned int gHandList[HandListlen+1];

//银行

#define SYS_HEAD_STR	"城市通卡"
#define SYS_HEAD_STREn	"  SLZR  "
#define SYS_cityName "FZ"
#define PCityCode "\x00\x27"
#define CLOUD_BUSSINESSNO "\x99\x98"	//商户号

#define GJ_IP 0x64646464
#define GJ_PORT 8819
#define NETAPN "CMNET"
//-----------定义为设备功能配置。-------
	#define _New_Bu_mode_
	//SLZR自已定义
//  #define CDMA_MC323_
//  M50也开启 MG323定义，使用STGPRSM50Lib.lib
//	#define GPRS_MG323_
//	#define OTHER_GPRS
	//从用串口转网口，使用的是透传，和其它的DVR相接。打开COM2RJ45_  和 OTHER_GPRS
//	#define COM2RJ45_
//	#define WIFI_TLH_

//	//CDMA 3g模块  CEM630 3g模块
	#define CDMA_3G
//全网通4G
//	#define ALL_4G
	//测试使用
//	#define IP_OF_SLZR_	
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
	#define NORM_DLY_TIME 1

//有年票季票的一定要打开
//	#define New_Times_Card__
	#define line_older_
	//定义为CPU卡,应该不用2.4G的
//  	#define BUS_CPU_

	//定义为银行卡
	#define qPBOC_BUS	
	//都外于收费状态
	#define _BU_NO_STOP_

//设置用户卡卡类型从0开始。普通卡为0
	#define Card_typeM_Min_is0_
//最开始的取PSAM密钥的方法，只有公司的PSAM卡有效
	#define _old_PSAM_GET_KEY_
	//********************************
	//#define _Money_zero_

	//没有附加费功能
	#define NO_PlusSta_
	//压缩字母设备号使用这个宏
//	#define ZBCD_DRICENO	
//-----------定义为设备功能配置。-------

#ifndef KEYB_COM
#define KEYB_COM 4
#endif

///////////////////////////////各种卡的定义/////////////////////////////////
#define CARD_DATA_GATHER	61				//数据采集卡
#define CARD_DEV_SET		62				//设备设置卡
#define CARD_LINE_SET		63				//线路设置卡
#define CARD_NUMBER_SET		64				//数码设置卡
#define CARD_DELAY_SET		65				//延时卡
#define CARD_TOLL_START		66				//收费启动卡
#define CARD_TIME_SET		67				//时间设置卡
#define CARD_LEV_BUS		68				//发车卡
#define CARD_ARV_BUS		69				//到站卡
#define CARD_DRIVER_BUS		70				//司机卡
#define CARD_CLERK_BUS		71				//站务员卡
#define CARD_AIR_BUS		72				//空调卡
#define CARD_MONTH_STA		73				//月票启动卡
#define CARD_WHITE_BUS		50				//白卡	
//---------宝鸡
#define CARD_NORMAL_BUS		0				//普通卡
#define CARD_STUDENT_BUS	1				//学生卡
#define CARD_SX_BUS			2				//寿星卡
#define CARD_STUFF_BUS		3				//员工卡
#define CARD_YH_BUS			4				//优惠卡 //成人月票卡(鄂州市内)
#define CARD_YF_BUS			5				//优扶卡 //成人月票卡(鄂州郊区)
#define CARD_OLDMAN_BUS		6				//老年卡 爱心卡-残疾卡(鄂州)
#define CARD_Love_BUS		7				//爱心卡
#define CARD_ZILV_BUS		8				//子女卡 大众卡（鄂州）
#define CARD_SCHOOL_BUS		9				//校园卡（鄂州）

#define CARD_NORMAL				CARD_NORMAL_BUS				//普通A卡



#endif




