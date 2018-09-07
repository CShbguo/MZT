/********************************************
*GPRS公交协议处理程序
*支持JYS的公交协议接收程序
*编写：KSLi 
*********************************************/
#include <stdlib.h>
#include "string.h"
#include "stdio.h"
//#include "Board.h"

#include "..\include\include.h"
//#include "public.h"
#include "szct.h"

#include "Gszct.h"
#include "E_Deal24GCard.h"
#include "E_Gmobile.h"

#include "FenDuanTicket.h"

//#define _debug_blk

#define DDMAX_RCV_PACKET_NUM 2

extern stLineInfo LineInfo[2];//线路信息定义包括站点信息 正反两组0组 正向    1组反向

extern unsigned char StufferNO[4];
extern pFistVary_1 pFistVary;
extern stcomtime SysTime;
extern stPricetable NewPriceTab;
//extern unsigned char gbusblkbuf[30];//下载程序、黑名单、线路及参数时的缓存
extern stbusdownPara gBusblkDownPro;//黑名单下载，指向gbusblkbuf
#ifdef _GPS_DIAODU_
extern stAlammessage galamessage;
#endif
//黑名单每块条数
extern unsigned short gBlackBlockNum[16];

extern stBusTicketV gBusTicketV;
extern stBusVerIfo gBusVerInfo;

extern stbusdownpro gBusDownPro;
extern stGPSdownLine gGPSDownLine;//线路信息
extern unsigned char restore_flag;
extern unsigned char gTTime[14];
unsigned char gmissOldflag;//上个任务的任务标志 0x00 登陆移动,签到.
unsigned char gmissflag;//任务标志 0x00 登陆移动,签到.
extern unsigned char DriveCardNo[4];
extern Parameter3 ComSumeFile;
extern Parameter2 POINT_MESSAGE;
extern unsigned char GPRSLinkProcess;
extern unsigned char GprsCardList[16];
extern fFlagVary_1 F_FLAG_VARY;
extern unsigned char gchn_eng;//中英文标志
extern unsigned short gErrorFlag;//错误标识,高字节表示错误编号，低字节表示错次数据 详见"*错误编码标识定义"
extern unsigned int gErrortimes[2];//错误次数据,主要是用于GPRS连接
extern unsigned char gGPRS_Cmd_outtime;//GPRS包超时计数

extern TCP_IP_PACKET1 tcpipPacket;
#ifdef _GPS_DIAODU_
//调度信息
extern stDiaoDuCMD gDiaoDuCMD;
//调度营运信息
extern unsigned int Build_GJ_ALAM(unsigned char *dat);
#endif
extern stDiaoDuinfo gDiaoDuinfo;
#ifdef KEY_PAD
extern volatile unsigned char gKeyRestorDis;//需要重新刷新KEYPAD显示
#endif
#ifdef SelMsg_enable
extern int findSelMsg(void);
extern void SelMsgInit(void);
extern void addSelMsg(char *SelNo, char *msg, unsigned char len);
#endif

extern void saveCpuInfo(unsigned char mode, unsigned char *dat);
extern unsigned int ClientIP;
extern unsigned short ClientPort;
#ifdef GPRS_MG323_
extern unsigned char gTcpIpbusy;//模块是正正在发数据，模块工作是发命令，然后回应后再发数据。在等回应时不能发其它数据。
extern volatile stGPrsDataStyle gGPRS_data_style;
extern unsigned char gMC55iSendbuff[256];//MC55i发送数据缓冲，前8字节为：“数据有效”+长度（4）+数据...
extern void gprs_DATA_snd(unsigned int len, unsigned char *dat);
#endif
extern unsigned short BCC( unsigned char *Bcc_pointer,unsigned short x);
extern void stopCCgprs(unsigned short mode);
//取记录指针
extern void savesysbupara(void);
extern void readmobilsendStyle(void);
extern unsigned char posnoIsOK(unsigned char *dat);
extern unsigned int Build8583Packge(unsigned char *revBuf, unsigned char mode);

extern unsigned char checkNewPrice(void);
extern unsigned int IsInBlackList(unsigned char *c_card_no);
extern unsigned char GetDateTime(void);
extern void Load_Line_info(void);
extern void getProVer(void);
#ifdef GPRS_MG323_
extern void initTCPIPnode(void);
#endif
extern unsigned char SYSgetbussinessNO(unsigned char *dat);
extern unsigned char gGPSdly;//GPS延时
extern unsigned char isNetOK[2];//两路链接是否成功;
extern void find_new_mission(void);
extern void find_G_new_mission(void);//此任务一秒进一次;
extern unsigned int BuildGJPackge(unsigned char *revBuf, unsigned char mode);
extern void gprs_send_data(unsigned char linkNum, unsigned int len, unsigned char *dat);
extern void MC55i_SISREAD(void);
extern void tcpipClose(unsigned char mode);

#ifdef _GPS_ENABLE_
extern unsigned int Build_GJ_GPS(unsigned char *dat);
#endif
extern void out_GPRS(void);
extern unsigned char gAPN_Style;//APN状态，是专用网还是公网
#define GPRSCOMMAX 1024
extern unsigned char gprsRevBuf[GPRSCOMMAX];//GPRS接收缓冲
extern volatile unsigned short gprsIndex;
extern unsigned char gprsErrorTimes;//出现ＥＲＲＯＲ返回错误次数
extern PACKET_RCV gPacketRcvBuf[MAX_RCV_PACKET_NUM];	//接收帧缓冲


#ifndef NO_GJ_SERVISE
unsigned char gsendError;//发送错误次数，例：用于程序发送多次无响应时停止发送
unsigned short BlackLength;
unsigned int Sendpincount;//重发响应或心跳包计数
unsigned int gSendGLogin;//登陆命令
unsigned int gGprsResertTime;	//重启GPRS模块记数，GPRS模块如果没收到数据10分钟重启一次。
#endif

extern unsigned char BUflag;//车载机状态 0x02 需要刷司机卡 
volatile unsigned char gpacSNo;//包序号

#ifdef SelMsg_enable
extern stSelMsg gsendMsg;
#endif


void gprs_send_data_OTHER_CMD(unsigned char cmd, unsigned int len, unsigned char *dat)
{
	unsigned char buff[200];
	unsigned int pos=0;
	unsigned int CRC;
	unsigned short i;
	
	memset(buff, '$', 4);//包头
	pos = 4;
	
	i = len+10;
	memcpy(buff+pos, (unsigned char*)&i, 2);
	over_turn(2, (unsigned char*)buff+pos);//数据长度，数据类型+应用层数据 的长度,高字节在前
	pos += 2;
	
	buff[pos++] = cmd;//命令
	buff[pos++] = gpacSNo;//包序号
	memcpy(buff+pos, dat, len);
	pos += len;
	
	CRC = crc16_isr((unsigned char*)buff, pos);
	
	memcpy(buff+pos, (unsigned char*)&CRC, 2);
	//over_turn(2, buff+pos);
	pos += 2;
	com_snd(COM1, pos, (unsigned char*)buff);
}
#ifdef OTHER_GPRS
void gprs_other_sndbuinfo(void)
{
#ifndef COM2RJ45_
	unsigned char ddbuff[32];
	unsigned int pos = 0;
	
	memcpy(ddbuff+pos, pFistVary.DeviceNo, 8); pos += 8;
	Get_SerialNum(ddbuff+pos); pos += 4;
	memcpy(ddbuff+pos, StufferNO, 4); pos += 4;
	memcpy(ddbuff+pos, pFistVary.LineNo, 2); pos += 2;
	
	gprs_send_data_OTHER_CMD(0x02, pos, ddbuff);
#endif
}
#endif
//----------------外接调机传数据-----------------
void gprs_send_data_OTHER(unsigned char linkNum, unsigned int len, unsigned char *dat)
{
	unsigned char buff[1200];
	unsigned int pos=0;
	
#ifdef COM2RJ45_
	memcpy(buff, dat, len);
	pos = len;
#else		 
	unsigned int CRC;
	unsigned short i;
	memset(buff, '$', 4);//包头
	pos = 4;
	
	i = len+10;
	memcpy(buff+pos, (unsigned char*)&i, 2);
	over_turn(2, (unsigned char*)buff+pos);//数据长度，数据类型+应用层数据 的长度,高字节在前
	pos += 2;
	
	buff[pos++] = 0x01;//公交数据，直接发送到后台，此字节不需要发送
	buff[pos++] = gpacSNo;//包序号
	memcpy(buff+pos, dat, len);
	pos += len;
	
	CRC = crc16_isr((unsigned char*)buff, pos);
	
	memcpy(buff+pos, (unsigned char*)&CRC, 2);
	//over_turn(2, buff+pos);
	pos += 2;
#endif
	com_snd(COM1, pos, (unsigned char*)buff);
}
//调度设备传回数据
void GPSDD_Deal(unsigned char mode, unsigned char pasn)
{
	unsigned char sendbuf[200];
	unsigned char pos=0;
	unsigned int CRC;

	memset(sendbuf, '$', 4);
	pos = 4;
	switch(mode){
	case 0x21://转发服务器文件
		sendbuf[pos] = 0; pos++;
		sendbuf[pos] = 10; pos++;
		sendbuf[pos] = 0x21; pos++;
		sendbuf[pos] = pasn; pos++;//包序号
		break;
	case 0x22://车载机取IC 卡机设备信息
		sendbuf[pos] = 0; pos++;
		sendbuf[pos] = 0x1C; pos++;
		sendbuf[pos] = 0x22; pos++;
		sendbuf[pos] = pasn; pos++;//包序号
		memcpy(sendbuf+pos, pFistVary.DeviceNo, 8); pos += 8;
		Get_SerialNum(sendbuf+pos); pos += 4;
		memcpy(sendbuf+pos, StufferNO, 4); pos += 4;
		memcpy(sendbuf+pos, pFistVary.LineNo, 2); pos += 2;
		break;
	case 0x23://刷卡人次上传报
		sendbuf[pos] = 0; pos++;
		sendbuf[pos] = 12; pos++;
		sendbuf[pos] = 0x23; pos++;
		sendbuf[pos] = pasn; pos++;//包序号
		sendbuf[pos++] = 0;//上车刷卡人数
		sendbuf[pos++] = 0;//下车刷卡人数
		break;
	case 0x24://刷卡人次清除报
		sendbuf[pos] = 0; pos++;
		sendbuf[pos] = 10; pos++;
		sendbuf[pos] = 0x24; pos++;
		sendbuf[pos] = pasn; pos++;//包序号
//		gOTGrc = 0;
		break;
	case 0x25://司机上班记录报应答
		sendbuf[pos] = 0; pos++;
		sendbuf[pos] = 26; pos++;
		sendbuf[pos] = 0x25; pos++;
		sendbuf[pos] = pasn; pos++;//包序号
		memcpy(sendbuf+pos, (unsigned char*)&SysTime, 7);
		pos += 7;
		if(F_FLAG_VARY.stop_flag){
			sendbuf[pos++] = 0;
		}
		else
			sendbuf[pos++] = 1;
		memset(sendbuf+pos, 0, 8);
		memcpy(sendbuf+pos+4, DriveCardNo, 4);
		over_turn(4, sendbuf+pos+4);
		pos += 8;
		break;
	default:
		break;
	}
	CRC = crc16_isr(sendbuf, pos);
	memcpy(sendbuf+pos, (unsigned char*)&CRC, 2);
	over_turn(2, sendbuf+pos);
	pos += 2;

	com_snd(COM1, pos, sendbuf);
}
#ifdef OTHER_GPRS
#ifdef COM2RJ45_	//直接收BDBDBD的数据
void GPRS_BDBD_Data_hand(unsigned char value)
{
	unsigned short plen;
	unsigned char ii;
	
	ii = value;
	//	debugdata((unsigned char*)&ii, 1, 0);
	switch (gprsIndex)
	{
	case 0:
	case 1:
	case 2:
		if(ii != 0xBD){
			gprsIndex = 0;
		}
		else{
			gprsRevBuf[gprsIndex++] = ii;
		}
		break;
// 	case 14://长度高位
// 		gprsRevBuf[gprsIndex++] = ii;
// 		break;
// 	case 15://长底低位
// 		gprsRevBuf[gprsIndex++] = ii;
// 		memcpy((unsigned char*)&plen, gprsRevBuf+14, 2);
// 		over_turn(2, (unsigned char*)&plen);
// 		if(plen > 2048){//长度错误
// 			gprsIndex = 0;
// 			memset(gprsRevBuf,0,10);
// 			break;
// 		}
// 		break;
	default:
		gprsRevBuf[gprsIndex++] = ii;
		if(gprsIndex <= 15)
			break;
		memcpy((unsigned char*)&plen, gprsRevBuf+14, 2);
		over_turn(2, (unsigned char*)&plen);
		if(plen > 1024){//长度错误
			gprsIndex = 0;
			memset(gprsRevBuf,0,10);
			break;
		}
		if(gprsIndex >= (plen+18)){//数据接收完成
#ifdef _debug_
			debugstring("收:");
			debugdata(gprsRevBuf, gprsIndex,1);
#endif
			miniDispstr(6, 0, "RR_OKK", 0);

			gprsErrorTimes = 0;
			for(ii=0;ii<MAX_RCV_PACKET_NUM;ii++){
				if(gPacketRcvBuf[ii].ucItemStatus != 0xea)
					break;
			}
			if(ii>=MAX_RCV_PACKET_NUM){
				gprsIndex = 0;
				memset(gprsRevBuf, 0, 20);	//缓冲区满，此包丢弃
//					debugstring("满了\r\n");
				return;
			}
			memcpy(gPacketRcvBuf[ii].LinkFrame.FrameBuf, gprsRevBuf, gprsIndex);
			gPacketRcvBuf[ii].LinkFrame.ucFrameType = 0x07;
			memcpy(gPacketRcvBuf[ii].LinkFrame.uiFrameLen, (unsigned char*)&plen, 2);
			gPacketRcvBuf[ii].ucItemStatus = 0xea;
#ifdef _debug_
			debugstring("接收:");
			debugdata((unsigned char*)&gPacketRcvBuf[ii].LinkFrame, plen, 1);
#endif

			gprsIndex = 0;
			memset(gprsRevBuf,0,10);
		}
		break;
	}
}
#else
void OTHER_GPRS_hand(unsigned char value)
{	
	unsigned short plen;
	unsigned char ii;

	ii = value;
//	debugdata((unsigned char*)&ii, 1, 0);
	switch (gprsIndex)
	{
	case 0:
	case 1:
	case 2:
	case 3:
		if(ii != '$'){
			gprsIndex = 0;
		}
		else{
			gprsRevBuf[gprsIndex++] = ii;
		}
		break;
	case 4://长度高位
	case 5://长底低位
		gprsRevBuf[gprsIndex++] = ii;
		memcpy((unsigned char*)&plen, gprsRevBuf+4, 2);
		over_turn(2, (unsigned char*)&plen);
		if(plen > 2048){//长度错误
			gprsIndex = 0;
			memset(gprsRevBuf,0,10);
			break;
		}
		break;
	default:
		gprsRevBuf[gprsIndex++] = ii;
		memcpy((unsigned char*)&plen, gprsRevBuf+4, 2);
		over_turn(2, (unsigned char*)&plen);
		if(plen > 2048){//长度错误
			gprsIndex = 0;
			memset(gprsRevBuf,0,10);
			break;
		}
		if(gprsIndex >= plen){//数据接收完成
#ifdef _debug_
			debugstring("收:");
			debugdata(gprsRevBuf, gprsIndex,1);
#endif
			if(gprsRevBuf[6] == 0x21){//是后台数据
				gprsErrorTimes = 0;
				for(ii=0;ii<MAX_RCV_PACKET_NUM;ii++){
					if(gPacketRcvBuf[ii].ucItemStatus != 0xea)
						break;
				}
				if(ii>=MAX_RCV_PACKET_NUM){
					gprsIndex = 0;
					memset(gprsRevBuf, 0, 20);	//缓冲区满，此包丢弃
//					debugstring("满了\r\n");
					return;
				}
				memcpy(gPacketRcvBuf[ii].LinkFrame.FrameBuf, gprsRevBuf+8, plen-10);
				gPacketRcvBuf[ii].LinkFrame.ucFrameType = 0x07;
				memcpy(gPacketRcvBuf[ii].LinkFrame.uiFrameLen, (unsigned char*)&plen, 2);
				gPacketRcvBuf[ii].ucItemStatus = 0xea;
#ifdef _debug_
				debugstring("接收:");
				debugdata((unsigned char*)&gPacketRcvBuf[ii].LinkFrame, plen, 1);
#endif
			}
			if((gprsRevBuf[6]==0x21)||(gprsRevBuf[6]==0x22)||(gprsRevBuf[6]==0x23)||(gprsRevBuf[6]==0x24))
				GPSDD_Deal(gprsRevBuf[6], gprsRevBuf[7]);
			else 
				gpacSNo++;//如果发包有回应，就加，下面将是新包
			gprsIndex = 0;
			memset(gprsRevBuf,0,10);
		}
		break;
	}
}
#endif
#endif
//----------------外接调机传数据↑↑↑↑↑↑↑↑↑↑
//--------------------------------------------------------
#ifdef _YiDa_Card_
extern void YD_GPSMsginfo(void);
#endif
#ifdef BUS_YICHUN
extern void checkDataSend(void);
#endif

//每一秒钟进一次
void in_dispGprsProcess(void)
{
#ifndef NO_GJ_SERVISE
	char buff[32];
	
	gGprsResertTime++;
// 	sprintf(buff, "gGprsResertTime:%d", gGprsResertTime);
// 	debugstring(buff);
	if((gGprsResertTime >= 600)&&((GPRSLinkProcess&0xF0) != 0xF0)){
		gGprsResertTime = 0;
		gmissflag = 0;
		GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
	}

	sprintf(buff, "%02X", GPRSLinkProcess);
	miniDispstr(7, 19, buff, 1);
#ifdef NO_MOBILE_SERVISE	//如果没有移动平台，则默认移动平台连接错误,不然会导致检测到GPRS错误时不重启模块.
	gErrorFlag |= ERROR_CONNECT_MOBILE;
#endif

#ifndef NO_MOBILE_24G_
	if((g24PurError>0)&&(g24PurError<0xa0))//等手机模块初始化完成
		miniDispstr(6, 19, "--", 1);
	else{
		sprintf(buff, "%02X", gmissflag);
		miniDispstr(6, 19, buff, 1);
	}
#elif defined _CPU_24G_
	if((g24PurError>0)&&(g24PurError<0xa0))//等手机模块初始化完成
		miniDispstr(6, 19, "--", 1);
	else{
		sprintf(buff, "%02X", gmissflag);
		miniDispstr(6, 19, buff, 1);
	}
#else
	sprintf(buff, "%02X", gmissflag);
	miniDispstr(6, 19, buff, 1);
#endif
#endif//#ifndef NO_GJ_SERVISE
#ifdef BUS_YICHUN
	YD_GPSMsginfo();
#endif
	if(BUflag > 0){
		BUflag--;
		if(BUflag == 0)
			restore_flag = 3;
	}
	return;
}

unsigned char in_getGprsModeStyle(void)
{
	unsigned char inmmdd;

#ifdef GPRS_MG323_
	inmmdd = Gmode_MG323;
#elif defined CDMA_MC323_
	inmmdd = Gmode_MC323;
#elif defined OTHER_GPRS
	inmmdd = Gmode_OTHER;
#elif defined WIFI_TLH_
	inmmdd = Gmode_WIFI_TLH;
#else
	inmmdd = Gmode_900C;
#endif

#ifdef Modem_BUS_
	inmmdd |= Gmode_MODEM;
#endif

	return inmmdd;
}
//每一秒钟进一次
int in_sendGPs(void)
{
#ifdef _GPS_NEED_REC_
	unsigned int t;
	unsigned char sndbuf[128];

	if(gGPSdly > 50)
		gGPSdly = 8;

	if(gGPSdly>0)
		gGPSdly--;
	if(gGPSdly == 0){//10秒钟发一次GPS信息
		gGPSdly = 8;
		if((GPRSLinkProcess == TCPSTARTSTAT)||(GPRSLinkProcess == 0xA0)){//GPRS已经登陆，可以发数据
			t = Build_GJ_GPS((unsigned char*)sndbuf);//发送GPS信息数据
			if(t != 0){
				gprs_send_data(1, t, (unsigned char*)sndbuf);
				return 1;
			}
		}
	}
#endif
#ifdef BUS_YICHUN
	checkDataSend();
#endif
	return 0;
}
//打开GPRS电源 
void GPRS_open(void)
{
#ifdef BUS_JIUJIANG_
#ifdef CDMA_CASE_
	gprs_pwon_lo();
#else
	gprs_pwon_hi();
#endif
#else
	gprs_pwon_lo();
#endif
	gprsIndex = 0;
	gprsErrorTimes = 0;
}

//打开GPRS电源,MC55I的开机程序
void GPRS_MC55i_open(void)
{
#ifdef BUS_JIUJIANG_
#ifdef CDMA_CASE_
	gprs_pwon_hi();//九江以后的机器
#else
	gprs_pwon_lo();
#endif
#else
	gprs_pwon_hi();
#endif
	gprsIndex = 0;
	gprsErrorTimes = 0;
	
#ifdef _debug_gprs
	debugstring("GPRS_MC55i_open::\r");
#endif
}

void in_decgM_Senddll(void)
{
#ifndef NO_MOBILE_SERVISE
	if(gM_Senddll > 0)
		gM_Senddll--;//计数据到减就发记录
	if(gM_Senddll > 50)
		gM_Senddll = 0;//等待时间不超过50
#else
	;
#endif
#ifndef NO_MOBILE_SERVISE
	if((g24PurError>0)&&(g24PurError<0xa0))
		g24PurError--;
#elif defined _CPU_24G_
	if((g24PurError>0)&&(g24PurError<0xa0))
		g24PurError--;
#endif
	
#ifdef SelMsg_enable
	if(gsendMsg.TLL > 0)
		gsendMsg.TLL--;
#endif
}
unsigned char getGprsCom(void)
{
	return COM_GPRS;
}
unsigned int getDefaultIP(void)
{
	return GJ_IP;
}
unsigned short getDefaultPort(void)
{
	return GJ_PORT;
}
void getDefaultAPN(char *rAPN, unsigned char mode)//mode=0 公交，1 移动。。
{
	if(mode == 0){
		strcpy(rAPN, NETAPN);
	}
	else{
#ifdef _MOBILE_CMNET_
		strcpy(rAPN, "CMNET");
#elif defined _MOBILE_IP_CMPAY
		strcpy(rAPN, "CMPAY");
#else
		strcpy(rAPN, "CMPAY.HB");
#endif
	}
}

