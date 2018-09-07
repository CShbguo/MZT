
#include "string.h"
#include "stdio.h"
#include "..\include\include.h"
#include "Gszct.h"
#include "DEF8583.h"

#include "szct.h"
//#include "QRZFB.h"
//#define _debug_gprs
//#define _debug_time	//
#include "ProcCharacter.h"
#include "qpboc_8583.h"
//V3 : 加入4G模块
#define SOFT_VAL 3	//库版本

//固定位置为二进制文件的0x1000处，CPU的位置内在应用程序的起始位置加上0x1000.
char  const  MachineCode[64]  __attribute__ ((at(0x8010000+0x1000))) = "二维码车载机ESF3000L\x00";


#define GPRSCOM_BAUD 230400

#define CP_head  0
#define CP_len 4
#define CP_cmd 6
#define CP_sta 7
#define CP_dat 8
#define CP_minlen 10

typedef struct
{
	unsigned char styleEA;		//有数据时此值置为0xEA，处理完后置0
	unsigned char dflag;	//数据标识：1=网络状态 	2=文件下载状态 3=文件参数
	unsigned short datalen;
	unsigned char data[300];
}stEC20Style;
stEC20Style gEC20Style;

unsigned char GPRS_ICCID[20];
extern unsigned int str2int(unsigned char *pstr, unsigned char len);
extern unsigned char checkIP(char *IPb);

extern unsigned char Xor8(const unsigned char *pData, unsigned long DataLen);
extern void XFsoud(unsigned char num);
extern unsigned char StufferNO[4];
extern unsigned char DriveCardNo[4];

extern void setSendTimeOut(int link, int time);
extern int calFileCRC(unsigned int fileName, unsigned int fileLen);
extern void gprs_send_data_OTHER_CMD(unsigned char cmd, unsigned int len, unsigned char *dat);
extern void savesysbupara(void);
extern unsigned int BuildGJPackge(unsigned char *revBuf, unsigned char mode);
extern unsigned char SPI_Snd_dat(unsigned short len, unsigned char *dat, unsigned char cmd);
extern unsigned char GetDateTime(void);
//extern unsigned char GprsSend(unsigned short serialno,unsigned char cmd,unsigned short len,unsigned char *buff);
extern unsigned char DealStationBTM(unsigned int len, unsigned char *info);
extern void UPData_Line_Info_gprs(unsigned char *info);
extern void uploadMobilePara(void);
extern void first_clr_mobile_para(void);
extern unsigned char GJDataDeal(unsigned char *pakege);
extern void getMobilePara(void);
extern void stopCCgprs(unsigned short mode);
extern void find_G_new_mission(void);
extern void KeyPad_restordis(void);
extern void in_deal_ring(void);
extern int in_dealSoundIn(void);
extern int in_checkRingstyle(void);
extern int in_sendGPs(void);
extern void GPRS_MC55i_open(void);
extern void GPRS_open(void);
extern unsigned char in_getGprsModeStyle(void);
extern void in_dispGprsProcess(void);

extern void sendmsgHead(void);
extern void sendmsgBody(void);
extern void SendmsgEnd(void);

extern void in_gErrortimes(unsigned char val, unsigned char mode);
extern void in_decgM_Senddll(void);
extern void in_IF2TLVDeal(unsigned char sindex);
extern void in_IF18583Deal(unsigned char sindex);
extern void set_KEK_SN(unsigned char val);
extern int checkKEK(void);

extern void powerOff4V0(void);
extern void powerON4V0(unsigned int baund);

void gprs_send_CMD(unsigned char cmd, unsigned int len, unsigned char *dat);

extern unsigned char gmissOldflag;
extern volatile unsigned char gmissflag;//任务标志 0x00 登陆移动,签到.

extern stbusdownPara gBusblkDownPro;//黑名单下载
extern stbusdownpro gBusDownPro;
extern stGPSdownLine gGPSDownLine;
extern pFistVary_1 pFistVary;
extern stcomtime SysTime;
extern unsigned int a_sum1, s_sum1;
extern stBusVerIfo gBusVerInfo;
extern unsigned short gErrorFlag;//错误标识,高字节表示错误编号，低字节表示错次数据 详见"*错误编码标识定义"
extern unsigned int gErrortimes[2];//错误次数据,主要是用于GPRS连接

extern unsigned char gchn_eng;//中英文标志
extern volatile unsigned char gpacSNo;//包序号
//GPRS主板状态
volatile unsigned char gAPN_Style;//APN状态，是专用网还是公网
unsigned char gGPSdly;//GPS延时
unsigned short gSendOverTime;//发送后收应答超时计数器
unsigned char omissDly;
extern unsigned int gSendGLogin;//登陆命令 用这个查询信号

TCP_IP_PACKET1 tcpipPacket;//500
volatile unsigned char GPRSLinkProcess;
//gprs的中断信息 每次拨号前复位	
PACKET_RCV gPacketRcvBuf[MAX_RCV_PACKET_NUM];	//接收帧缓冲	//发送队列缓冲

volatile unsigned int CDMA_REV_LEN;//需要接收数据的长度,如果是0xffffffff为没有数据需要接收。

unsigned char gTcpIpbusy;//模块是正正在发数据，模块工作是发命令，然后回应后再发数据。在等回应时不能发其它数据。
volatile unsigned int guiMC323_REV_LEN;//需要接收数据的长度,如果是0xffffffff为没有数据需要接收。
unsigned char gTcpIpbusydly;//收不到空闲延时处理
unsigned char tcpipsdnode;
stTcpIpSp gTcpIPsd[4];//发送暂存，不能超过256字节一个包
volatile stGPrsDataStyle gGPRS_data_style;


#define GPRSCOMMAX 1600
unsigned char gprsRevBuf[GPRSCOMMAX];//GPRS接收缓冲
volatile unsigned short gprsIndex;

#define GPRS_CLEAR_INDEX	0x55	//处理后可以清理缓冲区
#define GPRS_DATA_OK		0xaa	//数据准备好
#define GPRS_GOON			0		//继续收数据
volatile unsigned char gprsReady = GPRS_GOON;//收到GPRS回传的数据,需要处理.

unsigned int ClientIP;
unsigned short ClientPort;

unsigned char gprsSingle;//GPRS信号强度
unsigned char gprsAnswer;//ＧＰＲＳ模块回应ＯＫ标志
unsigned char gprsSendDlyflag;//模块发送是否延时标志
unsigned char isNetOK[5];//两路链接是否成功

unsigned char gprsErrorTimes;//出现ＥＲＲＯＲ返回错误次数
unsigned char gprsSNDerro;//和后台通讯出现的错误
unsigned char gTTime[14];//保存中心发下来的日期时间
unsigned char gGPRS_Cmd_outtime;//GPRS包超时计数
unsigned char GprsCardList[16];


#define  ChinaMobile   1
#define  ChinaUnicom   2
#define  ChinaTelecom  3 
unsigned char ugISMI_type;	///全网通时指示当前卡的运营商

extern unsigned char restore_flag;

int gprs_send_CMD_WaitACK(unsigned char cmd, unsigned int len, unsigned char *dat, unsigned char *odat);
void GPRS_close_Gt900(unsigned int dlyt)
{
;	
}
unsigned short BCC(unsigned char *Bcc_pointer, unsigned short x)					//生成BCC校验码
{
	unsigned char data;
	unsigned char k, h;
	unsigned char BCC_L = 0;
	unsigned char BCC_H = 0;
	unsigned short  BCC = 0;
	unsigned short i, j;

	for (i = 0; i < x; i++)
	{
		data = *(Bcc_pointer + i);
		for (j = 0; j < 8; j++)
		{
			if (BCC_H & 0x01) k = 1;
			else k = 0;
			BCC_H = BCC_H >> 1;                 //保存高字节。
			if (BCC_L & 0x01) h = 1;
			else h = 0;
			BCC_L = BCC_L >> 1;
			if (k) BCC_L = BCC_L | 0x80;
			else BCC_L = BCC_L & 0X7f;
			if (h) BCC_H = BCC_H | 0x80;
			else BCC_H = BCC_H & 0x7f;
			if (data & 0x01) h = 1;
			else h = 0;
			data = data >> 1;
			if (h)
			{
				BCC_H = BCC_H ^ 0x80;
				goto bcc_1;
			}
			else
			{
			bcc_1:
				if (BCC_H & 0x80)
				{
					BCC_H = BCC_H ^ 0x04;
					BCC_L = BCC_L ^ 0x08;
				}
			}
		}
	}
	BCC = 0;
	BCC = BCC | BCC_H;
	BCC = BCC << 8;
	BCC = BCC | BCC_L;
	return(BCC);
}

