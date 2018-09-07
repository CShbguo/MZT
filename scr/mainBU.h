/*
	File Name	：MAIN.h
	Object		：head file for main.c
	Creation	：22/3/2008 by KSLi
*/

#define CARD_SOUND
/////////////////////////各种卡的设置语音///////////////////////

//***************************************************************************
//                       卡中记录的格式定义 
//***************************************************************************
#define LEN_CARD_RECORD 16

typedef struct
{
	unsigned char DealTime[4];			//交易时间4
	unsigned char Balance[4];		    //交易前余额
	unsigned char DealMoney[3];			//交易金额
	unsigned char DealType;				//交易类型
	unsigned char DeviceNo[4];			//设备号
}cardRecord1;
cardRecord1 gHisMassage;//最近的历史记录

Parameter6 ErrorDetail[3];
unsigned char error_pointer;		  //缓冲区的指针

typedef struct  {
	unsigned char Sec;
	unsigned char CardSno[4];
} card_Delay;
card_Delay PCard_delay;
//*************************************
#define POSTCODE_LEN			4 
#define DEVICENO_LEN			9 
#define DEVICENO1_LEN			4
#define LINENO_LEN				3

//***************************************************************
//				当前消费信息文件结构
//***************************************************************
struct BAK_WG1
{
	unsigned char WGcrc;
	unsigned char wBuffer_po;
	unsigned char GPRS_po;
} ;
struct pMonthTime_1
{
	unsigned char endptr;				
	unsigned char startptr;
	unsigned char LastList[LAST_LIST_LEN][7];              //月票保存时间表
};

typedef struct 
{
	unsigned int strAddr;				//开始地址
	unsigned int endAddr;				//结束地址
	unsigned int fileLength;			//文件长度
	unsigned char hisName[7];			//文件头
	unsigned char fileSno[2];			//文件序列号
	unsigned char resev[3];				//填充
}Parameter10;
Parameter10 hisFCB[10];

stcomtime SysTime;

unsigned int s_sum1,a_sum1,dis_sum2;
unsigned int a_sumR;//手机钱包中的真实金额

unsigned char gucBLight;

unsigned char gucRestorFlag;
unsigned int face_DisTime;

stBusTicketV gBusTicketV;
stBusVerIfo gBusVerInfo;
stbusdownpro gBusDownPro;//程序下载
stbusdownPara gBusblkDownPro;//黑名单下载
stGPSdownLine gGPSDownLine;//线路信息

unsigned char gchn_eng;//中英文标志
unsigned short gErrorFlag;//错误标识,高字节表示错误编号，低字节表示错次数据 详见"*错误编码标识定义"
unsigned int gErrortimes[2];//错误次数据,主要是用于GPRS连接

unsigned char gPSAMin;//psam卡所在位置

#ifdef MOBILE_XIANYANG_
CardUserTimes gUserTimes;//卡使用的次数
#define XIANYANG_STU_CARD_TIMES_ 800
#endif

#ifdef _GPS_ENABLE_
unsigned char gGpsDisEN;	//GPS显示状态 
extern volatile unsigned char ucAlarmFlag;
extern unsigned int GpsDistance(unsigned int dsLa, unsigned int dsLo);
#endif
#ifdef KEY_PAD
extern volatile unsigned char gKeyRestorDis;//需要重新刷新KEYPAD显示
extern void KeyPadDis(unsigned char line,unsigned char col, char*dat, unsigned char len);
extern void KeyPadClr(void);
extern void KeyPad_main(void);
#endif
extern unsigned char Keybuffer[10];
//--------GPS------------------
extern void GPS_PAR_CLR(void);
extern void gps_par_init(void);
extern void main_GpsRs(void);
extern void showGps(unsigned char mode);
//Rstr:返回的字符
extern unsigned char GetInSound(char *Rstr);
extern stSoundInfo gSoundInfo;					//语音


extern void getCpuInfo(stCpuInfo *inP);
extern void savesysbupara(void);