int in_buildSevece(char *gpsrsP)
{
#ifndef NO_GJ_SERVISE
	unsigned int t;
#endif
	int i=0;
// #ifdef _debug_gprs
// 		debugstring("gmissflag:");
// 		debugdata((unsigned char*)&gmissflag,1, 0);
// #endif
	if((gmissflag & 0xf0)==0x10){//登陆移动IF1
#ifdef NO_MOBILE_SERVISE
		gmissflag = 0;
		set_timer0(1000, 1);
		return -2;
#else
// 			if(gmissOldflag != MISS_M_TradeBatch){
// 				if((gmissflag == MISS_M_SINGIN)&&(F_FLAG_VARY.stop_flag)){//如果是到站状态，不允许签到。
// 					gmissflag = MISS_G_FREE;
// 					set_timer0(500, 1);
// 					break;
// 				}
// 			}
		if(isNetOK[0] != 0){
			tcpipClose(5);
			//out_GPRS();
			return -1;
		}
#ifndef _MOBILE_CMNET_
		if(gAPN_Style != 'Y')
		{
			GPRSLinkProcess = 16;
			out_GPRS();
			return -1;
		}
#endif
		memset((char *)gpsrsP, 0, 100);
//			strcpy((char*)gpsrsP, "AT%IPOPENX=1,\"TCP\",\"211.138.236.210\",26207\r\n");//打开一条tcp链接
//			strcpy((char*)gpsrsP, "AT%IPOPEN=\"TCP\",\"172.031.011.193\",36007\r\n");//打开一条IF2 tcp链接
#ifdef CDMA_MC323_
		memcpy((char*)gpsrsP, "AT^IPOPEN=1,\"TCP\",\"", 19);//211.138.236.210\",26207\r\n");//打开一条tcp链接
		i = 19;
#elif defined GPRS_MG323_
		memcpy((char*)gpsrsP, "at^siss=0,address,\"socktcp://", 29);//211.138.236.210\",26207\r\n");//打开一条tcp链接
		i = 29;
#else
		memcpy((char*)gpsrsP, "AT%IPOPEN=\"TCP\",\"", 17);//211.138.236.210\",26207\r\n");//打开一条tcp链接
		i = 17;
#endif
		if(checkIP((char*)gMobileParameter.purIP) == 0){ //if(memcmp(gMobileParameter.purIP, "\x00\x00\x00\x00\x00", 5) == 0)
#ifdef _MOBILE_IP_CHANGSHA_
			memcpy((char*)gpsrsP+i, "211.138.236.210", 15);//长沙
#endif
#ifdef _MOBILE_IP_HUBEI_
			memcpy((char*)gpsrsP+i, "172.031.011.193", 15);
#endif
#ifdef _MOBILE_IP_FUJIAN_
			memcpy((char*)gpsrsP+i, "218.207.183.082", 15);//长沙
#endif
#ifdef _MOBILE_IP_XiAN_
#ifdef MOBILE_XIANYANG_
			memcpy((char*)gpsrsP+i, "010.129.005.100", 15);//咸阳
#else 
			memcpy((char*)gpsrsP+i, "211.137.133.038", 15);//铜川
#endif
#endif
#ifdef _MOBILE_IP_QINHAI
			memcpy((char*)gpsrsP+i, "218.206.165.003", 15);
#endif
			;
		}
		else
			memcpy(gpsrsP+i, gMobileParameter.purIP, 15);
		i+=15;
#ifdef GPRS_MG323_
		gpsrsP[i] = ':';
		i++;
#else
		memcpy(gpsrsP+i, "\",", 2);
		i += 2;
#endif
// 			debugstring("..........");
// 			debugstring((char*)gpsrsP);
// 			debugstring((char*)gMobileParameter.purIP);
//			if(memcmp(gMobileParameter.purPort, "\x00\x00\x00\x00", 4) == 0){
#ifdef _MOBILE_IP_HUBEI_
			memcpy((char*)gpsrsP+i, "36007", 5);
			i += 5;
#endif
#ifdef _MOBILE_IP_CHANGSHA_
			memcpy((char*)gpsrsP+i, "26207", 5);
			i += 5;
#endif
#ifdef _MOBILE_IP_FUJIAN_
			memcpy((char*)gpsrsP+i, "9092", 4);//福建
			i += 4;
#endif
#ifdef _MOBILE_IP_XiAN_
			memcpy((char*)gpsrsP+i, "8000", 4);//长沙
			i += 4;
#endif
#ifdef _MOBILE_IP_QINHAI
			memcpy((char*)gpsrsP+i, "8000", 4);
			i += 4;
#endif
//			}
//			else{
#ifdef _MOBILE_IP_NOLOCK_
			t = strlen((char*)gMobileParameter.purPort);
			if(t>= 6)
				t = 5;
			memcpy(gpsrsP+i, gMobileParameter.purPort, t);
			i += t;
#endif
//				strcat((char*)gpsrsP, (char*)gMobileParameter.purPort);
//			}
#ifdef GPRS_MG323_
			memcpy((char*)gpsrsP+i, "\"\r\n", 3);
			i+=3;
#elif defined CDMA_MC323_
			memcpy((char*)gpsrsP+i, ",1000\r\n", 7);
			i+=7;
#else
			memcpy((char*)gpsrsP+i, "\r\n", 2);
			i+=2;
#endif
#endif
	}
	else if((gmissflag & 0xf0)==0x20){//登陆移动IF2
#ifdef NO_MOBILE_SERVISE
		gmissflag = 0;
		set_timer0(1000, 1);
		return -1;
#else

		if(isNetOK[0] != 0){
			tcpipClose(5);
			set_timer0(1000, 1);
			return -1;
		}
#ifndef _MOBILE_CMNET_
		if(gAPN_Style != 'Y')
		{
			GPRSLinkProcess = 16;
			out_GPRS();
			return -1;
		}
#endif
		memset(gpsrsP, 0, 100);
//			strcpy((char*)gpsrsP, "AT%IPOPENX=1,\"TCP\",\"211.138.236.210\",26202\r\n");//打开一条IF2 tcp链接
//			strcpy((char*)gpsrsP, "AT%IPOPEN=\"TCP\",\"211.138.236.210\",26202\r\n");//打开一条IF2 tcp链接
//			strcpy((char*)gpsrsP, "AT%IPOPEN=\"TCP\",\"172.31.11.193\",36002\r\n");//打开一条IF2 tcp链接
#ifdef CDMA_MC323_
		memcpy((char*)gpsrsP, "AT^IPOPEN=1,\"TCP\",\"", 19);//211.138.236.210\",26207\r\n");//打开一条tcp链接
		i = 19;
#elif defined GPRS_MG323_
		memcpy((char*)gpsrsP, "at^siss=0,address,\"socktcp://", 29);//211.138.236.210\",26207\r\n");//打开一条tcp链接
		i = 29;
#else
		memcpy((char*)gpsrsP, "AT%IPOPEN=\"TCP\",\"", 17);//211.138.236.210\",26207\r\n");//打开一条tcp链接
		i = 17;
#endif
		if(checkIP((char*)gMobileParameter.purIP) == 0){ //if(memcmp(gMobileParameter.purIP, "\x00\x00\x00\x00\x00", 5) == 0)
#ifdef _MOBILE_IP_CHANGSHA_
			memcpy((char*)gpsrsP+i, "211.138.236.210", 15);//长沙
#endif
#ifdef _MOBILE_IP_HUBEI_
			memcpy((char*)gpsrsP+i, "172.031.011.193", 15);
#endif
#ifdef _MOBILE_IP_FUJIAN_
			memcpy((char*)gpsrsP+i, "218.207.183.082", 15);//长沙
#endif
#ifdef _MOBILE_IP_XiAN_
#ifdef MOBILE_XIANYANG_
			memcpy((char*)gpsrsP+i, "010.129.005.100", 15);//咸阳
#else 
			memcpy((char*)gpsrsP+i, "211.137.133.038", 15);//铜川
#endif
#endif
#ifdef _MOBILE_IP_QINHAI
			memcpy((char*)gpsrsP+i, "218.206.165.003", 15);
#endif
			;
		}
		else
			memcpy(gpsrsP+i, gMobileParameter.purIP, 15);
		i+=15;
#ifdef GPRS_MG323_
		gpsrsP[i] = ':';
		i++;
#else
		memcpy(gpsrsP+i, "\",", 2);
		i += 2;
#endif
//			if(memcmp(gMobileParameter.managePort, "\x00\x00\x00\x00", 4) == 0){
#ifdef _MOBILE_IP_HUBEI_
		memcpy((char*)gpsrsP+i, "36002", 5);
		i+=5;
#endif
#ifdef _MOBILE_IP_CHANGSHA_
		memcpy((char*)gpsrsP+i, "26202", 5);
		i+=5;
#endif
#ifdef _MOBILE_IP_FUJIAN_
		memcpy((char*)gpsrsP+i, "9091", 4);//福建
		i+=4;
#endif
#ifdef _MOBILE_IP_XiAN_
		memcpy((char*)gpsrsP+i, "8001", 4);//长沙
		i+=4;
#endif
#ifdef _MOBILE_IP_QINHAI
		memcpy((char*)gpsrsP+i, "8002", 4);
		i+=4;
#endif
// 			}
// 			else{
#ifdef _MOBILE_IP_NOLOCK_
			t = strlen((char*)gMobileParameter.managePort);
			if(t >= 6)
				t = 5;
			memcpy(gpsrsP+i, gMobileParameter.managePort, t);
			i += t;
#endif
//				strcat((char*)gpsrsP, (char*)gMobileParameter.managePort);
// 			}
#ifdef GPRS_MG323_
			memcpy((char*)gpsrsP+i, "\"\r\n", 3);
			i+=3;
#elif defined CDMA_MC323_
			memcpy((char*)gpsrsP+i, ",1000\r\n", 7);
			i+=7;
#else
			memcpy((char*)gpsrsP+i, "\r\n", 2);
			i += 2;
#endif
#endif
	}
	else{// if((gmissflag & 0xf0)==MISS_GJ){//登陆公交数据中心
#ifdef NO_GJ_SERVISE
		gmissflag = 0;
		set_timer0(1000, 1);
		i = -1;
#else//#ifdef NO_GJ_SERVISE
		if(isNetOK[0] != 0){
#ifdef _debug_gprs
			debugstring("in bsc 1:\r\n");
#endif
			set_timer0(1000, 1);
			tcpipClose(5);
 			return -1;
		}

#ifndef _MOBILE_CMNET_
		if(gAPN_Style != 'N')
		{
#ifdef _debug_gprs
			debugstring("in bsc 2:\r\n");
#endif
			GPRSLinkProcess = 16;
			out_GPRS();
			return -1;
		}
#endif
#ifdef _debug_gprs
		debugstring("in bsc 3:\r\n");
#endif
		memset((char *)gpsrsP, 0, 100);
#ifdef _debug_gprs
		debugstring("in bsc 4:\r\n");
#endif
//			strcpy((char*)gpsrsP, "AT%IPOPENX=1,\"TCP\",\"211.138.236.210\",26202\r\n");//打开一条IF2 tcp链接
//			strcpy((char*)gpsrsP, "AT%IPOPEN=\"TCP\",\"211.138.236.210\",26202\r\n");//打开一条IF2 tcp链接
//			strcpy((char*)gpsrsP, "AT%IPOPEN=\"TCP\",\"172.31.11.193\",36002\r\n");//打开一条IF2 tcp链接
#ifdef CDMA_MC323_
		memcpy((char*)gpsrsP, "AT^IPOPEN=1,\"TCP\",\"", 19);//211.138.236.210\",26207\r\n");//打开一条tcp链接
		i = 19;
#elif defined GPRS_MG323_
		memcpy((char*)gpsrsP, "at^siss=0,address,\"socktcp://", 29);//211.138.236.210\",26207\r\n");//打开一条tcp链接
		i = 29;
#else
		memcpy((char*)gpsrsP, "AT%IPOPEN=\"TCP\",\"", 17);//211.138.236.210\",26207\r\n");//打开一条tcp链接
		i += 17;
#endif

#ifdef BUS_XINYI
		ClientIP = getDefaultIP();
		ClientPort = getDefaultPort();
#elif defined IP_OF_SLZR_
		ClientIP = GJ_IP;
		ClientPort = GJ_PORT;
#endif
//			ClientIP = getDefaultIP();//写死IP地址。。
		memcpy((unsigned char*)gpsrsP+50, (unsigned char*)&ClientIP, 4);
		sprintf((char*)gpsrsP+60, "%03u.%03u.%03u.%03u", gpsrsP[50],gpsrsP[51],gpsrsP[52],gpsrsP[53]);
		if((memcmp(gpsrsP+50, "\x00\x00\x00\x00", 4) == 0)||(memcmp(gpsrsP+50, "\xFF\xFF\xFF\xFF", 4) == 0)){
			ClientIP = getDefaultIP();
			memcpy((unsigned char*)gpsrsP+50, (unsigned char*)&ClientIP, 4);
			sprintf((char*)gpsrsP+60, "%03u.%03u.%03u.%03u", gpsrsP[50],gpsrsP[51],gpsrsP[52],gpsrsP[53]);
		}
		memcpy(gpsrsP+i, gpsrsP+60, 15);
		i += 15;
#ifdef GPRS_MG323_
		gpsrsP[i] = ':';
		i++;
#else
		memcpy(gpsrsP+i, "\",", 2);
		i += 2;

#endif
		if((ClientPort == 0)||(ClientPort >= 9000)){
			ClientPort = getDefaultPort();
		}
		sprintf((char*)gpsrsP+50, "%u", ClientPort);
		t = strlen((char*)gpsrsP+50);
		if(t >= 6)
			t = 5;
		memcpy(gpsrsP+i, gpsrsP+50, t);
		i += t;
#ifdef GPRS_MG323_
		memcpy((char*)gpsrsP+i, "\"\r\n", 3);
		i+=3;
#elif defined CDMA_MC323_
		memcpy((char*)gpsrsP+i, ",1000\r\n", 7);
		i+=7;
#else
		memcpy((char*)gpsrsP+i, "\r\n", 2);
		i += 2;
#endif
		
#endif //#ifdef NO_GJ_SERVISE
	}
#ifdef _debug_gprs
	debugstring("in buildSevece:");
	debugstring(gpsrsP);
#endif
	return i;
}
void TaskRecWrite(void)
{
#ifndef NO_GJ_SERVISE
//	unsigned short i=0;
//	unsigned short sum;
	unsigned char buffer[1025];//[2100];
	unsigned int uilen;
//	int temp1;

//	sprintf((char*)buffer, "TaskRecWrite**%d:* ", gmobileErrTimes);
//	debugstring((char*)buffer);
#ifndef NO_MOBILE_SERVISE
	if(gmobileErrTimes >= 5){
		stopCCgprs(ERROR_CONNECT_MOBILE);
		gmobileErrTimes = 0;
	}
#endif
#ifdef SelMsg_enable
	if((gmissflag == MISS_G_LOGINGJ)&&(GPRSLinkProcess < 21)){
		find_G_new_mission();
		return;
	}
#endif
	if((GPRSLinkProcess > 14)&&((gmissflag == 0)||(gmissflag == MISS_G_FREE))){
#ifndef NO_MOBILE_SERVISE
		if((gmissflag == 0)||(gmissflag == MISS_G_FREE)){//没有任务，找找任务
			find_new_mission();
		}
#endif	
		
		if((gmissflag == 0)||(gmissflag == MISS_G_FREE)||(gmissflag == MISS_G_LOGINGJ)){
			find_G_new_mission();
		}
		return;
	}

	if((GPRSLinkProcess != TCPSTARTSTAT)&&//还没连接TCPIP
		(GPRSLinkProcess != GPRS_SENDING_CMD)){
		return;
	}

	if(isNetOK == 0){//GPRSLinkProcess = 21,但是已经断开了，需要重新连接
		GPRSLinkProcess = 20;
		return;
	}

	if(GPRSLinkProcess == GPRS_SENDING_CMD){//正在等待上个命令应答
#ifdef _debug_
		debugstring("gSendOverTime:");
		debugdata((unsigned char*)&gSendOverTime, 1, 0);
#endif
		if(gSendOverTime > 0){
			gSendOverTime--;
#ifdef GPRS_MG323_
			if((gSendOverTime == 10)||(gSendOverTime == 5))//快超时了，看看模块有没有数据，有时有数据也没有上报的。
				MC55i_SISREAD();
#endif 
		}
		if(gSendOverTime == 0){//接收应签超时
#ifdef _GPS_DIAODU_
			if(gmissflag == MISS_G_ALAM){
#ifdef _debug_
				debugstring("galamessage.style:");
				debugdata((unsigned char*)&galamessage.style, 1, 0);
#endif
				if(galamessage.style != 0x5A){
					gmissflag = 0;
#ifdef KEY_PAD
					gKeyRestorDis = 3;
#endif
#ifdef _debug_
					debugstring("报警发送失败!\r\n");
#endif
				}
			}
#endif
			GPRSLinkProcess = TCPSTARTSTAT;
#ifndef NO_MOBILE_SERVISE
			if(gmissflag == MISS_M_TradeBatch){
				readmobilsendStyle();
				load_fe_SendSeril();
				GPRSLinkProcess = GPRS_NEED_CLOSEIP;//tcpipClose(3);
			}
#endif
			
#ifdef _debug_
			debugstring("gGPRS_Cmd_outtime:");
			debugdata((unsigned char*)&gGPRS_Cmd_outtime, 1, 0);
#endif
			gGPRS_Cmd_outtime++;
			if(gGPRS_Cmd_outtime == 2){
				gmissflag = 0;
				gmissOldflag = 0;
				GPRSLinkProcess = GPRS_NEED_CLOSEIP;//tcpipClose(3);
			}
			if(gGPRS_Cmd_outtime > 4){//如果接收大于三次的处理
				if(gmissflag == MISS_M_ActiveTest)//如果是回响，不再执行
				{
					gmissflag = 0;
					gmissOldflag = 0;
				}
				if(gmissflag & MISS_GJ){//如果是公交任务,则停止
					stopCCgprs(ERROR_BUS_CONNECT);
					gmissflag = 0;
					gmissOldflag = 0;
				}
				else {
					stopCCgprs(ERROR_CONNECT_MOBILE);
				}
				gGPRS_Cmd_outtime = 0;
//				GPRSLinkProcess = GPRS_NEED_CLOSEIP;//tcpipClose(3);
#ifdef WIFI_TLH_
				GPRSLinkProcess = GPRS_AT_WAIT;//GPRS_MODE_NEED_CLOSE;
#else
				GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
#endif
			}
		}
		return;
	}
#ifdef _debug_f
	debugstring("gmissflag:");
	debugdata((unsigned char*)&gmissflag, 1, 0);
#endif
	
#ifndef NO_MOBILE_SERVISE
	if((gmissflag & MISS_IF1) == MISS_IF1){
		if(isNetOK[0] != 1){//没有连接IF1，重新连接
#ifdef _debug_
			debugstring("没有连接IF1，重新连接\r\n");
#endif
			if(isNetOK[0] != 0){
				if(gAPN_Style != 'Y')
					tcpipClose(5);
				else 
					GPRSLinkProcess = GPRS_NEED_CLOSEIP;//tcpipClose(0);
			}
			else
				GPRSLinkProcess = 20;
			return;
		}
#ifndef _MOBILE_CMNET_
		if(gAPN_Style != 'Y'){
			tcpipClose(5);
			return;
		}
#endif

#ifdef _debug_
		debugstring("IF1:");
		debugdata((unsigned char*)&gmissflag, 1, 1);
#endif
		switch(gmissflag){
		case MISS_M_SINGIN:
			if(gmissOldflag != MISS_M_TradeBatch){
				if(F_FLAG_VARY.stop_flag){//如果是到站状态，不允许签到。
					gmissflag = 0;
					return;
				}
			}
			if(posnoIsOK(gMobileParameter.posNo) == 1)
			{
				gmissflag = MISS_M_DPARAM;
				return;
			}
			uilen = Build8583Packge(buffer, MISS_M_SINGIN);
//			strcpy((char*)buffer+1000, "000000000008000038000001C00010000089142620200909045A5A5A5A5A5A5A5A5A5A31303030323537333130303034363838383037333134313131313233390072D15600010180038000000001000000003230303930363033313333303030303032303039303833313130333434384646303130303030303230312020202020202020202020202020");
//			strcpy((char*)buffer+1000, "600000000008000038000001C00010000545162852200909045157515751575157515730303130323137333130303034343838383037333134353131343531310072D15600010180038000000001000000003230303930363239313131313131303032303039303930343131333835314646303130303030353530312020202020200000000000000000");
//			Ascii2BCD(buffer+1000, buffer, strlen((char*)buffer+1000));
			gprs_send_data(1, uilen, buffer);
			GPRSLinkProcess = GPRS_SENDING_CMD;
			gSendOverTime = DE_SendOverTime;
			break;
		case MISS_M_Settlement:
			uilen = Build8583Packge(buffer, MISS_M_Settlement);
			gprs_send_data(1, uilen, buffer);
			GPRSLinkProcess = GPRS_SENDING_CMD;
			gSendOverTime = DE_SendOverTime;
			break;
		case MISS_M_TradeBatch:
			if(gMobilStyle.ISOK != 1){
				gmissflag = MISS_M_SINGIN;
				gmissOldflag = MISS_M_TradeBatch;
				gseflag = 0;
				return;
			}
			uilen = Build8583Packge(buffer, MISS_M_TradeBatch);
			if(uilen == 0){
				gmissflag = MISS_M_Settlement;
				return;//批上送完成,再发送汇总
			}
			gprs_send_data(1, uilen, buffer);
			GPRSLinkProcess = GPRS_SENDING_CMD;
			gSendOverTime = DE_SendOverTime;
			break;
		default:
			find_new_mission();//搜寻其它任务
			break;
		}
		return;
	}
	
	if((gmissflag & MISS_IF2) == MISS_IF2){
		if(isNetOK[0] != 2){//没有连接IF2，重新连接
#ifdef _debug_
			debugstring("没有连接IF2，重新连接\r\n");
#endif
			if(isNetOK[0] != 0){
				if(gAPN_Style != 'Y')
					tcpipClose(5);
				else 
					GPRSLinkProcess = GPRS_NEED_CLOSEIP;//tcpipClose(0);
			}
			else
				GPRSLinkProcess = 20;
			return;
		}
		
#ifndef _MOBILE_CMNET_
		if(gAPN_Style != 'Y'){
			tcpipClose(5);
			return;
		}
#endif
		switch(gmissflag){
		case MISS_M_DPARAM:
			uilen = Build8583Packge(buffer, MISS_M_DPARAM);
			gprs_send_data(1, uilen, buffer);
			GPRSLinkProcess = GPRS_SENDING_CMD;
			gSendOverTime = DE_SendOverTime;
			break;
		case MISS_M_DPGRAM:
			gmissflag = 0;	//不从手机POSP平台下载程序
			gmissOldflag = 0;	//不从手机POSP平台下载程序
			break;
// 			uilen = Build8583Packge(buffer, MISS_M_DPGRAM);
// 			gprs_send_data(1, uilen, buffer);
// 			GPRSLinkProcess = GPRS_SENDING_CMD;
// 			gSendOverTime = DE_SendOverTime;
// 			break;
		case MISS_M_DPGRAMOK:
			gmissflag = 0;	//不从手机POSP平台下载程序
			gmissOldflag = 0;	//不从手机POSP平台下载程序
			break;
// 			uilen = Build8583Packge(buffer, MISS_M_DPGRAMOK);
// 			gprs_send_data(1, uilen, buffer);
// 			GPRSLinkProcess = GPRS_SENDING_CMD;
// 			gSendOverTime = DE_SendOverTime;
// 			break;
		case MISS_M_KEK:
			uilen = Build8583Packge(buffer, MISS_M_KEK);
			gprs_send_data(1, uilen, buffer);
			GPRSLinkProcess = GPRS_SENDING_CMD;
			gSendOverTime = DE_SendOverTime;
			break;
		case MISS_M_ActiveTest:
			uilen = Build8583Packge(buffer, MISS_M_ActiveTest);
			gprs_send_data(1, uilen, buffer);
			GPRSLinkProcess = GPRS_SENDING_CMD;
			gSendOverTime = DE_SendOverTime;
			break;
		default:
			find_new_mission();//搜寻其它任务
			break;
		}
		return;
	}
#else //#ifndef NO_MOBILE_SERVISE
	if(((gmissflag & MISS_IF2) == MISS_IF2)||((gmissflag & MISS_IF1) == MISS_IF1)){
		gmissflag = 0;
		return;
	}
#endif//#ifndef NO_MOBILE_SERVISE
	else if((gmissflag & MISS_GJ) == MISS_GJ){
		if(isNetOK[0] != 3){//没有连接IF2，重新连接
#ifdef _debug_
			debugstring("没有连接公交，重新连接\r\n");
#endif
			if(isNetOK[0] != 0)
				tcpipClose(5);
			else
				GPRSLinkProcess = 20;
			return;
		}
#ifndef _MOBILE_CMNET_
#ifndef WIFI_TLH_
		if(gAPN_Style != 'N'){
			tcpipClose(5);
			return;
		}
#endif
#endif
		switch(gmissflag){
		case MISS_G_TOO:
		case MISS_G_LOGINGJ:
		case MISS_G_DBLKD:
		case MISS_G_DBLKI:
		case MISS_G_UREC:
		case MISS_G_DPRO:
		case MISS_G_DLine:
		case MISS_G_DSound:
		case MISS_G_DPROOK:
		case MISS_G_DLineOK:
		case MISS_G_DSoundOK:
		case MISS_G_HART:
		case MISS_G_GPS:
		case MISS_G_ALAM:
			uilen = BuildGJPackge(buffer, gmissflag);
			if(uilen == 0){
				gmissflag = MISS_G_FREE;
				break;
			}
// 			if(gGPSdly < 3){
// 				gGPSdly = 3;//要发数据，GPS信息延迟发送
// 			}
#ifdef _debug_gprs
			debugstring("TaskRecWrite_snd data:");
			debugdata(buffer, uilen, 1);
#endif
			gprs_send_data(1, uilen, buffer);
			if((gmissflag == MISS_G_HART)||(gmissflag == MISS_G_GPS)){
				gmissflag = MISS_G_FREE;
				return;//不需要等应答
			}
			GPRSLinkProcess = GPRS_SENDING_CMD;
			gSendOverTime = DE_SendOverTime;
		case MISS_G_FREE:
			break;
		default:
			gmissflag = 0;
			break;
		}
	}
#ifndef NO_MOBILE_SERVISE
	if((gmissflag == 0)||(gmissflag == MISS_G_FREE)){//没有任务，找找任务
		find_new_mission();
	}
#endif

	if((gmissflag == 0)||(gmissflag == MISS_G_FREE)||\
		(gmissflag == MISS_G_LOGINGJ)){
		find_G_new_mission();
	}
//	debugstring("TASK END ");
#endif
	return;
}
void SetTcpPacketTTLTime(unsigned char val)
{
#ifndef NO_GJ_SERVISE
#ifdef WIFI_TLH_
	tcpipPacket.TTL = 0;
#else
	tcpipPacket.TTL = val;
#endif
	if(tcpipPacket.packetStatus!=RECORD_DEAL)
		tcpipPacket.packetStatus = 0;
//	tcpipClose(0);
#endif	//#ifndef NO_GJ_SERVISE
}