void gprs_snd(unsigned int len, char *dat)
{
	debugstring("GPRS SEND...此函数在这里没用了..\r\n");
}

extern unsigned short getCpuPort(void);
extern unsigned int getCpuIPaddr(void);

//发送IP及端口,模块收到后会重新建立连接
int buildSeverInfo_06(char cmd, char link)
{
	unsigned char sendbuf[128];
	unsigned char bu[8];
	unsigned int pos = 0;
	unsigned short itmp;
	unsigned int ip = 0;
	unsigned int port = 0;
	unsigned char posLinkCount = 0;

	MSG_LOG("buildSeverInfo_06 start:%d,%d\n", cmd, link);

	// 通讯协议格式版本
	memcpy(sendbuf + pos, "\x01\x01", 2);
	pos += 2;
	//加入设备信息
	memcpy(sendbuf + pos, "BUS", 3);
	pos += 3;
	memcpy(sendbuf + pos, pFistVary.DeviceNo, 8);
	pos += 8;
	Get_SerialNumF4(sendbuf + pos);
	pos += 4;
	memcpy(sendbuf + pos, DriveCardNo, 4);
	pos += 4;
	memcpy(sendbuf + pos, pFistVary.LineNo, 3);
	pos += 3;
	sendbuf[pos++] = 0;	//控制，第0位=1时开启GPS

	posLinkCount = pos;
	sendbuf[pos++] = 0;	//链接的条数。取值为1到4
	memset(sendbuf + pos, 0, 18);
	strcpy((char*)sendbuf + pos, "CMNET"); //APN1	18	ASCII码，后补0
	pos += 18;
	memset(sendbuf + pos, 0, 18);
	strcpy((char*)sendbuf + pos, "NO APN2"); //APN2	18	ASCII码，后补0
	pos += 18;
	memcpy(sendbuf + pos, gBusVerInfo.busProVer, 2);	//程序版本号
	pos += 0x02;
	itmp = 60;
	memcpy(sendbuf + pos, (unsigned char*)&itmp, 2);//重连时间	2	HEX低位在前，单位秒。模块没收到平台数据后重连。0xFFFF不自动重连。
	pos += 2;

	if (link == LINK_GJ || link == 5) {
		++sendbuf[posLinkCount];

		sendbuf[pos++] = LINK_GJ + 1;	//链接1编号	1	从1到4取值
		memset(sendbuf + pos, 0, 15);	//链接1IP地址	15	ASCII码，后补0，如：192.168.0.11为3139322E3136382E302E3131000000
		memcpy(bu, (unsigned char*)&ClientIP, 4);
		sprintf((char*)sendbuf + pos, "%d.%d.%d.%d", bu[0], bu[1], bu[2], bu[3]);
		pos += 15;
		memcpy(sendbuf + pos, (unsigned char*)&ClientPort, 2);
		pos += 2;
		// 链接1属性控制
		// HEX，共8位，每位为1控制有效。0位：HTTPS，1位为0用APN1，1位为1用APN2.
		sendbuf[pos] = 0x00;
		++pos;
	}
	//MSG_LOG("buildSeverInfo_06 start3:%d\n", pos);

	if (link == LINK_PBOC || link == 5) {
		++sendbuf[posLinkCount];
		MSG_LOG("buildSeverInfo_06 start11:%d\n", ip);

		// 链接2
		getMobileParameter(3, (unsigned char *)&ip);
		//MSG_LOG("buildSeverInfo_06 start12:%d\n", link);

		//MSG_LOG("buildSeverInfo_06 start1:%d\n", ip);
		getMobileParameter(4, (unsigned char *)&port);

		//MSG_LOG("buildSeverInfo_06 start2:%d\n", port);
		sendbuf[pos++] = LINK_PBOC + 1;	//链接2编号	1	从1到4取值
		memset(sendbuf + pos, 0, 15);	//链接1IP地址	15	ASCII码，后补0，如：192.168.0.11为3139322E3136382E302E3131000000
		memcpy(bu, (unsigned char*)&ip, 4);
		sprintf((char*)sendbuf + pos, "%d.%d.%d.%d", bu[0], bu[1], bu[2], bu[3]);
		pos += 15;
		//MSG_LOG("buildSeverInfo_06 start6:%d\n", port);
		memcpy(sendbuf + pos, (unsigned char*)&port, 2);
		pos += 2;
		//MSG_LOG("buildSeverInfo_06 start7:%d\n", link);
		// 链接1属性控制
		// HEX，共8位，每位为1控制有效。0位：HTTPS; 1位为0用APN1，1位为1用APN2; 2位为0代码有HTTP头,2位为1代表无HTTP头,3位为1代表银联SOCKET
		sendbuf[pos] = 0x0D;  //0000 1101    0000 0101
		++pos;
	}

	//MSG_LOG("buildSeverInfo_06:");
	//debugdata(sendbuf, pos, 1);

	gprs_send_CMD(cmd, pos, sendbuf);

	return pos;
}

int buildFileDown_HTTP_07(unsigned char *fflag, unsigned char *fflag2)
{
	unsigned char sndbuf[128];
	unsigned int pos = 0;

	//链接号1字节+2字节商户号+3字节文件标识+2字节需要下载的版本号
	sndbuf[pos++] = 1;
	memcpy(sndbuf + pos, CLOUD_BUSSINESSNO, 2);
	pos += 2;
	memcpy(sndbuf + pos, fflag, 3);	//文件标识主标识+从标识
	pos += 3;
	memcpy(sndbuf + pos, fflag2, 3);	//文件标识从标识
	pos += 3;
	memcpy(sndbuf + pos, gsl8583FileDownPara.Miss_ver, 2);
	pos += 2;

	gprs_send_CMD(7, pos, sndbuf);

	return pos;
}

//发送数据到模块 数据类型1字节【1=刷卡记录、2=当前记录文件、3=历史记录】+文件长度(4字节）+偏移量（4字节）+数据长度（4字节）+数据（L)+文件名的中间部分。
int buildDataSend_0B(unsigned char mode, unsigned int offset, unsigned int Alen, unsigned int datLen, unsigned char *data, unsigned char *obuf, unsigned char *tmb)
{
	unsigned char sndbuf[2048];
	unsigned int pos = 0;

	sndbuf[pos++] = mode;
	memcpy(sndbuf + pos, (unsigned char *)&Alen, 4);
	pos += 4;
	memcpy(sndbuf + pos, (unsigned char *)&offset, 4);
	pos += 4;
	memcpy(sndbuf + pos, (unsigned char *)&datLen, 4);
	pos += 4;
	memcpy(sndbuf + pos, data, datLen);
	pos += datLen;
	memcpy(sndbuf + pos, pFistVary.DeviceNo, 8);	//后面发送设备号加日期时间，作为本数据文件名的一部分,LINUX模块中当前只有当前记录和备份(历史)记录有效。
	pos += 8;
	memcpy(sndbuf + pos, tmb, 14);
	pos += 14;
	sndbuf[pos++] = 0;	//最后发结束符

	return gprs_send_CMD_WaitACK(0x0B, pos, sndbuf, obuf);
}

int BuildHTTPPackge(unsigned char *revBuf, unsigned char mode)
{
	int relen = 0;

	switch (mode)
	{
	case MISS_HTTP_BLK:
		MSG_LOG("发送模块下载文件指令.Ver:%02X%02X\r\n", gsl8583FileDownPara.Miss_ver[0], gsl8583FileDownPara.Miss_ver[1]);
		relen = buildFileDown_HTTP_07(SL8583FileFLAG_BLK, "BUS");


		GPRSLinkProcess = GPRS_SENDING_CMD;
		setSendTimeOut(LINK_GJ, DE_SendOverTime);
		break;
	case MISS_HTTP_EC20:
		MSG_LOG("发送模块下载文件指令.Ver:%02X%02X\r\n", gsl8583FileDownPara.Miss_ver[0], gsl8583FileDownPara.Miss_ver[1]);
		relen = buildFileDown_HTTP_07(SL8583FileFLAG_EC20, POS_EC20_FLAG);

		GPRSLinkProcess = GPRS_SENDING_CMD;
		setSendTimeOut(LINK_GJ, DE_SendOverTime);
		break;
	case MISS_HTTP_PRO:
		MSG_LOG("发送模块下载文件指令.Ver:%02X%02X\r\n", gsl8583FileDownPara.Miss_ver[0], gsl8583FileDownPara.Miss_ver[1]);
		relen = buildFileDown_HTTP_07(SL8583FileFLAG_PRO, POS_Cand_FLAG);

		GPRSLinkProcess = GPRS_SENDING_CMD;
		setSendTimeOut(LINK_GJ, DE_SendOverTime);
		break;
	default:
		break;
	}
	return relen;
}



