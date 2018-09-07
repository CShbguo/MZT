
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
#include "Font.h"
#include "Gszct.h"
#include "E_Deal24GCard.h"
#include "E_Gmobile.h"
#include "DEF8583.h"
#ifdef qPBOC_BUS
#include "qpboc_head.h"
#endif

#include "qpboc_8583.h"
#define DDMAX_RCV_PACKET_NUM 2

extern stLineInfo LineInfo[2];//线路信息定义包括站点信息 正反两组0组 正向    1组反向

extern unsigned char gMCardCand;
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
extern unsigned short gSendOverTime;//发送后收应答超时计数器
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
extern void clr_Record_point(void);
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
extern unsigned char isNetOK[5];//两路链接是否成功;
extern void find_new_mission(void);
extern void find_G_new_mission(void);//此任务一秒进一次;
extern unsigned int BuildGJPackge(unsigned char *revBuf, unsigned char mode);
extern void gprs_send_data(unsigned char linkNum, unsigned int len, unsigned char *dat);
extern void MC55i_SISREAD(void);
extern void tcpipClose(unsigned char mode);

extern void getbusticketVer(void);
extern void getCpuDeviceNO(unsigned char *dat);

#ifdef _GPS_ENABLE_
extern unsigned int Build_GJ_GPS(unsigned char *dat);
#endif
extern void out_GPRS(void);
extern unsigned char gAPN_Style;//APN状态，是专用网还是公网
#define GPRSCOMMAX 1024+64
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

#ifdef CPU_JSB
//无效返回0  有效返回条数
unsigned int load_white_list(void);
#endif


unsigned char Xor8(const unsigned char *pData, unsigned long DataLen)
{
	unsigned char ret;
	unsigned long i;
	
	ret = 0;
	for(i=0; i<DataLen; i++)
	{
		ret ^= pData[i];		
	}	
	
	return ret;
}
void gprs_send_data_OTHER_CMD(unsigned char cmd, unsigned int len, unsigned char *dat)
{
	unsigned char buff[200];
	unsigned int pos=0;
//	unsigned int CRC;
	unsigned short i;
	
	memset(buff, '$', 4);//包头
	pos = 4;
	
	i = len+9;
	memcpy(buff+pos, (unsigned char*)&i, 2);
	over_turn(2, (unsigned char*)buff+pos);//数据长度，数据类型+应用层数据 的长度,高字节在前
	pos += 2;
	
	buff[pos++] = cmd;//命令
	buff[pos++] = gpacSNo;//包序号
	if (len !=0)
	{
		memcpy(buff+pos, dat, len);
		pos += len;
	}
	
// 	CRC = crc16_isr((unsigned char*)buff, pos);
// 	
// 	memcpy(buff+pos, (unsigned char*)&CRC, 2);
// 	//over_turn(2, buff+pos);
// 	pos += 2;
	buff[pos++] = Xor8(buff,pos);
#ifdef _debug_DVR
//	miniDispstr(6,0,"S",0);
	//beep(1,50,50);
//	delayxms(200);
//	miniDispstr(6,0," ",0);
	debugstring("Send DVR:");
	debugdata(buff,pos,1);
#endif
	com_snd(COM1, pos, (unsigned char*)buff);

	if (cmd == 0x02)
	{
// 		MSG_LOG("存储到临时区----\r\n");
// 		sysfewrite(BIT_DVR_PACKET,pos,buff);
	}
	

}
#ifdef OTHER_GPRS
void deal_DVR_ack(void)
{
	unsigned char sendbuf[10];
	
	memset(sendbuf,0,10);
	sysfewrite(BIT_DVR_PACKET,10,sendbuf);
#ifdef _debug_
	debugstring("清除DVR发送区::::\r\n");
#endif	
}
void gprs_other_sndbuinfo(void)
{
	unsigned char ddbuff[32];
	unsigned int pos = 0;
	
	memcpy(ddbuff+pos, pFistVary.DeviceNo, 8); pos += 8;
	Get_SerialNumF4(ddbuff+pos); pos += 4;
	memcpy(ddbuff+pos, StufferNO, 4); pos += 4;
	memcpy(ddbuff+pos, pFistVary.LineNo, 2); pos += 2;
	memcpy(ddbuff+pos,&SysTime.year_h,7); pos +=7;
	ddbuff[pos++] = F_FLAG_VARY.stop_flag;
	
	
	
	gprs_send_data_OTHER_CMD(0x02, pos, ddbuff);
}

void gprs_other_check_dvr(void)
{
	unsigned char ddbuff[32];
	unsigned int pos = 0;
	
// 	memcpy(ddbuff+pos, pFistVary.DeviceNo, 8); pos += 8;
// 	Get_SerialNumF4(ddbuff+pos); pos += 4;
// 	memcpy(ddbuff+pos, StufferNO, 4); pos += 4;
// 	memcpy(ddbuff+pos, pFistVary.LineNo, 2); pos += 2;
// 	memcpy(ddbuff+pos,&SysTime.year_h,7); pos +=7;
// 	ddbuff[pos++] = F_FLAG_VARY.stop_flag;
	
	
	
	gprs_send_data_OTHER_CMD(0x03, pos, ddbuff);

}

void Deal_DVR_Status(unsigned char *dat)
{
	// 	unsigned char *infor;
	// 	infor =( stDVRstation *)( dat+8);
	
	debugstring("NULL!!!!!!!!!!!!\r\n");
	;
}

//处理04命令，dVR返回的站点数据
void Deal_station_infor(unsigned char * dat)
{
	unsigned int plen=0;
	// 	unsigned char resave=0;
	stDVRstation *infor;
#ifdef _debug_DVR
	unsigned char uctemp=0;
	char disbuff[32];
#endif
	
	
	memcpy((unsigned char*)&plen, dat+4, 2);
	over_turn(2, (unsigned char*)&plen);
#ifdef _debug_
	MSG_LOG("deal--04--Deal_station_infor\r\n");
	BCD_LOG(dat,plen,1);
#endif
	
	if (plen !=35)
	{
		debugstring("line infor不够35字节!\r\n");
		return;
	}
	
	infor =( stDVRstation *)( dat+8);
	
	
#ifdef _debug_DVR
	uctemp = infor->ucDirect;
	cls();
	sprintf(disbuff,"Direct:%02d,%s,Now:%02d\x0\r\n",uctemp,uctemp==0 ?"UP":"DOWN",infor->ucStation);
	miniDispstr(0,0,disbuff,0);
	sprintf(disbuff,"Max:%02d,Middle:%02d\x0\r\n",infor->ucStationMax,infor->ucStaMiddle);
	miniDispstr(1,0,disbuff,0);
	memcpy(disbuff,infor->name,20);
	disbuff[20]=0;
	display(2,0,disbuff,0);
	
	sprintf(disbuff,"LineNO:%02X%02X\x0",infor->LineNO[0],infor->LineNO[1]);
	miniDispstr(4,0,disbuff,0);
	
	beep(1,50,50);
	delayxms(1000);
	
	restore_flag = 3;
	
	// 	MSG_LOG("报文::方向:%02d,%s,编号:%02d\r\n",uctemp,uctemp==0 ?"上行":"下行",infor->ucStation);
	// 	MSG_LOG("报文::最大编号%d,中转编号%d\r\n",infor->ucStationMax,infor->ucStaMiddle);
	// 	MSG_LOG("gFDStaIndex[gFDderict]%d\r\n",gFDStaIndex[gFDderict]);
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
//	unsigned int CRC;
	unsigned short i;
	memset(buff, '$', 4);//包头
	pos = 4;
	
	i = len+9;		//只算一个XOR
	memcpy(buff+pos, (unsigned char*)&i, 2);
	over_turn(2, (unsigned char*)buff+pos);//数据长度，数据类型+应用层数据 的长度,高字节在前
	pos += 2;
	
	buff[pos++] = 0x01;//公交数据，直接发送到后台，此字节不需要发送
	buff[pos++] = gpacSNo;//包序号
	if (len !=0)
	{
		memcpy(buff+pos, dat, len);
		pos += len;
	}
	
// 	CRC = crc16_isr((unsigned char*)buff, pos);	
// 	memcpy(buff+pos, (unsigned char*)&CRC, 2);
// 	//over_turn(2, buff+pos);
//	pos += 2;
	buff[pos++] = Xor8(buff,pos);
#endif

#ifdef _debug_DVR
//	miniDispstr(6,0,"S",0);
	//beep(1,50,50);
//	delayxms(200);
//	miniDispstr(6,0," ",0);
	debugstring("Send DvrData:");
	debugdata(buff,pos,1);
#endif

	com_snd(COM1, pos, buff);

}
//调度设备传回数据
void GPSDD_Deal(unsigned char mode, unsigned char pasn)
{
	unsigned char sendbuf[200];
	unsigned char pos=0;
//	unsigned int CRC;

	memset(sendbuf, '$', 4);
	pos = 4;
	switch(mode){
	case 0x21://转发服务器文件
		sendbuf[pos] = 0; pos++;
		sendbuf[pos] = 9; pos++;
		sendbuf[pos] = 0x21; pos++;
		sendbuf[pos] = pasn; pos++;//包序号
		break;
	case 0x22://车载机取IC 卡机设备信息
		sendbuf[pos] = 0; pos++;
		sendbuf[pos] = 0x1b; pos++;
		sendbuf[pos] = 0x22; pos++;
		sendbuf[pos] = pasn; pos++;//包序号
		memcpy(sendbuf+pos, pFistVary.DeviceNo, 8); pos += 8;
		Get_SerialNumF4(sendbuf+pos); pos += 4;
		memcpy(sendbuf+pos, StufferNO, 4); pos += 4;
		memcpy(sendbuf+pos, pFistVary.LineNo, 2); pos += 2;
		break;
	case 0x23://刷卡人次上传报
		sendbuf[pos] = 0; pos++;
		sendbuf[pos] = 11; pos++;
		sendbuf[pos] = 0x23; pos++;
		sendbuf[pos] = pasn; pos++;//包序号
		sendbuf[pos++] = 0;//上车刷卡人数
		sendbuf[pos++] = 0;//下车刷卡人数
		break;
	case 0x24://刷卡人次清除报
		sendbuf[pos] = 0; pos++;
		sendbuf[pos] = 9; pos++;
		sendbuf[pos] = 0x24; pos++;
		sendbuf[pos] = pasn; pos++;//包序号
//		gOTGrc = 0;
		break;
	case 0x25://司机上班记录报应答
		sendbuf[pos] = 0; pos++;
		sendbuf[pos] = 25; pos++;
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
// 	CRC = crc16_isr(sendbuf, pos);
// 	memcpy(sendbuf+pos, (unsigned char*)&CRC, 2);
// 	over_turn(2, sendbuf+pos);
// 	pos += 2;
	sendbuf[pos++] = Xor8(sendbuf,pos);

#ifdef _debug_
	debugstring("GPSDD_Deal Send:");
	debugdata(sendbuf,pos,1);
	com_snd(COM4, pos, sendbuf);
#else
	com_snd(COM1, pos, sendbuf);

#endif
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
#ifdef _debug_gprs
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
			memcpy(gPacketRcvBuf[ii].LinkFrame.uiFrameLen, (unsigned char*)&gprsIndex, 2);
			gPacketRcvBuf[ii].ucItemStatus = 0xea;
#ifdef _debug_gprs
			debugstring("接收:");
			debugdata((unsigned char*)&gPacketRcvBuf[ii].LinkFrame, gprsIndex+3, 1);
#endif

			gprsIndex = 0;
			memset(gprsRevBuf,0,10);
		}
		break;
	}
}