#ifndef NO_GJ_SERVISE
void aoutMcheckIP(void)
{							  
	unsigned char Ipbuf[50];
	memcpy(Ipbuf, (unsigned char*)&ClientIP, 4);
	sprintf((char*)Ipbuf+5, "%03u.%03u.%03u.%03u", Ipbuf[0],Ipbuf[1],Ipbuf[2],Ipbuf[3]);
	if((checkIP((char*)Ipbuf+5) == 0)&&(GJ_IP != 0x64646464)){	//IP不正确，把IP置成默认值
		ClientIP = GJ_IP;
		ClientPort = GJ_PORT;
		sysfewrite(BIT_GPRS_IP,4,(unsigned char*)&ClientIP);
		sysfewrite(BIT_GPRS_PORT,2,(unsigned char*)&ClientPort);
		savesysbupara();
	}

}

unsigned char SetupClientIP(void)
{
	unsigned char i;
	unsigned int tm;
	unsigned char x,col;
	unsigned char buffer[2][20];
	unsigned char Ipbuf[50];
	unsigned int uitemp;
//	unsigned long ultempIP;
	
	memcpy(Ipbuf, (unsigned char*)&ClientIP, 4);
	sprintf((char*)Ipbuf+5, "%03u.%03u.%03u.%03u", Ipbuf[0],Ipbuf[1],Ipbuf[2],Ipbuf[3]);
	if(checkIP((char*)Ipbuf+5) == 0){
		ClientIP = GJ_IP;
	}

	memset(Ipbuf, 0, 4);
	tm=ClientIP;//ClientIP;
ip_start:
//	ultempIP=ClientIP;
	i=0;
	x=0;
	memset(buffer[0],0,20);
	memcpy(Ipbuf, (unsigned char*)&tm, 4);
	sprintf((char*)buffer[0],"%03u",Ipbuf[0]);
	i+=3;
	buffer[0][i]='.';
	i++;
	sprintf((char*)buffer[0]+i,"%03u",Ipbuf[1]);
	i+=3;
	buffer[0][i]='.';
	i++;
	sprintf((char*)buffer[0]+i,"%03u",Ipbuf[2]);
	i+=3;
	buffer[0][i]='.';
	i++;
	sprintf((char*)buffer[0]+i,"%03u",Ipbuf[3]);

	cls();
	if(gchn_eng == 'E')
		display(0,1,(char *)"sevice IP:",1);
	else
		display(0,1,(char *)"设置服务器IP:",1);
	display(2,1,(char*)buffer[0],0);
	buffer[0][18]=buffer[0][0];
	display(2,1,(char*)buffer[0]+18,1);
//	display(4,1,(char *)"设置服务器PORT:",1);
	memset(buffer[1], 0, 20);
	sprintf((char*)buffer[1], "%04d", ClientPort);
	buffer[1][4] = 0;
	display(4, 1, (char*)buffer[1], 0);
	col = 0;
	while(1)
	{
		delayxms(50);
		i=getkey(1);
		switch(i)
		{
#ifndef _New_Bu_mode_
		case KEY_F1:
			buffer[0][18]=buffer[col][x];
			display(2*(1+col),1+x,(char*)buffer[0]+18,0);
			if(col == 0){
				if(x>0) x--;
				else{
					col = 1;
					x = 3;
				}
			}
			else{
				if(x>0) x--;
				else{
					col = 0;
					x = 14;
				}
			}
			if(col == 0){
				if((x==3)||(x==7)||(x==11)) x--;
			}

			buffer[0][18]=buffer[col][x];
			display(2*(1+col),1+x,(char*)buffer[0]+18,1);
			break;
		case KEY_F2:
			buffer[0][18]=buffer[col][x];
			display(2*(1+col),1+x,(char*)buffer[0]+18,0);
			if(col == 0){
				if(x<14) x++;
				else{
					col = 1;
					x = 0;
				}
			}
			else{
				if(x<3) x++;
				else{
					col = 0;
					x = 0;
				}
			}
			if(col == 0){
				if((x==3)||(x==7)||(x==11)) x++;
			}
			buffer[0][18]=buffer[col][x];
			display(2*(1+col),1+x,(char*)buffer[0]+18,1);
			break;
		case KEY_DOWN:
			if((buffer[col][x]<0x30)||(buffer[col][x]>0x39)) buffer[col][x]=0x30;
			if(buffer[col][x]==0x30) buffer[col][x]=0x39;
			else if(buffer[col][x]>0x30) buffer[col][x]--;
			buffer[0][18]=buffer[col][x];
			display(2*(1+col),1+x,(char*)buffer[0]+18,1);
			break;
#endif
		case KEY_UP:
			if((buffer[col][x]<0x30)||(buffer[col][x]>0x39)) buffer[col][x]=0x39;
			if(buffer[col][x]==0x39) buffer[col][x]=0x30;
			else if(buffer[col][x]<0x39) buffer[col][x]++;
			buffer[0][18]=buffer[col][x];
			display(2*(1+col),1+x,(char*)buffer[0]+18,1);
			break;
		case KEY_ESC:		//-10
//			ClientIP=tm;
			return 1;
		case KEY_ENT:		//ok
#ifdef _New_Bu_mode_
			
			buffer[0][18]=buffer[col][x];
			display(2*(1+col),1+x,(char*)buffer[0]+18,0);
			if(col == 0){
				if(x<14) x++;
				else{
					col = 1;
					x = 0;
				}
			}
			else{
				if(x<4) 
					x++;
				// 				else{
				// 					col = 0;
				// 					x = 0;
				// 				}
			}
			if(col == 0){
				if((x==3)||(x==7)||(x==11)) x++;
			}
			buffer[0][18]=buffer[col][x];
			display(2*(1+col),1+x,(char*)buffer[0]+18,1);
#else
		col = 1; 
		x = 4;
#endif
			if((col == 1)&&(x >= 4))
			{
				uitemp=(buffer[0][0]-0x30)*100+(buffer[0][1]-0x30)*10+(buffer[0][2]-0x30);
				if(uitemp>255) goto ip_error;
				uitemp=(buffer[0][4]-0x30)*100+(buffer[0][5]-0x30)*10+(buffer[0][6]-0x30);
				if(uitemp>255) goto ip_error;
				uitemp=(buffer[0][8]-0x30)*100+(buffer[0][9]-0x30)*10+(buffer[0][10]-0x30);
				if(uitemp>255) goto ip_error;
				uitemp=(buffer[0][12]-0x30)*100+(buffer[0][13]-0x30)*10+(buffer[0][14]-0x30);
				if(uitemp>255) goto ip_error;
				
				Ipbuf[0]=(buffer[0][0]-0x30)*100+(buffer[0][1]-0x30)*10+(buffer[0][2]-0x30);
				Ipbuf[1]=(buffer[0][4]-0x30)*100+(buffer[0][5]-0x30)*10+(buffer[0][6]-0x30);
				Ipbuf[2]=(buffer[0][8]-0x30)*100+(buffer[0][9]-0x30)*10+(buffer[0][10]-0x30);
				Ipbuf[3]=(buffer[0][12]-0x30)*100+(buffer[0][13]-0x30)*10+(buffer[0][14]-0x30);
				memcpy((unsigned char*)&tm, Ipbuf, 4);		
				ClientIP=tm; 
				//debugdata((unsigned char *)&ClientIP, 4, 1);
				ClientPort = (buffer[1][0]-0x30)*1000+(buffer[1][1]-0x30)*100+(buffer[1][2]-0x30)*10+(buffer[1][3]-0x30);
				//debugdata((unsigned char*)&ClientPort, 2, 1);
				i=0;
				if(ClientIP<0x00FFFFFFl) i |= 0x10;
				if((ClientPort<100)||(ClientPort>9999)) i |= 0x20;
				if(i&0x30) {
					cls();
					if(gchn_eng == 'E'){
						display(0,0,(char *)"input error:",0);
						display(2,1,"data illegal",0);
					}
					else{
						display(0,0,"输入错误:",0);
						display(2,1,"IP地址不合法",0);
					}
					delayxs(1);
					goto ip_start;
				}

				sysfewrite(BIT_GPRS_IP,4,(unsigned char*)&ClientIP);
				sysfewrite(BIT_GPRS_PORT,2,(unsigned char*)&ClientPort);
				cls();
				if(gchn_eng == 'E'){
					display(2,1,"complete!",0);
				}
				else
					display(2,4,"设置完毕",0);
				savesysbupara();
				return 0; 
ip_error:
				cls();
				if(gchn_eng == 'E'){
					display(0,0,(char *)"input error:",0);
					display(2,1,"data illegal",0);
				}
				else{
					display(0,0,"输入错误:",0);
					display(2,1,"输入值要小于256",0);
				}
				goto ip_start;
			}
//			else
//			{
//				buffer[18]=buffer[x];
//				Display(2,1+x,buffer+18,0);
//				x++;
//				if((x==3)||(x==7)||(x==11)) x++;
//
//				buffer[18]=buffer[x];
//				Display(2,1+x,buffer+18,1);
//
//			}
			break;
		default:
			break;
		}
	}
}

#ifdef WIFI_TLH_
void setWiFISSID(void)
{
	unsigned char oldSSID[10];
	unsigned char SSIDbuf[64];
	char buffer[2];
	unsigned char CMFlag;//输入类型：0数据，1大写，2小写
	unsigned char Slen, index, disflag, ret;

	disflag = 1;
	CMFlag = 0;

	memset(SSIDbuf, 0, sizeof(SSIDbuf));
	getWifiInfo(SSIDbuf, SSIDbuf+20);
	SSIDbuf[10] = 0; //最多10个字节 
	memcpy(oldSSID, SSIDbuf, 10);
	Slen = strlen((char*)SSIDbuf);
	index = Slen-1;
	
	cls();
	display(0,0," 设置SSID ", DIS_CENTER|DIS_CONVERT);

	while(1)
	{
		delayxms(10);
		if(disflag == 1){
			miniDispstr(3, 0, "                      ", 0);
			miniDispstr(3,2,(char*)SSIDbuf,0);
			buffer[0] = SSIDbuf[index];
			buffer[1] = 0;
			if(buffer[0] == 0)
				buffer[0] = ' ';
			miniDispstr(3, 2+index, buffer, DIS_CONVERT);
			if(CMFlag == 0)
				display(6, 0, "F1-数字  F2-右移", 0);
			else if(CMFlag == 1)
				display(6, 0, "F1-大写  F2-右移", 0);
			else if(CMFlag == 2)
				display(6, 0, "F1-小写  F2-右移", 0);
			disflag = 0;
		}

		ret =getkey(1);
		switch(ret)
		{
		case UP:
			if(CMFlag == 0){
				if((SSIDbuf[index]<='0')||(SSIDbuf[index]>'9'))
					SSIDbuf[index]='9';
				else
					SSIDbuf[index]--;
			}
			else if(CMFlag == 1){
				if((SSIDbuf[index]<='A')||(SSIDbuf[index]>'Z'))
					SSIDbuf[index]='Z';
				else
					SSIDbuf[index]--;
			}
			else if(CMFlag == 2){
				if((SSIDbuf[index]<='a')||(SSIDbuf[index]>'z'))
					SSIDbuf[index]='z';
				else
					SSIDbuf[index]--;
			}
			disflag = 1;
			break;
		case DOWN:			
			if(CMFlag == 0){
				if((SSIDbuf[index]<'0')||(SSIDbuf[index]>='9'))
					SSIDbuf[index]='0';
				else
					SSIDbuf[index]++;
			}
			else if(CMFlag == 1){
				if((SSIDbuf[index]<'A')||(SSIDbuf[index]>='Z'))
					SSIDbuf[index]='A';
				else
					SSIDbuf[index]++;
			}
			else if(CMFlag == 2){
				if((SSIDbuf[index]<'a')||(SSIDbuf[index]>='z'))
					SSIDbuf[index]='a';
				else
					SSIDbuf[index]++;
			}
			disflag = 1;
			break;
		case ESC:
			if(index > 0){
				SSIDbuf[index] = 0;
				index--;
				disflag = 1;
			}
			else
				return;
			break;
		case F2:
			if(SSIDbuf[index] == 0)
				break;
			if(index >= 9)
				index = 0;
			else
				index++;
// 			if(CMFlag == 0){
// 				if((SSIDbuf[index]<'0')||(SSIDbuf[index]>='9'))
// 					SSIDbuf[index]='0';
// 				if((SSIDbuf[index]+3) > '9')
// 					SSIDbuf[index] = '9';
// 				else
// 					SSIDbuf[index] += 3;
// 			}
// 			else if(CMFlag == 1){
// 				if((SSIDbuf[index]<'A')||(SSIDbuf[index]>='Z'))
// 					SSIDbuf[index]='A';
// 				if((SSIDbuf[index]+10) > 'Z')
// 					SSIDbuf[index] = 'Z';
// 				else
// 					SSIDbuf[index] += 10;
// 			}
// 			else if(CMFlag == 2){
// 				if((SSIDbuf[index]<'a')||(SSIDbuf[index]>='z'))
// 					SSIDbuf[index]='a';
// 				if((SSIDbuf[index]+10) > 'z')
// 					SSIDbuf[index] = 'z';
// 				else
// 					SSIDbuf[index] += 10;
// 			}
			
			disflag = 1;
			break;
		case F1:
			if(CMFlag >= 2)
				CMFlag = 0;
			else
				CMFlag++;
			disflag = 1;
			break;
		case ENTER:
#ifdef _debug_
			debugdata(SSIDbuf, 10, 1);
#endif
			if(memcmp(SSIDbuf, oldSSID, 10) != 0){
				saveCpuInfo(10, SSIDbuf);
				GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;//tcpipClose(0);
			}
			return;
		default:
			break;
			
		}
	}
}

void setWiFIKEK(void)
{
	unsigned char oldKEK[20];
	unsigned char SSIDbuf[64];
	char buffer[2];
	unsigned char CMFlag;//输入类型：0数据，1大写，2小写
	unsigned char Slen, index, disflag, ret;

	disflag = 1;
	CMFlag = 0;

	memset(SSIDbuf, 0, sizeof(SSIDbuf));
	getWifiInfo(SSIDbuf+30, SSIDbuf);
	SSIDbuf[20] = 0; //最多10个字节 
	memcpy(oldKEK, SSIDbuf, 20);
	Slen = strlen((char*)SSIDbuf);
	index = Slen-1;
	
	cls();
	display(0,0," 设置无线密码 ", DIS_CENTER|DIS_CONVERT);

	while(1)
	{
		delayxms(10);
		if(disflag == 1){
			miniDispstr(3,0,"                      ",0);
			miniDispstr(3,2,(char*)SSIDbuf,0);
			buffer[0] = SSIDbuf[index];
			buffer[1] = 0;
			if(buffer[0] == 0)
				buffer[0] = ' ';
			miniDispstr(3, 2+index, buffer, DIS_CONVERT);
			if(CMFlag == 0)
				display(6, 0, "F1-数字  F2-右移", 0);
			else if(CMFlag == 1)
				display(6, 0, "F1-大写  F2-右移", 0);
			else if(CMFlag == 2)
				display(6, 0, "F1-小写  F2-右移", 0);
			disflag = 0;
		}

		ret =getkey(1);
		switch(ret)
		{
		case UP:
			if(CMFlag == 0){
				if((SSIDbuf[index]<='0')||(SSIDbuf[index]>'9'))
					SSIDbuf[index]='9';
				else
					SSIDbuf[index]--;
			}
			else if(CMFlag == 1){
				if((SSIDbuf[index]<='A')||(SSIDbuf[index]>'Z'))
					SSIDbuf[index]='Z';
				else
					SSIDbuf[index]--;
			}
			else if(CMFlag == 2){
				if((SSIDbuf[index]<='a')||(SSIDbuf[index]>'z'))
					SSIDbuf[index]='z';
				else
					SSIDbuf[index]--;
			}
			disflag = 1;
			break;
		case DOWN:			
			if(CMFlag == 0){
				if((SSIDbuf[index]<'0')||(SSIDbuf[index]>='9'))
					SSIDbuf[index]='0';
				else
					SSIDbuf[index]++;
			}
			else if(CMFlag == 1){
				if((SSIDbuf[index]<'A')||(SSIDbuf[index]>='Z'))
					SSIDbuf[index]='A';
				else
					SSIDbuf[index]++;
			}
			else if(CMFlag == 2){
				if((SSIDbuf[index]<'a')||(SSIDbuf[index]>='z'))
					SSIDbuf[index]='a';
				else
					SSIDbuf[index]++;
			}
			disflag = 1;
			break;
		case ESC:
			if(index > 0){
				SSIDbuf[index] = 0;
				index--;
				disflag = 1;
			}
			else
				return;
			break;
		case F2:
			if(SSIDbuf[index] == 0)
				break;
			if(index >= 19)
				index = 0;
			else
				index++;
// 			if(CMFlag == 0){
// 				if((SSIDbuf[index]<'0')||(SSIDbuf[index]>='9'))
// 					SSIDbuf[index]='0';
// 				if((SSIDbuf[index]+3) > '9')
// 					SSIDbuf[index] = '9';
// 				else
// 					SSIDbuf[index] += 3;
// 			}
// 			else if(CMFlag == 1){
// 				if((SSIDbuf[index]<'A')||(SSIDbuf[index]>='Z'))
// 					SSIDbuf[index]='A';
// 				if((SSIDbuf[index]+10) > 'Z')
// 					SSIDbuf[index] = 'Z';
// 				else
// 					SSIDbuf[index] += 10;
// 			}
// 			else if(CMFlag == 2){
// 				if((SSIDbuf[index]<'a')||(SSIDbuf[index]>='z'))
// 					SSIDbuf[index]='a';
// 				if((SSIDbuf[index]+10) > 'z')
// 					SSIDbuf[index] = 'z';
// 				else
// 					SSIDbuf[index] += 10;
// 			}
			
			disflag = 1;
			break;
		case F1:
			if(CMFlag >= 2)
				CMFlag = 0;
			else
				CMFlag++;
			disflag = 1;
			break;
		case ENTER:
			if(memcmp(oldKEK, SSIDbuf, 20)!= 0){
				saveCpuInfo(11, SSIDbuf);
				GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;//tcpipClose(0);
			}
			return;
		default:
			break;
			
		}
	}
}
#endif
void SetSevice(void)
{
//	unsigned int ll;
	unsigned char buff[30];
	unsigned char bu[5];
	if(SetupClientIP()==0){
		memcpy(bu, (unsigned char*)&ClientIP, 4);
		sprintf((char*)buff, "%03d.%03d.%03d.%03d%04d", bu[0],bu[1],bu[2],bu[3],ClientPort);
		if(gErrorFlag&ERROR_BUS_CONNECT){
			gErrortimes[1] = 0;
		}
		if((GPRSLinkProcess >= 20)&&(GPRSLinkProcess < 0xB0)){
			GPRSLinkProcess = GPRS_NEED_CLOSEIP;//tcpipClose(0);
			gSendOverTime = 0;
			gmissflag = 0;
		}
	}
}
//---------------------------------------------------------------------------------