void initTCPIPnode(void)
{
	memset((unsigned char*)&gTcpIPsd[0], 0, 10);
	memset((unsigned char*)&gTcpIPsd[1], 0, 10);
	memset((unsigned char*)&gTcpIPsd[2], 0, 10);
	memset((unsigned char*)&gTcpIPsd[3], 0, 10);

	tcpipsdnode = 0;

}
void TcpIPSd_SAVE(unsigned char *dat, unsigned int len)
{
	unsigned char i;

#ifdef _debug_gprs
	debugstring("TcpIpsd save --\r\n");
#endif

	if ((len > 1024) || (len == 0)) {
#ifdef _debug_gprs
		debugstring("TcpIpsd Save Data too long!!\r\n");
#endif
		return;
	}//BDBDBD226868752001020000000000072012070508265618E6
	for (i = 0; i < 4; i++) {//找相同的数据包
		if (gTcpIPsd[i].T == 0xE501) {//
			if (memcmp(gTcpIPsd[i].V + GCMD_CmdCode, dat + GCMD_CmdCode, 2) == 0) {
				// 				debugstring("有相同的命令，壶盖！\r\n");
				// 				debugdata((unsigned char *)gTcpIPsd[i].V, 10, 1);
				// 				debugdata((unsigned char *)dat, 10, 1);
				gTcpIPsd[i].T = 0xE501;
				gTcpIPsd[i].L = len;
				memcpy(gTcpIPsd[i].V, dat, len);
				break;
			}
		}
	}
	if (i >= 4) {//没有找到相同包
		for (i = 0; i < 4; i++) {//找空余地方
			if (gTcpIPsd[i].T != 0xE501) {//空表
				gTcpIPsd[i].T = 0xE501;
				gTcpIPsd[i].L = len;
				memcpy(gTcpIPsd[i].V, dat, len);
				break;
			}
		}
	}
	if (i >= 4) {//没有找到空闲的，用最后
		if (tcpipsdnode > 0)//
			i = tcpipsdnode - 1;
		else
			i = 3;

		gTcpIPsd[i].T = 0xE501;
		gTcpIPsd[i].L = len;
		memcpy(gTcpIPsd[i].V, dat, len);
	}
#ifdef _debug_gprs
	// 	for(i=0;i<4;i++){
	debugstring("TcpIPSd_SAVE:");
	debugdata((unsigned char *)&gTcpIPsd[i], gTcpIPsd[i].L + 4, 1);
	// 	}
#endif
}

//取出链表中要发送数据
unsigned short TcpIPSd_Pick(unsigned char *dat)
{
	unsigned char i;
	unsigned short rLen;

	rLen = 0xFFFF;
	if (tcpipsdnode > 3)
		tcpipsdnode = 0;
	for (i = 0; i < 4; i++) {
		if (gTcpIPsd[tcpipsdnode].T == 0xE501) {
			rLen = gTcpIPsd[tcpipsdnode].L;
			memcpy(dat, gTcpIPsd[tcpipsdnode].V, gTcpIPsd[tcpipsdnode].L);
			memset((unsigned char*)&gTcpIPsd[tcpipsdnode], 0, 10);//清除
			tcpipsdnode++;
#ifdef _debug_gprs
			debugstring("TCPip_pick OK:");
			debugdata((unsigned char*)&rLen, 2, 1);
#endif
			break;
		}
		else {
			tcpipsdnode++;
			if (tcpipsdnode > 3)
				tcpipsdnode = 0;
		}
	}
#ifdef _debug_gprs
	if (rLen != 0xFFFF)
	{
		debugstring("TcpIPSd_Pick:");
		debugdata(dat, rLen, 1);
	}
#endif
	return rLen;
}

//只发送01命令
void gprs_send_data(unsigned char linkNum, unsigned int len, unsigned char *dat)
{
	unsigned char buff[1200];
	unsigned int pos = 0;

	//	unsigned int CRC;
	unsigned short i;

	MSG_LOG("gprs_send_data:%d,%d\n", linkNum, len);
	memset(buff, '$', 4);//包头
	pos = 4;

	i = len + 10;		//只算一个XOR,加上链接号
	memcpy(buff + pos, (unsigned char*)&i, 2);
	over_turn(2, (unsigned char*)buff + pos);//数据长度，数据类型+应用层数据 的长度,高字节在前
	pos += 2;

	buff[pos++] = 0x01;//公交数据，直接发送到后台，此字节不需要发送
	buff[pos++] = gpacSNo;//包序号
	buff[pos++] = linkNum + 1;
	if (len != 0)
	{
		memcpy(buff + pos, dat, len);
		pos += len;
	}

	// 	CRC = crc16_isr((unsigned char*)buff, pos);	
	// 	memcpy(buff+pos, (unsigned char*)&CRC, 2);
	// 	//over_turn(2, buff+pos);
	//	pos += 2;
	i = pos + 1;
	memcpy(buff + 4, (unsigned char*)&i, 2);
	over_turn(2, (unsigned char*)buff + 4);//重新计算长度

	buff[pos++] = Xor8(buff, pos);

#if defined _debug_//DVR
	debugstring("SEND DVR:::");
	debugdata(buff, pos, 1);
#endif
	com_snd(COM_GPRS, pos, buff);
}

void gprs_send_CMD(unsigned char cmd, unsigned int len, unsigned char *dat)
{
	unsigned char buff[1200];
	unsigned int pos = 0;
	unsigned short i;

	memset(buff, '$', 4);//包头
	pos = 4;

	i = len + 9;
	memcpy(buff + pos, (unsigned char*)&i, 2);
	over_turn(2, (unsigned char*)buff + pos);//数据长度，数据类型+应用层数据 的长度,高字节在前
	pos += 2;

	buff[pos++] = cmd;//命令
	buff[pos++] = gpacSNo;//包序号
	if (len != 0)
	{
		memcpy(buff + pos, dat, len);
		pos += len;
	}

	buff[pos++] = Xor8(buff, pos);

#ifdef _debug_
	debugstring("Send DVR-CMD:");
	debugdata(buff, pos, 1);
#endif

	com_snd(COM_GPRS, pos, buff);

}

//发送命 令后等待应答，并把应答数据返回。
int gprs_send_CMD_WaitACK(unsigned char cmd, unsigned int len, unsigned char *dat, unsigned char *odat)
{
	unsigned char i;

	memset(irda_rxBuf, 0, sizeof(irda_rxBuf));//通过中断接收模块返回的值

	gprs_send_CMD(cmd, len, dat);

	set_timer0(500, 2);
	while (1) {
		if (irda_rxBuf[0] == 0xEA) {
			i = irda_rxBuf[1];
			MSG_LOG("收到应答了len=%d:", i);
			memcpy(odat, irda_rxBuf + 2, i);
			BCD_LOG(odat, i, 1);
			return irda_rxBuf[1];
		}
		if (get_timer0(2) == 0)
			return -1;	//超时
	}
}

//在Linux模块中查找黑名单
unsigned int Linux4GFindBLK(unsigned char *sno)
{
	unsigned char sndbuf[16];
	int ret;

	memset(sndbuf, 0, sizeof(sndbuf));
	memcpy(sndbuf, SL8583FileFLAG_BLK, 3);
	sndbuf[3] = 10;	//LINUX内的黑名单的单条为10字节
	memcpy(sndbuf + 4, sno, BLK_SNO_LEN);//都发10字节，如果不够后面补0

	ret = gprs_send_CMD_WaitACK(0x0A, 14, sndbuf, sndbuf);
	MSG_LOG("LinuxBLKF ret=%d\r\n", ret);
	if (ret >= 0) {
		BCD_LOG(sndbuf, ret, 1);
		if (memcmp(sndbuf, "\x90\x00", 2) == 0) {//是黑名单
			return 0;
		}
	}
	return 0xFFFFFFFF;
}

//mode == 1 关闭链接1 mode==2:关闭链接2 mode==0:关闭单链接
void tcpipClose(unsigned char mode)
{
	GPRSLinkProcess = 20;
	set_timer0(1, 1);
}