#else

void DVR_test(void)
{
//	unsigned char ddbuff[32];
	//	unsigned int pos = 0;
	
	
	//MSG_LOG("test----\r\n");
//	gprs_send_data_OTHER_CMD(2,0,ddbuff);	//发送测试命令04  看看DVR有没有回应
	gprs_other_sndbuinfo();
	
}




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
#ifdef _debug_DVR
			debugstring("长度错误*************\r\n");
#endif
			gprsIndex = 0;
			memset(gprsRevBuf,0,10);
			break;
		}
		if(gprsIndex >= plen){//数据接收完成
#ifdef _debug_
			debugstring("收DVR_CMD:0x");
// 			debugdata(gprsRevBuf+6,1,1);
// 			debugdata(gprsRevBuf, gprsIndex,1);
#endif

#ifndef _debug_
			if (Xor8(gprsRevBuf,gprsIndex-1) != gprsRevBuf[gprsIndex-1])
			{
				debugstring("XOR error*************\r\n");

				gprsIndex = 0;
				memset(gprsRevBuf,0,10);
				break;
			}
#endif

#ifdef _debug_DVR
			miniDispstr(6,2,"R",0);
			beep(1,50,50);
			delayxms(100);
			miniDispstr(6,2," ",0);
#endif

#ifdef _debug_DVR
//			debugstring("----DVR--ready------\r\n");
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
				memcpy(gPacketRcvBuf[ii].LinkFrame.FrameBuf, gprsRevBuf+8, plen-9);
				gPacketRcvBuf[ii].LinkFrame.ucFrameType = 0x07;
				memcpy(gPacketRcvBuf[ii].LinkFrame.uiFrameLen, (unsigned char*)&plen, 2);
				gPacketRcvBuf[ii].ucItemStatus = 0xea;
			}
			if((gprsRevBuf[6]==0x21))
				GPSDD_Deal(gprsRevBuf[6], gprsRevBuf[7]);
			else if ((gprsRevBuf[6]==0x02))
			{		//IC卡机主动上报设备信息内容(0x02)
				
				deal_DVR_ack();
			}
			else if ((gprsRevBuf[6]==0x03))
			{		//IC卡机主动上报设备信息内容(0x02)
				
				Deal_DVR_Status(gprsRevBuf);
			}

			else if ((gprsRevBuf[6]==0x04))
			{
				Deal_station_infor(gprsRevBuf);
			}
			else 
				gpacSNo++;//如果发包有回应，就加，下面将是新包
#ifdef _debug_DVR
// 			delayxms(200);
//			miniDispstr(6,2," ",0);
#endif

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
//		unsigned char t=0;
#ifdef OTHER_GPRS
	static unsigned int dvr_timer=0;
#endif
	gGprsResertTime++;
	// 	sprintf(buff, "gGprsResertTime:%d", gGprsResertTime);
	// 	debugstring(buff);
	if((gGprsResertTime >= 600)&&((GPRSLinkProcess&0xF0) != 0xF0)){
		gGprsResertTime = 0;
		gmissflag = 0;
		GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;
	//	isNetOK[0] = 0;	//重新断开连接下  2017.05.19
#ifdef _debug_gprs
		debugstring("超时-------\r\n");
#endif
	}


	
	sprintf(buff, "%02X", GPRSLinkProcess);
	miniDispstr(15, 19, buff, DIS_RIGHT|DIS_CONVERT);
#ifdef NO_MOBILE_SERVISE	//如果没有移动平台，则默认移动平台连接错误,不然会导致检测到GPRS错误时不重启模块.
	gErrorFlag |= ERROR_CONNECT_MOBILE;
#endif
	
#ifndef NO_MOBILE_24G_
	if((g24PurError>0)&&(g24PurError<0xa0))//等手机模块初始化完成
		miniDispstr(14, 19, "--", DIS_RIGHT|DIS_CONVERT);
	else{
		sprintf(buff, "%02X", gmissflag);
		miniDispstr(14, 19, buff, DIS_RIGHT|DIS_CONVERT);
	}
#elif defined _CPU_24G_
	if((g24PurError>0)&&(g24PurError<0xa0))//等手机模块初始化完成
		miniDispstr(14, 19, "--", DIS_RIGHT|DIS_CONVERT);
	else{
		sprintf(buff, "%02X", gmissflag);
		miniDispstr(14, 19, buff, DIS_RIGHT|DIS_CONVERT);
	}
#else
	sprintf(buff, "%02X", gmissflag);
	miniDispstr(14, 19, buff, DIS_RIGHT|DIS_CONVERT);
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
	
#ifdef OTHER_GPRS
	if ((++dvr_timer % 4) == 0)
	{
		
		//DVR_test();
		//dvr_timer = 0;
	}
#endif
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
#elif defined ALL4G
	inmmdd = Gmode_ALL4G;
#elif defined YYLinux4G
	inmmdd = Gmode_YYLinux;
#else
	inmmdd = Gmode_ALL4G;
#endif
	
#ifdef Modem_BUS_
	inmmdd |= Gmode_MODEM;
#endif
	
	return inmmdd;
}