void delallBlacInbus(void)
{
#ifdef NO_GJ_SERVISE
	return;//没有公交平台，不清除
#else
	unsigned int ulTemp;
	unsigned char pp[64];

	if(gBusblkDownPro.Miss_PPack != 0)//正在下载黑名单
		return;
	
	memset(gBusVerInfo.busBLKVer, 0xff, 2);//版本号清零
	ulTemp = FLASH_BLK_START;
	flasherase(ERASE64KBYTESECTOR, ulTemp);
	BlackLength =0;
	pFistVary.BlackListNum=0;
	//	pFistVary.BlackListNum_1=0;
	//	pFistVary.BlackListNum_2=0;
	
	// 	memset(pp, 0xff, 14);
	// 	memcpy(pp, gBusVerInfo.busBLKVer, 2);
	// 	memcpy(pp+2, (unsigned char*)&BlackLength, 4);
	// 	flashwrite(ulTemp, pp, 14);//写入黑名单版本及长度
	
	memset(pp, 0x00, 32);
	memset((unsigned char *)&gBlackBlockNum, 0, 32);
	sysfewrite(BIT_BLACK_SUM, 4, (unsigned char *)&pFistVary.BlackListNum);
	sysfewrite(BIT_BLACK_2BLOK, 32, pp);
#endif	
}
unsigned int Build_GJ_HART(unsigned char *dat)
{
	memset(dat, 0xee, 20);
	return 2;
}

unsigned int Build_GJ_His_ACHO3003(unsigned char *dat, unsigned char *sndbuf)
{
	unsigned int len;
	unsigned short crc16=0;
	
	len = 0;
	dat[len++] = 0xBD;//报文头(Head)
	dat[len++] = 0xBD;//
	dat[len++] = 0xBD;//
	
	Get_SerialNum(dat+len);
	//Ascii2BCD(pFistVary.DeviceNo, dat+len, 8);
	len += 4;
	
	dat[len++] = 0x30;//CmdCode
	dat[len++] = 0x03;
	
	dat[len++] = 1;//GnCode
	
	memset(dat+len, 0, 4);//P1p2p3p4
	len+=4;
	
	dat[len++] = 0x00;//长度=00字节
	dat[len++] = 0x18;

	memcpy(dat+len, sndbuf, 0x18);
	len += 0x18;
	
	crc16 = Getcrc16(dat, len, 0xFF, 0xFF);
	memcpy(dat+len, (unsigned char*)&crc16, 2);//CRC32 校验码
	len+=2;
	return len;	
}

extern void getCpuDeviceNO(unsigned char *dat);
//发送登陆或信息命令
//0x11:接收机监测指令 -2001: 接收机主动监测终端机，提取终端机基本信息。0x12 不发信息
//0x21:终端机告知指令 -发送基本信息,0x22:终端机告知指令 - 不发基本信息
//0x01:终端签到指令 -1001:终端机登录接收机
//BDBDBD00FBD8D810010100000000003C1001010000004A020020110428095944000000000000000007550101490132300000000000000000000000000000000000000000000000000000000059DA
unsigned int Build_GJ_LOGIN(unsigned char *dat, unsigned char mode)
{
#ifndef NO_MOBILE_24G_
	unsigned int sendenda;
	unsigned int ltemp1;
	unsigned char *ptrp;
#else
	int headp, curp;
#endif
	unsigned int ltemp;
	unsigned int i;
	unsigned int len;
	unsigned short crc16;

	unsigned char disbuf[50];
	
	getCpuDeviceNO(pFistVary.DeviceNo);
	len = 0;
	dat[len++] = 0xBD;//报文头(Head)
	dat[len++] = 0xBD;//
	dat[len++] = 0xBD;//

#ifdef BUS_SERRIER_No
	getposSN((unsigned char*)disbuf, 0);//取POS机终端编号
	dat[len++] = 0;//一共4字节，前一字节补0
	Ascii2BCD(disbuf+10, dat+len, 6);//Address:终端机编号
	len += 3;
#else 
	Get_SerialNum(dat+len);
	//Ascii2BCD(pFistVary.DeviceNo, dat+len, 8);
	len += 4;
#endif

	if((mode == 0x21)||(mode == 0x22)){//终端机告知指令 -2002: 终端机主动发送终端机基本信息给接
		dat[len++] = 0x20;//CmdCode
		dat[len++] = 0x02;
	}
	else if(mode == 0x01){//终端签到指令 -1001:终端机登录接收机
		dat[len++] = 0x10;//CmdCode
		dat[len++] = 0x01;
	}
	else{// if((mode == 0x11)||(mode == 0x12)){//终端机告知指令 -2002: 终端机主动发送终端机基本信息给接
		dat[len++] = 0x20;//CmdCode
		dat[len++] = 0x01;
	}
	
	dat[len++] = (mode&0x0F);//GnCode  取MODE的低4位

	memcpy(dat+len, "BU", 2);//设备类型 P1 P2
	len += 2;
	memset(dat+len, 0x01, 2);//P3到P4
	len+=2;
	
	if((mode & 0x0F) == 0x01){//需要发送基本信息
		dat[len++] = 0x00;//长度=60字节
		dat[len++] = 0x3C;
		
		dat[len++] = 0;
		memcpy(dat+len, pFistVary.LineNo, 2);//线路3B,后台是6位BCD。用主线，不发支线
		len += 2;
		
		ASCII2ZBcod((char*)pFistVary.DeviceNo, dat+len, 4);
//		Ascii2BCD(pFistVary.DeviceNo, dat+len, 8);//车号4B
		len +=4;
#ifdef BUS_Cloud_
		memcpy(dat+len, StufferNO, 4);//人员4B
#else
		memcpy(dat+len, DriveCardNo, 4);//人员4B
#endif
		len +=4;

		memcpy(dat+len, (unsigned char*)&SysTime.year_h, 7);
		len += 7;
		
		Ascii2BCD(GprsCardList, dat+len, 8);	//这里传输手机卡码
		len += 4;
		Ascii2BCD(GprsCardList+8, dat+len, 8);
		len += 4;
// #ifdef _GPS_ENABLE_  //不用经纬度了
// 		memcpy(dat+len, (unsigned char*)&gGpsInfo.Longitude, 4);//经度4B
// 		len += 4;
// 		memcpy(dat+len, (unsigned char*)&gGpsInfo.Latitude, 4);//+纬度4B
// 		len += 4;
// #else
// 		memset(dat+len, 0, 4);//经度4B
// 		len += 4;
// 		memset(dat+len, 0, 4);//+纬度4B
// 		len += 4;
// #endif
		
		memcpy(dat+len, PCityCode, 2);//城市代码2B
		len += 2;
		memcpy(dat+len, "\x01\x01", 2);//行业代码2B
		len += 2;

		getProVer();

		memcpy(dat+len, gBusVerInfo.busProVer, 2);//程序版本
		over_turn(2, dat+len);
#ifdef _debug_gprs
		debugstring("busProVer:");
		debugdata(gBusVerInfo.busProVer, 2, 1);
#endif
		len += 2;

		if(memcmp(gBusVerInfo.busBLKVer, "\xff\xff", 2) == 0)
			memset(gBusVerInfo.busBLKVer, 0, 2);
		memcpy(dat+len, gBusVerInfo.busBLKVer, 2);//黑名单版本2B
		over_turn(2, dat+len);
#ifdef _debug_gprs
		debugstring("buBLKVer:");
		debugdata(gBusVerInfo.busBLKVer, 2, 1);
#endif
		len += 2;
		memcpy(dat+len, gBusVerInfo.busVoiceVer, 2);//公钥信息版本2B
		over_turn(2, dat+len);

		len += 2;
		memcpy(dat+len, gBusVerInfo.busLineVer, 2);//定位限速信息版本2B
		over_turn(2, dat+len);
		len += 2;
		
		memcpy(dat+len, gBusTicketV.busticketVer, 2);//票价信息版本2B
		len += 2;

#ifdef CDMA_CASE_
		memcpy(dat+len, "7S", 2);//2B
#elif defined Card_snd1111
		memcpy(dat+len, "7X", 2);//2B
#elif defined Modem_BUS_
		memcpy(dat+len, "7X", 2);//2B
#elif defined _New_Bu_mode_
		memcpy(dat+len, "ET", 2);//2B
#else
		memcpy(dat+len, "ST", 2);//2B
#endif
		len += 2;
		
		memset(disbuf, 0, 50);
#ifndef NO_MOBILE_24G_
		if((gmissNotTrad&0xF0) == 0xA0){//上送不成功
			memcpy(disbuf, "UErr,", 5);
		}
		else{
			sendenda = mobilsendStyle.EndAddr;
			
			ltemp = (sendenda-mobilsendStyle.StartAddr);//正在发送的数据
			ltemp /= MOBILE_REC_LEN;
			ltemp1 = (gMobilREC_End+LEN_M_REC-gMobilREC_Start)%LEN_M_REC;//当前数
			ltemp1 /= MOBILE_REC_LEN;
			sprintf((char*)disbuf, "%d/%d,",ltemp,(ltemp+ltemp1));
		}
		i = strlen((char*)disbuf);
		if(gMobilStyle.ISOK == 1){//是否已经签到成功
			disbuf[i++] = 'Y';
		}
		else{
			disbuf[i++] = 'N';
		}
		disbuf[i++] = ',';
		BCD2Ascii((unsigned char*)&gG24Ver, disbuf+i, 1);
		i+= 2;
		disbuf[i++] = ',';
		
		ptrp = (unsigned char*)(0x00103FE0);//POS机序列号
		memcpy(disbuf+i, ptrp+12, 4);
#else
		
		curp = Get_Record_point((unsigned char*)&headp, 0);
		ltemp=(curp - headp)/RECORD_LEN;
		
		sprintf((char*)disbuf, "%d",ltemp);
		i = strlen((char*)disbuf);
		disbuf[i++] = ',';
		if(F_FLAG_VARY.stop_flag){
			disbuf[i] = 'P';
		}
		else{
			disbuf[i] = 'R';
		}
		i = strlen((char*)disbuf);
		disbuf[i++] = 0;//加入结束符
#endif
		
		memcpy(dat+len, disbuf, 15);//车辆编号
		len += 15;
		SYSgetbussinessNO(dat+len);//两字节商户编号

		len += 2;
		dat[len] = 0;  //无用
		len += 1;
	}
	else{
		dat[len++] = 0x00;//长度=00字节
		dat[len++] = 0x00;
	}
	
	crc16 = Getcrc16(dat, len, 0xFF, 0xFF);
	memcpy(dat+len, (unsigned char*)&crc16, 2);//CRC32 校验码
	len+=2;
	return len;
}

//下载程序应答
unsigned int Build_GJ_DLine(unsigned char *dat)
{
	unsigned int len;
	unsigned int temp;
	unsigned short crc16=0;
	
	len = 0;
	dat[len++] = 0xBD;//报文头(Head)
	dat[len++] = 0xBD;//
	dat[len++] = 0xBD;//
	
	Get_SerialNum(dat+len);
	//Ascii2BCD(pFistVary.DeviceNo, dat+len, 8);
	len += 4;
	
	dat[len++] = 0x40;//CmdCode
	dat[len++] = 0x04;
	
	dat[len++] = gGPSDownLine.Miss_style;//GnCode
	
	memcpy(dat+len, gGPSDownLine.Miss_PPack, 2);//p1,p2收到的包序号
	over_turn(2, dat+len);
	len += 2;
	
	memcpy((unsigned char *)&temp, gGPSDownLine.MISS_MAXPack, 4);//最大包号
	memcpy((unsigned char *)&crc16, gGPSDownLine.Miss_PPack, 2);//当前包号
#ifdef debug_GJ_TLVDeal_
	debugstring("Build_GJ_DLINE:");
	debugdata((unsigned char *)&temp, 4, 1);
	debugdata((unsigned char *)&crc16, 2, 1);
	
#endif
	if(crc16 < temp)
		crc16 += 1;//需要下一个包
	else
		crc16 = 0xffff;//下载完成，不需要下载了
	if(temp == 0)
		crc16 = 1;
	
	memcpy(dat+len, (unsigned char*)&crc16, 2);//P3到P4 要求传输的包序号 如果为FFFF,结束该终端传送程序
	over_turn(2, dat+len);
	len+=2;
	
	dat[len++] = 0x00;//长度=00字节
	dat[len++] = 0x00;
	
	crc16 = Getcrc16(dat, len, 0xFF, 0xFF);
	memcpy(dat+len, (unsigned char*)&crc16, 2);//CRC32 校验码
	len+=2;
	return len;
}

unsigned int Build_GJ_UREC(unsigned char *sbuf)
{
	unsigned short crc16; 
	//	unsigned char buff[10];
	if(tcpipPacket.packetStatus == RECORD_DEAL){//有需要发送的数据
		if((tcpipPacket.length > 0)&&(memcmp(tcpipPacket.tcpBuffer, "\xBD\xBD\xBD", 3) == 0)){
			// 			Get_SerialNum(buff);
			// 			if(memcmp(tcpipPacket.tcpBuffer+3, buff, 4) != 0){
			// 				memcpy(tcpipPacket.tcpBuffer+3, buff, 4);
			// 			}
			crc16 = Getcrc16(tcpipPacket.tcpBuffer, tcpipPacket.length-2, 0xFF, 0xFF);//重新计算校验码
			memcpy(tcpipPacket.tcpBuffer+tcpipPacket.length-2, (unsigned char*)&crc16, 2);//CRC32 校验码
			
			memcpy(sbuf, tcpipPacket.tcpBuffer, tcpipPacket.length);
#ifdef _debug_
			debugstring("Build Snd rec:");
			debugdata(tcpipPacket.tcpBuffer, tcpipPacket.length, 1);
#endif
			return (unsigned int)tcpipPacket.length;
		}
		tcpipPacket.packetStatus = 0;//此记录包不正常
		sysfewrite(BIT_TCPIP_PACKET, 5, (unsigned char*)&tcpipPacket);
	}
	return 0;
}
//下载程序应答
unsigned int Build_GJ_DPro(unsigned char *dat)
{
	unsigned int len;
	unsigned int temp;
	unsigned short crc16=0;
	
	len = 0;
	dat[len++] = 0xBD;//报文头(Head)
	dat[len++] = 0xBD;//
	dat[len++] = 0xBD;//
	
	Get_SerialNum(dat+len);
//Ascii2BCD(pFistVary.DeviceNo, dat+len, 8);
	len += 4;
	
	dat[len++] = 0x40;//CmdCode
	dat[len++] = 0x02;
	
	dat[len++] = gBusDownPro.Miss_style;//GnCode
	
	memcpy(dat+len, gBusDownPro.Miss_PPack, 2);//p1,p2收到的包序号
	over_turn(2, dat+len);
	len += 2;
	
	memcpy((unsigned char *)&temp, gBusDownPro.MISS_MAXPack, 4);//最大包号
	memcpy((unsigned char *)&crc16, gBusDownPro.Miss_PPack, 2);//当前包号
#ifdef debug_GJ_TLVDeal_
	debugstring("Build_GJ_DPro:");
	debugdata((unsigned char *)&temp, 4, 1);
	debugdata((unsigned char *)&crc16, 2, 1);

#endif
	if(crc16 < temp)
		crc16 += 1;//需要下一个包
	else
		crc16 = 0xffff;//下载完成，不需要下载了
	if(temp == 0)
		crc16 = 1;

	memcpy(dat+len, (unsigned char*)&crc16, 2);//P3到P4 要求传输的包序号 如果为FFFF,结束该终端传送程序
	over_turn(2, dat+len);
	len+=2;
	
	dat[len++] = 0x00;//长度=00字节
	dat[len++] = 0x00;
	
	crc16 = Getcrc16(dat, len, 0xFF, 0xFF);
	memcpy(dat+len, (unsigned char*)&crc16, 2);//CRC32 校验码
	len+=2;
	return len;
}
//黑名单应答
unsigned int Buid_GJ_DBLK(unsigned char *dat)
{
	unsigned int len;
	unsigned int temp;
	unsigned short crc16;
	
	len = 0;
	dat[len++] = 0xBD;//报文头(Head)
	dat[len++] = 0xBD;//
	dat[len++] = 0xBD;//
	
	Get_SerialNum(dat+len);
//Ascii2BCD(pFistVary.DeviceNo, dat+len, 8);
	len += 4;
	
	dat[len++] = 0x40;//CmdCode
	dat[len++] = 0x01;
	
	dat[len++] = gBusblkDownPro.Miss_style;//GnCode
	
	memcpy(dat+len, (unsigned char*)&gBusblkDownPro.Miss_PPack, 2);//p1,p2收到的包序号
	over_turn(2, dat+len);
	len += 2;
#ifdef debug_GJ_TLVDeal_
	debugstring("Buid_GJ_DBLK 包头：");
	debugdata((unsigned char*)&gBusblkDownPro.Miss_style, 1, 1);
	debugdata((unsigned char*)&gBusblkDownPro.Miss_PPack, 2, 1);
	debugdata((unsigned char*)&gBusblkDownPro.MISS_MAXPack, 4, 1);
	debugdata((unsigned char*)&gBusblkDownPro.Miss_ver, 2, 1);
	debugdata((unsigned char*)&gBusblkDownPro.Miss_ALL_LEn, 4, 1);
#endif

	temp = gBusblkDownPro.MISS_MAXPack;//最大包号
	crc16 = gBusblkDownPro.Miss_PPack;//当前包号

#ifdef debug_GJ_TLVDeal_
	debugstring("Build_GJ_DPro:");
	debugdata((unsigned char *)&temp, 4, 1);
	debugdata((unsigned char *)&crc16, 2, 1);
#endif
	if(crc16 < temp)
		crc16 += 1;//需要下一个包
	else
		crc16 = 0xffff;//下载完成，不需要下载了
	memcpy(dat+len, (unsigned char*)&crc16, 2);//P3到P4 要求传输的包序号 如果为FFFF,结束该终端传送程序
	over_turn(2, dat+len);
	len+=2;
	
	dat[len++] = 0x00;//长度=00字节
	dat[len++] = 0x00;
	
	crc16 = Getcrc16(dat, len, 0xFF, 0xFF);
	memcpy(dat+len, (unsigned char*)&crc16, 2);//CRC32 校验码
	len+=2;
	return len;
}

unsigned int BuildGJPackge(unsigned char *revBuf, unsigned char mode)
{
	unsigned int len = 0;
	//	unsigned int blen;
	
#ifdef _debug_
	debugstring("BuildGJPackge mode:");
	debugdata((unsigned char*)&mode, 1, 1);
#endif
	switch (mode)
	{
	case MISS_G_TOO:
		len = Build_GJ_LOGIN(revBuf, 0x22);
		break;
	case MISS_G_LOGINGJ://公交登陆
		len = Build_GJ_LOGIN(revBuf, 0x01);
		break;
	case MISS_G_DBLKD://下黑名单
	case MISS_G_DBLKI://下黑名单
		debugstring("MISS_G_DBLKD\r\n");
		//		len = Buid_GJ_DBLK(revBuf, mode);
		break;
	case MISS_G_UREC://上传记录
		len = Build_GJ_UREC(revBuf);
		break;
#ifdef _GPS_DIAODU_
	case MISS_G_GPS:
		len = Build_GJ_GPS(revBuf);
		break;
	case MISS_G_ALAM:
		len = Build_GJ_ALAM(revBuf);
#ifdef _debug_
		debugstring("galamessage.reSendTimes:");
		debugdata((unsigned char*)&galamessage.reSendTimes, 1, 0);
#endif
		galamessage.reSendTimes++;
		if((galamessage.reSendTimes > 3)||(len == 0)){//连发三次，取消
			galamessage.style = 0xE1;
		}
		break;
#endif
	case MISS_G_DPRO://程序下载
	case MISS_G_DLine://线路参数据下载
	case MISS_G_DSound://语音参数据下载
		debugstring("MISS_D_DPRO\r\n");
		// 		len = Build_GJ_DPro(revBuf, mode);
		break;
	case MISS_G_DPROOK:
	case MISS_G_DLineOK:
	case MISS_G_DSoundOK:
		debugstring("MISS_G_DPROOK\r\n");
		break;
	case MISS_G_HART://发心跳包
		//		len = Build_GJ_HART(revBuf);
		debugstring("MISS_G_HART\r\n");
		break;
	default:
		len = 0;
		break;
	}
	return len;
}