int DealComData0x2B(unsigned char *inbuf)
{//数据类型1字节【1=刷卡记录、2=当前记录文件、3=历史记录、4=车载机程序（模块发送）】+文件长度(4字节）+偏移量（4字节）+数据长度（4字节）+数据（L)
	unsigned int addr, itmp, pos, flen, offset, dlen, i;
	unsigned char buftemp[64];

	pos = 0;

	if (inbuf[pos] != 4) {//不是程序文件,当前只有程序文件，标识为4
		return -1;
	}
	pos++;
	memcpy((unsigned char *)&flen, inbuf + pos, 4);	//文件长度
	pos += 4;
	memcpy((unsigned char *)&offset, inbuf + pos, 4);	//偏移量
	pos += 4;
	memcpy((unsigned char *)&dlen, inbuf + pos, 4);	//数据长度
	pos += 4;

	if (offset == 0) {//第一个包，需要擦除
		clr_wdt();
		addr = FLASH_PRO_START;//写入的地址 程序起始地址
		for (i = 0; i < 7; i++) {
#ifdef _debug_
			debugstring("清空存贮器X:");
			debugdata((unsigned char*)&addr, 4, 1);
#endif
			flasherase(ERASE64KBYTESECTOR, addr);//先擦除 
			addr += 0x10000;//下一个64K的块
		}
		flasherase(ERASE4KBYTESECTOR, FLASH_PRO_FLAG);//先擦除标志区4K
	}

	sprintf((char*)buftemp, "P:%d%% ", (((offset + dlen) * 100) / flen));
	miniDispstr(14, 0, (char*)buftemp, 0);
	addr = FLASH_PRO_START + offset;

	flashwrite(addr, inbuf + pos, dlen);	//写入数据

	MSG_LOG("%s, flen:%d, offset:%d, dlen:%d\r\n", __FUNCTION__, flen, offset, dlen);

	itmp = offset + dlen;
	if (itmp >= flen)//下载完成
	{
		memset(buftemp, '$', 4);
		pos = 4;
		buftemp[pos++] = 0;
		buftemp[pos++] = 9;
		buftemp[pos++] = 0x2B;
		buftemp[pos++] = 0;

		buftemp[pos++] = 0x90;
		buftemp[pos++] = 0x00;
		i = pos + 1;
		memcpy(buftemp + 4, (unsigned char*)&i, 2);
		over_turn(2, buftemp + 4);	//重新填入长度
		buftemp[pos++] = Xor8(buftemp, pos);

#ifdef _debug_
		printf("%s:", __FUNCTION__);
		debugdata(buftemp, pos, 1);
#endif
		com_snd(COM_GPRS, pos, buftemp);


		delayxs(10);//致少要延时10秒钟才能重启设备， 不然会影响到LINUX模块的文件操作

// 		feread(BIT_PRO_LINUX_CRC32, 4, (unsigned char*)&addr);//读取CRC32值
// 		crc32 = calFileCRC(FLASH_PRO_START, itmp);//计算CRC32值 
// 		MSG_LOG("file len:%d\r\n",itmp);
// 		for(i=0;i<5;i++){
// 			MSG_LOG("cal crc32:%08X,crd32:%08X\r\n",crc32, addr);
// 			if(crc32 != addr)
// 				crc32 = calFileCRC(FLASH_TEMP_START,itmp);
// 			else
// 				break;
// 		}
// 		
// 		if (i>=5)
// 		{
// 			return 0;
// 		}

		flashread(FLASH_PRO_START + 0x1000, buftemp, 64);	//读出标识
		buftemp[63] = 0;
		if (memcmp(MachineCode, buftemp, strlen(MachineCode)) != 0) {//不是这个车载机的程序
			MessageBox(1, "不是本设备程序");
			display(6, 0, (char*)buftemp, DIS_CENTER);
			beep(5, 50, 50);
			while (1) {
				if (getkey(1) == KEY_ENT) {
					return ST_ERROR;
				}
				delayxms(50);
			}
		}



		memset(buftemp, 0, 64);
		memcpy(buftemp, "程序需要更新", 12);
		memcpy(buftemp + 12, gsl8583FileDownPara.Miss_ver, 2);
		memcpy(buftemp + 14, (unsigned char*)&SysTime, 7);
#ifdef _debug_
		debugstring("程序下载完成:");
		debugdata(buftemp, 21, 1);
#endif
		display(3, 0, "完成,请重启...", DIS_CENTER);
		addr = FLASH_PRO_FLAG;

#ifdef _debug_
		debugstring("程序下载完成2:");
		debugdata(buftemp, 34, 1);
#endif
		flashwrite(addr, buftemp, 64);

		beep(1, 10, 10);
		delayxms(400);
		ReStartCPU();//下载完成，重启设备以便更新程序。
		while (1);
	}
	return 0;
}

extern stBusVerIfo gBusVerInfo;
//调度设备传回数据
void GPSDD_Deal_4GLinux(unsigned char mode, unsigned char pasn)
{
	//	unsigned char bu[16];
	unsigned char sendbuf[128];
	unsigned char pos = 0;
	unsigned int itmp;
	unsigned short packLen = 0;

	//MSG_LOG("GPSDD_Deal_4GLinux:\n");
	memcpy((unsigned char*)&packLen, gprsRevBuf + CP_len, 2);
	over_turn(2, (unsigned char*)&packLen);

	memset(sendbuf, '$', 4);
	pos = 4;
	sendbuf[pos++] = 0;
	sendbuf[pos++] = 9;
	sendbuf[pos++] = mode;
	sendbuf[pos++] = pasn;//包序号
	switch (mode) {
	case 0x21://转发服务器文件
		break;
	case 0x06://主动发送
	case 0x26://返回模块请求服务器IP信息给模块
		if (GPRSLinkProcess == 0) {
			GPRSLinkProcess++;
		}
		if (packLen > 0x09)
			memcpy((unsigned char*)&gBusVerInfo.Linux4GVer, gprsRevBuf + CP_dat + 1, 2);	//模块返回的版本号

		gsl8583Style.ISOK = 0;	//收到这个信息要重新签到
		//if (gEC20Style.styleEA == 0) {
		//MSG_LOG("4G模块请求服务器IP端口配置:%04X\n", GET_INT16(gBusVerInfo.Linux4GVer));
		gEC20Style.styleEA = 0x26;
		//}
		break;
	case 0x07:
	case 0x27:	//下载文件回应 3字节文件标识+2字节版本号+9000（文件已经存在)或8001其它,87007  LINUX有下载任务，需要等任务下载完后才能下载。
		if (memcmp(gprsRevBuf + CP_dat, SL8583FileFLAG_BLK, 3) == 0) {//黑名单
			if (memcmp(gprsRevBuf + CP_dat + 5, "\x80\x07", 2) != 0)
				clrgsl8583filelist(SL8583FileFLAG_BLK); //已经收到了，就不再发这命令
		}
		else if (memcmp(gprsRevBuf + CP_dat, SL8583FileFLAG_EC20, 3) == 0) {
			if (memcmp(gprsRevBuf + CP_dat + 5, "\x80\x07", 2) != 0)
				clrgsl8583filelist(SL8583FileFLAG_EC20);
		}
		else if (memcmp(gprsRevBuf + CP_dat, SL8583FileFLAG_PRO, 3) == 0) {
			if (memcmp(gprsRevBuf + CP_dat + 5, "\x80\x07", 2) != 0)
				clrgsl8583filelist(SL8583FileFLAG_PRO);
		}
		if (gmissflag >= 0x80)
			gmissflag = MISS_G_FREE;
		if (GPRSLinkProcess == GPRS_SENDING_CMD)
			GPRSLinkProcess = TCPSTARTSTAT;
		break;
	case 0x09:
	case 0x29:	//模块发状态信息，保存后需要处理 处理函数：void dealgEC20Style(void)
		if (gEC20Style.styleEA == 0) {
			gEC20Style.styleEA = 0x29;
			gEC20Style.dflag = gprsRevBuf[CP_dat + 1];//信息标识，前面有个链接号，没用，所以加1
			gEC20Style.datalen = packLen - CP_dat - 2;
			memcpy(gEC20Style.data, gprsRevBuf + CP_dat + 1 + 1, gEC20Style.datalen);
			sendbuf[pos++] = gEC20Style.dflag;
			sendbuf[pos++] = 0x90;
			sendbuf[pos++] = 0x00;
		}
		break;
	case 0x0A:
	case 0x2A:	//查询黑名单，有函数在等，通过irda_buff返回
	case 0x0B:	//发送数据等应答
		MSG_LOG("DEAL 0x0A CMD\r\n");
		irda_rxBuf[0] = 0xEA; //有返回标识
		irda_rxBuf[1] = packLen - CP_dat - 1;;//返回数据长度
		memcpy(irda_rxBuf + 2, gprsRevBuf + CP_dat, irda_rxBuf[1]);	//返回状态码
		break;
	case 0x2B:	//平台发送文件到主机
		MSG_LOG("DEAL 0x2B CMD\r\n");
		gEC20Style.styleEA = 0x29;
		gEC20Style.dflag = 0x2B;//gprsRevBuf[CP_dat+1];//信息标识，前面有个链接号，没用，所以加1
		gEC20Style.datalen = packLen - CP_dat - 1;
		memcpy(gEC20Style.data, gprsRevBuf + CP_dat + 1, gEC20Style.datalen);
		return;	//数据回应在处理数据后发送
	default:
		break;
	}

	if (mode > 0x21 && mode != 0x26) {//模块主动发的命令需要应答
		itmp = pos + 1;//先加入长度
		memcpy(sendbuf + 4, (unsigned char*)&itmp, 2);
		over_turn(2, sendbuf + 4);	//重新填入长度
		sendbuf[pos++] = Xor8(sendbuf, pos);

		// #ifdef _debug_
		// 		debugstring("GPSDD_Deal Send:");
		// 		debugdata(sendbuf,pos,1);
		// #endif
		com_snd(COM_GPRS, pos, sendbuf);
	}
}

