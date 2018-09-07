#ifndef _SLZR_TEST_HEAD_
#define _SLZR_TEST_HEAD_

//----无------
#define HandListlen 0
extern const unsigned int gHandList[HandListlen+1];

#define SYS_HEAD_STR	"城市通卡"
#define SYS_HEAD_STREn	"  SLZR  "
#define PCityCode "\x00\x27"

#define CLOUD_BUSSINESSNO "\x99\x98"
#define SYS_cityName "SL"


#define GJ_IP 0x64646464
#define GJ_PORT 8819
#define NETAPN "CMNET"

//公司平台电信IP:202.96.137.46   联通IP:58.251.128.49
// #define GJ_IP 0x2E8960CA
// #define GJ_PORT 8819
//端口 制卡:8818  消费:8819
//远程：202.96.137.46:2012


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
//	#define BUS_PRO_JSB
	//Cloud
	#define BUS_Cloud_ 
	#define _M1KEY_CLOUD_
	//使用9996商户,此商户是汉威那里演示用。
	//#define CLOUD_9996_
	//定义用线路卡内设置的语音
	//#define AutoAudio_line
	//定义为CPU卡,应该不用2.4G的

//SLZR自已定义 
	#define BUS_CPU_

	#define CPU_JSB		//住建部互联互通
	// #define BUS_PRO_JTB	//交通部互联互通

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
//	#define _CPU_24G_ 
	//厦门盛华电子2.4G读头，使用M1卡的功能，只有发行区、钱包区和公共信息区,密钥使用租赁平台的方法。
				//CPU卡使用的公司的PSAM卡，计算的密钥
//	#define _XMSHDZ_24G_
//	#define G24Com COM5
//新ESF3000E  COM6
//	#define G24Com COM6
	//定义为使用3DES计算密钥
	//#define _M1KEY_3DES_
	//定义为中科讯联2.4G读头
//	#define ZKXL_24G_
//在公司CPU卡区取卡类编号
//	#define qPBOC_CPU_CARDTYPE_
	//定义为银行卡
//	#define qPBOC_BUS
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
	//#define _TTSSound_enable_
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
	//蓝牙功能测试 同时指示COM口
	#define _BlueTooth_COM COM6
	//是否使用FLASH文件系统
	#define _FASH_FATs_SYS_
	#define Factory_NUM "\x04\x02\x61\x10"

//-----------定义为设备功能配置。-------

#ifndef KEYB_COM
#define KEYB_COM 4
#endif

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
#define CARD_STUDENT_BUS	0x42			//2学生卡66 
#define CARD_YOUDAI_BUS		0x43			//9优待卡67  .. 
#define CARD_ZILU_BUS		0x44			//4子女卡 68 桂林-学生优惠卡
#define CARD_STUFFC_BUS		0x45			//5租赁员工卡69  季票卡
#define CARD_OLDMAN_BUS		0x46			//6老年卡70  ..福利卡 老年卡

#define CARD_COUPON_BUS		0x47			//7残优卡71  宜春用来做员工司机卡（季票）..
#define CARD_STUFF_BUS		0x48			//8员工卡72
#define CARD_YOUHUI_BUS		0x49			//9优惠卡73  ..
#define CARD_RELATION_BUS	0x4A			//10家属卡74	桂林-低保成人卡
#define CARD_JILIAN_BU		0x4B			//11纪念卡75	桂林-离休人员卡

#define CARD_HANDICAPPED	0x4C			//12残疾人卡76
#define SCARD_HETONG_BUS	0x4D			//合同员工卡77 广西的军人卡
#define	CARD_PRACTISE_BUS	0x4F			//13学员卡79	桂林-低保学生卡
#define	CARD_JICHA_BUS		0x50			//稽查卡80

#define CARD_CONGCI_BUS		87				//充次卡 月票卡(邵东异型学生卡)

#define CARD_SD_YIXIN		0x56			//86 邵东异型普通卡
#define CARD_TELECOM_USER   0x58			//邵东电信用户卡的卡类型
#define CARD_TELECOM_BUS    0x59			//邵东电信手机卡的卡类型

#define CARD_MOBILE_BUS		0x5A			//90 //移动手机在线路卡中的卡类型

#define CARD_WH_QIYE_DRIVER 105				//武汉的司机卡

#define CARD_NORMAL				CARD_NORMAL_BUS				//普通A卡
#define CARD_STUDENT			CARD_STUDENT_BUS				//学生A卡

#define CARD_OLDMAN				CARD_OLDMAN_BUS				//老年卡
#define CARD_STUFF				CARD_STUFF_BUS				//员工卡
#define CARD_COUPON				CARD_YOUHUI_BUS				//优惠卡
#define CARD_NORMAL_B			CARD_ZILU_BUS				//普通B卡


#define CARD_qPBOC_BUS		0x5B			//91 银行卡卡类 [在有些地方银行卡卡类写成不同的]

#define  CARD_FENDUAN_Line	33		//分段线路票价卡

#endif 