//组织发送的公交记录包
unsigned int Build_GJRec_Send(unsigned char cnum, unsigned int *headp)//
{
	unsigned int len;
//	unsigned int temp;
	unsigned short crc16;
	unsigned int addr;
	unsigned char disbuf[256];
//	unsigned int i;
	unsigned char dat[500];

	len = 0;
	dat[len++] = 0xBD;//报文头(Head)
	dat[len++] = 0xBD;//
	dat[len++] = 0xBD;//
	
	Get_SerialNum(dat+len);
	//Ascii2BCD(pFistVary.DeviceNo, dat+len, 8);
	len += 4;
	
	dat[len++] = 0x50;//CmdCode
#ifdef BUS_SYBAOSHI_
	dat[len++] = 0x05; //回应时需要传回手机号码和短信内容。
#else
	dat[len++] = 0x03;
#endif
	
	dat[len++] = 0x01;//GnCode
	
	
	sysferead(FE_PACKSERIAL, 2, dat+len);	//P1,p2包序列号
	memcpy((unsigned char*)&tcpipPacket.Serial, dat+len, 2);
	len += 2;

	memcpy(dat+len, (unsigned char**)&headp, 2);//p1,p2,p3,p4 用于保存这条记录的地址
	len += 2;
	
	dat[len++] = 0x00;//长度=64字节
	dat[len++] = 0x40;
	
	addr = *headp;
//	for(i=0; i<cnum; i++){//只发送一条
		flashread(addr, disbuf, RECORD_LEN);
		if(memcmp(disbuf, "银行数据", 8) == 0){//
			memcpy(dat+len, disbuf+8, (RECORD_LEN-8));
			len += (RECORD_LEN-8);//56
			
			if(addr < FLASH2_START1){//当前区才写0,历史区不写0
				memset(disbuf, 0, RECORD_LEN);
				flashwrite(addr, disbuf, RECORD_LEN);//把这条写成全0，
			}
			addr += RECORD_LEN;
			if(addr >= FLASH2_START2END){
				addr = FLASH2_START1;
			}

			flashread(addr, dat+len, RECORD_LEN);
			len += RECORD_LEN;//64
			
			if(addr < FLASH2_START1){//当前区才写0,历史区不写0
				memset(disbuf, 0, RECORD_LEN);
				flashwrite(addr, disbuf, RECORD_LEN);//把这条写成全0，
			}
			addr += RECORD_LEN;
			if(addr >= FLASH2_START2END){
				addr = FLASH2_START1;
			}

			flashread(addr, dat+len, 30);
			len += 30;//30          56+64+30 = 150
		}
		else{
			memcpy(dat+len, disbuf, RECORD_LEN);
			len+=RECORD_LEN;
		}

#ifdef _debug_gprs
		if(addr < FLASH2_START1)
			debugstring("生成当前发送包:");
		else
			debugstring("生成历史发送包:");
		debugdata(dat+len, RECORD_LEN, 1);
#endif
		if(addr < FLASH2_START1){//当前区才写0,历史区不写0
			memset(disbuf, 0, RECORD_LEN);
			flashwrite(addr, disbuf, RECORD_LEN);//把这条写成全0，
		}
		
		addr += RECORD_LEN;
		if(addr >= FLASH2_START2END){
			addr = FLASH2_START1;
		}
		if(addr >= FLASH2_START1)
			*headp = addr;
//	}

	dat[GCMD_Ln] = ((len - 16)/256);
	dat[GCMD_Ln+1] = ((len - 16)%256);//重新计算长度

	crc16 = Getcrc16(dat, len, 0xFF, 0xFF);
	memcpy(dat+len, (unsigned char*)&crc16, 2);//CRC32 校验码
	len+=2;

	tcpipPacket.packetStatus = RECORD_DEAL;
	tcpipPacket.length = len;//要发送的长度
	memcpy(tcpipPacket.tcpBuffer, dat, len);//要发送的数据
	sysfewrite(BIT_TCPIP_PACKET, sizeof(TCP_IP_PACKET1), (unsigned char*)&tcpipPacket);
#ifdef debug_GJ_TLVDeal_
	debugstring("生成发送包完成:");
	debugdata((unsigned char*)&*headp, 4, 1);
	debugdata((unsigned char*)&tcpipPacket.packetStatus, 1, 1);
	debugdata((unsigned char*)&tcpipPacket.length, 2, 1);
	debugdata((unsigned char*)&tcpipPacket.tcpBuffer, tcpipPacket.length, 1);
#endif
	return len;
}
unsigned int Build_GJ_LINE_ACHO(unsigned char *dat, unsigned char Gncode, stLoadPtable *pp)
{
	unsigned int len;
	unsigned short crc16;
	
	len = 0;
	dat[len++] = 0xBD;//报文头(Head)
	dat[len++] = 0xBD;//
	dat[len++] = 0xBD;//
	
	Get_SerialNum(dat+len);
//Ascii2BCD(pFistVary.DeviceNo, dat+len, 8);
	len += 4;
	
	memcpy(dat+len, "\x40\x05", 2);
	len += 2;
	
	dat[len++] = Gncode;//GnCode
	
	memset(dat+len, 0, 4);//p1,p2收到的包序号
	len += 4;
	
	dat[len++] = 0x00;//长度=12字节
	dat[len++] = 12;
	memcpy(dat+len, pp->bussiness, 2);
	len += 2;
	memcpy(dat+len, pp->lineVer, 2);
	len += 2;
	memcpy(dat+len, pp->LineNo, 4);
	len += 4;
	memcpy(dat+len, pp->deviceNo, 4);
	len += 4;
	
	crc16 = Getcrc16(dat, len, 0xFF, 0xFF);
	memcpy(dat+len, (unsigned char*)&crc16, 2);//CRC32 校验码
	len+=2;
	return len;
}
#ifdef _GPS_DIAODU_
//mode = 0：成功 其它：失败
unsigned int Build_DIAODU_Echo(unsigned char *dat, unsigned short cmdcode, unsigned char Gncode, unsigned char mode, unsigned char *p1p2p3)
{
	unsigned int len;
	unsigned short crc16;
	
	len = 0;
	dat[len++] = 0xBD;//报文头(Head)
	dat[len++] = 0xBD;//
	dat[len++] = 0xBD;//
	
	Get_SerialNum(dat+len);
//Ascii2BCD(pFistVary.DeviceNo, dat+len, 8);
	len += 4;
	
	memcpy(dat+len, (unsigned char*)&cmdcode, 2);
	len += 2;
	
	dat[len++] = Gncode;//GnCode
	
	memcpy(dat+len, p1p2p3, 3);//p1,p2,p3调度机号
	len += 3;
	dat[len++] = 0x01;//车载编端

	dat[len++] = 0x00;//长度=02字节
	dat[len++] = 0x05;
	dat[len++] = gDiaoDuCMD.ucCMD;
	dat[len++] = gDiaoDuCMD.ZLseiral[0];
	dat[len++] = gDiaoDuCMD.ZLseiral[1];
	if(mode == 0){
		dat[len++] = 0x90;//
		dat[len++] = 0x00;
	}
	else{
		dat[len++] = 0x10;//
		dat[len++] = 0x00;
	}

	crc16 = Getcrc16(dat, len, 0xFF, 0xFF);
	memcpy(dat+len, (unsigned char*)&crc16, 2);//CRC32 校验码
	len+=2;
	return len;
}
#endif
//mode = 0：成功 其它：失败
unsigned int Build_Echo(unsigned char *dat, unsigned short cmdcode, unsigned char Gncode, unsigned char mode)
{
	unsigned int len;
	unsigned short crc16;
	
	len = 0;
	dat[len++] = 0xBD;//报文头(Head)
	dat[len++] = 0xBD;//
	dat[len++] = 0xBD;//
	
	Get_SerialNum(dat+len);
//Ascii2BCD(pFistVary.DeviceNo, dat+len, 8);
	len += 4;
	
	memcpy(dat+len, (unsigned char*)&cmdcode, 2);
	len += 2;
	
	dat[len++] = Gncode;//GnCode
	
	memset(dat+len, 0, 4);//p1,p2收到的包序号
	len += 4;
	
	dat[len++] = 0x00;//长度=02字节
	dat[len++] = 0x02;
	
	if(mode == 0){
		dat[len++] = 0x90;//
		dat[len++] = 0x00;
	}
	else{
		dat[len++] = 0x10;//
		dat[len++] = 0x00;
	}

	crc16 = Getcrc16(dat, len, 0xFF, 0xFF);
	memcpy(dat+len, (unsigned char*)&crc16, 2);//CRC32 校验码
	len+=2;
	return len;
}

void GJRec_Send(void)
{
	struct{
		unsigned int shadd;
		unsigned int sLen;
		unsigned char sdate[8];
		unsigned int scrc32;
	}hisda;
	unsigned int sum;
	unsigned int curp, headp;
	
	if(tcpipPacket.packetStatus==RECORD_DEAL)
	{
		return;
	}
	else
	{
		if(tcpipPacket.packetStatus>0)
			tcpipPacket.packetStatus--;

		if((tcpipPacket.packetStatus == 0) || (tcpipPacket.packetStatus > 60))
			tcpipPacket.packetStatus = 0;
		else
			return;
		//先处理是否有历史记录发送
		sysferead(BIT_HISREC_SND, 20, (unsigned char *)&hisda);
		sum = __cpuCrc32((unsigned char *)&hisda, 16);
#ifdef _debug_
		debugstring("GJRec Send his data:");
		debugdata((unsigned char *)&hisda, 20, 1);
#endif
		if((hisda.scrc32 == sum)&&(hisda.sLen != 0)&&(hisda.sLen <= (FLASH2_BAKR_LEN/RECORD_LEN))&&(hisda.shadd < FLASH2_BAKR_LEN)){//有历史记录
			headp = FLASH2_START1+hisda.shadd;
			sum = 1;//新协议，一次传一条
			Build_GJRec_Send(sum, &headp);

			hisda.shadd = headp-FLASH2_START1;
			if(hisda.sLen >= 1)
				hisda.sLen -= 1;
			else
				hisda.sLen = 0;
			hisda.scrc32 = __cpuCrc32((unsigned char *)&hisda, 16);
#ifdef _debug_
			debugstring("GJRec Send his END:");
			debugdata((unsigned char *)&hisda, 20, 1);
#endif
			sysfewrite(BIT_HISREC_SND, 20, (unsigned char*)&hisda);
			return;
		}

		//---------------------------------------

		curp = Get_Record_point((unsigned char*)&headp, 0);
	//如果记录缓冲中没有记录可发将发送E2PROM中没有发成功的记录
		if(curp>headp)
		{
#ifdef _debug_
			debugstring("Read Record From E2prom to TCP/IP !\r\n");
#endif
// 			sum=0;
// 			sum=(unsigned int)(curp - headp);
// 		
// 			if(sum > 384){//数据总长度在384以内 32字节最多12条，64字节最多6条
// 				sum = (384/RECORD_LEN);
// 			}
// 			else
// 				sum /= RECORD_LEN;

			sum = 1;//新协议，一次传一条
			Build_GJRec_Send(sum, &headp);
		}
		else{
			if(curp >= (FLASH1_START+FLASH_RCLR)){//0x3E800如果记录地址已经达到了40000条（64字节），则重置存贮区，重新开始
				lib_clr_Record_point();
			}
			tcpipPacket.packetStatus = 40;
		}
	}
}

void find_G_new_mission(void)//此任务一秒进一次
{
	unsigned short usTemp=0;
#ifdef _debug_
	unsigned char btf[20];
#endif

#ifdef SelMsg_enable
	if(findSelMsg() == 0){//有短信息需要发送
		gmissflag = MISS_G_MSG1;
		return;
	}
#endif

	if(gErrorFlag&ERROR_BUS_CONNECT){
#ifdef _debug_
		sprintf((char*)btf, "公:%u", gErrortimes[1]);
		debugstring((char*)btf);
#endif
		if(gErrortimes[1] > 0){
			if((gmissflag & MISS_GJ)!=0)
				gmissflag = 0;
			return;//上次连接错误,时间没到不给任务.
		}
	}
	
	gErrorFlag &= (~ERROR_BUS_CONNECT);// 清除错误标识
#ifdef BUS_JIUJIANG_
	if(tcpipPacket.TTL != 0){	//刚刷了卡，没到时间，不外理GPRS任务
		gmissflag = 0;
		return;
	}
#endif

	memcpy((unsigned char*)&usTemp, gBusVerInfo.newbusBLKVer, 2);
	if((usTemp ==  0)||(usTemp ==  0xffff)){//服务器的版本号.没有取到
#ifdef _debug_gprs
		debugstring("登陆回应黑名单不对，重发回应:");
		debugdata(gBusVerInfo.newbusBLKVer, 2, 1);
//		debugdata(btf, 7, 1);
#endif
		gmissflag = MISS_G_LOGINGJ;
		return;
	}
	//下载程序需要后台主动，后台程序版本需要后台主动给下载命令才能重新判断续传位置

	if(BlackLength>=BLACKSUM){//黑名单条数错误，重下黑名单
#ifdef _debug_
		debugstring("黑名单条数错误，重下黑名单\r\n");
#endif
		memset((unsigned char*)&gBusblkDownPro, 0, sizeof(stbusdownPara));
		sysfewrite(BIT_BUSblkDownPRO_INFO, sizeof(stbusdownPara), (unsigned char*)&gBusblkDownPro);//保存任务参数
		delallBlacInbus();
	}
	//黑名单下载需也是后台主动下载。
	
#ifdef _GPS_DIAODU_
	if(galamessage.style == 0x5A)//5A 表示需要发送GPS报警等信息
	{
		gmissflag = MISS_G_ALAM;
		set_timer0(100,0);
		return;
	}
#endif
	if(tcpipPacket.packetStatus == RECORD_DEAL){//有记录包需要发送
		if(tcpipPacket.TTL == 0){
			gmissflag = MISS_G_UREC;//发送任务
			set_timer0(100, 0);//加快进入发送任务
		}
	}
	else{
		if(tcpipPacket.TTL == 0)
			GJRec_Send();//处理GPRS发送数据
		Sendpincount++;
		gSendGLogin++;
		gmissflag = MISS_G_FREE;//无任务
		if(gSendGLogin > SEND_PIN_COUNT_){
			gSendGLogin = 0;
			gmissflag = MISS_G_TOO;
		}
	}
}

void load_black_list(void)
{
	unsigned int addr;
//	unsigned char btf[20];
	unsigned short ii=0;
	unsigned char i;
	
	sysferead(BIT_BLACK_SUM,4,(unsigned char*)&pFistVary.BlackListNum);
	sysferead(BIT_BLACK_2BLOK, 32, (unsigned char*)&gBlackBlockNum[0]);
		// 
#ifdef debug_GJ_TLVDeal_
	debugstring("BlackListNum:::");
	debugdata((unsigned char*)&pFistVary.BlackListNum,4,1); //黑名单总数
	debugdata((unsigned char*)&gBlackBlockNum[0],32,1);
#endif

	addr = FLASH_BLK_START;
	flashread(addr, gBusVerInfo.busBLKVer, 2);//黑名单版本
#ifdef debug_GJ_TLVDeal_
	debugstring("gBusVerInfo.busBLKVer:");
	debugdata(gBusVerInfo.busBLKVer, 2, 1);
	debugstring("gBusVerInfo.busProVer:");
	debugdata(gBusVerInfo.busProVer, 2, 1);
#endif
	memcpy((unsigned char*)&ii, gBusVerInfo.busBLKVer, 2);
	
	for(i=0; i<16; i++){
#ifdef debug_GJ_TLVDeal_
		debugstring("BIT_BLACK_2BLOK:::");
		debugdata((unsigned char*)&gBlackBlockNum[i],2,1); //黑名单总数
#endif
		if(gBlackBlockNum[i] == 0xffff)
			gBlackBlockNum[i] = 0;
		if(gBlackBlockNum[i] > 1000)
		{
			gBlackBlockNum[i] = 0;
			ii = 0;
		}
	}

	if((ii == 0)||(ii == 0xffff)||(pFistVary.BlackListNum >= BLACKSUM)){//黑名单版本不正确，条数不正确，清0重新下载
		delallBlacInbus();
		pFistVary.BlackListNum = 0;
	}
	
	if(pFistVary.BlackListNum > 0){
		if(gBlackBlockNum[0] == 0){
			ii = pFistVary.BlackListNum/1000;
			for(i=0; i<ii; i++){
				gBlackBlockNum[i] = 1000;
			}
			sysfewrite(BIT_BLACK_2BLOK, 32, (unsigned char*)&gBlackBlockNum[0]);
			gBlackBlockNum[i] = pFistVary.BlackListNum%1000;
		}
	}

	addr+=14;
//	temp = pFistVary.BlackListNum*4;//黑名单数据
	BlackLength = pFistVary.BlackListNum;
#ifdef debug_GJ_TLVDeal_
	debugdata(gBusVerInfo.busBLKVer, 2, 1);
	debugstring("pFistVary.BlackListNum:");
	debugdata((unsigned char*)&pFistVary.BlackListNum, 4, 1);
	debugdata((unsigned char*)&gBlackBlockNum, 32, 1);
#endif

}

void ClrGPRSFE(void)
{
	unsigned int addr;

	addr = FLASH_BLK_START;
	flasherase(ERASE64KBYTESECTOR, addr);

	memset((unsigned char*)&tcpipPacket, 0, sizeof(tcpipPacket));
	sysfewrite(BIT_TCPIP_PACKET, 10, (unsigned char*)&tcpipPacket);

	memset((unsigned char*)&gBusDownPro, 0, sizeof(stbusdownpro));//下载信息不正确，初始化
	sysfewrite(BIT_BUSDownPRO_INFO, 27, (unsigned char*)&gBusDownPro);//取上次下载参数据,用于续传

	memset((unsigned char*)&gGPSDownLine, 0, sizeof(stGPSdownLine));//下载信息不正确，初始化
	sysfewrite(BIT_GPSDownLine_INFO, sizeof(stGPSdownLine), (unsigned char*)&gGPSDownLine);//取上次下载参数据,用于续传
}

void loadGPRSFE(void)
{
	unsigned int i;//,j;
	unsigned short j;
//	unsigned char *ptrp;
	
	sysferead(BIT_TCPIP_PACKET, sizeof(TCP_IP_PACKET1), (unsigned char*)&tcpipPacket.packetStatus);
#ifdef debug_GJ_TLVDeal_
	debugstring("tcpippacet packetStatus:");
	debugdata((unsigned char*)&tcpipPacket.packetStatus, 1, 1);
	debugstring("tcpippacet len:");
	debugdata((unsigned char*)&tcpipPacket.length, 2, 1);
	debugstring("tcpippacet Serial:");
	debugdata((unsigned char*)&tcpipPacket.Serial, 2, 1);
#endif

	memset(gBusVerInfo.busLineVer, 0, 2);
	memset(gBusVerInfo.busVoiceVer, 0, 2);
	
	getProVer();
	
	sysferead(BIT_GPSDownLine_INFO, sizeof(stGPSdownLine), (unsigned char*)&gGPSDownLine);//取上次下载参数据,用于续传
	sysferead(BIT_BUSDownPRO_INFO, sizeof(stbusdownpro), (unsigned char*)&gBusDownPro);//取上次下载参数据,用于续传
#ifdef debug_GJ_TLVDeal_
	debugstring("gBusDownPro:");
	debugdata((unsigned char*)&gBusDownPro, sizeof(stbusdownpro), 1);
#endif
	if(((gBusDownPro.Miss_style != 0x01) && (gBusDownPro.Miss_style != 0x02))||(gBusDownPro.MISS_flag != 0xEA)){
		memset((unsigned char*)&gBusDownPro, 0, sizeof(stbusdownpro));//下载信息不正确，初始化:EA 01 E2 02 00 00 02 00 F0 E1 02 00 20 11 05 16 10 43 30 00 00 00 00 00 00 00 00 
	}
	if(gBusDownPro.MISS_flag == 0xEA){
		memcpy((unsigned char*)&j, gBusDownPro.Miss_PPack, 2);
		memcpy((unsigned char*)&i, gBusDownPro.MISS_MAXPack, 4);
#ifdef debug_GJ_TLVDeal_
		debugstring("gBusDownPro 2:");
		debugdata((unsigned char*)&j, 2, 1);
		debugdata((unsigned char*)&i, 4, 1);
#endif
		if((j == 0) ||(j > i))
			memset((unsigned char*)&gBusDownPro, 0, sizeof(stbusdownpro));
	}
#ifdef debug_GJ_TLVDeal_
	debugstring("gBusDownPro 3:");
	debugdata((unsigned char*)&gBusDownPro, sizeof(stbusdownpro), 1);
#endif
	
//	memset((unsigned char*)&gBusblkDownPro, 0, sizeof(stbusdownPara));//清除gBusblkDownPro ，gBusblkDownPro指向gbusblkbuf
	sysferead(BIT_BUSblkDownPRO_INFO, sizeof(stbusdownPara), (unsigned char*)&gBusblkDownPro);//任务参数
	
	load_black_list();

	memset(gBusVerInfo.newbusBLKVer, 0, 2);
	memset(gBusVerInfo.newbusLineVer, 0, 2);
	memset(gBusVerInfo.newbusProVer, 0, 2);
	memset(gBusVerInfo.newbusVoiceVer, 0, 2);
	memset(gBusTicketV.newbusticketVer, 0, 2);
	
	Sendpincount = 0;
	gSendGLogin = 0;
	gsendError = 0;
	gGPRS_Cmd_outtime = 0;
#ifdef SelMsg_enable
	SelMsgInit();
#endif
}
void saveBlackInfo(void)
{
	unsigned int ulTemp;
	unsigned char i;
	unsigned char buf[20];
	unsigned char btt[14];

//	temp1 = BlackLength*4;
	
	ulTemp = FLASH_BLK_START;
/*	flasherase(ERASE64KBYTESECTOR, ulTemp);*/  //只写版本号，不擦除

	memset(btt, 0xff, 14);
	memcpy(btt, gBusVerInfo.busBLKVer, 2);
	memcpy(btt+2, (unsigned char *)&BlackLength, 4);

	for(i=0;i<3;i++){
		flashwrite(ulTemp, btt, 14);//写入版本号
		flashread(ulTemp, buf, 14);
//	debugdata(buf, 14, 1);
		if(memcmp(buf, btt, 14) == 0)
			break;
	}
// 	ulTemp+=14;
// 	for(uiTemp=0;uiTemp<temp1;){
// 		flashwrite(ulTemp, blkBuf+uiTemp, 128);
// 		uiTemp+=128;
// 		ulTemp+=128;
// 	}
// 	sysfewrite(BIT_BLACK_SUM, 2, (unsigned char *)&BlackLength);//写入黑名单长度
// 	pFistVary.BlackListNum =BlackLength;
#ifdef _debug_gprs
	debugstring("写黑名单完毕\r\n");
	debugdata(gBusVerInfo.busBLKVer, 2, 1);
	debugstring("刷新后的黑名单总数::");
	debugdata((unsigned char *)&BlackLength, 2, 1);
	
	ulTemp = FLASH_BLK_START;
	flashread(ulTemp, gBusVerInfo.busBLKVer, 2);
	debugstring("读出版本号：");
	debugdata(gBusVerInfo.busBLKVer, 2, 1);
#endif
}