void OTHER_Linux4G_hand(unsigned char value)
{
	unsigned short plen, st;
	unsigned char ii;

	ii = value;
	//	debugdata((unsigned char*)&ii, 1, 0);
#ifdef _debug_gprs
	com_snd(COM1, 1, (unsigned char*)&ii);
#endif

	switch (gprsIndex)
	{
	case 0:
	case 1:
	case 2:
	case 3:
		if (ii != '$') {
			//			debugstring("^");
			gprsIndex = 0;
		}
		else {
			gprsRevBuf[gprsIndex++] = ii;
		}
		break;
	case 4://长度高位
		gprsRevBuf[gprsIndex++] = ii;
		break;
	case 5://长底低位

		gprsRevBuf[gprsIndex++] = ii;
		memcpy((unsigned char*)&plen, gprsRevBuf + 4, 2);
		over_turn(2, (unsigned char*)&plen);
		if (plen > 2048) {//长度错误
			MSG_LOG("长度错误--:%04X\r\n", plen);
			gprsIndex = 0;
			memset(gprsRevBuf, 0, 10);
			break;
		}
		break;
	default:
		gprsRevBuf[gprsIndex++] = ii;
		memcpy((unsigned char*)&plen, gprsRevBuf + 4, 2);
		over_turn(2, (unsigned char*)&plen);
		if (plen > 2048) {//长度错误
#ifdef _debug_
			debugstring("长度错误*************\r\n");
#endif
			gprsIndex = 0;
			memset(gprsRevBuf, 0, 10);
			break;
		}
		if (gprsIndex >= plen) {//数据接收完成
			clr_wdt();
#ifdef _debug_
			debugstring("收DVR_CMD:0x");
			debugdata(gprsRevBuf + 6, 1, 1);
			debugdata(gprsRevBuf, gprsIndex, 1);
#endif

			//gSendOverTime = 0;
			//#ifndef _debug_
			if (Xor8(gprsRevBuf, gprsIndex - 1) != gprsRevBuf[gprsIndex - 1])
			{
				debugstring("XOR error*************\r\n");
				delayxms(1000);
				gprsIndex = 0;
				memset(gprsRevBuf, 0, 10);
				break;
			}
			//#endif
#if 0
#ifdef _debug_DVR
			miniDispstr(6, 2, "R", 0);
			beep(1, 50, 50);
			delayxms(100);
			miniDispstr(6, 2, " ", 0);
#endif
#endif

			//MSG_LOG("DVR CMD:%02X\r\n", gprsRevBuf[6]);

			clr_wdt();
			if (gprsRevBuf[CP_cmd] == 0x21) {//是后台数据
				gprsErrorTimes = 0;
				for (ii = 0; ii < MAX_RCV_PACKET_NUM; ii++) {
					if (gPacketRcvBuf[ii].ucItemStatus != 0xea)
						break;
				}
				if (ii >= MAX_RCV_PACKET_NUM) {
					gprsIndex = 0;
					memset(gprsRevBuf, 0, 20);	//缓冲区满，此包丢弃
//					debugstring("满了\r\n");
					return;
				}
				st = plen - 10;	//数据度长，
				memcpy(gPacketRcvBuf[ii].LinkFrame.FrameBuf, gprsRevBuf + CP_dat + 1, st);//前面有一个字节的链接号
				gPacketRcvBuf[ii].LinkFrame.ucFrameType = gprsRevBuf[CP_dat];	//这里用于存链接号
				memcpy(gPacketRcvBuf[ii].LinkFrame.uiFrameLen, (unsigned char*)&st, 2);
				gPacketRcvBuf[ii].ucItemStatus = 0xea;
			}

			GPSDD_Deal_4GLinux(gprsRevBuf[CP_cmd], gprsRevBuf[CP_sta]);
			if (gprsRevBuf[CP_cmd] < 0x20) {	//模块主动包，需要回应
				gpacSNo++;//如果发包有回应，就加，下面将是新包
			}
#ifdef _debug_DVR
			// 			delayxms(200);
			//			miniDispstr(6,2," ",0);
#endif

			gprsIndex = 0;
			memset(gprsRevBuf, 0, 10);
		}
		break;
	}
}

void GPRS_handler(unsigned char value)
{
#ifdef _debug_gprs
	// 	if(CDMA_REV_LEN>0)
	// 	{
	// 		//debugstring("CDMA_REV_LEN:");
	// 		debugdata((unsigned char *)&value, 1, 0);
	// 	}
	// 	else if(guiMC323_REV_LEN>0){
	// 		//debugstring("guiMC323_REV_LEN:");
	// 		debugdata((unsigned char *)&value, 1, 0);
	// 	}
	// 	else
	// 	com_snd(COM1, 1, (unsigned char*)&value);
		//debugdataCOM(COM4,(unsigned char*)&value,1,0);
	//	debugdata(&value,1,0);
#endif

	OTHER_Linux4G_hand(value);
}
extern unsigned char getGprsCom(void);
//初始化GPRS端口及模块
void GPRS_INIT(unsigned int band)
{
	unsigned char gprcom;
	gprcom = getGprsCom();

	band = GPRSCOM_BAUD;

#ifdef _debug_gprs
	debugstring("INI gprcom:");
	debugdata((unsigned char*)&gprcom, 1, 0);
	debugstring(":");
	debugdata((unsigned char*)&band, 4, 0);
	debugstring("|\r\n");
#endif

	com_init(gprcom, band);
	COM_it_enable(gprcom);
}

//V231 初始化放到GPRS库里面。
void GPRSConfig(void)
{
	if (in_getGprsModeStyle() != Gmode_Close) {
		if ((in_getGprsModeStyle() == Gmode_MODEM) || (in_getGprsModeStyle() == Gmode_CDMA2000)//){
			|| (in_getGprsModeStyle() == Gmode_ALL4G) || (in_getGprsModeStyle() == Gmode_YYLinux)) {
			GPRS_INIT(GPRSCOM_BAUD);
		}
		else {
			GPRS_INIT(9600);//此端口用于GPRS连接口
		}
	}
}

void gprs_par_init(void)
{
	unsigned char i;

	GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;//0;

	gprsErrorTimes = 0;
	isNetOK[0] = 0;
	isNetOK[1] = 0;
	gGPRS_data_style.ucSendstyle = 0;
	gGPRS_data_style.ucReadStyle = 0;
	gGPRS_data_style.uiNum = 0;
	guiMC323_REV_LEN = 0;

	initTCPIPnode();
	for (i = 0; i < MAX_RCV_PACKET_NUM; i++)
		gPacketRcvBuf[i].ucItemStatus = 0;
	memset(GprsCardList, 0, 16);
	gpacSNo = 0;//包序号
}





//获得一个空闲的帧缓冲,返回空闲帧索引jgwei updata
unsigned char GetFrameBuf(void)
{
	unsigned char i;

	for (i = 0; i < MAX_RCV_PACKET_NUM; i++)
	{
		if (!(gPacketRcvBuf[i].ucItemStatus & MASK_RCV_BUF_USED))
		{
			break;
		}
	}
	if (i == MAX_RCV_PACKET_NUM)
	{								//没有可以获得的缓冲
		i = 0xff;
	}
	else
	{
		memset(gPacketRcvBuf[i].LinkFrame.FrameBuf, 0x00, sizeof(gPacketRcvBuf[i].LinkFrame.FrameBuf));	//清零将要使用的帧缓冲
	}
	return i;
}

