
#ifndef _GPS_SLF_HEAD_
#define _GPS_SLF_HEAD_

#define GPS_MODIFY_LONG_

#include "stdio.h"
#include "string.h"
//#include "board.h"
#include "szct.h"
//#include "public.h"
#include "..\include\include.h"

unsigned char LineRevFlag;	//接收线路信息1 接收中， 0接收完成
unsigned int revLineInfoOut;
unsigned int gGPSStatinoSUM;

//volatile unsigned char gllstyle;//GLL计数，如果超过两次都没有，就认为是G595，发送配置信息.

unsigned char gpsTimeOK;//gps时间正确

#define IDSoundPowerOn 0xa1
#define IDSoundStartST 0xa2
#define IDSoundEndST 0xa3
#define IDSoundOUT 0xa4
#define IDSoundIN1 0xa5
#define IDSoundComm 0xa6
#define IDSoundDefin 0xa7
#define IDSoundRound 0xa8
#define IDSoundFree 0xa9

#define IDSoundOverSpeed 0xB1 //超速
#define IDSoundOverStation 0xB2//越站
#define IDSoundOverSpeedAT 0xB3 //超速
#define IDSoundOverStationAT 0xB4 //超速



stLineInfo LineInfo[2];//线路信息定义包括站点信息 正反两组0组 正向    1组反向
stNSpeedInfo SpeedInfo[2];//限速信息定义 正反两组0组 正向    1组反向

#define RS_IN_SOUND 0x01	//进站报站
#define RS_OUT_SOUND 0x02	//出站报站
#define RS_IN_MAIN 0x04		//到达主站位置
#define RS_IN_SEND 0x10		//进站数据上传
#define RS_OUT_SEND 0x20	//出站数据上传
typedef struct  {
	unsigned char ucStation;			//当前站台号
	unsigned char ucPeStation;			//前一站台号
	unsigned char inOrOut;				//进站出站标志(1进2出)
	unsigned char ucOverSpeed;			//是否超速 正常00超速0C
	unsigned char ucDirect;				//方向 正0 负1
	unsigned char ucFlag;				//标志，是否报站低4字节，传数高4字节
}stRunStation;
stRunStation RunStation;				//运行中的站点信息

typedef struct  {
	unsigned char ucStation[2];
	unsigned int ulDisxx[2];
	unsigned int ulDisnn[2];
	unsigned char ucFlag[3];
}stFindStation;
stFindStation FindS;					//找站点变量

stAlammessage galamessage;

#ifdef _GPS_DIAODU_
//调度命令
stDiaoDuCMD gDiaoDuCMD;
#endif
//调度营运信息
stDiaoDuinfo gDiaoDuinfo;

typedef struct  {
	unsigned char Head[3];				//8A8A8A
	unsigned char deviceNo[4];			//设备号
	unsigned char cmdgps[6];			//8000000000F0 
	unsigned short length;				//长度
	unsigned char ucAlarmFlag;			//报警标志
	unsigned char dataBuf[50];			//报警内容
	unsigned short CrcCode;				//2字节校验
}stAlARM;

typedef struct {
	unsigned char flag;					//站点编号
	unsigned char direct;				//方向
	unsigned char buff[7];				//日期时间
}stGpsFED;
stGpsFED gGpsFeD;						//GPS方向存于铁电中

#define START_MAX 0x03					//进入发车点的最大次数
unsigned char gStartFlag;				//起点站标志（0x00:没有进入起始发车点,其它:进入了发车点）

stSoundInfo gSoundInfo;				//语音

//stLcdCmd gLcdCmd;

volatile unsigned char ucAlarmFlag;				//报警标志

#define GPS_SEND_DLY 5				//GPS非站点上报时间
#define GPS_RE_SEND_DLY	65000			//重发GPS信息
#define GPSVALID				0x41	//有效
#define GPSINVALID				0x56	//无效
#define GPSUES					0xaa	//使用中

GPS_INFO gGpsInfo;		//GPS信息,应用程序判断的缓冲
stGpsPar gGPsPar;				//基本信息

#define GPS_RCV_BUFFER_SIZE 100

volatile unsigned char gGStyle=0;//GPS信息类型
volatile unsigned char gucGPSindex=0;
unsigned char gps_RcvBuf[GPS_RCV_BUFFER_SIZE+1];	//GPS信息接收缓冲
volatile unsigned char gGpscand;//收到了多少种GPS命令
typedef struct
{
	unsigned char GGA[100];
	unsigned char GLL[100];
	unsigned char VTG[100];
	unsigned char ZDA[100];
	unsigned char RMC[100];
}stGpscmdbuf;
stGpscmdbuf gGpsCmdBuf;

//---------------------------------------------
extern unsigned char GetDateTime(void);
extern unsigned int GetCrcWord(unsigned char* ptr,int length);
extern unsigned int GpsDistance(unsigned int dsLa, unsigned int dsLo);
extern unsigned int GpsDistanceTow(unsigned int srLa, unsigned int srLo, unsigned int dsLa, unsigned int dsLo);
extern fFlagVary_1 F_FLAG_VARY;
extern pFistVary_1 pFistVary;
extern unsigned char DriveCardNo[4];
extern Parameter2 POINT_MESSAGE;
extern stcomtime SysTime;

//-------------gprs-----------------------

#define TCP_ID_VER 0x01		//TCP包版本
#define DVICE_STYLE 0x01	//设备类型

#define GPSDATA_LEN	34					//记录长度
extern unsigned char GPRSLinkProcess;
extern unsigned char GprsCardList[16];
extern unsigned int ClientIP;
extern unsigned short ClientPort;

//mobile
extern unsigned char gmissflag;//任务标志 0x00 登陆移动,签到.
//0x01 下载参数
//0x02 程序下载请求
//0x03 程序下载结果通知
//0x04 批结算
//0x05 批上送交易
//0xF0 登陆公交后台
#endif