//只用于增加
unsigned char BlackUpdataAddOrDel(unsigned char *ulCardNo,unsigned int blkLen,unsigned char *blkBuf)
{
	unsigned short i_start=0;
	unsigned short i_end;
	unsigned short i_cur;
	unsigned short invalid_point;
	unsigned char flag =0 ;
	unsigned int ulTempBlackNo;
	unsigned int ulTempCardNo;
	unsigned int i;

//复制黑名单到中
	memcpy((unsigned char *)&ulTempCardNo,ulCardNo,4);

	i_end =blkLen;
	
	if (blkLen ==0) 
	{
		memset(blkBuf,0,4);
	}
#ifdef _debug_
	debugstring("黑名单BlackLength:");debugdata((unsigned char *)&blkLen, 4, 1);
#endif

	if(BlackLength >= BLACKSUM)
		return 0;
#ifdef _debug_
	debugstring("增加:");debugdata(ulCardNo, 4, 1);
#endif
	if(blkLen == 0){
		memcpy(blkBuf,(unsigned char *)&ulTempCardNo,4);
		return 1;
	}

	while(1)
	{
		clr_wdt();
		i_cur=i_start+(i_end-i_start)/2;
		invalid_point=i_cur*4;
		memcpy((unsigned char *)&ulTempBlackNo,blkBuf+invalid_point,4);
		
#ifdef _debug_
	debugstring("invalid_point:");
	debugdata((unsigned char *)&invalid_point,2,1);
	debugstring("i_cur:");
	debugdata((unsigned char *)&i_cur,2,1);
	debugdata((unsigned char *)&ulTempBlackNo,4,1);
#endif
			if (ulTempBlackNo ==ulTempCardNo) 
			{
				flag =0;
				break;
			}
			else if (ulTempCardNo > ulTempBlackNo) 
			{
				if(i_start==i_end)
					{	
						flag =1;
						i_cur =i_start;
						break;
					}
					else
					{
						i_start=i_cur+1;
						if(i_start == i_end){
							flag =1;
							break;
						}
					}
			}
			else if(ulTempCardNo < ulTempBlackNo) 
			{
				if(i_start==i_cur)
				{
					flag =1;
					break;
				}
				else
				{
					i_end=i_cur;//-1
				}
			
			}

		}

//添加处理
#ifdef _debug_
	debugstring("invalid_point:");
	debugdata((unsigned char *)&invalid_point,2,1);
	debugstring("i_start:");
	debugdata((unsigned char *)&i_start,2,1);
	debugstring("i_cur:");
	debugdata((unsigned char *)&i_cur,2,1);
	debugstring("i_end:");
	debugdata((unsigned char *)&i_end,2,1);
	debugdata((unsigned char *)&ulTempBlackNo,4,1);
#endif
	if (flag ==1) 
	{
#ifdef _debug_
		debugstring("移动黑名单 ");
#endif			
		if (ulTempCardNo < ulTempBlackNo) //38.39
		{
#ifdef _debug_
			debugstring("1\r\n");
			debugdata((unsigned char*)&i, 4, 1);
#endif
		}
		else if(ulTempCardNo > ulTempBlackNo)//38<39
		{
#ifdef _debug_
			debugstring("2\r\n");
#endif			
			i_cur++;
		}
		else{
			return 0;
		}
		i = 4096-(i_cur+1)*4;
		memmove(blkBuf+(i_cur+1)*4,blkBuf+i_cur*4,i);
		memcpy(blkBuf+i_cur*4,(unsigned char *)&ulTempCardNo,4);
		return 1;

	}
	else
	{
		flag =0;
		return 0;
	}
}
//删除黑名单
unsigned char DealGPrsBLOckDEC(unsigned char block, unsigned char *blkBuff)
{
	stAddBLKTEMP *blkTemp;
	unsigned char buff[20];
	unsigned char buffT[20];
	unsigned int addr;
	unsigned int uit;
	unsigned char i, ucBcc;

	addr = FLASH_TEMP_START;
	while(addr < FLASH_TEMP_END){
		memset(buffT, 0xff, 20);
		flashread(addr, buff, 14);
#ifdef _debug_gprs
		debugstring("临时黑名单:");
		debugdata((unsigned char*)&addr, 4, 0);
		debugstring("||");
		debugdata(buff, 14, 1);
#endif
		if(memcmp(buff, buffT, 14) == 0){//到FFFFFF了，结束
			return ST_OK;
		}
		
		clr_wdt();
		ucBcc = 0;
		for(i=0;i<13;i++)
			ucBcc ^= buff[i];
		if(ucBcc != buff[13]){
#ifdef _debug_gprs
			debugstring("临时黑名单BCC错\r\n");
#endif
			addr += 14;
			continue;
		}
		blkTemp = (stAddBLKTEMP *)buff;
		if((memcmp(blkTemp->flag, "黑单", 4) != 0)||(blkTemp->DorI == 0)){
#ifdef _debug_gprs
			debugstring("DealGPrsBLOckDEC 单标志错\r\n");
#endif
			addr += 14;
			continue;
		}
		uit = 0;
		memcpy((unsigned char*)&uit, blkTemp->index, 3); // 判断块号
		if((uit > (FLASH_BLK_START+((block) * 4096))) &&(uit < (FLASH_BLK_START+((block+1) * 4096)))){//在此块的范围内
//			uit = uit - FLASH_BLK_START;
			if(block == 0)
				uit = BinSCH(gBlackBlockNum[block], blkBuff+14, blkTemp->CardNo, BLK_SNO_LEN);//第一块前14字节为版本号
			else
				uit = BinSCH(gBlackBlockNum[block], blkBuff, blkTemp->CardNo, BLK_SNO_LEN);
#ifdef _debug_gprs
			debugstring("删除黑名单1:");
			debugdata((unsigned char*)&uit, 4, 0);
			debugdata((unsigned char*)&gBlackBlockNum[block], 2, 1);
			
#endif
			if(uit == 0xffffffff){
				addr += 14;
				continue;
			}
			if(block == 0)
				memmove(blkBuff+14+uit, blkBuff+14+uit+BLK_SNO_LEN, ((gBlackBlockNum[block]*BLK_SNO_LEN)-uit));
			else
				memmove(blkBuff+uit, blkBuff+uit+BLK_SNO_LEN, ((gBlackBlockNum[block]*BLK_SNO_LEN)-uit));
			if(gBlackBlockNum[block] > 0)
				gBlackBlockNum[block]--;
			if(BlackLength > 0)
				BlackLength--;
			memset(buffT, 0, 14);
			flashwrite(addr, buffT, 14);
#ifdef _debug_gprs
				debugstring("删除黑名单2:");
			debugdata((unsigned char*)&uit, 4, 0);
			debugdata((unsigned char*)&gBlackBlockNum[block], 2, 1);
			
#endif
		}
		addr += 14;
	}
	return ST_OK;
}
//增加黑名单
unsigned char DealGPrsBLOckADD(unsigned char block, unsigned char *blkBuff)
{
	stAddBLKTEMP *blkTemp;
	unsigned char buff[20];
	unsigned char buffT[20];
	unsigned int addr;
	unsigned char i, ucBcc;
	
	addr = FLASH_TEMP_START;
	while(addr < FLASH_TEMP_END){
		memset(buffT, 0xff, 20);
		flashread(addr, buff, 14);
#ifdef _debug_gprs
		debugstring("临时黑名单:");
		debugdata((unsigned char*)&addr, 4, 0);
		debugstring("||");
		debugdata(buff, 14, 1);
#endif
		clr_wdt();
		if(memcmp(buff, buffT, 14) == 0){//到FFFFFF了，结束
			return 0;
		}
		
		ucBcc = 0;
		for(i=0;i<13;i++)
			ucBcc ^= buff[i];
		if(ucBcc != buff[13]){
#ifdef _debug_gprs
			debugstring("临时黑名单BCC错\r\n");
#endif
			addr += 14;
			continue;
		}
		blkTemp = (stAddBLKTEMP *)buff;
		if((memcmp(blkTemp->flag, "黑单", 4) != 0)||(blkTemp->DorI == 1)){
#ifdef _debug_gprs
			debugstring("临时黑名单标志错\r\n");
#endif
			addr += 14;
			continue;
		}
 		if(gBlackBlockNum[block] < 1000){//此块有空间，可以存入
			if(block == 0){
				if(BlackUpdataAddOrDel(blkTemp->CardNo, gBlackBlockNum[block], blkBuff+14) == 1){
					gBlackBlockNum[block]++;
					BlackLength++;
				}
			}
			else{
				if(BlackUpdataAddOrDel(blkTemp->CardNo, gBlackBlockNum[block], blkBuff) == 1){
					gBlackBlockNum[block]++;
					BlackLength++;
				}
			}
			memset(buffT, 0, 14);
			flashwrite(addr, buffT, 14);
		}
		else
			return 1;//此块已经存满
	}
	return 1;
}
//处理临时区的增减量黑名单 先处理删除的名单，再处理增加的名单。都按块单独处理。
unsigned char TempDealBlack(void)
{
	unsigned char blkTempbuff[5120];
	unsigned int addr, uiT;
	unsigned char block, tt;
	unsigned char addflag = 1;//如果是1有增量黑名单
	
	addflag = 1;
	for(block=0; block<16; block++){//从第0块开始处理
		clr_wdt();
		addr = FLASH_BLK_START+(block*4096);//每块4K
		tt = 0;
		if(block == 0){//读出前面的版本号
			flashread(addr, blkTempbuff, 14);
			addr += 14;
			tt = 14;
		}
		for(uiT=0; uiT<(16); uiT++){
			flashread(addr, blkTempbuff+tt+(uiT*256), 256);
			addr += 256;
		}
#ifdef debug_GJ_TLVDeal_
			debugstring("Temp Deal Black 1 ");
			debugdata((unsigned char*)&block, 1, 1);
			debugdata((unsigned char*)&gBlackBlockNum[block], 2, 1);
#endif
		if(gBlackBlockNum[block] == 0){//下一个包开始没有黑名单了，
			if(addflag == 0){//而且没有增加的，处理完成
#ifdef debug_GJ_TLVDeal_
				debugstring("Temp ST_OK 4 \r\n");
#endif
				return ST_OK;
			}
		}
		else{
#ifdef debug_GJ_TLVDeal_
			debugstring("Temp BLOckDEC 4 \r\n");
#endif
			DealGPrsBLOckDEC(block, blkTempbuff);//处理这块的删除卡号
#ifdef debug_GJ_TLVDeal_
			debugstring("Temp BLOckDEC OK \r\n");
#endif
		}

		if(gBlackBlockNum[block]< 1000){//有剩余空间.处理增加的黑名单
			if(addflag){//如果还有
#ifdef debug_GJ_TLVDeal_
				debugstring("Temp Deal Black 2 ");
				debugdata((unsigned char*)&block, 1, 1);
#endif
				addflag = DealGPrsBLOckADD(block, blkTempbuff);
			}
		}

		addr = FLASH_BLK_START+(block*4096);//每块4K 此块删除、增加都已经处理完  写入数据
		flasherase(ERASE4KBYTESECTOR, addr);
		if(block == 0){
			memset(blkTempbuff, 0xFF, 14);//第一块是版本号，因在后面写入，所以这里先写入FF
		}
#ifdef debug_GJ_TLVDeal_
			debugstring("Temp Deal Black 3 ");
#endif
		for(uiT=0; uiT<(16); uiT++){
			flashwrite(addr, blkTempbuff+(uiT*256), 256);
			addr += 256;
		}
		pFistVary.BlackListNum = BlackLength;
		sysfewrite(BIT_BLACK_2BLOK, 32, (unsigned char*)&gBlackBlockNum[0]);
		sysfewrite(BIT_BLACK_SUM,4,(unsigned char*)&pFistVary.BlackListNum); //处理完一个包，保存黑名单条数
		
	}
#ifdef debug_GJ_TLVDeal_
	debugstring("Temp Deal Black 4 ");
#endif
	return ST_OK;
}
//处理接收到的黑名单，全量下载直接放到黑名单区，增减量放到临时区
void DealGPRSBlack(unsigned char len, unsigned char *dat)
{
	unsigned int addr;
	unsigned int i,itemp;
	unsigned char buff[20];
	unsigned char buffT[20];
	unsigned char cardNo[20];

	if((gBusblkDownPro.Miss_style == 1)||(gBusblkDownPro.Miss_style == 2)){//全量下载 直接写入
#ifdef _debug_gprs
		debugstring("deal GPS black  ALLL\r\n");
#endif
		sysferead(BIT_BLACK_SUM,4,(unsigned char*)&pFistVary.BlackListNum); //黑名单条数
		sysferead(BIT_BLACK_2BLOK, 32, (unsigned char*)&gBlackBlockNum[0]);
		for(i=0;i<16;i++)
		{
			if(gBlackBlockNum[i] > 1000)
				gBlackBlockNum[i] = 0;
		}
		BlackLength = pFistVary.BlackListNum;
		itemp = BlackLength/1000; //到了哪一块了

		if(itemp>=16)
			return;

		if(itemp == 0)
			addr = FLASH_BLK_START+14+(itemp*4096)+(gBlackBlockNum[itemp]*BLK_SNO_LEN); // 需要写入黑名单的地址 前14字节是版本号
		else
			addr = FLASH_BLK_START+(itemp*4096)+(gBlackBlockNum[itemp]*BLK_SNO_LEN); // 需要写入黑名单的地址
		memset(buff, 0, 20);
		memset(buffT, 0xFF, 20);
		for(i=0; i<len; i++){//len 要处理的黑名单条数
			clr_wdt();
			if((memcmp(dat+(i*10), buff, 8) == 0)||(memcmp(dat+(i*10), buffT, 8) == 0)){
				break;	//如果全是0或全是FF，则认为已经没有黑名单了
			}
			else{
				flashwrite(addr, dat+(i*10), BLK_SNO_LEN);//保存黑名单
				BlackLength++;
				pFistVary.BlackListNum=BlackLength;
				itemp = (BlackLength-1)/1000;
				if(itemp < 16)
					gBlackBlockNum[itemp] = (((BlackLength-1)%1000)+1);
				addr+=4;
			}
		}
#ifdef _debug_gprs
		debugdata((unsigned char*)&BlackLength, 4, 1);
		debugdata((unsigned char*)&gBlackBlockNum, 32, 1);
#endif
		sysfewrite(BIT_BLACK_2BLOK, 32, (unsigned char*)&gBlackBlockNum[0]);
		sysfewrite(BIT_BLACK_SUM,4,(unsigned char*)&pFistVary.BlackListNum); //处理完一个包，保存黑名单条数
	}
	else{//增减量下载
		addr = FLASH_TEMP_START;//向FLAHS后面加。
		memset(buffT, 0xff, 20);
		while(addr<FLASH_TEMP_END){
			clr_wdt();
			flashread(addr, buff, 14);
			if(memcmp(buff, buffT, 14) == 0)
				break;	//此处能写入
			addr += 14;
		}
		if(addr >= FLASH_TEMP_END){
			debugstring("写增减量溢出:");
			debugdata((unsigned char*)&addr, 4, 1);
			return;
		}
		for(i=0; i<len; i++){//len 要处理的黑名单条数
			clr_wdt();
			memset(buff, 0, 10);
			if(memcmp(dat+(i*10), buff, 8) == 0){
				sysfewrite(BIT_BLACK_SUM,4,(unsigned char*)&pFistVary.BlackListNum);
				return;	//如果全是0，则认为已经没有黑名单了
			}
#ifdef _debug_gprs
			debugstring("addr:");
			debugdata((unsigned char*)&addr, 4, 1);
#endif
			clr_wdt();
			memcpy(cardNo, dat+(i*10), 10);//每条黑名单10节字组成，最后个字节 0删 1增
			itemp = IsInBlackList(cardNo);
#ifdef _debug_gprs
			debugstring("cardNo:");
			debugdata(cardNo, 10, 1);
			debugdata((unsigned char*)&itemp, 4,1);
#endif
			if(itemp < FLASH_BLK_END){//已经存在黑名单中
				if(cardNo[9] == 1){//减
					buff[13] = 0;
					memcpy(buff, "黑单", 4);
					memcpy(buff+4, (unsigned char*)&itemp, 3);
					buff[7] = 1;
					memcpy(buff+8, cardNo, 5);
					for (itemp=0; itemp<13; itemp++)
					{
						buff[13] ^= buff[itemp];
					}
					flashwrite(addr, buff, 14);//写入
					addr += 14;
#ifdef _debug_gprs
					debugstring("写减buff:");
					debugdata(buff, 14, 1);;
#endif
				}//增加就不理
			}
			else{//不存在黑名单中,只处理增
				if(cardNo[9] == 0){//增
					buff[13] = 0;
					memcpy(buff, "黑单", 4);
					memcpy(buff+4, (unsigned char*)&itemp, 3);
					buff[7] = 0;
					memcpy(buff+8, cardNo, 5);
					for (itemp=0; itemp<13; itemp++)
					{
						buff[13] ^= buff[itemp];
					}
					flashwrite(addr, buff, 14);//写入
					addr += 14;
#ifdef _debug_gprs
					debugstring("写增buff:");
					debugdata(buff, 14, 1);;
#endif
				}//增加就不理
			}
		}
	}
}
#endif //#ifndef NO_GJ_SERVISE 

//extern void MobileCOMDeal(unsigned char val);
#ifndef NO_MOBILE_24G_
//用COM5处理
void G24Card_handler(unsigned char value)
{
	unsigned char ii;
	
	ii = value;
	
	if(g24ComFlag == 0xaa){//读头信息
		switch (g24ComRindex)
		{
		case 0:
			if (ii != 0x02)
				return;
			com1Rlen = 0;
			g24ComRBuf[g24ComRindex++] = ii;
			break;
		case 1:
			com1Rlen = ii*256;
			g24ComRBuf[g24ComRindex++] = ii;
			break;
		case 2:
			com1Rlen += ii;
			g24ComRBuf[g24ComRindex++] = ii;
			break;
		default:
			if(g24ComRindex >= 512){
				g24ComFlag = 0xee;//接收错误
				break;
			}
			g24ComRBuf[g24ComRindex++] = ii;
			if(g24ComRindex >= com1Rlen+5){
				g24ComFlag = 0x55;//接收完成
			}
		}
		return;
	}
	
	// 	if(gmissflag == MISS_COM_PARA){ 
	// 		MobileCOMDeal(ii);//如果是移动下载数据,用移动的处理方式
	// 		return;
	// 	}
}
#elif defined _CPU_24G_
void G24Card_handler(unsigned char value)
{
	unsigned char ii;
	
	ii = value;
	
	if(g24ComFlag == 0xaa){//读头信息
		switch (g24ComRindex)
		{
		case 0:
			if (ii != 0x02)
				return;
			com1Rlen = 0;
			g24ComRBuf[g24ComRindex++] = ii;
			break;
		case 1:
			com1Rlen = ii*256;
			g24ComRBuf[g24ComRindex++] = ii;
			break;
		case 2:
			com1Rlen += ii;
			g24ComRBuf[g24ComRindex++] = ii;
			break;
		default:
			if(g24ComRindex >= 512){
				g24ComFlag = 0xee;//接收错误
				break;
			}
			g24ComRBuf[g24ComRindex++] = ii;
			if(g24ComRindex >= com1Rlen+5){
				g24ComFlag = 0x55;//接收完成
			}
		}
		return;
	}
	
	// 	if(gmissflag == MISS_COM_PARA){ 
	// 		MobileCOMDeal(ii);//如果是移动下载数据,用移动的处理方式
	// 		return;
	// 	}
}
#endif