//从接收缓冲中获得一个有效的数据帧 OK
unsigned char PeekFrame(void)
{
	unsigned char i;

	for (i = 0; i < MAX_RCV_PACKET_NUM; i++)
	{
		if (gPacketRcvBuf[i].ucItemStatus == 0xea)
		{				//接收帧不空
			break;
		}
	}
	if (i == MAX_RCV_PACKET_NUM)
	{
		return 0xff;
	}
	else
	{
		return i;	 //返回有效的接收帧索引
	}
}

void mainGprs(void)
{
	unsigned char i, ret;
	unsigned short plen = 0;//,uslen=0;
#ifdef	_debug_gprs 
	
#endif
	int packLen = 0;

	gprsReady = 0;//处理完后,置0
//	if((isNetOK[0] != 0)||(isNetOK[1] != 0)){
	i = PeekFrame();
	if (i != 0xff)
	{
#ifdef _debug_gprs
		// 			debugstring("RcvFrame: ");
		// 			memcpy(&plen,gPacketRcvBuf[i].LinkFrame.uiFrameLen,2);
		// 			debugdata((unsigned char *)&gPacketRcvBuf[i].LinkFrame.FrameBuf,plen,1);
#endif
			memcpy(&plen, gPacketRcvBuf[i].LinkFrame.uiFrameLen, 2);
//应用层数据帧
// #ifdef	_debug_gprs 
// 		debugstring("TaskGprsRev-------:\r\n");
// 		debugdata((unsigned char *)&i, 1, 1);
// 		memcpy(&plen, gPacketRcvBuf[i].LinkFrame.uiFrameLen, 2);
// 		debugdata((unsigned char *)&gPacketRcvBuf[i].LinkFrame.FrameBuf, plen, 1);
// #endif
		ret = gPacketRcvBuf[i].LinkFrame.ucFrameType;	//
		MSG_LOG("link:%d\r\n", ret);

		switch (ret - 1)
		{
		case LINK_PBOC:
			gGPRS_Cmd_outtime = 0;
			//printf("HTTP通讯连接 %d\r\n",link);
			ret = QPBOC_DataDeal((unsigned char*)&gPacketRcvBuf[i].LinkFrame, plen);//银联数据处理

			break;
//		case ZFB_LINK - 1:
		case LINK_GJ:
			gGPRS_Cmd_outtime = 0;
#ifdef _debug_
			printf("mainGprs公交链接 %d\r\n", ret);
#endif
			GPRSLinkProcess = TCPSTARTSTAT;
			gmissflag = MISS_G_FREE;
			//gsl8583Style.ISOK= 'O';
			ret = GJDataDeal((unsigned char*)&gPacketRcvBuf[i].LinkFrame);//公交数据处理

			//	XFsoud(100);
			break;
		default:
			printf("mainGprs连接错误 %d\r\n", ret);
			break;
		}


		gPacketRcvBuf[i].ucItemStatus = 0;
		gGPRS_data_style.ucSendstyle = 0;
		//			gGPRS_data_style.ucReadStyle = GPRS_SISR;
		gTcpIpbusy = 0;

	}
	//	}
}

//暂停当前的GPRS
void stopCCgprs(unsigned short mode)
{
	in_gErrortimes(20, mode);
	gErrorFlag |= mode;
	gmissflag = 0;
}


extern void getDefaultAPN(char *rAPN, unsigned char mode);//mode=0 公交，1 移动。。;
extern void US1_It(void);
extern void TaskRecWrite(void);
//黑名单应答
extern unsigned int Buid_GJ_DBLK(unsigned char *dat);
#ifdef _debug_
extern void SetTcpPacketTTLTime(unsigned char val);
extern unsigned char WriteRecord(unsigned char *buffer, unsigned char mode);

#endif

extern unsigned char s_isAuthOk;
//1=网络状态 2=文件下载状态 3=文件参数
void dealgEC20StyleIp(void)
{
	unsigned char disbuf[64];
	unsigned int i, t, pos;

	if (gEC20Style.styleEA != 0) {//有未处理的信息
		MSG_LOG("deal EC20设备状态:");
		BCD_LOG(gEC20Style.data, gEC20Style.datalen, 1);

		if (gEC20Style.styleEA == 0x29) {
			clr_wdt();
			if (gEC20Style.dflag == 2) {//2文件下载状态：文件名（20字节，以0结束）+总长度（4字节，低位在前）+当前下载长度（4字节，低位在前）。
				memset(disbuf, 0, sizeof(disbuf));
				memcpy(disbuf, gEC20Style.data, 20);//文件名
				memcpy((unsigned char*)&i, gEC20Style.data + 20, 4);
				memcpy((unsigned char*)&t, gEC20Style.data + 24, 4);
				if (t == 0xFFFFFFFF) {//下载失败
					sprintf((char*)disbuf, "%s下载失败", gEC20Style.data);
					MessageBox(1, (char*)disbuf);
					beep(2, 100, 100);
					delayxs(1);
					restore_flag = 3;
				}
				else {
					sprintf((char*)disbuf + strlen((char*)disbuf), "%d|%d %d%%", t, i, (t * 100) / i);
					MSG_LOG((char*)disbuf);
					miniDispstr(13, 0, (char*)disbuf, 0);
				}
			}
			else if (gEC20Style.dflag == 1) {//1网络状态：//网络是否成功标识+20位SIM卡号+IP1连接状态+IP2连接状态+IP3连接状态+IP4连接状态
				if (gEC20Style.data[0] == 1) {//网络连接成功
					if ((GPRSLinkProcess == 0x2F) || (GPRSLinkProcess >= 0xF0) || (GPRSLinkProcess < 9))
						GPRSLinkProcess = 9;
				}
				else {
					GPRSLinkProcess = 8;
				}
				memcpy(GprsCardList, gEC20Style.data + 1, 16);

				BCD_LOG(gEC20Style.data + 21, 4, 1);
				i = 0;
				for (i = 0; i < 2; ++i)
				{

					if (gEC20Style.data[21 + i] == 1) {//连接服务器成功了
						if ((GPRSLinkProcess == 0x2F) || (GPRSLinkProcess >= 0xF0) || (GPRSLinkProcess < TCPSTARTSTAT))
							GPRSLinkProcess = TCPSTARTSTAT;
						isNetOK[i] = 3;
					}
					else {
						//MSG_LOG("deal EC20设备状态:");
						//BCD_LOG(gEC20Style.data + 21, 4, 1);
						isNetOK[i] = 0;
						if (i == LINK_PBOC) {
							s_isAuthOk = 1;
						}
						//gsl8583Style.ISOK = 0;
					}
				}
			MSG_LOG("gEC20Style.data:%d,%d,%d,%d\r\n", gEC20Style.data[21],gEC20Style.data[22], gEC20Style.data[23], gEC20Style.data[24]);
		MSG_LOG("isNetOK__before:%d,%d,%d,%d\r\n", isNetOK[0], isNetOK[1], isNetOK[2], isNetOK[3]);
			}
			else if (gEC20Style.dflag == 3) {//3文件参数：1字节文件数量+3字节文件标识_4字节版本号+4字节长度....
				pos = 1;
				for (i = 0; i < gEC20Style.data[0]; i++)
				{
					if (memcmp(gEC20Style.data + pos, SL8583FileFLAG_BLK, 3) == 0) {//是黑名单文件.
	// 					MSG_LOG("收到黑名单版本信息!!");
	// 					Ascii2BCD(gEC20Style.data+pos+4, gBusVerInfo.busBLKVer, 4);//后4字节是版本号，保存版本号
	// 					over_turn(2, gBusVerInfo.busBLKVer);
	// 					BCD_LOG(gBusVerInfo.busBLKVer, 2, 1);
	// 					memcpy((unsigned char*)&pFistVary.BlackListNum, gEC20Style.data+pos+8, 4);//文件大小
	// 					MSG_LOG("长度:%d\r\n", pFistVary.BlackListNum);
	// 
	// 					if(pFistVary.BlackListNum < 10){//文件不正确
	// 						memset(gBusVerInfo.busBLKVer, 0, 2);	//重新下载
	// 					}
	// 					else{
	// 						pFistVary.BlackListNum /= 10;	//平台的文件是10字节一条
	// 						pFistVary.BlackListNum -= 1;	//前10字节不是黑名单
	// 						sysfewrite(BIT_BLACK_SUM,4,(unsigned char*)&pFistVary.BlackListNum); //黑名单条数		
	// 					}
	// 					t = FLASH_BLK_START;
	// 					flasherase(ERASE4KBYTESECTOR, t);
	// 					flashwrite(t, gBusVerInfo.busBLKVer, 2);//更新黑名单版本
	// 					
	// 					memset((unsigned char*)&gsl8583FileDownPara, 0, sizeof(st8583filedown));
	// 					sysfewrite(BIT_8583filedown, sizeof(st8583filedown), (unsigned char*)&gsl8583FileDownPara);//保存任务参数
	// 					gsl8583Style.ISOK = 0;
	// 					restore_flag = 3;
					}

					pos += 12;
				}
				if (gmissflag > MISS_HTTP) {
					gmissflag = MISS_G_FREE;
					if (GPRSLinkProcess == GPRS_SENDING_CMD)
						GPRSLinkProcess = TCPSTARTSTAT;
				}
			}
			else if (gEC20Style.dflag == 0x2B) {//2B-下载了程序文件
				memset(disbuf, '$', 4);
				pos = 4;
				disbuf[pos++] = 0;
				disbuf[pos++] = 9;
				disbuf[pos++] = 0x2B;
				disbuf[pos++] = 0;

				if (DealComData0x2B(gEC20Style.data) >= 0) {
					disbuf[pos++] = 0x90;
					disbuf[pos++] = 0x00;
				}
				else {
					disbuf[pos++] = 0x80;
					disbuf[pos++] = 0x01;
				}
				t = pos + 1;
				memcpy(disbuf + 4, (unsigned char*)&t, 2);
				over_turn(2, disbuf + 4);	//重新填入长度
				disbuf[pos++] = Xor8(disbuf, pos);

#ifdef _debug_
				debugstring("GPSDD_Deal Send:");
				debugdata(disbuf, pos, 1);
#endif
				com_snd(COM_GPRS, pos, disbuf);
			}
		}
		else if (gEC20Style.styleEA == 0x26) {
			buildSeverInfo_06(0x26, 5);
			Sign_Infor.ISOK = 0;
		}

		memset((unsigned char*)&gEC20Style, 0, sizeof(gEC20Style));
	}
}