extern unsigned char gprsSingle;//GPRS信号强度
//每一秒钟进一次
int in_sendGPs(void)
{
	#ifdef _GPS_NEED_REC_
	unsigned int t;
	unsigned char sndbuf[128];
	#endif
unsigned char static Dly=0;
if((gmissflag==MISS_G_UREC)&&(Dly++<10))//传数据的时候会加快  所以这里减速
	return 0;
	Dly=0;
#ifdef _GPS_NEED_REC_

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
	//显示信号图标
	ShowGprsSinal(2,14,gprsSingle);
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
unsigned int getCpuIPaddr(void)
{
	return ClientIP;
}
unsigned short getCpuPort(void)
{
	return ClientPort;
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

//			ClientIP = getDefaultIP();//写死IP地址。。
		memcpy((unsigned char*)gpsrsP+50, (unsigned char*)&ClientIP, 4);
		sprintf((char*)gpsrsP+60, "%u.%u.%u.%u", gpsrsP[50],gpsrsP[51],gpsrsP[52],gpsrsP[53]);
		if((memcmp(gpsrsP+50, "\x00\x00\x00\x00", 4) == 0)||(memcmp(gpsrsP+50, "\xFF\xFF\xFF\xFF", 4) == 0)){
			ClientIP = getDefaultIP();
			memcpy((unsigned char*)gpsrsP+50, (unsigned char*)&ClientIP, 4);
			sprintf((char*)gpsrsP+60, "%u.%u.%u.%u", gpsrsP[50],gpsrsP[51],gpsrsP[52],gpsrsP[53]);
		}
		memcpy(gpsrsP+i, gpsrsP+60, strlen((char*)gpsrsP+60));
		i += strlen((char*)gpsrsP+60);
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

extern unsigned int Buildsl8583Packge(unsigned char *revBuf, unsigned char mode);
extern int open_sock(unsigned int addr, unsigned short port, unsigned int outtime, unsigned char link);
void TaskRecWrite(void)
{
// #ifdef OTHER_GPRS
// 	unsigned short i=0;
// 	static unsigned char dvr_ack_outtime=0;		//车载机给dvr厂商的数据，要求DVR回复的这里有个超时等待
// 	#define DVR_OUTTIME	20
// 	#define DVR_STATUSTIME 40
// #endif

	

#ifndef NO_GJ_SERVISE
//	unsigned short i=0;
//	unsigned short sum;
	unsigned char buffer[1025];//[2100];
	unsigned int uilen;
//	int temp1;

	unsigned int ip = 0, port = 0;

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
//	beep(1,100,100);
		//set_timer0(1000,1);
// #ifdef OTHER_GPRS
// 	dvr_ack_outtime++;
// 	if((dvr_ack_outtime% DVR_OUTTIME) == 0){
// 		sysferead(BIT_DVR_PACKET,6,buffer);
// 		if ((memcmp(buffer,"$$$$",4)) == 0)
// 		{
// 			memcpy((unsigned char *)&i,buffer+4,2);
// 			over_turn(2,(unsigned char *)&i);
// 			sysferead(BIT_DVR_PACKET,i,buffer);
// #ifdef _debug_
// 			com_snd(COM4, i, buffer);
// 			debugstring("\r\nsysread$$$$:");
// 			debugdata(buffer,i,1);
// #else
// 			com_snd(COM1, i, buffer);
// #endif
// 			gpacSNo = buffer[8];		//报序号			
// 		}
// //		dvr_ack_outtime = 0;
// 		
// 	}
// 	if((dvr_ack_outtime% DVR_STATUSTIME) == 0){
// 	//	gprs_other_check_dvr();
// 	}
// 
// 	
// #endif
	qpoc_flag_or();

	if((GPRSLinkProcess > 14)&&((gmissflag == 0)||(gmissflag == MISS_G_FREE))){
// #ifndef NO_MOBILE_SERVISE
// 		if((gmissflag == 0)||(gmissflag == MISS_G_FREE)){//没有任务，找找任务
// 			find_new_mission();
// 		}
// #endif	
		
		if((gmissflag == 0)||(gmissflag == MISS_G_FREE)||(gmissflag == MISS_G_LOGINGJ)){
		
			find_G_new_mission();

		if(qpoc_flag_or()==ST_OK)
				find_qpboc_new_mission();
		return;
	}
	}
next_qpoc:
	if((GPRSLinkProcess != TCPSTARTSTAT)&&//还没连接TCPIP
		(GPRSLinkProcess != GPRS_SENDING_CMD)){
		//	debugstring("任务-----");
	
		return;
	}

	if(isNetOK[0] == 0){//GPRSLinkProcess = 21,但是已经断开了，需要重新连接
		GPRSLinkProcess = GPRS_NEED_CLOSEIP;
	//	debugstring("任务-----");
		return;
	}

	if(GPRSLinkProcess == GPRS_SENDING_CMD){//正在等待上个命令应答
#ifdef _debug_gprs
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
				MSG_LOG("接收应签超时 closeIP\r\n");
				GPRSLinkProcess = GPRS_NEED_CLOSEIP;//tcpipClose(3);
			}
#endif
			
#ifdef _debug_gprs
			debugstring("gGPRS_Cmd_outtime:");
			debugdata((unsigned char*)&gGPRS_Cmd_outtime, 1, 0);
#endif

			//

			if(gmissflag==MISS_GJ)
			{
					MSG_LOG("公交超时-1-3-\r\n");
				set_timer0(5000, 0);//10ms钟进一次
			}
			//
			gGPRS_Cmd_outtime++;
			if(gGPRS_Cmd_outtime == 2){

				gmissflag = 0;
				gmissOldflag = 0;
				MSG_LOG("outtime closeIP\r\n");
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
#ifdef _debug_
				debugstring("连接错误gGPRS_Cmd_outtime > 4,断开---\r\n");
#endif
				gGPRS_Cmd_outtime = 0;
//				GPRSLinkProcess = GPRS_NEED_CLOSEIP;//tcpipClose(3);
#ifdef WIFI_TLH_
				GPRSLinkProcess = GPRS_AT_WAIT;//GPRS_MODE_NEED_CLOSE;
#else
				GPRSLinkProcess = GPRS_NEED_CLOSEIP;//GPRS_MODE_NEED_CLOSE;
#ifdef GPRS_MG323_

				gGPRS_data_style.ucReadStyle=0;
#endif
#endif
			}
		}
		return;
	}
	debugstring("任务标志位3：gmissflag=");
	debugdata(&gmissflag,1,1);
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
// 	if(((gmissflag & MISS_IF2) == MISS_IF2)||((gmissflag & MISS_IF1) == MISS_IF1)){
// 		gmissflag = 0;
// 		return;
// 	}
#endif//#ifndef NO_MOBILE_SERVISE

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

		switch(gmissflag){
		case MISS_G_TOO:
		case MISS_G_LOGINGJ:
		case MISS_G_SINGOUT:
		case MISS_G_PRICE:
		case MISS_G_DBLKD:
		case MISS_G_DBLKI:
		case MISS_G_UREC:
		case MISS_G_FILES:
		case MISS_G_DLine:
		case MISS_G_DSound:
		case MISS_G_DPROOK:
		case MISS_G_DLineOK:
		case MISS_G_DSoundOK:
		case MISS_G_HART:
		case MISS_G_GPS:
		case MISS_G_ALAM:
		MSG_LOG("--------公交任务:%02X\r\n", gmissflag);
		if (isNetOK[LINK_GJ] == 0)
		{
				clr_wdt();   //
			if (open_sock(ClientIP, ClientPort, 1500, LINK_GJ) < 0)
			{
				MSG_LOG("公交链接有问题\r\n");
				return;
			}
		}
		else
		{
			clr_wdt(); 
			MSG_LOG("LINK_GJ链路%d正常:%d\r\n", LINK_GJ, isNetOK[LINK_GJ]);
		}
			uilen = Buildsl8583Packge(buffer, gmissflag);//BuildGJPackge(buffer, gmissflag);
			if(uilen == 0){
				gmissflag = MISS_G_FREE;
				break;
			}
// 			if(gGPSdly < 3){
// 				gGPSdly = 3;//要发数据，GPS信息延迟发送
// 			}
#ifdef _debug_
			debugstring("TaskRecWrite_snd data:");
			debugdata(buffer, uilen, 1);
#endif
		gprs_send_data(LINK_GJ, uilen, buffer);
		if ((gmissflag == MISS_G_HART) || (gmissflag == MISS_G_GPS) || (gmissflag == MISS_G_DBLKI)) {
			MSG_LOG("gmissflag:%02X,不等待\r\n", gmissflag);
			gmissflag = MISS_G_FREE;
			return;//不需要等应答
		}

		GPRSLinkProcess = GPRS_SENDING_CMD;
		setSendTimeOut(LINK_GJ, DE_SendOverTime);
		break;

	case MISS_PBOC_LOGIN:
	case MISS_PBOC_PURSE:
	case MISS_PBOC_RE_PURSE:
	case MISS_PBOC_UPREC_ODA:
	case MISS_PBOC_DOWN_ODA_BLK:
	case MISS_PBOC_UPREC_ODA_first:
	case MISS_PBOC_UPREC_ca:
		//MSG_LOG("银联链路%d情况:%d\r\n", LINK_POBC, isNetOK[LINK_POBC]);
		if (isNetOK[LINK_PBOC] == 0)
		{
			// 			ClientPort=GJ_PORT;
			// 			ClientIP=GJ_IP;
			printf("链接有问题1\r\n");
			getMobileParameter(3, (unsigned char *)&ip);
			getMobileParameter(4, (unsigned char *)&port);
			clr_wdt();
			if (open_sock(ip, port, 1500, LINK_PBOC) < 0)
			{
				printf("链接有问题\r\n");
				return;
			}
		}
		else
		{
			MSG_LOG("LINK_POBC链路%d正常:%d\r\n", LINK_PBOC, isNetOK[LINK_PBOC]);
			clr_wdt();
		}


		//uilen = Buildsl8583Packge(buffer, gmissflag);//BuildGJPackge(buffer, gmissflag);

		memset(msgf, 0, sizeof(msgf));
		MSG_LOG("清空msgf--\r\n");
		memset(buffer, 0, sizeof(buffer));//算MAC时候buff里面不够8字节要填\x00 ，先清空
		uilen = Build_qpboc_8583Packge(buffer, gmissflag);
		if (uilen == 0) {
			if (gmissflag == MISS_PBOC_RE_PURSE) {
				// 消除冲正记录
				save_repurse_infor(FeRC_Dlelt, NULL);
			}
			gmissflag = MISS_G_FREE;
			break;
		}

#ifdef _debug_//GJ_TLVDeal_
		MSG_LOG("8583 data: len:%d\r\n", uilen);
		debugdata(buffer, uilen, 1);
#endif

#ifdef HTTP_HEAD
		memset(http_head, 0, sizeof(http_head));
		http_len = Build_http_pack(http_head, ClientIP, ClientPort, uilen);
		MSG_LOG("httphead len:%d\r\n%s\r\n", http_len, http_head);
		memmove(buffer + http_len, buffer, uilen);
		memcpy(buffer, http_head, http_len);
		uilen += http_len;
#endif
		// 			if(gGPSdly < 3){
		// 				gGPSdly = 3;//要发数据，GPS信息延迟发送
		// 			}
#ifdef debug_GJ_TLVDeal_
		MSG_LOG("TaskRecWrite_snd PBOC data(%d):", uilen);
		debugdata(buffer, uilen, 1);
#endif
		if (gmissflag == MISS_PBOC_PURSE)
		{
			if (gMCardCand == CARDSTYLE_UNPAY_ODA) {
				save_ODA_infor(ODA_FeRC_Write, repurse_infor);

			}
			else {
				MSG_LOG("保存冲正\r\n");
				save_repurse_infor(FeRC_Write, repurse_infor);
			}
		}

		gprs_send_data(LINK_PBOC, uilen, buffer);
		if ((gmissflag == MISS_G_HART) || (gmissflag == MISS_G_GPS) || (gmissflag == MISS_G_DBLKI)) {
			MSG_LOG("gmissflag:%02X,不等待\r\n", gmissflag);
			gmissflag = MISS_G_FREE;
			return;//不需要等应答
		}
		GPRSLinkProcess = GPRS_SENDING_CMD;
		setSendTimeOut(LINK_PBOC, 50);
		gGPRS_Cmd_outtime = 0;

		break;
	case MISS_HTTP_BLK:	//通过模块下载
	case MISS_HTTP_EC20:
	case MISS_HTTP_PRO:
		uilen = BuildHTTPPackge(buffer, gmissflag);
		break;
	case MISS_G_FREE:
		break;
	default:
		gmissflag = 0;
		break;
	}
// 
// 
// 	if((gmissflag == 0)||(gmissflag == MISS_G_FREE)||\
// 		(gmissflag == MISS_G_LOGINGJ)){
// 				find_G_new_mission();
// 			if(gmissflag==0x40|| (gmissflag == MISS_G_FILES  && memcmp(gsl8583FileDownPara.Miss_Fileflag,SL8583FileFLAG_PRO,3)==0 ))
// 			{
// 				debugstring("公交有登录和下载任务\r\n");
// 				return;
// 			}
// #ifdef qPBOC_BUS	
// 			find_qpboc_new_mission();
// #endif
// 			
// 			//银联有任务
// 			if ((gmissflag&MISS_PBOC_LOGIN)==MISS_PBOC_LOGIN)
// 			{
// 				debugstring("有银联任务\r\n");
// 				return ;
// 			}
// 
// 			find_G_new_mission();
// 
// 	}
	debugstring("TASK END ");
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
				saveCpuInfo(3, Ipbuf);
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
/*
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

  	*/



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
			MSG_LOG("SetSevice closeIP\r\n");
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

// 	if(gBusblkDownPro.Miss_PPack != 0)//正在下载黑名单
// 		return;
	if(gsl8583FileDownPara.Miss_offset >10 )//正在下载黑名单
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
	
	Get_SerialNumF4(dat+len);
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
extern void getbusticketVer(void);
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
	Get_SerialNumF4(dat+len);
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
#ifdef ZBCD_DRICENO
		ASCII2ZBcod((char *)pFistVary.DeviceNo, dat+len, 4);//车号4B
#else
		Ascii2BCD(pFistVary.DeviceNo, dat+len, 8);//车号4B
#endif
		
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
#ifdef CPU_JSB
		if (load_white_list() == 0)
		{
			dat[len] = 0xaa;		//告诉后台要白名单
		}else{
			dat[len] = 0;
		}
		len ++;
		 dat[len++] = 0;
#else
		memcpy(dat+len, "\x01\x01", 2);//行业代码2B
		len += 2;
#endif


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
		getbusticketVer();
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
	
	Get_SerialNumF4(dat+len);
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
			// 			Get_SerialNumF4(buff);
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
	
	Get_SerialNumF4(dat+len);
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
	if(crc16 < temp)
		crc16 += 1;//需要下一个包
	else
		crc16 = 0xffff;//下载完成，不需要下载了
	if(temp == 0)
		crc16 = 1;

#ifdef debug_GJ_TLVDeal_
	debugstring("Build_GJ_DPro:\r\n");
	debugstring("MISS_MAXPack::");
	debugdata((unsigned char *)&temp, 4, 1);
	debugstring("需要的包::");
	debugdata((unsigned char *)&crc16, 2, 1);
#endif


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
	
	Get_SerialNumF4(dat+len);
//Ascii2BCD(pFistVary.DeviceNo, dat+len, 8);
	len += 4;
	
	dat[len++] = 0x40;//CmdCode
	dat[len++] = 0x01;
	
	dat[len++] = gBusblkDownPro.Miss_style;//GnCode
	
	memcpy(dat+len, (unsigned char*)&gBusblkDownPro.Miss_PPack, 2);//p1,p2收到的包序号
	over_turn(2, dat+len);
	len += 2;
#ifdef _debug_blk
// 	debugstring("Buid_GJ_DBLK 包头：");
// 	debugdata((unsigned char*)&gBusblkDownPro.Miss_style, 1, 1);
// 	debugdata((unsigned char*)&gBusblkDownPro.Miss_PPack, 2, 1);
// 	debugdata((unsigned char*)&gBusblkDownPro.MISS_MAXPack, 4, 1);
// 	debugdata((unsigned char*)&gBusblkDownPro.Miss_ver, 2, 1);
// 	debugdata((unsigned char*)&gBusblkDownPro.Miss_ALL_LEn, 4, 1);
#endif

	temp = gBusblkDownPro.MISS_MAXPack;//最大包号
	crc16 = gBusblkDownPro.Miss_PPack;//当前包号

	if(crc16 < temp)
		crc16 += 1;//需要下一个包
	else
		crc16 = 0xffff;//下载完成，不需要下载了


#ifdef _debug_blk
	debugstring("Build_GJ_DPro:");
	debugstring("MISS_MAXPack::");
	debugdata((unsigned char *)&temp, 4, 1);
	debugstring("需要的包::");
	debugdata((unsigned char *)&crc16, 2, 1);
#endif

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
	case MISS_G_FILES://程序下载
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
	
	Get_SerialNumF4(dat+len);
	//Ascii2BCD(pFistVary.DeviceNo, dat+len, 8);
	len += 4;
	
	dat[len++] = 0x50;//CmdCode
	dat[len++] = 0x03;
	
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

	unsigned int len=0;
	unsigned short crc16;
	#if 0
	len = 0;
	dat[len++] = 0xBD;//报文头(Head)
	dat[len++] = 0xBD;//
	dat[len++] = 0xBD;//
	
	Get_SerialNumF4(dat+len);
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
	#endif
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
	
	Get_SerialNumF4(dat+len);
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
	
	Get_SerialNumF4(dat+len);
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



int GJRec_Send(void)
{
	struct{
		unsigned int shadd;
		unsigned int sLen;
		unsigned char sdate[8];
		unsigned int scrc32;
	}hisda;
	unsigned int sum;
	unsigned int curp, headp;
	

	stFAT_hisRec hismsg;
	
#ifdef 	_FASH_FATs_SYS_
	sum = Check_Send_FAT_Rcord(&hismsg);
	if (sum == 1 )
	{
		return 1;//找到后台要采集的信息		
	}
#endif


	//先处理是否有历史记录发送
	sysferead(BIT_HISREC_SND, 20, (unsigned char *)&hisda);
	sum = __cpuCrc32((unsigned char *)&hisda, 16);
#ifdef _debug_X
	debugstring("GJRec Send his data:");
	debugdata((unsigned char *)&hisda, 20, 1);
#endif
	if((hisda.scrc32 == sum)&&(hisda.sLen != 0)&&(hisda.sLen <= (FLASH2_BAKR_LEN/RECORD_LEN))&&(hisda.shadd < FLASH2_BAKR_LEN)){//有历史记录
		sum = hisda.sLen;
	}
	else{

		curp = Get_Record_point((unsigned char*)&headp, 0);
//如果记录缓冲中没有记录可发将发送E2PROM中没有发成功的记录
		if(curp>headp)
		{
			sum=(curp - headp)/RECORD_LEN;
#ifdef _debug_
			debugstring("有记录!\r\n");
#endif		
		}
		else{
			sum = 0;
			if(curp >= (FLASH1_START+FLASH_RCLR)){//0x3E800如果记录地址已经达到了40000条（64字节），则重置存贮区，重新开始
				//clr_Record_point();
				lib_clr_Record_point();//原来用的是 clr_Record_point();   这个函数会把备份区的地址也清掉。。
				
			}
			tcpipPacket.packetStatus = 40;
		}
	}
	return sum;
}

extern void tcpipClose(unsigned char mode);
void find_G_new_mission(void)//此任务一秒进一次
{
	unsigned int iver;
//	unsigned short usTemp=0;
	unsigned char i;//, flag;
	unsigned char btf[20];

	//MSG_LOG("do %s\r\n",__FUNCTION__);

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
// 		if ((gmissflag&MISS_PBOC_LOGIN) == MISS_PBOC_LOGIN)
// 	{
// 		return;
// 	}

	gErrorFlag &= (~ERROR_BUS_CONNECT);// 清除错误标识

	if(gsl8583Style.ISOK == 0){//还没有签过到
		if(gmissflag == MISS_G_LOGINGJ)
		{
//				debugstring("没有签到！:");
			return;	//已经是签到状态了。
		}
//#ifdef _debug_gprs
	//	debugstring("没有签到！:");
//#endif
		gmissflag = MISS_G_LOGINGJ;
		if(GPRSLinkProcess == GPRS_SENDING_CMD)
			GPRSLinkProcess = TCPSTARTSTAT;
		return;
	}

	if(BlackLength>BLACKSUM){//黑名单条数错误，重下黑名单
#ifdef _debug_
		MSG_LOG("BlackLength:%d,>%d\r\n",BlackLength,BLACKSUM);
		debugstring("黑名单条数错误，重下黑名单\r\n");
#endif
		delallBlacInbus();
	}

	
	//先找参数文件
		for (i = 0; i < sl8583fileNum; i++) {
			if (memcmp(gsl8583filelist[i].filename, SL8583FileFLAG_CSN, 3) == 0)
				break;

		}
		//	MSG_LOG("i========%d\r\n",i);
		if (i < sl8583fileNum)
		{

			if (memcmp(gsl8583filelist[i].filename, SL8583FileFLAG_CSN, 3) == 0) {//有黑名单下载
				iver = 0;
				MSG_LOG("find:%s\r\n", SL8583FileFLAG_CSN);

				memcpy((unsigned char*)&iver, gBusVerInfo.CSN_BUSVer, 2);
				MSG_LOG("find:%s:本地ver:%d,服务器ver:%d\r\n", SL8583FileFLAG_CSN, iver, gsl8583filelist[i].fileVer);

				if (gsl8583filelist[i].fileVer == iver) {//已经是最新版了
					memset(gsl8583filelist[i].filename, 0, 4);//清除平台文件参数
				}
				else {//不一致，需要下载
					goto load_cs;
				}
			}

		}
		//线路文件
		//先找参数文件
		for (i = 0; i < sl8583fileNum; i++) {
			if (memcmp(gsl8583filelist[i].filename, SL8583FileFLAG_PRI, 3) == 0)
				break;

		}
		//	MSG_LOG("i=====22=%d\r\n",i);
		if (i < sl8583fileNum)
		{

			if (memcmp(gsl8583filelist[i].filename, SL8583FileFLAG_PRI, 3) == 0) {//有设备参数下载
				iver = 0;

				Ascii2BCD(gsl8583filelist[i].fileVer2, btf, 8);
				if (memcmp(btf, pFistVary.LineNo, 2) != 0)
				{
					MSG_LOG("下载线路不一致\r\n");
					memset(gsl8583filelist[i].filename, 0, 4);//清除平台文件参数

				}



				memcpy((unsigned char*)&iver, gBusTicketV.busticketVer, 2);
				MSG_LOG("find:%s:本地ver:%d,服务器ver:%d\r\n", SL8583FileFLAG_PRI, iver, gsl8583filelist[i].fileVer);
				if (gsl8583filelist[i].fileVer == iver) {//已经是最新版了
					memset(gsl8583filelist[i].filename, 0, 4);//清除平台文件参数
					MSG_LOG("票价为最新版本\r\n");
				}
				else {//不一致，需要下载
					MSG_LOG("票价需要下载\r\n");
					goto load_cs;
				}
			}

		}



	
	//如果已经签到，看看是否有参数要下发 
	for(i=0; i<sl8583fileNum; i++){
		if(memcmp(gsl8583filelist[i].filename, SL8583FileFLAG_PRI, 3) == 0){//有设备参数下载
			iver = 0;

			Ascii2BCD(gsl8583filelist[i].fileVer2,btf,4);
			if (memcmp(btf,pFistVary.LineNo,2) != 0)
			{
				memset(gsl8583filelist[i].filename, 0, 4);//清除平台文件参数
				continue;
			}

			memcpy((unsigned char*)&iver, gBusTicketV.busticketVer, 2);
			over_turn(2,(unsigned char*)&iver);
			if(gsl8583filelist[i].fileVer == iver){//已经是最新版了
				memset(gsl8583filelist[i].filename, 0, 4);//清除平台文件参数
			}
			else{//不一致，需要下载
				break;
			}
		}
//#ifndef _debug_
		else if(memcmp(gsl8583filelist[i].filename, SL8583FileFLAG_BLK, 3) == 0){//有黑名单下载
			iver = 0;
			memcpy((unsigned char*)&iver, gBusVerInfo.busBLKVer, 2);
			if(gsl8583filelist[i].fileVer == iver){//已经是最新版了
				memset(gsl8583filelist[i].filename, 0, 4);//清除平台文件参数
			}
			else{//不一致，需要下载
				break;
			}
		}
//#endif
		else if(memcmp(gsl8583filelist[i].filename, SL8583FileFLAG_qpoc, 3) == 0){//有黑名单下载
			iver = 0;
			memcpy((unsigned char*)&iver, gBusVerInfo.QPOCVer, 2);
			if(gsl8583filelist[i].fileVer == iver){//已经是最新版了
				memset(gsl8583filelist[i].filename, 0, 4);//清除平台文件参数
			}
			else{//不一致，需要下载
				break;
			}
		}

		else if(memcmp(gsl8583filelist[i].filename, SL8583FileFLAG_GPS, 3) == 0){//有定位限速下载
			iver = 0;
			memcpy((unsigned char*)&iver, gBusVerInfo.busLineVer, 2);
			if(gsl8583filelist[i].fileVer == iver){//已经是最新版了
				memset(gsl8583filelist[i].filename, 0, 4);//清除平台文件参数
			}
			else{//不一致，需要下载
				break;
			}
		}
		else if(memcmp(gsl8583filelist[i].filename, SL8583FileFLAG_SPK, 3) == 0){//有公钥下载       公司二维码公钥
			iver = 0;
			memcpy((unsigned char*)&iver, gBusVerInfo.busVoiceVer, 2);
			if(gsl8583filelist[i].fileVer == iver){//已经是最新版了
				memset(gsl8583filelist[i].filename, 0, 4);//清除平台文件参数
			}
			else{//不一致，需要下载
				break;
			}
		}
		
		else if(memcmp(gsl8583filelist[i].filename, SL8583FileFLAG_TAC, 3) == 0){//有公钥下载       公司二维码对称密钥    TAC
			iver = 0;
			memcpy((unsigned char*)&iver, gBusVerInfo.busTacVer, 2);
			if(gsl8583filelist[i].fileVer == iver){//已经是最新版了
				memset(gsl8583filelist[i].filename, 0, 4);//清除平台文件参数
			}
			else{//不一致，需要下载
				break;
			}
		}

		else if(memcmp(gsl8583filelist[i].filename, SL8583FileFLAG_WHT, 3) == 0){//有白名单下载
			
			if (memcmp(gsl8583filelist[i].fileVer2,WHT_BANK,4) != 0)
			{
				MSG_LOG("类型不一样:%s,%s\r\n",WHT_BANK,gsl8583filelist[i].fileVer2);
				memset(gsl8583filelist[i].filename, 0, 4);//清除平台文件参数
				continue;
			}
			
			iver = 0;
			memcpy((unsigned char*)&iver, gBusVerInfo.WHT_Bank, 2);
			if(gsl8583filelist[i].fileVer == iver){//已经是最新版了
				memset(gsl8583filelist[i].filename, 0, 4);//清除平台文件参数
			}
			else{//不一致，需要下载
				break;
			}
		}
		#ifndef _debug_
		else if(memcmp(gsl8583filelist[i].filename, SL8583FileFLAG_PRO, 3) == 0){//有设备程序下载
//50524F 5053540000000000 0121   PROPST  0121
			if (memcmp(gsl8583filelist[i].fileVer2,POS_Cand_FLAG,3) != 0)
			{
				memset(gsl8583filelist[i].filename, 0, 4);//清除平台文件参数
				continue;
			}

			iver = 0;
			memcpy((unsigned char*)&iver, gBusVerInfo.busProVer, 2);
			MSG_LOG("PROD:%04X,%04X\r\n", iver, gsl8583filelist[i].fileVer);
			if(gsl8583filelist[i].fileVer == iver){//已经是最新版了
				memset(gsl8583filelist[i].filename, 0, 4);//清除平台文件参数
			}
			else{//不一致，需要下载
				break;
			}
		}
#endif
	}
load_cs:
	if(i < sl8583fileNum){//找到了有需要下载的任务
		MSG_LOG("有文件任务:%s\r\n", (char*)gsl8583FileDownPara.Miss_Fileflag);
		MSG_LOG("版本:%02X%02X\r\n", gsl8583FileDownPara.Miss_ver[0], gsl8583FileDownPara.Miss_ver[1]);
		MSG_LOG("版本:0x%04X\r\n", gsl8583filelist[i].fileVer);

		if((memcmp(gsl8583FileDownPara.Miss_Fileflag, gsl8583filelist[i].filename, 3) == 0)&&
			(memcmp(gsl8583FileDownPara.Miss_ver, (unsigned char*)&gsl8583filelist[i].fileVer, 2) == 0)&&
			(gsl8583FileDownPara.Miss_ALL_LEn == gsl8583filelist[i].filelen) &&
			(gsl8583FileDownPara.filecrc == gsl8583filelist[i].crc32)
			)

		{//如果下在下载的版本和平台的内容一致，则续传
			;
		}
		else{//否则从头开始下载
			memcpy(gsl8583FileDownPara.Miss_Fileflag, gsl8583filelist[i].filename, 3);
			memcpy(gsl8583FileDownPara.Miss_FileVer2, gsl8583filelist[i].fileVer2, 8);
			memcpy(gsl8583FileDownPara.Miss_ver, (unsigned char*)&gsl8583filelist[i].fileVer, 2);
			gsl8583FileDownPara.Miss_ALL_LEn = gsl8583filelist[i].filelen;
			if(memcmp(gsl8583filelist[i].filename, SL8583FileFLAG_BLK, 3) == 0)
			{
				MSG_LOG("BLK注意:Miss_offset+=10\r\n");
				gsl8583FileDownPara.Miss_offset = 10;		//跳过黑名单.blk文件的里面8字节版本和条数 +2 预留对齐

			}else{
				gsl8583FileDownPara.Miss_offset = 0;

			}
			gsl8583FileDownPara.filecrc = gsl8583filelist[i].crc32;

		}

		MSG_LOG("找到downfile:%s,VER:0x%02X%02X\r\n",gsl8583FileDownPara.Miss_Fileflag,gsl8583FileDownPara.Miss_ver[0],gsl8583FileDownPara.Miss_ver[1]);


		gsl8583FileDownPara.CRC32 = checkgsl8583FileDownPara(1);	//计算CRC32
		sysfewrite(BIT_8583filedown, sizeof(st8583filedown), (unsigned char*)&gsl8583FileDownPara);//取上次下载参数据,用于续传
// 		if(memcmp(gsl8583filelist[i].filename, SL8583FileFLAG_PPP, 3) == 0){//有设备参数下载
// 			gmissflag = MISS_G_PRICE;
// 		}
// 		else{//其它
			gmissflag = MISS_G_FILES;//都使用同一个命令
// 		}
		return;
	}


// 	if (gGprs_Check_QR_BLK >0)
// 	{
// 		gGprs_Check_QR_BLK--;
// 	}
// 
// 	if (gGprs_Check_QR_BLK == 0)
// 	{
// 		//隔一段时间查询一下黑名单
// 		gGprs_Check_QR_BLK = 15*60;
// 		
// 		gmissflag = MISS_G_DBLKI;
// 		if(GPRSLinkProcess == GPRS_SENDING_CMD)
// 			GPRSLinkProcess = TCPSTARTSTAT;
// 		return;
// 		
// 		
// 	}

	
	//记录发送不再使用铁电。直接从存贮区打包，8583会把发送的记录起始地址和条数上发，后台收到后会发下来，收到回应后再把数据删除掉
	//不再使用 tcpipPacket变量

	if(	DELAY_SEND()==ST_OK)
	{
		if(GJRec_Send() != 0)//处理GPRS发送数据
			gmissflag = MISS_G_UREC;
		else{
			gmissflag = MISS_G_FREE;//没有记录要传 无任务
	}
// 		if(((GPRSLinkProcess >= 20)&&(GPRSLinkProcess < 0x30))||(GPRSLinkProcess == 0xA0)){
// 			MSG_LOG("没任务，关掉!\r\n");
// 			tcpipClose(0);
// 			GPRSLinkProcess = GPRS_STANDBY;
// 		}

		gSendGLogin++;
//	MSG_LOG("gSendGLogin==%d\r\n",gSendGLogin);
		if(gSendGLogin > SEND_PIN_COUNT_){

			gSendGLogin = 0;
//			gsl8583Style.ISOK = 0;	//重签到
			gmissflag = MISS_G_TOO;
			GPRSLinkProcess =TCPSTARTSTAT;
		}

	}
}

void load_black_list(void)
{
	unsigned int addr;
	unsigned short ii=0;
	unsigned char i;
#ifdef _debug_blk

	unsigned int j=0;
	unsigned char buffer[20];
#endif

#ifdef _debug_
	debugstring("加载黑名单:::\r\n");
#endif
	sysferead(BIT_BLACK_SUM,4,(unsigned char*)&pFistVary.BlackListNum);
	sysferead(BIT_BLACK_2BLOK, 32, (unsigned char*)&gBlackBlockNum[0]);
		// 
#ifdef _debug_blk
	debugstring("BlackListNum:::");
	debugdata((unsigned char*)&pFistVary.BlackListNum,4,1); //黑名单总数
	debugdata((unsigned char*)&gBlackBlockNum[0],32,1);
#endif

	addr = FLASH_BLK_START;
	flashread(addr, gBusVerInfo.busBLKVer, 2);//黑名单版本
#ifdef _debug_blk
	debugstring("gBusVerInfo.busBLKVer:");
	debugdata(gBusVerInfo.busBLKVer, 2, 1);
#endif
	memcpy((unsigned char*)&ii, gBusVerInfo.busBLKVer, 2);
	
	for(i=0; i<16; i++){
#ifdef _debug_blk
		debugstring("BIT_BLACK_2BLOK:::");
		debugdata((unsigned char*)&gBlackBlockNum[i],2,1); //黑名单总数
#endif
		if(gBlackBlockNum[i] == 0xffff)
			gBlackBlockNum[i] = 0;
		if(gBlackBlockNum[i] > BLACKLEN)		//16块，
		{
			gBlackBlockNum[i] = 0;
			ii = 0;
		}
	}

#ifdef _debug_blk
	for(i=0; i<16; i++){
		if(gBlackBlockNum[i] == 0)
			break;
		memset(buffer,0,4);
		debugdata(&i,1,0);
		debugstring("------");
		debugdata((unsigned char *)&gBlackBlockNum[i],2,0);
		debugstring("------");
		debugstring("\r\n");
		for (j=0;j<gBlackBlockNum[i];j++)
		{
			if(i == 0)
				addr = FLASH_BLK_START+(i*4096)+14+j*BLK_SNO_LEN;//最前14字节是版本号,红外的也加入了
			else
				addr = FLASH_BLK_START+(i*4096)+j*BLK_SNO_LEN;//
			flashread(addr, buffer, BLK_SNO_LEN);
			debugdata(buffer,BLK_SNO_LEN,1);
			if (j%300 == 0)
			{
				clr_wdt();
			}
		}
	}
#endif


	if((ii == 0)||(ii == 0xffff)||(pFistVary.BlackListNum > BLACKSUM)){//黑名单版本不正确，条数不正确，清0重新下载
		delallBlacInbus();
		pFistVary.BlackListNum = 0;
	}
	
	if(pFistVary.BlackListNum > 0){
		if(gBlackBlockNum[0] == 0){
			ii = pFistVary.BlackListNum/BLACKLEN;	//16块
			for(i=0; i<ii; i++){
				gBlackBlockNum[i] = BLACKLEN;
			}
			sysfewrite(BIT_BLACK_2BLOK, 32, (unsigned char*)&gBlackBlockNum[0]);
			gBlackBlockNum[i] = pFistVary.BlackListNum%(BLACKLEN);
		}
	}

	addr+=14;
//	temp = pFistVary.BlackListNum*4;//黑名单数据
	BlackLength = pFistVary.BlackListNum;
#ifdef _debug_blk
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

	memset((unsigned char*)&gsl8583FileDownPara, 0, sizeof(st8583filedown));//下载信息不正确，初始化
	sysfewrite(BIT_8583filedown, sizeof(st8583filedown), (unsigned char*)&gsl8583FileDownPara);//取上次下载参数据,用于续传
}


void loadGPRSFE(void)
{
#ifdef CPU_JSB
	unsigned int j=0;
#endif

	sysferead(BIT_TCPIP_PACKET, sizeof(TCP_IP_PACKET1), (unsigned char*)&tcpipPacket.packetStatus);
	
	memset(gBusVerInfo.busLineVer, 0, 2);
	memset(gBusVerInfo.busVoiceVer, 0, 2);
	
	getProVer();
	
	sysferead(BIT_8583filedown, sizeof(st8583filedown), (unsigned char*)&gsl8583FileDownPara);//取上次下载参数,用于续传
#ifdef debug_GJ_TLVDeal_
	debugstring("gsl8583FileDownPara:");
	debugdata((unsigned char*)&gsl8583FileDownPara, sizeof(st8583filedown), 1);
#endif

	load_black_list();

#ifdef CPU_JSB
	j= load_white_list();
#ifdef _debug_
	debugstring("白名单数:");
	debugdata((unsigned char *)&j,2,1);
	
#endif
#endif

#ifdef qPBOC_BUS
	WHT_BANK_Load();
#endif



	memset(gBusVerInfo.newbusBLKVer, 0, 2);
	memset(gBusVerInfo.newbusLineVer, 0, 2);
	memset(gBusVerInfo.newbusProVer, 0, 2);
	memset(gBusVerInfo.newbusVoiceVer, 0, 2);
	memset(gBusTicketV.newbusticketVer, 0, 2);
	
	gprsIndex = 0;
	Sendpincount = 0;
	gSendGLogin = 0;
	gsendError = 0;
	gGPRS_Cmd_outtime = 0;

	initsl8583para();
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
#ifdef _debug_
	debugstring("写黑名单完毕\r\n");
	debugdata(gBusVerInfo.busBLKVer, 2, 1);
	MSG_LOG("刷新后的黑名数:%d\r\n",BlackLength);
	
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
	unsigned long long ulTempBlackNo;
	unsigned long long ulTempCardNo;
	unsigned int i;

//复制黑名单到中
	memcpy((unsigned char *)&ulTempCardNo,ulCardNo,BLK_SNO_LEN);

	i_end =blkLen;
	
	if (blkLen ==0) 
	{
		memset(blkBuf,0,4);
	}
#ifdef _debug_
	MSG_LOG("增BlackLength:%d\r\n",BlackLength);
#endif

	if(BlackLength >= BLACKSUM)
		return 0;
#ifdef _debug_
	debugstring("增加:");
	debugdata(ulCardNo, BLK_SNO_LEN, 1);
#endif
	if(blkLen == 0){
		memcpy(blkBuf,(unsigned char *)&ulTempCardNo,4);
		return 1;
	}

	while(1)
	{
		clr_wdt();
		i_cur=i_start+(i_end-i_start)/2;
		invalid_point=i_cur*BLK_SNO_LEN;
		memcpy((unsigned char *)&ulTempBlackNo,blkBuf+invalid_point,BLK_SNO_LEN);
		
// #ifdef _debug_
// 	debugstring("invalid_point:");
// 	debugdata((unsigned char *)&invalid_point,2,1);
// 	debugstring("i_cur:");
// 	debugdata((unsigned char *)&i_cur,2,1);
// 	debugdata((unsigned char *)&ulTempBlackNo,8,1);
// #endif
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
// #ifdef _debug_
// 	debugstring("invalid_point:");
// 	debugdata((unsigned char *)&invalid_point,2,1);
// 	debugstring("i_start:");
// 	debugdata((unsigned char *)&i_start,2,1);
// 	debugstring("i_cur:");
// 	debugdata((unsigned char *)&i_cur,2,1);
// 	debugstring("i_end:");
// 	debugdata((unsigned char *)&i_end,2,1);
// 	debugdata((unsigned char *)&ulTempBlackNo,8,1);
// #endif
	if (flag ==1) 
	{
#ifdef _debug_
		debugstring("移动黑名单 ");
#endif			
		if (ulTempCardNo < ulTempBlackNo) //38.39
		{
#ifdef _debug_
			debugstring("===1\r\n");
			debugdata((unsigned char*)&i, 4, 1);
#endif
		}
		else if(ulTempCardNo > ulTempBlackNo)//38<39
		{
#ifdef _debug_
			debugstring("===2\r\n");
#endif			
			i_cur++;
		}
		else{
			return 0;
		}
		i = 4096-(i_cur+1)*BLK_SNO_LEN;
		memmove(blkBuf+(i_cur+1)*BLK_SNO_LEN,blkBuf+i_cur*BLK_SNO_LEN,i);
		memcpy(blkBuf+i_cur*BLK_SNO_LEN,(unsigned char *)&ulTempCardNo,BLK_SNO_LEN);
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
#ifdef debug_GJ_TLVDeal_
	char disbuff[64];
#endif

	addr = FLASH_TEMP_START;
	while(addr < FLASH_TEMP_END){
		flashread(addr, buff, 14);
#ifdef debug_GJ_TLVDeal_
		debugstring("临时黑名单:");
		debugdata(buff, 14, 1);
#endif
		memset(buffT, 0xff, 20);
		if(memcmp(buff, buffT, 14) == 0){//到FFFFFF了，结束
#ifdef debug_GJ_TLVDeal_
			debugstring("临时区找完了:::\r\n");
#endif
			return ST_OK;
		}
		
		clr_wdt();
		blkTemp = (stAddBLKTEMP *)buff;

		memset(buffT,0,sizeof(buffT));
		if(memcmp(blkTemp->index, buffT, 14) == 0){//14字节都为0 说明这个位置原来是一个减量，然后处理了，清零了。现在是看地址就行
		//全0 ^的结果也是0，增量的时候会误将这一条当成一个正常的处理了，这里也加上判断，直接跳过
#ifdef _debug_blk
			debugstring("刚处理了---==\r\n");
#endif
			addr += 14;
			continue;
		}
		ucBcc = 0;
		for(i=0;i<13;i++)
			ucBcc ^= buff[i];
		if(ucBcc != blkTemp->BCC){
#ifdef debug_GJ_TLVDeal_
			debugstring("临时黑名单BCC错 ,删除\r\n");
#endif
			flashwrite(addr,buffT,14);	//清除
			addr += 14;
			continue;
		}
		if((blkTemp->DorI == BLK_ADD)){
#ifdef debug_GJ_TLVDeal_
			debugstring("不是减,继续下一个;;;\r\n");
#endif
			addr += 14;
			continue;
		}
		uit = 0;
		memcpy((unsigned char*)&uit, blkTemp->index, 3); // 判断块号
		if((uit >= (FLASH_BLK_START+((block) * 4096))) &&(uit < (FLASH_BLK_START+((block+1) * 4096)))){//在此块的范围内
//			uit = uit - FLASH_BLK_START;
#ifdef debug_GJ_TLVDeal_
			memset(disbuff,0,sizeof(disbuff));
			sprintf(disbuff,"此减量名单在:::%d块\r\n",block);
			debugstring(disbuff);
#endif
			if(block == 0)
				uit = BinSCH(gBlackBlockNum[block], blkBuff+14, blkTemp->CardNo, BLK_SNO_LEN);//第一块前14字节为版本号
			else
				uit = BinSCH(gBlackBlockNum[block], blkBuff, blkTemp->CardNo, BLK_SNO_LEN);
#ifdef debug_GJ_TLVDeal_
// 			MSG_LOG("原上下三个:::");
// 			BCD_LOG(blkBuff+uit,4,1);
// 			BCD_LOG(blkBuff+uit+14,4,1);
			debugstring("删除黑名单1:");
			debugdata((unsigned char*)&uit, 4, 1);
			debugdata((unsigned char*)&gBlackBlockNum[block], 2, 1);
			
#endif
			if(uit == 0xffffffff){
				MSG_LOG("居然找不到!绝对有问题，是不是要删掉？!\r\n");
				addr += 14;
				continue;
			}
			if (block == 0)
			{
				memmove(blkBuff+14+uit, blkBuff+14+uit+BLK_SNO_LEN, ((gBlackBlockNum[block]*BLK_SNO_LEN)-uit));//2B版版号，跳过14字节
			}else{
				memmove(blkBuff+uit, blkBuff+uit+BLK_SNO_LEN, ((gBlackBlockNum[block]*BLK_SNO_LEN)-uit));//???????
			}
			if(gBlackBlockNum[block] > 0)
				gBlackBlockNum[block]--;
			if(BlackLength > 0)
				BlackLength--;
			memset(buffT, 0, 14);
			flashwrite(addr, buffT, 14);
#ifdef debug_GJ_TLVDeal_
			
			debugstring("删除一条##############\r\n");
			debugstring("删除黑名单2:");
			debugdata((unsigned char*)&uit, 4, 1);
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
	
#ifdef debug_GJ_TLVDeal_
	debugstring("给这块增量:::\r\n");
#endif
	addr = FLASH_TEMP_START;
	while(addr < FLASH_TEMP_END){
		flashread(addr, buff, 14);
#ifdef debug_GJ_TLVDeal_
		debugstring("临时黑名单:");
		debugdata((unsigned char*)&addr, 4, 0);
		debugstring("||");
		debugdata(buff, 14, 1);
#endif
		clr_wdt();


		blkTemp = (stAddBLKTEMP *)buff;

		memset(buffT, 0xff, 20);
		if(memcmp(buff, buffT, 14) == 0){//到FFFFFF了，结束
			return 0;
		}

		memset(buffT,0,sizeof(buffT));
		if(memcmp(blkTemp->index, buffT, 14) == 0){//14字节都为0 说明这个位置原来是一个减量，然后处理了，清零了。现在是看地址就行
			//全0 ^的结果也是0，增量的时候会误将这一条当成一个正常的处理了，这里也加上判断，直接跳过
#ifdef _debug_blk
			debugstring("刚处理了---==\r\n");
#endif
			addr += 14;
			continue;
		}

		
		ucBcc = 0;
		for(i=0;i<13;i++)
			ucBcc ^= buff[i];
// #ifdef _debug_blk
// 		debugstring("ucBcc:");
// 		debugdata(&ucBcc,1,1);
// 		debugstring("buff[13]:");
// 		debugdata(&buff[13],1,1);
// #endif
		if(ucBcc != blkTemp->BCC){
#ifdef debug_GJ_TLVDeal_
			debugstring("临时黑名单BCC错 ,删除\r\n");
#endif
			flashwrite(addr,buffT,14);	//清除
			addr += 14;
			continue;
		}
		if((blkTemp->DorI == BLK_DEC)){
#ifdef debug_GJ_TLVDeal_
			debugstring("不是增,继续下一个;;;\r\n");
#endif
			addr += 14;
			continue;
		}
 		if(gBlackBlockNum[block] < BLACKLEN){//此块有空间，可以存入

			#ifdef debug_GJ_TLVDeal_
			debugstring("此块有空间，可以存入:::\r\n");
#endif
			if(block == 0){
				if(BlackUpdataAddOrDel(blkTemp->CardNo, gBlackBlockNum[block], blkBuff+14) == 1){
					gBlackBlockNum[block]++;
					BlackLength++;
#ifdef _debug_blk
					debugstring("插入一条##############\r\n");
#endif
				}
			}
			else{
				if(BlackUpdataAddOrDel(blkTemp->CardNo, gBlackBlockNum[block], blkBuff) == 1){
					gBlackBlockNum[block]++;
					BlackLength++;
#ifdef _debug_blk
					debugstring("插入一条##############\r\n");
#endif
				}
			}
			memset(buffT, 0, 14);
			flashwrite(addr, buffT, 14);
#ifdef _debug_blk
			debugstring("addr:::++++14:##############\r\n");
#endif
			addr +=14;
		}
		else{
#ifdef _debug_blk
			debugstring("此块已经存满\r\n");
#endif
			return 1;//此块已经存满
		}
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


	
#ifdef _debug_blk
	unsigned char buff[15];
	stAddBLKTEMP *blkTemp;
	unsigned int uitemp;
#endif


	
#ifdef _debug_blk
//	unsigned int uitemp =0;
	char disbuff[64];
	debugstring("增减量处理:::\r\n");
#endif

#ifdef _debug_blk
	addr = FLASH_TEMP_START;
	while(addr<FLASH_TEMP_END){
		clr_wdt();
		flashread(addr, buff, 14);
		blkTemp = (stAddBLKTEMP *)buff;
		uitemp = 0;
		memcpy(&uitemp ,blkTemp->index,3); 
		uitemp = (uitemp -FLASH_BLK_START)/4096;
		memset(disbuff,0,sizeof(disbuff));
		if (blkTemp->DorI == BLK_DEC)
		{
			sprintf(disbuff,"BLOCK:%02d||减%d||||",uitemp, blkTemp->DorI);
		}else{
			sprintf(disbuff,"BLOCK:%02d||增%d||||",uitemp, blkTemp->DorI);
		}
		BCD2Ascii(blkTemp->CardNo,(unsigned char *)disbuff+16,8);
		debugstring(disbuff);
		debugstring("\r\n");
		memset(blkTempbuff,0xff,sizeof(stAddBLKTEMP));
		if(memcmp(buff, blkTempbuff, sizeof(stAddBLKTEMP)) == 0)
			break;	//停止
		addr += 14;
		}
#endif



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
#ifdef _debug_blk
		memset(disbuff,0,sizeof(disbuff));
		sprintf(disbuff,"gBlackBlockNum[%d]=%d\r\n",block,gBlackBlockNum[block]);
		debugstring(disbuff);
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
			debugstring("处理减量111::::\r\n");
#endif
			DealGPrsBLOckDEC(block, blkTempbuff);//在临时表中，处理这块中有的卡号，将这个卡号删除
#ifdef debug_GJ_TLVDeal_
			debugstring("Temp BLOckDEC OK \r\n");
#endif
		}
		
		if(gBlackBlockNum[block]< BLACKLEN){//有剩余空间.处理增加的黑名单BLACKLEN/16
			
#ifdef debug_GJ_TLVDeal_
			debugstring("这一块有空间了::\r\n");
			debugstring("处理增量:111:::\r\n");
#endif
			if(addflag){//如果还有
#ifdef _debug_blk
				debugstring("还有数据::");
				memset(disbuff,0,sizeof(disbuff));
				sprintf(disbuff,"block:%d,%d条::\r\n",block,gBlackBlockNum[block]);
				debugstring(disbuff);
// 				debugstring("Temp Deal Black 2 ");
// 				debugdata((unsigned char*)&block, 1, 1);
#endif
				addflag = DealGPrsBLOckADD(block, blkTempbuff);
#ifdef _debug_blk
				if(addflag==1)debugstring("后续还有!!!\r\n");
#endif
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
		
		sysfewrite(BIT_BLACK_2BLOK, sizeof(gBlackBlockNum), (unsigned char*)&gBlackBlockNum[0]);
		sysfewrite(BIT_BLACK_SUM,4,(unsigned char*)&pFistVary.BlackListNum); //处理完一个包，保存黑名单条数
		
	}
#ifdef debug_GJ_TLVDeal_
	debugstring("Temp Deal Black 4 ");
#endif
	return ST_OK;
}
//mode=2全量下载，其它增减量下载,处理接收到的黑名单，全量下载直接放到黑名单区，增减量放到临时区
void DealGPRSBlack(unsigned char mode, unsigned int len, unsigned char *dat)
{
	unsigned int addr;
	unsigned int i,itemp=0;
	unsigned char buff[20];
	unsigned char buffT[20];
	unsigned char cardNo[20];
	stAddBLKTEMP *blacktemp;
#ifdef _debug_blk
	char disbuff[50];
#endif

	if((mode == 2)){//全量下载 直接写入
#ifdef _debug_blk
		debugstring("deal GPRS black  ALLL\r\n");
//		debugdata(dat,len*10,1);
#endif
		sysferead(BIT_BLACK_SUM,4,(unsigned char*)&pFistVary.BlackListNum); //黑名单条数
		sysferead(BIT_BLACK_2BLOK, 32, (unsigned char*)&gBlackBlockNum[0]);
		for(i=0;i<16;i++)
		{
			if(gBlackBlockNum[i] > BLACKLEN)		//原1000，现在折半
				gBlackBlockNum[i] = 0;
		}
		BlackLength = pFistVary.BlackListNum;
		itemp = BlackLength/BLACKLEN; //到了哪一块了

		if(itemp>=16)
			return;

		if(itemp == 0)
			addr = FLASH_BLK_START+14+(itemp*4096)+(gBlackBlockNum[itemp]*BLK_SNO_LEN); // 需要写入黑名单的地址 前14字节是版本号
		else
			addr = FLASH_BLK_START+(itemp*4096)+(gBlackBlockNum[itemp]*BLK_SNO_LEN); // 需要写入黑名单的地址
		memset(buff, 0, 20);
		MSG_LOG("几条:%d\r\n",len);
		for(i=0; i<len; i++){//len 要处理的黑名单条数
			clr_wdt();
			if(memcmp(dat+(i*10), buff, BLK_SNO_LEN) == 0){//保存黑名单  统一10字节 用8字节
				
				break;	//如果全是0，则认为已经没有黑名单了
			}
			else{
				flashwrite(addr, dat+(i*10), BLK_SNO_LEN);//保存黑名单  统一10字节 用8字节
				BlackLength++;
				pFistVary.BlackListNum=BlackLength;
				itemp = (BlackLength-1)/BLACKLEN;

				if(itemp < 16)
					gBlackBlockNum[itemp] = (((BlackLength-1)%(BLACKLEN))+1);

				if(  (gBlackBlockNum[0]!=0) && ((BlackLength%BLACKLEN)== 0))	// 500  
				{
					MSG_LOG("写下一块:%d+1,len:%d\r\n",itemp+1,gBlackBlockNum[itemp]);

					addr = FLASH_BLK_START+((itemp+1)*4096)+(gBlackBlockNum[(itemp+1)]*BLK_SNO_LEN);	//下一块的首地址 pfquan
				}else{
					addr+=BLK_SNO_LEN;

				}


			}
		}
#ifdef _debug_blk
		memset(disbuff,0,sizeof(disbuff));
		sprintf(disbuff,"统计条数:%d\r\n",BlackLength);
		debugstring(disbuff);
		debugstring("gBlackBlockNum:");
		for (i=0;i<16;i++)
		{
			MSG_LOG("%d ",gBlackBlockNum[i]);
		}
		debugstring("\r\n");
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
			memset(buff, 0, sizeof(buff));
			if(memcmp(dat+(i*10), buff, 8) == 0){
				sysfewrite(BIT_BLACK_SUM,4,(unsigned char*)&pFistVary.BlackListNum);
				return;	//如果全是0，则认为已经没有黑名单了
			}
// #ifdef _debug_blk
// 			debugstring("addr:");
// 			debugdata((unsigned char*)&addr, 4, 1);
// #endif
			clr_wdt();
			memcpy(cardNo, dat+(i*10), 10);//每条黑名单10节字组成，最后个字节 0删 1增
			itemp = IsInBlackList(cardNo);
#ifdef _debug_blk
			debugstring("cardNo:");
			debugdata(cardNo, 10, 1);
			debugdata((unsigned char*)&itemp, 4,1);
#endif
			memset(buff,0xff,14);
			blacktemp = (stAddBLKTEMP *)buff;
			if(itemp < FLASH_BLK_END){//已经存在黑名单中
				if(cardNo[9] == BLK_DEC){//减
					MSG_LOG("减_");
					blacktemp->DorI = BLK_DEC;
					memcpy(blacktemp->CardNo, cardNo, BLK_SNO_LEN);
				}//增加就不理
			}
			else{//不存在黑名单中,只处理增
				if(cardNo[9] == BLK_ADD){//增
					MSG_LOG("增_");
					blacktemp->DorI = BLK_ADD;
					memcpy(buff+4, cardNo, BLK_SNO_LEN);
				}//增加就不理
			}

			memcpy(blacktemp->index, (unsigned char*)&itemp, 3);
			blacktemp->reserved[1] = 0xFF;
			
			for (itemp=0; itemp<13; itemp++)
			{
				blacktemp->BCC ^= buff[itemp];
			}
			flashwrite(addr, buff, 14);//写入
			addr += 14;
#ifdef _debug_blk
			MSG_LOG("写buff:");
			debugdata(buff, 14, 1);;
#endif

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