//一个程序包的大小
#define CMD_G_SIZE 256
//如果是登陆回应，需要把 gBusVerInfo.newbusBLKVer 值置成 1，否则会不停的发登陆信息
unsigned char GJDataDeal(unsigned char *pakege)
{
#ifndef NO_GJ_SERVISE 
	unsigned short ustemp;
	unsigned short CmdCode=0;//指令码
	//	unsigned short GnCode=0;//指令功能码
	unsigned short PallLen=0;//整个接收长度，有时有重包
	unsigned short Alen=0;//此包总长度
	unsigned short Tack=0, proLen=0;
	unsigned short crc16;

	unsigned char buftemp[300];
	unsigned char blktemp[32];
	unsigned char hadware[2];
//	unsigned char Tver[14];//应答的参数版本
	unsigned char *p;
	unsigned short nestpLen;//其它包的长度
	unsigned char *nestpP;//是否有重包，如果有就指向下一包。
	unsigned int index,itemp, i, addr=0;
	unsigned char bbb;//
	stLoadPtable *pprice;//
	int iret;

#ifdef _debug_
	debugstring("GJdeal IF2 CMD:");
#endif
	Sendpincount = 0;

	memset(hadware, 0, 2);
	index = 0;
	p = pakege;
	nestpP = 0;

	memcpy((unsigned char*)&Alen, p+1, 2);//数据域长度
	nestpLen = 0;
	PallLen = Alen;
	p += 3;//前三个字节是 帧类型和帧长度，和数据无关
#ifdef _debug_
	debugdata(p, PallLen, 1);
#endif
	if(memcmp(p+index, "\xBD\xBD\xBD" , 3) != 0)
	{
#ifdef debug_GJ_TLVDeal_
		debugstring("STX Error!!");
		debugdata(p, 10, 1);
#endif
		return 0;
	}
	index += 3;//报文来源地址 4字节

	gGprsResertTime = 0;//有收到数据，清0
//	memcpy((unsigned char *)&, p+index, 4);
	//指令码(CmdCode)
	memcpy((unsigned char*)&CmdCode, p+GCMD_CmdCode, 2);
	over_turn(2, (unsigned char*)&CmdCode);
	//.指令功能码(GnCode)
//	GnCode = p[GCMD_GnCode];
	memcpy((unsigned char*)&Alen, p+GCMD_Ln, 2);//数据域长度
	over_turn(2, (unsigned char*)&Alen);

	crc16 = Getcrc16(p, (Alen+GCMD_Data), 0xFF, 0xFF);
	memcpy((unsigned char*)&proLen, (p+GCMD_Data+Alen), 2);//CRC值

#ifdef debug_GJ_TLVDeal_
	debugstring("crc16:");
	debugdata((unsigned char*)&crc16, 4, 1);
	debugstring("crc16proLen:");
	debugdata((unsigned char*)&proLen, 4, 1);
	debugstring("Alen:");
	debugdata((unsigned char*)&Alen, 2, 1);
	debugstring("CmdCode:");
	debugdata((unsigned char*)&CmdCode, 2, 1);
	debugstring("GnCode:");
	debugdata(p+GCMD_GnCode, 1, 1);
	debugstring("GCMD_Data:");
	debugdata(p+GCMD_Data, Alen+2, 1);
#endif

	if((Alen+21) < PallLen){//还有其它数据
		nestpP = pakege+Alen+21;
		nestpLen = (PallLen +3 - Alen - 21);
#ifdef debug_GJ_TLVDeal_
		debugstring("  还有另一个包::");
		debugdata(nestpP, nestpLen, 1);
#endif 
	}
	
	Tack = crc16;
	over_turn(2, (unsigned char*)&Tack);
	if((crc16 != proLen)&&(Tack != proLen)){
#ifdef _debug_gprs
		debugstring("crc16 ERROR!!!\r\n");
#endif
		if(CmdCode == 0x4002){
			Alen = Build_GJ_DPro(buftemp);
			gprs_send_data(1, Alen, buftemp);//发送应答
		}
		goto GJdataDealEnd;
	}

	gSendGLogin = 0;//有应答，不发心跳包

	switch(CmdCode)
	{
	case 0x1001://终端签到 应答
		memcpy((unsigned char*)&Tack, p+GCMD_Data+7, 2);
		if(Tack == 0x0090){
			if(time_invalid(p+GCMD_Data) == 1){//时间有效，校时
#ifdef _debug_gprs
				debugstring("时间有效\r\n");
#endif
				GetDateTime();
				if(memcmp((unsigned char*)&SysTime.year_h, p+GCMD_Data, 5) != 0){
#ifdef _debug_gprs
					debugstring("校准 \r\n");
#endif
					timewrite(p+GCMD_Data);
				}
			}
			memcpy(gBusVerInfo.newbusBLKVer, "\x00\x01", 2);//把收到的黑名单标志置为1,判断是否需要重新发登陆命令
			gmissflag = 0;
#ifdef _debug_gprs
			debugstring("签到成功\r\n");
#endif
#ifdef Center_Ctrl_BU_ENABEL
			BUCtrl_Save_Flag(*(p+GCMD_CmdPara));//P1是禁止标识
#endif
		}
		else
			goto GJdataDealEnd;
		if(GPRSLinkProcess == GPRS_SENDING_CMD)
			GPRSLinkProcess = TCPSTARTSTAT;
		break;//登陆失败，不处理，重新发登陆命令
	case 0x1102://终端签退
		memcpy((unsigned char*)&Tack, p+GCMD_Data, 2);
		if(Tack == 0x0090){
			gmissflag = 0;
			stopCCgprs(ERROR_BUS_CONNECT);//签退成功,长时间延时后再重新登陆
		}
		gmissflag = 0;
		if(GPRSLinkProcess == GPRS_SENDING_CMD)
			GPRSLinkProcess = TCPSTARTSTAT;
		break;//登陆失败，不处理，重新发登陆命令
	case 0x2001://接收机监测
		if(time_invalid(p+GCMD_Data) == 1){//时间有效，校时
			GetDateTime();
			if(memcmp((unsigned char*)&SysTime.year_h, p+GCMD_Data, 5) != 0)
				timewrite(p+GCMD_Data);
			
#ifdef Center_Ctrl_BU_ENABEL
			BUCtrl_Save_Flag(*(p+GCMD_CmdPara));//P1是禁止标识
#endif
		}
		i = Build_GJ_LOGIN(buftemp, (0x10|p[GCMD_GnCode]));
		gprs_send_data(1, i, buftemp);//发送应答
		if(gmissflag == 0x4C){//如果车载机正在发送通知，认为OK
			gmissflag = MISS_G_FREE;//置空闲，
		}
		goto GJdataDealEnd;
//		break;
	case 0x3101://数据单包传输
		break;
	case 0x3002://数据多包传输
		break;
	case 0x3003://采集历史记录//index,itemp, i, addr=0;
		addr = 0;	//起始地址
		index = 0;	//记录总条数
		Get_SerialNum(buftemp);
#ifdef debug_GJ_TLVDeal_
		debugstring("snd 3003:1:");
		debugdata(p, Alen, 1);
		debugdata(buftemp, 4, 1);
		debugdata(p+GCMD_Data+6, 4, 1);
#endif
		if(memcmp(buftemp, p+GCMD_Data+6, 4) != 0){//唯一号不一致
			memcpy(blktemp, p+GCMD_Data, 6);//发来的命令商户编号 2byte (BCD)+汽车编号 4Byte (BCD)+设备序号4Byte (HEX)+起始日期 4Byte (BCD)+ 终止日期 4Byte (BCD)
			memcpy(blktemp+6, buftemp, 4);
			memset(blktemp+10, 0, 12);
			memcpy(blktemp+22, "\x20\x00", 2);
			goto GJDEAL3003;
		}
		
		ASCII2ZBcod((char*)pFistVary.DeviceNo, blktemp, 4);
//		Ascii2BCD(pFistVary.DeviceNo, blktemp, 8);
		if(memcmp(blktemp, p+GCMD_Data+2, 4) != 0){//设备号不一致
			memcpy(blktemp, p+GCMD_Data, 6);//发来的命令商户编号 2byte (BCD)+汽车编号 4Byte (BCD)+设备序号4Byte (HEX)+起始日期 4Byte (BCD)+ 终止日期 4Byte (BCD)
			memcpy(blktemp+6, buftemp, 4);
			memset(blktemp+10, 0, 12);
			memcpy(blktemp+22, "\x10\x00", 2);
			goto GJDEAL3003;
		}
 
		iret = getHisAddr(p+GCMD_Data+13, p+GCMD_Data+17, &addr, &itemp);
		if((iret != 0)||(addr == itemp)){//没有记录
			memcpy(blktemp, p+GCMD_Data, 6);
			memcpy(blktemp+6, buftemp, 4);
			memset(blktemp+10, 0, 12);
			memcpy(blktemp+22, "\x30\x00", 2);
			goto GJDEAL3003;
		}
		else{//有记录
			i = ((itemp+FLASH2_BAKR_LEN-addr)%FLASH2_BAKR_LEN);
			i /= RECORD_LEN;
#ifdef _debug_
			debugstring("w hisrec 1:");
			debugdata((unsigned char*)&addr, 4, 1);
			debugdata((unsigned char*)&itemp, 4, 1);
			debugdata((unsigned char*)&i, 4, 1);
#endif
			memcpy(blktemp, (unsigned char*)&addr, 4);//先把要传的数据写入铁电
			memcpy(blktemp+4, (unsigned char*)&i, 4);
			memcpy(blktemp+8, (unsigned char*)&SysTime, 7);
			blktemp[15] = 0;
			index = __cpuCrc32(blktemp, 16);
			memcpy(blktemp+16, (unsigned char*)&index, 4);
#ifdef _debug_
			debugstring("w hisrec 3003:");
			debugdata(blktemp, 20, 1);
#endif
			sysfewrite(BIT_HISREC_SND, 20, blktemp);
			

			memcpy(blktemp, p+GCMD_Data, 6);
			memcpy(blktemp+6, buftemp, 4);
			memcpy(blktemp+10, (unsigned char*)&addr, 4);
			memcpy(blktemp+14, (unsigned char*)&itemp, 4);
			memcpy(blktemp+18, (unsigned char*)&i, 4);
			memcpy(blktemp+22, "\x90\x00", 2);
		}
		
GJDEAL3003:
		i = Build_GJ_His_ACHO3003(buftemp, blktemp);
#ifdef debug_GJ_TLVDeal_
		debugstring("snd 3003::");
		debugdata(buftemp, i, 1);
#endif
		gprs_send_data(1, i, buftemp);//发送应答
		break;
	case 0x4001://黑名单下传
#ifdef debug_GJ_TLVDeal_
		debugstring("黑名单下传:");
		debugdata(p+GCMD_GnCode, 1, 1);
#endif

		if(memcmp(gBusblkDownPro.Miss_cmdCode, "\x40\x05", 2) == 0){//正在下载资费
#ifdef debug_GJ_TLVDeal_
			debugstring("正在下载资费\r\n");
#endif
			break;
		}
// 		if(gBusDownPro.MISS_flag == 0xEA){//正在更新程序，暂停下黑名单
// #ifdef debug_GJ_TLVDeal_
// 			debugstring("下载程序中，黑名单停\r\n");
// #endif
// 			break;
// 		}
		
		gBusblkDownPro.Miss_time = GPRS_Login_dly;
		if((p[GCMD_GnCode] == 1)||(p[GCMD_GnCode] == 3)){//包头
			memcpy((unsigned char*)&crc16, p+GCMD_Ln, 2);
			over_turn(2, (unsigned char*)&crc16);
#ifdef debug_GJ_TLVDeal_
			debugstring("黑名单下传2:");
			debugdata((unsigned char*)&crc16, 2, 1);
#endif
			if(crc16 != 0x0A){
				debugstring("黑名单头长度错误\r\n");
				return 0;
			}
			over_turn(2, gBusblkDownPro.Miss_ver);
			memcpy(gBusblkDownPro.Miss_cmdCode, "\x40\x01", 2);
			gBusblkDownPro.Miss_style = p[GCMD_GnCode];

			if(gBusblkDownPro.Miss_PPack >= gBusblkDownPro.MISS_MAXPack) //当前包错误，重新下载黑名单
				gBusblkDownPro.Miss_PPack = 0;
			if((gBusblkDownPro.Miss_PPack > 0) && (memcmp(gBusblkDownPro.Miss_ver, p+GCMD_Data+4, 2) != 0))//版本不正确，重新下载黑名单
				gBusblkDownPro.Miss_PPack = 0;

			memcpy(blktemp, p+GCMD_Data, 4);
			over_turn(4, blktemp);
			memcpy((unsigned char*)&gBusblkDownPro.MISS_MAXPack, blktemp, 4);

			memcpy(gBusblkDownPro.Miss_ver, p+GCMD_Data+4, 2);
			over_turn(2, gBusblkDownPro.Miss_ver);
			memcpy((unsigned char*)&gBusblkDownPro.Miss_ALL_LEn, p+GCMD_Data+6, 4);
			over_turn(4, (unsigned char*)&gBusblkDownPro.Miss_ALL_LEn);
#ifdef debug_GJ_TLVDeal_
			debugstring("黑名单 包头：");
			debugdata((unsigned char*)&gBusblkDownPro.Miss_style, 1, 1);
			debugdata((unsigned char*)&gBusblkDownPro.Miss_PPack, 2, 1);
			debugdata((unsigned char*)&gBusblkDownPro.MISS_MAXPack, 4, 1);
			debugdata((unsigned char*)&gBusblkDownPro.Miss_ver, 2, 1);
			debugdata((unsigned char*)&gBusblkDownPro.Miss_ALL_LEn, 4, 1);
#endif
			if((gBusblkDownPro.Miss_ALL_LEn == 0)||(gBusblkDownPro.MISS_MAXPack == 0)||
				(memcmp(gBusVerInfo.busBLKVer, gBusblkDownPro.Miss_ver, 2) == 0))//如果下的版本号和车载机已经有的是一样的，不下载
			{
				memset((unsigned char*)&gBusblkDownPro, 0, sizeof(stbusdownPara));
				sysfewrite(BIT_BUSblkDownPRO_INFO, sizeof(stbusdownPara), (unsigned char*)&gBusblkDownPro);//保存任务参数
				goto BLKSENDECHO;
			}

			if(p[GCMD_GnCode] == 1){//如果是全量下载的头，先清除黑名单
				if(gBusblkDownPro.Miss_PPack == 0){
					delallBlacInbus();//清除机内黑名单
					memcpy(gBusVerInfo.busBLKVer, "\x00\x01", 2);
					restore_flag = 3;
				}
				addr = FLASH_TEMP_START;
				flasherase(ERASE64KBYTESECTOR, addr);//清除临时区
			}
			else{//是增减量下载，清除临时区
				restore_flag = 3;
				addr = FLASH_TEMP_START;
				flasherase(ERASE64KBYTESECTOR, addr);//清除临时区
			}
			sysfewrite(BIT_BUSblkDownPRO_INFO, sizeof(stbusdownPara), (unsigned char*)&gBusblkDownPro);//保存任务参数
		}
		else if((p[GCMD_GnCode] == 2)||(p[GCMD_GnCode] == 4)){//包体
			if(Alen < 10)//数据长度不正确，不处理
				break;
			memcpy(gBusblkDownPro.Miss_cmdCode, "\x40\x01", 2);
			gBusblkDownPro.Miss_style = p[GCMD_GnCode];

			memcpy((unsigned char*)&proLen, p+GCMD_CmdPara, 2);//包序号
			over_turn(2, (unsigned char*)&proLen);

			DealGPRSBlack(25, p+GCMD_Data); // 处理黑名单函数

			gBusblkDownPro.Miss_PPack = proLen;			//更新下载参数,更新包号

#ifdef debug_GJ_TLVDeal_
			debugstring("BlackLength:");
			debugdata((unsigned char*)&BlackLength, 2, 1);
#endif
			sysfewrite(BIT_BUSblkDownPRO_INFO, sizeof(stbusdownPara), (unsigned char*)&gBusblkDownPro);//保存任务参数
			
#ifdef debug_GJ_TLVDeal_
			debugstring("黑名单@@@：");
			debugdata((unsigned char*)&gBusblkDownPro.Miss_style, 1, 1);
			debugdata((unsigned char*)&gBusblkDownPro.Miss_PPack, 2, 1);
			debugdata((unsigned char*)&gBusblkDownPro.MISS_MAXPack, 4, 1);
			debugdata((unsigned char*)&gBusblkDownPro.Miss_ver, 2, 1);
			debugdata((unsigned char*)&gBusblkDownPro.Miss_ALL_LEn, 4, 1);
			debugdata((unsigned char*)&proLen, 2, 1);
#endif
			sprintf((char*)buftemp, "B:%d%% ",  ((proLen*100)/gBusblkDownPro.MISS_MAXPack));
			//display(5, 0, (char*)buftemp, DIS_CENTER);
			miniDispstr(6, 7, (char*)buftemp, 0);

			if(proLen == gBusblkDownPro.MISS_MAXPack)//下载完成
			{
#ifdef debug_GJ_TLVDeal_
				debugstring("黑名单下完:");
				debugdata(gBusVerInfo.busBLKVer, 2, 1);
#endif
				if((gBusblkDownPro.Miss_style == 1)||(gBusblkDownPro.Miss_style == 2)){//是全量下载
#ifdef debug_GJ_TLVDeal_
					debugstring("黑名单全量下载1");
					debugdata((unsigned char*)&BlackLength, 2, 1);
#endif
					if((gBusblkDownPro.Miss_ALL_LEn == 0)||(gBusblkDownPro.MISS_MAXPack == 0)||
						(BlackLength+20 < (gBusblkDownPro.Miss_ALL_LEn/10))){//如果是全量下载，黑名单条数不对就重新下载
#ifdef debug_GJ_TLVDeal_
						debugstring("黑名单增下载错误2\r\n");
#endif
						memset(gBusVerInfo.busBLKVer, 0, 2);
						BlackLength = 0;
						memset((unsigned char*)&gBusblkDownPro, 0, sizeof(stbusdownPara));
						saveBlackInfo();
// 						if(GPRSLinkProcess == GPRS_SENDING_CMD)
// 							GPRSLinkProcess = TCPSTARTSTAT;
						goto BLKSENDECHO;
					}
				}
				else{//是增量下载,要处理增量黑名单
#ifdef debug_GJ_TLVDeal_
					debugstring("黑名单增下载");
#endif
					TempDealBlack();
				}
#ifdef debug_GJ_TLVDeal_
				memset(buftemp, 0, 64);
				memcpy(buftemp,gBusblkDownPro.Miss_ver, 2);
				memcpy(buftemp, (unsigned char*)&BlackLength, 4);
#endif
				memcpy(gBusVerInfo.busBLKVer, gBusblkDownPro.Miss_ver, 2);//新黑名单版本号替现在的。
				memcpy(gBusVerInfo.newbusBLKVer, gBusblkDownPro.Miss_ver, 2);//新黑名单版本号替现在的。
				memset(gBusVerInfo.newbusBLKVer, 0, 2);//重新登陆
				gmissflag = MISS_G_FREE;
				gmissOldflag = 0;
				restore_flag = 3;//刷新界面
				memset((unsigned char*)&gBusblkDownPro, 0, sizeof(stbusdownPara));
				saveBlackInfo();  //只更新版本号
#ifdef debug_GJ_TLVDeal_
				debugstring("黑名单增下载完成:");
				debugstring((char*)buftemp);
				debugstring("\r\n");
#endif
			}
		}
BLKSENDECHO:
#ifdef debug_GJ_TLVDeal_
		debugstring("黑名单###：");
		debugdata((unsigned char*)&gBusblkDownPro.Miss_style, 1, 1);
		debugdata((unsigned char*)&gBusblkDownPro.Miss_PPack, 2, 1);
		debugdata((unsigned char*)&gBusblkDownPro.MISS_MAXPack, 4, 1);
		debugdata((unsigned char*)&gBusblkDownPro.Miss_ver, 2, 1);
		debugdata((unsigned char*)&gBusblkDownPro.Miss_ALL_LEn, 4, 1);
		debugdata((unsigned char*)&proLen, 2, 1);
#endif
		gBusblkDownPro.Miss_style = p[GCMD_GnCode]; //回应需要发这个。
		sysfewrite(BIT_BUSblkDownPRO_INFO, sizeof(stbusdownPara), (unsigned char*)&gBusblkDownPro);//保存任务参数
		Alen = Buid_GJ_DBLK(buftemp);
#ifdef debug_GJ_TLVDeal_
		debugstring("黑名单ACHO:");
		debugdata(buftemp, Alen, 1);
#endif
		gprs_send_data(1, Alen, buftemp);//发送应答
		break;
	case 0x4002://车载程序下载
		if(memcmp(gBusblkDownPro.Miss_cmdCode, "\x40\x05", 2) == 0){//正在下载资费
			break;
		}

		if(p[GCMD_GnCode] == 0x01){//包头
#ifdef debug_GJ_TLVDeal_
			debugstring("包头:");
			debugdata((unsigned char*)&gBusDownPro, sizeof(stbusdownpro), 1);
#endif
			if(gBusDownPro.MISS_flag == 0xEA){//当前下载数据有效
				memcpy(blktemp, gBusDownPro.MISS_MAXPack, 4);
				over_turn(4, blktemp);
				memcpy(blktemp+4, gBusDownPro.Miss_ver, 2);
				over_turn(2, blktemp+4);
				memcpy(blktemp+6, gBusDownPro.Miss_ALL_LEn, 4);
				over_turn(4, blktemp+6);
				
#ifdef debug_GJ_TLVDeal_
				debugdata(p+GCMD_Data, 10, 1);
				debugstring("blktemp:");
				debugdata(blktemp, 10, 1);
#endif
//				if((memcmp(blktemp, p+GCMD_Data, 4)!=0)||(memcmp(blktemp+4, p+GCMD_Data+4, 2)!=0)||(memcmp(blktemp+6, p+GCMD_Data+6, 4)!=0))//最大包号、版本号、总字节数不一致，从头下载否则续传
				if(memcmp(blktemp, p+GCMD_Data, 10) != 0)
				{
					memcpy(gBusDownPro.MISS_MAXPack, p+GCMD_Data, 4);
					over_turn(4, gBusDownPro.MISS_MAXPack);
					memcpy(gBusDownPro.Miss_ver, p+GCMD_Data+4, 2);
					over_turn(2, gBusDownPro.Miss_ver);
					memcpy(gBusDownPro.Miss_ALL_LEn, p+GCMD_Data+6, 4);
					over_turn(4, gBusDownPro.Miss_ALL_LEn);
					memcpy(gBusDownPro.Miss_StartData, p+GCMD_Data+10, 7);
					gBusDownPro.MISS_flag = 0xEA;
					gBusDownPro.Miss_style = p[GCMD_GnCode];
					memcpy(gBusDownPro.Miss_PPack, "\x00\x00", 2);
				}
			}
			else{
				memcpy(gBusDownPro.MISS_MAXPack, p+GCMD_Data, 4);
				over_turn(4, gBusDownPro.MISS_MAXPack);
				memcpy(gBusDownPro.Miss_ver, p+GCMD_Data+4, 2);
				over_turn(2, gBusDownPro.Miss_ver);
				memcpy(gBusDownPro.Miss_ALL_LEn, p+GCMD_Data+6, 4);
				over_turn(4, gBusDownPro.Miss_ALL_LEn);
				memcpy(gBusDownPro.Miss_StartData, p+GCMD_Data+10, 7);
				gBusDownPro.MISS_flag = 0xEA;
				gBusDownPro.Miss_style = p[GCMD_GnCode];
				memcpy(gBusDownPro.Miss_PPack, "\x00\x00", 2);
			}
#ifdef debug_GJ_TLVDeal_
			debugstring("MISS_MAXPack:");
			debugdata(gBusDownPro.MISS_MAXPack, 4, 1);
			debugstring("MISS_MAXPack:");
			debugdata(gBusDownPro.Miss_ver, 2, 1);
			debugstring("Miss_ALL_LEn:");
			debugdata(gBusDownPro.Miss_ALL_LEn, 4, 1);
			debugstring("Miss_PPack:");
			debugdata(gBusDownPro.Miss_PPack, 2, 1);
#endif
			memcpy((unsigned char*)&i, gBusDownPro.MISS_MAXPack, 4);
			itemp = 0;
			memcpy((unsigned char*)&itemp, gBusDownPro.Miss_PPack, 2);
			
			if((i == itemp)&&(itemp > 0)){//已经下载完成了
				goto DoenPEND;
			}
		}
		else if(p[GCMD_GnCode] == 0x02){//包体
			if(Alen < 10)//数据长度不正确，不处理
				break;
			memcpy((unsigned char *)&Tack, p+GCMD_CmdPara, 2);//包序号
			over_turn(2, (unsigned char *)&Tack);
			memcpy((unsigned char *)&crc16, p+GCMD_CmdPara+2, 2);//程序版本号
			over_turn(2, (unsigned char *)&crc16);
			if(memcmp((unsigned char*)&crc16, gBusDownPro.Miss_ver, 2) != 0){//下载的版本号不一致
#ifdef debug_GJ_TLVDeal_
				debugstring("下载和正在下载版本不一致\r\n");
#endif
				memset((unsigned char*)&gBusDownPro, 0, sizeof(stbusdownpro));//重新传
				GPRSLinkProcess = GPRS_NEED_CLOSEIP;
				goto PROSENDECHO;
			}
			if (Tack == 1)//第一个包
			{
				addr = FLASH_PRO_START;//写入的地址 程序起始地址
				for(i=0; i<4; i++){
#ifdef debug_GJ_TLVDeal_
					debugstring("清空存贮器:");
					debugdata((unsigned char*)&addr, 4, 1);
#endif
					flasherase(ERASE64KBYTESECTOR, addr);//先擦除 256K
					addr += 0x10000;//下一个64K的块
				}
			}
			memcpy((unsigned char*)&Alen, gBusDownPro.Miss_PPack, 2);//上一次更新的包序号
			if(Tack > (Alen+1)){//跳包了
				memcpy(gBusDownPro.Miss_PPack, "\x00\x00", 2);//重新传
#ifdef debug_GJ_TLVDeal_
				debugstring("重新传:");
				debugdata((unsigned char*)&Tack, 2, 1);
				debugdata((unsigned char*)&Alen, 2, 1);
#endif
#ifdef GPRS_MG323_
				initTCPIPnode();
#endif
//				memset((unsigned char*)&gBusDownPro.MISS_flag, 0, sizeof(stbusdownpro));
				goto PROSENDECHO;
			}
			addr = (FLASH_PRO_START + ((Tack-1) * CMD_G_SIZE));//写入的地址，包序号乘以一个包下载的大小
			//需要校验CRC16
			memcpy((unsigned char*)&itemp, gBusDownPro.MISS_MAXPack, 4);
// 			debugstring("Tack:");
// 			debugdata((unsigned char*)&Tack, 2, 1);
// 			debugstring("itemp:");
// 			debugdata((unsigned char*)&itemp, 4, 1);
			sprintf((char*)buftemp, "P:%d%% ",  ((Tack*100)/itemp));
			//display(5, 0, (char*)buftemp, DIS_CENTER);
			miniDispstr(6, 7, (char*)buftemp, 0);
			
			if(addr > FLASH_PRO_END){//地址出错，此包不写入，重新申请此包数据
				restore_flag = 1;
				goto PROSENDECHO;
			}
			else{
				memcpy((unsigned char *)&Alen, p+GCMD_Ln, 2);
				over_turn(2, (unsigned char*)&Alen);

				
#ifdef debug_GJ_TLVDeal_
				debugstring("MISS_MAXPack:");
				debugdata(gBusDownPro.MISS_MAXPack, 4, 1);
				debugstring("MISS_MAXPack:");
				debugdata(gBusDownPro.Miss_ver, 2, 1);
				debugstring("Miss_ALL_LEn:");
				debugdata(gBusDownPro.Miss_ALL_LEn, 4, 1);
				debugstring("Miss_PPack:");
				debugdata(gBusDownPro.Miss_PPack, 2, 1);
#endif

#ifdef debug_GJ_TLVDeal_
				debugstring("addr:");
				debugdata((unsigned char*)&addr, 4, 1);
				debugstring("Alen:");
				debugdata((unsigned char*)&Alen, 2, 1);
#endif
				for(bbb =0;bbb<3;bbb++){
					flashwrite(addr, p+GCMD_Data, CMD_G_SIZE);//写入
					flashread(addr, buftemp, CMD_G_SIZE);
					if(memcmp(p+GCMD_Data, buftemp, CMD_G_SIZE) == 0){
						break;
					}
					else{
#ifdef debug_GJ_TLVDeal_
						debugstring("数据写入错误\r\n");
						debugdata(p+GCMD_Data, proLen, 1);
						debugdata(buftemp, proLen, 1);
#endif
						continue;
					}
				}

				if(bbb >=3){
#ifdef debug_GJ_TLVDeal_
					debugstring("从头开始\r\n");
#endif
					addr = FLASH_PRO_START;//写入的地址 程序起始地址
					
					memset((unsigned char*)&gBusDownPro, 0, sizeof(stbusdownpro));
#ifdef GPRS_MG323_
					initTCPIPnode();
#endif
					goto PROSENDECHO;//写入错误，全部重新下载
				}
		
				memcpy((unsigned char*)&Alen, gBusDownPro.MISS_MAXPack, 4);//上一次更新的包序号
				if(Tack == Alen)//下载完成
				{
DoenPEND:
#ifdef debug_GJ_TLVDeal_
					debugstring("任务类型:");
					debugdata((unsigned char*)&gBusDownPro.Miss_style, 1, 1);
#endif
					memset(buftemp, 0, 64);
					memcpy(buftemp,"程序需要更新", 12);
					memcpy(buftemp+12,gBusDownPro.Miss_ver, 2);
					memcpy(buftemp+14, gBusDownPro.Miss_StartData, 7);
#ifdef debug_GJ_TLVDeal_
					debugstring("程序下载完成:");
					debugdata(buftemp, 21, 1);
#endif
					display(3,0,"完成,请重启...",DIS_CENTER);
					addr = FLASH_PRO_FLAG;
					
#ifdef debug_GJ_TLVDeal_
					debugstring("程序下载完成2:");
					debugdata(buftemp, 34, 1);
#endif
					flashwrite(addr, buftemp, 64);
					
					memcpy(gBusDownPro.Miss_PPack, p+GCMD_CmdPara, 2);//记录当前正在更新的包序列号
					over_turn(2, gBusDownPro.Miss_PPack);
					
					gBusDownPro.Miss_style = p[GCMD_GnCode];//这个在应答要用，所以这个必须是最新的。
					Alen = Build_GJ_DPro(buftemp);
					gprs_send_data(1, Alen, buftemp);//发送应答
#ifdef GPRS_MG323_
					while(1){
						clr_wdt();
						if(gGPRS_data_style.ucSendstyle == GPRS_SISW){
#ifdef _debug_gprs
							debugstring("数据:");
							debugdata((unsigned char*)&gGPRS_data_style.uiNum, 4, 1);
#endif
							//		if(memcmp(gMC55iSendbuff, "数据有效", 8)==0){//不管是否有效，都应该发
							//			memcpy((unsigned char*)&i, gMC55iSendbuff+8, 4);
							gprs_DATA_snd(gGPRS_data_style.uiNum, gMC55iSendbuff+12);//串口中断中不能发送数据
							memset(gMC55iSendbuff, 0, 50);
							//		}
							
							gGPRS_data_style.ucSendstyle = 0;
							gGPRS_data_style.uiNum = 0;
							break;
						}
					}
#endif
					
#ifdef FengDuan_BU_
					FDParChange_QM(FDParChange_Program);
#endif
					
					memset((unsigned char*)&gBusDownPro, 0, sizeof(stbusdownpro));
					sysfewrite(BIT_BUSDownPRO_INFO, 27, (unsigned char*)&gBusDownPro);//保存任务参数

					beep(1,10,10);
					delayxms(400);
					ReStartCPU();//下载完成，重启设备以便更新程序。
					while(1);
				}
			}
			memcpy(gBusDownPro.Miss_PPack, p+GCMD_CmdPara, 2);//记录当前正在更新的包序列号
			over_turn(2, gBusDownPro.Miss_PPack);
		}
PROSENDECHO:
		gBusDownPro.Miss_Time = GPRS_Login_dly;//如果过了40秒，没有再收到程序下载信息，则重新登陆
		gBusDownPro.Miss_style = p[GCMD_GnCode];//这个在应答要用，所以这个必须是最新的。
		sysfewrite(BIT_BUSDownPRO_INFO, 27, (unsigned char*)&gBusDownPro);//

		Alen = Build_GJ_DPro(buftemp);
		gprs_send_data(1, Alen, buftemp);//发送应答
		break;
	case 0x4005://资费信息传输
		if(p[GCMD_GnCode] != 3){//是全量，一个包
			break;	
		}
		pprice = (stLoadPtable*)(p+GCMD_Data);
		
#ifdef BUS_SERRIER_No
		memset(blktemp, 0, 4);
		getposSN((unsigned char*)buftemp, 0);//取POS机终端编号
		Ascii2BCD(buftemp+10, blktemp+1, 6);//Address:终端机编号
#else 
		Get_SerialNum(blktemp);
//Ascii2BCD(pFistVary.DeviceNo, blktemp, 8);
#endif
		
		if(memcmp(blktemp, pprice->SerialNo, 4) != 0){//设备序列号不一致
#ifdef debug_GJ_TLVDeal_
			debugstring("设备序列号不一致：");
			debugdata(blktemp, 4, 1);
			debugdata(pprice->SerialNo, 4, 1);
#endif
			break;//不发应答
		}

		SYSgetbussinessNO(blktemp);
		if(memcmp(blktemp, pprice->bussiness, 2) != 0){//商户编号不一致
#ifdef debug_GJ_TLVDeal_
			debugstring("商户编号不一致：");
			debugdata(blktemp, 4, 1);
			debugdata(pprice->deviceNo, 4, 1);
#endif
			break;//不发应答
		}
		
		addr = FLASH_TEMP_PRICE;
		flasherase(ERASE4KBYTESECTOR, addr);	//先保存下载下来的线路信息  checkNewPrice函数中处理信息
		for(i=0; i<3; i++){
			flashwrite(addr, p+GCMD_Data, Alen);
			flashread(addr, buftemp, Alen);
			if(memcmp(buftemp, p+GCMD_Data, Alen) == 0)
				break;
		}
		if(i >= 3){
			debugstring(" 下载线路信息写入错误!\r\n");
		}
		
		Alen = Build_GJ_LINE_ACHO(buftemp, p[GCMD_GnCode], pprice);
		gprs_send_data(1, Alen, buftemp);//发送应答
		
#ifdef FengDuan_BU_
		FDParChange_QM(FDParChange_Piaojia);
#endif
		checkNewPrice(); //检查是否要立刻需要更新
		memset(gBusVerInfo.newbusBLKVer, 0, 2);//重新登陆
		gmissflag = MISS_G_FREE;
		gmissOldflag = 0;
		restore_flag = 3;
		break;
	case 0x4004://报站限速信息传输
		if(p[GCMD_GnCode] == 0x01){//包头
#ifdef debug_GJ_TLVDeal_
			debugstring("报站限速包头:");
			debugdata((unsigned char*)&gGPSDownLine, sizeof(stGPSdownLine), 1);
#endif
			if(gGPSDownLine.MISS_flag == 0xEA){//当前下载数据有效
				memcpy(blktemp, gGPSDownLine.MISS_MAXPack, 4);
				over_turn(4, blktemp);
				memcpy(blktemp+4, gGPSDownLine.Miss_ver, 2);
				over_turn(2, blktemp+4);
				memcpy(blktemp+6, gGPSDownLine.Miss_Line, 2);
				over_turn(2, blktemp+6);
				
#ifdef debug_GJ_TLVDeal_
				debugdata(p+GCMD_Data, 10, 1);
				debugstring("blktemp:");
				debugdata(blktemp, 10, 1);
#endif
				if((memcmp(blktemp, p+GCMD_Data, 4)!=0)||(memcmp(blktemp+4, p+GCMD_Data+4, 2)!=0))//最大包号、版本号
				{
#ifdef debug_GJ_TLVDeal_
					debugstring("Restart!!\r\n");
#endif
					memcpy(gGPSDownLine.MISS_MAXPack, p+GCMD_Data, 4);
					over_turn(4, gGPSDownLine.MISS_MAXPack);
					memcpy(gGPSDownLine.Miss_ver, p+GCMD_Data+4, 2);
					over_turn(2, gGPSDownLine.Miss_ver);
					memcpy(gGPSDownLine.Miss_Line, p+GCMD_Data+8, 2);//线路编号取后2字节
					gGPSDownLine.MISS_flag = 0xEA;
					gGPSDownLine.Miss_style = p[GCMD_GnCode];
					memcpy(gGPSDownLine.Miss_PPack, "\x00\x00", 2);
				}
			}
			else{
#ifdef debug_GJ_TLVDeal_
				debugstring("Continue!!\r\n");
#endif
				memcpy(gGPSDownLine.MISS_MAXPack, p+GCMD_Data, 4);
				over_turn(4, gGPSDownLine.MISS_MAXPack);
				memcpy(gGPSDownLine.Miss_ver, p+GCMD_Data+4, 2);
				over_turn(2, gGPSDownLine.Miss_ver);
				memcpy(gGPSDownLine.Miss_Line, p+GCMD_Data+8, 2);//线路编号取后2字节
				gGPSDownLine.MISS_flag = 0xEA;
				gGPSDownLine.Miss_style = p[GCMD_GnCode];
				memcpy(gGPSDownLine.Miss_PPack, "\x00\x00", 2);
			}
#ifdef debug_GJ_TLVDeal_
			debugstring("MISS_MAXPack:");
			debugdata(gGPSDownLine.MISS_MAXPack, 4, 1);
			debugstring("MISS_MAXPack:");
			debugdata(gGPSDownLine.Miss_ver, 2, 1);
			debugstring("Miss_Line:");
			debugdata(gGPSDownLine.Miss_Line, 2, 1);
			debugstring("Miss_PPack:");
			debugdata(gGPSDownLine.Miss_PPack, 2, 1);
#endif
		}
		else if(p[GCMD_GnCode] == 0x02){//包体
			if(Alen < 10)//数据长度不正确，不处理
				break;
			memcpy((unsigned char *)&Tack, p+GCMD_CmdPara, 2);//包序号
			over_turn(2, (unsigned char *)&Tack);
			memcpy((unsigned char *)&crc16, p+GCMD_CmdPara+2, 2);//版本号
			over_turn(2, (unsigned char *)&crc16);
			if(memcmp((unsigned char*)&crc16, gGPSDownLine.Miss_ver, 2) != 0){//下载的版本号不一致
#ifdef debug_GJ_TLVDeal_
				debugstring("报站限速版本不一致\r\n");
#endif
				memset((unsigned char*)&gGPSDownLine, 0, sizeof(stGPSdownLine));//重新传
				GPRSLinkProcess = GPRS_NEED_CLOSEIP;
				goto PROSENDECHO2;
			}
			if (Tack == 1)//第一个包
			{
				addr = FLASH_GPS_LINE_START;//写入的地址 程序起始地址
#ifdef debug_GJ_TLVDeal_
				debugstring("清空存贮器:");
				debugdata((unsigned char*)&addr, 4, 1);
#endif
				flasherase(ERASE64KBYTESECTOR, addr);//先擦除 64K
				addr += 0x10000;//下一个64K的块
				Delay(2);
				flasherase(ERASE64KBYTESECTOR, addr);//先擦除 64K
			}
			memcpy((unsigned char*)&Alen, gGPSDownLine.Miss_PPack, 2);//上一次更新的包序号
			if(Tack < Alen){//正在更新的序号比上次更新的序号小。认为出错
				memcpy(gGPSDownLine.Miss_PPack, "\x00\x00", 2);//重新传
//				memset((unsigned char*)&gGPSDownLine.MISS_flag, 0, sizeof(stbusdownpro));
				goto PROSENDECHO2;
			}
			addr = (FLASH_GPS_LINE_START + ((Tack-1) * CMD_G_SIZE));//写入的地址，包序号乘以一个包下载的大小
			//需要校验CRC16
			memcpy((unsigned char*)&itemp, gGPSDownLine.MISS_MAXPack, 4);
// 			debugstring("Tack:");
// 			debugdata((unsigned char*)&Tack, 2, 1);
// 			debugstring("itemp:");
// 			debugdata((unsigned char*)&itemp, 4, 1);
			sprintf((char*)buftemp, "L:%d%% ",  ((Tack*100)/itemp));
			//display(5, 0, (char*)buftemp, DIS_CENTER);
			miniDispstr(6, 7, (char*)buftemp, 0);
			
			if(addr > FLASH_GPS_LENE_END){//地址出错，此包不写入，重新申请此包数据
				restore_flag = 1;
				goto PROSENDECHO2;
			}
			else{
				memcpy((unsigned char *)&Alen, p+GCMD_Ln, 2);
				over_turn(2, (unsigned char*)&Alen);

				
#ifdef debug_GJ_TLVDeal_
				debugstring("MISS_MAXPack:");
				debugdata(gGPSDownLine.MISS_MAXPack, 4, 1);
				debugstring("MISS_MAXPack:");
				debugdata(gGPSDownLine.Miss_ver, 2, 1);
				debugstring("Miss_Line:");
				debugdata(gGPSDownLine.Miss_Line, 2, 1);
				debugstring("Miss_PPack:");
				debugdata(gGPSDownLine.Miss_PPack, 2, 1);
#endif

#ifdef debug_GJ_TLVDeal_
				debugstring("addr:");
				debugdata((unsigned char*)&addr, 4, 1);
				debugstring("Alen:");
				debugdata((unsigned char*)&Alen, 2, 1);
#endif
				for(bbb =0;bbb<3;bbb++){
					flashwrite(addr, p+GCMD_Data, CMD_G_SIZE);//写入
					flashread(addr, buftemp, CMD_G_SIZE);
					if(memcmp(p+GCMD_Data, buftemp, CMD_G_SIZE) == 0){
						break;
					}
					else{
#ifdef debug_GJ_TLVDeal_
						debugstring("数据写入错误\r\n");
						debugdata(p+GCMD_Data, proLen, 1);
						debugdata(buftemp, proLen, 1);
#endif
						continue;
					}
				}

				if(bbb >=3){
#ifdef debug_GJ_TLVDeal_
					debugstring("从头开始\r\n");
#endif
					addr = FLASH_GPS_LINE_START;//写入的地址 程序起始地址
					
					memset((unsigned char*)&gGPSDownLine, 0, sizeof(stGPSdownLine));
					
					goto PROSENDECHO2;//写入错误，全部重新下载
				}
				
#ifdef FengDuan_BU_
				FDParChange_QM(FDParChange_Line);//因为要重启，设置用处不大。
#endif
				memcpy((unsigned char*)&Alen, gGPSDownLine.MISS_MAXPack, 4);//上一次更新的包序号
				if(Tack == Alen)//下载完成
				{
#ifdef debug_GJ_TLVDeal_
					debugstring("任务类型:");
					debugdata((unsigned char*)&gGPSDownLine.Miss_style, 1, 1);
#endif
#ifdef debug_GJ_TLVDeal_
					debugstring("线路信息下载完成:");
					debugdata((unsigned char*)&gGPSDownLine, 27, 1);
#endif
					
					gGPSDownLine.Miss_style = p[GCMD_GnCode];//这个在应答要用，所以这个必须是最新的。
					Alen = Build_GJ_DLine(buftemp);
					gprs_send_data(1, Alen, buftemp);//发送应答
					
					flashwrite(FLASH_GPS_LENE_VER, gGPSDownLine.Miss_ver, 2);

					memset((unsigned char*)&gGPSDownLine, 0, sizeof(stGPSdownLine));
					sysfewrite(BIT_GPSDownLine_INFO, sizeof(stGPSdownLine), (unsigned char*)&gGPSDownLine);//保存任务参数

					beep(1,10,10);
					delayxms(400);
					ReStartCPU();//下载完成，重启设备以便更新程序。
					while(1);
				}
			}
			memcpy(gGPSDownLine.Miss_PPack, p+GCMD_CmdPara, 2);//记录当前正在更新的包序列号
			over_turn(2, gGPSDownLine.Miss_PPack);
		}
PROSENDECHO2:
		gGPSDownLine.Miss_Time = GPRS_Login_dly;//如果过了40秒，没有再收到程序下载信息，则重新登陆
		gGPSDownLine.Miss_style = p[GCMD_GnCode];//这个在应答要用，所以这个必须是最新的。
		sysfewrite(BIT_GPSDownLine_INFO, sizeof(stGPSdownLine), (unsigned char*)&gGPSDownLine);//

		Alen = Build_GJ_DLine(buftemp);
		gprs_send_data(1, Alen, buftemp);//发送应答
		break;
	case 0x4003://GPS终端程序下载
	case 0x4006://补采集信息传输
		break;
	case 0x5002://报警信息传输
		memcpy((unsigned char*)&Tack, p+GCMD_Data, 2);
		if(Tack != 0x0090){
#ifdef debug_GJ_TLVDeal_
			debugstring("警信息回应失败:");
			sprintf((char*)buftemp, "ACK:%02X%02X\r\n", (Tack>>8), (Tack&0xFF));
			debugstring((char*)buftemp);
#endif
			gmissflag = MISS_G_ALAM;
			goto GJdataDealEnd;
		}
		else{
#ifdef _GPS_DIAODU_
//			galamessage.alamFlag = 0;  不清除，用于给平台上次报警信息
			if(galamessage.style == 0x5A)//正在发送 
				galamessage.style = 0xE0;//完成发送标志
			else
				galamessage.style = 0;
#endif
			gmissflag = MISS_G_FREE;//置空闲，
#ifdef KEY_PAD
			gKeyRestorDis = 3;		
#endif
		}
		if(GPRSLinkProcess == GPRS_SENDING_CMD)
			GPRSLinkProcess = TCPSTARTSTAT;
		break;
	case 0x2002://终端机告知
	case 0x5001://GPS定位数据传输
	case 0x5003://车载机消费数据传输
	case 0x5004://5004没有用,以前用银行传数据，后来合并到公交记录中了
	case 0x5005://沈阳宝石记录回传带有手机号及短信内容
#ifdef debug_GJ_TLVDeal_
		debugstring("Alen 5003|5005:");
		debugdata((unsigned char*)&Alen, 2,1);
		debugdata(p+GCMD_Data+Alen-2, 2,1);
#endif
//		返回1000 为CRC16 不正确
//		返回7007 系统正忙于其他事务
//		返回8008 系统出现异常错误，不能存储数据
//		返回4000 接受到的通讯头不正确

		memcpy((unsigned char*)&Tack, p+GCMD_Data+Alen-2, 2);
		if((Tack == 0x0010)||(Tack == 0x0040)){
			memset((unsigned char*)&tcpipPacket, 0, 5);
			sysfewrite(BIT_TCPIP_PACKET, 5, (unsigned char*)&tcpipPacket);
		}
		else if(Tack != 0x0090){
#ifdef debug_GJ_TLVDeal_
			debugstring("上传记录回应失败:");
			sprintf((char*)buftemp, "ACK:%02X%02X\r\n", (Tack>>8), (Tack&0xFF));
			debugstring((char*)buftemp);
#endif
			if(CmdCode == 0x5001)
				gmissflag = MISS_G_GPS;
			else if((CmdCode == 0x5003)||(CmdCode == 0x5005))
				gmissflag = MISS_G_UREC;
			else
				gmissflag = MISS_G_FREE;//置空闲，
//			stopCCgprs(ERROR_BUS_CONNECT);
			goto GJdataDealEnd;
		}
		else{
			if((CmdCode == 0x5003)){
				memcpy(&ustemp,p+GCMD_CmdPara,2);
				
				if (tcpipPacket.Serial == ustemp)	//包号和刚传输的一致
				{
#ifdef debug_GJ_TLVDeal_
					debugstring("DEL record======\r\n");
#endif
					
					memset((unsigned char*)&tcpipPacket, 0, 5);
					sysfewrite(BIT_TCPIP_PACKET, 5, (unsigned char*)&tcpipPacket);
					ustemp++;
					sysfewrite(FE_PACKSERIAL, 2, (unsigned char*)&ustemp);
				}
			}
			gmissflag = MISS_G_FREE;//置空闲，
		}
		if(GPRSLinkProcess == GPRS_SENDING_CMD)
			GPRSLinkProcess = TCPSTARTSTAT;
		break;
	case 0x7001://汽车编号设置
	case 0x7002://线路编号设置
		break;
	case 0x6001://调度指令接收
#ifdef _GPS_DIAODU_
		memcpy((unsigned char*)&gDiaoDuCMD, p+GCMD_Data, 3);//先取前三位，不然语音会冲掉调度信息。如果是语音文本就不再理更改其它字节
		memcpy(gDiaoDuCMD.Date, (unsigned char*)&SysTime.year_h, 4);
		if((gDiaoDuCMD.ucCMD == 0x10)||(gDiaoDuCMD.ucCMD == 0x11)){//发车信息
			sysferead(BIT_GPSChildLine, 2, hadware);
			memcpy((unsigned char*)&gDiaoDuCMD, p+GCMD_Data, 15);
			if(memcmp(hadware, gDiaoDuCMD.ucZiLine+2, 2)!=0){//如果分线路不一致,重新设置分线路并重新读取调度信息
#ifdef _debug_gprs
				debugstring("分线路不一致:");
				debugdata((unsigned char*)&gDiaoDuCMD.ucZiLine, 4, 1);
				debugdata(hadware, 2, 1);
#endif
				sysfewrite(BIT_GPSChildLine, 2, gDiaoDuCMD.ucZiLine+2);//更新子线路编号
				savesysbupara();
				Load_Line_info();//重新读取调度信息
			}
			gDiaoDuCMD.ucDDstyle = 0x10;
		}
		else if((gDiaoDuCMD.ucCMD == 0x13)||(gDiaoDuCMD.ucCMD == 0x14)){//语音或文字信息,需要另外保存
			memcpy((unsigned char*)&Alen, p+GCMD_Ln, 2);//数据域长度
			over_turn(2, (unsigned char*)&Alen);//先取前三位，不然语音会冲掉调度信息。如果是语音文本就不再理更改其它字节,语音保存在铁电中
			if(Alen > 64)
				Alen = 64;
			p[GCMD_Data+Alen] = 0;//加入结束符
			Alen++;
			sysfewrite(BIT_GPS_MESSAGE, (Alen-3), p+GCMD_Data+3);
			sysfewrite(BIT_GPS_MESSAGE+65, 7, (unsigned char*)&SysTime);//写入日期时间
		}
#ifdef _debug_gprs
		debugstring("调度指令:");
		debugdata((unsigned char*)&gDiaoDuCMD, sizeof(stDiaoDuCMD), 1);
#endif 
		gDiaoDuCMD.ucStartYorN = 0;
		Save_DIAODUCMD_info();
#ifdef KEY_PAD
		gKeyRestorDis = 3;//刷新按键板显示
#endif
		Alen = Build_DIAODU_Echo(buftemp, 0x0160, 0x02, 0, p+GCMD_CmdPara);
		gprs_send_data(1, Alen, buftemp);//发送应答
#endif
		break;
	case 0x7003://时间设置
		if(time_invalid(p+GCMD_Data) == 1){//时间有效，校时
			timewrite(p+GCMD_Data);
		}
		Alen = Build_Echo(buftemp, 0x0370, p[GCMD_GnCode], 0);
		gprs_send_data(1, Alen, buftemp);//发送应答
		break;
	case 0x7004://上网IP和节点设置
		break;
	case 0x7005://车载机程序启用时间设置
		break;
	case 0x7006://GPS机程序启用时间设置
		break;
	}
GJdataDealEnd:
	if((nestpP != 0) && (nestpLen > 0))//有重包，需要移到前面
	{
		pakege[0] = 0x01;//还有数据
		memcpy(pakege+1, (unsigned char*)&nestpLen, 2);//剩余的长度
		memmove(pakege+3, nestpP, nestpLen);
		return 1;
	}
	else
		return 0;
#else
	return 0;
#endif
}