int open_sock(unsigned int addr, unsigned short port, unsigned int outtime, unsigned char link)
{
	unsigned char gprsMode = 0;
	char gpsrsP[200];
	unsigned int delay = 0;

	// 	unsigned int outdly=0;
	//	unsigned int i=0;
	//unsigned long long read_dat=0;
	//unsigned int count=0;
#ifdef _debug_gprs00
	unsigned char disbuff[20];
#endif

	MSG_LOG("do %s\r\n", __FUNCTION__);

	gprsMode = in_getGprsModeStyle();

	//	display(6, 0, "打开连接...", DIS_ClsLine|DIS_CENTER);

	memset(gpsrsP, 0, sizeof(gpsrsP));
	memcpy((unsigned char*)gpsrsP + 50, (unsigned char*)&addr, 4);
	//	printf("打开连接...\r\n");

	gGPRS_data_style.openLink = link;

	if (gprsMode == Gmode_CDMA2000) {
		sprintf((char*)gpsrsP, "AT^IPOPEN=%u,1200,\"%u.%u.%u.%u\",%u,0\r\n", link + 1, gpsrsP[50], gpsrsP[51], gpsrsP[52], gpsrsP[53], port);//1,2,3,4 有效的 socket ID
	}
	else if (gprsMode == Gmode_MC323) {
		sprintf((char*)gpsrsP, "AT^IPOPEN=1,\"TCP\",\"%u.%u.%u.%u\",%u,1000\r\n", gpsrsP[50], gpsrsP[51], gpsrsP[52], gpsrsP[53], port);
	}
	else if (gprsMode == Gmode_MG323) {
		sprintf((char*)gpsrsP, "at^siss=0,address,\"socktcp://%u.%u.%u.%u:%u\"\r\n", gpsrsP[50], gpsrsP[51], gpsrsP[52], gpsrsP[53], port);
	}
	else if (gprsMode == Gmode_ALL4G) {	//Push模式：AT+QIOPEN=1,0,"TCP","61.190.19.174",8705,0,1   收到数据后命令+QIURC: “recv”,0,4\r\n 后返回数据。
		if (link == LINK_PBOC)
		{
			//buildSeverInfo_06(0x26, LINK_PBOC);
			sprintf((char*)gpsrsP, "AT+QSSLOPEN=1,1,%d,\"%u.%u.%u.%u\",%u\r\n", link, gpsrsP[50], gpsrsP[51], gpsrsP[52], gpsrsP[53], port);
		}
		else
		{		//AT+QSSLOPEN=1,1,1,"202.101.25.188",20141        最后用0，0 就是收到数据必须读，才能出来
			sprintf((char*)gpsrsP, "AT+QIOPEN=1,%u,\"TCP\",\"%u.%u.%u.%u\",%u,0,0\r\n", link, gpsrsP[50], gpsrsP[51], gpsrsP[52], gpsrsP[53], port);
		}
	}
	else if (gprsMode == Gmode_YYLinux)
	{
		gpsrsP[0] = '\0';
#if 0
		if (link == LINK_POBC)
		{
			gpsrsP[0] = '\0';
			//return 1;
			//sprintf((char*)gpsrsP, "AT+QSSLOPEN=1,1,%d,\"%u.%u.%u.%u\",%u\r\n", link, gpsrsP[50], gpsrsP[51], gpsrsP[52], gpsrsP[53], port);
		}
		else
		{		//AT+QSSLOPEN=1,1,1,"202.101.25.188",20141        最后用0，0 就是收到数据必须读，才能出来
			sprintf((char*)gpsrsP, "AT+QIOPEN=1,%u,\"TCP\",\"%u.%u.%u.%u\",%u,0,0\r\n", link, gpsrsP[50], gpsrsP[51], gpsrsP[52], gpsrsP[53], port);
		}
#endif
	}
	else {
		sprintf((char*)gpsrsP, "AT%%IPOPEN=\"TCP\",\"%u.%u.%u.%u\",%u\r\n", gpsrsP[50], gpsrsP[51], gpsrsP[52], gpsrsP[53], port);
	}

	MSG_LOG("gpsrsP:%s", gpsrsP);



	// 	gprs_snd(strlen(gpsrsP), gpsrsP);

	//LINUX 4G 模块的要这样发送，打开打开连接后要判断一个状态， 短链接，发送数据时候再打开，
	if (gpsrsP[0] != '\0')
	{
		gprs_send_data(link, strlen(gpsrsP), gpsrsP);

	}
	else if (link == LINK_PBOC) {	// 银联才需要这样做
		//buildSeverInfo_06(0x06, LINK_PBOC);
	}
	while (1)
	{
		delayxms(1);
		if (getkey(1) != 0)
		{
			return -1;
		}
#ifdef _debug_gprs00
		memset(disbuff, 0, sizeof(disbuff));
		sprintf((char *)disbuff, "T2:%04d", outtime - delay);
		miniDispstr(7, 30, (const char *)disbuff, 0);
#endif

		if (isNetOK[link] == 3)
		{
#ifdef _debug_gprs
			MSG_LOG("open time:%u\r\n", delay);
#endif

			break;
		}
		if (delay++ > outtime)
		{

			MSG_LOG("=^=");
			//gprsErrorTimes+=25;		//直接这样试试
			MSG_LOG("19==\r\n");
			GPRSLinkProcess = 19;	//
			return -1;
		}

	}

	return 1;

}


