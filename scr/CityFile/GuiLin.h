#ifndef _GuangXI_HEAD_
#define _GuangXI_HEAD_

//----无------
#define HandListlen 0
extern const unsigned int gHandList[HandListlen+1];

#define SYS_HEAD_STR	" 桂林公交 "
#define SYS_HEAD_STREn	"  SLZR  "
#define PCityCode "\x00\x27"

#define CLOUD_BUSSINESSNO "\x99\x98"
#define SYS_cityName "GX"

#define GJ_IP 0x64646464
#define GJ_PORT 8819
#define NETAPN "CMNET"

//-----------定义为设备功能配置。-------
	#define _New_Bu_mode_
	//SLZR自已定义 
	#define BUS_PRO_JTB
	#define BUS_CPU_
//	#define CDMA_MC323_
//  M50也开启 MG323定义，使用STGPRSM50Lib.lib
//	#define GPRS_MG323_
	#define ALL4G
//	#define OTHER_GPRS
//	#define COM2RJ45_
//	#define WIFI_TLH_
//从用串口转网口，使用的是透传，和其它的DVR相接。打开COM2RJ45_  和 OTHER_GPRS
//	#define COM2RJ45_
	//外接MODEM，还是要定义上面的无线模块的类型
	#define _MOBILE_IP_CHANGSHA_
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
	//Cloud
// 	#define BUS_Cloud_ 
// 	#define _M1KEY_CLOUD_
	#define _YiDa_Card_
	//*********************************
	//使用外接键盘，现在用COM1
//	#define KEYB_COM 1
//	#define KEYBOARD
	//老的扣款方式（电信的卡用）
	#define _PURSE_OLD_STYLE
	//都外于收费状态
	#define _BU_NO_STOP_
	//没有附加费功能
	#define NO_PlusSta_
	//加入GPS功能,自动报站  
//	#define _GPS_ENABLE_
//	#define _TTSSound_enable_
	//启用分段收费功能，一台车挂两台机。
//	#define FengDuan_BU_
//增加二维码功能,同时指示COM口位置
	#define QR_CODE_COM COM_QR
//蓝牙功能测试 同时指示COM口
	#define _BlueTooth_COM COM6
//是否使用FLASH文件系统
	#define _FASH_FATs_SYS_
	#define Factory_NUM "\x04\x02\x61\x10"


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