void main_GpsRs(void)
{
	char gpsrsP[100];
	int ret;
	unsigned int t;

	t = get_timer0(1);

	if (t != 0) {
		goto maingprs1;
	}

	//MSG_LOG("G");

	if (gprsErrorTimes > 20) {//错误次数太多，重启模块
		gprsErrorTimes = 0;
#ifdef _debug_gprs
		debugstring("err too much\r\n");
#endif
		if ((GPRSLinkProcess == 20) || (GPRSLinkProcess == 21)) {
			if ((gmissflag & MISS_GJ) || (gmissflag == 0)) {//如果正在登陆公交,登不上去,先暂停,做个标志
				stopCCgprs(ERROR_BUS_CONNECT);
			}
			else if ((gmissflag & MISS_IF1) || (gmissflag & MISS_IF2)) {
				stopCCgprs(ERROR_CONNECT_MOBILE);
			}
			// 			if(((gErrorFlag&ERROR_BUS_CONNECT)==0)||((gErrorFlag&ERROR_CONNECT_MOBILE)==0)){
			// 				return; //如果不是同时有问题,则退出重新找任务.
			// 			}
		}

#ifdef _debug_gprs
		debugstring("--4 close\r\n");
#endif
		GPRSLinkProcess = GPRS_MODE_NEED_CLOSE;//
		return;
	}


	switch (GPRSLinkProcess)
	{
	case GPRS_MODE_POWER_DOWN:
#ifdef _debug_gprs
		debugstring("GPRS_MODE_POWER_DOWN!\r\n");
#endif
		gprsErrorTimes = 0;
		isNetOK[0] = 0;

		gmissOldflag = 0;
		gmissflag = 0;

		powerOff4V0();
		set_timer0(5000, 1);
		GPRSLinkProcess = GPRS_MODE_POWER_ON;
		break;
	case GPRS_MODE_POWER_ON:
#ifdef _debug_gprs
		debugstring("GPRS_MODE_POWER_ON!\r\n");
#endif

		gprsErrorTimes = 0;
		isNetOK[0] = 0;

		gmissOldflag = 0;
		gmissflag = 0;
		GPRSLinkProcess = GPRS_LINK_CMD;

		GPRS_open();//上电时，先拉低，再拉高。,一直是高的模块开不了机
		set_timer0(4000, 1);
		break;
	case GPRS_MODE_NEED_CLOSE://模块需要重启，发关闭模块命令.旧的板子，这里关闭电源
#ifdef _debug_gprs
		debugstring("case==GPRS_MODE_NEED_CLOSE\r\n");
#endif
		GPRSLinkProcess = GPRS_MC55_CLOSE;
		GPRS_open();////拉低POWN  //EC20 开机低电平>=100ms，关机低电平>=600ms;  关机命令AT+QPOWD
		gprsErrorTimes = 0;
		isNetOK[0] = 0;

		gmissOldflag = 0;
		gmissflag = 0;
		break;
	case GPRS_MC55_CLOSE:
#ifdef _debug_gprs
		debugstring("case==GPRS_MC55_CLOSE");
#endif

		GPRS_MC55i_open();     //拉高POWN
		GPRSLinkProcess = GPRS_LINK_CMD;
		gprsErrorTimes = 0;
		set_timer0(300, 1);	//等待POWERED DOWN提示才算关机响应正常，但不是实际关机
		isNetOK[0] = 0;
		break;
	case GPRS_LINK_CMD://关闭中，打开模块电源,低电平 800ms
#ifdef _debug_gprs
		debugstring("case==GPRS_LINK_CMD\r\n");
#endif
		GPRSLinkProcess = GPRS_LINK_UP;
		gprsIndex = 0;
		gprsErrorTimes = 0;
		memset(gprsRevBuf, 0, 50);
		GPRS_open();			//拉低POWN
		set_timer0(300, 1);	//长时间拉低，直到收到模块“RDY”提示后退出,目的是复位时候需要，如果正常开机只需>100ms即有效
		break;
	case GPRS_LINK_UP:
#ifdef _debug_gprs
		debugstring("case==GPRS_LINK_UP\r\n");
#endif

		isNetOK[0] = 3;
		GPRSLinkProcess = GPRS_AT_WAIT;
		GPRS_MC55i_open();//再变成高电平，启动模块.旧的板子，这里打开电源
		set_timer0(400, 1);
		break;
	case GPRS_NEED_CLOSEIP:
#ifdef _debug_gprs
		debugstring("case==GPRS_NEED_CLOSEIP\r\n");
#endif
		//buildSeverInfo_06(0x06, 4);
		//isNetOK[0] = 0;
		GPRSLinkProcess = 0;
		gprsErrorTimes = 0;
		break;
	case GPRS_AT_WAIT:
#ifdef _debug_gprs
		debugstring("case==GPRS_AT_WAIT\r\n");
#endif
		GPRSLinkProcess = 0;

		isNetOK[0] = 3;
		break;
	case GPRS_TCPCLOSE_CMD:
#ifdef _debug_gprs
		debugstring("case==GPRS_TCPCLOSE_CMD\r\n");
#endif

		GPRSLinkProcess = 20;
		break;
	case GPRS_Out_CMD:
		GPRSLinkProcess = 9;
		set_timer0(1000, 1);
		break;
	case 0:
		//dealgEC20StyleIp();	//处理EC20的状态信息
		break;	//等4G模块的IP查询命令，表示模块启动
	case 2:
		break;//等4G模块报告SIM卡状态。
	case 8:
		break;//等4G模块报告网络状态。
	case 1:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
	case 15:// 
	case 16:
	case 17:
	case 18:
		GPRSLinkProcess++;
		break;
	case 19:
		GPRSLinkProcess++;
		break;
	case 20:
		gTcpIpbusy = TcpIPFree;
		initTCPIPnode(); //重新边接后，清除发送缓存中的数据
		GPRSLinkProcess++;
		break;
	case 21:	//等服务器连接成功
		if (gGPRS_data_style.openLink == LINK_PBOC) {
			GPRSLinkProcess++;
		}
		break;
	case 22:
		break;
	default:
		break;
	}
maingprs1:
	t = get_timer0(0);
	if (t == 0) {
		set_timer0(1000, 0);//10ms钟进一次

		TaskRecWrite();//处理要发送的数据
		in_decgM_Senddll();
		in_dispGprsProcess();

		//MSG_LOG("P");
		
		reduce_delay_send();
		if (gSendOverTime > 0) {
			gSendOverTime--;
		}

		if (tcpipPacket.TTL > 0)
			tcpipPacket.TTL--;
		if (tcpipPacket.TTL > 60)
			tcpipPacket.TTL = 0;
		if (gErrortimes[1] > 0) {
			gErrortimes[1]--;
			if ((gErrortimes[1] == 0) && (gErrorFlag&ERROR_BUS_CONNECT)) {//如果是连公交出错,重启
#ifdef _debug_gprs
				debugstring("~!!!~~\r\n");
#endif
				GPRSLinkProcess = GPRS_NEED_CLOSEIP;//GPRS_MODE_NEED_CLOSE;ttttttttttttttttt;
			}
		}
		if (gErrortimes[0] > 0) {
			gErrortimes[0]--;
		}
		if (gBusDownPro.Miss_Time > 0)
			gBusDownPro.Miss_Time--;


		gSendGLogin++;

		if (gBusblkDownPro.Miss_time > 0)
			gBusblkDownPro.Miss_time--;

		if ((gBusblkDownPro.Miss_time == 0))
		{
			gBusblkDownPro.Miss_time = GPRS_Login_dly;
			if ((memcmp(gBusVerInfo.newbusBLKVer, "\x00\x00", 2) != 0))
			{//GPRS已经登陆，可以发数据
				if (memcmp(gBusblkDownPro.Miss_ver, "\x00\x00", 2) != 0)
				{
					ret = Buid_GJ_DBLK((unsigned char *)gpsrsP);
#ifdef _debug_gprs
					debugstring("send DBLK agin\r\n");
#endif
					gprs_send_data(1, ret, (unsigned char *)gpsrsP);//发送应答
				}
			}
		}
		if (gTcpIpbusydly > 0) {

			gTcpIpbusydly--;
			if (gTcpIpbusydly == 0)
			{
#ifdef _debug_gprs
				debugstring("超时--free--\r\n");
#endif
				if (gTcpIpbusy == TcpIPBusy) {//应该有数据发送
					gGPRS_data_style.ucSendstyle = GPRS_SISW;		//去发送数据吧
				}
				else {
					gTcpIpbusy = TcpIPFree;
					gGPRS_data_style.ucSendstyle = 0;
					gGPRS_data_style.ucReadStyle = 0;

					GPRSLinkProcess = 19;
				}

			}
		}

		in_sendGPs();//不为0，已经发送一条GPS信息


		//MSG_LOG("Q");

	}

// #ifndef PBOC_TEST
// 	if ((gmissflag == 0) || (gmissflag == MISS_G_FREE) || (gmissflag == MISS_G_LOGINGJ)) {//加快任务查找，不再每一秒钟查找一次。
// 
// 		find_G_new_mission();
// 	}
// #endif

	dealgEC20StyleIp();	//处理EC20的状态信息
	//MSG_LOG("R");
	mainGprs();//处理缓冲中的GPRS数据
	//MSG_LOG("S");
}

//如果没有GPRS，调用这个函数据，主要是用到1秒钟计数功能
void main_Onsec(void)
{
	unsigned int t;

	t = get_timer0(0);
	if (t == 0) {
		set_timer0(1000, 0);//1S钟进一		
		in_dispGprsProcess();
		in_decgM_Senddll();
		in_sendGPs();//不为0，已经发送一条GPS信息

	}
}



int getgprslibVer(void)		//获取库版本号
{
	return SOFT_VAL;
}


void setSendTimeOut(int link, int time) {
	clr_wdt();
	gGPRS_data_style.sendLink = link;
	gSendOverTime = time;
}

